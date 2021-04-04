#ifndef BOARD_H
#define BOARD_H

#include <unordered_map>
#include "util.hpp"
#include "bitboard.hpp"

class ChessBoard : public BitBoard{

    // Default constructed ply => Ply placeholder for missing data
    ply missing;

public:

    // Bitboard of pinned pieces
    U64 pins = 0,
    // Bitboard of where an en passant capture can be made
        enpas = 0;

    // Which player moves next
    color next = white;

    // color of player IN check
    color check = NOCOLOR;

    // Map of bitboards (each with exactly one 1-bit identifying a checking piece's location) to ptypes
    std::unordered_map<U64, ptype> checkers;

    // Map square numbers to pnames & vice versa
    std::unordered_map<square, pname> map;
    std::unordered_map<pname, square> mat;

    // Castling availability - 0000KQkq
    BYTE cancas = 0b00001111;

    // Half and full move counters
    int half = 0, full = 1;

    ChessBoard();

    void newgame();

    void clear();

    pname update(const ply&);

    void update_state(const U64&, ptype, color, bool);

    void castle(int, color);

    void promote(color, const U64&, ptype);

    pname swap(const U64&, const U64&);

    void pinsearch();

    void pinch(color);

    void scan(ptype, color, const U64&, const U64&, const U64&);

    U64 legal() const;

    U64 legal(color, bool=false) const;

    U64 legal(ptype, color) const;

    template<typename T>
    U64 legal(const T&, ptype, color) const;

    U64 legalc(U64, ptype, color) const;

    std::vector<ply> legal_plies() const;

    std::vector<ply> legal_plies(color, bool=false) const;

    std::vector<ply> legal_plies(ptype, color) const;

    template<typename T>
    std::vector<ply> legal_plies(const T&, ptype, color) const;

    void get_checkers(color);

    std::string ply2san(const ply&) const;
};


#endif