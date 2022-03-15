#ifndef CHESS_H
#define CHESS_H

#include <immintrin.h>
#include <cctype>
#include <cstdint>
#include <cstdlib>

#include <array>
#include <exception>
#include <random>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

/**************************************************
                    TYPEDEFS
**************************************************/

/*
    INTEGRAL TYPES
*/

typedef unsigned char BYTE;

typedef uint64_t U64;

typedef int square;

typedef uint16_t eply;

/*
    ENUMS
*/

typedef enum color{ white = 1, black = -1, NOCOLOR = 0 } color;

typedef enum ptype{ pawn = 0, knight, bishop, rook, queen, king, NOTYPE = -1 } ptype;

typedef enum direction{ east = 0, northeast, north, northwest, west, southwest, south, southeast, NODIR = -1} direction;

typedef enum pgntag{
    // Seven Tag Roster
    event,
    site,
    date,
    roundn,
    playerw,
    playerb,
    result,

    // Other common (and informative) tags
    eco,
    fenstr,
    mode,
    time_control,
    termination,

    // Player Ratings
    white_elo,
    black_elo,
    white_uscf,
    black_uscf
} pgntag;

/*
    STRUCTS
*/

typedef struct ply{

    U64 src,
        dst;
    color c;
    ptype type,
          promo;
    int castle;             // 0 => No castle; 1 => kingside; -1 => queenside
    bool capture,
         check,
         mate;

    /*
        CONSTRUCTORS
    */

    // Zero Initialize
    ply(): src(0), dst(0), c(NOCOLOR), type(pawn), promo(pawn), castle(0), capture(false), check(false), mate(false) {}

    // Initialize everything
    ply(U64 s, U64 d, ptype pt, ptype po, color col, int cas, bool cap, bool ch, bool m)
        : src(s), dst(d), c(col), type(pt), promo(po), castle(cas), capture(cap), check(ch), mate(m) {}

    ply(square s, square d, ptype pt, ptype po, color col, int cas, bool cap, bool ch, bool m)
        : src(1ULL << s), dst(1ULL << d), c(col), type(pt), promo(po), castle(cas), capture(cap), check(ch), mate(m) {}

    // Copy constructor
    ply(const ply& other){ *this = other; }

    /*
        COMPARISONS
    */
    constexpr bool operator==(const ply& other) const{
        return (this->src     == other.src)     &&
               (this->dst     == other.dst)     &&
               (this->c       == other.c)       &&
               (this->type    == other.type)    &&
               (this->promo   == other.promo)   &&
               (this->castle  == other.castle)  &&
               (this->capture == other.capture) &&
               (this->check   == other.check)   &&
               (this->mate    == other.mate);
    }
    constexpr bool operator!=(const ply& other) const{ return not (*this == other); }

    /*
        ASSIGNMENT
    */
    constexpr ply& operator=(const ply& other){ std::memcpy(this, &other, sizeof(other)); return *this; }
} ply;

typedef struct bitboard : std::array<U64, 15>{
    inline const U64& operator()(ptype pt, color c) const{ return this->operator[](pt + 6*(c<0)); }
    inline       U64& operator()(ptype pt, color c)      { return this->operator[](pt + 6*(c<0)); }

    inline const U64& operator()(color c)           const{ return this->operator[](12 + (c<0)); }
    inline       U64& operator()(color c)                { return this->operator[](12 + (c<0)); }

    inline       U64  operator()(ptype pt)          const{ return this->operator[](pt) | this->operator[](pt+6); }

    inline const U64& operator()()                  const{ return this->operator[](14); }
    inline       U64& operator()()                       { return this->operator[](14); }
} bitboard;

typedef struct coords : std::array<int, 2>{
    coords() : std::array<int, 2>{0} {}
    coords(const square x) : std::array<int, 2>{x/8, x%8} {}
    coords(const U64 src) : coords((square)_tzcnt_u64(src)) {}
    coords(const int r, const int f) : std::array<int, 2>{r, f} {}
    coords operator+(const coords& other) const{ return {this->at(0) + other.at(0), this->at(1) + other.at(1)}; }
} coords;

