#ifndef DECODE_H
#define DECODE_H

#include <iostream>
#include <utility>

#include "types.h"
#include "util.h"

static const ply blank_ply;

class Decoder{
private:
    static ply cply;
    static pname name;
    static int action[2], castle;
    static bool capture, check, mate, queen_rook;
    static ptype promo;

    void decode_name(BYTE);
    void decode_queen_rookmove(BYTE);
    void decode_pawn_promotion(BYTE);

    void decode_check(BYTE);
    void decode_mate(BYTE);
    void decode_castle(BYTE);

    void pawn_ply(BYTE);
    void knight_ply(BYTE);
    void bishop_ply(BYTE);
    void rook_ply(BYTE);
    void queen_ply(BYTE, bool);
    void king_ply(BYTE);

    void reset();
    void set();

public:
    Decoder();
    
    void decode_ply(eply, ply*);

    void decode_name(BYTE, pname*);
    void decode_queen_rookmove(BYTE, bool*);
    void decode_pawn_promotion(BYTE, ptype*);

    void decode_check(BYTE, bool*);
    void decode_mate(BYTE, bool*);
    void decode_castle(BYTE, int*);

    void pawn_ply(BYTE, int*);
    void knight_ply(BYTE, int*);
    void bishop_ply(BYTE, int*);
    void rook_ply(BYTE, int*);
    void queen_ply(BYTE, int*, bool=false);
    void king_ply(BYTE, int*, int*);
};

ply Decoder::cply;
bool Decoder::capture, Decoder::check, Decoder::mate, Decoder::queen_rook;
ptype Decoder::promo;
int Decoder::action[2], Decoder::castle;
pname Decoder::name;

Decoder::Decoder(){ this->reset(); }

void Decoder::reset(){
    this->cply = blank_ply;
    this->name = NONAME;
    for(int i=0;i<2;i++){ this->action[i] = -1; }
    this->castle = 0;
    this->capture = false;
    this->check = false;
    this->mate = false;
    this->queen_rook = false;
    this->promo = pawn;
}

void Decoder::set(){
    this->cply = ply{
        pce{this->name},
        this->action[0],
        this->action[1],
        this->castle,
        this->capture,
        this->check,
        this->mate,
        this->promo
    };
}

void Decoder::decode_ply(eply e, ply *dst){
    this->reset();
    this->decode_name(e.name);
    switch(util::name2type(this->name)){
        case king: this->king_ply(e.action); break;
        case queen: this->decode_queen_rookmove(e.name); this->queen_ply(e.action, this->queen_rook); break;
        case rook: this->rook_ply(e.action); break;
        case bishop: this->bishop_ply(e.action); break;
        case knight: this->knight_ply(e.action); break;
        case pawn: this->decode_pawn_promotion(e.name); this->pawn_ply(e.action); break;
    }
    this->set();
    *dst = this->cply;
}

/*
    PRIVATE FUNCS - MODIFY MEMBERS
*/

void Decoder::decode_name(BYTE name){ this->decode_name(name, &(this->name)); }

void Decoder::decode_queen_rookmove(BYTE name){ this->decode_queen_rookmove(name, &(this->queen_rook)); }

void Decoder::decode_pawn_promotion(BYTE name){ this->decode_pawn_promotion(name, &(this->promo)); }

void Decoder::decode_check(BYTE act){ this->decode_check(act, &(this->check)); }

void Decoder::decode_mate(BYTE act){ this->decode_mate(act, &(this->mate)); }

void Decoder::decode_castle(BYTE act){ this->decode_castle(act, &(this->castle)); }

void Decoder::pawn_ply(BYTE act){ this->pawn_ply(act, this->action); }

void Decoder::knight_ply(BYTE act){ this->knight_ply(act, this->action); }

void Decoder::bishop_ply(BYTE act){ this->bishop_ply(act, this->action); }

void Decoder::rook_ply(BYTE act){ this->rook_ply(act, this->action); }

void Decoder::queen_ply(BYTE act, bool qr){ this->queen_ply(act, this->action, qr); }

void Decoder::king_ply(BYTE act){ this->king_ply(act, this->action, &(this->castle)); }


/*
    PUBLIC FUNCS - MODIFY EXTERNAL ADDRESSES
*/

void Decoder::decode_name(BYTE name, pname *ply_name){ *ply_name = static_cast<pname>(name & ~(7<<5)); }

void Decoder::decode_queen_rookmove(BYTE name, bool *qr){ *qr = 1<<5 & name; }

void Decoder::decode_pawn_promotion(BYTE name, ptype *promo){ *promo = static_cast<ptype>((224 & name)>>5); }

void Decoder::decode_check(BYTE act, bool *chk){ *chk = (bool) act>>6; }

void Decoder::decode_mate(BYTE act, bool *mte){ *mte = (bool) act>>7; }

void Decoder::decode_castle(BYTE act, int *cas){ *cas = (act & 12) == 12 ? 1 - (2 * (act>>4)) : 0; }

void Decoder::pawn_ply(BYTE act, int *dst){
    // 0xpppcmm
    bool cap = act & (1<<5);
    int mov = 7 & act;

    dst[0] = ((act & 4)>>2 ? -1 : 1) * ((act & 3)==3 ? 2 : 1);
    dst[1] = !cap ? 0 : ((act & 1) ? 1 : -1);
}

void Decoder::knight_ply(BYTE act, int *dst){
    // 00000odt
    int step = 1 - 2 * ((act & 4)>>2);
    int skip = 2 - 4 * (act & 1);
    dst[0] = (act & 2) ? step : skip;
    dst[1] = (act & 2) ? skip : step;
}

void Decoder::bishop_ply(BYTE act, int *dst){
    // 000admmm
    int axis = 1 - (2 * (act & 16)>>4);
    int dir = 1 - (2 * (act & 8)>>3);
    int mag = act & 7;
    dst[0] = dir * mag;
    dst[1] = axis * dst[0];
}

void Decoder::rook_ply(BYTE act, int *dst){
    // 000admmm
    bool axis = act & 16;
    int dir = 1 - (2 * (act & 8)>>3);
    int mag = act & 7;
    dst[0] = axis ? dir * mag : 0;
    dst[1] = axis ? 0 : dir * mag;
}

void Decoder::queen_ply(BYTE act, int *dst, bool qr){
    // 000admmm
    if(qr){ this->rook_ply(act, dst); }
    else{ this->bishop_ply(act, dst); }
}

void Decoder::king_ply(BYTE act, int *dst, int *cas){
    // 000cffrr
    int radj = (act & 3);
    int fadj = (act & 12)>>2;
    this->decode_castle(act, cas);
    dst[0] = radj - (3 * (radj>>1));
    dst[1] = *cas ? 2 * *cas : fadj - (3 * fadj>>1);
}

#endif