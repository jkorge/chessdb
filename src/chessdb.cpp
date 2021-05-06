#include "include/chessdb.hpp"

/**************************************************
                      ENCODER
**************************************************/

eply Encoder::encode_ply(const ply p, Board& board){
    if(p == this->missing){ return this->emissing; }

    BYTE _piece = this->encode_piece(p, board),
         _action = this->encode_action(p);

    return (static_cast<uint16_t>(_piece) << 8) | _action;
}

std::vector<eply> Encoder::encode_game(const game& g){
    std::vector<eply> res;

    if(!g.tags.at(fenstr).empty()){ fen::parse(g.tags.at(fenstr), this->board); }
    else                          { this->board.reset(); }

    for(std::vector<ply>::const_iterator it=g.plies.begin(); it!=g.plies.end(); ++it){
        res.emplace_back(this->encode_ply(*it, this->board));
        this->board.update(*it);
    }
    return res;
}

BYTE Encoder::encode_piece(const ply p, Board& board){
    U64 bb = board.board(p.type, p.c);
    int cnt = 0, src = bitscan(p.src);
    while(bb){
        if(lsbpop(bb) == src){ break; }
        ++cnt;
    }
    BYTE _piece = static_cast<BYTE>(p.type + 6*(p.c<0)) | (cnt << 4);
    if(p.type == queen){ _piece |= this->encode_queen_axis(coords(p.src), coords(p.dst)); }
    return _piece;
}

BYTE Encoder::encode_action(const ply p){
    coords src = {p.src},
           dst = {p.dst};

    BYTE _action = this->encode_capture(p.capture) | this->encode_check(p.check) | this->encode_mate(p.mate);
    switch(p.type){
        case pawn:   _action |= this->pawn_action(src, dst, p.capture, p.promo); break;
        case knight: _action |= this->knight_action(src, dst);                   break;
        case bishop: _action |= this->bishop_action(src, dst);                   break;
        case rook:   _action |= this->rook_action(src, dst);                     break;
        case queen:  _action |= this->queen_action(src, dst);                    break;
        default:     _action |= this->king_action(src, dst);
    }
    return _action;
}

BYTE Encoder::encode_capture(bool cap){ return cap ? CAPTURE_ : 0; }

BYTE Encoder::encode_check(bool chk){ return chk ? CHECK_ : 0; }

BYTE Encoder::encode_mate(bool mte){ return mte ? MATE_ : 0; }

BYTE Encoder::encode_pawn_promotion(ptype promo, bool pce){ return static_cast<BYTE>(promo) << (pce ? 5 : 2); }

BYTE Encoder::encode_queen_axis(const coords src, const coords dst){ return ((src[0]==dst[0]) || (src[1]==dst[1])) ? QAROOK_ : QABISHOP_; }

BYTE Encoder::pawn_action(const coords src, const coords dst, bool cap, ptype promo){
    // 000ppp<rr/ff>
    BYTE res;
    if(cap)  { res = dst[1] > src[1] ? 2 : 1; }
    else     { res = abs(dst[0]-src[0]); }
    if(promo){ res |= this->encode_pawn_promotion(promo, false); }
    return res;
}

BYTE Encoder::knight_action(const coords src, const coords dst){
    // 00000aot
    bool f2 = abs(dst[1] - src[1]) == 2,
         rneg = dst[0] < src[0],
         fneg = dst[1] < src[1];

    return  (4 * f2) |
            2*((f2 & rneg) || (!f2 & fneg)) |
            1*((f2 & fneg) || (!f2 & rneg));
}

BYTE Encoder::bishop_action(const coords src, const coords dst){
    // 000admmm
    coords delta{dst[0] - src[0], dst[1] - src[1]};

    return  16*(delta[0] != delta[1]) |
            8*(delta[0] < 0) |
            abs(delta[0]);
}

BYTE Encoder::rook_action(const coords src, const coords dst){
    // 000admmm
    coords delta{dst[0] - src[0], dst[1] - src[1]};

    return  16 * !delta[1] |
            8*((delta[0] | delta[1]) < 0) |
            (abs(delta[0]) | abs(delta[1]));
}