typedef struct pgndict : std::map<pgntag, std::string>{
    pgndict () : std::map<pgntag, std::string>{
        {event,        std::string()},
        {site,         std::string()},
        {date,         std::string()},
        {roundn,       std::string()},
        {playerw,      std::string()},
        {playerb,      std::string()},
        {result,       std::string()},
        {eco,          std::string()},
        {fenstr,       std::string()},
        {mode,         std::string()},
        {time_control, std::string()},
        {termination,  std::string()},
        {white_elo,    std::string()},
        {white_uscf,   std::string()},
        {black_elo,    std::string()},
        {black_uscf,   std::string()}
    } {}

    void reset(){ for(std::map<pgntag, std::string>::iterator it=this->begin(); it!=this->end(); ++it){ it->second.clear(); } }
} pgndict;

typedef struct game{
    pgndict tags;
    std::vector<ply> plies;

    game () {}
    game (pgndict _tags, std::vector<ply> _plies) : tags{_tags}, plies{_plies} {}
    game (std::vector<ply> _plies, pgndict _tags) : tags{_tags}, plies{_plies} {}
    ~game ();
    bool operator==(const game& other) const{ return (this->tags == other.tags) && (this->plies == other.plies); }
    bool operator!=(const game& other) const{ return not (*this == other); }
    bool operator< (const game& other) const{ return this->tags.at(date) < other.tags.at(date); }
} game;

typedef struct Magic{
    U64 mask, magic;
    U64* attacks;
    uint32_t shift;

    int index(U64 occ) const{ return ((occ & this->mask) * this->magic) >> this->shift; }

    U64 value(U64 occ) const{ return this->attacks[this->index(occ)]; }
} Magic;

/**************************************************
                GLOBAL CONSTANTS
**************************************************/

// Castling rights (KQkq)
template<color c=NOCOLOR, int side=0>
constexpr BYTE castle = 0b1111;

template<>
constexpr BYTE castle<white, -1> = 0b0100;

template<>
constexpr BYTE castle<white,  0> = 0b1100;

template<>
constexpr BYTE castle<white,  1> = 0b1000;

template<>
constexpr BYTE castle<black, -1> = 0b0001;

template<>
constexpr BYTE castle<black,  0> = 0b0011;

template<>
constexpr BYTE castle<black,  1> = 0b0010;

// Rook src and dst during castling
template<color c, int side>
constexpr U64 rook_castle_src = 0ULL;

template<>
constexpr U64 rook_castle_src<white, -1> = 0x0000000000000001ULL;

template<>
constexpr U64 rook_castle_src<white,  1> = 0x0000000000000080ULL;

template<>
constexpr U64 rook_castle_src<black, -1> = 0x0100000000000000ULL;

template<>
constexpr U64 rook_castle_src<black,  1> = 0x8000000000000000ULL;

template<color c, int side>
constexpr U64 rook_castle_dst = 0ULL;

template<color c>
constexpr U64 rook_castle_dst<c, -1> = rook_castle_src<c, -1> << 3;

template<color c>
constexpr U64 rook_castle_dst<c, 1> = rook_castle_src<c,  1> >> 2;

// King's traversal during castling
template<color c>
constexpr U64 ksl  = 0x60ULL << (c>0 ? 0 : 56);

template<color c>
constexpr U64 qslc = 0x0eULL << (c>0 ? 0 : 56);

template<color c>
constexpr U64 qsls = 0x0cULL << (c>0 ? 0 : 56);

// Board axes
constexpr U64 MAINDIAG_     = 0x8040201008040201;
constexpr U64 ANTIDIAG_     = 0x0102040810204080;
constexpr U64 RANK_         = 0xff;
constexpr U64 FILE_         = 0x0101010101010101;
constexpr U64 BACK_RANK_B_  = RANK_;
constexpr U64 BACK_RANK_W_  = RANK_ << 56;

// 0, 1, All squares
constexpr U64 ONE_          = 0x01;
constexpr U64 ZERO_         = 0x00;
constexpr U64 ALL_          = 0xffffffffffffffff;

