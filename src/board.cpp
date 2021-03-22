#include "include/board.hpp"

int ChessBoard::cnt = 0;

ChessBoard::ChessBoard(log::LEVEL lvl, bool conlog) : BitBoard(), logger(ChessBoard::cnt, lvl, conlog) {
    ++this->cnt;
    this->newgame();
}

void ChessBoard::newgame(){
    this->logger.debug("New game");

    this->pins = 0;

    this->white_pawns   = util::constants::start_coords.at(white).at(pawn);
    this->white_knights = util::constants::start_coords.at(white).at(knight);
    this->white_bishops = util::constants::start_coords.at(white).at(bishop);
    this->white_rooks   = util::constants::start_coords.at(white).at(rook);
    this->white_queens  = util::constants::start_coords.at(white).at(queen);
    this->white_king    = util::constants::start_coords.at(white).at(king);

    this->black_pawns   = util::constants::start_coords.at(black).at(pawn);
    this->black_knights = util::constants::start_coords.at(black).at(knight);
    this->black_bishops = util::constants::start_coords.at(black).at(bishop);
    this->black_rooks   = util::constants::start_coords.at(black).at(rook);
    this->black_queens  = util::constants::start_coords.at(black).at(queen);
    this->black_king    = util::constants::start_coords.at(black).at(king);

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
    int ssq = util::transform::sq(src),
        dsq = util::transform::sq(dst);

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
        U64 ploc = util::transform::mask(util::transform::bitscan(plocs));
        if(okloc & this->attackfrom(ploc, pt)){
            U64 lbt = this->linebt(ploc, okloc),
                pnc = lbt & oppt,
                pnb = lbt & same;
            if(lbt && !pnb && !(pnc & pnc-1)){ this->pins |= pnc; }
        }
        plocs &= plocs-1;
    }
}

void ChessBoard::log_board(){ if(this->logger.minl <= log::DEBUG){ this->logger.debug(this->display()); } }