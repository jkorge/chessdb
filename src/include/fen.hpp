#ifndef FEN_H
#define FEN_H

#include "logging.hpp"
#include "util.hpp"
#include "board.hpp"

class Fen{

    static int cnt;
    logging::Logger logger;
    
    bool is_avail(int, pname*);

    pname choose_pawn(color, pname*);

    pname choose(ptype, color, pname*);

    void arrange(std::string, ChessBoard&);

public:

    Fen(logging::LEVEL=logging::NONE, bool=false);
    
    color parse(std::string, ChessBoard&);

};

#endif