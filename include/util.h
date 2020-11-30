#ifndef UTIL_H
#define UTIL_H

#include <map>
#include <string>
#include <regex>
#include <algorithm>
#include <cctype>

#include "types.h"



namespace util{
    /*
        UTILITY OBJECTS FOR ENTIRE PROJECT
    */

    // Whitespace regex
    const std::regex ws("\\s");

    // Get string from pname
    std::map<pname, str> pname2s{
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

    // Map pieces to their starting positions for a standard game of chess
    const std::map<pname, ipair> start_coords{
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

    // Piece and file identifiers
    const str pieces = "KQRBN";
    const str files = "abcdefgh";

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

    // Get char from rank
    std::map<int, char> rank2c{
        {0, '1'},
        {1, '2'},
        {2, '3'},
        {3, '4'},
        {4, '5'},
        {5, '6'},
        {6, '7'},
        {7, '8'}
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
    std::map<ptype, str> ptype2s{
        {king, "king"},
        {queen, "queen"},
        {rook, "rook"},
        {bishop, "bishop"},
        {knight, "knight"},
        {pawn, "pawn"},
        {NOTYPE, "none"}
    };

    /* Map of (square color, piece color) to bishop names
        outer key => flag for dark square
        inner key => flag for black ply
    */
    std::map<bool, std::map<bool, pname>> bmap{
        {true,  {{true, black_bishop_f}, {false, white_bishop_c}}},
        {false, {{true, black_bishop_c}, {false, white_bishop_f}}}
    };

    // True if c in util::pieces
    bool ispiece(char c){ return pieces.find(c) != str::npos; }

    // True if c in util::files
    bool isfile(char c){ return files.find(c) != str::npos; }

    // Compute piece type from its name (doesn't work for promoted pawns)
    ptype name2type(pname pn){
        int v = pn%16;
        return static_cast<ptype>(v/8 + v/10 + v/12 + v/14 + v/15);
    }

    bool inbounds(int r, int f){ return (r>=0 && r<8) && (f>=0 && f<8); }
    bool inbounds(int *dst){ return (dst[0]>=0 && dst[0]<8) && (dst[1]>=0 && dst[1]<8); }
    bool inbounds(ipair dst){ return (dst.first>=0 && dst.first<8) && (dst.second>=0 && dst.second<8); }

    str coord2s(int *src){ return str("(") + std::to_string(src[0]) + ", " + std::to_string(src[1]) + ')'; }

    str coord2s(int r, int f){ return str("(") + std::to_string(r) + ", " + std::to_string(f) + ')'; }

    void uppercase(char *start, char *end){ std::transform(start, end, start, toupper); }

    void uppercase(str& s){ std::transform(s.begin(), s.end(), s.begin(), toupper); }

    void lowercase(char *start, char *end){ std::transform(start, end, start, tolower); }

    void lowercase(str& s){ std::transform(s.begin(), s.end(), s.begin(), tolower); }

    namespace pgn{
        /*
            UTILITY OBJECTS FOR PGN PARSING
        */

        // Regex for game results (1-0, 0-1, 1/2-1/2, *)
        const std::regex game_result("\\s*((1/)*[0-9]-[0-9](/2)*|\\*)\\s*");

        // Int values for each outcome
        const std::map<str, int> game_result_values{
            {"1-0", 1},
            {"0-1", -1},
            {"1/2-1/2", 2},
            {"*", 0}
        };

        // Regex for continuation (eg. "1... Rc5" implies white's move is missing - usually preceded by FEN tag)
        const std::regex missing_ply("(-{2}|\\.{2})");

        // Delimiters for parsing PGN tags
        const char tag_delims[]{"[]"};
        const char key_delim = ' ';
        const char value_delim = '\"';

        // Delimiter for parsing moves in SAN
        const std::regex move_num_r("(\\d{1,}\\.)");

        // Get pgntag from string
        std::map<str, pgntag> s2tag{
            {"event", event},
            {"site", site},
            {"date", date},
            {"round", round},
            {"white", white},
            {"black", black},
            {"result", result},
            {"eco", eco},
            {"fen", fenstr},
            {"mode", mode},
            {"timecontrol", time_control},
            {"termination", termination},
            {"whiteelo", white_elo},
            {"blackelo", black_elo},
            {"whiteuscf", white_uscf},
            {"blackuscf", black_uscf}
        };

        // Get string from pgntag
        std::map<pgntag, str> tag2s{
            {event, "event"},
            {site, "site"},
            {date, "date"},
            {round, "round"},
            {white, "white"},
            {black, "black"},
            {result, "result"},
            {eco, "eco"},
            {fenstr, "fen"},
            {mode, "mode"},
            {time_control, "timecontrol"},
            {termination, "termination"},
            {white_elo, "whiteelo"},
            {black_elo, "blackelo"},
            {white_uscf, "whiteuscf"},
            {black_uscf, "blackuscf"}
        };
    }

    namespace fen{
        /*
            UTILITY OBJECTS FOR FEN PARSING
        */

        const char black_pieces[7]{"pnbrqk"};
        const char white_pieces[7]{"PNBRQK"};
        const std::regex delim("/");

        // FEN string for new (standard) game of chess
        const str new_game("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

        std::map<char, ptype> c2type{
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
    }
}

#endif