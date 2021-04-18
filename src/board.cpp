#include "include/board.hpp"

namespace { constexpr std::array<U64, 4> rook_castle{0x0000000000000080, 0x0000000000000001, 0x8000000000000000, 0x0100000000000000}; }

ChessBoard::ChessBoard() : BitBoard() { this->newgame(); }

void ChessBoard::newgame(){

    // Arrange material
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

    // Reset state descriptors
    this->pins = 0;
    this->enpas = 0;
    this->check = NOCOLOR;
    this->next = white;
    this->checkers.clear();
    this->cancas = 0b00001111;
    this->half = 0;
    this->full = 1;
}

void ChessBoard::clear(){
    
    // Remove all material
    this->remove();
    for(int i=0; i<64; ++i){ this->map[i] = NONAME; }
    for(int i=0; i<32; ++i){ this->mat[static_cast<pname>(i)] = -1; }

    // Clear state descriptors
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

pname ChessBoard::update(const ply p){
    
    if(p == this->missing){ return NONAME; }
    
    /*
        APPLY PLAYER ACTION
    */

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
    if(p.castle){
        U64 src = rook_castle.at((p.castle<0) + 2*(p.c<0)),
            dst = (p.castle > 0) ? (src >> 2) : (src << 3);
        this->move(src, dst, rook, p.c);
        this->swap(src, dst);
        this->cancas &= p.c>0 ? 0b0011 : 0b1100;
    }
    if(p.promo){
        this->remove(p.dst, pawn, p.c);
        this->place(p.dst, p.promo, p.c);
    }

    /*
        UPDATE BOARD STATE
    */
    // Castle Availability
    BYTE bsl = 0b0011 << (p.c>0 ? 2 : 0);
    if(this->cancas & bsl){
        if(p.type == king){ this->cancas &= ~bsl; }
        else
        if(p.type == rook){
            BYTE ksl = 0b0010 << (p.c>0 ? 2 : 0),
                 qsl = 0b0001 << (p.c>0 ? 2 : 0);
            int idx = 2 * (p.c<0);
            if(p.src == rook_castle.at(    idx)){ this->cancas &= ~ksl; }
            else
            if(p.src == rook_castle.at(1 + idx)){ this->cancas &= ~qsl; }
        }
    }

    // Pins
    this->pinsearch();

    // Next to move
    this->next = !p.c;

    // Find all material executing check
    if(p.check)    { this->get_checkers(p.c); this->check = this->next; }
    else           { this->checkers.clear();  this->check = NOCOLOR; }

    // Half/Full move counters
    (p.capture || p.type == pawn) ? (this->half = 0) : ++this->half;
    p.c<0 ? ++this->full : 0;

    return name;
}

pname ChessBoard::swap(const U64 src, const U64 dst){
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
    this->scan(white);
    this->scan(black);
}

void ChessBoard::scan(color c){
    /*
        A pin is found if the following are all true:
            1. There exists an attack line (lbt) between candidate and king
            2. There are no pieces with the same color as candidate on lbt
            3. There is only one piece with the same color as king on lbt
    */

    U64 okloc = this->board(king, !c),
        same = this->board(c),
        oppt = this->board(!c);

    for(int i=bishop; i<=queen; ++i){
        ptype pt = static_cast<ptype>(i);
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
U64 ChessBoard::legal(const T src, ptype pt, color c) const{
    U64 res,
        msk = util::transform::mask(src),
        kloc = this->board(king, c);
    switch(pt){
        case pawn:   res = this->legal_pawn(msk, c); break;
        case knight: res = util::bitboard::attackfrom(src, pt); break;
        case bishop:
        case rook:
        case queen:  res = this->sliding_atk(src, pt); break;
        default:     res = this->legal_king(msk, c);
    }

    // Discount any moves that break a pin
    if(msk & this->pins){ res &= util::bitboard::axisalign(util::bitboard::linebt(msk, kloc, true)); }

    // Modify results if in check
    if(this->check == c){
        U64 filter = 0;
        for(std::unordered_map<U64, ptype>::const_iterator it=this->checkers.begin(); it!=this->checkers.end(); ++it){
            U64 lbt = util::bitboard::linebt(it->first, kloc, true);
            if(pt != king && this->checkers.size() == 1){ filter |= lbt; }
            else
            if(pt == king && it->second > knight)       { filter |= util::bitboard::axisalign(lbt) & ~it->first; }
        }
        res &= pt==king ? ~filter : filter;
    }

    // Discount squares occupied by same colored pieces
    res &= ~this->board(c);

    return res;
}

U64 ChessBoard::legal_king(const U64 src, color c) const{
    U64 res = 0,
        atk = 0,
        opwn = this->board(pawn, !c);

    // Bitboard of squares opponent is attacking
    while(opwn){
        atk |= util::bitboard::attackfrom(util::transform::bitscan(opwn), pawn, !c);
        util::transform::lsbflip(opwn);
    }
    for(int i=knight; i<king; ++i){ atk |= this->legal(static_cast<ptype>(i), !c); }

    // Can move to squares other than `atk` and enemy king's attackable squares
    res |= util::bitboard::attackfrom(src, king)
            & ~atk
            & ~util::bitboard::attackfrom(this->board(king, !c), king);

    // Castles - ensure availability and not in check
    if(this->check != c && this->cancas & (c == white ? 0b1100 : 0b0011)){
        // Verify that squares b/t king and target squares are empty and not attacked
        U64 ksl = 0x0cLL << (c == white ? 0 : 56),
            qsl = 0x60LL << (c == white ? 0 : 56);
        if(ksl == (ksl & ~this->board() & ~atk)){ res |= src >> 2; }
        if(qsl == (qsl & ~this->board() & ~atk)){ res |= src << 2; }
    }
    return res;
}

U64 ChessBoard::legal_pawn(const U64 src, color c) const{
    U64 res = 0,
        occ = this->board(),
        opp = this->board(!c),
        kloc = this->board(king, c),
        pst = c>0 ? util::constants::white_pawns : util::constants::black_pawns;

    // Single push
    res |= (c>0 ? src << 8 : src >> 8) & ~occ;

    // Double push
    if(src & pst && res){ res |= (c>0 ? src << 16 : src >> 16) & ~occ; }

    // Captures (including en passant)
    res |= util::bitboard::attackfrom(src, pawn, c) & (this->next == c ? (opp | this->enpas) : opp);

    if(this->enpas & res){
        /* Special case - e.p. capture would put king in check. Example:

            8/2p5/3p4/KP5r/1R2Pp1k/8/6P1/8 b - e3 0 1

               a   b   c   d   e   f   g   h
            8 [  ][  ][  ][  ][  ][  ][  ][  ]
            7 [  ][  ][b^][  ][  ][  ][  ][  ]
            6 [  ][  ][  ][b^][  ][  ][  ][  ]
            5 [wK][w^][  ][  ][  ][  ][  ][bR]
            4 [  ][wR][  ][  ][w^][b^][  ][bK]
            3 [  ][  ][  ][  ][  ][  ][  ][  ]
            2 [  ][  ][  ][  ][  ][  ][w^][  ]
            1 [  ][  ][  ][  ][  ][  ][  ][  ]

           If black plays fxe3 then black's king would be in check by white's rook on b4 => fxe3 is NOT a legal move
           Require that e.p. captures do not expose king to enemy rook/queen on the same rank
        */
        U64 rqo = (this->board(rook, !c) | this->board(queen, !c)) & util::bitboard::rmasks[util::transform::bitscan(kloc)];
        while(rqo){
            square x = util::transform::bitscan(rqo);
            util::transform::lsbflip(rqo);
            if(not (util::bitboard::linebt(x, kloc) & occ & ~(src | (c>0 ? this->enpas >> 8 : this->enpas << 8)))){ res &= ~this->enpas; break; }
        }
    }
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
std::vector<ply> ChessBoard::legal_plies(const T src, ptype pt, color c) const{
    std::vector<ply> plies;
    U64 pseudo = this->legal(src, pt, c),
        msk = util::transform::mask(src),
        okloc = this->board(king, !c),
        dst;
    int cas = 0;
    bool cap = false, chk = false, mte = false, pmo = false;

    while(pseudo){
        dst = util::transform::mask(util::transform::bitscan(pseudo));
        util::transform::lsbflip(pseudo);

        // Capture
        cap = (pt!=pawn ? this->board() : (this->board() | this->enpas)) & dst;

        // Castle
        if(pt == king){
            if(dst == (msk << 2)){ cas =  1; }
            else
            if(dst == (msk >> 2)){ cas = -1; }
            else                 { cas =  0; }
        }
        else{ cas = 0; }

        // Promotion
        pmo = !pt && (c>0 ? dst & util::bitboard::rmasks[56] : dst & util::bitboard::rmasks[7]);

        // Construct plies
        for(int i=(pmo ? knight : pawn); i<(pmo ? king : knight); ++i){
            plies.emplace_back(msk, dst, pt, static_cast<ptype>(i), c, cas, cap, false, false);

            // Check/Mate
            ChessBoard b = *this;
            b.update(plies.back());

            chk = b.legal(c) & b.board(king, !c);
            if(chk){
                b.get_checkers(c);
                b.check = b.next;
                mte = !b.legal(b.next, true);
            }
            else{ mte = false; }

            plies.back().check = chk;
            plies.back().mate = mte;
        }
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
            util::transform::lsbflip(bb);
            // piece on x can move to kloc => opposing king in check
            if(this->legal(x, pt, c) & kloc){ this->checkers[util::transform::mask(x)] = pt; }
        }
    }
}


std::string ChessBoard::ply2san(const ply p) const{
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



template U64 ChessBoard::legal(const square, ptype, color) const;
template U64 ChessBoard::legal(const U64, ptype, color) const;
template U64 ChessBoard::legal(const coords, ptype, color) const;

template std::vector<ply> ChessBoard::legal_plies(const square, ptype, color) const;
template std::vector<ply> ChessBoard::legal_plies(const U64, ptype, color) const;
template std::vector<ply> ChessBoard::legal_plies(const coords, ptype, color) const;