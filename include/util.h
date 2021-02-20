#ifndef UTIL_H
#define UTIL_H

#include <cctype>
#include <string>
#include <map>
#include <regex>
#include <algorithm>

#include "types.h"

namespace util{
    /*
        UTILITY OBJECTS FOR ENTIRE PROJECT
    */

    // Whitespace regex
    const std::regex ws("\\s");

    // Newline string
    const char endl = '\n';

    // Starting positions for a standard game of chess
    const U64 white_pawns = 0x000000000000ff00,
        white_knights = 0x0000000000000042,
        white_bishops = 0x0000000000000024,
        white_rooks = 0x0000000000000081,
        white_queen = 0x0000000000000008,
        white_king = 0x0000000000000010;

    const U64 black_pawns = white_pawns << 40,
        black_knights = white_knights << 56,
        black_bishops =  white_bishops << 56,
        black_rooks =  white_rooks << 56,
        black_queen =  white_queen << 56,
        black_king =  white_king << 56;

    std::map<color,std::map<ptype, U64> > start_coords{
        {
            white,
            {
                {pawn,   white_pawns},
                {knight, white_knights},
                {bishop, white_bishops},
                {rook,   white_rooks},
                {queen,  white_queen},
                {king,   white_king}
            }
        },

        {
            black,
            {
                {pawn,   black_pawns},
                {knight, black_knights},
                {bishop, black_bishops},
                {rook,   black_rooks},
                {queen,  black_queen},
                {king,   black_king}
            }
        }
    };

    // Piece and file identifiers
    const std::string pieces = "KQRBN";
    const std::string files = "abcdefgh";
    const std::string ranks = "12345678";

    // Get file from char
    int c2file(char f){
        switch(f){
            case 'a': return 0;
            case 'b': return 1;
            case 'c': return 2;
            case 'd': return 3;
            case 'e': return 4;
            case 'f': return 5;
            case 'g': return 6;
            default:  return 7;
        }
    }

    // Get char from file
    char file2c(int f){
        switch(f){
            case 0:  return 'a';
            case 1:  return 'b';
            case 2:  return 'c';
            case 3:  return 'd';
            case 4:  return 'e';
            case 5:  return 'f';
            case 6:  return 'g';
            default: return 'h';
        }
    }

    // Get rank from char
    int c2rank(char r){
        switch(r){
            case '1': return 0;
            case '2': return 1;
            case '3': return 2;
            case '4': return 3;
            case '5': return 4;
            case '6': return 5;
            case '7': return 6;
            default:  return 7;
        }
    }

    // Get char from rank
    char rank2c(int r){
        switch(r){
            case 0:  return '1';
            case 1:  return '2';
            case 2:  return '3';
            case 3:  return '4';
            case 4:  return '5';
            case 5:  return '6';
            case 6:  return '7';
            default: return '8';
        }
    }

    // Get ptype from char
    ptype c2ptype(char p){
        switch(p){
            case '^': return pawn;
            case 'N': return knight;
            case 'B': return bishop;
            case 'R': return rook;
            case 'Q': return queen;
            case 'K': return king;
            default:  return NOTYPE;
        }
    }

    // Get char from ptype
    char ptype2c(ptype p){
        switch(p){
            case pawn:   return '^';
            case knight: return 'N';
            case bishop: return 'B';
            case rook:   return 'R';
            case queen:  return 'Q';
            case king:   return 'K';
            default:     return '_';
        }
    }

    // Get string from ptype
    std::string ptype2s(ptype p){
        switch(p){
            case pawn:   return "pawn";
            case knight: return "knight";
            case bishop: return "bishop";
            case rook:   return "rook";
            case queen:  return "queen";
            case king:   return "king";
            default:     return "none";
        }
    }

