#ifndef PGN_H
#define PGN_H

#include <iostream>
#include <regex>
#include <vector>

#include "parsebuf.h"
#include "types.h"
#include "util.h"
#include "log.h"
#include "board.h"
#include "disamb.h"
#include "fen.h"

/******************************
    PGN
******************************/
template<typename CharT=char, typename Traits=std::char_traits<CharT> >
class PGN : virtual public ParseBuf<CharT, Traits>{

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;
    typedef std::regex_token_iterator<std::string::iterator> rtok;

    string _delim = {"\n\n"};
    Logger log{__FILE__};

    void logply(ply);

    void read();
    void rcharb(char_type);
    void rchar(char_type, string&);
    void clear_tags();
    int_type parse();
    int_type tags();
    int_type movetext();
    std::vector<ply> pplies(std::vector<string>&, bool=false);
    ply pply(string&, ChessBoard&, bool);
    int_type cmp(string&, const char*, short=1);

public:

    pgndict _tags;
    std::vector<ply> _plies;

    PGN(string file) : ParseBuf<CharT,Traits>(file) {}

};

/******************************
    PGN Member Funcs
******************************/

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::logply(ply p){
    int src[] = {p.piece.rank, p.piece.file};
    log.debug("name:", util::pname2s[p.piece.name]);
    log.debug("src:", util::coord2s(src));
    log.debug("dst:", util::coord2s(p.dst));
    log.debug("promo:", util::ptype2s[p.promo]);
    log.debug("castle:", p.castle);
    log.debug("capture:", p.capture);
    log.debug("check:", p.check);
    log.debug("mate:", p.mate);
}

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::read(){ this->_fdev.xsgetn(this->_buf, BUFSIZE, this->_delim); }

// Remove all instances of a character from a string
template<typename CharT, typename Traits>
void PGN<CharT, Traits>::rchar(PGN<CharT, Traits>::char_type c, PGN<CharT, Traits>::string& s){
    short idx;
    while((idx = s.find(c, 0)) != string::npos){ s.erase(idx, 1); }
}

// Remove all instances of a character from _buf
template<typename CharT, typename Traits>
void PGN<CharT, Traits>::rcharb(PGN<CharT, Traits>::char_type c)
{ this->rchar(c, this->_buf); }

template<typename CharT, typename Traits>
typename PGN<CharT, Traits>::int_type PGN<CharT, Traits>::parse(){
    int_type size = (this->_buf[0] == '[') ? this->tags() : this->movetext();
    return this->_fdev.eof ? Traits::eof() : size;
}

template<typename CharT, typename Traits>
typename PGN<CharT, Traits>::int_type PGN<CharT, Traits>::tags(){
    
    std::basic_stringstream<CharT, Traits> tmp;
    string keytok, valtok;
    int_type pos, size;
    std::map<std::string, pgntag>::iterator end = util::pgn::s2tag.end();

    log.info("Parsing Tags");
    log.debug(this->_buf);
    size = this->_buf.size();

    this->clear_tags();

    // strip brackets
    for(const char *c=util::pgn::tag_delims; c!=std::end(util::pgn::tag_delims); c++){ this->rcharb(*c); }

    // Copy _buf to sstream for tokenizing
    tmp.str(this->_buf);
    while(std::getline(tmp, this->_buf)){

        pos = this->_buf.find(' ');
        keytok = this->_buf.substr(0, pos);
        util::lowercase(keytok);

        if(util::pgn::s2tag.find(keytok) != end){
            valtok = this->_buf.substr(pos+1);
            this->rchar(util::pgn::value_delim, valtok);
            this->_tags[util::pgn::s2tag[keytok]] = valtok;
        }
    }
    this->log.info("End of Tags");
    return size;
}

template<typename CharT, typename Traits>
void PGN<CharT, Traits>::clear_tags(){
    for(pgndict::iterator it=this->_tags.begin(); it!=this->_tags.end(); ++it){
        this->_tags[it->first].clear();
    }
}

template<typename CharT, typename Traits>
typename PGN<CharT, Traits>::int_type PGN<CharT, Traits>::movetext(){
    
    int_type idx, size;
    rtok end;
    string turn, p;
    std::vector<string> ply_s;
    bool start_on_black;

    this->log.info("Parsing Movetext");
    this->log.debug(this->_buf);
    size = this->_buf.size();

    /*
        TOKENIZE
    */

    // Concatenate lines (all \n must be preceded by '.' or ' ')
    while((idx = this->_buf.find('\n')) != string::npos){
        if((this->_buf[idx-1] != ' ') && (this->_buf[idx-1] != '.')){ this->_buf.insert(idx++, 1, ' '); }
        this->_buf.erase(idx, 1);
    }

    // Split on "\d{1,}\." (ie. turn numbers)
    rtok g(this->_buf.begin(), this->_buf.end(), util::pgn::move_num_r, -1);
    while(g != end){
        turn = *g++;
        if(turn.empty()){ continue; }

        // Add space if continuation (.. or --) is found at beginning of string
        if(std::regex_match(turn.substr(0,2), util::pgn::missing_ply)){ turn.insert(turn.begin()+2, ' '); }

        // Split again on \s (ie. ply separator)
        rtok t(turn.begin(), turn.end(), util::ws, -1);
        while(t != end){
            p = *t;
            if(!p.empty()){ ply_s.push_back(p); }
            t++;
        }
    }

    // Remove elided ply (".." or "--")
    if(start_on_black = std::regex_match(ply_s[0], util::pgn::missing_ply)){ ply_s.erase(ply_s.begin()); }

    /*
        PARSE PLIES
    */
    this->_plies = this->pplies(ply_s, start_on_black);

    return size;
}

