#include "include/chessdb.hpp"

namespace {
    ply missing;
    eply emissing{UINT16_MAX};
}

namespace encode{
    std::vector<eply> Game(const game& g){
        std::vector<eply> res;
        Board board;

        if(!g.tags.at(fenstr).empty()){ fen::parse(g.tags.at(fenstr), board); }
        else                          { board.reset(); }

        for(std::vector<ply>::const_iterator it=g.plies.begin(); it!=g.plies.end(); ++it){
            res.emplace_back(Ply(*it, board));
            board.update(*it);
        }
        return res;
    }

    eply Ply(const ply p, Board& board){
        if(p == missing){ return emissing; }

        BYTE _piece = piece(p, board),
             _action = action(p);

        return (static_cast<uint16_t>(_piece) << 8) | _action;
    }

    BYTE piece(const ply p, Board& board){
        U64 bb = board.board(p.type, p.c);
        int cnt = 0, src = bitscan(p.src), dst = bitscan(p.dst);
        while(bb){
            if(lsbpop(bb) == src){ break; }
            ++cnt;
        }
        BYTE _piece = static_cast<BYTE>(p.type + 6*(p.c<0)) | (cnt << 4);
        if(p.type == queen){ _piece |= ((rank(src) == rank(dst)) || (file(src) == file(dst))) ? QAROOK_ : QABISHOP_; }
        return _piece;
    }

    BYTE action(const ply p){
        coords src = {p.src},
               dst = {p.dst};

        BYTE _action = (p.capture ? CAPTURE_ : 0) | (p.check ? CHECK_ : 0) | (p.mate ? MATE_ : 0 );
        switch(p.type){
            case pawn:   _action |= pawn_action(src, dst, p.capture, p.promo); break;
            case knight: _action |= knight_action(src, dst);                   break;
            case bishop: _action |= bishop_action(src, dst);                   break;
            case rook:   _action |= rook_action(src, dst);                     break;
            case queen:  _action |= queen_action(src, dst);                    break;
            default:     _action |= king_action(src, dst);
        }
        return _action;
    }

    BYTE pawn_action(const coords src, const coords dst, bool cap, ptype promo){
        // 000ppp<rr/ff>
        BYTE res;
        if(cap)  { res = dst[1] > src[1] ? 2 : 1; }
        else     { res = abs(dst[0]-src[0]); }
        if(promo){ res |= promo << 2; }
        return res;
    }

    BYTE knight_action(const coords src, const coords dst){
        // 00000aot
        bool f2 = abs(dst[1] - src[1]) == 2,
             rneg = dst[0] < src[0],
             fneg = dst[1] < src[1];

        return  (4 * f2) |
                2*((f2 & rneg) || (!f2 & fneg)) |
                1*((f2 & fneg) || (!f2 & rneg));
    }

    BYTE bishop_action(const coords src, const coords dst){
        // 000admmm
        coords delta{dst[0] - src[0], dst[1] - src[1]};

        return  16*(delta[0] != delta[1]) |
                8*(delta[0] < 0) |
                abs(delta[0]);
    }

    BYTE rook_action(const coords src, const coords dst){
        // 000admmm
        coords delta{dst[0] - src[0], dst[1] - src[1]};

        return  16 * !delta[1] |
                8*((delta[0] | delta[1]) < 0) |
                (abs(delta[0]) | abs(delta[1]));
    }

    BYTE queen_action(const coords src, const coords dst){
        // 000admmm
        if((dst[0]==src[0]) | (dst[1]==src[1])){ return rook_action(src, dst); }
        else                                   { return bishop_action(src, dst); }
    }

    BYTE king_action(const coords src, const coords dst){
        // 000cffrr
        coords delta{dst[0] - src[0], dst[1] - src[1]};

        return  16*(abs(delta[1]) == 2) |
                (delta[1] ? (delta[1] > 0 ? 4 : 8) : 0) |
                (delta[0] ? (delta[0] > 0 ? 1 : 2) : 0);
    }
}

