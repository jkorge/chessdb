#ifndef TYPES_H
#define TYPES_H

/**************************
    SHORTHAND TYPES
**************************/
// BYTE type
typedef unsigned char BYTE;

// regex_token_iterators
typedef std::regex_token_iterator<std::string::const_iterator> crtok;
typedef std::regex_token_iterator<std::string::iterator> rtok;

/********************************
    COORDINATE REPRESENTATIONS
********************************/

// Bitboard
typedef uint64_t U64;

// Alias for square numbers
typedef int square;

// Array wrapper
typedef std::array<int, 2> coords;
coords operator+(const coords& x, const coords& y);

// Compass directions (for sliding piece attack rays)
typedef enum direction{E = 0, NE, N, NW, W, SW, S, SE, NODIR = -1} direction;
direction operator++(direction&);
direction operator++(direction&, int);

/**************************
    MATERIAL
**************************/

// Material color
typedef enum color{ white = 1, black = -1, NOCOLOR = 0 } color;
color operator!(const color& c);

// Types of material
typedef enum ptype {pawn = 0, knight, bishop, rook, queen, king, NOTYPE = -1} ptype;
ptype operator++(ptype&);
ptype operator++(ptype&, int);

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

    U64 src, dst;
    color c;
    ptype type,
          promo;
    pname name;
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
          name(NONAME),
          castle(0),
          capture(false),
          check(false),
          mate(false) {}

    // Initialize everything but name
    ply (U64 s, U64 d, ptype pt, ptype po, color col, int cas, bool cap, bool ch, bool m)
        : src(s),
          dst(d),
          c(col),
          type(pt),
          promo(po),
          name(NONAME),
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
          name(NONAME),
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
          name(other.name),
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