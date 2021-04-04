#include "include/bitboard.hpp"

/*
    Retrieve occupancy bitmap
*/
U64 BitBoard::board() const{
    return this->white_pawns | this->white_knights | this->white_bishops | this->white_rooks | this->white_queens | this->white_king |
           this->black_pawns | this->black_knights | this->black_bishops | this->black_rooks | this->black_queens | this->black_king;
}


U64 BitBoard::board(){
    if(this->changed){
        this->state = this->white_pawns | this->white_knights | this->white_bishops | this->white_rooks | this->white_queens | this->white_king |
                      this->black_pawns | this->black_knights | this->black_bishops | this->black_rooks | this->black_queens | this->black_king;
        this->changed = false;
    }
    return this->state;
}

U64 BitBoard::board(color c) const{
    switch(c){
        case white: return this->white_pawns | this->white_knights | this->white_bishops | this->white_rooks | this->white_queens | this->white_king;
        default:    return this->black_pawns | this->black_knights | this->black_bishops | this->black_rooks | this->black_queens | this->black_king;
    }
}

U64 BitBoard::board(ptype pt) const{
    switch(pt){
        case pawn:   return this->white_pawns   | this->black_pawns;
        case knight: return this->white_knights | this->black_knights;
        case bishop: return this->white_bishops | this->black_bishops;
        case rook:   return this->white_rooks   | this->black_rooks;
        case queen:  return this->white_queens  | this->black_queens;
        default:     return this->white_king    | this->black_king;
    }
}

const U64& BitBoard::board(ptype pt, color c) const{
    switch(c){
        case white:
            switch(pt){
                case pawn:   return this->white_pawns;
                case knight: return this->white_knights;
                case bishop: return this->white_bishops;
                case rook:   return this->white_rooks;
                case queen:  return this->white_queens;
                default:     return this->white_king;
            }

        default:
            switch(pt){
                case pawn:   return this->black_pawns;
                case knight: return this->black_knights;
                case bishop: return this->black_bishops;
                case rook:   return this->black_rooks;
                case queen:  return this->black_queens;
                default:     return this->black_king;
            }
    }
}

U64& BitBoard::board(ptype pt, color c){
    switch(c){
        case white:
            switch(pt){
                case pawn:   return this->white_pawns;
                case knight: return this->white_knights;
                case bishop: return this->white_bishops;
                case rook:   return this->white_rooks;
                case queen:  return this->white_queens;
                default:     return this->white_king;
            }

        default:
            switch(pt){
                case pawn:   return this->black_pawns;
                case knight: return this->black_knights;
                case bishop: return this->black_bishops;
                case rook:   return this->black_rooks;
                case queen:  return this->black_queens;
                default:     return this->black_king;
            }
    }
}

/*
    Get piece info from square
*/

template<typename T>
color BitBoard::lookupc(const T& src) const{
    U64 smsk = util::transform::mask(src);
    if     (this->board(white) & smsk){ return white; }
    else if(this->board(black) & smsk){ return black; }
    else                              { return NOCOLOR; }
}

template<typename T>
ptype BitBoard::lookupt(const T& src) const{
    U64 smsk = util::transform::mask(src);
    if     (this->board(pawn)   & smsk){ return pawn; }
    else if(this->board(knight) & smsk){ return knight; }
    else if(this->board(bishop) & smsk){ return bishop; }
    else if(this->board(rook)   & smsk){ return rook; }
    else if(this->board(queen)  & smsk){ return queen; }
    else if(this->board(king)   & smsk){ return king; }
    else                               { return NOTYPE; }
}

/*
    Remove piece(s) from board
*/

// Given color, piece type, and location
template<typename T>
void BitBoard::remove(const T& src, ptype pt, color c){
    this->board(pt, c) &= ~util::transform::mask(src);
    this->changed = true;
}

// Given location only (remove piece from all occupancy maps)
template<typename T>
void BitBoard::remove(const T& src){
    U64 smsk = util::transform::mask(src);

    this->white_pawns   &= ~smsk;
    this->white_knights &= ~smsk;
    this->white_bishops &= ~smsk;
    this->white_rooks   &= ~smsk;
    this->white_queens  &= ~smsk;
    this->white_king    &= ~smsk;

    this->black_pawns   &= ~smsk;
    this->black_knights &= ~smsk;
    this->black_bishops &= ~smsk;
    this->black_rooks   &= ~smsk;
    this->black_queens  &= ~smsk;
    this->black_king    &= ~smsk;

    this->changed = true;
}

// Remove all pieces from all occupancy maps
void BitBoard::remove(){
    this->white_pawns   = util::constants::ZERO_;
    this->white_knights = util::constants::ZERO_;
    this->white_bishops = util::constants::ZERO_;
    this->white_rooks   = util::constants::ZERO_;
    this->white_queens  = util::constants::ZERO_;
    this->white_king    = util::constants::ZERO_;

    this->black_pawns   = util::constants::ZERO_;
    this->black_knights = util::constants::ZERO_;
    this->black_bishops = util::constants::ZERO_;
    this->black_rooks   = util::constants::ZERO_;
    this->black_queens  = util::constants::ZERO_;
    this->black_king    = util::constants::ZERO_;

    this->changed = true;
}

/*
    Place piece(s) on board
*/

