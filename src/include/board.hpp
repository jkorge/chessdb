#ifndef BOARD_H
#define BOARD_H

#include <unordered_map>
#include "log.hpp"
#include "bitboard.hpp"

class ChessBoard : public BitBoard{
    static int cnt;
    ply missing;

public:

    log::Logger logger;

    // Bitboard of pinned pieces
    U64 pins = 0,
    // Bitboard of where an en passant capture can be made
        enpas = 0;

    // Which player moves next
    color next = white;

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

    U64 legal() const;

    U64 legal(color c) const;

    U64 legal(ptype pt, color c) const;

    U64 legal(square sq, ptype pt, color c) const;

};


#endif