#ifndef DISAMB_H
#define DISAMB_H

#include "log.hpp"
#include "util.hpp"
#include "board.hpp"


class Disamb{

    static int cnt;
    log::Logger logger;

public:

    Disamb(log::LEVEL=log::NONE, bool=false);

    bool moveable_pin(const U64&, const U64&, const U64&);

    void ppins(U64& candidates, const U64&, const color&, const ChessBoard&);

    U64 dpawn(const U64&, const U64&, ptype, color, const ChessBoard&, bool);

    U64 dpiece(const U64&, const U64&, ptype, color, const ChessBoard&, bool);

    U64 pgn(const U64&, const U64&, ptype, color, const ChessBoard&, bool);
};

#endif