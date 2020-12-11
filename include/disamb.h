#ifndef DISAMB_H
#define DISAMB_H

// Independent project headers
#include "log.h"
#include "types.h"

// Nested dependencies (ie. util already depends on types)
#include "util.h"
#include "board.h"


namespace disamb{

    Logger log;

    U64 dpawn(const U64& src, const U64& dst, ptype pt, color c, ChessBoard &board, bool capture){
        U64 candidates = board.board(pt, c) & ~board.pins;
        if(capture){ return candidates & board.attackfrom(dst, pt, !c) & src; }
        else{
            U64 b = c == white ? (dst >> 8) : (dst << 8);
            return (candidates & b) ? b : (c == white ? (dst >> 16) : (dst << 16));
        }
    }

    U64 dpiece(const U64& src, const U64& dst, ptype pt, color c, ChessBoard &board, bool capture){
        U64 candidates = board.board(pt, c) & ~board.pins,
            grid = board.board();

        while(candidates){
            U64 pros = mask(bitscan(candidates));
            if(
                (!src || pros & src) &&
                (dst & board.attackfrom(pros, pt, c)) &&
                (pt == knight || !(grid & board.linebt(pros, dst)))
            ){ return pros; }
            else{ candidates &= candidates-1; }
        }
        return 0;
    }

    U64 pgn(const U64& src, const U64& dst, ptype pt, color c, ChessBoard &board, bool capture){

        // log.debug(
        //     "Disambiguating",
        //     util::color2s(c),
        //     util::ptype2s(pt),
        //     "to",
        //     util::coord2s(rf(dst))
        // );

        U64 res(0);

        if(pt == king){ res = board.board(pt, c); }
        else if(pt==pawn){ res = dpawn(src, dst, pt, c, board, capture); }
        else{ res = dpiece(src, dst, pt, c, board, capture); }

        return res;
    }
}

#endif