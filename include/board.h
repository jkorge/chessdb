#ifndef BOARD_H
#define BOARD_H

#include <map>

#include "log.h"
#include "types.h"

#include "util.h"
#include "bitboard.h"

class ChessBoard : public BitBoard{
public:

    U64 pins = 0;
    Logger log;

    // Map castles to rook locations
    std::map<int, std::map<color, U64> >rc{
        {
            1,
            {
                {white, mask(7)},
                {black, mask(63)}
            }
        },

        {
            -1,
            {
                {white, mask(0)},
                {black, mask(56)}
            }
        }
    };

    ChessBoard();

    void newgame();

    const ply& update(const ply&);

    void castle(int, color);

    void promote(color, const U64&, ptype);

    void pinsearch();

    void pinch(color);

    void log_board();

};

ChessBoard::ChessBoard() : BitBoard() { this->newgame(); }

void ChessBoard::newgame(){
    this->log.debug("New game");

    this->pins = 0;

    this->white_pawns   = util::start_coords[white][pawn];
    this->white_knights = util::start_coords[white][knight];
    this->white_bishops = util::start_coords[white][bishop];
    this->white_rooks   = util::start_coords[white][rook];
    this->white_queens  = util::start_coords[white][queen];
    this->white_king    = util::start_coords[white][king];

    this->black_pawns   = util::start_coords[black][pawn];
    this->black_knights = util::start_coords[black][knight];
    this->black_bishops = util::start_coords[black][bishop];
    this->black_rooks   = util::start_coords[black][rook];
    this->black_queens  = util::start_coords[black][queen];
    this->black_king    = util::start_coords[black][king];
}


/*********************************
    UPDATE BOARD FROM PLY
*********************************/

const ply& ChessBoard::update(const ply& p){
    
    if(p.capture){
        if(p.type != pawn || (this->board() & p.dst)){ this->remove(p.dst); }
        // Pawn capture && empty dst => En passant capture
        else{ this->remove(this->rshift(p.dst, !p.c), pawn, !p.c); }
    }

    this->move(p.src, p.dst, p.type, p.c);
    if(p.castle){ this->castle(p.castle, p.c); }
    if(p.promo){ this->promote(p.c, p.dst, p.promo); }
    this->pinsearch();

    // this->log_board();

    return p;
}

void ChessBoard::castle(int cas, color c){
    U64 src = this->rc[cas][c],
        dst = (cas > 0) ? (src >> 2) : (src << 3);
    this->move(src, dst, rook, c);
}

void ChessBoard::promote(color c, const U64& src, ptype promo){
    this->remove(src, pawn, c);
    this->place(src, promo, c);
}

/*********************************
    PINS
*********************************/

void ChessBoard::pinsearch(){
    this->pins = 0;
    this->pinch(white);
    this->pinch(black);
}

void ChessBoard::pinch(color c){
    U64 okloc = this->board(king, !c),
        plocs = this->board(rook, c) | this->board(bishop, c) | this->board(queen, c),
        same = this->board(c),
        oppt = this->board(!c);

    while(plocs){
        U64 ploc = mask(bitscan(plocs)),
            lbt = this->linebt(ploc, okloc),
            pc = lbt & oppt,
            up = lbt & same;

        /*
            A pin is found if the following are all true:
                1. There exists a line (lbt) between candidate and king
                2. There are no pieces with the same color as candidate on lbt
                3. There is only one piece with the same color as king on lbt
        */
        if(lbt && !up && !(pc & pc-1)){ this->pins |= pc; }
        plocs &= plocs-1;
    }
}

void ChessBoard::log_board(){ this->log.debug(this->display()); }


#endif