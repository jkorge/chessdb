#ifndef PGN_H
#define PGN_H

#include <regex>
#include <vector>

#include "log.h"
#include "types.h"
#include "util.h"
#include "parsebuf.h"

#include "board.h"
#include "disamb.h"
#include "fen.h"

/******************************
    PGN
******************************/
template<typename CharT, typename Traits=std::char_traits<CharT> >
class PGN : virtual public ParseBuf<CharT, Traits>{

    static int cnt;

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;

    string _delim = {"\n\n"};
    Logger log{cnt};
    ChessBoard board = ChessBoard();

    // ParseBuf overrides
    void read();
    int_type parse();

    // Util functions
    void rcharb(char_type);
    void rchar(char_type, string&);
    void clear_tags();
    void fixendl(string&);
    bool badendl(const string&);

    // Parsing functions
    int_type tags();
    int_type movetext();
    string ptok(string&);
    void tokenize();
    bool isend(const string&);
    void pplies(color=white);
    ply pply(string&, color);
    ply pcastle(bool, color, bool, bool);
    ply prest(const string&, color, bool, bool, ptype);
    int_type cmp(string&, const char, short=1);

public:

    pgndict _tags;
    std::vector<ply> _plies;
    std::vector<string> _tokens;

    PGN(const string& file) : ParseBuf<CharT,Traits>(file) { this->cnt++; }

};

template<typename CharT, typename Traits>
int PGN<CharT, Traits>::cnt = 0;

/******************************
    PGN Member Funcs
******************************/

/*
    ParseBuf Overrides
*/
template<typename CharT, typename Traits>
void PGN<CharT, Traits>::read(){ this->_fdev.xsgetn(this->_buf, BUFSIZE, this->_delim); }

template<typename CharT, typename Traits>
typename PGN<CharT, Traits>::int_type PGN<CharT, Traits>::parse(){
    int_type size = (this->_buf[0] == '[') ? this->tags() : this->movetext();
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

// Remove all instances of a character from _buf
template<typename CharT, typename Traits>
void PGN<CharT, Traits>::rcharb(PGN<CharT, Traits>::char_type c){ this->rchar(c, this->_buf); }

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::clear_tags(){
    for(pgndict::iterator it=this->_tags.begin(); it!=this->_tags.end(); ++it){
        this->_tags[it->first].clear();
    }
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
typename PGN<CharT, Traits>::int_type PGN<CharT, Traits>::tags(){
    this->log.info("Parsing Tags");
    this->log.debug(this->_buf);

    int_type size = this->_buf.size();

    this->clear_tags();

    // strip brackets
    for(const char *c=util::pgn::tag_delims; c!=std::end(util::pgn::tag_delims); c++){ this->rcharb(*c); }

    // Copy _buf to sstream for tokenizing
    std::basic_stringstream<CharT, Traits> bufstr(this->_buf);
    while(std::getline(bufstr, this->_buf)){

        int_type pos = this->_buf.find(' ');
        string keytok = this->_buf.substr(0, pos);
        util::lowercase(keytok);

        if(util::pgn::s2tag.find(keytok) != util::pgn::s2tag.end()){
            string valtok = this->_buf.substr(pos+1);
            this->rchar(util::pgn::value_delim, valtok);
            this->_tags[util::pgn::s2tag[keytok]] = valtok;
        }
    }
    this->log.info("End of Tags");
    return size;
}

template<typename CharT, typename Traits>
typename PGN<CharT, Traits>::int_type
PGN<CharT, Traits>::movetext(){

    this->log.info("Parsing Movetext");
    this->log.debug(this->_buf);

    // Return value
    int_type size = this->_buf.size();

    // Check start for elided white ply at start of movetext
    bool black_starts = std::regex_search(this->_buf.substr(0,6), util::pgn::black_starts);

    // Concatenate lines
    this->fixendl(this->_buf);

    // Tokenize into individual plies
    this->tokenize();

    // Parse ply tokens
    this->pplies(black_starts ? black : white);

    this->log.info("End of Movetext.");
    this->log.debug("Ply count:", this->_plies.size());

    return size;
}

template<typename CharT, typename Traits>
typename PGN<CharT, Traits>::string
PGN<CharT, Traits>::ptok(PGN<CharT, Traits>::string& tok){
    if(isdigit(tok[0])){ tok.erase(0, tok.find('.')+1); }
    while((tok[0] == '.') | (tok[0] == '-')){ tok.erase(0,1); }
    return tok;
}

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::tokenize(){

    this->_tokens.clear();
    std::stringstream str(this->_buf);
    string tok;
    while(!str.eof()){
        str >> tok;
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

    if(!this->_tags[fenstr].empty()){ c = fen::parse(this->_tags[fenstr], this->board); }

    for(typename std::vector<string>::iterator it=this->_tokens.begin(); it!=this->_tokens.end(); ++it){

        this->log.debug("Parsing:", util::color2c(c), *it);

        if(it->empty()){ c = !c; continue; }
        else if(this->isend(*it)){ break; }
        
        ply p = this->pply(*it, c);
        p.name = board.update(p);
        this->_plies.emplace_back(p);
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
        if(util::ispiece(ch)){ pt = util::c2ptype(ch); }
        else if(util::isfile(ch)){ (!f++ ? dstsq : srcsq) += util::c2file(ch); }
        else if(isdigit(ch)){ (!r++ ? dstsq : srcsq) += 8*util::c2rank(ch); }
        else if(ch == 'x'){ capture = true; }
        else{ this->log.error("Unrecognized character in ply:", ch); }
    }

    U64 dst = mask(dstsq),
        src = 0;
    if(r==2 && f==2) { src = mask(srcsq); }
    else{
        if(r==2)     { src = board.rmasks[srcsq]; }
        else if(f==2){ src = board.fmasks[srcsq]; }
        src = disamb::pgn(src, dst, pt, c, this->board, capture);
        // this->log.debug("Disambiguated src:",src);
    }

    return {src, dst, pt, promo, c, 0, capture, check, mate};
}

template<typename CharT, typename Traits>
typename PGN<CharT, Traits>::int_type
PGN<CharT, Traits>::cmp(PGN<CharT, Traits>::string& p, const char ch, short l){
    short retval = 0, mod;
    if((mod = p.find(ch)) != string::npos){
        retval = (l==2) ? (int_type) util::c2ptype(p[mod+1]) : 1;
        p.erase(mod, l);
    }
    return retval;
}







/******************************
    PGNStream
******************************/

template<typename CharT, typename Traits=std::char_traits<CharT> >
class PGNStream : public std::basic_istream<CharT, Traits>{

    // Underlying buffer
    PGN<CharT, Traits> _pbuf;

public:

    PGNStream(const std::basic_string<CharT>& file) : _pbuf(file), std::basic_istream<CharT, Traits>(&_pbuf) {}
    pgndict tags();
    std::vector<ply> moves();
};

/******************************
    PGNStream Member Funcs
******************************/

template<typename CharT, typename Traits>
pgndict PGNStream<CharT, Traits>::tags(){
    this->get();
    return this->_pbuf._tags;
}

template<typename CharT, typename Traits>
std::vector<ply> PGNStream<CharT, Traits>::moves(){
    this->get();
    return this->_pbuf._plies;
}


#endif