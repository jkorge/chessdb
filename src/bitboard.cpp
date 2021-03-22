#include "include/bitboard.hpp"


BitBoard::BitBoard(){
    for(int i=0; i<64; i++){

        /*
            SQUARE NUMBER
        */
        U64 msk = util::transform::mask(i);

        /*
            RANK/FILE/DIAGONAL/ANTIDIAGONAL
        */
        this->rmasks.emplace_back(this->rmask(i));
        this->fmasks.emplace_back(this->fmask(i));
        this->dmasks.emplace_back(this->dmask(i));
        this->amasks.emplace_back(this->amask(i));

        /*
            PAWN ATTACKS
        */
        U64 wpatk = util::constants::ZERO_,
            bpatk = util::constants::ZERO_;
        if(i<56){
            wpatk |= this->shift(msk, 1, -1);
            wpatk |= this->shift(msk, 1,  1);
        }
        this->wpattack.emplace_back(wpatk);

        if(i>=8){
            bpatk |= this->shift(msk, -1, -1);
            bpatk |= this->shift(msk, -1,  1);
        }
        this->bpattack.emplace_back(bpatk);

        /*
            KNIGHT ATTACKS
        */
        U64 natk = util::constants::ZERO_;
        for(int r=1; r<3; r++){

            int f = r==1 ? 2 : 1;
            natk |= this->shift(msk,  r, f);                                         // shift(...) returns zero if msk + (r,f) goes off board
            natk |= this->shift(msk,  r,-f);
            natk |= this->shift(msk, -r, f);
            natk |= this->shift(msk, -r,-f);
        }
        this->nattack.emplace_back(natk);

        /*
            SLIDING PIECE ATTACKS
        */
        this->rattack.emplace_back(msk ^ (this->rmasks[i]  | this->fmasks[i]));
        this->battack.emplace_back(msk ^ (this->dmasks[i]  | this->amasks[i]));
        this->qattack.emplace_back(       this->rattack[i] | this->battack[i]);      // xor not needed for queen - inherits from rook and bishop

        /*
            KING ATTACKS
        */
        U64 katk = util::constants::ZERO_;
        for(int rs=-1; rs<2; rs++){
            for(int fs=-1; fs<2; fs++){
                if(!rs && !fs){ continue; }
                katk |= this->shift(msk, rs, fs);
            }
        }
        this->kattack.emplace_back(katk);
    }

    /*
        LINE BETWEEN ALL PAIRS OF SQUARES (0 if none exists)
    */
    for(int s1=0; s1<64; ++s1){
        for(int s2=0; s2<64; ++s2){
            if(this->battack[s1] & util::transform::mask(s2)){
                this->lines[s1][s2] = (
                    this->battack[s1] &
                    this->battack[s2] &
                    ((util::constants::ALL_ << s1) ^ (util::constants::ALL_ << s2))
                );
            }
            else if(this->rattack[s1] & util::transform::mask(s2)){
                this->lines[s1][s2] = (
                    this->rattack[s1] &
                    this->rattack[s2] &
                    ((util::constants::ALL_ << s1) ^ (util::constants::ALL_ << s2))
                );
            }
            else{ this->lines[s1][s2] = util::constants::ZERO_; }
        }
    }
}


/*
    Retrieve occupancy bitmap
*/
U64 BitBoard::board(){
    if(this->changed){
        this->state = this->white_pawns | this->white_knights | this->white_bishops | this->white_rooks | this->white_queens | this->white_king |
                      this->black_pawns | this->black_knights | this->black_bishops | this->black_rooks | this->black_queens | this->black_king;
        this->changed = false;
    }
    return this->state;
}

U64 BitBoard::board(color c){
    switch(c){
        case white: return this->white_pawns | this->white_knights | this->white_bishops | this->white_rooks | this->white_queens | this->white_king;
        default:    return this->black_pawns | this->black_knights | this->black_bishops | this->black_rooks | this->black_queens | this->black_king;
    }
}

