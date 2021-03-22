#include "include/util.hpp"

color operator!(const color& c){ return static_cast<color>(black * c); }

namespace util{
    namespace transform{
        // Char to bool
        template<typename CharT>
        bool ispiece(CharT c){ return util::constants::pieces.find(c) != std::string::npos; }

        template<typename CharT>
        bool isfile(CharT c){ return util::constants::files.find(c) != std::string::npos; }

        // Change case
        template<typename CharT>
        void uppercase(CharT *start, CharT *end){ std::transform(start, end, start, toupper); }

        template<typename CharT>
        void uppercase(std::basic_string<CharT>& s){ std::transform(s.begin(), s.end(), s.begin(), toupper); }

        template<typename CharT>
        void lowercase(CharT *start, CharT *end){ std::transform(start, end, start, tolower); }

        template<typename CharT>
        void lowercase(std::basic_string<CharT>& s){ std::transform(s.begin(), s.end(), s.begin(), tolower); }

        // Flip least-significant bit
        void lsbflip(U64& x){ x &= x-1; }


        // Coordinate conversions
        namespace {
            // Index of least significant 1-bit (aka square number)
            square bitscan_(const U64& src){ return _tzcnt_u64(src); }

            // sq => sq
            square sq_(const square& src){ return src; }

            // mask => sq
            square sq_(const U64& src){ return bitscan_(src); }

            // coords => sq
            square sq_(const coords& src){ return (8*src[0]) + src[1]; }

            // sq => mask
            U64 mask_(const square& src){ return util::constants::ONE_ << src; }

            // mask => mask
            U64 mask_(const U64& src){ return src; }

            // coords => mask
            U64 mask_(const coords& src){ return mask_(sq_(src)); }

            // sq => coords
            coords rf_(const square& src){ return {src/8, src%8}; }

            // mask => coords
            coords rf_(const U64& src){ return rf_(sq_(src)); }

            // coords => coords
            coords rf_(const coords& src){ return src; }
        }

        // Generic functions to route to preceding
        template<typename T>
        square sq(const T& src){ return sq_(src); }

        template<typename T>
        U64 mask(const T& src){ return mask_(src); }

        template<typename T>
        coords rf(const T& src){ return rf_(src); }

        template<typename T>
        square bitscan(const T& src){ return bitscan_(mask(src)); }

        square bitscan(const U64& src){ return bitscan_(src); }
    }
    namespace repr{

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

        // Get string from pname
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

        // Get string from coordinates; Returns string "(r,f)" where r = rank and f = file
        std::string coord2s_(const coords& src){ return std::string("(") + std::to_string(src[0]) + ", " + std::to_string(src[1]) + ')'; }

        template<typename T>
        std::string coord2s(const T& src){ return coord2s_(util::transform::rf(src)); }
    }
    namespace pgn{

        // Regex for newline NOT preceded by dot or space
        std::regex nds("[^\\.\\s]\\n");

        // Get pgntag from string
        std::map<std::string, pgntag> s2tag = {
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
        std::map<std::string, pgntag>::iterator stend = s2tag.end();

        // Get string from pgntag
        std::map<pgntag, std::string> tag2s = {
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

        std::string pgndict2s(const pgndict& pd){
            std::string res;
            for(pgndict::const_iterator it=pd.begin(); it!=pd.end(); ++it){
                res += tag2s[it->first] + ": " + it->second + util::constants::endl;
            }
            return res;
        }
    }
    namespace fen{
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

/*
    TEMPLATE INSTANTIATIONS
*/
template bool util::transform::ispiece(char);
template bool util::transform::ispiece(wchar_t);

template bool util::transform::isfile(char);
template bool util::transform::isfile(wchar_t);

template void util::transform::uppercase(char*, char*);
template void util::transform::uppercase(wchar_t*, wchar_t*);
template void util::transform::uppercase(std::basic_string<char>&);
template void util::transform::uppercase(std::basic_string<wchar_t>&);

template void util::transform::lowercase(char*, char*);
template void util::transform::lowercase(wchar_t*, wchar_t*);
template void util::transform::lowercase(std::basic_string<char>&);
template void util::transform::lowercase(std::basic_string<wchar_t>&);

template square util::transform::sq(const square&);
template square util::transform::sq(const U64&);
template square util::transform::sq(const coords&);

template U64 util::transform::mask(const square&);
template U64 util::transform::mask(const U64&);
template U64 util::transform::mask(const coords&);

template coords util::transform::rf(const square&);
template coords util::transform::rf(const U64&);
template coords util::transform::rf(const coords&);

template square util::transform::bitscan(const square&);
template square util::transform::bitscan(const coords&);

template std::string util::repr::coord2s(const square&);
template std::string util::repr::coord2s(const U64&);
template std::string util::repr::coord2s(const coords&);