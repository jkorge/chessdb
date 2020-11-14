#ifndef PGN_
#define PGN_H

#include <utility>
#include <string>
#include <cctype>
#include <regex>
#include <vector>
#include <algorithm>

#include "types.h"
#include "util.h"
#include "encode.h"
#include "decode.h"
#include "board.h"
#include "fen.h"

int v = 0, pc = 0;
int target = 4408;


class PGN {
private:
    void disambiguate_loop(pname&, ptype&, int*, int*, ChessBoard&, bool, bool, bool=false, bool=false);
    bool is_valid_move(ptype, int*, int*, ChessBoard&);
    bool is_valid_piece(ptype, pname, int*, int*, ChessBoard&);

public:
    FENCE fen_parser;

    void parse_file(std::fstream&, std::vector<game>&, std::vector<pgndict>&);
    pgndict parse_tagtext(std::string);
    std::string compact_date(std::string);
    std::string compact_value(std::string);
    int check_mate_promo(std::string&, const char*, int=1);

    game parse_game(std::string, std::string="");
    turn parse_turn(std::string, ChessBoard&);
    ply parse_ply(std::string, ChessBoard&, bool=false, bool=false);

    pname disambiguate(ptype&, int*, int*, ChessBoard&, bool=false, bool=false);
};

void PGN::parse_file(std::fstream &fin, std::vector<game> &games, std::vector<pgndict> &game_tags){

    // Containers for file's text
    std::string line, movetext, tagtext;
    pgndict tags;
    int N;

    std::cout << "Parsing games" << std::endl;

    while(getline(fin, line)){
        N = line.length();
        // Blank line
        if(!N){ continue; }

        else if(!line.compare(0,1,"[")){
            // Line contains tag
            while(N){
                tagtext += line;
                getline(fin, line);
                N = line.length();
            }
            // Collect tag info in `tags` map
            tags = this->parse_tagtext(tagtext);
        }

        else{

            // Line contains movetext
            while(N){
                // Add space at EOL if not already there and doesn't end in '.'
                if(line.compare(N-1, 1, " ") & line.compare(N-1, 1, ".")){ line+= " "; }
                movetext += line;
                if(!getline(fin, line)){ break; }
                N = line.length();
            }

            // Add results to game
            if(tags[fen].length()){ games.push_back(this->parse_game(movetext, tags[fen])); }
            else{ games.push_back(this->parse_game(movetext)); }
            game_tags.push_back(tags);

            // Reset containers for next game
            movetext.clear();
            tagtext.clear();
            tags.clear();
            // if(!((++v)%10000)){ std::cout << v << "... "; }
            std::cout << ++v << std::endl;
        }
    }
}

pgndict PGN::parse_tagtext(std::string tagtext){
    // Tokenize tag lines (eg. `[Event "Cool Chess Tourney, 1920"][Date "1985.10.??"]...`)

    // Extract key/value from tagtext and tokenize them independently
    static pgntag key;
    static std::map<std::string, pgntag>::iterator end = util::s2pgntag.end();
    static std::string key_token;
    char *save_ptr_tagtext, *save_ptr_key, *tag_token, *value_token;

    pgndict tags;
    tag_token = strtok_r(&tagtext[0], util::tag_delims, &save_ptr_tagtext);

    while(tag_token != NULL){

        // Get tag key and make lowercase
        key_token = strtok_r(&tag_token[0], util::key_delims, &save_ptr_key);
        util::lowercase(key_token);

        // Ensure this tag is in target keys - not recording every tag found!
        if(util::s2pgntag.find(key_token) != end){

            // Enum and value token
            key = util::s2pgntag[key_token];
            value_token = strtok_r(NULL, util::value_delims, &save_ptr_key);

            // Remove any ? chars in value string
            if(value_token != NULL){ tags[key] = (key == date) ? this->compact_date(value_token) : this->compact_value(value_token); }
        }

        tag_token = strtok_r(NULL, util::tag_delims, &save_ptr_tagtext);
    }

    return tags;
}

std::string PGN::compact_date(std::string date){
    static const char rem = '?';
    if(date[0] == rem){ date = ""; }
    else if(date[5] == rem){ date = date.substr(0,4); }
    else if(date[8] == rem){ date = date.substr(0,7); }
    return date;
}

