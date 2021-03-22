#include "include/encode.hpp"

eply Encoder::encode_ply(const ply& p){ return p == this->missing ? this->emissing : (static_cast<uint16_t>(this->encode_piece(p)) << 8) | this->encode_action(p); }

std::vector<eply> Encoder::encode_game(const std::vector<ply>& g){
    std::vector<eply> res;
    for(std::vector<ply>::const_iterator it=g.begin(); it!=g.end(); ++it){ res.emplace_back(this->encode_ply(*it)); }
    return res;
}

BYTE Encoder::encode_piece(const ply& p){
    BYTE _piece = static_cast<BYTE>(p.name);
    if(p.type != pawn && (p.name >= 8 & p.name <= 23)){ _piece |= this->encode_pawn_promotion(p.type); }
    if(p.type == queen)                               { _piece |= this->encode_queen_axis(util::transform::rf(p.src), util::transform::rf(p.dst)); }
    return _piece;
}

BYTE Encoder::encode_action(const ply& p){
    coords src = util::transform::rf(p.src),
                dst = util::transform::rf(p.dst);

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

BYTE Encoder::encode_capture(bool cap){ return cap ? util::constants::CAPTURE_ : 0; }

BYTE Encoder::encode_check(bool chk){ return chk ? util::constants::CHECK_ : 0; }

BYTE Encoder::encode_mate(bool mte){ return mte ? util::constants::MATE_ : 0; }

BYTE Encoder::encode_pawn_promotion(ptype promo, bool pce){ return static_cast<BYTE>(promo) << (pce ? 5 : 2); }

BYTE Encoder::encode_queen_axis(const coords& src, const coords& dst){ return ((src[0]==dst[0]) || (src[1]==dst[1])) ? util::constants::QAROOK_ : util::constants::QABISHOP_; }

BYTE Encoder::pawn_action(const coords& src, const coords& dst, bool cap, ptype promo){
    // 000ppp<rr/ff>
    BYTE res;
    if(cap)  { res = dst[1] > src[1] ? 2 : 1; }
    else     { res = abs(dst[0]-src[0]); }
    if(promo){ res |= this->encode_pawn_promotion(promo, false); }
    return res;
}

BYTE Encoder::knight_action(const coords& src, const coords& dst){
    // 00000aot
    bool f2 = abs(dst[1] - src[1]) == 2,
         rneg = dst[0] < src[0],
         fneg = dst[1] < src[1];

    return  (4 * f2) |
            2*((f2 & rneg) || (!f2 & fneg)) |
            1*((f2 & fneg) || (!f2 & rneg));
}

BYTE Encoder::bishop_action(const coords& src, const coords& dst){
    // 000admmm
    coords delta{dst[0] - src[0], dst[1] - src[1]};

    return  16*(delta[0] != delta[1]) |
            8*(delta[0] < 0) |
            abs(delta[0]);
}

BYTE Encoder::rook_action(const coords& src, const coords& dst){
    // 000admmm
    coords delta{dst[0] - src[0], dst[1] - src[1]};

    return  16 * !delta[1] |
            8*((delta[0] | delta[1]) < 0) |
            (abs(delta[0]) | abs(delta[1]));
}

BYTE Encoder::queen_action(const coords& src, const coords& dst){
    // 000admmm
    if((dst[0]==src[0]) | (dst[1]==src[1])){ return this->rook_action(src, dst); }
    else                                   { return this->bishop_action(src, dst); }
}

BYTE Encoder::king_action(const coords& src, const coords& dst){
    // 000cffrr
    coords delta{dst[0] - src[0], dst[1] - src[1]};

    return  16*(abs(delta[1]) == 2) |
            (delta[1] ? (delta[1] > 0 ? 4 : 8) : 0) |
            (delta[0] ? (delta[0] > 0 ? 1 : 2) : 0);
}