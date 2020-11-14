#ifndef ENCODE_H
#define ENCODE_H

#include <utility>
#include <stdlib.h>
#include <cctype>
#include <vector>
#include <unordered_set>

#include "types.h"
#include "util.h"

static const eply blank_eply;   // name and action default to 0

class Encoder {
private:
    static eply cply;
    static BYTE name, action;

    void encode_name(pname);
    void encode_queen_rookmove(int*, int*);
    void encode_pawn_promotion(ptype);

    void encode_capture(bool);
    void encode_check(bool);
    void encode_mate(bool);
    
    void pawn_ply(int*, int*);
    void knight_ply(int*, int*);
    void bishop_ply(int*, int*);
    void rook_ply(int*, int*);
    void queen_ply(int*, int*);
    void king_ply(int*, int*);

    void reset();
    void set();

public:
    Encoder();

    void encode_game(game, egame*);
    void encode_turn(turn, eturn*);
    void encode_ply(ply, eply*);

    // Overloads of private funcs - requires address(es) to store results
    void encode_name(pname, BYTE*);
    void encode_queen_rookmove(int*, int*, BYTE*);
    void encode_pawn_promotion(ptype, BYTE*);

    void encode_capture(bool, BYTE*);
    void encode_check(bool, BYTE*);
    void encode_mate(bool, BYTE*);

    void pawn_ply(int*, int*, BYTE*, bool=false);
    void knight_ply(int*, int*, BYTE*);
    void bishop_ply(int*, int*, BYTE*);
    void rook_ply(int*, int*, BYTE*);
    void queen_ply(int*, int*, BYTE*);
    void king_ply(int*, int*, BYTE*);

    void encode_header(std::vector<pgndict>&, std::map<int, std::string>&);
};

eply Encoder::cply;
BYTE Encoder::name, Encoder::action;

Encoder::Encoder(){ this->reset(); }

void Encoder::reset(){
    this->cply = blank_eply;
    this->name = 0;
    this->action = 0;
}

void Encoder::set(){
    this->cply = eply{
        this->name,
        this->action
    };
}

void Encoder::encode_game(game g, egame *dst){
    for(int i=0; i<g.turns.size(); i++){
        eturn et;
        this->encode_turn(g[i], &et);
        (*dst).append(et);
    }
}

void Encoder::encode_turn(turn t, eturn *dst){
    this->encode_ply(t.white, &(dst->white));
    if(t.black.piece.name != NONAME){ this->encode_ply(t.black, &(dst->black)); }
}

void Encoder::encode_ply(ply p, eply *dst){
    this->reset();

    // Piece name
    this->encode_name(p.piece.name);

    // Capture, check, and mate flags
    this->encode_capture(p.capture);
    this->encode_check(p.check);
    this->encode_mate(p.mate);

    // Piece movement
    int src[]{p.piece.rank, p.piece.file};
    switch(p.piece.type){
        case king: this->king_ply(src, p.dst); break;
        case queen: this->encode_queen_rookmove(src, p.dst); this->queen_ply(src, p.dst); break;
        case rook: this->rook_ply(src, p.dst); break;
        case bishop: this->bishop_ply(src, p.dst); break;
        case knight: this->knight_ply(src, p.dst); break;
        case pawn: this->encode_pawn_promotion(p.promo); this->pawn_ply(src, p.dst); break;
    }

    this->set();
    *dst = this->cply;
}

/*
    PRIVATE FUNCS - MODIFY MEMBERS
*/

void Encoder::encode_name(pname p){ this->encode_name(p, &(this->name)); }

void Encoder::encode_queen_rookmove(int *from, int *to){ this->encode_queen_rookmove(from, to, &(this->name)); }

void Encoder::encode_pawn_promotion(ptype promo){ this->encode_pawn_promotion(promo, &(this->name)); }

void Encoder::encode_capture(bool cap){ this->encode_capture(cap, &(this->action)); }

void Encoder::encode_check(bool chk){ this->encode_check(chk, &(this->action)); }

void Encoder::encode_mate(bool mte){ this->encode_mate(mte, &(this->action)); }

void Encoder::pawn_ply(int *from, int *to){ this->pawn_ply(from, to, &(this->action), this->action & 1<<5); }

void Encoder::knight_ply(int *from, int *to){ this->knight_ply(from, to, &(this->action)); }

void Encoder::bishop_ply(int *from, int *to){ this->bishop_ply(from, to, &(this->action)); }

