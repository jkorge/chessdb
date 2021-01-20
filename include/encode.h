#ifndef ENCODE_H
#define ENCODE_H

#include "types.h"

class Encoder {
public:

    eply encode_ply(const ply&);

    BYTE encode_piece(const ply&);

    BYTE encode_action(const ply&);

    BYTE encode_mat(ptype, color);

    BYTE encode_capture(bool);

    BYTE encode_check(bool);

    BYTE encode_mate(bool);

    BYTE encode_pawn_promotion(ptype, bool=true);

    template<typename T>
    BYTE encode_queen_axis(const coords<T>&, const coords<T>&);

    template<typename T>
    BYTE pawn_action(const coords<T>&, const coords<T>&, bool, ptype);

    template<typename T>
    BYTE knight_action(const coords<T>&, const coords<T>&);

    template<typename T>
    BYTE bishop_action(const coords<T>&, const coords<T>&);

    template<typename T>
    BYTE rook_action(const coords<T>&, const coords<T>&);

    template<typename T>
    BYTE queen_action(const coords<T>&, const coords<T>&);

    template<typename T>
    BYTE king_action(const coords<T>&, const coords<T>&);
};

eply Encoder::encode_ply(const ply& p){ return eply(this->encode_piece(p), this->encode_action(p)); }

BYTE Encoder::encode_piece(const ply& p){
    BYTE _piece = static_cast<BYTE>(p.name);
    // if(p.type == pawn && p.promo){ _piece |= this->encode_pawn_promotion(p.promo); }
    if(p.type != pawn && (p.name >= 8 & p.name <= 23)){ _piece |= this->encode_pawn_promotion(p.type); }
    if(p.type == queen)                               { _piece |= this->encode_queen_axis(rf(p.src), rf(p.dst)); }
    return _piece;
}

BYTE Encoder::encode_action(const ply& p){
    coords<int> src = rf(p.src),
                dst = rf(p.dst);

    BYTE _action = this->encode_capture(p.capture) | this->encode_check(p.check) | this->encode_mate(p.mate);
    switch(p.type){
        case pawn:   _action |= this->pawn_action(src, dst, p.capture, p.promo); break;
        case knight: _action |= this->knight_action(src, dst);                   break;
        case bishop: _action |= this->bishop_action(src, dst);                   break;
        case rook:   _action |= this->rook_action(src, dst);                     break;
        case queen:  _action |= this->queen_action(src, dst);                    break;
        default:     _action |= this->king_action(src, dst);
    }
    return _action;
}

BYTE Encoder::encode_capture(bool cap){ return cap ? CAPTURE_ : 0; }

BYTE Encoder::encode_check(bool chk){ return chk ? CHECK_ : 0; }

BYTE Encoder::encode_mate(bool mte){ return mte ? MATE_ : 0; }

BYTE Encoder::encode_pawn_promotion(ptype promo, bool pce){ return static_cast<BYTE>(promo) << (pce ? 5 : 2); }

template<typename T>
BYTE Encoder::encode_queen_axis(const coords<T>& src, const coords<T>& dst){ return ((src[0]==dst[0]) || (src[1]==dst[1])) ? QAROOK_ : QABISHOP_; }

template<typename T>
BYTE Encoder::pawn_action(const coords<T>& src, const coords<T>& dst, bool cap, ptype promo){
    // 000ppp<rr/ff>
    BYTE res;
    if(cap)  { res = dst[1] > src[1] ? 2 : 1; }
    else     { res = abs(dst[0]-src[0]); }
    if(promo){ res |= this->encode_pawn_promotion(promo, false); }
    return res;
};

template<typename T>
BYTE Encoder::knight_action(const coords<T>& src, const coords<T>& dst){
    // 00000aot
    bool f2 = abs(dst[1] - src[1]) == 2,
         rneg = dst[0] < src[0],
         fneg = dst[1] < src[1];

    return  (4 * f2) |
            2*((f2 & rneg) || (!f2 & fneg)) |
            1*((f2 & fneg) || (!f2 & rneg));
};

template<typename T>
BYTE Encoder::bishop_action(const coords<T>& src, const coords<T>& dst){
    // 000admmm
    coords<T> delta{dst[0] - src[0], dst[1] - src[1]};

    return  16*(delta[0] != delta[1]) |
            8*(delta[0] < 0) |
            abs(delta[0]);
};

template<typename T>
BYTE Encoder::rook_action(const coords<T>& src, const coords<T>& dst){
    // 000admmm
    coords<T> delta{dst[0] - src[0], dst[1] - src[1]};

    return  16 * !delta[1] |
            8*((delta[0] | delta[1]) < 0) |
            (abs(delta[0]) | abs(delta[1]));
};

template<typename T>
BYTE Encoder::queen_action(const coords<T>& src, const coords<T>& dst){
    // 000admmm
    if((dst[0]==src[0]) | (dst[1]==src[1])){ return this->rook_action(src, dst); }
    else                                   { return this->bishop_action(src, dst); }
};

template<typename T>
BYTE Encoder::king_action(const coords<T>& src, const coords<T>& dst){
    // 000cffrr
    coords<T> delta{dst[0] - src[0], dst[1] - src[1]};

    return  16*(abs(delta[1]) == 2) |
            (delta[1] ? (delta[1] > 0 ? 4 : 8) : 0) |
            (delta[0] ? (delta[0] > 0 ? 1 : 2) : 0);
};


#endif