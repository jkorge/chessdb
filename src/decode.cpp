#include "include/decode.hpp"

std::vector<ply> Decoder::decode_game(const std::vector<eply>& g, const std::string& fstr){

    std::vector<ply> res;
    if(!fstr.empty()){ fen.parse(fstr, this->board); }
    else             { this->board.newgame(); }

    int i = 0;
    for(std::vector<eply>::const_iterator it=g.begin(); it!=g.end(); ++it){
        res.emplace_back(this->decode_ply(*it, this->board));
        this->board.update(res.back());
    }
    return res;
}

ply Decoder::decode_ply(eply e, ChessBoard& board){

    if(e == this->emissing){ return this->missing; }

    ply p;
    BYTE piece  = e >> 8,
         action = e & 255;

    p.name = this->decode_name(piece);
    p.type = this->decode_type(piece);
    p.c = this->decode_color(p.name);

    p.capture = this->decode_capture(action);
    p.check = this->decode_check(action);
    p.mate = this->decode_mate(action);

    if(p.type == pawn){ p.promo = this->decode_pawn_promotion(action); }
    else              { p.promo = pawn; }
    
    coords src = util::transform::rf(board.mat[p.name]),
                dst,
                delta;
    switch(p.type){
        case pawn:   delta = this->pawn_action(action, p.c); break;
        case knight: delta = this->knight_action(action); break;
        case bishop: delta = this->bishop_action(action); break;
        case rook:   delta = this->rook_action(action); break;
        case queen:  delta = this->queen_action(action, this->decode_queen_axis(piece)); break;
        default:     delta = this->king_action(action);
    }
    dst = src + delta;
    p.src = util::transform::mask(src);
    p.dst = util::transform::mask(dst);

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
    // switch(static_cast<int>(_piece & 31)){
    switch(_piece & 31){
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
        case 23: {
            ptype res = static_cast<ptype>((_piece & 224) >> 5);
            return res == king ? queen : res;
        }
        default: return NOTYPE;
    }
}


color Decoder::decode_color(pname name){ return name == NONAME ? NOCOLOR : (name > 15 ? black : white); }

pname Decoder::decode_name(BYTE _piece){ return static_cast<pname>(_piece & 31); }

bool Decoder::decode_capture(BYTE _action){ return util::constants::CAPTURE_ & _action; }

bool Decoder::decode_check(BYTE _action){ return util::constants::CHECK_ & _action; }

bool Decoder::decode_mate(BYTE _action){ return util::constants::MATE_ & _action; }

ptype Decoder::decode_pawn_promotion(BYTE _action){ return static_cast<ptype>((_action & 28) >> 2); }

ptype Decoder::decode_queen_axis(BYTE _piece){ return (util::constants::QAROOK_ & _piece) ? rook : bishop; }

coords Decoder::pawn_action(BYTE _action, color c){
    // 000ppp<rr/ff>
    bool cap = _action & 32,
         m = _action & 2;

    int dr = c * (cap ? 1 : (m ? 2 : 1)),
        df = cap ? (m ? 1 : -1) : 0;

    return {dr, df};
}

coords Decoder::knight_action(BYTE _action){
    // 00000aot
    int o = 1 - (2 * (bool)(_action & 2)),
        t = 2 - (4 * (bool)(_action & 1));
    if(_action & 4){ return {o, t}; }
    else           { return {t, o}; }
}

coords Decoder::bishop_action(BYTE _action){
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

coords Decoder::rook_action(BYTE _action){
    // 000admmm
    int val = (1 - (2 * ((_action & 8) >> 3))) * (_action & 7);
    if(_action & 16){ return {val, 0}; }
    else            { return {0, val}; }
}

coords Decoder::queen_action(BYTE _action, ptype pt){
    // 000admmm
    switch(pt){
        case rook: return this->rook_action(_action);
        default:   return this->bishop_action(_action);
    }
}

coords Decoder::king_action(BYTE _action){
    // 000cffrr
    int dr = _action & 3,
        df = (_action & 12) >> 2;

    if(dr == 2){ dr = -1; }
    if(df == 2){ df = -1; }
    if(_action & 16){ df *= 2; }
    return {dr, df};

}