#ifndef FEN_H
#define FEN_H

#include "log.h"
#include "types.h"
#include "util.h"
#include "board.h"

namespace fen{

    Logger log;

    
    inline bool is_avail(int x, pname* avail){ return avail[x] != NONAME; }

    pname choose_pawn(color c, pname* avail){
        pname res;
        int start = c<0 ? 16 : 8,
            stop  = c<0 ? 24 : 16;
        for(int i=start; i<stop ; ++i){
            if(is_avail(i, avail)){
                res = avail[i];
                avail[i] = NONAME;
                break;
            }
        }
        return res;
    }

    pname choose(ptype pt, color c, pname* avail){

        pname res = NONAME;
        int idx = c<0 ? 24 : 0;

        switch(pt){
            case pawn:
                return choose_pawn(c, avail);

            case knight:
                if     (is_avail(idx + 1, avail)){ res = white_knight_b; avail[idx+1] = NONAME; }
                else if(is_avail(idx + 6, avail)){ res = white_knight_g; avail[idx+6] = NONAME; }
                break;

            case bishop:
                if     (is_avail(idx + 2, avail)){ res = white_bishop_c; avail[idx+2] = NONAME; }
                else if(is_avail(idx + 5, avail)){ res = white_bishop_f; avail[idx+5] = NONAME; }
                break;

            case rook:
                if     (is_avail(idx    , avail)){ res = white_rook_a;   avail[idx  ] = NONAME; }
                else if(is_avail(idx + 7, avail)){ res = white_rook_h;   avail[idx+7] = NONAME; }
                break;

            case queen:
                if     (is_avail(idx + 3, avail)){ res = white_queen_d;  avail[idx+3] = NONAME; }
                break;

            default:
                // king
                return c<0 ? black_king_e : white_king_e;
        }

        // No piece of given type & color available - assume promoted pawn
        if(res == NONAME){ res = choose_pawn(c, avail); }
        // Change color of res as needed
        else if(c < 0){ res = static_cast<pname>(res + 24); }

        return res;
    }

    void arrange(str fs, ChessBoard& board){
        pname avail[32];
        for(int i=0; i<32; ++i){ avail[i] = static_cast<pname>(i); }

        int rank = 7, file =0;
        rtok boardtok(fs.begin(), fs.end(), util::fen::delim, -1);

        while(boardtok != util::rtokend){
            str rs = *boardtok++;
            for(str::iterator it=rs.begin(); it!=rs.end(); ++it){
                if(isdigit(*it)){ file += (*it - '0'); }
                else{
                    color c = islower(*it) ? black : white;
                    ptype pt = util::fen::c2type(*it);
                    square loc = 8*rank + file;
                    pname name = choose(pt, c, avail);

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
    color parse(str fs, ChessBoard& board){

        log.info("Parsing FEN string:", fs);
        color c;
        // FEN matches setup for new game - no processing needed
        if(!fs.compare(util::fen::new_game)){
            log.debug("FEN string for standard new game");
            board.newgame();
            c = white;
        }
        else{
            board.clear();
            rtok fentok(fs.begin(), fs.end(), util::ws, -1);
            // First field in FEN string contains board arrangment
            arrange(*fentok++, board);
            // Second field indicates next player to move
            c = (*fentok++) == "b" ? black : white;
            // Remaining fields (castling availability, en passants, etc.) unused for now
            while(fentok != util::rtokend){ fentok++; }
        }

        return c;
    }

}

#endif