#ifndef PGN_H
#define PGN_H

#include <regex>

#include "logging.hpp"
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

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;

    template<typename CharT_, typename Traits_>
    friend class PGNStream;

    const string _delim = {"\n\n"};
    std::vector<string> _tokens;

    logging::Logger logger;
    Disamb disamb;
    Fen fen;
    ChessBoard board;

protected:
    std::vector<ply> _plies;
    pgndict _tags;

private:
    int tend;
    static int cnt;

    // ParseBuf overrides
    void read();
    int_type rparse();

    // Util functions
    void rchar(char_type, string&);
    void fixendl(string&);

    // Parsing functions
    void tags();
    void movetext();

    void tokenize(string&);
    bool isend(const string&);
    void pplies(color=white);
    ply pply(string&, color);
    ply pcastle(bool, color, bool, bool);
    ply prest(const string&, color, bool, bool);

public:
    PGN(const string&, logging::LEVEL=logging::NONE, bool=false);
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

    PGNStream(const std::basic_string<CharT>&, logging::LEVEL=logging::NONE, bool=false);
    PGNStream(const CharT*, logging::LEVEL=logging::NONE, bool=false);

    PGNStream<CharT, Traits>& operator>>(game&);
    PGNStream<CharT, Traits>& operator>>(std::vector<game>& games);

    game next();
    void close();
};

#endif