namespace decode{
    std::vector<ply> Game(const std::vector<eply>& g, const std::string fstr){
        std::vector<ply> res;
        Board board;

        if(!fstr.empty()){ fen::parse(fstr, board); }
        else             { board.reset(); }

        for(std::vector<eply>::const_iterator it=g.begin(); it!=g.end(); ++it){
            res.emplace_back(Ply(*it, board));
            board.update(res.back());
        }
        return res;
    }

    ply Ply(eply e, Board& board){

        if(e == emissing){ return missing; }

        ply p;
        BYTE _piece  = e >> 8,
             _action = e & 255;

        // color, type, promo
        p.c = (_piece & 15) >= 6 ? black : white;
        p.type = static_cast<ptype>((_piece & 15) % 6);
        p.promo = (p.type == pawn) ? static_cast<ptype>((_action & 28) >> 2) : pawn;

        // capture, check, mate
        p.capture = CAPTURE_ & _action;
        p.check = CHECK_ & _action;
        p.mate = MATE_ & _action;

        // src coordinates
        U64 bb = board.board(p.type, p.c);
        int cnt = (_piece >> 4) & 7;
        for(int i=0; i<cnt; ++i){ lsbflip(bb); }
        coords src(bitscan(bb)),
               dst, delta;

        // dst coordinates
        switch(p.type){
            case pawn:   delta = pawn_action(_action, p.c);    break;
            case knight: delta = knight_action(_action);       break;
            case bishop: delta = bishop_action(_action);       break;
            case rook:   delta = rook_action(_action);         break;
            case queen:  delta = queen_action(_action, _piece); break;
            default:     delta = king_action(_action);
        }

        dst = src + delta;
        p.src = mask(8*src[0] + src[1]);
        p.dst = mask(8*dst[0] + dst[1]);

        // castle
        if(p.type == king){
            switch(delta[1]){
                case  2: p.castle =  1; break;
                case -2: p.castle = -1; break;
                default: p.castle =  0;
            }
        }

        return p;
    }

    coords pawn_action(BYTE _action, color c){
        // 000ppp<rr/ff>
        bool cap = _action & 32,
             m = _action & 2;

        int dr = c * (cap ? 1 : (m ? 2 : 1)),
            df = cap ? (m ? 1 : -1) : 0;

        return {dr, df};
    }

    coords knight_action(BYTE _action){
        // 00000aot
        int o = 1 - (2 * (bool)(_action & 2)),
            t = 2 - (4 * (bool)(_action & 1));
        if(_action & 4){ return {o, t}; }
        else           { return {t, o}; }
    }

