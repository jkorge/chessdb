#include "include/board.hpp"

namespace { constexpr std::array<U64, 4> rook_castle{0x0000000000000080, 0x0000000000000001, 0x8000000000000000, 0x0100000000000000}; }

ChessBoard::ChessBoard() : BitBoard() { this->newgame(); }

void ChessBoard::newgame(){

    this->pins = 0;
    this->enpas = 0;
    this->check = NOCOLOR;
    this->next = white;
    this->checkers.clear();
    this->cancas = 0b00001111;
    this->half = 0;
    this->full = 1;

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
    this->cancas = 0;
    this->half = 0;
    this->full = 1;
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
        }
        else{ this->remove(p.dst); }
    }
    if(p.type == pawn && p.dst == (p.c > 0 ? p.src << 16 : p.src >> 16)){
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

    this->update_state(p.src, p.type, p.c, p.capture);

    return name;
}

void ChessBoard::update_state(const U64& src, ptype pt, color c, bool cap){
    // called by this->update ; adjust member vars as needed

    // Half/Full move counters
    (cap || pt == pawn) ? (this->half = 0) : ++this->half;
    c<0 ? ++this->full : 0;

    // Pins
    this->pinsearch();

    // Castle Availability
    BYTE bsl = 0b0011 << (c>0 ? 2 : 0);
    if(this->cancas & bsl){
        if(pt == king){ this->cancas &= ~bsl; }
        else
        if(pt == rook){
            BYTE ksl = 0b0010 << (c>0 ? 2 : 0),
                 qsl = 0b0001 << (c>0 ? 2 : 0);
            int idx = 2 * (c<0);
            if(src == rook_castle.at(    idx)){ this->cancas &= ~ksl; }
            else
            if(src == rook_castle.at(1 + idx)){ this->cancas &= ~qsl; }
        }
    }

    this->next = !c;
}

void ChessBoard::castle(int cas, color c){
    U64 src = rook_castle.at((cas<0) + 2*(c<0)),
        dst = (cas > 0) ? (src >> 2) : (src << 3);
    this->move(src, dst, rook, c);
    this->swap(src, dst);
    this->cancas &= c == white ? 0b0011 : 0b1100;
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
        square x = util::transform::bitscan(plocs);
        if(okloc & util::bitboard::attackfrom(x, pt)){
            U64 lbt = util::bitboard::linebt(x, okloc),
                pnc = lbt & oppt,
                pnb = lbt & same;
            if(lbt && !pnb && !(pnc & pnc-1)){ this->pins |= pnc; }
        }
        util::transform::lsbflip(plocs);
    }
}

/*********************************
    LEGAL MOVES
*********************************/

U64 ChessBoard::legal() const{
    U64 res = 0;
    for(int i=white; i>=black; i-=2){ res |= this->legal(static_cast<color>(i)); }
    return res;
}

