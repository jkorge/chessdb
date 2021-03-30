#include "include/pgn.hpp"

template<typename CharT, typename Traits>
int PGN<CharT, Traits>::cnt = 0;

/******************************
    PGN Member Funcs
******************************/

/*
    ParseBuf Overrides
*/
template<typename CharT, typename Traits>
void PGN<CharT, Traits>::read(){
    // Tags
    this->_fdev.xsgetn(this->_buf, BUFSIZE, this->_delim);
    this->tend = this->_buf.size()-1;
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
    short idx;
    while((idx = s.find(c, 0)) != string::npos){ s.erase(idx, 1); }
}

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::fixendl(PGN<CharT, Traits>::string& buf){
    int_type idx;
    while((idx = buf.find('\n')) != string::npos){
        // All \n must be preceded by '.' or ' '
        if(badendl(buf.substr(idx-1,2))){ buf.insert(idx++, 1, ' '); }
        buf.erase(idx, 1);
    }
}

template<typename CharT, typename Traits>
inline bool PGN<CharT, Traits>::badendl(const PGN<CharT, Traits>::string& substr){ return std::regex_match(substr, util::pgn::nds); }

/*
    Parsing Functions
*/

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::tags(){
    this->logger.info("Parsing Tags");

    string tbuf = this->_buf.substr(0,this->tend);
    this->logger.debug(tbuf);
    this->_tags.reset();

    // strip brackets
    for(const char *c=util::pgn::tag_delims; c!=std::end(util::pgn::tag_delims); ++c){ this->rchar(*c, tbuf); }

    // Copy _buf to sstream for tokenizing
    std::basic_stringstream<CharT, Traits> bufstr(tbuf);
    while(std::getline(bufstr, tbuf)){

        int_type pos = tbuf.find(' ');
        string keytok = tbuf.substr(0, pos);
        util::transform::lowercase(keytok);

        if(util::pgn::s2tag.find(keytok) != util::pgn::stend){
            string valtok = tbuf.substr(pos+1);
            this->rchar(util::pgn::value_delim, valtok);
            this->_tags[util::pgn::s2tag[keytok]] = valtok;
        }
    }
    this->logger.info("End of Tags");
}

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::movetext(){
    this->logger.info("Parsing Movetext");

    string mbuf = this->_buf.substr(this->tend+1);
    this->logger.debug(mbuf);

    // Check start for elided white ply at start of movetext
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
typename PGN<CharT, Traits>::string
PGN<CharT, Traits>::ptok(PGN<CharT, Traits>::string& tok){
    if(isdigit(tok[0])){ tok.erase(0, tok.find('.')+1); }
    while((tok[0] == '.') | (tok[0] == '-')){ tok.erase(0,1); }
    return tok;
}

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::tokenize(PGN<CharT, Traits>::string& mbuf){

    this->_tokens.clear();
    std::stringstream sstr(mbuf);
    string tok;
    while(!sstr.eof()){
        sstr >> tok;
        this->_tokens.emplace_back(this->ptok(tok));
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

    if(!this->_tags[fenstr].empty()){ c = fen.parse(this->_tags[fenstr], this->board); }

    for(typename std::vector<string>::iterator it=this->_tokens.begin(); it!=this->_tokens.end(); ++it){

        this->logger.debug("Parsing:", util::repr::color2c(c), *it);
        
        if(it->empty()){ this->_plies.emplace_back(ply()); }
        else if(this->isend(*it)){ break; }
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
    bool check = (bool) this->cmp(p, '+');
    bool mate = (bool) this->cmp(p, '#');
    ptype promo = static_cast<ptype>(this->cmp(p, '=', 2));

    bool kingside{!p.compare("O-O")},
         queenside{!p.compare("O-O-O")};

    // Shortcut for castles
    if(kingside | queenside){ return this->pcastle(queenside, c, check, mate); }
    else                    { return this->prest(p, c, check, mate, promo); }
}

template<typename CharT, typename Traits>
ply PGN<CharT, Traits>::pcastle(bool qs, color c, bool check, bool mate){
    U64 src = this->board.board(king, c),
        dst = qs ? (src >> 2) : (src << 2);
    return {src, dst, king, pawn, c, qs ? -1 : 1, false, check, mate};
}

template<typename CharT, typename Traits>
ply PGN<CharT, Traits>::prest(const PGN<CharT, Traits>::string& p, color c, bool check, bool mate, ptype promo){
    int_type f = 0, r = 0;
    square srcsq = 0, dstsq = 0;
    bool capture = false;
    ptype pt = pawn;

    for(typename string::const_reverse_iterator it=p.rbegin(); it!=p.rend(); ++it){
        char_type ch = *it;
        if     (util::transform::ispiece(ch)){ pt = util::repr::c2ptype(ch); }
        else if(util::transform::isfile(ch)) { (!f++ ? dstsq : srcsq) += util::repr::c2file(ch); }
        else if(isdigit(ch))      { (!r++ ? dstsq : srcsq) += 8*util::repr::c2rank(ch); }
        else if(ch == 'x')        { capture = true; }
        else                      { this->logger.error("Unrecognized character in ply:", ch); }
    }

    U64 dst = util::transform::mask(dstsq),
        src = 0;
    if(r==2 && f==2) { src = util::transform::mask(srcsq); }
    else{
        if(r==2)     { src = util::bitboard::rmasks[srcsq]; }
        else if(f==2){ src = util::bitboard::fmasks[srcsq]; }
        src = disamb.pgn(src, dst, pt, c, this->board, capture);
        this->logger.debug("src is", util::repr::coord2s(src));
    }

    return {src, dst, pt, promo, c, 0, capture, check, mate};
}

template<typename CharT, typename Traits>
typename PGN<CharT, Traits>::int_type
PGN<CharT, Traits>::cmp(PGN<CharT, Traits>::string& p, const char ch, short l){
    short retval = 0, mod;
    if((mod = p.find(ch)) != string::npos){
        retval = (l==2) ? (int_type) util::repr::c2ptype(p[mod+1]) : 1;
        p.erase(mod, l);
    }
    return retval;
}

/******************************
    PGNStream Member Funcs
******************************/

template<typename CharT, typename Traits>
PGNStream<CharT, Traits>& PGNStream<CharT, Traits>::operator>>(game& g){
    this->read(nullptr, 0);
    g.tags = this->_pbuf._tags;
    g.plies = this->_pbuf._plies;
    return *this;
}

template<typename CharT, typename Traits>
void PGNStream<CharT, Traits>::close(){ this->_pbuf.close(); }


template class PGN<char>;
// template class PGN<wchar_t>;

template class PGNStream<char>;
// template class PGNStream<wchar_t>;