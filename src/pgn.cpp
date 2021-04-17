#include "include/pgn.hpp"

template<typename CharT, typename Traits>
int PGN<CharT, Traits>::cnt = 0;

/******************************
    PGN Member Funcs
******************************/

template<typename CharT, typename Traits>
PGN<CharT, Traits>::PGN(const string& file, logging::LEVEL lvl, bool flog)
    : ParseBuf<CharT,Traits>(file, "rb"),
      logger(PGN::cnt, lvl, flog),
      disamb(lvl, flog),
      fen(lvl, flog) { ++this->cnt; }

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
    Util Functions
*/

// Remove all instances of a character from a string
template<typename CharT, typename Traits>
void PGN<CharT, Traits>::rchar(PGN<CharT, Traits>::char_type c, PGN<CharT, Traits>::string& s){
    s.erase(std::remove(s.begin(), s.end(), c), s.end());
}

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::fixendl(PGN<CharT, Traits>::string& buf){
    int_type idx;
    while((idx = buf.find('\n')) != string::npos){
        // All \n must be preceded by '.' or ' '
        if(buf[idx-1] != '.' && buf[idx-1] != ' '){ buf.insert(idx++, 1, ' '); }
        buf.erase(idx, 1);
    }
}

/*
    Parsing Functions
*/

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::tags(){
    this->logger.info("Parsing Tags");

    string tbuf = this->_buf.substr(0,this->tend);
    this->logger.debug(util::constants::endl + tbuf);
    this->_tags.reset();

    // strip brackets
    for(const char* c=util::pgn::tag_delims; c!=std::end(util::pgn::tag_delims); ++c){ this->rchar(*c, tbuf); }

    // move tbuf to sstream for tokenizing
    std::basic_stringstream<CharT, Traits> bufstr;
    bufstr.str(std::move(tbuf));

    while(std::getline(bufstr, tbuf)){

        int_type pos = tbuf.find(' ');
        std::transform(tbuf.begin(), tbuf.begin()+pos, tbuf.begin(), tolower);
        string keytok = tbuf.substr(0, pos);

        if(util::pgn::s2tag.find(keytok) != util::pgn::stend){
            this->_tags[util::pgn::s2tag[keytok]] = tbuf.substr(pos+2, this->tend-1);
        }
    }
    this->logger.info("End of Tags");
}

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::movetext(){
    this->logger.info("Parsing Movetext");

    string mbuf = this->_buf.substr(this->tend);
    this->logger.debug(util::constants::endl + mbuf);

    // Check for elided white ply at start of movetext
    bool black_starts = std::regex_search(mbuf.substr(0,6), util::pgn::black_starts);

    // Concatenate lines
    this->fixendl(mbuf);

    // Tokenize into individual plies
    this->tokenize(mbuf);

    // Parse ply tokens
    this->pplies(black_starts ? black : white);

    this->logger.info("End of Movetext.");
    this->logger.debug("Ply count:", this->_plies.size());
}

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::tokenize(PGN<CharT, Traits>::string& mbuf){

    this->_tokens.clear();
    std::stringstream sstr(mbuf);
    while(!sstr.eof()){
        sstr >> mbuf;
        if(isdigit(mbuf[0])){ mbuf.erase(0, mbuf.find('.')+1); }
        while((mbuf[0] == '.') | (mbuf[0] == '-')){ mbuf.erase(0,1); }
        this->_tokens.emplace_back(mbuf);
    }
}

template<typename CharT, typename Traits>
bool PGN<CharT, Traits>::isend(const PGN<CharT, Traits>::string& tok){
    if(
        (tok ==       "*") ||
        (tok ==     "1-0") ||
        (tok ==     "0-1") ||
        (tok == "1/2-1/2")
    )   { return true; }
    else{ return false; }
}

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::pplies(color c){

    this->board.newgame();
    this->_plies.clear();

    if(!this->_tags[fenstr].empty()){ fen.parse(this->_tags[fenstr], this->board); }
    else{ this->board.next = c; }

    for(typename std::vector<string>::iterator it=this->_tokens.begin(); it!=this->_tokens.end(); ++it){

        this->logger.debug("Parsing:", util::repr::color2c(c), *it);
        
        if(it->empty()){ this->_plies.emplace_back(); }
        else
        if(this->isend(*it)){ break; }
        else{
            ply p = this->pply(*it, c);
            p.name = board.update(p);
            this->_plies.emplace_back(p);
        }
        c = !c;
    }
}

template<typename CharT, typename Traits>
ply PGN<CharT, Traits>::pply(PGN<CharT, Traits>::string& p, color c){

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
ply PGN<CharT, Traits>::prest(const PGN<CharT, Traits>::string& p, color c, bool check, bool mate){
    int_type f = 0, r = 0;
    square srcsq = 0, dstsq = 0;
    bool capture = false;
    ptype pt = pawn, promo = pawn;

    for(typename string::const_reverse_iterator it=p.rbegin(); it!=p.rend(); ++it){
        char_type ch = *it;
        if(util::transform::isfile(ch)) { (!f++ ? dstsq : srcsq) += util::repr::c2file(ch);     }
        else
        if(std::isdigit(ch))            { (!r++ ? dstsq : srcsq) += util::repr::c2rank(ch) * 8; }
        else
        if(util::transform::ispiece(ch)){ pt = util::repr::c2ptype(ch); }
        else
        if(ch == 'x')                   { capture = true; }
        else
        if(ch == '=')                   { promo = pt; pt = pawn; }
        else
                                        { this->logger.warn("Unrecognized character in ply:", ch); }
    }

    U64 dst = util::transform::mask(dstsq),
        src = 0;
    if(r==2 && f==2) { src = util::transform::mask(srcsq);  }
    else{
        if(r==2)     { src = util::bitboard::rmasks[srcsq]; }
        else
        if(f==2)     { src = util::bitboard::fmasks[srcsq]; }

        src = this->disamb.pgn(src, dst, pt, c, this->board, capture);
        this->logger.debug("src:", util::repr::coord2s(src));
    }

    return {src, dst, pt, promo, c, 0, capture, check, mate};
}

/******************************
    PGNStream Member Funcs
******************************/

template<typename CharT, typename Traits>
PGNStream<CharT, Traits>::PGNStream(const std::basic_string<CharT>& file, logging::LEVEL lvl, bool flog)
    : ParseStream<CharT, Traits>(),
    _pbuf(file, lvl, flog) { this->rdbuf(&this->_pbuf); }

template<typename CharT, typename Traits>
PGNStream<CharT, Traits>::PGNStream(const CharT* file, logging::LEVEL lvl, bool flog)
    : PGNStream(std::string(file), lvl, flog) {}

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


template class PGN<char>;
// template class PGN<wchar_t>;

template class PGNStream<char>;
// template class PGNStream<wchar_t>;