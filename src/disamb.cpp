#include "include/disamb.hpp"

int Disamb::cnt = 0;

Disamb::Disamb(logging::LEVEL lvl, bool flog) : logger(Disamb::cnt, lvl, flog) { ++this->cnt; }

bool Disamb::moveable_pin(const U64& pros, const U64& dst, const U64& kloc){
    // Pins can only move along the line connecting the king and the pinning piece
    return (util::bitboard::linebt(pros, kloc, true) & util::bitboard::linebt(dst, kloc, true)) & ~kloc;
}

void Disamb::ppins(U64& candidates, const U64& dst, const color& c, const ChessBoard& board){
    // Seek pinned pawns
    U64 pinned = candidates & board.pins;
    if(pinned){
        // Remove from candidates and add back in only if `moveable_pin` returns `true`
        candidates &= ~pinned;
        U64 kloc = board.board(king, c);

        while(pinned){
            U64 pros = util::transform::mask(util::transform::bitscan(pinned));
            if(this->moveable_pin(pros, dst, kloc)){ candidates |= pros; }
            util::transform::lsbflip(pinned);
        }
    }
}

U64 Disamb::dpawn(const U64& src, const U64& dst, ptype pt, color c, const ChessBoard& board, bool capture){
    U64 candidates = board.board(pt, c);
    ppins(candidates, dst, c, board);
    if(capture){ return candidates & util::bitboard::attackfrom(dst, pt, !c) & src; }
    else{
        U64 b = c == white ? (dst >> 8) : (dst << 8);
        return (candidates & b) ? b : (c == white ? (dst >> 16) : (dst << 16));
    }
}

U64 Disamb::dpiece(const U64& src, const U64& dst, ptype pt, color c, const ChessBoard& board, bool capture){
    U64 candidates = board.board(pt, c),
        kloc = board.board(king, c),
        grid = board.board();

    while(candidates){
        U64 pros = util::transform::mask(util::transform::bitscan(candidates));

        if(pros & board.pins){
            if(not this->moveable_pin(pros, dst, kloc)){
                util::transform::lsbflip(candidates);
                continue;
            }
        }
        if(
            (!src || pros & src) &&
            (dst & util::bitboard::attackfrom(pros, pt, c)) &&
            (pt == knight || !(grid & util::bitboard::linebt(pros, dst)))
        ){ return pros; }
        else{ util::transform::lsbflip(candidates); }
    }
    this->logger.debug("Failed to disambiguate");
    return 0;
}

U64 Disamb::pgn(const U64& src, const U64& dst, ptype pt, color c, const ChessBoard& board, bool capture){
    this->logger.debug("Disambiguating", util::repr::color2s(c), util::repr::ptype2s(pt), "to", util::repr::coord2s(dst));
    if(src){ this->logger.debug("Given src: ", util::repr::coord2s(util::transform::bitscan(src))); }
    switch(pt){
        case king: return board.board(pt,c);
        case pawn: return this->dpawn(src, dst, pt, c, board, capture);
        default:   return this->dpiece(src, dst, pt, c, board, capture);
    }
}