U64 ChessBoard::legal(color c, bool kl) const{
    U64 res = 0;
    int ub = kl ? king+1 : king;
    for(int j=pawn; j<ub; ++j){ res |= this->legal(static_cast<ptype>(j), c); }
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
    switch(pt){
        case pawn: {
            U64 opp = this->board(!c),
                pst = c>0 ? util::constants::white_pawns : util::constants::black_pawns;
            // Single push
            res |= (c>0 ? msk << 8 : msk >> 8) & ~occ;
            // Double push
            if(msk & pst && res){ res |= (c>0 ? msk << 16 : msk >> 16) & ~occ; }
            // Captures (including en passant)
            res |= util::bitboard::attackfrom(src, pt, c) & (this->next == c ? (opp | this->enpas) : opp);
            break;
        }
        case knight: {
            res |= util::bitboard::attackfrom(src, pt);
            break;
        }
        case bishop:
        case rook:
        case queen: {
            res |= this->sliding_atk(src, pt);
            break;
        }
        default: {
            U64 atk = this->legal(!c);
            res |= util::bitboard::attackfrom(src, pt)
                    & ~atk
                    & ~util::bitboard::attackfrom(this->board(pt, !c), pt);

            if(this->check != c && this->cancas & (c == white ? 0b1100 : 0b0011)){
                // Castles - check that squares b/t king and target squares are empty and not attacked
                U64 ksl = 0x0cLL << (c == white ? 0 : 56),
                    qsl = 0x60LL << (c == white ? 0 : 56);
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

std::vector<ply> ChessBoard::legal_plies(color c, bool klp) const{
    std::vector<ply> plies;
    int ub = klp ? king+1 : king;
    for(int j=0; j<ub; j++){
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

        // Capture and check
        bool cap = (pt!=pawn ? occ : (occ | this->enpas)) & dst,
             chk = okloc & util::bitboard::attackfrom(dst, pt, c) && (pt!=knight ? this->clearbt(dst, okloc) : true);

        // Castle
        int cas = 0;
        if(pt == king){
            if(dst == (msk << 2)){ cas =  1; }
            else
            if(dst == (msk >> 2)){ cas = -1; }
        }

        // Promotion
        bool pmo = pt == pawn && x >= 56;
        int lb = pmo ? knight : pawn,
            ub = pmo ? king   : knight;
        for(int i=lb; i<ub; ++i){
            plies.emplace_back(msk, dst, pt, static_cast<ptype>(i), c, cas, cap, chk, false);
            ChessBoard b = *this;
            b.update(plies.back());
            plies.back().mate = b.legal(!c, true) == 0;
        }

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
            // piece on x can move to kloc => opposing king in check
            if(this->legal(x, pt, c) & kloc){ this->checkers[util::transform::mask(x)] = pt; }
            util::transform::lsbflip(bb);
        }
    }
}


std::string ChessBoard::ply2san(const ply& p) const{
    /*
        [<N,B,R,Q,K>][f][r][x]fr[=[<N,B,R,Q>]][+#]
            - Piece type ID
            - Source rank disambiguation
            - Source file disambiguation
            - Capture indicator ('x')
            - Destination file and rank (required for all plies)
            - Promotion indicator ('=') and promotion value
            - Check/Mate inidcator ('+' or '#')

        Castles:
            - O-O   (kingside)
            - O-O-O (queenside)

    */
    std::string res;
    if(p.castle){
        res = p.castle == -1 ? "O-O-O" : "O-O";
        if(p.mate) { res += '#'; }
        else
        if(p.check){ res += '+'; }
        return res;
    }

    // flags indicating need to disambiguate on rank and file, respectively
    bool dbr = false,
         dbf = (p.type==pawn && p.capture) ? true : false;

    square srcx = util::transform::sq(p.src),
           dstx = util::transform::sq(p.dst);

    int srcf = srcx%8,
        srcr = srcx/8,
        dstf = dstx%8,
        dstr = dstx/8;

    // Other pieces of same type and color
    U64 bb = this->board(p.type, p.c) & ~p.src;

    while(bb){
        // Seek pieces which can reach dst (ie. seek need for disambiugation)
        square x = util::transform::bitscan(bb);
        if(this->legal(x, p.type, p.c) & p.dst){
            if     (srcf != x%8){ dbf = true; }
            else if(srcr != x/8){ dbr = true; }
        }
        util::transform::lsbflip(bb);
    }

    // piece type and disambiguating characters
    if(p.type > pawn){ res += util::repr::ptype2c(p.type); }
    if(dbf)          { res += util::repr::file2c(srcf); }
    if(dbr)          { res += util::repr::rank2c(srcr); }
    if(p.capture)    { res += 'x'; }

    // target square
    res += util::repr::file2c(dstf);
    res += util::repr::rank2c(dstr);

    // pawn promotion
    if(p.promo){ res += '='; res += util::repr::ptype2c(p.promo); }

    // mate or check
    if(p.mate) { res += '#'; }
    else
    if(p.check){ res += '+'; }

    return res;
}



template U64 ChessBoard::legal(const square&, ptype, color) const;
template U64 ChessBoard::legal(const U64&, ptype, color) const;
template U64 ChessBoard::legal(const coords&, ptype, color) const;

template std::vector<ply> ChessBoard::legal_plies(const square&, ptype, color) const;
template std::vector<ply> ChessBoard::legal_plies(const U64&, ptype, color) const;
template std::vector<ply> ChessBoard::legal_plies(const coords&, ptype, color) const;