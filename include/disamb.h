#ifndef DISAMB_H
#define DISAMB_H

#include <exception>

#include "types.h"
#include "board.h"
#include "util.h"
#include "log.h"


namespace disamb{

    Logger log(__FILE__);


    // King disambiguation
    pname K(bool bply){ return bply ? black_king_e : white_king_e; }


    // xmove: Can piece type x reach dst from src?
    template<typename IntT>
    bool bmove(IntT *src, IntT *dst){ return abs(src[0] - dst[0]) == abs(src[1] - dst[1]); }

    template<typename IntT>
    bool rmove(IntT *src, IntT *dst){ return !(src[0] - dst[0]) ^ !(src[1] - dst[1]); }

    template<typename IntT>
    bool pmove(IntT *src, IntT *dst, bool cap){
        IntT dr = abs(src[0] - dst[0]),
             df = abs(src[1] - dst[1]);
        return cap ? (dr == 1 && df == 1) : (!df && (dr == 1 || dr == 2));
    }

    template<typename IntT>
    bool nmove(IntT *src, IntT *dst){
        IntT dr = abs(src[0] - dst[0]),
             df = abs(src[1] - dst[1]);
        return (dr == 2 ^ df == 2) && (dr == 1 ^ df == 1);
    }

    template<typename IntT>
    bool qmove(IntT *src, IntT *dst){ return (!(src[0] - dst[0]) ^ !(src[1] - dst[1])) ? rmove(src, dst) : bmove (src, dst); }

    template<typename IntT>
    bool reachable(IntT *src, IntT *dst, ptype pt){
        bool res;
        switch(pt){
            case knight: res = nmove(src, dst); break;
            case bishop: res = bmove(src, dst); break;
            case rook: res = rmove(src, dst); break;
            case queen: res = qmove(src, dst); break;
        }
        return res;
    }

    // Filter pieces on type, color, and captured status
    std::vector<pname> get_pieces(ptype pt, bool bply, ChessBoard &board){
        std::vector<pname> res;
        std::vector<pce>::iterator start = board.pieces.begin() + (bply * 16),
                                   end = board.pieces.end() - (!bply * 16);

        for(std::vector<pce>::iterator it=start; it!=end; ++it){
            if(it->type != pt || it->captured){ continue; }
            else{ res.emplace_back(it->name); }
        }

        return res;
    }


    /*
        DISAMBIGUATION DRIVER FUNCTION
            Given data from a parsed PGN ply:
                updates src coords
                returns piece name
            Updated coords and returned name are those of the piece to be moved.
    */

    template<typename IntT>
    pname pgnply(ptype pt, IntT *src, IntT *dst, ChessBoard &board, bool bply, bool capture){

        // "Disambiguating <black/white> <piece_type> to <dst>"
        log.debug("Disambiguating", (bply ? "black" : "white"), util::ptype2s[pt], "to", util::file2c[dst[1]], util::rank2c[dst[0]]);

        pname idx;
        if(pt == pawn){
            IntT one = dst[0] - (1 - (2 * bply)),
                 two = dst[0] - (2 - (4 * bply));

            src[0] = (capture || (board.board[one][dst[1]]->type == pawn)) ? one : two;
            src[1] = capture ? src[1] : dst[1];

            idx = board.board[src[0]][src[1]]->name;
        }
        else if(pt == king){
            idx = K(bply);
            src[0] = board[idx].first;
            src[1] = board[idx].second;
        }
        else{

            bool hsr = src[0] != -1,
                 hsf = src[1] != -1,
                 partial = hsr ^ hsf;

            if(partial){ log.debug("Partial disambiguation:", (hsf ? util::file2c[src[1]] : '?'), (hsr ? util::rank2c[src[0]] : '?')); }

            // Filter candidates on partial disambiguation, if applicable; otherwise, filter on ability to access dst
            std::vector<pname> candidates = get_pieces(pt, bply, board);
            if(!candidates.size()){ log.error("Zero candidate pieces for this ply"); }

            bool found;
            for(std::vector<pname>::iterator it=candidates.begin(); it!=candidates.end(); ++it){
                IntT csrc[2] = {board[*it].first, board[*it].second};
                found = partial
                        ? src[hsf] == (hsf ? board[*it].second : board[*it].first)
                        : reachable(csrc, dst, pt) && ((pt == knight) || board.clear(csrc, dst));

                if(found){ idx = *it; break; }

            }
            src[0] = board[idx].first;
            src[1] = board[idx].second;
        }
        log.debug(util::pname2s[idx]);
        return idx;
    }

}

#endif