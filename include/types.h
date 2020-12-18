#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <map>
#include <regex>


/**************************
    SHORTHAND TYPES
**************************/
// BYTE type
typedef unsigned char BYTE;

// Integer types
typedef unsigned short int ushort;
typedef unsigned long int ulong;
typedef unsigned long long int ullong;

// Fixed width
typedef uint64_t U64;

// String
typedef std::string str;

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
constexpr U64 A1            = C64(0x00);
constexpr U64 H8            = C64(0x8000000000000000);

#undef C64

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

    coords (T r, T f) : _rf{r,f} {}
    coords (T* _rf) : _rf{_rf} {}
    T operator[](int i) const{ return _rf[i]; }
};

namespace {
    // Least significant 1-bit
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

/**************************
    PLY
**************************/

struct ply{

    color c;
    ptype type, promo;
    U64 src, dst;

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

    // Initialize everything
    ply (int s, int d, ptype pt, ptype po, color col, int cas, bool cap, bool ch, bool m)
        : src(1ULL << s),
          dst(1ULL << d),
          type(pt),
          promo(po),
          c(col),
          castle(cas),
          capture(cap),
          check(ch),
          mate(m) {}
};

/**************************
    ENCODED PLY
**************************/

struct eply{
    BYTE piece, action;

    // Constructors
    eply () : piece{0}, action{0} {}
    eply (BYTE p, BYTE a) : piece{p}, action{a} {}
};


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
        {event, str()},
        {site, str()},
        {date, str()},
        {round, str()},
        {playerw, str()},
        {playerb, str()},
        {result, str()},
        {eco, str()},
        {fenstr, str()},
        {mode, str()},
        {time_control, str()},
        {termination, str()},
        {white_elo, str()},
        {white_uscf, str()},
        {black_elo, str()},
        {black_uscf, str()}
    } {}
};

#endif