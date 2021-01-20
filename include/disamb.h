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

    inline bool moveable_pin(const U64& pros, const U64& dst, const U64& kloc, ChessBoard& board){
        // Pins can only move along the line connecting the king and the pinning piece
        return (board.linebt(pros, kloc, true) & board.linebt(dst, kloc, true)) & ~kloc;
    }

    void ppins(U64& candidates, const U64& dst, const color& c, ChessBoard& board){
        // Seek pinned pawns
        U64 pinned = candidates & board.pins;
        if(pinned){
            // Remove from candidates and add back in only if `moveable_pin` returns `true`
            candidates &= ~pinned;
            U64 kloc = board.board(king, c);

            while(pinned){
                U64 pros = mask(bitscan(pinned));
                if(moveable_pin(pros, dst, kloc, board)){ candidates |= pros; }
                util::lsbflip(pinned);
            }
        }
    }

    U64 dpawn(const U64& src, const U64& dst, ptype pt, color c, ChessBoard& board, bool capture){
        U64 candidates = board.board(pt, c);
        ppins(candidates, dst, c, board);
        if(capture){ return candidates & board.attackfrom(dst, pt, !c) & src; }
        else{
            U64 b = c == white ? (dst >> 8) : (dst << 8);
            return (candidates & b) ? b : (c == white ? (dst >> 16) : (dst << 16));
        }
    }

    U64 dpiece(const U64& src, const U64& dst, ptype pt, color c, ChessBoard& board, bool capture){
        U64 candidates = board.board(pt, c),
            kloc = board.board(king, c),
            grid = board.board();

        while(candidates){
            U64 pros = mask(bitscan(candidates));

            if(pros & board.pins){
                if(not moveable_pin(pros, dst, kloc, board)){
                    util::lsbflip(candidates);
                    continue;
                }
            }
            if(
                (!src || pros & src) &&
                (dst & board.attackfrom(pros, pt, c)) &&
                (pt == knight || !(grid & board.linebt(pros, dst)))
            ){ return pros; }
            else{ util::lsbflip(candidates); }
        }
        log.debug("Failed to disambiguate");
        return 0;
    }

    U64 pgn(const U64& src, const U64& dst, ptype pt, color c, ChessBoard& board, bool capture){
        log.debug("Disambiguating", util::color2s(c), util::ptype2s(pt), "to", util::coord2s(rf(dst)));
        if(src){ log.debug("Given src: ", util::coord2s(src)); }
        switch(pt){
            case king: return board.board(pt,c);
            case pawn: return dpawn(src, dst, pt, c, board, capture);
            default:   return dpiece(src, dst, pt, c, board, capture);
        }
    }
}

#endif