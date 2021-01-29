#ifndef DECODE_H
#define DECODE_H

#include "types.h"
#include "board.h"

class Decoder{

    ply missing;                // default construct for missing or elided plies

    eply emissing{255, 255};    // encode missing plies as 11111111 for both piece and action

public:    
    ply decode_ply(eply, ChessBoard&);

    ptype decode_type(BYTE);

    color decode_color(pname);

    pname decode_name(BYTE);

    bool decode_capture(BYTE);

    bool decode_check(BYTE);

    bool decode_mate(BYTE);

    ptype decode_pawn_promotion(BYTE);

    ptype decode_queen_axis(BYTE);

    coords<int> pawn_action(BYTE, color);

    coords<int> knight_action(BYTE);

    coords<int> bishop_action(BYTE);

    coords<int> rook_action(BYTE);

    coords<int> queen_action(BYTE, ptype);

    coords<int> king_action(BYTE);
};

ply Decoder::decode_ply(eply e, ChessBoard& board){

    if(e == this->emissing){ return this->missing; }

    ply p;

    p.name = this->decode_name(e.piece);
    p.type = this->decode_type(e.piece);
    if(p.type == king && (p.name >= 8 & p.name <= 23)){ p.type = queen; }
    p.c = this->decode_color(p.name);

    p.capture = this->decode_capture(e.action);
    p.check = this->decode_check(e.action);
    p.mate = this->decode_mate(e.action);

    if(p.type == pawn){ p.promo = this->decode_pawn_promotion(e.action); }
    else              { p.promo = pawn; }
    
    coords<int> src = rf(board.mat[p.name]),
                dst,
                delta;
    switch(p.type){
        case pawn:   delta = this->pawn_action(e.action, p.c); break;
        case knight: delta = this->knight_action(e.action); break;
        case bishop: delta = this->bishop_action(e.action); break;
        case rook:   delta = this->rook_action(e.action); break;
        case queen:  delta = this->queen_action(e.action, this->decode_queen_axis(e.piece)); break;
        default:     delta = this->king_action(e.action);
    }
    dst = src + delta;
    p.src = mask(src);
    p.dst = mask(dst);

    if(p.type == king){
        switch(delta[1]){
            case  2: p.castle =  1; break;
            case -2: p.castle = -1; break;
            default: p.castle =  0;
        }
    }

    return p;
}

ptype Decoder::decode_type(BYTE _piece){
    switch(static_cast<int>(_piece & 31)){
        case  0:
        case  7:
        case 24:
        case 31: return rook;
        case  1:
        case  6:
        case 25:
        case 30: return knight;
        case  2:
        case  5:
        case 26:
        case 29: return bishop;
        case  3:
        case 27: return queen;
        case  4:
        case 28: return king;
        case  8:
        case  9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23: return static_cast<ptype>((_piece & 224) >> 5);
        default: return NOTYPE;
    }
}


color Decoder::decode_color(pname name){ return name == NONAME ? NOCOLOR : (name > 15 ? black : white); }

pname Decoder::decode_name(BYTE _piece){ return static_cast<pname>(_piece & 31); }

bool Decoder::decode_capture(BYTE _action){ return CAPTURE_ & _action; }

bool Decoder::decode_check(BYTE _action){ return CHECK_ & _action; }

bool Decoder::decode_mate(BYTE _action){ return MATE_ & _action; }

ptype Decoder::decode_pawn_promotion(BYTE _action){ return static_cast<ptype>((_action & 28) >> 2); }

ptype Decoder::decode_queen_axis(BYTE _piece){ return (QAROOK_ & _piece) ? rook : bishop; }

coords<int> Decoder::pawn_action(BYTE _action, color c){
    // 000ppp<rr/ff>
    bool cap = _action & 32,
         m = _action & 2;

    int dr = c * (cap ? 1 : (m ? 2 : 1)),
        df = cap ? (m ? 1 : -1) : 0;

    return {dr, df};
}

coords<int> Decoder::knight_action(BYTE _action){
    // 00000aot
    int o = 1 - (2 * (bool)(_action & 2)),
        t = 2 - (4 * (bool)(_action & 1));
    if(_action & 4){ return {o, t}; }
    else           { return {t, o}; }
}

coords<int> Decoder::bishop_action(BYTE _action){
    // 000admmm
    bool d = _action & 8,
         a = _action & 16;
    int  m = _action & 7;
    if(d ^ a){
        if(d){ return {-m, -m}; }
        else { return {m, -m}; }
    }
    else{
        if(d){ return {-m, m}; }
        else { return {m, m}; }
    }
}

coords<int> Decoder::rook_action(BYTE _action){
    // 000admmm
    int val = (1 - (2 * ((_action & 8) >> 3))) * (_action & 7);
    if(_action & 16){ return {val, 0}; }
    else            { return {0, val}; }
}

coords<int> Decoder::queen_action(BYTE _action, ptype pt){
    // 000admmm
    switch(pt){
        case rook: return this->rook_action(_action);
        default:   return this->bishop_action(_action);
    }
}

coords<int> Decoder::king_action(BYTE _action){
    // 000cffrr
    int dr = _action & 3,
        df = (_action & 12) >> 2;

    if(dr == 2){ dr = -1; }
    if(df == 2){ df = -1; }
    if(_action & 16){ df *= 2; }
    return {dr, df};

}

#endif