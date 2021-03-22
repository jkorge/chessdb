#ifndef FEN_H
#define FEN_H

#include "log.hpp"
#include "util.hpp"
#include "board.hpp"

class Fen{

    static int cnt;
    log::Logger logger;
    
    bool is_avail(int, pname*);

    pname choose_pawn(color, pname*);

    pname choose(ptype, color, pname*);

    void arrange(std::string, ChessBoard&);

public:

    Fen(log::LEVEL=log::NONE, bool=false);
    
    color parse(std::string, ChessBoard&);

};

#endif