#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <map>

#include "types.h"
#include "util.h"
#include "chess_set.h"
#include "log.h"


/**************************
    BOARD
**************************/

class ChessBoard{

    static Logger log;                              // Logger instance
    static const cset roster;                       // Container of pce objects and their starting coords
    void fill_square(pce*, char*);                  // Used by print_board to fill char array for each square printed

public:
    /*
        VARS
    */
    pce *board[8][8];                               // Array of pointers to pieces (given coords get piece)
    std::map<pname, ipair> board_map;               // Map for reverse lookup (given name get coords)

    // TODO: Find a way to have a distinct vector for each board instance.
    //       Requires that elements within `pieces` have const memory address
    static std::vector<pce> pieces;                 // Vector of pieces pointed to by board


    /*
        FUNCS
    */

    ChessBoard();                                   // Assemble pieces on board for standard start of chess game

    ~ChessBoard();                                  // Reset `pieces` vector for future instances of this class

    void clear();                                   // Remove all pieces from board (all array elements point to empty)

    void reset();                                   // Reset members for new game

    void place_pieces(std::vector<pce>);            // Set board pointers and board_map values

    static bool is_dark(int, int);                  // Return true if square at (r,f) is dark; false, otherwise

    static bool is_dark(int*);                      // Overload of square_color for array inputs

    bool update(ply&);                              // Update members according to ply. Handles castling/promotions separately. Returns ply.mate

    void move_piece(pname, int*);                   // Performs member updates when pieces are moved. Called by update

    void castle_rook(int, bool);                    // Computs dst for rook during castle

    void log_consistency();                         // Checks that all members agree as to position of pieces

    str board_viz();                                // Produces string containing visualization of board array

    void log_board();                               // Logs results of board_viz

    void log_map();                                 // Prints values in board_map to console

    bool clear(int*, int*);                         // Checks if every square along line connecting src and dst is empty

    void pins();                                    // Updates status of pieces' pins

    void clear_pins();                              // Resets all pieces' pin status

    ipair operator[](pname);                        // Access coords for piece using []
    pce* operator()(int, int);                      // Access name of piece located @ (r,f)
    pce* operator()(int*);                          // Overload of previous for array inputs
    pce* operator()(ipair dst);                     // Overload of previous for integer pair inputs
};

const cset ChessBoard::roster;

Logger ChessBoard::log;

std::vector<pce> ChessBoard::pieces = ChessBoard::roster.pieces;

ChessBoard::ChessBoard(){ this->log.debug(str()); this->reset(); }

ChessBoard::~ChessBoard(){ this->clear(); this->log.debug(str()); }

void ChessBoard::clear(){
    this->log.debug("Clearing board");
    for(int j=0; j<64; j++){ this->board[j/8][j%8] = nullptr; }
}

void ChessBoard::reset(){ this->place_pieces(this->roster.pieces); }

void ChessBoard::place_pieces(std::vector<pce> pieces){
    this->clear();
    this->pieces = pieces;
    this->log.debug("Placing pieces on board");
    for(std::vector<pce>::iterator it=this->pieces.begin(); it!=this->pieces.end(); ++it){
        // Point board squares to instances in `pieces` vector
        if(it->captured){ continue; }
        this->board[it->rank][it->file] = &(*it);
        this->board_map[it->name] = std::make_pair(it->rank, it->file);
    }
}

bool ChessBoard::is_dark(int r, int f){ return (r%2) == (f%2); }

bool ChessBoard::is_dark(int *c){ return (c[0]%2) == (c[1]%2); }

bool ChessBoard::update(ply &p){

    if(p.capture){
        /*
            En passant captures
                Capturing piece is a pawn &&
                Target square is empty &&
                Square one file step back* contains a pawn
            Set captured pawn's `captured` flag
            Point captured pawn's square to `empty`

            *NB: "back" is relative to the catpuring pawn
                 eg. white pawn captures en passant => "back" is minus one rank
        */
        int back = p.dst[0] + (p.piece.black() ? 1 : -1);
        if(
            (p.piece.type == pawn) &&
            (not (*this)(p.dst)) &&
            ((*this)(back, p.dst[1])->type == pawn)
        ){
            this->board[back][p.dst[1]]->captured = true;
            this->board[back][p.dst[1]] = nullptr;
        }

        /*
            Regular Captures
            Set captured material's `captured` flag
        */
        else{ this->board[p.dst[0]][p.dst[1]]->captured = true; }
    }

    // Relocate piece and update members
    this->move_piece(p.piece.name, p.dst);

    // Move rooks for castle plies
    if(p.castle){ this->castle_rook(p.castle, p.piece.black()); }

    // Change type of piece for pawn promotion
    if(p.promo){ this->pieces[p.piece.name].type = p.promo; }

    this->pins();

    return p.mate;
}

void ChessBoard::move_piece(pname p, int *dst){

    pce *piece_ptr = &(this->pieces[p]);

    (this->board)[piece_ptr->rank][piece_ptr->file] = nullptr;                // Point src to empty

    piece_ptr->place(dst);                                                    // Update rank/file for piece in `pieces` vector

    (this->board)[piece_ptr->rank][piece_ptr->file] = piece_ptr;              // Point dst to piece

    this->board_map[p] = std::make_pair(dst[0], dst[1]);                      // Update board_map coords
}

