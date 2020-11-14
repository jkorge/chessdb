#ifndef FENCE_H
#define FENCE_H

#include <iostream>
#include <string>
#include <regex>
#include <map>
#include <vector>
#include <cctype>

#include "types.h"
#include "util.h"
#include "board.h"
#include "chess_set.h"


static const cset roster;

/**********************************************
    Forsyth-Edwards Notation Class Extractor
**********************************************/

class FENCE{
public:

    static std::map<char, ptype> piece_types;
    static std::vector<pce> pieces;

    const char black_pieces[7]{"pnbrqk"};
    const char white_pieces[7]{"PNBRQK"};
    const char rank_delim[2]{"/"};

    std::map<int, std::string> fen_fields;
    std::vector<pname> placed;

    void parse_fen_string(std::string, ChessBoard&);
    void parse_piece_placement(std::string, ChessBoard&);
    bool isplaced(pname);
    void reset();
    pname parse_piece_type(char, bool);
};

std::vector<pce> FENCE::pieces = roster.pieces;
std::map<char, ptype> FENCE::piece_types{
    {'p',pawn},
    {'n',knight},
    {'b',bishop},
    {'r',rook},
    {'q',queen},
    {'k',king},

    {'P',pawn},
    {'N',knight},
    {'B',bishop},
    {'R',rook},
    {'Q',queen},
    {'K',king}
};


void FENCE::reset(){
    this->pieces = roster.pieces;
    this->placed.clear();
    this->fen_fields.clear();
}


void FENCE::parse_fen_string(std::string fen_string, ChessBoard &board){
    this->reset();

    char *token = strtok(&fen_string[0], " ");
    for(int i=0; i<6; i++){
        this->fen_fields[i] = token;
        token = strtok(NULL, " ");
    }
    this->parse_piece_placement(this->fen_fields[0], board);
}

void FENCE::parse_piece_placement(std::string piece_string, ChessBoard &board){
    static int coord[2];
    static std::string token;
    static pname name;
    static ptype type;

    token = strtok(&piece_string[0], this->rank_delim);
    coord[0] = 7;
    for(int i=0; i<8; i++){
        coord[1] = 0;

        for(int k=0; k<token.size(); k++){
            if(isdigit(token[k])){ coord[1] += (token[k] - '0'); }
            else{
                // Get first available piece that matches
                name = this->parse_piece_type(token[k], board.square_color(coord));
                // Check its type; pawns may haven been promoted
                type = util::name2type(name);
                if(type != this->piece_types[token[k]]){ this->pieces[name].type = type; }
                // Update coords and mark as placed
                this->pieces[name].rank = coord[0];
                this->pieces[name].file = coord[1];
                this->placed.push_back(name);
                // Next file
                coord[1] += 1;
            }
        }
        if(coord[0] > 0){
            // Next token for next rank
            token = strtok(NULL, this->rank_delim);
            coord[0] -= 1;
        }
    }


    // Mark unplaced pieces as captured
    for(int i=0; i<32; i++){
        name = static_cast<pname>(i);
        if(!(this->isplaced(name))){ this->pieces[name].captured = true; }
    }

    // Assemble pieces on board
    board.place_pieces(this->pieces);

}

pname FENCE::parse_piece_type(char p, bool is_dark){
    static ptype type;
    static pname name;
    static int s;
    static bool c1, c2;

    type = this->piece_types[p];
    s = isupper(p) ? 0 : 16;

    // Go through piece roster
    for(int i=0; i<16; i++){
        name = static_cast<pname>(i + s);
        // Find piece of appropriate type that hasn't been placed
        c1 = util::name2type(name) == type;
        // Dark bishops on dark squares and light bishops on light squares
        if(type == bishop){ c2 = name == (is_dark ? (s ? black_bishop_f : white_bishop_c) : (s ? black_bishop_c : white_bishop_f)); }
        else{ c2 = true; }
        if((c1 & c2) && !(this->isplaced(name))){ return name; }
    }

    // If no matching piece found, it must be a promoted pawn
    for(int i=0; i<8; i++){
        name = static_cast<pname>(i + s);
        if(!(this->isplaced(name))){ return name; }
    }

    // Something went wrong if this happens...
    return NONAME;
}

bool FENCE::isplaced(pname n){
    for(int i=0; i<this->placed.size(); i++){ if(n == placed[i]){ return true; } }
    return false;
}

#endif