#ifndef CHESS_H
#define CHESS_H

#include <immintrin.h>
#include <cctype>
#include <cstdint>
#include <cstdlib>

#include <array>
#include <random>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

/**************************************************
                    TYPEDEFS
**************************************************/

typedef unsigned char BYTE;

typedef uint64_t U64;

typedef int square;

typedef enum color{ white = 1, black = -1, NOCOLOR = 0 } color;

typedef enum ptype { pawn = 0, knight, bishop, rook, queen, king, NOTYPE = -1 } ptype;

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

    // Intialize nothing
    ply ()
        : src(0),
          dst(0),
          c(NOCOLOR),
          type(pawn),
          promo(pawn),
          castle(0),
          capture(false),
          check(false),
          mate(false) {}

    // Initialize everything
    ply (U64 s, U64 d, ptype pt, ptype po, color col, int cas, bool cap, bool ch, bool m)
        : src(s),
          dst(d),
          c(col),
          type(pt),
          promo(po),
          castle(cas),
          capture(cap),
          check(ch),
          mate(m) {}

    ply (square s, square d, ptype pt, ptype po, color col, int cas, bool cap, bool ch, bool m)
        : src(1ULL << s),
          dst(1ULL << d),
          c(col),
          type(pt),
          promo(po),
          castle(cas),
          capture(cap),
          check(ch),
          mate(m) {}

    // Copy constructor
    ply (const ply& other)
        : src(other.src),
          dst(other.dst),
          c(other.c),
          type(other.type),
          promo(other.promo),
          castle(other.castle),
          capture(other.capture),
          check(other.check),
          mate(other.mate) {}

    // Comparison operators
    constexpr bool operator==(const ply& other) const{
        return (this->src == other.src) &&
               (this->dst == other.dst) &&
               (this->c == other.c) &&
               (this->type == other.type) &&
               (this->promo == other.promo) &&
               (this->castle == other.castle) &&
               (this->capture == other.capture) &&
               (this->check == other.check) &&
               (this->mate == other.mate);
    }

    constexpr bool operator!=(const ply& other) const{ return not (*this == other); }

    // Assignment (implicit assign is deprecated)
    constexpr ply& operator=(const ply& other) = default;
} ply;

typedef uint16_t eply;

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
    coords(const U64 src); // defined below under `INLINE FUNCTIONS`
    coords(const int r, const int f) : std::array<int, 2>{r, f} {}
    coords operator+(const coords& other) const{ return {this->at(0) + other.at(0), this->at(1) + other.at(1)}; }
} coords;

typedef enum pgntag{
    // Seven Tag Roster
    event,
    site,
    date,
    roundn,
    playerw,
    playerb,
    result,

    // Other common (and informative) Keys
    eco,
    fenstr,
    mode,
    time_control,
    termination,

    // Player Rating Keys
    white_elo,
    black_elo,
    white_uscf,
    black_uscf
} pgntag;

struct pgndict : std::map<pgntag, std::string>{
    pgndict () : std::map<pgntag, std::string>{
        {event, std::string()},
        {site, std::string()},
        {date, std::string()},
        {roundn, std::string()},
        {playerw, std::string()},
        {playerb, std::string()},
        {result, std::string()},
        {eco, std::string()},
        {fenstr, std::string()},
        {mode, std::string()},
        {time_control, std::string()},
        {termination, std::string()},
        {white_elo, std::string()},
        {white_uscf, std::string()},
        {black_elo, std::string()},
        {black_uscf, std::string()}
    } {}

    void reset(){ for(std::map<pgntag, std::string>::iterator it=this->begin(); it!=this->end(); ++it){ it->second.clear(); } }
};

struct game{
    pgndict tags;
    std::vector<ply> plies;

