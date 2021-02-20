#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <regex>


/**************************
    SHORTHAND TYPES
**************************/
// BYTE type
typedef unsigned char BYTE;

// Fixed width
typedef uint64_t U64;

// Alias for square numbers
typedef int square;

// regex_token_iterators
typedef std::regex_token_iterator<std::string::const_iterator> crtok;
typedef std::regex_token_iterator<std::string::iterator> rtok;

/**************************
    CONSTANTS
**************************/

#define C64(constantU64) constantU64##ULL

constexpr U64 DEBRUIJN64_   = C64(0x03f79d71b4cb0a89);
constexpr U64 MAINDIAG_     = C64(0x8040201008040201);
constexpr U64 ANTIDIAG_     = C64(0x0102040810204080);
constexpr U64 ALL_          = C64(0xffffffffffffffff);
constexpr U64 RANK_         = C64(0xff);
constexpr U64 FILE_         = C64(0x0101010101010101);
constexpr U64 ONE_          = C64(0x01);
constexpr U64 ZERO_         = C64(0x00);
constexpr U64 A1            = C64(0x01);
constexpr U64 H8            = C64(0x8000000000000000);

#undef C64

const BYTE CAPTURE_   {1<<5};
const BYTE CHECK_     {1<<6};
const BYTE MATE_      {1<<7};
const BYTE QABISHOP_  {0};
const BYTE QAROOK_    {1<<5};

constexpr int i64[64] = {
     0, 47,  1, 56, 48, 27,  2, 60,
    57, 49, 41, 37, 28, 16,  3, 61,
    54, 58, 35, 52, 50, 42, 21, 44,
    38, 32, 29, 23, 17, 11,  4, 62,
    46, 55, 26, 59, 40, 36, 15, 53,
    34, 51, 20, 43, 31, 22, 10, 45,
    25, 39, 14, 33, 19, 30,  9, 24,
    13, 18,  8, 12,  7,  6,  5, 63
};

/********************************
    COORDINATE REPRESENTATIONS
********************************/

template<typename T=int>
struct coords{
    T _rf[2];

    coords () {}
    coords (T r, T f) : _rf{r,f} {}
    coords (T* _rf) : _rf{_rf} {}
    T& operator[](int i) { return _rf[i]; }
    const T& operator[](int i) const{ return _rf[i]; }
    coords<T> operator+(const coords<T>& other){ return {this->_rf[0] + other[0], this->_rf[1] + other[1]}; }
};

namespace {
    // Index of least significant 1-bit (aka square number)
    inline square bitscan_(const U64& src){ return i64[((src ^ (src-1)) * DEBRUIJN64_) >> 58]; }

    // sq => sq
    inline square sq_(const square& src){ return src; }

    // mask => sq
    inline square sq_(const U64& src){ return bitscan_(src); }

    // coords => sq
    template<typename T>
    inline square sq_(const coords<T>& src){ return (8*src[0]) + src[1]; }

    // sq => mask
    inline U64 mask_(const square& src){ return ONE_ << src; }

    // mask => mask
    inline U64 mask_(const U64& src){ return src; }

    // coords => mask
    template<typename T>
    inline U64 mask_(const coords<T>& src){ return mask_(sq_(src)); }

    // sq => coords
    inline coords<int> rf_(const square& src){ return {src/8, src%8}; }

    // mask => coords
    inline coords<int> rf_(const U64& src){ return rf_(sq_(src)); }

    // coords => coords
    template<typename T>
    inline coords<T> rf_(const coords<T>& src){ return src; }
}

// Generic functions to route to preceding
template<typename T>
inline square sq(T&& src){ return sq_(src); }

template<typename T>
inline U64 mask(T&& src){ return mask_(src); }

template<typename T>
inline coords<int> rf(T&& src){ return rf_(src); }

template<typename T>
inline square bitscan(const T& src){ return bitscan_(mask(src)); }

/**************************
    MATERIAL
**************************/

// Material color
typedef enum color{ white = 1, black = -1, NOCOLOR = 0 } color;
color operator!(const color& c){ return static_cast<color>(black * c); }

// Types of material
typedef enum ptype {pawn = 0, knight, bishop, rook, queen, king, NOTYPE = -1} ptype;

// Piece names
typedef enum pname{
    white_rook_a = 0,
    white_knight_b,
    white_bishop_c,
    white_queen_d,
    white_king_e,
    white_bishop_f,
    white_knight_g,
    white_rook_h,

    white_pawn_a,
    white_pawn_b,
    white_pawn_c,
    white_pawn_d,
    white_pawn_e,
    white_pawn_f,
    white_pawn_g,
    white_pawn_h,

    black_pawn_a,
    black_pawn_b,
    black_pawn_c,
    black_pawn_d,
    black_pawn_e,
    black_pawn_f,
    black_pawn_g,
    black_pawn_h,

    black_rook_a,
    black_knight_b,
    black_bishop_c,
    black_queen_d,
    black_king_e,
    black_bishop_f,
    black_knight_g,
    black_rook_h,

    NONAME = -1
} pname;

/**************************
    PLY
**************************/

struct ply{

    color c;
    ptype type, promo;
    pname name;
    U64 src, dst;

    int castle;             // 0 => No castle; 1 => kingside; -1 => queenside
    bool capture;
    bool check;
    bool mate;

    /*
        CONSTRUCTORS
    */

    // Intialize nothing
    ply ()
        : src(0),
          dst(0),
          name(NONAME),
          type(pawn),
          promo(pawn),
          c(NOCOLOR),
          castle(0),
          capture(false),
          check(false),
          mate(false) {}

    // Initialize everything but name
    ply (U64 s, U64 d, ptype pt, ptype po, color col, int cas, bool cap, bool ch, bool m)
        : src(s),
          dst(d),
          type(pt),
          promo(po),
          c(col),
          castle(cas),
          capture(cap),
          check(ch),
          mate(m) {}

    ply (square s, square d, ptype pt, ptype po, color col, int cas, bool cap, bool ch, bool m)
        : src(1ULL << s),
          dst(1ULL << d),
          type(pt),
          promo(po),
          c(col),
          castle(cas),
          capture(cap),
          check(ch),
          mate(m) {}

    ply (const ply& other)
        : src(other.src),
          dst(other.dst),
          name(other.name),
          type(other.type),
          promo(other.promo),
          c(other.c),
          castle(other.castle),
          capture(other.capture),
          check(other.check),
          mate(other.mate) {}

    bool operator==(const ply& other) const{
        return (this->c == other.c) &&
               (this->type == other.type) &&
               (this->promo == other.promo) &&
               (this->name == other.name) &&
               (this->src == other.src) &&
               (this->dst == other.dst) &&
               (this->castle == other.castle) &&
               (this->capture == other.capture) &&
               (this->check == other.check) &&
               (this->mate == other.mate);
    }

    bool operator!=(const ply& other) const{ return not (*this == other); }
};

/**************************
    ENCODED PLY
**************************/

typedef uint16_t eply;


/**************************
    PGN TAGS
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
        {round, std::string()},
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


/**************************
    GAME DATA
**************************/

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

#endif