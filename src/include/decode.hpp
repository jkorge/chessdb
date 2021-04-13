#ifndef DECODE_H
#define DECODE_H

#include "util.hpp"
#include "board.hpp"
#include "fen.hpp"

class Decoder{

    ChessBoard board;
    Fen fen;

    ply missing;                // default construct for missing or elided plies
    eply emissing{UINT16_MAX};  // use `11111111 11111111` for missing plies

public:
    std::vector<ply> decode_game(const std::vector<eply>&, const std::string& ="");

    ply decode_ply(eply, ChessBoard&);

    ptype decode_type(BYTE);

    color decode_color(pname);

    pname decode_name(BYTE);

    bool decode_capture(BYTE);

    bool decode_check(BYTE);

    bool decode_mate(BYTE);

    ptype decode_pawn_promotion(BYTE);

    ptype decode_queen_axis(BYTE);

    coords pawn_action(BYTE, color);

    coords knight_action(BYTE);

    coords bishop_action(BYTE);

    coords rook_action(BYTE);

    coords queen_action(BYTE, ptype);

    coords king_action(BYTE);
};

#endif