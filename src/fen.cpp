#include "include/fen.hpp"

int Fen::cnt = 0;

Fen::Fen(logging::LEVEL lvl, bool flog) : logger(Fen::cnt, lvl, flog) { ++this->cnt; }

bool Fen::is_avail(int x, pname* avail){ return avail[x] != NONAME; }

pname Fen::choose_pawn(color c, pname* avail){
    pname res;
    int start = c<0 ? 16 : 8,
        stop  = c<0 ? 24 : 16;
    for(int i=start; i<stop ; ++i){
        if(this->is_avail(i, avail)){
            res = avail[i];
            avail[i] = NONAME;
            break;
        }
    }
    return res;
}

pname Fen::choose(ptype pt, color c, pname* avail){

    pname res = NONAME;
    int idx = c<0 ? 24 : 0;

    switch(pt){
        case pawn:
            return this->choose_pawn(c, avail);

        case knight:
            if     (this->is_avail(idx + 1, avail)){ res = white_knight_b; avail[idx+1] = NONAME; }
            else if(this->is_avail(idx + 6, avail)){ res = white_knight_g; avail[idx+6] = NONAME; }
            break;

        case bishop:
            if     (this->is_avail(idx + 2, avail)){ res = white_bishop_c; avail[idx+2] = NONAME; }
            else if(this->is_avail(idx + 5, avail)){ res = white_bishop_f; avail[idx+5] = NONAME; }
            break;

        case rook:
            if     (this->is_avail(idx    , avail)){ res = white_rook_a;   avail[idx  ] = NONAME; }
            else if(this->is_avail(idx + 7, avail)){ res = white_rook_h;   avail[idx+7] = NONAME; }
            break;

        case queen:
            if     (this->is_avail(idx + 3, avail)){ res = white_queen_d;  avail[idx+3] = NONAME; }
            break;

        default:
            // king
            return c<0 ? black_king_e : white_king_e;
    }

    // No piece of given type & color available - assume promoted pawn
    if(res == NONAME){ res = this->choose_pawn(c, avail); }
    // Change color of res as needed
    else if(c < 0){ res = static_cast<pname>(res + 24); }

    return res;
}

void Fen::arrange(std::string fs, ChessBoard& board){
    pname avail[32];
    for(int i=0; i<32; ++i){ avail[i] = static_cast<pname>(i); }

    int rank = 7, file =0;
    rtok boardtok(fs.begin(), fs.end(), util::fen::delim, -1);

    while(boardtok != util::constants::rtokend){
        std::string rs = *boardtok++;
        for(std::string::iterator it=rs.begin(); it!=rs.end(); ++it){
            if(isdigit(*it)){ file += (*it - '0'); }
            else{
                color c = islower(*it) ? black : white;
                ptype pt = util::fen::c2type(*it);
                square loc = 8*rank + file;
                pname name = this->choose(pt, c, avail);

                board.place(loc, pt, c);
                board.map[loc] = name;
                board.mat[name] = loc;

                ++file;
            }
        }
        // Next token => next rank, first file
        rank--; file = 0;
    }
}

// Arrange pieces on board according to FEN string. Return color indicating which player moves next
void Fen::parse(std::string fs, ChessBoard& board){

    this->logger.info("Parsing FEN string:", fs);
    color c;
    // FEN matches setup for new game - no processing needed
    if(!fs.compare(util::fen::new_game)){
        this->logger.debug("FEN string for standard new game");
        board.newgame();
    }
    else{
        board.clear();
        rtok fentok(fs.begin(), fs.end(), util::constants::wsr, -1);

        // First field in FEN string contains board arrangment
        this->arrange(*fentok++, board);

        // Next player to move
        board.next = (*fentok++) == 'b' ? black : white;

        // Castling availability
        std::string cas = *fentok++;
        BYTE i = 1;
        for(const char* c=util::fen::castles; c!=std::end(util::fen::castles); ++c){
            if(cas.find(*c) != std::string::npos){ board.cancas |= i; }
            i <<= 1;
        }

        // En Passant
        std::string enpas = *fentok++;
        board.enpas = util::transform::mask(coords{util::repr::c2rank(enpas[0]), util::repr::c2file(enpas[1])});

        // Move counters
        board.half = std::stoi(*fentok++);
        board.full = std::stoi(*fentok++);

        // Check for pins
        board.pinsearch();

        // Check for...check
        board.get_checkers(!board.next);
        if(board.checkers.size()){ board.check = board.next; }

    }
}