    coords bishop_action(BYTE _action){
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

    coords rook_action(BYTE _action){
        // 000admmm
        int val = (1 - (2 * ((_action & 8) >> 3))) * (_action & 7);
        if(_action & 16){ return {val, 0}; }
        else            { return {0, val}; }
    }

    coords queen_action(BYTE _action, BYTE _piece){
        // 000admmm
        if(QAROOK_ & _piece){ return rook_action(_action); }
        else                { return bishop_action(_action); }
    }

    coords king_action(BYTE _action){
        // 000cffrr
        int dr = _action & 3,
            df = (_action & 12) >> 2;

        if(dr == 2){ dr = -1; }
        if(df == 2){ df = -1; }
        if(_action & 16){ df *= 2; }
        return {dr, df};
    }
}

/**************************************************
                      PGN
**************************************************/

template<typename CharT, typename Traits>
PGN<CharT, Traits>::~PGN() = default;

template<typename CharT, typename Traits>
PGN<CharT, Traits>::PGN(const string& filename) : ParseBuf<CharT,Traits>(filename, "rb"){
    ptype pt;
    for(int i=pawn; i<=king; ++i){
        pt = static_cast<ptype>(i);
        switch(pt){
            case pawn:   this->ply_map[pt].reserve(psz); break;
            case knight: this->ply_map[pt].reserve(nsz); break;
            case bishop: this->ply_map[pt].reserve(bsz); break;
            case rook:   this->ply_map[pt].reserve(rsz); break;
            case queen:  this->ply_map[pt].reserve(qsz); break;
            default:     this->ply_map[pt].reserve(ksz);
        }
    }
    /**************************************************
                            PAWNS
    **************************************************/

    pt = pawn;
    for(square dst=0; dst<64; ++dst){

        int f = dst%8;
        U64 dmask = mask(dst);
        
        std::string dsts = coord2s(dst);
        int lb, ub;
        if(dst<8 || dst>55){
            // promotion
            dsts += '=';
            lb = knight;
            ub = queen;
        }
        else{
            lb = pawn;
            ub = pawn;
        }
        
        color c = white;
        for(int i=lb; i<=ub; ++i){
            ptype promo = static_cast<ptype>(i);
            if(promo){ dsts += ptype2c(promo); }

            this->ply_map[pt].emplace_back(file(dst), dmask, pawn, promo, c, 0, false, false, false);
            this->san_map[dsts] = &this->ply_map[pt].back();
            this->ply_map[pt].emplace_back(file(dst), dmask, pawn, promo, c, 0, false, true, false);
            this->san_map[dsts + '+'] = &this->ply_map[pt].back();
            this->ply_map[pt].emplace_back(file(dst), dmask, pawn, promo, c, 0, false, true, true);
            this->san_map[dsts + '#'] = &this->ply_map[pt].back();

            if(f>0){
                std::string pref = std::string(1, file2c(f-1)) + 'x';
                this->ply_map[pt].emplace_back(file(dst-1), dmask, pawn, promo, c, 0, true, false, false);
                this->san_map[pref + dsts] = &this->ply_map[pt].back();
                this->ply_map[pt].emplace_back(file(dst-1), dmask, pawn, promo, c, 0, true, true, false);
                this->san_map[pref + dsts + '+'] = &this->ply_map[pt].back();
                this->ply_map[pt].emplace_back(file(dst-1), dmask, pawn, promo, c, 0, true, true, true);
                this->san_map[pref + dsts + '#'] = &this->ply_map[pt].back();
            }
            if(f<7){
                std::string pref = std::string(1, file2c(f+1)) + 'x';
                this->ply_map[pt].emplace_back(file(dst+1), dmask, pawn, promo, c, 0, true, false, false);
                this->san_map[pref + dsts] = &this->ply_map[pt].back();
                this->ply_map[pt].emplace_back(file(dst+1), dmask, pawn, promo, c, 0, true, true, false);
                this->san_map[pref + dsts + '+'] = &this->ply_map[pt].back();
                this->ply_map[pt].emplace_back(file(dst+1), dmask, pawn, promo, c, 0, true, true, true);
                this->san_map[pref + dsts + '#'] = &this->ply_map[pt].back();
            }

            dsts.erase(dsts.size()-1, 1);
        }
    }

    /**************************************************
                    BACK ROW PIECES
    **************************************************/
    for(int i=knight; i<=queen; ++i){
        pt = static_cast<ptype>(i);

        for(square dst=0; dst<64; ++dst){
            U64 dmask = mask(dst),
                src = attack(dst, pt);
            brp(src, dmask, pt, false, false, false);
            brp(src, dmask, pt, true, false, false);
            brp(src, dmask, pt, false, true, false);
            brp(src, dmask, pt, false, true, true);
            brp(src, dmask, pt, true, true, false);
            brp(src, dmask, pt, true, true, true);
        }
    }

    /**************************************************
                            KING
    **************************************************/
    pt = king;
    std::string str = "K";
    for(square src=0; src<64; ++src){
        U64 bb = attack(src, pt);
        while(bb){
            square dst = lsbpop(bb);
            std::string dsts = coord2s(dst);

            this->ply_map[pt].emplace_back(src, dst, pt, pawn, white,  0, false, false, false);
            this->san_map[str + dsts] = &this->ply_map[pt].back();

            this->ply_map[pt].emplace_back(src, dst, pt, pawn, white,  0, false, true, false);
            this->san_map[str + dsts + '+'] = &this->ply_map[pt].back();

            this->ply_map[pt].emplace_back(src, dst, pt, pawn, white,  0, false, true, true);
            this->san_map[str + dsts + '#'] = &this->ply_map[pt].back();

            this->ply_map[pt].emplace_back(src, dst, pt, pawn, white,  0, true, false, false);
            this->san_map[str + 'x' + dsts] = &this->ply_map[pt].back();

            this->ply_map[pt].emplace_back(src, dst, pt, pawn, white,  0, true, true, false);
            this->san_map[str + 'x' + dsts + '+'] = &this->ply_map[pt].back();

            this->ply_map[pt].emplace_back(src, dst, pt, pawn, white,  0, true, true, true);
            this->san_map[str + 'x' + dsts + '#'] = &this->ply_map[pt].back();
        }
    }
    // castles
    this->ply_map[pt].emplace_back(4, 6, pt, pawn, white,  1, false, false, false);
    this->san_map["O-O"] = &this->ply_map[pt].back();

    this->ply_map[pt].emplace_back(4, 6, pt, pawn, white,  1, false, true, false);
    this->san_map["O-O+"] = &this->ply_map[pt].back();

    this->ply_map[pt].emplace_back(4, 6, pt, pawn, white,  1, false, true, true);
    this->san_map["O-O#"] = &this->ply_map[pt].back();

    this->ply_map[pt].emplace_back(4, 2, pt, pawn, white, -1, false, false, false);
    this->san_map["O-O-O"] = &this->ply_map[pt].back();

    this->ply_map[pt].emplace_back(4, 2, pt, pawn, white, -1, false, true, false);
    this->san_map["O-O-O+"] = &this->ply_map[pt].back();

    this->ply_map[pt].emplace_back(4, 2, pt, pawn, white, -1, false, true, true);
    this->san_map["O-O-O#"] = &this->ply_map[pt].back();
}

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::brp(U64 src, U64 dst, ptype pt, bool cap, bool chk, bool mte){

    // SAN string
    std::string str = std::string(1, ptype2c(pt)),
                dsts = coord2s(bitscan(dst));

    if(cap){ dsts = std::string(1, 'x') + dsts; }
    if(mte){ dsts += '#'; }
    else
    if(chk){ dsts += '+'; }

    // Construct ply and add ptr to map
    this->ply_map[pt].emplace_back(0ULL, dst, pt, pawn, white, 0, cap, chk, mte);
    this->san_map[str + dsts] = &this->ply_map[pt].back();

    // Further entries in the map for disambiguated SAN strings
    std::array<bool, 8> rs = {false},
                        fs = {false};
    std::array<bool, 64> both = {false};
    while(src){
        square tst = lsbpop(src);
        int r = tst/8, f = tst%8;

        if(rs[r] && fs[f] && both[tst]){ continue; }
        if(!rs[r]){
            this->ply_map[pt].emplace_back(rank(tst), dst, pt, pawn, white, 0, cap, chk, mte);
            this->san_map[str + rank2c(r) + dsts] = &this->ply_map[pt].back();
        }
        if(!fs[f]){
            this->ply_map[pt].emplace_back(file(tst), dst, pt, pawn, white, 0, cap, chk, mte);
            this->san_map[str + file2c(f) + dsts] = &this->ply_map[pt].back();
        }
        if(!both[tst]){
            this->ply_map[pt].emplace_back(mask(tst), dst, pt, pawn, white, 0, cap, chk, mte);
            this->san_map[str + file2c(f) + rank2c(r) + dsts] = &this->ply_map[pt].back();
        }

        rs[tst/8] = true;
        fs[tst%8] = true;
        both[tst] = true;
    }
}

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
            this->_tags[s2tag[keytok]] = tbuf.substr(pos+2, tbuf.size()-(pos+3));
        }
    }
}

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::movetext(){

    string mbuf = this->_buf.substr(this->tend);

    // Check for elided white ply at start of movetext
    bool black_starts = std::regex_search(mbuf.substr(0,6), elided);

    // Concatenate lines
    typename std::allocator<CharT>::size_type idx;
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
        while((mbuf[0] == '.') || (mbuf[0] == '-')){ mbuf.erase(0,1); }
        this->_tokens.emplace_back(mbuf);
    }

    // Reset board
    this->_plies.clear();
    this->board.reset();
    if(!this->_tags[fenstr].empty()){ fen::parse(this->_tags[fenstr], this->board);    }
    else                            { this->board.next = black_starts ? black : white; }

    // Parse ply tokens
    ply p;
    for(typename std::vector<string>::iterator it=this->_tokens.begin(); it!=this->_tokens.end(); ++it){
        
        if(it->empty()){
            // Default constructed ply == Missing
            this->_plies.emplace_back();
            board.next = !board.next;
        }
        else
        if((*it == "*") || (*it == "1-0") || (*it == "0-1") || (*it == "1/2-1/2")){
            // End-of-game token
            break;
        }
        else{

            // Lookup pre-constructed ply for this token
            p = *this->san_map[*it];
            p.c = board.next;

            if(p.castle && p.c<0){
                // Adjust castle plies for black
                p.src = black_king;
                p.dst <<= 56;
            }
            else
            if(p.type == king){
                // King is never ambiguous
                p.src = this->board.board(king, p.c);
            }
            else
            if(popcnt(p.src) != 1){
                // Disambiguate
                p.src = disamb::pgn(p.src, p.dst, p.type, p.c, this->board, p.capture);
            }

            // Update board and save ply
            this->board.update(p);
            this->_plies.emplace_back(p);
        }
    }
}