BYTE Encoder::queen_action(const coords src, const coords dst){
    // 000admmm
    if((dst[0]==src[0]) | (dst[1]==src[1])){ return this->rook_action(src, dst); }
    else                                   { return this->bishop_action(src, dst); }
}

BYTE Encoder::king_action(const coords src, const coords dst){
    // 000cffrr
    coords delta{dst[0] - src[0], dst[1] - src[1]};

    return  16*(abs(delta[1]) == 2) |
            (delta[1] ? (delta[1] > 0 ? 4 : 8) : 0) |
            (delta[0] ? (delta[0] > 0 ? 1 : 2) : 0);
}

/**************************************************
                      DECODER
**************************************************/

std::vector<ply> Decoder::decode_game(const std::vector<eply>& g, const std::string fstr){

    std::vector<ply> res;
    if(!fstr.empty()){ fen::parse(fstr, this->board); }
    else             { this->board.reset(); }

    for(std::vector<eply>::const_iterator it=g.begin(); it!=g.end(); ++it){
        res.emplace_back(this->decode_ply(*it, this->board));
        this->board.update(res.back());
    }
    return res;
}

ply Decoder::decode_ply(eply e, Board& board){

    if(e == this->emissing){ return this->missing; }

    ply p;
    BYTE piece  = e >> 8,
         action = e & 255;

    p.type = this->decode_type(piece);
    p.c = this->decode_color(piece);

    p.capture = this->decode_capture(action);
    p.check = this->decode_check(action);
    p.mate = this->decode_mate(action);

    if(p.type == pawn){ p.promo = this->decode_pawn_promotion(action); }
    else              { p.promo = pawn; }

    U64 bb = board.board(p.type, p.c);
    int cnt = (piece >> 4) & 7;
    for(int i=0; i<cnt; ++i){ lsbflip(bb); }
    coords src(mask(bitscan(bb))),
           dst, delta;

    switch(p.type){
        case pawn:   delta = this->pawn_action(action, p.c); break;
        case knight: delta = this->knight_action(action); break;
        case bishop: delta = this->bishop_action(action); break;
        case rook:   delta = this->rook_action(action); break;
        case queen:  delta = this->queen_action(action, this->decode_queen_axis(piece)); break;
        default:     delta = this->king_action(action);
    }
    dst = src + delta;
    p.src = mask(8*src[0] + src[1]);
    p.dst = mask(8*dst[0] + dst[1]);

    if(p.type == king){
        switch(delta[1]){
            case  2: p.castle =  1; break;
            case -2: p.castle = -1; break;
            default: p.castle =  0;
        }
    }

    return p;
}

ptype Decoder::decode_type(BYTE _piece){ return static_cast<ptype>((_piece & 15) % 6); }

color Decoder::decode_color(BYTE _piece){ return (_piece & 15) >= 6 ? black : white; }

bool Decoder::decode_capture(BYTE _action){ return CAPTURE_ & _action; }

bool Decoder::decode_check(BYTE _action){ return CHECK_ & _action; }

bool Decoder::decode_mate(BYTE _action){ return MATE_ & _action; }

ptype Decoder::decode_pawn_promotion(BYTE _action){ return static_cast<ptype>((_action & 28) >> 2); }

ptype Decoder::decode_queen_axis(BYTE _piece){ return (QAROOK_ & _piece) ? rook : bishop; }

coords Decoder::pawn_action(BYTE _action, color c){
    // 000ppp<rr/ff>
    bool cap = _action & 32,
         m = _action & 2;

    int dr = c * (cap ? 1 : (m ? 2 : 1)),
        df = cap ? (m ? 1 : -1) : 0;

    return {dr, df};
}

coords Decoder::knight_action(BYTE _action){
    // 00000aot
    int o = 1 - (2 * (bool)(_action & 2)),
        t = 2 - (4 * (bool)(_action & 1));
    if(_action & 4){ return {o, t}; }
    else           { return {t, o}; }
}

coords Decoder::bishop_action(BYTE _action){
    // 000admmm
    bool d = _action & 8,
         a = _action & 16;
    int  m = _action & 7;
    if(d ^ a){
        if(d){ return {-m, -m}; }
        else { return { m, -m}; }
    }
    else{
        if(d){ return {-m, m}; }
        else { return { m, m}; }
    }
}

