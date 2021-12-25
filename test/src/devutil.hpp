#include <cstring>

#include <bitset>
#include <iomanip>
#include <iostream>
#include <locale>
#include <string>

#include "chessdb.hpp"

/*
    FUNCTION DECLARATIONS
*/
std::string bar(int);
void bbprint(const U64&, std::string="", char='1');
void bprint(Board&);
void pprint(const ply&, const Board&);
void println(unsigned int=__builtin_LINE());
ply pply(std::string p, color c, const Board& board);
ply pcastle(bool qs, color c, bool check, bool mate, const Board& board);
ply prest(const std::string p, color c, bool check, bool mate, const Board& board);


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
        std::cout << '\n'
                  << header<W>::bar_unit << '\n'
                  << std::setw(W) << std::left
                  << val << '\n'
                  << header<W>::bar_unit << '\n';
    }
};

template<int W>
std::string header<W>::bar_unit = bar(W);

template<int N, int W, bool idx=false>
struct table{
    static std::string bar_unit;
    static int index;

    static void _row(){ std::cout << "|\n"; }
    static void rrow(){ std::cout << "|\r"; }

    template<typename T, typename... Ts>
    static void row(T val, Ts... args){
        if(idx){ std::cout << '|' << std::setw(4) << std::left << table<N, W, idx>::index++; }
        table<N, W>::_row(val, args...);
    }

    template<typename T, typename... Ts>
    static void _row(T val, Ts... args){
        std::cout << '|' << std::setw(W) << std::left << val;
        table<N, W>::_row(args...);
    }

    template<typename T, typename... Ts>
    static void header(T val, Ts... args){
        table<N, W, idx>::sep();
        if(idx){ std::cout << '|' << std::setw(4) << std::left << ""; }
        table<N, W, idx>::_row(val, args...);
        table<N, W, idx>::sep();
        table<N, W, idx>::index = 1;
    }

    template<typename T, typename... Ts>
    static void rrow(T val, Ts... args){
        std::cout << '|' << std::setw(W) << std::left << val;
        table<N, W>::rrow(args...);
    }

    static void prow(const ply& p){
        table<N, W>::row(
            color2s(p.c),
            ptype2s(p.type),
            ptype2s(p.promo),
            coord2s(p.src),
            coord2s(p.dst),
            (p.castle ? (p.castle>0 ? "ks" : "qs") : "__"),
            p.capture,
            p.check,
            p.mate
        );
    }

    static void prrow(const ply& p){
        table<N, W>::rrow(
            color2s(p.c),
            ptype2s(p.type),
            ptype2s(p.promo),
            coord2s(p.src),
            coord2s(p.dst),
            (p.castle ? (p.castle>0 ? "ks" : "qs") : "__"),
            p.capture,
            p.check,
            p.mate
        );
    }

    static void sep(){
        std::string tmp(1, '|');
        for(int i=0; i<N; ++i){ tmp += table<N, W, idx>::bar_unit; }
        tmp += bar(idx ? N+4 : N-1) + '|';
        std::cout << tmp << '\n';
    }

    static void sep(int n){
        std::string tmp(1, '|');
        for(int i=0; i<n; ++i){ tmp += table<N, W, idx>::bar_unit; }
        tmp += bar(idx ? n+4 : n-1) + '|';
        std::cout << tmp << '\n';
    }
};

template<int N, int W, bool idx>
std::string table<N, W, idx>::bar_unit = bar(W);

template<int N, int W, bool idx>
int table<N, W, idx>::index = 1;

/*
    FUNCTION DEFINITIONS
*/

std::string bar(int length){ return std::string(length, '-'); }

void bbprint(const U64& bb, std::string txt, char c){ std::cout << txt << bb2s(bb, c) << '\n'; }

void bprint(Board& board){ std::cout << board.to_string() << '\n'; }

void pprint(const ply& p, const Board& board){
    table<10, 10>::row(
        board.ply2san(p),
        color2s(p.c),
        ptype2s(p.type),
        ptype2s(p.promo),
        coord2s(p.src),
        coord2s(p.dst),
        (p.castle ? (p.castle>0 ? "ks" : "qs") : "__"),
        p.capture,
        p.check,
        p.mate
    );
}

void println(unsigned int ln){ std::cout << ln << '\n'; }

ply pply(std::string p, color c, const Board& board){
    // Parse and remove flags
    bool check = p.back() == '+',
         mate = p.back() == '#';
    if(check || mate){ p.erase(p.size()-1, 1); }
    if(mate){ check = true; }

    bool kingside{!p.compare("O-O")},
         queenside{!p.compare("O-O-O")};

    // Shortcut for castles
    if(kingside | queenside){ return pcastle(queenside, c, check, mate, board); }
    else                    { return prest(p, c, check, mate, board); }
}


ply pcastle(bool qs, color c, bool check, bool mate, const Board& board){
    U64 src = board.board(king, c),
        dst = qs ? (src >> 2) : (src << 2);
    return {src, dst, king, pawn, c, qs ? -1 : 1, false, check, mate};
}

ply prest(const std::string p, color c, bool check, bool mate, const Board& board){
    int f = 0, r = 0;
    square srcsq = 0, dstsq = 0;
    bool capture = false;
    ptype pt = pawn, promo = pawn;

    for(typename std::string::const_reverse_iterator it=p.rbegin(); it!=p.rend(); ++it){
        char ch = *it;
        if(isfile(ch))      { (!f++ ? dstsq : srcsq) += c2file(ch);     }
        else
        if(std::isdigit(ch)){ (!r++ ? dstsq : srcsq) += c2rank(ch) * 8; }
        else
        if(ispiece(ch))     { pt = c2ptype(ch); }
        else
        if(ch == 'x')       { capture = true; }
        else
        if(ch == '=')       { promo = pt; pt = pawn; }
    }
    
    U64 dst = mask(dstsq),
        src = 0;
    if(pt == king)   { src = board.board(king ,c); }
    else
    if(r==2 && f==2) { src = mask(srcsq);  }
    else{
        if(r==2)     { src = rankof(srcsq); }
        else
        if(f==2)     { src = fileof(srcsq); }

        src = disamb::pgn(src, dst, pt, c, board, capture);
    }

    return {src, dst, pt, promo, c, 0, capture, check, mate};
}