U64 BitBoard::board(ptype pt){
    switch(pt){
        case pawn:   return this->white_pawns   | this->black_pawns;
        case knight: return this->white_knights | this->black_knights;
        case bishop: return this->white_bishops | this->black_bishops;
        case rook:   return this->white_rooks   | this->black_rooks;
        case queen:  return this->white_queens  | this->black_queens;
        default:     return this->white_king    | this->black_king;
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
color BitBoard::lookupc(const T& src){
    U64 smsk = util::transform::mask(src);
    if     (this->board(white) & smsk){ return white; }
    else if(this->board(black) & smsk){ return black; }
    else                              { return NOCOLOR; }
}

template<typename T>
ptype BitBoard::lookupt(const T& src){
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
    Determine if two points are colinear
*/

template<typename Ts, typename Td>
bool BitBoard::colinear(const Ts& src, const Td& dst){
    int ssq = util::transform::sq(src),
        dsq = util::transform::sq(dst);

    if(this->lines[ssq][dsq]){ return true; }
    else{ return false; }
}

/*
    Determine direction of line connecting two points
*/

template<typename Ts, typename Td>
ptype BitBoard::linetype(const Ts& src, const Td& dst){
    int ssq = util::transform::sq(src),
        dsq = util::transform::sq(dst);

    ptype res;

    if(
        (this->dmasks[ssq] & this->dmasks[dsq]) ||
        (this->amasks[ssq] & this->amasks[dsq])
    ){ res = bishop; }
    else if(
        (this->rmasks[ssq] & this->rmasks[dsq]) ||
        (this->fmasks[ssq] & this->fmasks[dsq])
    ){ res = rook; }
    else{ res = NOTYPE; }

    return res;
}

/*
    Bitmap of rank/file/diagonal/anti-diagonal for given square number
*/

U64 BitBoard::rmask(const square& sq){ return util::constants::RANK_ << (sq & 56); }

U64 BitBoard::fmask(const square& sq){ return util::constants::FILE_ << (sq & 7); }

U64 BitBoard::dmask(const square& sq){
   int diag = 8*(sq & 7) - (sq & 56);
   int nort = -diag & ( diag >> 31);
   int sout =  diag & (-diag >> 31);
   return (util::constants::MAINDIAG_ >> sout) << nort;
}

U64 BitBoard::amask(const square& sq){
   int diag = 56 - 8*(sq & 7) - (sq & 56);
   int nort = -diag & ( diag >> 31);
   int sout =  diag & (-diag >> 31);
   return (util::constants::ANTIDIAG_ >> sout) << nort;
}

/*
    Bitmap of squares attacked by pt when positioned on src
*/

template<typename T>
U64 BitBoard::attackfrom(const T& src, ptype pt, color c){
    square ssq = util::transform::sq(src);
    switch(pt){
        case pawn:
            switch(c){
                case white: return this->wpattack[ssq];
                default:    return this->bpattack[ssq];
            }
        case knight:        return this->nattack[ssq];
        case bishop:        return this->battack[ssq];
        case rook:          return this->rattack[ssq];
        case queen:         return this->qattack[ssq];
        default:            return this->kattack[ssq];
    }
}

/*
    Bitmap of line between src and dst
*/

template<typename Ts, typename Td>
U64 BitBoard::linebt(const Ts& src, const Td& dst, bool endp){
    U64 res = this->lines[util::transform::sq(src)][util::transform::sq(dst)];
    if(endp){ res |= util::transform::mask(src) | util::transform::mask(dst); }
    return res;
}

/*
    Determine if line connecting src and dst is unoccupied
*/
template<typename Ts, typename Td>
bool BitBoard::clearbt(const Ts& src, const Td& dst, bool endp){ return not (this->linebt(src, dst, endp) & this->board()); }

/*
    Shift coordinates
*/

template<typename T>
U64 BitBoard::rshift(const T& src, int s){
    int r = (util::transform::sq(src)/8) + s;
    if(r<0 || r>7){ return util::constants::ZERO_; }
    else{ return s<0 ? (src >> 8*abs(s)) : (src << 8*s); }
}

template<typename T>
U64 BitBoard::fshift(const T& src, int s){
    int f = (util::transform::sq(src)%8) + s;
    if(f<0 || f>7){ return util::constants::ZERO_; }
    else{ return s<0 ? (src >> abs(s)) : (src << s); }
}

template<typename T>
U64 BitBoard::shift(const T& src, int rs, int fs){
    int ssq = util::transform::sq(src),
          r = (ssq/8) + rs,
          f = (ssq%8) + fs;
    if(r<0 || r>7 || f<0 || f>7){ return util::constants::ZERO_; }
    else{ return util::transform::mask(coords(r,f)); }
}

/*
    Visualizations
*/

std::string BitBoard::display(){
    
    std::string viz(1, util::constants::endl);
    U64 grid = this->board();

    // File headers
    for(int k=0; k<8; k++){ viz += std::string("   ") + util::constants::files[k]; }

    char square[]{"[  ]"};
    coords loc;

    for(int r=7; r>=0; --r){
        loc[0] = r;
        for(int f=0; f<8; ++f){
            loc[1] = f;

            // Rank headers
            if(!f){ viz += std::string(1, util::constants::endl) + util::repr::rank2c(r) + " "; }

            U64 src = util::transform::mask(loc);

            if(grid & src){
                square[1] = util::repr::color2c(this->lookupc(src));
                square[2] = util::repr::ptype2c(this->lookupt(src));
            }
            else{
                square[1] = ' ';
                square[2] = ' ';
            }

            viz += square;
        }
    }

    return viz;
}

std::string BitBoard::odisplay(const U64& grid){
    std::bitset<64> bsg = static_cast<std::bitset<64> >(grid);
    std::string viz;
    for(int r=7; r>=0; --r){
        viz += util::constants::endl;
        for(int f=0; f<8; ++f){ viz += bsg.test((8*r) + f) ? " 1 " : " . "; }
    }
    return viz;
}

std::string BitBoard::odisplay(){ return this->odisplay(this->board()); }

std::string BitBoard::odisplay(color c){ return this->odisplay(this->board(c)); }

std::string BitBoard::odisplay(ptype pt){ return this->odisplay(this->board(pt)); }

std::string BitBoard::odisplay(ptype pt, color c){ return this->odisplay(this->board(pt, c)); }


/*
    TEMPLATE INSTANCES
*/
template color BitBoard::lookupc(const square&);
template color BitBoard::lookupc(const U64&);
template color BitBoard::lookupc(const coords&);

template ptype BitBoard::lookupt(const square&);
template ptype BitBoard::lookupt(const U64&);
template ptype BitBoard::lookupt(const coords&);

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

template bool BitBoard::colinear(const square&, const square&);
template bool BitBoard::colinear(const U64&, const U64&);
template bool BitBoard::colinear(const coords&, const coords&);
template bool BitBoard::colinear(const square&, const U64&);
template bool BitBoard::colinear(const square&, const coords&);
template bool BitBoard::colinear(const U64&, const coords&);
template bool BitBoard::colinear(const U64&, const square&);
template bool BitBoard::colinear(const coords&, const square&);
template bool BitBoard::colinear(const coords&, const U64&);

template ptype BitBoard::linetype(const square&, const square&);
template ptype BitBoard::linetype(const U64&, const U64&);
template ptype BitBoard::linetype(const coords&, const coords&);
template ptype BitBoard::linetype(const square&, const U64&);
template ptype BitBoard::linetype(const square&, const coords&);
template ptype BitBoard::linetype(const U64&, const coords&);
template ptype BitBoard::linetype(const U64&, const square&);
template ptype BitBoard::linetype(const coords&, const square&);
template ptype BitBoard::linetype(const coords&, const U64&);

template U64 BitBoard::attackfrom(const square&, ptype, color=white);
template U64 BitBoard::attackfrom(const U64&, ptype, color=white);
template U64 BitBoard::attackfrom(const coords&, ptype, color=white);

template U64 BitBoard::linebt(const square&, const square&, bool=false);
template U64 BitBoard::linebt(const U64&, const U64&, bool=false);
template U64 BitBoard::linebt(const coords&, const coords&, bool=false);
template U64 BitBoard::linebt(const square&, const U64&, bool=false);
template U64 BitBoard::linebt(const square&, const coords&, bool=false);
template U64 BitBoard::linebt(const U64&, const coords&, bool=false);
template U64 BitBoard::linebt(const U64&, const square&, bool=false);
template U64 BitBoard::linebt(const coords&, const square&, bool=false);
template U64 BitBoard::linebt(const coords&, const U64&, bool=false);

template bool BitBoard::clearbt(const square&, const square&, bool=false);
template bool BitBoard::clearbt(const U64&, const U64&, bool=false);
template bool BitBoard::clearbt(const coords&, const coords&, bool=false);
template bool BitBoard::clearbt(const square&, const U64&, bool=false);
template bool BitBoard::clearbt(const square&, const coords&, bool=false);
template bool BitBoard::clearbt(const U64&, const coords&, bool=false);
template bool BitBoard::clearbt(const U64&, const square&, bool=false);
template bool BitBoard::clearbt(const coords&, const square&, bool=false);
template bool BitBoard::clearbt(const coords&, const U64&, bool=false);

template U64 BitBoard::rshift(const square&, int);
template U64 BitBoard::rshift(const U64&, int);

template U64 BitBoard::fshift(const square&, int);
template U64 BitBoard::fshift(const U64&, int);

template U64 BitBoard::shift(const square&, int, int);
template U64 BitBoard::shift(const U64&, int, int);