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

    U64 lines[64][64];                                      // Bitmaps of lines between all pairs of squares

    std::vector<U64> rmasks, fmasks, dmasks, amasks,
                     wpattack, bpattack, nattack,           // Bitmaps of pawn/knight attacks
                     rattack, battack, qattack,             //            rook/bishop/queen attacks
                     kattack;                               //            king attacks

    U64 state{0};                                           // Current occupancy bitmap of game board
    bool changed{true};                                     // Flag indicating if board has changed since last read

    /*
        Constructor
    */
    BitBoard();

    /*
        Retrieve occupancy bitmap
    */
    U64 board();

    U64 board(color);

    U64 board(ptype);

    U64& board(ptype, color);

    /*
        Get piece info from square
    */

    template<typename T>
    color lookupc(const T&);

    template<typename T>
    ptype lookupt(const T&);

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
        Determine if two points are colinear
    */

    template<typename Ts, typename Td>
    bool colinear(const Ts&, const Td&);

    /*
        Determine direction of line connecting two points
    */

    template<typename Ts, typename Td>
    ptype linetype(const Ts&, const Td&);

    /*
        Bitmap of rank/file/diagonal/anti-diagonal for given square number
    */

    U64 rmask(const square&);

    U64 fmask(const square&);

    U64 dmask(const square&);

    U64 amask(const square&);

    /*
        Bitmap of squares attacked by pt when positioned on sq
    */

    template<typename T>
    U64 attackfrom(const T&, ptype, color=white);

    /*
        Bitmap of line between src and dst
    */

    template<typename Ts, typename Td>
    U64 linebt(const Ts&, const Td&, bool=false);

    /*
        Determine if line connecting src and dst is unoccupied
    */
    template<typename Ts, typename Td>
    bool clearbt(const Ts&, const Td&, bool=false);

    /*
        Shift coordinates
    */

    template<typename T>
    U64 rshift(const T&, int);

    template<typename T>
    U64 fshift(const T&, int);

    template<typename T>
    U64 shift(const T&, int, int);

    /*
        Visualizations
    */

    std::string display();

    std::string odisplay(const U64&);

    std::string odisplay();

    std::string odisplay(color);

    std::string odisplay(ptype);

    std::string odisplay(ptype, color);
};

#endif