coords Decoder::rook_action(BYTE _action){
    // 000admmm
    int val = (1 - (2 * ((_action & 8) >> 3))) * (_action & 7);
    if(_action & 16){ return {val, 0}; }
    else            { return {0, val}; }
}

coords Decoder::queen_action(BYTE _action, ptype pt){
    // 000admmm
    switch(pt){
        case rook: return this->rook_action(_action);
        default:   return this->bishop_action(_action);
    }
}

coords Decoder::king_action(BYTE _action){
    // 000cffrr
    int dr = _action & 3,
        df = (_action & 12) >> 2;

    if(dr == 2){ dr = -1; }
    if(df == 2){ df = -1; }
    if(_action & 16){ df *= 2; }
    return {dr, df};
}

/**************************************************
                      PGN
**************************************************/

template<typename CharT, typename Traits>
PGN<CharT, Traits>::PGN(const string& file) : ParseBuf<CharT,Traits>(file, "rb"){}

/*
    ParseBuf Overrides
*/
template<typename CharT, typename Traits>
void PGN<CharT, Traits>::read(){
    // Tags
    this->_fdev.xsgetn(this->_buf, BUFSIZE, this->_delim);
    this->tend = this->_buf.size();
    // Moves
    this->_fdev.xsgetn(this->_buf, BUFSIZE, this->_delim);
}

template<typename CharT, typename Traits>
typename PGN<CharT, Traits>::int_type PGN<CharT, Traits>::rparse(){
    int_type size = this->_buf.size();
    this->tags();
    this->movetext();
    return this->_fdev.eof() ? Traits::eof() : size;
}

/*
    Parsing Functions
*/

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::tags(){
    string tbuf = this->_buf.substr(0,this->tend);
    this->_tags.reset();

    // strip brackets
    for(const char* c=tag_delims; c!=std::end(tag_delims); ++c){ tbuf.erase(std::remove(tbuf.begin(), tbuf.end(), *c), tbuf.end()); }

    // move tbuf to sstream for tokenizing
    std::basic_stringstream<CharT, Traits> bufstr;
    bufstr.str(std::move(tbuf));

    while(std::getline(bufstr, tbuf)){

        int_type pos = tbuf.find(' ');
        std::transform(tbuf.begin(), tbuf.begin()+pos, tbuf.begin(), tolower);
        string keytok = tbuf.substr(0, pos);

        if(s2tag.find(keytok) != s2tag.end()){
            // this->_tags[s2tag[keytok]] = tbuf.substr(pos+2, this->tend-1);
            this->_tags[s2tag[keytok]] = tbuf.substr(pos+2, tbuf.size()-1);
        }
    }
}

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::movetext(){

    string mbuf = this->_buf.substr(this->tend);

    // Check for elided white ply at start of movetext
    bool black_starts = std::regex_search(mbuf.substr(0,6), elided);

    // Concatenate lines
    int_type idx;
    while((idx = mbuf.find('\n')) != string::npos){
        // All \n must be preceded by '.' or ' '
        if(mbuf[idx-1] != '.' && mbuf[idx-1] != ' '){ mbuf.insert(idx++, 1, ' '); }
        mbuf.erase(idx, 1);
    }

    // Tokenize into individual plies
    this->_tokens.clear();
    std::stringstream sstr(mbuf);
    while(!sstr.eof()){
        sstr >> mbuf;
        if(isdigit(mbuf[0])){ mbuf.erase(0, mbuf.find('.')+1); }
        while((mbuf[0] == '.') | (mbuf[0] == '-')){ mbuf.erase(0,1); }
        this->_tokens.emplace_back(mbuf);
    }

    // Parse ply tokens
    this->pplies(black_starts ? black : white);
}

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::pplies(color c){

    this->board.reset();
    this->_plies.clear();

    if(!this->_tags[fenstr].empty()){ fen::parse(this->_tags[fenstr], this->board); }
    else{ this->board.next = c; }

    for(typename std::vector<string>::iterator it=this->_tokens.begin(); it!=this->_tokens.end(); ++it){
        
        if(it->empty()){ this->_plies.emplace_back(); }
        else
        if(
            (*it ==       "*") ||
            (*it ==     "1-0") ||
            (*it ==     "0-1") ||
            (*it == "1/2-1/2")
        ){ break; }
        else{
            ply p = this->pply(*it, c);
            board.update(p);
            this->_plies.emplace_back(p);
        }
        c = !c;
    }
}

