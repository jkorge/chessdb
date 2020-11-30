#ifndef FEN_H
#define FEN_H

#include <iostream>

#include "log.h"
#include "types.h"
#include "util.h"
#include "board.h"
#include "chess_set.h"

namespace fen{

    Logger log(__FILE__);

    pname type2name(std::vector<pce> &pieces, ptype t, bool bply, bool dark){
        /*
            Seek piece which hasn't been placed and is of the correct type
            For bishops, further seek to place light(dark)-squared bishops on light(dark)-squares
            If such a piece isn't found, it must then be a promoted pawn
        */

        std::vector<pce>::iterator start = pieces.begin() + (bply * 16),
                                   end = pieces.end() - (!bply * 16);
        for(std::vector<pce>::iterator it=start; it!=end; ++it){
            if(t != it->type || it->rank != -1 || it->file != -1){ continue; }
            if(t == bishop && !(it->name == util::bmap[dark][bply])){ continue; }
            return it->name;
        }

        // If the piece couldn't be found, it must then be a promoted pawn
        for(std::vector<pce>::iterator it=start; it!=start+8; ++it){
            if(it->rank != -1 || it->file != -1){ continue; }
            return it->name;
        }

        // Suppress warnings about flow control reaching end of non-void function
        return NONAME;
    }

    std::vector<pce> arrange(str fs){

        xcset roster;

        std::regex_token_iterator<std::string::iterator> end;
        std::regex_token_iterator<std::string::iterator> rtok(fs.begin(), fs.end(), util::fen::delim, -1);

        int rank = 7, file =0;
        while(rtok != end){
            str rs = *rtok++;
            for(str::iterator it=rs.begin(); it!=rs.end(); ++it){
                if(isdigit(*it)){ file += (*it - '0'); }
                else{

                    // Color, type, and name
                    bool bply = islower(*it);
                    ptype t = util::fen::c2type[*it];
                    pname idx = type2name(roster.pieces, t, bply, ChessBoard::is_dark(rank, file));
                    // Apply pawn promotions as required
                    if(t != pawn && util::name2type(idx) == pawn){ roster.pieces[idx].type = t; }

                    // Set pce coords
                    // log.info("Placing", util::pname2s[idx], "on", util::coord2s(rank, file));
                    roster.pieces[idx].place(rank, file++);
                }
            }
            // Next token, next rank
            rank--;
            file = 0;
        }

        // Mark unplaced pce as captured
        for(std::vector<pce>::iterator it=roster.pieces.begin(); it!=roster.pieces.end(); ++it){
            if(it->rank == -1 & it->file == -1){ it->captured = true; }
        }

        return roster.pieces;
    }

    // Arrange pieces on board according to FEN string. Return boolean indicating if black moves next
    bool parse(str fs, ChessBoard &board){

        log.info("Parsing FEN string:", fs);

        // FEN matches setup for new game - no processing needed
        if(!fs.compare(util::fen::new_game)){
            log.debug("FEN string for standard new game");
            return false;
        }
        
        std::regex_token_iterator<std::string::iterator> end;
        std::regex_token_iterator<std::string::iterator> rtok(fs.begin(), fs.end(), util::ws, -1);

        // First field in FEN string contains board arrangment
        board.place_pieces(arrange(*rtok++));
        // Second field indicates next player to move
        bool bply = (*rtok++) == "b";
        // Remaining fields (castling availability, en passants, etc.) unused for now
        while(rtok != end){ rtok++; }

        return bply;
    }

}

#endif