    game () {}
    game (pgndict _tags, std::vector<ply> _plies) : tags{_tags}, plies{_plies} {}
    game (std::vector<ply> _plies, pgndict _tags) : tags{_tags}, plies{_plies} {}
    ~game ();
    bool operator==(const game& other) const{ return (this->tags == other.tags) && (this->plies == other.plies); }
    bool operator!=(const game& other) const{ return not (*this == other); }
};

struct Magic{
    U64 mask, magic;
    U64* attacks;
    uint32_t shift;

    int index(U64 occ) const{ return ((occ & this->mask) * this->magic) >> this->shift; }

    U64 value(U64 occ) const{ return this->attacks[this->index(occ)]; }
};

/**************************************************
                GLOBAL CONSTANTS
**************************************************/

constexpr std::array<U64, 4> rook_castle{0x0000000000000080LL, 0x0000000000000001LL, 0x8000000000000000LL, 0x0100000000000000LL};

// Board axes
constexpr U64 MAINDIAG_     = 0x8040201008040201,
              ANTIDIAG_     = 0x0102040810204080,
              RANK_         = 0xff,
              FILE_         = 0x0101010101010101,

// A1, H8, All squares
              ONE_          = 0x01,
              ZERO_         = 0x00,
              ALL_          = 0xffffffffffffffff,

// White starting coordinates
              white_pawns   = 0x000000000000ff00,
              white_knights = 0x0000000000000042,
              white_bishops = 0x0000000000000024,
              white_rooks   = 0x0000000000000081,
              white_queen   = 0x0000000000000008,
              white_king    = 0x0000000000000010,

// Black starting coordinates
              black_pawns   = white_pawns   << 40,
              black_knights = white_knights << 56,
              black_bishops = white_bishops << 56,
              black_rooks   = white_rooks   << 56,
              black_queen   = white_queen   << 56,
              black_king    = white_king    << 56,

// Occupancies
              white_mat = white_pawns | white_knights | white_bishops | white_rooks | white_queen | white_king,
              black_mat = black_pawns | black_knights | black_bishops | black_rooks | black_queen | black_king,
              occupancy =   white_mat | black_mat;

// Collect start coords and occupancies for newgame bitboard
constexpr bitboard newgame = {white_pawns, white_knights, white_bishops, white_rooks, white_queen, white_king,
                              black_pawns, black_knights, black_bishops, black_rooks, black_queen, black_king,
                              white_mat, black_mat, occupancy};

constexpr char bcell[]{"[  ]"},
               bbcell[]{" . "};

extern const std::array<U64, 64> rmasks;

extern const std::array<U64, 64> fmasks;

extern const std::array<U64, 64> dmasks;

extern const std::array<U64, 64> amasks;

extern const std::array<U64, 64> wpattack;

extern const std::array<U64, 64> bpattack;

extern const std::array<U64, 64> nattack;

extern const std::array<U64, 64> battack;

extern const std::array<U64, 64> rattack;

extern const std::array<U64, 64> qattack;

extern const std::array<U64, 64> kattack;

extern const std::array<std::array<U64, 64>, 64> axes;

extern const std::array<std::array<U64, 64>, 64> lines;

extern const std::array<std::array<std::array<U64, 8>, 64>, 3> rays;

extern const std::array<U64, 64> battackm;

extern const std::array<U64, 64> rattackm;

extern const std::string pieces;

extern const std::string files;

extern const std::string ranks;

extern const std::string endl;

extern const std::string bviz;

extern const std::string bbviz;

extern std::map<std::string, pgntag> s2tag;

extern std::map<pgntag, std::string> tag2s;

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

// Flip LS1B
constexpr void lsbflip(U64& x){ x &= x-1; }

// Square number => U64
constexpr U64 mask(const square x){ return 1LL << x; }

// Printing utils
constexpr int bvizidx(square sq){ return 36 + 35*(7 - sq/8) + 4*(sq%8); }

constexpr int bbvizidx(square sq){ return 2 + 25*(7 - sq/8) + 3*(sq%8); }

