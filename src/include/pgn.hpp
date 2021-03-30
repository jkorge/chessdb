#ifndef PGN_H
#define PGN_H

#include <regex>

#include "log.hpp"
#include "parsestream.hpp"
#include "util.hpp"
#include "board.hpp"
#include "disamb.hpp"
#include "fen.hpp"

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
    int tend;

    log::Logger logger;
    ChessBoard board;
    Disamb disamb;
    Fen fen;

    // ParseBuf overrides
    void read();
    int_type rparse();

    // Util functions
    void rchar(char_type, string&);
    void fixendl(string&);
    bool badendl(const string&);

    // Parsing functions
    void tags();
    void movetext();

    string ptok(string&);
    void tokenize(string&);
    bool isend(const string&);
    void pplies(color=white);
    ply pply(string&, color);
    ply pcastle(bool, color, bool, bool);
    ply prest(const string&, color, bool, bool, ptype);
    int_type cmp(string&, const char, short=1);

    template<typename CharT_, typename Traits_>
    friend class PGNStream;

public:

    pgndict _tags;
    std::vector<ply> _plies;
    std::vector<string> _tokens;

    PGN(const string& file, log::LEVEL lvl=log::NONE, bool conlog=false)
        : ParseBuf<CharT,Traits>(file, "r"),
          logger(PGN::cnt, lvl, conlog),
          disamb(lvl, conlog),
          fen(lvl, conlog) { ++this->cnt; }

};


/******************************
    PGNStream
******************************/

template<typename CharT, typename Traits=std::char_traits<CharT> >
class PGNStream : public ParseStream<CharT, Traits>{

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;

    // Underlying buffer
    PGN<CharT, Traits> _pbuf;

public:
    PGNStream(const std::basic_string<CharT>& file, log::LEVEL lvl=log::NONE, bool conlog=false)
                : ParseStream<CharT, Traits>(),
                  _pbuf(file, lvl, conlog) { this->rdbuf(&this->_pbuf); }
    PGNStream<CharT, Traits>& operator>>(game&);
    void close();
};

#endif