// Squares @ ends of BB
constexpr U64 A1            = 0x01;
constexpr U64 H8            = A1 << 63;

// White starting coordinates
constexpr U64 white_pawns   = 0x000000000000ff00;
constexpr U64 white_knights = 0x0000000000000042;
constexpr U64 white_bishops = 0x0000000000000024;
constexpr U64 white_rooks   = 0x0000000000000081;
constexpr U64 white_queen   = 0x0000000000000008;
constexpr U64 white_king    = 0x0000000000000010;

// Black starting coordinates
constexpr U64 black_pawns   = white_pawns   << 40;
constexpr U64 black_knights = white_knights << 56;
constexpr U64 black_bishops = white_bishops << 56;
constexpr U64 black_rooks   = white_rooks   << 56;
constexpr U64 black_queen   = white_queen   << 56;
constexpr U64 black_king    = white_king    << 56;

// Starting occupancies
constexpr U64 white_mat = white_pawns | white_knights | white_bishops | white_rooks | white_queen | white_king;
constexpr U64 black_mat = black_pawns | black_knights | black_bishops | black_rooks | black_queen | black_king;
constexpr U64 occupancy =   white_mat | black_mat;

// Collect start coords and occupancies for newgame bitboard
constexpr bitboard newgame = {
    white_pawns, white_knights, white_bishops, white_rooks, white_queen, white_king,
    black_pawns, black_knights, black_bishops, black_rooks, black_queen, black_king,
    white_mat, black_mat, occupancy
};

constexpr char bcell[]{"[  ]"};
constexpr char bbcell[]{" . "};

// Board axes
extern const std::array<U64, 64> rmasks;
extern const std::array<U64, 64> fmasks;
extern const std::array<U64, 64> dmasks;
extern const std::array<U64, 64> amasks;

// Empty-board attack patterns
extern const std::array<U64, 64> wpattack;
extern const std::array<U64, 64> bpattack;
extern const std::array<U64, 64> nattack;
extern const std::array<U64, 64> battack;
extern const std::array<U64, 64> rattack;
extern const std::array<U64, 64> qattack;
extern const std::array<U64, 64> kattack;

// Lines between squares
extern const std::array<std::array<U64, 64>, 64> axes;
extern const std::array<std::array<U64, 64>, 64> lines;

// Directional rays
extern const std::array<std::array<std::array<U64, 8>, 64>, 3> rays;

// Magic bitboard attack patterns (includes endpoints)
extern const std::array<U64, 64> battackm;
extern const std::array<U64, 64> rattackm;

// Piece, rank, and file IDs
extern const std::string pieces;
extern const std::string files;
extern const std::string ranks;

// New line shorthand
extern const std::string endl;

// Strings for visualizing boards
extern const std::string bviz;
extern const std::string bbviz;

// Tag enums <-> Strings
extern std::map<std::string, pgntag> s2tag;
extern std::map<pgntag, std::string> tag2s;

// Arrays for magic numbers
extern Magic BMagics[64];
extern Magic RMagics[64];

/**************************************************
                CONSTEXPR FUNCTIONS
**************************************************/

// invert color (NOCOLOR => NOCOLOR)
constexpr color operator!(const color& c){ return static_cast<color>(black * c); }

// Convert to/from character representations
constexpr int c2file(char f){
    switch(f){
        case 'a': return 0;
        case 'b': return 1;
        case 'c': return 2;
        case 'd': return 3;
        case 'e': return 4;
        case 'f': return 5;
        case 'g': return 6;
        case 'h':
        default:  return 7;
    }
}

constexpr int c2rank(char r){
    switch(r){
        case '1': return 0;
        case '2': return 1;
        case '3': return 2;
        case '4': return 3;
        case '5': return 4;
        case '6': return 5;
        case '7': return 6;
        case '8':
        default:  return 7;
    }
}