void ChessBoard::castle_rook(int c, bool bply){
    pname rk = (c > 0)
               ? (bply ? black_rook_h : white_rook_h)
               : (bply ? black_rook_a : white_rook_a);
    int dst[2]{this->pieces[rk].rank, this->pieces[rk].file + (-2 * c) + (c < 0)};
    this->move_piece(rk, dst);
}

void ChessBoard::log_consistency(){
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

        if(c1 && c2 && c3){ this->log.debug("Board members consistent"); }
        if(!c1){ this->log.warn("Board doesn't match piece for ", util::pname2s[p]); }
        if(!c2){ this->log.warn("Board doesn't match map for ", util::pname2s[p]); }
        if(!c3){ this->log.warn("Map doesn't match piece for ", util::pname2s[p]); }
    }
}

str ChessBoard::board_viz(){
    str viz("\n ");

    // File headers
    for(int k=0; k<8; k++){ viz += str(" ") + util::files[k] + "  "; }

    int r, f;
    char square[]{"[  ]"};

    // Board squares
    for(int i=0; i<64; i++){

        // Rank headers
        r = (63-i)/8; f = i%8;
        if(!f){ viz += str("\n") + util::rank2c[r] + ' '; }

        this->fill_square((this->board)[r][f], square);
        viz += square;
    }
    viz += '\n';
    return viz;
}

void ChessBoard::log_board(){ this->log.debug(this->board_viz()); }

void ChessBoard::fill_square(pce *p, char *sq){
    if(not p){
        sq[1] = ' ';
        sq[2] = ' ';
    }
    else{
        sq[1] = (p->black() ? 'b' : 'w');           // color indicator
        sq[2] = util::ptype2c[p->type];             // Piece type indicator
    }
}

void ChessBoard::log_map(){
    str viz("\n");
    for(std::map<pname, ipair>::iterator it=board_map.begin(); it!=board_map.end(); ++it){
        viz += util::pname2s[it->first];
        viz += str(": ") + util::file2c[board_map[it->first].second] + util::rank2c[board_map[it->first].first];
    }
    this->log.debug(viz);
}

// Determine if line connecting (not including) src and dst is cleared of all material
// When used for disambiguating, put candidate-piece's coords in src and target square in dst
bool ChessBoard::clear(int *src, int *dst){
    int delta[2] = {dst[0]-src[0], dst[1]-src[1]},
        rstep = delta[0] / (delta[0] ? abs(delta[0]) : 1),
        fstep = delta[1] / (delta[1] ? abs(delta[1]) : 1);

    for(int r=src[0]+rstep, f=src[1]+fstep; r!=src[0]+delta[0] | f!=src[1]+delta[1]; r+=rstep, f+=fstep){
        if(this->board[r][f]){ return false; }
    }
    return true;

}

void ChessBoard::clear_pins(){ for(short i=0; i<this->pieces.size(); i++){ if(this->pieces[i].pin.val){ this->pieces[i].pin = util::nullpin; } } }

void ChessBoard::pins(){

    this->clear_pins();

    for(pname kname : {white_king_e, black_king_e}){

        int kloc[2] = {this->pieces[kname].rank, this->pieces[kname].file};

        for(short i=0; i<util::xrays.size(); i++){

            pce *nearest[2];
            int c = 0;
            ipair steps = util::xrays[i];

            for(short r=kloc[0]+steps.first, f=kloc[1]+steps.second; util::inbounds(r,f); r+=steps.first, f+=steps.second){
                
                // Record two nearest (to the king) pieces along the ray
                if(this->board[r][f]){ nearest[c] = this->board[r][f]; c++; }

                if(c==2){
                    ptype n_1_t = nearest[1]->type;
                    bool n_0_sk = nearest[0]->black() == kname>16,                              // Nearest is same color as king
                         n_1_dk = nearest[1]->black() == kname<16,                              // Second nearest is different color
                         n_1_bp = (n_1_t == bishop || n_1_t == queen) && (i < 4),               // Second nearest is bishop and ray is diagonal
                         n_1_rp = (n_1_t == rook || n_1_t == queen) && (i >= 4);                // Second nearest is rook and ray is horizontal/vertical

                    if(n_0_sk && n_1_dk && (n_1_bp || n_1_rp)){
                        this->log.debug(util::pname2s[nearest[0]->name], "is pinned by", util::pname2s[nearest[1]->name]);
                        nearest[0]->pin = pint{true, nearest[1]->name, i};
                    }
                    break;
                }
            }
        }
    }
}

ipair ChessBoard::operator[](pname p){ return this->board_map[p]; }
pce* ChessBoard::operator()(int r, int f){ return this->board[r][f]; }
pce* ChessBoard::operator()(int *dst){ return this->board[dst[0]][dst[1]]; }
pce* ChessBoard::operator()(ipair dst){ return this->board[dst.first][dst.second]; }

#endif