/**************************************************
                INLINE FUNCTIONS
**************************************************/

// Bitscan Foward - Index of LS1B (64 if src == 0)
inline square bitscan(const U64 src){ return _tzcnt_u64(src); }

// Bitscan Reverse - Index of MS1B (-1 if src == 0)
inline square bitscanr(const U64 src){ return 63 - _lzcnt_u64(src); }

// Count number of 1 bits
inline int popcnt(const U64 x){ return _mm_popcnt_u64(x); }

// Coords constructor - depends on bitscan
inline coords::coords(const U64 src) : coords::coords(bitscan(src)) {}

// Bitscan, flip ls1b, return bitscan result
inline square lsbpop(U64& x){ square s = bitscan(x); lsbflip(x); return s; }

// Empty board patterns for sliding pieces
inline U64 ray(const square sq, const ptype pt, const int dir){ return rays[pt-2][sq][dir]; }

// Board axes
inline U64 axis(const square sq1, const square sq2){ return axes[sq1][sq2]; }

inline U64 rank(const square sq){ return rmasks[sq]; }

inline U64 file(const square sq){ return fmasks[sq]; }

inline U64 diag(const square sq){ return dmasks[sq]; }

inline U64 adag(const square sq){ return amasks[sq]; }

/**************************************************
                FUNCTION DECLARATIONS
**************************************************/

bool ispiece(char);

bool isfile(char);

// Convert to string
std::string ptype2s(ptype);

std::string color2s(color);

std::string coord2s(square);

std::string coord2s(U64);

std::string pgndict2s(const pgndict&);

std::string bb2s(U64, char='1');

// Line between two squares (optionally inclusive)
U64 line(const square, const square, bool=false);

// Empty board patterns
U64 slide_atk(const square, const ptype, const U64);

U64 attack(const square, const ptype, const color=NOCOLOR);

// Sliding attack masks (exludes edge squares)
U64 attackm(const square, const ptype);

// RNG for magic bitboard initialization
U64 random_magic();

// Intialize magic bitboards
void magic_init(const ptype, U64*, Magic*);

/**************************************************
                    BOARD CLASS
**************************************************/

class Board{
public:
    
    bitboard board = {0};
    U64 pins = 0,
        enpas = 0;
    std::unordered_map<U64, ptype> checkers;
    color next = white,
          check = NOCOLOR;
    short half{0},
          full{1};
    BYTE cancas = 0b00001111;

    Board();

    void reset();
    void clear();

    color lookupc(const U64) const;
    ptype lookupt(const U64) const;

    void remove();
    void remove(const U64);
    void remove(const U64, const ptype, const color);

    void place(const U64, const ptype, const color);

    void move(const U64, const U64, const ptype, const color);

    bool clearbt(const square, const square) const;

    void update(const ply);

    void search_pins();

    void search_checks(color);

    U64 legal(bool=false) const;
    U64 legal(color, bool=false) const;
    U64 legal(ptype, color, bool=false) const;
    U64 legal(square, ptype, color, bool=false) const;

    std::vector<ply> legal_plies() const;
    std::vector<ply> legal_plies(color) const;
    std::vector<ply> legal_plies(ptype, color) const;
    std::vector<ply> legal_plies(square, ptype, color) const;

    std::string ply2san(const ply) const;
    std::string to_string();
};

/**************************************************
            DISAMBIGUATION FUNCTIONS
**************************************************/

namespace disamb{
    U64 pgn(const U64, const U64, ptype, color, const Board&, bool);
}

/**************************************************
            FORSYTH-EDWARDS NOTATION
**************************************************/
namespace fen{

    constexpr char castles[5] = {'q', 'k', 'Q', 'K'};

    extern const std::regex delim;

    extern const std::string new_game;

    ptype c2type(char);

    void arrange(std::string, Board&);

    void parse(std::string, Board&);

    std::string from_board(const Board&);
}

#endif