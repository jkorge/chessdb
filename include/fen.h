#ifndef FEN_H
#define FEN_H

#include "log.h"
#include "types.h"
#include "util.h"
#include "board.h"

namespace fen{

    Logger log;

    void arrange(str fs, ChessBoard& board){
        int rank = 7, file =0;
        rtok boardtok(fs.begin(), fs.end(), util::fen::delim, -1);

        while(boardtok != util::rtokend){
            str rs = *boardtok++;
            for(str::iterator it=rs.begin(); it!=rs.end(); ++it){
                if(isdigit(*it)){ file += (*it - '0'); }
                else{
                    color c = islower(*it) ? black : white;
                    ptype pt = util::fen::c2type(*it);
                    board.place(coords<int>(rank, file), pt, c);
                }
            }
            // Next token, next rank, first file
            rank--; file = 0;
        }
    }

    // Arrange pieces on board according to FEN string. Return boolean indicating if black moves next
    color parse(str fs, ChessBoard& board){

        log.info("Parsing FEN string:", fs);

        // FEN matches setup for new game - no processing needed
        if(!fs.compare(util::fen::new_game)){
            log.debug("FEN string for standard new game");
            board.newgame();
            return white;
        }
        
        rtok end;
        rtok fentok(fs.begin(), fs.end(), util::ws, -1);

        // First field in FEN string contains board arrangment
        arrange(*fentok++, board);
        // Second field indicates next player to move
        color c = (*fentok++) == "b" ? black : white;
        // Remaining fields (castling availability, en passants, etc.) unused for now
        while(fentok != end){ fentok++; }

        return c;
    }

}

#endif