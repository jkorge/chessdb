#ifndef UTIL_H
#define UTIL_H

#include <map>
#include <string>
#include <regex>
#include <algorithm>
#include <cctype>

#include "types.h"



namespace util{

/**************************
    UTIL VARS
**************************/

    /*
        GAME
    */

    // Regex for game results (1-0, 0-1, 1/2-1/2, *)
    static const std::regex game_result("\\s*((1/)*[0-9]-[0-9](/2)*|\\*)\\s*");
    static std::map<std::string, int> game_result_values{
        {"1-0", 1},
        {"0-1", -1},
        {"1/2-1/2", 2},
        {"*", 0}
    };

    // Regex for continuation (eg. "1... Rc5" implies white's move is missing)
    static const std::regex missing_ply("(-{2}|\\.{2})");

    // Get string from pname
    std::map<pname, std::string> pname2s{
        {white_pawn_a, "white_pawn_a"},
        {white_pawn_b, "white_pawn_b"},
        {white_pawn_c, "white_pawn_c"},
        {white_pawn_d, "white_pawn_d"},
        {white_pawn_e, "white_pawn_e"},
        {white_pawn_f, "white_pawn_f"},
        {white_pawn_g, "white_pawn_g"},
        {white_pawn_h, "white_pawn_h"},
        {white_knight_b, "white_knight_b"},
        {white_knight_g, "white_knight_g"},
        {white_bishop_c, "white_bishop_c"},
        {white_bishop_f, "white_bishop_f"},
        {white_rook_a, "white_rook_a"},
        {white_rook_h, "white_rook_h"},
        {white_queen_d, "white_queen_d"},
        {white_king_e, "white_king_e"},

        {black_pawn_a, "black_pawn_a"},
        {black_pawn_b, "black_pawn_b"},
        {black_pawn_c, "black_pawn_c"},
        {black_pawn_d, "black_pawn_d"},
        {black_pawn_e, "black_pawn_e"},
        {black_pawn_f, "black_pawn_f"},
        {black_pawn_g, "black_pawn_g"},
        {black_pawn_h, "black_pawn_h"},
        {black_knight_b, "black_knight_b"},
        {black_knight_g, "black_knight_g"},
        {black_bishop_c, "black_bishop_c"},
        {black_bishop_f, "black_bishop_f"},
        {black_rook_a, "black_rook_a"},
        {black_rook_h, "black_rook_h"},
        {black_queen_d, "black_queen_d"},
        {black_king_e, "black_king_e"},

        {NONAME, "none"}
    };


    /*
        BOARD
    */

    // Map pieces to their starting positions for a standard game of chess
    static const std::map<pname, ipair> start_coords{
        {white_pawn_a, std::make_pair(1,0)},
        {white_pawn_b, std::make_pair(1,1)},
        {white_pawn_c, std::make_pair(1,2)},
        {white_pawn_d, std::make_pair(1,3)},
        {white_pawn_e, std::make_pair(1,4)},
        {white_pawn_f, std::make_pair(1,5)},
        {white_pawn_g, std::make_pair(1,6)},
        {white_pawn_h, std::make_pair(1,7)},
        {white_knight_b, std::make_pair(0,1)},
        {white_knight_g, std::make_pair(0,6)},
        {white_bishop_c, std::make_pair(0,2)},
        {white_bishop_f, std::make_pair(0,5)},
        {white_rook_a, std::make_pair(0,0)},
        {white_rook_h, std::make_pair(0,7)},
        {white_queen_d, std::make_pair(0,3)},
        {white_king_e, std::make_pair(0,4)},

        {black_pawn_a, std::make_pair(6,0)},
        {black_pawn_b, std::make_pair(6,1)},
        {black_pawn_c, std::make_pair(6,2)},
        {black_pawn_d, std::make_pair(6,3)},
        {black_pawn_e, std::make_pair(6,4)},
        {black_pawn_f, std::make_pair(6,5)},
        {black_pawn_g, std::make_pair(6,6)},
        {black_pawn_h, std::make_pair(6,7)},
        {black_knight_b, std::make_pair(7,1)},
        {black_knight_g, std::make_pair(7,6)},
        {black_bishop_c, std::make_pair(7,2)},
        {black_bishop_f, std::make_pair(7,5)},
        {black_rook_a, std::make_pair(7,0)},
        {black_rook_h, std::make_pair(7,7)},
        {black_queen_d, std::make_pair(7,3)},
        {black_king_e, std::make_pair(7,4)}
    };

