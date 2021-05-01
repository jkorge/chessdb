#ifndef CHESS_H
#define CHESS_H

#include <immintrin.h>
#include <cctype>
#include <cstdint>

#include <iostream>
#include <array>
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

// typedef std::array<int, 2> coords;

typedef enum color{ white = 1, black = -1, NOCOLOR = 0 } color;

typedef enum ptype {pawn = 0, knight, bishop, rook, queen, king, NOTYPE = -1} ptype;

typedef struct ply{

    U64 src, dst;
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

    bool operator==(const ply& other) const{
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

    bool operator!=(const ply& other) const{ return not (*this == other); }
} ply;

typedef uint16_t eply;

typedef struct bitboard : std::array<U64, 15>{
    const U64& operator()(ptype pt, color c) const{ return this->operator[](pt + 6*(c<0)); }
    U64& operator()(ptype pt, color c){ return this->operator[](pt + 6*(c<0)); }

    const U64& operator()(color c) const{ return this->operator[](12 + (c<0)); }
    U64& operator()(color c){ return this->operator[](12 + (c<0)); }

    U64 operator()(ptype pt){ return this->operator[](pt) | this->operator[](pt+6); }

    const U64& operator()() const{ return this->operator[](14); }
    U64& operator()(){ return this->operator[](14); }
} bitboard;

typedef struct coords : std::array<int, 2>{
    coords() : std::array<int, 2>{0} {}
    coords(square x) : std::array<int, 2>{x/8, x%8} {}
    coords(U64 src); // defined below under `INLINE FUNCTIONS`
    coords(int r, int f) : std::array<int, 2>{r, f} {}
    coords operator+(const coords& other) const{ return {this->at(0) + other.at(0), this->at(1) + other.at(1)}; }
} coords;

typedef enum pgntag{
/*
    This is NOT an enum of all PGN-tag keys
    Only those deemed most useful in analyzing games are included here
    See https://www.chessclub.com/help/PGN-spec for more info on PGN specifications
*/

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
    bool operator==(const game& other) const{ return (this->tags == other.tags) && (this->plies == other.plies); }
    bool operator!=(const game& other) const{ return not (*this == other); }
};

struct egame{
    std::vector<uint32_t> tags;
    std::vector<eply> plies;
    egame () {}
    egame (std::vector<uint32_t> _tags, std::vector<eply> _plies) : tags{_tags}, plies{_plies} {}
    egame (std::vector<eply> _plies, std::vector<uint32_t> _tags) : tags{_tags}, plies{_plies} {}
    bool operator==(const egame& other) const{ return (this->tags == other.tags) && (this->plies == other.plies); }
    bool operator!=(const egame& other) const{ return not (*this == other); }
};

/**************************************************
                GLOBAL CONSTANTS
**************************************************/

constexpr std::array<U64, 4> rook_castle{0x0000000000000080LL, 0x0000000000000001LL, 0x8000000000000000LL, 0x0100000000000000LL};

constexpr U64 MAINDIAG_     = 0x8040201008040201,
              ANTIDIAG_     = 0x0102040810204080,
              RANK_         = 0xff,
              FILE_         = 0x0101010101010101,

              ONE_          = 0x01,
              ZERO_         = 0x00,
              ALL_          = 0xffffffffffffffff,

              white_pawns   = 0x000000000000ff00,
              white_knights = 0x0000000000000042,
              white_bishops = 0x0000000000000024,
              white_rooks   = 0x0000000000000081,
              white_queen   = 0x0000000000000008,
              white_king    = 0x0000000000000010,

              black_pawns   = white_pawns   << 40,
              black_knights = white_knights << 56,
              black_bishops = white_bishops << 56,
              black_rooks   = white_rooks   << 56,
              black_queen   = white_queen   << 56,
              black_king    = white_king    << 56,

              white_mat = white_pawns | white_knights | white_bishops | white_rooks | white_queen | white_king,
              black_mat = black_pawns | black_knights | black_bishops | black_rooks | black_queen | black_king,
              occupancy = white_mat | black_mat;

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

extern const std::string pieces;

extern const std::string files;

extern const std::string ranks;

extern const std::string endl;

extern const std::string bviz;

extern const std::string bbviz;

extern std::map<std::string, pgntag> s2tag;

extern std::map<pgntag, std::string> tag2s;

/**************************************************
                INLINE FUNCTIONS
**************************************************/

// Bitscans
inline square bitscan(const U64 src){ return _tzcnt_u64(src); }

inline square bitscanr(const U64 src){ return 63 - _lzcnt_u64(src); }

// Coords constructor - depends on bitscan
inline coords::coords(U64 src) : coords::coords(bitscan(src)) {}

// Flip LS1B
inline void lsbflip(U64& x){ x &= x-1; }

// Square number => U64
inline U64 mask(const square x){ return 1LL << x; }

// Printing utils
inline int bvizidx(square sq){ return 36 + 35*(7 - sq/8) + 4*(sq%8); }

inline int bbvizidx(square sq){ return 2 + 25*(7 - sq/8) + 3*(sq%8); }

// Empty board patterns
inline U64 ray(const square sq, const ptype pt, const int dir){ return rays[pt-2][sq][dir]; }

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

int c2file(char);

int c2rank(char);

ptype c2ptype(char);

char file2c(int);

char rank2c(int);

char ptype2c(ptype);

char color2c(color);

color operator!(const color&);

std::string color2s(color);

std::string coord2s(square);

std::string ptype2s(ptype);

std::string pgndict2s(const pgndict&);

std::string bb2s(U64, char='1');

// Empty board patterns
U64 attack(const square, const ptype, const color=NOCOLOR);

U64 line(const square, const square, bool=false);


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
    BYTE cancas = 0b00001111;

    Board();

    void reset();
    void clear();

    color lookupc(const U64);
    ptype lookupt(const U64);

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
    bool moveable_pin(const U64, const U64, const U64);

    void ppins(U64, const U64, const color, const Board&);

    U64 dpawn(const U64, const U64, ptype, color, const Board&, bool);

    U64 dpiece(const U64, const U64, ptype, color, const Board&);

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
}

#endif