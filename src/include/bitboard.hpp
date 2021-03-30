#ifndef BITBOARD_H
#define BITBOARD_H

#include "util.hpp"

class BitBoard{
public:

    U64 white_pawns{0},    black_pawns{0},
        white_knights{0},  black_knights{0},
        white_bishops{0},  black_bishops{0},
        white_rooks{0},    black_rooks{0},
        white_queens{0},   black_queens{0},
        white_king{0},     black_king{0};

    U64 state{0};                                           // Current occupancy bitmap of game board
    bool changed{true};                                     // Flag indicating if board has changed since last read

    /*
        Retrieve occupancy bitmap
    */
    U64 board() const;

    U64 board();

    U64 board(color) const;

    U64 board(ptype) const;

    const U64& board(ptype, color) const;

    U64& board(ptype, color);

    /*
        Get piece info from square
    */

    template<typename T>
    color lookupc(const T&) const;

    template<typename T>
    ptype lookupt(const T&) const;

    /*
        Remove piece(s) from board
    */

    template<typename T>
    void remove(const T&, ptype, color);

    template<typename T>
    void remove(const T&);

    void remove();

    /*
        Place piece(s) on board
    */

    template<typename T>
    void place(const T&, ptype, color);

    /*
        Move piece from src to dst
    */

    template<typename Ts, typename Td>
    void move(const Ts&, const Td&, ptype, color);

    /*
        Bitmap of squares attacked by single ray of sliding piece attack pattern
    */
    template<typename T>
    U64 ray(const T&, ptype, direction) const;

    /*
        Determine if line connecting src and dst is unoccupied
    */
    template<typename Ts, typename Td>
    bool clearbt(const Ts&, const Td&) const;

    /*
        Visualizations
    */

    std::string display() const;

    std::string odisplay() const;

    std::string odisplay(color) const;

    std::string odisplay(ptype) const;

    std::string odisplay(ptype, color) const;
};

#endif