    /*
        PGN/SAN
    */

    // Piece and file identifiers
    static const std::string pieces = "KQRBN";
    static const std::string files = "abcdefgh";

    // Delimiters for parsing PGN tags
    static const char tag_delims[]{"[]"};
    static const char key_delims[]{" "};
    static const char value_delims[]{"\""};

    // Delimiter for parsing moves in SAN
    static const std::regex move_num_r("(\\d{1,}\\.)");

    // Get file from char
    std::map<char, int> c2file{
        {'a', 0},
        {'b', 1},
        {'c', 2},
        {'d', 3},
        {'e', 4},
        {'f', 5},
        {'g', 6},
        {'h', 7}
    };

    // Get char from file
    std::map<int, char> file2c{
        {0, 'a'},
        {1, 'b'},
        {2, 'c'},
        {3, 'd'},
        {4, 'e'},
        {5, 'f'},
        {6, 'g'},
        {7, 'h'}
    };

    // Get ptype from char
    std::map<char, ptype> c2ptype{
        {'K', king},
        {'Q', queen},
        {'R', rook},
        {'B', bishop},
        {'N', knight},
        {'^', pawn},
        {'_', NOTYPE}
    };

    // Get char from ptype
    std::map<ptype, char> ptype2c{
        {king, 'K'},
        {queen, 'Q'},
        {rook, 'R'},
        {bishop, 'B'},
        {knight, 'N'},
        {pawn, '^'},
        {NOTYPE, '_'}
    };

    // pytpes as strings for iostreams
    std::map<ptype, std::string> ptype2s{
        {king, "king"},
        {queen, "queen"},
        {rook, "rook"},
        {bishop, "bishop"},
        {knight, "knight"},
        {pawn, "pawn"},
        {NOTYPE, "none"}
    };

    // Get pgntag from string
    std::map<std::string, pgntag> s2pgntag{
        {"event", event},
        {"site", site},
        {"date", date},
        {"round", round},
        {"white", white},
        {"black", black},
        {"result", result},
        {"eco", eco},
        {"fen", fen},
        {"mode", mode},
        {"timecontrol", time_control},
        {"termination", termination},
        {"whiteelo", white_elo},
        {"blackelo", black_elo},
        {"whiteuscf", white_uscf},
        {"blackuscf", black_uscf}
    };

    // Get string from pgntag
    std::map<pgntag, std::string> pgntag2s{
        {event, "event"},
        {site, "site"},
        {date, "date"},
        {round, "round"},
        {white, "white"},
        {black, "black"},
        {result, "result"},
        {eco, "eco"},
        {fen, "fen"},
        {mode, "mode"},
        {time_control, "timecontrol"},
        {termination, "termination"},
        {white_elo, "whiteelo"},
        {black_elo, "blackelo"},
        {white_uscf, "whiteuscf"},
        {black_uscf, "blackuscf"}
    };

/**************************
    FUNCS
**************************/

    bool ispiece(char c){
        if(pieces.find(c) != std::string::npos){ return true; }
        else{ return false; }
    }

    bool isfile(char c){
        if(files.find(c) != std::string::npos){ return true; }
        else{ return false; }
    }

    ptype name2type(pname pn){
        int v = pn%16;
        return static_cast<ptype>(v/8 + v/10 + v/12 + v/14 + v/15);
    }

    void uppercase(char *start, char *end){ std::transform(start, end, start, toupper); }

    void uppercase(std::string& str){ std::transform(str.begin(), str.end(), str.begin(), toupper); }

    void lowercase(char *start, char *end){ std::transform(start, end, start, tolower); }

    void lowercase(std::string& str){ std::transform(str.begin(), str.end(), str.begin(), tolower); }

}


#endif