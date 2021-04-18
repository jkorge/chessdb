#include "include/bitboard.hpp"

/*
    Retrieve occupancy bitmap
*/
U64 BitBoard::board() const{
    if(this->changed){
        return this->boards[0] | this->boards[1] | this->boards[2] | this->boards[3] | this->boards[ 4] | this->boards[ 5] |
               this->boards[6] | this->boards[7] | this->boards[8] | this->boards[9] | this->boards[10] | this->boards[11];
    }
    else{ return this->boards[12]; }
}

U64 BitBoard::board(){
    if(this->changed){
        this->boards[12] = this->boards[0] | this->boards[1] | this->boards[2] | this->boards[3] | this->boards[ 4] | this->boards[ 5] |
                           this->boards[6] | this->boards[7] | this->boards[8] | this->boards[9] | this->boards[10] | this->boards[11];
        this->changed = false;
    }
    return this->boards[12];
}

U64 BitBoard::board(color c) const{
    switch(c){
        case white: return this->boards[0] | this->boards[1] | this->boards[2] | this->boards[3] | this->boards[ 4] | this->boards[ 5];
        default:    return this->boards[6] | this->boards[7] | this->boards[8] | this->boards[9] | this->boards[10] | this->boards[11];
    }
}

U64 BitBoard::board(ptype pt) const{
    switch(pt){
        case pawn:   return this->boards[0] | this->boards[ 6];
        case knight: return this->boards[1] | this->boards[ 7];
        case bishop: return this->boards[2] | this->boards[ 8];
        case rook:   return this->boards[3] | this->boards[ 9];
        case queen:  return this->boards[4] | this->boards[10];
        default:     return this->boards[5] | this->boards[11];
    }
}

const U64& BitBoard::board(ptype pt, color c) const{
    switch(c){
        case white:
            switch(pt){
                case pawn:   return this->boards[ 0];
                case knight: return this->boards[ 1];
                case bishop: return this->boards[ 2];
                case rook:   return this->boards[ 3];
                case queen:  return this->boards[ 4];
                default:     return this->boards[ 5];
            }

        default:
            switch(pt){
                case pawn:   return this->boards[ 6];
                case knight: return this->boards[ 7];
                case bishop: return this->boards[ 8];
                case rook:   return this->boards[ 9];
                case queen:  return this->boards[10];
                default:     return this->boards[11];
            }
    }
}

U64& BitBoard::board(ptype pt, color c){
    switch(c){
        case white:
            switch(pt){
                case pawn:   return this->boards[ 0];
                case knight: return this->boards[ 1];
                case bishop: return this->boards[ 2];
                case rook:   return this->boards[ 3];
                case queen:  return this->boards[ 4];
                default:     return this->boards[ 5];
            }

        default:
            switch(pt){
                case pawn:   return this->boards[ 6];
                case knight: return this->boards[ 7];
                case bishop: return this->boards[ 8];
                case rook:   return this->boards[ 9];
                case queen:  return this->boards[10];
                default:     return this->boards[11];
            }
    }
}

/*
    Get piece info from square
*/

template<typename T>
color BitBoard::lookupc(const T src) const{
    U64 smsk = util::transform::mask(src);
    if     (this->board(white) & smsk){ return white; }
    else if(this->board(black) & smsk){ return black; }
    else                              { return NOCOLOR; }
}