std::string PGN::compact_value(std::string val){ val.erase(std::remove(val.begin(), val.end(), '?'), val.end()); return val; }

game PGN::parse_game(std::string movetext, std::string fen){
    // if(v>=target){ std::cout << movetext << std::endl << std::endl; }
    
    std::vector<turn> turns;
    std::string turn_string;

    // New game board
    ChessBoard board;
    if(fen.length()){ this->fen_parser.parse_fen_string(fen, board); }
    if(v>=target){ board.print_board(); }

    // Regex iterators to delimit on move numbers
    static std::regex_token_iterator<std::string::iterator> end;
    std::regex_token_iterator<std::string::iterator> rtok(movetext.begin(), movetext.end(), util::move_num_r, -1);

    while(rtok!=end){
        turn_string = *rtok++;
        if(turn_string.length()){ turns.push_back(this->parse_turn(turn_string, board)); }
    }

    return {turns};
}

turn PGN::parse_turn(std::string turn_string, ChessBoard& board){
    // if(v>=target){ std::cout << turn_string << std::endl;}
    std::string wtok, btok, otok, g;
    static std::string *tokptrs[]{&wtok, &btok, &otok};
    static bool missing, btok_outcome, wtok_outcome;

    // Add space if continuation (..) is found at beginning of string
    if(turn_string.substr(0,2) == ".."){ turn_string.insert(turn_string.begin()+2, ' '); }

    char *token = strtok(&turn_string[0], " "); int i = 0;
    while(token != NULL){
        missing = std::regex_match(token, util::missing_ply);
        *tokptrs[i++] = missing ? "" : token;
        token = strtok(NULL, " ");
    }
    btok_outcome = std::regex_match(btok, util::game_result);
    wtok_outcome = std::regex_match(wtok, util::game_result);
    g = otok.size() ? otok : (btok_outcome ? btok : (wtok_outcome ? wtok : "*"));

    return {
        ((wtok.size()>0) & !wtok_outcome) ? this->parse_ply(wtok, board, false) : ply(),
        ((btok.size()>0) & !btok_outcome) ? this->parse_ply(btok, board, true) : ply(),
        util::game_result_values[g]
    };
}

ply PGN::parse_ply(std::string ply_string, ChessBoard& board, bool black, bool update_board){
    if(v>=target){ std::cout << ++pc << (black ? "b: " : "w: ") << ply_string << std::endl; }
    int src[2]{-1, -1};
    int dst[2]{-1, -1};
    int castle = 0;
    bool check = false, mate = false, capture = false;
    ptype piece_type = pawn, promo = pawn;
    pname idx;

    check = (bool) this->check_mate_promo(ply_string, "+");
    mate = (bool) this->check_mate_promo(ply_string, "#");
    promo = static_cast<ptype>(this->check_mate_promo(ply_string, "=", 2));

    // Castle
    bool kingside = !ply_string.compare("O-O");
    bool queenside = !ply_string.compare("O-O-O");
    if(kingside | queenside){

        idx = static_cast<pname>(white_king_e + (black * 16));
        piece_type = king;

        src[0] = board[idx].first;
        dst[0] = src[0];

        src[1] = board[idx].second;
        dst[1] = src[1] + (queenside ? -2 : 2);

        castle = (kingside * 1) + (queenside * -1);
    }
    else{
        // Parse ranks and files separately
        // Use pointers to src and dst
        int *ranks[2]{&dst[0], &src[0]};
        int *files[2]{&dst[1], &src[1]};
        int f = 0, r = 0;
        char c;

        // Iterate over ply and collect characters into relevant containers
        for(std::string::reverse_iterator it=ply_string.rbegin(); it!=ply_string.rend(); ++it){
            c = *it;
            if(util::ispiece(c)){ piece_type = util::c2ptype[c]; }
            else if(util::isfile(c)){ *files[f++] = util::c2file[c]; }
            else if(isdigit(c)){ *ranks[r++] = (c - '0') - 1; }
            else if(c == 'x'){ capture = true; }
            else{ std::cout << "Unrecognized character: " << c << std::endl; std::cout << ply_string << std::endl; }
        }

        // Remove ambiguity - determine missing src values and return piece name
        idx = this->disambiguate(piece_type, src, dst, board, capture, black);
    }

    // Collect results into ply object
    ply res{pce{idx, src}, dst[0], dst[1], castle, capture, check, mate, promo};

    // Update board
    board.update(res);
    if(v>=target){ board.print_board(); }

    return res;
}