template<typename CharT, typename Traits>
ply PGN<CharT, Traits>::pply(PGN<CharT, Traits>::string p, color c){
    // Parse and remove flags
    bool check = p.back() == '+',
         mate = p.back() == '#';
    if(check || mate){ p.erase(p.size()-1, 1); }

    bool kingside{!p.compare("O-O")},
         queenside{!p.compare("O-O-O")};

    // Shortcut for castles
    if(kingside | queenside){ return this->pcastle(queenside, c, check, mate); }
    else                    { return this->prest(p, c, check, mate); }
}

template<typename CharT, typename Traits>
ply PGN<CharT, Traits>::pcastle(bool qs, color c, bool check, bool mate){
    U64 src = this->board.board(king, c),
        dst = qs ? (src >> 2) : (src << 2);
    return {src, dst, king, pawn, c, qs ? -1 : 1, false, check, mate};
}

template<typename CharT, typename Traits>
ply PGN<CharT, Traits>::prest(const PGN<CharT, Traits>::string p, color c, bool check, bool mate){
    int_type f = 0, r = 0;
    square srcsq = 0, dstsq = 0;
    bool capture = false;
    ptype pt = pawn, promo = pawn;

    for(typename string::const_reverse_iterator it=p.rbegin(); it!=p.rend(); ++it){
        char_type ch = *it;
        if(isfile(ch))      { (!f++ ? dstsq : srcsq) += c2file(ch);     }
        else
        if(std::isdigit(ch)){ (!r++ ? dstsq : srcsq) += c2rank(ch) * 8; }
        else
        if(ispiece(ch))     { pt = c2ptype(ch); }
        else
        if(ch == 'x')       { capture = true; }
        else
        if(ch == '=')       { promo = pt; pt = pawn; }
    }
    
    U64 dst = mask(dstsq),
        src = 0;
    if(r==2 && f==2) { src = mask(srcsq);  }
    else{
        if(r==2)     { src = rank(srcsq); }
        else
        if(f==2)     { src = file(srcsq); }

        src = disamb::pgn(src, dst, pt, c, this->board, capture);
    }

    return {src, dst, pt, promo, c, 0, capture, check, mate};
}

/**************************************************
                      PGNSTREAM
**************************************************/

template<typename CharT, typename Traits>
PGNStream<CharT, Traits>::PGNStream(const std::basic_string<CharT>& file)
    : ParseStream<CharT, Traits>(),
    _pbuf(file) { this->rdbuf(&this->_pbuf); }

template<typename CharT, typename Traits>
PGNStream<CharT, Traits>::PGNStream(const CharT* file)
    : PGNStream(std::string(file)) {}

template<typename CharT, typename Traits>
PGNStream<CharT, Traits>& PGNStream<CharT, Traits>::operator>>(game& g){
    this->read(nullptr, 0);
    g.tags = this->_pbuf._tags;
    g.plies = this->_pbuf._plies;
    return *this;
}

template<typename CharT, typename Traits>
PGNStream<CharT, Traits>& PGNStream<CharT, Traits>::operator>>(std::vector<game>& games){
    this->read(nullptr, 0);
    games.emplace_back(this->_pbuf._tags, this->_pbuf._plies);
    return *this;
}

template<typename CharT, typename Traits>
game PGNStream<CharT, Traits>::next(){
    this->read(nullptr, 0);
    return {this->_pbuf._tags, this->_pbuf._plies};
}


template<typename CharT, typename Traits>
void PGNStream<CharT, Traits>::close(){ this->_pbuf.close(); }


/**************************************************
                      CHESSDB
**************************************************/
template<typename CharT, typename Traits>
ChessDB<CharT, Traits>::ChessDB(const string& file, bool exists) : ParseBuf<CharT,Traits>(file, exists ? "rb+" : "wb+")
{
    if(exists){ this->load(); }
    else      { this->create(); }
}