void Encoder::rook_ply(int *from, int *to){ this->rook_ply(from, to, &(this->action)); }

void Encoder::queen_ply(int *from, int *to){ this->queen_ply(from, to, &(this->action)); }

void Encoder::king_ply(int *from, int *to){ this->king_ply(from, to, &(this->action)); }


/*
    PUBLIC FUNCS - MODIFY PROVIDED ADDRESSES
*/

void Encoder::encode_name(pname p, BYTE *name){ *name = static_cast<BYTE>(p); }

void Encoder::encode_queen_rookmove(int *from, int *to, BYTE *name){ *name |= ((from[0]==to[0]) | (from[1]==to[1])) ? 1<<5 : 0; }

void Encoder::encode_pawn_promotion(ptype promo, BYTE *name){ *name |= static_cast<BYTE>(promo)<<5; }

void Encoder::encode_capture(bool cap, BYTE *act){ *act |= cap ? 1<<5 : 0; }

void Encoder::encode_check(bool chk, BYTE *act){ *act |= chk ? 1<<6 : 0; }

void Encoder::encode_mate(bool mte, BYTE *act){ *act |= mte ? 1<<7 : 0; }

void Encoder::pawn_ply(int *from, int *to, BYTE *act, bool cap){
    // 00x00cmm
    int delta[]{to[0] - from[0], to[1] - from[1]};
    *act |= (delta[0] < 0) ? 4 : 0;

    if(cap){ *act |= (delta[1]>0) ? 1 : 0; }
    else{ *act |= abs(delta[0])==2 ? 3 : 2; }
};

void Encoder::knight_ply(int *from, int *to, BYTE *act){
    // 00000odt
    int delta[]{to[0] - from[0], to[1] - from[1]};
    bool r2 = abs(delta[0]) == 2,
         rneg = delta[0] < 0,
         fneg = delta[1] < 0;

    *act |= r2 ? 0 : 1<<1;
    *act |= ((r2 & rneg) | (!r2 & fneg)) ? 1 : 0;
    *act |= ((r2 & fneg) | (!r2 & rneg)) ? 1<<2 : 0;
};

void Encoder::bishop_ply(int *from, int *to, BYTE *act){
    // 000admmm
    int delta[]{to[0] - from[0], to[1] - from[1]};

    *act |= ((delta[0] ^ delta[1]) < 0) ? 1<<4 : 0;
    *act |= (delta[0] < 0) ? 1<<3 : 0;
    *act |= abs(delta[0]);
};

void Encoder::rook_ply(int *from, int *to, BYTE *act){
    // 000admmm
    int delta[]{to[0] - from[0], to[1] - from[1]};

    *act |= (!delta[1]) ? ((1<<4) | abs(delta[0])) : abs(delta[1]);
    *act |= ((delta[0] < 0) | (delta[1] < 0)) ? 1<<3 : 0;
};

void Encoder::queen_ply(int *from, int *to, BYTE *act){
    // 000admmm
    if((to[0]==from[0]) | (to[1]==from[1])){ this->rook_ply(from, to, act); }
    else{ this->bishop_ply(from, to, act); }
};

void Encoder::king_ply(int *from, int *to, BYTE *act){
    // 000cffrr
    int delta[]{to[0] - from[0], to[1] - from[1]};
    bool cas = abs(delta[1]) == 2;
    bool fneg = delta[1] < 0;

    *act |= (delta[0] < 0) ? 2 : delta[0];
    *act |= (fneg ? 2 : delta[1])<<2;
    *act |= cas ? (fneg ? 5<<2 : 1<<2) : 0;
};


void Encoder::encode_header(std::vector<pgndict> &tags, std::map<int,std::string> &dst){

    // First value ALWAYS
    dst[0] = "\0";

    std::unordered_set<std::string> vals;
    int i = 1;

    // Collect all strings from pgndicts into unorderd_set
    for(std::vector<pgndict>::iterator it=tags.begin(); it!=tags.end(); ++it){
        for(pgndict::iterator jt=(*it).begin(); jt!=(*it).end(); ++jt){
            if(!(jt->second).compare("\0")){ continue; }
            vals.insert(jt->second);
        }
    }

    // Enumerate them
    for(std::unordered_set<std::string>::iterator it=vals.begin(); it!=vals.end(); ++it){
        dst[i] = *it;
    }
}


#endif