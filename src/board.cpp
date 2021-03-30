#include "include/board.hpp"

ChessBoard::ChessBoard() : BitBoard() { this->newgame(); }

void ChessBoard::newgame(){

    this->pins = 0;
    this->enpas = 0;
    this->check = NOCOLOR;
    this->next = white;
    this->checkers.clear();

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
    this->pins = 0;
    this->enpas = 0;
    this->check = NOCOLOR;
    this->next = NOCOLOR;
    this->checkers.clear();
}


/*********************************
    UPDATE BOARD FROM PLY
*********************************/

pname ChessBoard::update(const ply& p){

    if(p == this->missing){ return NONAME; }
    
    if(p.capture){
        if(p.type==pawn && (p.dst & this->enpas)){
            // En Passant captures
            this->remove(p.c > 0 ? (p.dst >> 8) : (p.dst << 8), pawn, !p.c);
            this->enpas = 0;
        }
        else{ this->remove(p.dst); }
    }
    else if(p.type == pawn && p.dst == (p.c > 0 ? p.src << 16 : p.src >> 16)){
        // Double-push pawn => En Passant capture possible
        this->enpas = p.c > 0 ? (p.dst >> 8) : (p.dst << 8);
    }
    else{ this->enpas = 0; }

    // Material movement
    pname name = this->swap(p.src, p.dst);
    this->move(p.src, p.dst, p.type, p.c);

    // Special cases
    if(p.castle)        { this->castle(p.castle, p.c); }
    if(p.promo)         { this->promote(p.c, p.dst, p.promo); }
    if(p.check)         { this->check = !p.c; this->get_checkers(p.c); }
    else if(this->check){ this->check = NOCOLOR; this->checkers.clear(); }

    // State update
    this->pinsearch();
    this->next = !p.c;

    return name;
}

void ChessBoard::castle(int cas, color c){
    U64 src = util::constants::rook_castle.at(cas).at(c),
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
        if(okloc & util::bitboard::attackfrom(ploc, pt)){
            U64 lbt = util::bitboard::linebt(ploc, okloc),
                pnc = lbt & oppt,
                pnb = lbt & same;
            if(lbt && !pnb && !(pnc & pnc-1)){ this->pins |= pnc; }
        }
        plocs &= plocs-1;
    }
}

// void ChessBoard::log_board(){ if(this->logger.minl <= log::DEBUG){ this->logger.debug(this->display()); } }

/*********************************
    LEGAL MOVES
*********************************/

U64 ChessBoard::legal() const{
    U64 res = 0;
    for(int i=white; i>=black; i-=2){ res |= this->legal(static_cast<color>(i)); }
    return res;
}

U64 ChessBoard::legal(color c) const{
    U64 res = 0;
    for(int j=pawn; j<king; ++j){ res |= this->legal(static_cast<ptype>(j), c); }
    return res;
}

U64 ChessBoard::legal(ptype pt, color c) const{
    U64 res = 0, bb = this->board(pt, c);
    while(bb){
        res |= this->legal(util::transform::bitscan(bb), pt, c);
        util::transform::lsbflip(bb);
    }
    return res;
}

template<typename T>
U64 ChessBoard::legal(const T& src, ptype pt, color c) const{
    U64 res = 0,
        msk = util::transform::mask(src),
        kloc = this->board(king, c),
        occ = this->board();
    bool isnext = this->next == c;
    switch(pt){
        case pawn: {
            U64 opp = this->board(!c),
                pst = c>0 ? util::constants::white_pawns : util::constants::black_pawns;
            // Single push
            res |= c>0 ? msk << 8 : msk >> 8;
            // Double push
            if(msk & pst){ res |= c>0 ? msk << 16 : msk >> 16; }
            // Captures (including en passant)
            res |= util::bitboard::attackfrom(msk, pt, c) & (isnext ? (opp | this->enpas) : opp);
            break;
        }
        case knight: {
            res |= util::bitboard::attackfrom(msk, pt);
            break;
        }
        case bishop:
        case rook:
        case queen: {
            for(int i=0; i<8; ++i){ res |= this->ray(msk, pt, static_cast<direction>(i)); }
            break;
        }
        default: {
            U64 atk = this->legal(!c);
            res |= util::bitboard::attackfrom(msk, pt)
                    & ~atk
                    & ~util::bitboard::attackfrom(this->board(pt, !c), pt);

            if(this->check != c && kloc == util::constants::start_coords.at(c).at(pt)){
                // Castles
                U64 ksl = util::bitboard::linebt(kloc, kloc >> 2, true) & ~kloc,
                    qsl = util::bitboard::linebt(kloc, kloc << 2, true) & ~kloc;
                if(ksl == (ksl & ~occ & ~atk)){ res |= kloc >> 2; }
                if(qsl == (qsl & ~occ & ~atk)){ res |= kloc << 2; }
            }

        }
    }
    // Discount any moves that break a pin
    if(msk & this->pins){ res &= util::bitboard::axisalign(util::bitboard::linebt(msk, kloc, true)); }
    // Modify results if in check
    if(this->check == c){ res = this->legalc(res, pt, c); }
    // Discount squares occupied by same colored pieces
    res &= ~this->board(c);
    return res;
}

