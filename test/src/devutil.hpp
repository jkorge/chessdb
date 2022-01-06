#include <cstring>

#include <bitset>
#include <iomanip>
#include <iostream>
#include <locale>
#include <string>

#include "chessdb.hpp"
#include "table.hpp"

std::FILE* string_file(const std::string& str){

#ifdef _WIN32
    std::FILE* fp = std::tmpfile();
    if(fp == nullptr){ throw std::runtime_error("Must be admin to open tmpfile!"); }
    std::fwrite(str.data(), sizeof(char), str.size(), fp);
    std::fseek(fp, 0, SEEK_SET);
    return fp;

#else
    return fmemopen(str.data(), str.size(), "rb");

#endif
}

/*
    FUNCTION DECLARATIONS
*/
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

template<std::size_t N, std::size_t W>
void prow(const ply& p){
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

template<std::size_t N, std::size_t W>
void prrow(const ply& p){
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

/*
    FUNCTION DEFINITIONS
*/

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