template<typename CharT, typename Traits>
std::vector<ply>
PGN<CharT, Traits>::pplies(std::vector<PGN<CharT, Traits>::string>& ply_s, bool start_on_black){

    ChessBoard board;
    if(not this->_tags[fenstr].empty()){ start_on_black = fen::parse(this->_tags[fenstr], board); }

    std::vector<ply> res;
    bool bply = start_on_black ? true : false;

    for(typename std::vector<string>::iterator it=ply_s.begin(); it!=ply_s.end(); ++it){

        this->log.debug("Parsing:", (bply ? "b" : "w"), *it);

        if(std::regex_match(*it, util::pgn::game_result)){ break; }

        res.push_back(this->pply(*it, board, bply));
        bply = !bply;
    }

    this->log.info("End of Movetext.", res.size(), "plies");
    return res;
}

template<typename CharT, typename Traits>
ply PGN<CharT, Traits>::pply(PGN<CharT, Traits>::string& p, ChessBoard& board, bool bply){

    // Coord tracking
    int_type src[2]{-1, -1},
             dst[2]{-1, -1},
             *ranks[2]{&dst[0], &src[0]},
             *files[2]{&dst[1], &src[1]},
    // Castle tracking
             castle = 0;

    // Move flags
    bool check{false}, mate{false}, capture{false};

    // Piece info
    ptype piece_type{pawn}, promo{pawn};
    pname idx = NONAME;

    // Parse and remove flags
    check = (bool) this->cmp(p, "+");
    check = (bool) this->cmp(p, "#");
    promo = static_cast<ptype>(this->cmp(p, "=", 2));

    // Castle
    bool kingside{!p.compare("O-O")}, queenside{!p.compare("O-O-O")};
    if(kingside | queenside){

        this->log.debug(kingside ? "Kingside" : "Queenside", "castle");

        castle = kingside + (-1 * queenside);
        idx = disamb::K(bply);

        dst[0] = src[0] = board[idx].first;
        src[1] = board[idx].second;
        dst[1] = src[1] + (queenside ? -2 : 2);
    }
    else{
        int_type f = 0, r = 0;
        char_type c;

        for(typename string::reverse_iterator it=p.rbegin(); it!=p.rend(); ++it){
            c = *it;
            if(util::ispiece(c)){ piece_type = util::c2ptype[c]; }
            else if(util::isfile(c)){ *files[f++] = util::c2file[c]; }
            else if(isdigit(c)){ *ranks[r++] = (c - '0') - 1; }
            else if(c == 'x'){ capture = true; }
            else{ this->log.error("Unrecognized character in ply:", c); }
        }

        this->log.debug("piece_type:", util::ptype2s[piece_type]);
        this->log.debug("src:", util::coord2s(src));
        this->log.debug("dst:", util::coord2s(dst));

        idx = (src[0]==-1 || src[1]==-1)
              ? disamb::pgnply(piece_type, src, dst, board, bply, capture)
              : board.board[src[0]][src[1]]->name;

        this->log.debug(util::pname2s[idx]);
    }

    if(idx == NONAME){ this->log.error("Failed to disambiguate:", p); }

    this->log.debug("Constructing ply object");
    ply res{pce{idx, src}, dst, castle, capture, check, mate, promo};

    this->log.debug("Updating board");
    board.update(res);
    board.log_board();

    this->log.debug("Returning ply");
    return res;
}

template<typename CharT, typename Traits>
typename PGN<CharT, Traits>::int_type
PGN<CharT, Traits>::cmp(PGN<CharT, Traits>::string& p, const char *c, short l){
    size_t mod;
    short retval;
    if((mod = p.find(c)) != string::npos){
        retval = (l==2) ? (int_type) util::c2ptype[p[mod+1]] : 1;
        p.erase(mod, l);
    }
    else{ retval = 0; }
    return retval;
}


/******************************
    PGNStream
******************************/

template<typename CharT=char, typename Traits=std::char_traits<CharT> >
class PGNStream : public std::basic_istream<CharT, Traits>{

    // Underlying buffer
    PGN<CharT, Traits> _pbuf;

public:

    PGNStream(std::basic_string<CharT>& file) : _pbuf(file), std::basic_istream<CharT, Traits>(&_pbuf) {}
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