U64 ChessBoard::legalc(U64 res, ptype pt, color c) const{
    // color c = color of player IN check
    U64 filter = 0,
        kloc = this->board(king, c);
    bool bloc = this->checkers.size() == 1;
    for(std::unordered_map<U64, ptype>::const_iterator it=this->checkers.begin(); it!=this->checkers.end(); ++it){
        U64 lbt = util::bitboard::linebt(it->first, kloc, true);
        if(pt == king){ if(it->second > knight){ filter |= util::bitboard::axisalign(lbt); } }
        else if(bloc) { filter |= lbt; }
    }

    res &= pt==king ? ~filter : filter;
    return res;
}

std::vector<ply> ChessBoard::legal_plies() const{
    std::vector<ply> plies;
    for(int i=white; i>=black; i-=2){
        std::vector<ply> _p = this->legal_plies(static_cast<color>(i));
        plies.insert(plies.end(), _p.begin(), _p.end());
    }
    return plies;
}

std::vector<ply> ChessBoard::legal_plies(color c) const{
    std::vector<ply> plies;
    for(int j=0; j<king; j++){
        std::vector<ply> _p = this->legal_plies(static_cast<ptype>(j), c);
        plies.insert(plies.end(), _p.begin(), _p.end());
    }
    return plies;
}

std::vector<ply> ChessBoard::legal_plies(ptype pt, color c) const{
    std::vector<ply> plies;
    U64 bb = this->board(pt, c);
    while(bb){
        std::vector<ply> _p = this->legal_plies(util::transform::bitscan(bb), pt, c);
        plies.insert(plies.end(), _p.begin(), _p.end());
        util::transform::lsbflip(bb);
    }
    return plies;
}

template<typename T>
std::vector<ply> ChessBoard::legal_plies(const T& src, ptype pt, color c) const{
    std::vector<ply> plies;
    U64 res = this->legal(src, pt, c),
        msk = util::transform::mask(src),
        okloc = this->board(king, !c),
        occ = this->board();
    while(res){
        square x = util::transform::bitscan(res);
        U64 dst = util::transform::mask(x);
        bool cap = occ & dst,
             mte = this->legal(!c) == 0,
             chk = okloc & util::bitboard::attackfrom(dst, pt, c) & (pt!=knight ? this->clearbt(dst, okloc) : util::constants::ALL_);
        int cas = 0;
        if(pt == king){
            if(dst == (msk << 2)){ cas = 1; }
            else if(dst == (msk >> 2)){ cas = -1; }
        }
        if(pt == pawn && x >= 56){ for(int i=knight; i<king; ++i){ plies.emplace_back(msk, dst, pt, static_cast<ptype>(i), c, cas, cap, chk, mte); } }
        else                     { plies.emplace_back(msk, dst, pt, pawn, c, cas, cap, chk, mte); }
        util::transform::lsbflip(res);
    }
    return plies;
}

void ChessBoard::get_checkers(color c){
    // color c = color of player EXECUTING check
    this->checkers.clear();
    U64 kloc = this->board(king, !c);
    for(int i=pawn; i<king; ++i){
        ptype pt = static_cast<ptype>(i);
        U64 bb = this->board(pt, c);
        while(bb){
            square x = util::transform::bitscan(bb);
            if(this->legal(x, pt, c) & kloc){ this->checkers[util::transform::mask(x)] = pt; }
            util::transform::lsbflip(bb);
        }
    }
}



template U64 ChessBoard::legal(const square&, ptype, color) const;
template U64 ChessBoard::legal(const U64&, ptype, color) const;
template U64 ChessBoard::legal(const coords&, ptype, color) const;

template std::vector<ply> ChessBoard::legal_plies(const square&, ptype, color) const;
template std::vector<ply> ChessBoard::legal_plies(const U64&, ptype, color) const;
template std::vector<ply> ChessBoard::legal_plies(const coords&, ptype, color) const;