#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <bitset>


/**************************
    SHORTHAND TYPES
**************************/
typedef unsigned char BYTE;
typedef std::pair<int, int> ipair;
typedef std::bitset<8> bs8;


typedef struct coords{
    int arr[2];
    coords () {}
    coords (int r, int f) : arr{r, f} {}
    int& operator[](int i) { return this->arr[i]; }
    int* operator=(int* i) { return this->arr; }
} coords;

/**************************
    MATERIAL
**************************/

// Types of material
typedef enum ptype {pawn = 0, knight, bishop, rook, queen, king, NOTYPE = -1} ptype;

// Identifiers for all 32 pieces
// <player>_<type>_<starting file>
typedef enum pname {

    white_pawn_a = 0,
    white_pawn_b,
    white_pawn_c,
    white_pawn_d,
    white_pawn_e,
    white_pawn_f,
    white_pawn_g,
    white_pawn_h,
    white_knight_b,
    white_knight_g,
    white_bishop_c,
    white_bishop_f,
    white_rook_a,
    white_rook_h,
    white_queen_d,
    white_king_e,

    black_pawn_a,
    black_pawn_b,
    black_pawn_c,
    black_pawn_d,
    black_pawn_e,
    black_pawn_f,
    black_pawn_g,
    black_pawn_h,
    black_knight_b,
    black_knight_g,
    black_bishop_c,
    black_bishop_f,
    black_rook_a,
    black_rook_h,
    black_queen_d,
    black_king_e,

    NONAME = -1   // For unoccupied board squares

} pname;

// Piece object for all material
typedef struct pce{

    pname name;
    ptype type;
    int rank, file;
    bool captured = false;

    // Constructors
    pce (pname p, int r, int f) : name{p}, rank{r}, file{f} {
        if(p==NONAME){ this->type = NOTYPE; }
        else{ int v = p%16; this->type = static_cast<ptype>((v/8) + (v/10) + (v/12) + (v/14) + (v/15)); }
    }
    pce (pname p, int* c) : pce(p, c[0], c[1]) {}
    pce (pname p, ipair c) : pce(p, c.first, c.second) {}
    pce (pname p) : pce(p, -1, -1) {}
    pce () : pce(NONAME, -1, -1) {}

    // Misc utils
    bool white() { return ((this->name) < 16) & (this->name >= 0); }
    bool black() { return (this->name) >= 16; }
} pce;

/**************************
    GAME
**************************/

typedef struct ply{

    pce piece;
    ptype promo;     // ptype for pawn promotions

    int dst[2];             // Target coords (change in coords if returned from Decoder)
    int castle;             // 0 => No castle; 1 => kingside; -1 => queenside

    bool capture;
    bool check;
    bool mate;

    /*
        CONSTRUCTORS
    */

    // Intialize nothing
    ply () {}

    // Initialize everything
    ply (pce p, int r, int f, int c, bool cap, bool ch, bool m, ptype po)
        : piece{p},
          dst{r, f},
          castle{c},
          capture{cap},
          check{ch},
          mate{m},
          promo{po} {}

    // Piece
    ply (pce p) : ply(p, -1, -1, 0, false, false, false, pawn) {}

    // Pice to dst (ints)
    ply (pce p, int r, int f) : ply(p, r, f, 0, false, false, false, pawn) {}

    // Piece to dst (array)
    ply (pce p, int *d) : ply(p, d[0], d[1], 0, false, false, false, pawn) {}

    // Piece to dst (ipair)
    ply (pce p, ipair c) : ply(p, c.first, c.second, 0, false, false, false, pawn) {}

    // Castle (p should have type==king)
    ply (pce p, int c) : ply(p, -1, -1, c, false, false, false, pawn) {}

    // Piece to dst (ints) with promo
    ply (pce p, int r, int f, ptype po) : ply(p, r, f, 0, false, false, false, po) {}

    // Piece to dst (array) with promo
    ply (pce p, int *d, ptype po) : ply(p, d[0], d[1], 0, false, false, false, po) {}

    // Piece to dst (ipair) with promo
    ply (pce p, ipair c, ptype po) : ply(p, c.first, c.second, 0, false, false, false, po) {}


} ply;

typedef struct turn{
    ply white, black;
    int game_end;           // Indicates whether the game ended on this turn. 0 => no, 1 => white won, -1 => black won, 2 => draw

    // Constructors
    turn () {}
    turn (ply w, ply b, int g) : white{w}, black{b}, game_end{g} {}
    turn (ply w, ply b) : turn(w, b, 0) {}
    turn (ply w) : white{w}, game_end{0} {}
    turn (ply w, int g) : white{w}, game_end{g} {}
    turn (int g) : game_end{g} {}

    ply operator[](int i) { return (i ? this->black : this->white); }

} turn;

typedef struct game{
    std::vector<turn> turns;
    bool over;

    // Constructors
    game () : over{false} {}
    game (std::vector<turn> m) : turns{m} { this->over = !m.back().game_end; }

    // Misc utils
    void append(turn t) { this->turns.push_back(t); this->over = this->turns.back().game_end; }
    int length() { return this->turns.size(); }
    int result() { return this->back().game_end; }
    turn back() { return this->turns.back(); }
    turn operator[](int i) { return (this->turns)[i]; }
} game;

/**************************
    ENCODED GAME
**************************/

typedef struct eply{
    BYTE name, action;

    // Constructors
    eply () : name{0}, action{0} {}
    eply (BYTE p, BYTE a) : name{p}, action{a} {}
} eply;

typedef struct eturn{
    eply white, black;

    eturn () {}
    eturn (eply w, eply b) : white{w}, black{b} {}
    eturn (eply w) : white{w} {}
} eturn;

typedef struct egame{
    std::vector<eturn> turns;

    egame () {}
    egame (std::vector<eturn> t) : turns{t} {}

    void append(eturn t) { this->turns.push_back(t); }
    int length() { return this->turns.size(); }
    eturn operator[](int i) { return (this->turns).at(i); }
} egame;


/**************************
    PGN
**************************/

/*
    This is NOT a complete enum of all PGN tag keys
    Only those deemed most useful in analyzing games are included here
    See https://www.chessclub.com/help/PGN-spec for more info on PGN specifications
*/

typedef enum pgntag{
    // Seven Tag Roster
    event,
    site,
    date,
    round,
    white,
    black,
    result,

    // Other common (and informative) Keys
    eco,
    fen,
    mode,
    time_control,
    termination,

    // Player Rating Keys
    white_elo,
    black_elo,
    white_uscf,
    black_uscf

} pgntag;

typedef struct pgndict : std::map<pgntag, std::string>{
    pgndict () : std::map<pgntag, std::string>{
        {event, "\0"},
        {site, "\0"},
        {date, "\0"},
        {round, "\0"},
        {white, "\0"},
        {black, "\0"},
        {result, "\0"},
        {eco, "\0"},
        {fen, "\0"},
        {mode, "\0"},
        {time_control, "\0"},
        {termination, "\0"},
        {white_elo, "\0"},
        {white_uscf, "\0"},
        {black_elo, "\0"},
        {black_uscf, "\0"}
    } {}
} pgndict;

#endif