#ifndef CHESS_SET_H
#define CHESS_SET_H

#include "types.h"
#include "util.h"


typedef struct cset{
    const std::vector<pce> pieces{
        pce(white_pawn_a, util::start_coords.at(white_pawn_a)),
        pce(white_pawn_b, util::start_coords.at(white_pawn_b)),
        pce(white_pawn_c, util::start_coords.at(white_pawn_c)),
        pce(white_pawn_d, util::start_coords.at(white_pawn_d)),
        pce(white_pawn_e, util::start_coords.at(white_pawn_e)),
        pce(white_pawn_f, util::start_coords.at(white_pawn_f)),
        pce(white_pawn_g, util::start_coords.at(white_pawn_g)),
        pce(white_pawn_h, util::start_coords.at(white_pawn_h)),
        pce(white_knight_b, util::start_coords.at(white_knight_b)),
        pce(white_knight_g, util::start_coords.at(white_knight_g)),
        pce(white_bishop_c, util::start_coords.at(white_bishop_c)),
        pce(white_bishop_f, util::start_coords.at(white_bishop_f)),
        pce(white_rook_a, util::start_coords.at(white_rook_a)),
        pce(white_rook_h, util::start_coords.at(white_rook_h)),
        pce(white_queen_d, util::start_coords.at(white_queen_d)),
        pce(white_king_e, util::start_coords.at(white_king_e)),

        pce(black_pawn_a, util::start_coords.at(black_pawn_a)),
        pce(black_pawn_b, util::start_coords.at(black_pawn_b)),
        pce(black_pawn_c, util::start_coords.at(black_pawn_c)),
        pce(black_pawn_d, util::start_coords.at(black_pawn_d)),
        pce(black_pawn_e, util::start_coords.at(black_pawn_e)),
        pce(black_pawn_f, util::start_coords.at(black_pawn_f)),
        pce(black_pawn_g, util::start_coords.at(black_pawn_g)),
        pce(black_pawn_h, util::start_coords.at(black_pawn_h)),
        pce(black_knight_b, util::start_coords.at(black_knight_b)),
        pce(black_knight_g, util::start_coords.at(black_knight_g)),
        pce(black_bishop_c, util::start_coords.at(black_bishop_c)),
        pce(black_bishop_f, util::start_coords.at(black_bishop_f)),
        pce(black_rook_a, util::start_coords.at(black_rook_a)),
        pce(black_rook_h, util::start_coords.at(black_rook_h)),
        pce(black_queen_d, util::start_coords.at(black_queen_d)),
        pce(black_king_e, util::start_coords.at(black_king_e))
    }; // []operator adds values when key does not exist; use .at() to access from const map
} cset;


// Non-const, no start coords
// Good for custom game setups and FEN parsing
typedef struct xcset{
    std::vector<pce> pieces{
        pce(white_pawn_a),
        pce(white_pawn_b),
        pce(white_pawn_c),
        pce(white_pawn_d),
        pce(white_pawn_e),
        pce(white_pawn_f),
        pce(white_pawn_g),
        pce(white_pawn_h),
        pce(white_knight_b),
        pce(white_knight_g),
        pce(white_bishop_c),
        pce(white_bishop_f),
        pce(white_rook_a),
        pce(white_rook_h),
        pce(white_queen_d),
        pce(white_king_e),

        pce(black_pawn_a),
        pce(black_pawn_b),
        pce(black_pawn_c),
        pce(black_pawn_d),
        pce(black_pawn_e),
        pce(black_pawn_f),
        pce(black_pawn_g),
        pce(black_pawn_h),
        pce(black_knight_b),
        pce(black_knight_g),
        pce(black_bishop_c),
        pce(black_bishop_f),
        pce(black_rook_a),
        pce(black_rook_h),
        pce(black_queen_d),
        pce(black_king_e)
    };
} xcset;

#endif