int PGN::check_mate_promo(std::string &str, const char *c, int l){

    static size_t mod;
    static int retval;

    mod = str.find(c);
    if(mod != std::string::npos){
        retval = (l==2) ? (int) util::c2ptype[str[mod+1]] : 1;
        str.erase(mod, l);
    }
    else{ retval = 0; }

    return retval;
}

pname PGN::disambiguate(ptype &piece_type, int *src, int *dst, ChessBoard &board, bool capture, bool black){

    if(src[0]!=-1 & src[1]!=-1){ return board.board[src[0]][src[1]]->name; }

    static pname idx;

    // Kings are always unambiguous
    if(piece_type == king){
        idx = black ? black_king_e : white_king_e;
        src[0] = board[idx].first;
        src[1] = board[idx].second;
    }

    // Pawns are close to unambiguous
    else if(piece_type == pawn){
        int one = 1 - (2*black), two = 2 - (4*black);
        if(capture){ src[0] = dst[0]-one; }
        else{
            src[1] = dst[1];
            src[0] = (board.board[dst[0]-one][dst[1]]->type == pawn) ? dst[0]-one : dst[0]-two;
        }
        idx = board.board[src[0]][src[1]]->name;
    }

    // Get all pieces of the right type and check if they can execute the given move
    else{
        std::vector<pname> candidates;
        for(int i=0; i<15; i++){
            idx = static_cast<pname>(i + (16*black));
            if(board.pieces[idx].captured | board.pieces[idx].type!=piece_type){ continue; }
            else{ candidates.push_back(idx); }
        }
        for(std::vector<pname>::iterator it=candidates.begin(); it!=candidates.end(); ++it){
            idx = *it;
            if(this->is_valid_piece(piece_type, idx, src, dst, board)){ break; }
        }
    }
    return idx;
}

bool PGN::is_valid_move(ptype piece_type, int *src, int *delta, ChessBoard &board){
    // Knight moves don't require clear path
    if(piece_type == knight){
        delta[0] = abs(delta[0]); delta[1] = abs(delta[1]);
        return ((delta[0]==2) ^ (delta[1]==2)) & ((delta[0]==1) ^ (delta[1]==1));
    }
    // If candidate piece isn't a knight, then delta must indicate a rook or bishop move
    else if(piece_type==bishop && !(abs(delta[0]) == abs(delta[1]))){ return false; }
    else if(piece_type==rook && !(!delta[0] ^ !delta[1])){ return false; }
    else if(piece_type==queen && !((!delta[0] ^ !delta[1]) ^ (abs(delta[0]) == abs(delta[1])))){ return false; }


    int m = delta[1] ? delta[0]/delta[1] : 2;
    int rstep = m ? 1 - (2 * (delta[0]<0)) : 0;
    int fstep = (m<2) ? 1 - (2 * (delta[1]<0)) : 0;

    for(int r=src[0]+rstep, f=src[1]+fstep; r!=src[0]+delta[0] | f!=src[1]+delta[1]; r+=rstep, f+=fstep){
        if(board.board[r][f]->name != NONAME){ return false; }
    }
    return true;

}

bool PGN::is_valid_piece(ptype piece_type, pname idx, int *src, int *dst, ChessBoard &board){
    static int delta[2], curr[2];

    curr[0] = board[idx].first;
    curr[1] = board[idx].second;

    delta[0] = dst[0]-curr[0];
    delta[1] = dst[1]-curr[1];

    if(src[0]==-1 & src[1]==-1){
        if(this->is_valid_move(piece_type, curr, delta, board)){
            src[0] = curr[0];
            src[1] = curr[1];
            return true;
        }
    }
    else if(curr[0]==src[0] | curr[1]==src[1]){
        src[0] = curr[0];
        src[1] = curr[1];
        return true;
    }
    return false;
}




#endif