    std::string pname2s(pname n){
        switch(n){
            case white_rook_a:   return "white_rook_a";
            case white_knight_b: return "white_knight_b";
            case white_bishop_c: return "white_bishop_c";
            case white_queen_d:  return "white_queen_d";
            case white_king_e:   return "white_king_e";
            case white_bishop_f: return "white_bishop_f";
            case white_knight_g: return "white_knight_g";
            case white_rook_h:   return "white_rook_h";

            case white_pawn_a:   return "white_pawn_a";
            case white_pawn_b:   return "white_pawn_b";
            case white_pawn_c:   return "white_pawn_c";
            case white_pawn_d:   return "white_pawn_d";
            case white_pawn_e:   return "white_pawn_e";
            case white_pawn_f:   return "white_pawn_f";
            case white_pawn_g:   return "white_pawn_g";
            case white_pawn_h:   return "white_pawn_h";

            case black_pawn_a:   return "black_pawn_a";
            case black_pawn_b:   return "black_pawn_b";
            case black_pawn_c:   return "black_pawn_c";
            case black_pawn_d:   return "black_pawn_d";
            case black_pawn_e:   return "black_pawn_e";
            case black_pawn_f:   return "black_pawn_f";
            case black_pawn_g:   return "black_pawn_g";
            case black_pawn_h:   return "black_pawn_h";

            case black_rook_a:   return "black_rook_a";
            case black_knight_b: return "black_knight_b";
            case black_bishop_c: return "black_bishop_c";
            case black_queen_d:  return "black_queen_d";
            case black_king_e:   return "black_king_e";
            case black_bishop_f: return "black_bishop_f";
            case black_knight_g: return "black_knight_g";
            case black_rook_h:   return "black_rook_h";

            default:             return "NONAME";
        }
    }

    // Get string from color
    std::string color2s(color c){
        switch(c){
            case white: return "white";
            case black: return "black";
            default:    return "none";
        }
    }

    // Get char from color
    char color2c(color c){
        switch(c){
            case white: return 'w';
            case black: return 'b';
            default:    return ' ';
        }
    }

    // Iterators to end of regex_token_iterator results
    rtok rtokend;
    crtok crtokend;

    template<typename T=int>
    std::string coord2s_(const coords<T>& src){ return std::string("(") + std::to_string(src[0]) + ", " + std::to_string(src[1]) + ')'; }

    template<typename T>
    std::string coord2s(T&& src){ return coord2s_(rf(src)); }

    template<typename CharT>
    bool ispiece(CharT c){ return pieces.find(c) != std::string::npos; }

    template<typename CharT>
    bool isfile(CharT c){ return files.find(c) != std::string::npos; }

    template<typename CharT>
    void uppercase(CharT *start, CharT *end){ std::transform(start, end, start, toupper); }

    template<typename CharT>
    void uppercase(std::basic_string<CharT>& s){ std::transform(s.begin(), s.end(), s.begin(), toupper); }

    template<typename CharT>
    void lowercase(CharT *start, CharT *end){ std::transform(start, end, start, tolower); }

    template<typename CharT>
    void lowercase(std::basic_string<CharT>& s){ std::transform(s.begin(), s.end(), s.begin(), tolower); }

    // Flip least-significant bit
    inline void lsbflip(U64& x){ x &= x-1; }

    namespace pgn{
        /*
            UTILITY OBJECTS FOR PGN PARSING
        */

        // Regex for game results (1-0, 0-1, 1/2-1/2, *)
        const std::regex game_result("\\s*((1/)*[0-9]-[0-9](/2)*|\\*)\\s*");

        // Int values for each outcome
        const std::map<std::string, int> game_result_values{
            {"1-0", 1},
            {"0-1", -1},
            {"1/2-1/2", 2},
            {"*", 0}
        };

        // Regex for elided ply (".." or "--")
        const std::regex missing_ply("(-{2}|\\.{2})");

        // Delimiter for parsing moves in SAN
        const std::regex move_num_r("(\\d{1,}\\.)");

        // Regex for movetext that opens with an elided white ply (e.g. "1... Rd8")
        const std::regex black_starts("\\d{1,}\\.{3}");

        // Regex for newline NOT preceded by dot or space
        std::regex nds("[^\\.\\s]\\n");

        // Delimiters for parsing PGN tags
        const char tag_delims[]{"[]"};
        const char key_delim = ' ';
        const char value_delim = '\"';

        // Get pgntag from string
        std::map<std::string, pgntag> s2tag{
            {"event", event},
            {"site", site},
            {"date", date},
            {"round", round},
            {"white", playerw},
            {"black", playerb},
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

        // Iterator to end of s2tag
        std::map<std::string, pgntag>::iterator stend{s2tag.end()};

        // Get string from pgntag
        std::map<pgntag, std::string> tag2s{
            {event, "event"},
            {site, "site"},
            {date, "date"},
            {round, "round"},
            {playerw, "white"},
            {playerb, "black"},
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
        const std::regex delim("/");

        // FEN string for new (standard) game of chess
        const std::string new_game("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

        ptype c2type(char p){
            switch(p){
                case 'p': return pawn;
                case 'P': return pawn;
                case 'n': return knight;
                case 'N': return knight;
                case 'b': return bishop;
                case 'B': return bishop;
                case 'r': return rook;
                case 'R': return rook;
                case 'q': return queen;
                case 'Q': return queen;
                default:  return king;
            }
        }
    }
}

#endif