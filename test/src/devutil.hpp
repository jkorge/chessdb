#include <string>
#include <locale>
#include <iostream>
#include <bitset>

#include "util.hpp"
#include "board.hpp"
#include "disamb.hpp"

/*
    FUNCTION DECLARATIONS
*/
std::string bar(int);
void bbprint(const U64&, std::string="", char='1');
void bprint(const ChessBoard&);
void lprint(ptype, color, const ChessBoard&);
void lprint(color c, const ChessBoard& board);
void lprint(const ChessBoard& board);
void pprint(const ply&, const ChessBoard&);
ply pply(std::string p, color c, const ChessBoard& board);
ply pcastle(bool qs, color c, bool check, bool mate, const ChessBoard& board);
ply prest(const std::string& p, color c, bool check, bool mate, const ChessBoard& board);

/*
    STRUCTS, TYPEDEFS, ETC.
*/

typedef std::bitset<64> bs;

struct comma_sep : std::numpunct<char> { string_type do_grouping() const{ return "\3"; } };

template<int W>
struct header{
    static std::string bar_unit;

    template<typename T>
    static void print(T val){
        std::cout << header<W>::bar_unit << '\n'
                  << std::setw(W) << std::left
                  << val << '\n'
                  << header<W>::bar_unit << '\n';
    }
};

template<int W>
struct table{
    static std::string bar_unit;

    static void  row(){ std::cout << '|' << '\n'; }
    static void rrow(){ std::cout << '|' << '\r'; }

    template<typename T, typename... Ts>
    static void row(T val, Ts... args){
        std::cout << '|' << std::setw(W) << std::left << val;
        table<W>::row(args...);
    }

    template<typename T, typename... Ts>
    static void rrow(T val, Ts... args){
        std::cout << '|' << std::setw(W) << std::left << val;
        table<W>::rrow(args...);
    }

    static void prow(const ply& p){
        table<W>::row(
            util::repr::color2s(p.c),
            util::repr::ptype2s(p.type),
            util::repr::ptype2s(p.promo),
            util::repr::coord2s(p.src),
            util::repr::coord2s(p.dst),
            (p.castle ? (p.castle>0 ? "ks" : "qs") : "__"),
            p.capture,
            p.check,
            p.mate
        );
    }

    static void prrow(const ply& p){
        table<W>::rrow(
            util::repr::color2s(p.c),
            util::repr::ptype2s(p.type),
            util::repr::ptype2s(p.promo),
            util::repr::coord2s(p.src),
            util::repr::coord2s(p.dst),
            (p.castle ? (p.castle>0 ? "ks" : "qs") : "__"),
            p.capture,
            p.check,
            p.mate
        );
    }

    static void sep(int n){
        std::string tmp(1, '|');
        for(int i=0; i<n; ++i){ tmp += table<W>::bar_unit; }
        tmp += bar(n-1) + '|';
        std::cout << tmp << '\n';
    }
};

template<int W>
std::string table<W>::bar_unit = bar(W);

template<int W>
std::string header<W>::bar_unit = bar(W);

/*
    FUNCTION DEFINITIONS
*/

std::string bar(int length){ return std::string(length, '-'); }

void bbprint(const U64& bb, std::string txt, char c){
    std::cout << txt
              << util::bitboard::odisplay(bb, c)
              << '\n';
}

void bprint(const ChessBoard& board){ std::cout << board.display() << '\n'; }

void lprint(ptype pt, color c, const ChessBoard& board){
    U64 res = board.legal(pt, c);
    if(res){
        bbprint(
            res,
            util::repr::color2s(c) + " " + util::repr::ptype2s(pt),
            util::fen::ptype2c(pt, c)
        );
    }
}

void lprint(color c, const ChessBoard& board){ for(int j=pawn; j<=king; ++j){ lprint(static_cast<ptype>(j), c, board); } }

void lprint(const ChessBoard& board){ for(int i=white; i>=black; i-=2){ lprint(static_cast<color>(i), board); } }

void pprint(const ply& p, const ChessBoard& board){
    table<10>::row(
        board.ply2san(p),
        util::repr::color2s(p.c),
        util::repr::ptype2s(p.type),
        util::repr::ptype2s(p.promo),
        util::repr::coord2s(p.src),
        util::repr::coord2s(p.dst),
        (p.castle ? (p.castle>0 ? "ks" : "qs") : "__"),
        p.capture,
        p.check,
        p.mate
    );
}

ply pply(std::string p, color c, const ChessBoard& board){

    // Parse and remove flags
    bool check = p.back() == '+',
         mate = p.back() == '#';
    if(check || mate){ p.erase(p.size()-1, 1); }

    bool kingside{!p.compare("O-O")},
         queenside{!p.compare("O-O-O")};

    // Shortcut for castles
    if(kingside | queenside){ return pcastle(queenside, c, check, mate, board); }
    else                    { return prest(p, c, check, mate, board); }
}


ply pcastle(bool qs, color c, bool check, bool mate, const ChessBoard& board){
    U64 src = board.board(king, c),
        dst = qs ? (src >> 2) : (src << 2);
    return {src, dst, king, pawn, c, qs ? -1 : 1, false, check, mate};
}


ply prest(const std::string& p, color c, bool check, bool mate, const ChessBoard& board){
    int f = 0, r = 0;
    square srcsq = 0, dstsq = 0;
    bool capture = false;
    ptype pt = pawn, promo = pawn;

    for(std::string::const_reverse_iterator it=p.rbegin(); it!=p.rend(); ++it){
        char ch = *it;
        if(util::transform::isfile(ch)) { (!f++ ? dstsq : srcsq) += util::repr::c2file(ch);     }
        else
        if(std::isdigit(ch))            { (!r++ ? dstsq : srcsq) += util::repr::c2rank(ch) * 8; }
        else
        if(util::transform::ispiece(ch)){ pt = util::repr::c2ptype(ch); }
        else
        if(ch == 'x')                   { capture = true; }
        else
        if(ch == '=')                   { promo = pt; pt = pawn; }
    }

    U64 dst = util::transform::mask(dstsq),
        src = 0;
    if(r==2 && f==2) { src = util::transform::mask(srcsq);  }
    else{
        if(r==2)     { src = util::bitboard::rmasks[srcsq]; }
        else
        if(f==2)     { src = util::bitboard::fmasks[srcsq]; }
        Disamb disamb;
        src = disamb.pgn(src, dst, pt, c, board, capture);
    }

    return {src, dst, pt, promo, c, 0, capture, check, mate};
}

