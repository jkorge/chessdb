#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <map>

#include "types.h"
#include "util.h"
#include "chess_set.h"


/**************************
    BOARD
**************************/

class ChessBoard{
private:
    pce empty{NONAME};                              // NONAME piece for empty board squares
    void fill_square(pce*, char*);                  // Used by print_board to fill char array for each square printed
    void place_pieces();                            // Calls its public overload with arg `this->pieces`
    static const cset roster;                       // Container of pce objects and their starting coords

public:
    /*
        VARS
    */
    pce *board[8][8];                               // Array of pointers to pieces (given coords get piece)

    std::map<pname, ipair> board_map;               // Map for reverse lookup (given name get coords)

    static std::vector<pce> pieces;                 // Vector of pieces pointed to by board


    /*
        FUNCS
    */

    ChessBoard();                                   // Assemble pieces on board

    ~ChessBoard();                                  // Reset `pieces` vector for future instances of this class

    void clear();                                   // Remove all pieces from board (all array elements point to empty)

    void reset();                                   // Reset members for new game

    void place_pieces(std::vector<pce>);            // Set board pointers and board_map values omit arg for start of new game; give vector of pieces to resume suspended game

    bool square_color(int r, int f);                // Return true if square at (r,f) is dark; false, otherwise

    bool square_color(int *c);                      // Overload of square_color for array inputs

    bool update(ply&);                              // Update members according to ply. Handles castling/promotions separately. Returns ply.mate

    bool update(turn&);                             // Update overload for turn objects. if(not turn.white.mate){ return turn.black.mate } else{ return turn.white.mate }

    void move_piece(pname, int*);                   // Performs member updates when pieces are moved. Called by update

    void castle_rook(pname, int);                   // Computs dst for rook during castle

    void check_board(std::ostream& = std::cout);    // Checks that all members agree as to position of pieces

    void print_board(std::ostream& = std::cout);    // Prints board to console

    void print_map(std::ostream& = std::cout);      // Prints values in board_map to console

    ipair operator[](pname);                        // Access coords for piece using []
};

const cset ChessBoard::roster;

std::vector<pce> ChessBoard::pieces = ChessBoard::roster.pieces;

ChessBoard::ChessBoard(){ this->place_pieces(); }

ChessBoard::~ChessBoard(){ this->reset(); }

void ChessBoard::clear(){ for(int j=0; j<64; j++){ this->board[j/8][j%8] = &empty; } }

void ChessBoard::reset(){
    this->pieces = this->roster.pieces;
    this->place_pieces();
}

void ChessBoard::place_pieces(){ this->place_pieces(this->pieces); }

void ChessBoard::place_pieces(std::vector<pce> pieces){

    this->clear();
    this->pieces = pieces;
    for(std::vector<pce>::iterator it=this->pieces.begin(); it!=this->pieces.end(); ++it){
        // Point board squares to instances in `pieces` vector
        if(it->captured){ continue; }
        this->board[it->rank][it->file] = &(*it);
        this->board_map[it->name] = std::make_pair(it->rank, it->file);
    }
}

bool ChessBoard::square_color(int r, int f){ return (r%2) == (f%2); }

bool ChessBoard::square_color(int *c){ return (c[0]%2) == (c[1]%2); }

bool ChessBoard::update(ply &p){

    // Flag whatever piece is already at dst as captured
    if(p.capture){ this->board[p.dst[0]][p.dst[1]]->captured = true; }

    // Relocate piece and update members
    this->move_piece(p.piece.name, p.dst);

    // Handle rooks for castle plies
    if(p.castle){
        pname rook;
        switch(p.castle){
            case 1:
                // Kingside
                if(p.piece.white()){ rook = white_rook_h; }
                else{ rook = black_rook_h; }
                break;
            case -1:
                // Queenside
                if(p.piece.white()){ rook = white_rook_a; }
                else{ rook = black_rook_a; }
                break;
        }
        this->castle_rook(rook, p.castle);
    }

    // Change type of piece for pawn promotion
    if(p.promo){ this->pieces[p.piece.name].type = p.promo; }

    return p.mate;
}

bool ChessBoard::update(turn &t){
    if(!(this->update(t.white))){ return this->update(t.black); }
    else{ return t.white.mate; }
}

void ChessBoard::move_piece(pname p, int *dst){

    static pce *piece_ptr;
    piece_ptr = &(this->pieces[p]);

    (this->board)[piece_ptr->rank][piece_ptr->file] = &(this->empty);         // Point src to empty

    piece_ptr->rank = dst[0];                                                 // Update rank/file for piece in `pieces` vector
    piece_ptr->file = dst[1];

    (this->board)[piece_ptr->rank][piece_ptr->file] = piece_ptr;              // Point dst to piece

    this->board_map[p] = std::make_pair(dst[0], dst[1]);                      // Update board_map coords
}

void ChessBoard::castle_rook(pname p, int c){
    int dst[2]{pieces[p].rank, pieces[p].file + (-2 * c) + (c < 0)};
    this->move_piece(p, dst);
}

void ChessBoard::check_board(std::ostream &out){
    pname p;
    bool c1, c2, c3;
    int prank, pfile, brank, bfile;
    for(int i=0; i<32; i++){

        p = static_cast<pname>(i);

        prank = this->pieces[p].rank;
        pfile = this->pieces[p].file;

        brank = this->board_map[p].first;
        bfile = this->board_map[p].second;

        // check board matches piece
        c1 = (this->board)[prank][pfile]->name == p;
        // check board matches map
        c2 = (this->board)[brank][bfile]->name == p;
        // check piece matches map
        c3 = (this->board_map)[p] == std::make_pair(prank, pfile);

        if(!c1){ out << "Board doesn't match piece for " << util::pname2s[p] << std::endl; }
        if(!c2){ out << "Board doesn't match map for " << util::pname2s[p] << std::endl; }
        if(!c3){ out << "Map doesn't match piece for " << util::pname2s[p] << std::endl; }
    }
}

void ChessBoard::print_board(std::ostream &out){
    static char square[]{"[  ]"};

    // File headers
    for(int k=0;k<util::files.size();k++){ if(!k){ out << "  "; } out << " " << util::files[k] << "  "; }

    int r, f;
    for(int i=0; i<64; i++){
        r = (63-i)/8; f = i%8;
        if(!f){ out << std::endl << r+1 << " "; } // rank headers
        this->fill_square((this->board)[r][f], square);
        out << square;
    }
    out << std::endl << std::endl;
}

void ChessBoard::fill_square(pce *p, char *sq){
    if(p->name == NONAME){ for(int i=1;i<3;i++){ sq[i] = ' '; } }
    else{
        sq[1] = (p->black() ? 'b' : 'w');           // color indicator
        sq[2] = util::ptype2c[p->type];             // Piece type indicator
    }
}

void ChessBoard::print_map(std::ostream &out){

    int p;
    std::string t;

    for(std::map<pname, ipair>::iterator it=board_map.begin(); it!=board_map.end(); ++it){
        out << util::pname2s[it->first];
        out << " (" << board_map[it->first].first << ", " << board_map[it->first].second << ")" << std::endl;
    }
    out << std::endl;
}

ipair ChessBoard::operator[](pname p){ return this->board_map[p]; }

#endif