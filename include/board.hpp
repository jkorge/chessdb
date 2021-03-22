#ifndef BOARD_H
#define BOARD_H

#include <unordered_map>
#include "log.hpp"
#include "bitboard.hpp"

class ChessBoard : public BitBoard{
    static int cnt;
    ply missing;

public:

    U64 pins = 0;
    log::Logger logger;

    // Map square numbers to pnames & vice versa
    std::unordered_map<square, pname> map;
    std::unordered_map<pname, square> mat;

    // Map castle values to rook locations
    std::unordered_map<int, std::unordered_map<color, U64> >rc{
        {
            1,
            {
                {white, util::transform::mask(7)},
                {black, util::transform::mask(63)}
            }
        },

        {
            -1,
            {
                {white, util::transform::mask(0)},
                {black, util::transform::mask(56)}
            }
        }
    };

    ChessBoard(log::LEVEL=log::NONE, bool=false);

    void newgame();

    void clear();

    pname update(const ply&);

    void castle(int, color);

    void promote(color, const U64&, ptype);

    pname swap(const U64&, const U64&);

    void pinsearch();

    void pinch(color);

    void scan(ptype, color, const U64&, const U64&, const U64&);

    void log_board();

};


#endif