constexpr ptype c2ptype(char p){
    switch(p){
        case '^': return pawn;
        case 'N': return knight;
        case 'B': return bishop;
        case 'R': return rook;
        case 'Q': return queen;
        case 'K': return king;
        default:  return NOTYPE;
    }
}

constexpr char file2c(int f){
    switch(f){
        case 0:  return 'a';
        case 1:  return 'b';
        case 2:  return 'c';
        case 3:  return 'd';
        case 4:  return 'e';
        case 5:  return 'f';
        case 6:  return 'g';
        case 7:
        default: return 'h';
    }
}

constexpr char rank2c(int r){
    switch(r){
        case 0:  return '1';
        case 1:  return '2';
        case 2:  return '3';
        case 3:  return '4';
        case 4:  return '5';
        case 5:  return '6';
        case 6:  return '7';
        case 7:
        default: return '8';
    }
}

constexpr char ptype2c(ptype p){
    switch(p){
        case pawn:   return '^';
        case knight: return 'N';
        case bishop: return 'B';
        case rook:   return 'R';
        case queen:  return 'Q';
        case king:   return 'K';
        default:     return '_';
    }
}

constexpr char color2c(color c){
    switch(c){
        case white: return 'w';
        case black: return 'b';
        default:    return ' ';
    }
}

// Flip the least significant 1-bit (LS1B)
constexpr void lsbflip(U64& x){ x &= x-1; }

// Square number => U64
constexpr U64 mask(const square x){ return 1ULL << x; }

// Board axes
constexpr U64 rank(const int r){ return RANK_ << (8*r); }
constexpr U64 file(const int f){ return FILE_ << f; }

// Printing utils
constexpr int bvizidx(square sq){ return 36 + 35*(7 - sq/8) + 4*(sq%8); }
constexpr int bbvizidx(square sq){ return 2 + 25*(7 - sq/8) + 3*(sq%8); }

// Square for pushing pawns
constexpr U64 push(U64 src, color c){
    switch(c){
        case white: return src << 8;
        case black:
        default:    return src >> 8;
    }
}

constexpr U64 double_push(U64 src, color c){
    switch(c){
        case white: return src << 16;
        case black:
        default:    return src >> 16;
    }
}

// Rank at "back" of board, relative to given player's color
constexpr U64 back_rank(color c){
    switch(c){
        case white: return BACK_RANK_W_;
        case black:
        default:    return BACK_RANK_B_;
    }
}

/**************************************************
                INLINE FUNCTIONS
**************************************************/

// Bitscan Foward - Index of LS1B (64 if src == 0)
inline square bitscan(const U64 src){ return _tzcnt_u64(src); }

// Bitscan Reverse - Index of MS1B (-1 if src == 0)
inline square bitscanr(const U64 src){ return 63 - _lzcnt_u64(src); }

// Count number of 1 bits
inline int popcnt(const U64 x){ return _mm_popcnt_u64(x); }

// Bitscan, flip ls1b, return bitscan result
inline square lsbpop(U64& x){ square s = bitscan(x); lsbflip(x); return s; }

// Empty board movement patterns for sliding pieces
inline U64 ray(const square sq, const ptype pt, const int dir){ return rays[pt-2][sq][dir]; }

// Rank/File/Diagonal/Antidiagonal shared by two squares (0 if none)
inline U64 axis(const square sq1, const square sq2){ return axes[sq1][sq2]; }

// Board axes of the given square
inline U64 rankof(const square sq){ return rmasks[sq]; }
inline U64 fileof(const square sq){ return fmasks[sq]; }
inline U64 diagof(const square sq){ return dmasks[sq]; }
inline U64 adagof(const square sq){ return amasks[sq]; }

/**************************************************
                FUNCTION DECLARATIONS
**************************************************/

// Return true if given char identifies material/file (in SAN)
bool ispiece(char);
bool isfile(char);

// Convert to string
std::string ptype2s(ptype);
std::string color2s(color);
std::string coord2s(square);
std::string coord2s(U64);
std::string pgndict2s(const pgndict&);
std::string bb2s(U64, char='1');

// Convert from string
square s2coord(std::string);

// Line between two squares (optionally inclusive)
U64 line(const square, const square, bool=false);

