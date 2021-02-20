#ifndef BOARD_H
#define BOARD_H

#include <unordered_map>
#include "log.h"
#include "bitboard.h"

class ChessBoard : public BitBoard{
    static int cnt;

    ply missing;

public:

    U64 pins = 0;
    Logger log{cnt};

    // Map square numbers to pnames & vice versa
    std::unordered_map<square, pname> map;
    std::unordered_map<pname, square> mat;

    // Map castle values to rook locations
    std::unordered_map<int, std::unordered_map<color, U64> >rc{
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

    void clear();

    pname update(const ply&);

    void castle(int, color);

    void promote(color, const U64&, ptype);

    pname swap(const U64&, const U64&);

    void pinsearch();

    void pinch(color);

    void scan(ptype, color, const U64&, const U64&, const U64&);

    void log_board();

};

int ChessBoard::cnt = 0;

ChessBoard::ChessBoard() : BitBoard() {
    this->cnt++;
    this->newgame();
}

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

    for(int i= 0; i<16; ++i){ this->map[i]    = static_cast<pname>(i); }
    for(int i=16; i<32; ++i){ this->map[i+32] = static_cast<pname>(i); }
    for(int i=16; i<48; ++i){ this->map[i]    = NONAME; }

    for(int i= 0; i<16; ++i){ this->mat[static_cast<pname>(i)] = i; }
    for(int i=16; i<32; ++i){ this->mat[static_cast<pname>(i)] = i+32; }
}

void ChessBoard::clear(){
    this->remove();
    for(int i=0; i<64; ++i){ this->map[i] = NONAME; }
    for(int i=0; i<32; ++i){ this->mat[static_cast<pname>(i)] = -1; }
}


/*********************************
    UPDATE BOARD FROM PLY
*********************************/

pname ChessBoard::update(const ply& p){

    if(p == this->missing){ return NONAME; }
    
    if(p.capture){
        if(p.type != pawn || (this->board() & p.dst)){ this->remove(p.dst); }
        // Pawn capture && empty dst => En passant capture
        else{ this->remove(this->rshift(p.dst, !p.c), pawn, !p.c); }
    }

    // Material movement
    pname name = this->swap(p.src, p.dst);
    this->move(p.src, p.dst, p.type, p.c);

    // Special cases
    if(p.castle){ this->castle(p.castle, p.c); }
    if(p.promo){ this->promote(p.c, p.dst, p.promo); }

    // State update
    this->pinsearch();
    this->log_board();

    return name;
}

void ChessBoard::castle(int cas, color c){
    U64 src = this->rc[cas][c],
        dst = (cas > 0) ? (src >> 2) : (src << 3);
    this->move(src, dst, rook, c);
    this->swap(src, dst);
}

void ChessBoard::promote(color c, const U64& src, ptype promo){
    this->remove(src, pawn, c);
    this->place(src, promo, c);
}

pname ChessBoard::swap(const U64& src, const U64& dst){
    int ssq = sq(src),
        dsq = sq(dst);

    // Remove from dsq
    if(this->map[dsq] != NONAME){ this->mat[this->map[dsq]] = -1; }
    // Place on dsq
    this->map[dsq] = this->map[ssq];
    this->mat[this->map[dsq]] = dsq;
    // Remove from ssq
    this->map[ssq] = NONAME;
    return this->map[dsq];
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
        same = this->board(c),
        oppt = this->board(!c);

    this->scan(bishop, c, okloc, oppt, same);
    this->scan(rook, c, okloc, oppt, same);
    this->scan(queen, c, okloc, oppt, same);
}

void ChessBoard::scan(ptype pt, color c, const U64& okloc, const U64& oppt, const U64& same){
    /*
        A pin is found if the following are all true:
            1. There exists an attack line (lbt) between candidate and king
            2. There are no pieces with the same color as candidate on lbt
            3. There is only one piece with the same color as king on lbt
    */

    U64 plocs = this->board(pt, c);
    while(plocs){
        U64 ploc = mask(bitscan(plocs));
        if(okloc & this->attackfrom(ploc, pt)){
            U64 lbt = this->linebt(ploc, okloc),
                pnc = lbt & oppt,
                pnb = lbt & same;
            if(lbt && !pnb && !(pnc & pnc-1)){ this->pins |= pnc; }
        }
        plocs &= plocs-1;
    }
}

void ChessBoard::log_board(){ if(this->log.minl <= DEBUG){ this->log.debug(this->display()); } }


#endif