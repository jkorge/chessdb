#include "include/disamb.hpp"

int Disamb::cnt = 0;

Disamb::Disamb(log::LEVEL lvl, bool conlog) : logger(Disamb::cnt, lvl, conlog) { ++this->cnt; }

bool Disamb::moveable_pin(const U64& pros, const U64& dst, const U64& kloc, ChessBoard& board){
    // Pins can only move along the line connecting the king and the pinning piece
    return (board.linebt(pros, kloc, true) & board.linebt(dst, kloc, true)) & ~kloc;
}

void Disamb::ppins(U64& candidates, const U64& dst, const color& c, ChessBoard& board){
    // Seek pinned pawns
    U64 pinned = candidates & board.pins;
    if(pinned){
        // Remove from candidates and add back in only if `moveable_pin` returns `true`
        candidates &= ~pinned;
        U64 kloc = board.board(king, c);

        while(pinned){
            U64 pros = util::transform::mask(util::transform::bitscan(pinned));
            if(this->moveable_pin(pros, dst, kloc, board)){ candidates |= pros; }
            util::transform::lsbflip(pinned);
        }
    }
}

U64 Disamb::dpawn(const U64& src, const U64& dst, ptype pt, color c, ChessBoard& board, bool capture){
    U64 candidates = board.board(pt, c);
    ppins(candidates, dst, c, board);
    if(capture){ return candidates & board.attackfrom(dst, pt, !c) & src; }
    else{
        U64 b = c == white ? (dst >> 8) : (dst << 8);
        return (candidates & b) ? b : (c == white ? (dst >> 16) : (dst << 16));
    }
}

U64 Disamb::dpiece(const U64& src, const U64& dst, ptype pt, color c, ChessBoard& board, bool capture){
    U64 candidates = board.board(pt, c),
        kloc = board.board(king, c),
        grid = board.board();

    while(candidates){
        U64 pros = util::transform::mask(util::transform::bitscan(candidates));

        if(pros & board.pins){
            if(not this->moveable_pin(pros, dst, kloc, board)){
                util::transform::lsbflip(candidates);
                continue;
            }
        }
        if(
            (!src || pros & src) &&
            (dst & board.attackfrom(pros, pt, c)) &&
            (pt == knight || !(grid & board.linebt(pros, dst)))
        ){ return pros; }
        else{ util::transform::lsbflip(candidates); }
    }
    this->logger.debug("Failed to disambiguate");
    return 0;
}

U64 Disamb::pgn(const U64& src, const U64& dst, ptype pt, color c, ChessBoard& board, bool capture){
    this->logger.debug("Disambiguating", util::repr::color2s(c), util::repr::ptype2s(pt), "to", util::repr::coord2s(util::transform::rf(dst)));
    if(src){ this->logger.debug("Given src: ", util::repr::coord2s(src)); }
    switch(pt){
        case king: return board.board(pt,c);
        case pawn: return this->dpawn(src, dst, pt, c, board, capture);
        default:   return this->dpiece(src, dst, pt, c, board, capture);
    }
}