template<typename T>
ptype BitBoard::lookupt(const T src) const{
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
void BitBoard::remove(const T src, ptype pt, color c){
    U64 smsk = ~util::transform::mask(src);
    this->board(pt, c) &= smsk;
    this->boards[12] &= smsk;
}

// Given location only (remove piece from all occupancy maps)
template<typename T>
void BitBoard::remove(const T src){
    U64 smsk = ~util::transform::mask(src);

    this->boards[ 0] &= smsk;
    this->boards[ 1] &= smsk;
    this->boards[ 2] &= smsk;
    this->boards[ 3] &= smsk;
    this->boards[ 4] &= smsk;
    this->boards[ 5] &= smsk;

    this->boards[ 6] &= smsk;
    this->boards[ 7] &= smsk;
    this->boards[ 8] &= smsk;
    this->boards[ 9] &= smsk;
    this->boards[10] &= smsk;
    this->boards[11] &= smsk;

    this->boards[12] &= smsk;
}

// Remove all pieces from all occupancy maps
void BitBoard::remove(){ this->boards.fill(util::constants::ZERO_); }

/*
    Place piece(s) on board
*/

template<typename T>
void BitBoard::place(const T dst, ptype pt, color c){
    U64 dmsk = util::transform::mask(dst);
    this->board(pt, c) |= dmsk;
    this->boards[12] |= dmsk;
}


/*
    Move piece from src to dst
*/

template<typename Ts, typename Td>
void BitBoard::move(const Ts src, const Td dst, ptype pt, color c){
    this->remove(src, pt, c);
    this->place(dst, pt, c);
}


/*
    Bitmap of squares attacked by single ray of sliding piece attack pattern
        DOES NOT ASSUME EMPTY BOARD
        Uses current occupancy
*/
template<typename T>
U64 BitBoard::ray(const T src, ptype pt, int dir) const{
    U64 r = util::bitboard::rays[pt-2][util::transform::sq(src)][dir];
    if(not r){ return r; }
    square x = dir>3 ? util::transform::bitscanr(r & this->board()) : util::transform::bitscan(r & this->board());
    if(x == -1 || x == 64){ return r; }
    U64 rx = util::bitboard::rays[pt-2][x][dir];
    return r ^ rx;
}

/*
    Determine if line connecting src and dst is unoccupied
*/
template<typename Ts, typename Td>
bool BitBoard::clearbt(const Ts src, const Td dst) const{
    if(util::bitboard::attackfrom(src, king) & util::transform::mask(dst)){
        // src and dst are adjacent - always clear
        return true;
    }
    else{
        U64 lbt = util::bitboard::linebt(src, dst);
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

std::string BitBoard::odisplay() const{ return util::bitboard::odisplay(this->boards[12]); }

std::string BitBoard::odisplay(color c) const{ return util::bitboard::odisplay(this->board(c)); }

std::string BitBoard::odisplay(ptype pt) const{ return util::bitboard::odisplay(this->board(pt)); }

std::string BitBoard::odisplay(ptype pt, color c) const{ return util::bitboard::odisplay(this->board(pt, c)); }


/*
    TEMPLATE INSTANCES
*/
template color BitBoard::lookupc(const square) const;
template color BitBoard::lookupc(const U64) const;
template color BitBoard::lookupc(const coords) const;

template ptype BitBoard::lookupt(const square) const;
template ptype BitBoard::lookupt(const U64) const;
template ptype BitBoard::lookupt(const coords) const;

template void BitBoard::remove(const square, ptype, color);
template void BitBoard::remove(const U64, ptype, color);
template void BitBoard::remove(const coords, ptype, color);

template void BitBoard::remove(const square);
template void BitBoard::remove(const U64);
template void BitBoard::remove(const coords);

template void BitBoard::place(const square, ptype, color);
template void BitBoard::place(const U64, ptype, color);
template void BitBoard::place(const coords, ptype, color);

template void BitBoard::move(const square, const square, ptype, color);
template void BitBoard::move(const U64, const U64, ptype, color);
template void BitBoard::move(const coords, const coords, ptype, color);
template void BitBoard::move(const square, const U64, ptype, color);
template void BitBoard::move(const square, const coords, ptype, color);
template void BitBoard::move(const U64, const coords, ptype, color);
template void BitBoard::move(const U64, const square, ptype, color);
template void BitBoard::move(const coords, const square, ptype, color);
template void BitBoard::move(const coords, const U64, ptype, color);

template U64 BitBoard::ray(const square, ptype, int) const;
template U64 BitBoard::ray(const U64, ptype, int) const;
template U64 BitBoard::ray(const coords, ptype, int) const;

template bool BitBoard::clearbt(const square, const square) const;
template bool BitBoard::clearbt(const U64, const U64) const;
template bool BitBoard::clearbt(const coords, const coords) const;
template bool BitBoard::clearbt(const square, const U64) const;
template bool BitBoard::clearbt(const square, const coords) const;
template bool BitBoard::clearbt(const U64, const coords) const;
template bool BitBoard::clearbt(const U64, const square) const;
template bool BitBoard::clearbt(const coords, const square) const;
template bool BitBoard::clearbt(const coords, const U64) const;