/**************************************************
                      PGNSTREAM
**************************************************/

template<typename CharT, typename Traits>
PGNStream<CharT, Traits>::PGNStream(const string& file)
    : ParseStream<CharT, Traits>(),
    _pbuf(file) { this->rdbuf(&this->_pbuf); }

template<typename CharT, typename Traits>
PGNStream<CharT, Traits>::PGNStream(const CharT* file) : PGNStream(string(file)) {}

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
ChessDB<CharT, Traits>::ChessDB(const string& file, bool exists) : ParseBuf<CharT,Traits>(file, exists ? "rb+" : "wb+"){
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
    this->_buf.append((char_type*)&nplies, NPYSZ);

    // Plies
    std::vector<eply> eplies = encode::Game(g);
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
        this->_buf.append((char_type*)&this->index[i].first, FBTSZ);
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
    this->_buf.append((char_type*)&this->IDXPOS, FBTSZ);


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

    this->rg.plies = decode::Game(eplies, this->rg.tags[fenstr]);

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
    this->IDXPOS = *(uint64_t*)(this->_buf.data() + TMESZ + 2*TAGSZ);


    this->sync();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::load_tag_enum(){

    this->seek_tags();
    this->sync();

    // Read tags (from current position to EOF)
    uint64_t loc = this->_fdev.tell();
    this->_fdev.seek(0, SEEK_END);
    uint64_t end = this->_fdev.tell();
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

    std::streamsize sz = this->NGAMES * IDXSZ;
    this->_fdev.xsgetn(this->_buf, sz);

    this->index.reserve(this->NGAMES);
    for(unsigned int i=0; i<this->NGAMES; ++i){
        uint64_t loc = i * IDXSZ;
        uint64_t pos = *(uint64_t*)(this->_buf.data() + loc);
        uint16_t npl = *(uint16_t*)(this->_buf.data() + loc + FBTSZ);
        index.emplace(i, std::make_pair(pos, npl));
    }
    this->sync();
}

/**************************************************
                  CHESSDBSTREAM
**************************************************/

template<typename CharT, typename Traits>
ChessDBStream<CharT, Traits>::ChessDBStream(const string& file)
    : ParseStream<CharT, Traits>(),
      _pbuf(file, std::filesystem::exists(file)) { this->rdbuf(&this->_pbuf); }

template<typename CharT, typename Traits>
ChessDBStream<CharT, Traits>::ChessDBStream(const CharT* file) : ChessDBStream(string(file)) {};

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
    this->read(nullptr, 0);
    return this->_pbuf.rg;
}

template<typename CharT, typename Traits>
int ChessDBStream<CharT, Traits>::nplies(int i){ return this->_pbuf.index[i].second; }

template<typename CharT, typename Traits>
int ChessDBStream<CharT, Traits>::size(){ return this->_pbuf.NGAMES; }


template class PGN<char>;
// template class PGN<wchar_t>;

template class PGNStream<char>;
// template class PGNStream<wchar_t>;

template class ChessDB<char>;
// template class ChessDB<wchar_t>;

template class ChessDBStream<char>;
// template class ChessDBStream<wchar_t>;