template<typename T>
void BitBoard::place(const T& dst, ptype pt, color c){
    this->board(pt, c) |= util::transform::mask(dst);
    this->changed = true;
}


/*
    Move piece from src to dst
*/

template<typename Ts, typename Td>
void BitBoard::move(const Ts& src, const Td& dst, ptype pt, color c){
    this->remove(src, pt, c);
    this->place(dst, pt, c);
}


/*
    Bitmap of squares attacked by single ray of sliding piece attack pattern
        DOES NOT ASSUME EMPTY BOARD
        Uses current occupancy
*/
template<typename T>
U64 BitBoard::ray(const T& src, ptype pt, direction dir) const{
    U64 r = util::bitboard::rays[pt-2][util::transform::sq(src)][dir];
    if(not r){ return r; }
    square x = dir>3 ? util::transform::bitscanr(r & this->board()) : util::transform::bitscan(r & this->board());
    if(x == -1 || x == 64){ return r; }
    U64 rx = util::bitboard::rays[pt-2][x][dir];
    return r ^ rx;
}

/*
    Bitmap of squares attacked by sliding piece
        DOES NOT ASSUME EMPTY BOARD
        Calls `this->ray` for each direction
*/
template<typename T>
U64 BitBoard::sliding_atk(const T& src, ptype pt) const{
    U64 res = 0;
    for(int i=0; i<8; ++i){ res |= this->ray(src, pt, static_cast<direction>(i)); }
    return res;
}

/*
    Determine if line connecting src and dst is unoccupied
*/
template<typename Ts, typename Td>
bool BitBoard::clearbt(const Ts& src, const Td& dst) const{
    U64 lbt = util::bitboard::linebt(src, dst),
        smsk = util::transform::mask(src),
        dmsk = util::transform::mask(dst);
    if(util::bitboard::attackfrom(smsk, king) & dmsk){ return true; }
    else{
        switch(lbt){
            case 0:  return false;
            default: return !(lbt & this->board());
        }
    }
}

/*
    Visualizations
*/

std::string BitBoard::display() const{
    
    std::string viz = util::bitboard::bviz;
    U64 bb = this->board();

    while(bb){
        square x = util::transform::bitscan(bb);
        int idx = util::bitboard::bvizidx(x);
        viz[idx+1] = util::repr::color2c(this->lookupc(x));
        viz[idx+2] = util::repr::ptype2c(this->lookupt(x));
        util::transform::lsbflip(bb);
    }

    return viz;
}

std::string BitBoard::odisplay() const{ return util::bitboard::odisplay(this->board()); }

std::string BitBoard::odisplay(color c) const{ return util::bitboard::odisplay(this->board(c)); }

std::string BitBoard::odisplay(ptype pt) const{ return util::bitboard::odisplay(this->board(pt)); }

std::string BitBoard::odisplay(ptype pt, color c) const{ return util::bitboard::odisplay(this->board(pt, c)); }


/*
    TEMPLATE INSTANCES
*/
template color BitBoard::lookupc(const square&) const;
template color BitBoard::lookupc(const U64&) const;
template color BitBoard::lookupc(const coords&) const;

template ptype BitBoard::lookupt(const square&) const;
template ptype BitBoard::lookupt(const U64&) const;
template ptype BitBoard::lookupt(const coords&) const;

template void BitBoard::remove(const square&, ptype, color);
template void BitBoard::remove(const U64&, ptype, color);
template void BitBoard::remove(const coords&, ptype, color);

template void BitBoard::remove(const square&);
template void BitBoard::remove(const U64&);
template void BitBoard::remove(const coords&);

template void BitBoard::place(const square&, ptype, color);
template void BitBoard::place(const U64&, ptype, color);
template void BitBoard::place(const coords&, ptype, color);

template void BitBoard::move(const square&, const square&, ptype, color);
template void BitBoard::move(const U64&, const U64&, ptype, color);
template void BitBoard::move(const coords&, const coords&, ptype, color);
template void BitBoard::move(const square&, const U64&, ptype, color);
template void BitBoard::move(const square&, const coords&, ptype, color);
template void BitBoard::move(const U64&, const coords&, ptype, color);
template void BitBoard::move(const U64&, const square&, ptype, color);
template void BitBoard::move(const coords&, const square&, ptype, color);
template void BitBoard::move(const coords&, const U64&, ptype, color);

template U64 BitBoard::ray(const square&, ptype, direction) const;
template U64 BitBoard::ray(const U64&, ptype, direction) const;
template U64 BitBoard::ray(const coords&, ptype, direction) const;

template bool BitBoard::clearbt(const square&, const square&) const;
template bool BitBoard::clearbt(const U64&, const U64&) const;
template bool BitBoard::clearbt(const coords&, const coords&) const;
template bool BitBoard::clearbt(const square&, const U64&) const;
template bool BitBoard::clearbt(const square&, const coords&) const;
template bool BitBoard::clearbt(const U64&, const coords&) const;
template bool BitBoard::clearbt(const U64&, const square&) const;
template bool BitBoard::clearbt(const coords&, const square&) const;
template bool BitBoard::clearbt(const coords&, const U64&) const;

template U64 BitBoard::sliding_atk(const U64& src, ptype pt) const;
template U64 BitBoard::sliding_atk(const coords& src, ptype pt) const;
template U64 BitBoard::sliding_atk(const square& src, ptype pt) const;