template<typename CharT, typename Traits>
ChessDB<CharT, Traits>::~ChessDB(){
    // Dump remaining stats
    this->detach();
    // Close file
    this->close();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::load(){

    this->load_header();
    this->load_index();
    this->load_tag_enum();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::create(){

    this->write_header();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::detach(){

    this->encode = false;
    this->write_header();
    /*
        WRITE INDEX BEFORE TAG ENUM
    */
    this->write_index();
    this->write_tag_enum();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::seek_game(int i){ this->_fdev.seek(this->index[i].first); }

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::seek_index(){ this->_fdev.seek(this->IDXPOS); }

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::seek_tags(){ this->_fdev.seek(this->IDXPOS + this->NGAMES * IDXSZ); }



/*
    WRITE FUNCS
*/

template<typename CharT, typename Traits>
typename ChessDB<CharT, Traits>::int_type
ChessDB<CharT, Traits>::wparse(){
    if(this->encode){ return this->encode_game(); }
    else            { return 0;                   } // Unformatted output (i.e. anything BUT game data)
}

template<typename CharT, typename Traits>
typename ChessDB<CharT, Traits>::int_type
ChessDB<CharT, Traits>::encode_game(){
    const game g = *reinterpret_cast<game*>(this->_buf.data());
    this->sync();

    // Tags
    for(pgndict::const_iterator tagit=g.tags.cbegin(); tagit!=g.tags.cend(); ++tagit){
        uint32_t val = this->enum_extend(tagit->second);
        this->_buf.append((char_type*)&val, TAGSZ);
    }

    // Num plies
    uint16_t nplies = g.plies.size();
    this->_buf.append((char_type*)&nplies, sizeof(nplies));

    // Plies
    std::vector<eply> eplies = this->enc.encode_game(g);
    for(int i=0; i<nplies; ++i){ this->_buf.append((char_type*)&eplies[i], EPYSZ); }

    // Increment game count and update index
    this->index.insert({++this->NGAMES - 1, {this->_fdev.tell(), nplies}});

    // Update index pos
    this->IDXPOS += ATGSZ + NPYSZ + nplies*EPYSZ;

    return this->_buf.size();
}

template<typename CharT, typename Traits>
uint32_t ChessDB<CharT, Traits>::enum_extend(const ChessDB<CharT, Traits>::string& tag){
    if(this->tag_enumerations.find(tag) == this->tag_enumerations.end()){
        this->tag_enumerations.emplace(tag, ++this->NTAGS - 1);
        this->tags.emplace_back(tag);
    }
    return this->tag_enumerations[tag];
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::write_tag_enum(){

    this->seek_tags();
    this->sync();

    std::stringstream bufstr;
    std::copy(this->tags.begin(), this->tags.end(), std::ostream_iterator<std::string>(bufstr, "\n"));
    this->_buf = std::move(*bufstr.rdbuf()).str();

    this->write();
    this->sync();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::write_index(){

    this->seek_index();
    this->sync();

    for(unsigned int i=0; i<this->index.size(); ++i){
        this->_buf.append((char_type*)&this->index[i].first, sizeof(int32_t));
        this->_buf.append((char_type*)&this->index[i].second, NPYSZ);
    }

    this->write();
    this->sync();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::write_header(){

    this->_fdev.seek(0);
    this->sync();

    // Time, NGAMES, NTAGS, IDXPOS
    uint64_t tme = Tempus::time();
    this->_buf.append((char_type*)&tme, TMESZ);
    this->_buf.append((char_type*)&this->NGAMES, TAGSZ);
    this->_buf.append((char_type*)&this->NTAGS, TAGSZ);
    this->_buf.append((char_type*)&this->IDXPOS, TAGSZ);


    this->write();
    this->sync();
}

/*
    READ FUNCS
*/

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::read(){
    // Tags
    this->_fdev.xsgetn(this->_buf, ATGSZ);
    // Plies
    std::string tmp;
    this->_fdev.xsgetn(tmp, NPYSZ);
    this->_fdev.xsgetn(this->_buf, (*(uint16_t*)tmp.data()) * EPYSZ);
}

template<typename CharT, typename Traits>
typename ChessDB<CharT, Traits>::int_type
ChessDB<CharT, Traits>::rparse(){
    this->rg.tags.reset();
    this->rg.plies.clear();
    int loc = 0,
        N = this->_buf.size();

    for(pgndict::iterator it=this->rg.tags.begin(); it!=this->rg.tags.end(); ++it){
        it->second = this->tags[*(uint32_t*)(this->_buf.data() + loc)];
        loc += TAGSZ;
    }

    std::vector<eply> eplies;
    while(loc < N){
        eplies.emplace_back(*(eply*)(this->_buf.data() + loc));
        loc += EPYSZ;
    }

    this->rg.plies = this->dec.decode_game(eplies, this->rg.tags[fenstr]);

    return this->rg.plies.size();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::load_header(){

    this->_fdev.seek(0);
    this->sync();

    this->_fdev.xsgetn(this->_buf, HDRSZ);

    // Timestamp
    // uint64_t last_read_t = *(uint64_t*)this->_buf.data();

    this->NGAMES = *(uint32_t*)(this->_buf.data() + TMESZ);
    this->NTAGS = *(uint32_t*)(this->_buf.data() + TMESZ + TAGSZ);
    this->IDXPOS = *(uint32_t*)(this->_buf.data() + TMESZ + 2*TAGSZ);


    this->sync();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::load_tag_enum(){

    this->seek_tags();
    this->sync();

    // Read tags (from current position to EOF)
    int32_t loc = this->_fdev.tell();
    this->_fdev.seek(0, SEEK_END);
    int32_t end = this->_fdev.tell();
    this->_fdev.seek(loc, SEEK_SET);
    this->_fdev.xsgetn(this->_buf, end-loc);

    // Move buffer to stringstream and split on \n
    std::stringstream bufstr;
    bufstr.str(std::move(this->_buf));
    this->tags.resize(this->NTAGS);
    for(unsigned int i=0; i<this->NTAGS; ++i){
        std::getline(bufstr, this->tags[i], '\n');
        this->tag_enumerations[this->tags[i]] = i;
    }

    this->sync();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::load_index(){

    this->seek_index();
    this->sync();

    std::size_t sz = this->NGAMES * IDXSZ;
    this->_fdev.xsgetn(this->_buf, sz);

    this->index.reserve(this->NGAMES);
    for(unsigned int i=0; i<this->NGAMES; ++i){
        int loc = i*IDXSZ;
        int32_t pos = *(int32_t*)(this->_buf.data() + loc);
        uint16_t npl = *(uint16_t*)(this->_buf.data() + loc + sizeof(int32_t));
        index.emplace(i, std::make_pair(pos, npl));
    }
    this->sync();
}

/**************************************************
                  CHESSDBSTREAM
**************************************************/

template<typename CharT, typename Traits>
ChessDBStream<CharT, Traits>::ChessDBStream(const std::basic_string<CharT>& file)
    : ParseStream<CharT, Traits>(),
      _pbuf(file, std::filesystem::exists(file)) { this->rdbuf(&this->_pbuf); }

template<typename CharT, typename Traits>
ChessDBStream<CharT, Traits>& ChessDBStream<CharT, Traits>::operator<<(const game& g){
    this->_pbuf.encode = true;
    this->write((char_type*)&g, sizeof(g));
    return *this;
}

template<typename CharT, typename Traits>
ChessDBStream<CharT, Traits>& ChessDBStream<CharT, Traits>::operator>>(game& g){
    this->read(nullptr, 0);
    g = this->_pbuf.rg;
    return *this;
}

template<typename CharT, typename Traits>
void ChessDBStream<CharT, Traits>::insert(const game& g){
    this->_pbuf.seek_index();
    *this << g;
}

template<typename CharT, typename Traits>
game ChessDBStream<CharT, Traits>::select(int i){
    this->_pbuf.seek_game(i);
    game res;
    *this >> res;
    return res;
}

template<typename CharT, typename Traits>
int ChessDBStream<CharT, Traits>::size(){ return this->_pbuf.NGAMES; }



template class PGN<char>;
// template class PGN<wchar_t>;
template class PGNStream<char>;
// template class PGNStream<wchar_t>;
template class ChessDB<char>;
template class ChessDBStream<char>;