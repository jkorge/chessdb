#ifndef ENCODE_H
#define ENCODE_H

#include "util.hpp"

class Encoder {

    ply missing;                // default construct for missing or elided plies
    eply emissing{UINT16_MAX};  // use `11111111 11111111` for missing plies

public:

    std::vector<eply> encode_game(const std::vector<ply>&);

    eply encode_ply(const ply&);

    BYTE encode_piece(const ply&);

    BYTE encode_action(const ply&);

    BYTE encode_mat(ptype, color);

    BYTE encode_capture(bool);

    BYTE encode_check(bool);

    BYTE encode_mate(bool);

    BYTE encode_pawn_promotion(ptype, bool=true);

    BYTE encode_queen_axis(const coords&, const coords&);

    BYTE pawn_action(const coords&, const coords&, bool, ptype);

    BYTE knight_action(const coords&, const coords&);

    BYTE bishop_action(const coords&, const coords&);

    BYTE rook_action(const coords&, const coords&);

    BYTE queen_action(const coords&, const coords&);

    BYTE king_action(const coords&, const coords&);
};


#endif