// Sliding piece movements corresponding to the given occupancy
U64 slide_atk(const square, const ptype, const U64);

// Empty board attack patterns
U64 attack(const square, const ptype, const color=NOCOLOR);

// Sliding attack masks (same as `attack` for sliding pieces but exludes edge squares)
U64 attackm(const square, const ptype);

// Bitboard of pieces giving check
U64 search_checks(const bitboard, const color);

// Bitboard of pinned pieces
U64 search_pins(const bitboard);

// RNG for magic bitboard initialization
U64 random_magic();

// Intialize magic bitboards
void magic_init(const ptype, U64*, Magic*);

/**************************************************
                    BOARD CLASS
**************************************************/

// State struct contains all Board's member vars
// Makes lookahead easier (ie. copy state, apply update, replace state)
struct State{

    bitboard board = {0};
    U64 pins = 0;
    U64 enpas = 0;
    U64 checkers = 0;
    color next = white;
    color check = NOCOLOR;
    short half = 0;
    short full = 1;
    BYTE cancas = castle<>;

    State() = default;
    State(const State& other){ *this = other; }
    constexpr State& operator=(const State& other){
        std::memcpy(this, &other, sizeof(other));
        return *this;
    }
};

class Board{
public:

    State state;
    State _prev;

    // References to state members
    bitboard& board = state.board;
    U64& pins       = state.pins;
    U64& enpas      = state.enpas;
    U64& checkers   = state.checkers;
    color& next     = state.next;
    color& check    = state.check;
    short& half     = state.half;
    short& full     = state.full;
    BYTE& cancas    = state.cancas;

    // Constructors
    Board();
    Board(const Board&);

    // Assignment
    Board& operator=(const Board&);

    // Newgame/empty board
    void reset();
    void clear();

    // Get color/type info for given square
    color lookupc(const U64) const;
    ptype lookupt(const U64) const;

    // Line b/t squares is empty
    bool clearbt(const square, const square) const;

    // Remove material from board
    void remove();
    void remove(const U64);
    void remove(const U64, const ptype, const color);

    // Place material on board
    void place(const U64, const ptype, const color);

    // Move material from src to dst
    void move(const U64, const U64, const ptype, const color);

    // Update board state with ply
    void update(const ply);

    // Undo most recent update
    void undo();

    // Generate bitboard of legal moves
    U64 legal(const color) const;
    U64 legal(const ptype, const color) const;
    U64 legal(const square, const ptype, const color) const;

    // Generate vector of legal plies
    std::vector<ply> legal_plies(const color);

    // Write ply in Standard Algebraic Notation
    std::string ply2san(const ply) const;

    // String visualization of board
    std::string to_string() const;

    // Determine if ply gives check/mate and set ply members accordingly
    void lookahead(ply& p);

private:

    template<color c>
    void apply(const ply);

    template<ptype pt, color c>
    U64 pseudo(const square) const;

    template<ptype pt, color c>
    U64 pseudo() const;

    template<color c>
    U64 pseudo() const;

    template<color c>
    U64 legal_pawn(const square, const U64, const square, const U64) const;

    template<color c>
    U64 legal_king(const square, const U64, const U64) const;

    template<color c>
    U64 check_filter(const ptype, const square) const;

    template<color c>
    U64 legal_bb(const square, const ptype) const;
};

/**************************************************
            DISAMBIGUATION FUNCTIONS
**************************************************/

namespace disamb{
    U64 pgn(const U64, const U64, ptype, color, const Board&, bool);
    ply uci(std::string, const Board&);
}

/**************************************************
            FORSYTH-EDWARDS NOTATION
**************************************************/
namespace fen{

    constexpr char castles[5] = {'q', 'k', 'Q', 'K'};
    extern const std::regex delim;
    extern const std::regex fmt;
    extern const std::string new_game;

    ptype c2ptype(char);
    char ptype2c(ptype, color);
    void arrange(std::string, Board&);
    void parse(std::string, Board&);
    std::string from_board(const Board&);
}

#endif