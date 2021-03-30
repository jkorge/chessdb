#include "include/util.hpp"

color operator!(const color& c){ return static_cast<color>(black * c); }

ptype operator++(ptype& pt){
  pt = pt < 0 ? NOTYPE : static_cast<ptype>((static_cast<int>(pt) + 1) % 6);
  return pt;
}
ptype operator++(ptype& pt, int){
  ptype tmp = pt;
  ++pt;
  return tmp;
}

direction operator++(direction& dir){
  dir = static_cast<direction>((static_cast<int>(dir) + 1) % 8);
  return dir;
}
direction operator++(direction& dir, int){
  direction tmp = dir;
  ++dir;
  return tmp;
}
coords operator+(const coords& x, const coords& y){ return {x[0] + y[0], x[1] + y[1]}; }

namespace util{
    namespace constants{
        // Whitespace regex
        const std::regex wsr("\\s");

        // Whitespace string
        const std::string ws(1, ' ');

        const std::map<color,std::map<ptype, U64> > start_coords{
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

        // Map castle values to rook starting locations
        const std::unordered_map<int, std::unordered_map<color, U64> > rook_castle{
            {
                1,
                {
                    {white, 0x0000000000000080},
                    {black, 0x8000000000000000}
                }
            },

            {
                -1,
                {
                    {white, 0x0000000000000001},
                    {black, 0x0100000000000000}
                }
            }
        };

        // Piece and file identifiers
        const std::string pieces = "KQRBN";
        const std::string files = "abcdefgh";
        const std::string ranks = "12345678";
    }
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

            // Index of most significant 1-bit
            square bitscanr_(const U64& src){ return 63 - _lzcnt_u64(src); }

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
        square bitscan(const T& src){ return bitscan_(mask(src)); }

        template<typename T>
        square bitscanr(const T& src){ return bitscanr_(mask(src)); }

        square bitscan(const U64& src){ return bitscan_(src); }

        square bitscanr(const U64& src){ return bitscanr_(src); }

        template<typename T>
        square sq(const T& src){ return sq_(src); }

        template<typename T>
        U64 mask(const T& src){ return mask_(src); }

        template<typename T>
        coords rf(const T& src){ return rf_(src); }
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

        // Get string from coordinates; Returns string "(r, f)" where r = rank and f = file
        // std::string coord2s_(const coords& src){ return std::string("(") + std::to_string(src[0]) + ", " + std::to_string(src[1]) + ')'; }
        std::string coord2s_(const coords& src){ return std::string(1, file2c(src[1])) + rank2c(src[0]); }

        template<typename T>
        std::string coord2s(const T& src){ return coord2s_(util::transform::rf(src)); }
    }
    namespace bitboard{

        // Bitmap of squares attacked by pt when positioned on src (ASSUMES EMPTY BOARD)
        template<typename T>
        U64 attackfrom(const T& src, ptype pt, color c){
            square ssq = util::transform::sq(src);
            switch(pt){
                case pawn:
                    switch(c){
                        case white: return util::bitboard::wpattack[ssq];
                        default:    return util::bitboard::bpattack[ssq];
                    }
                case knight:        return util::bitboard::nattack[ssq];
                case bishop:        return util::bitboard::battack[ssq];
                case rook:          return util::bitboard::rattack[ssq];
                case queen:         return util::bitboard::qattack[ssq];
                default:            return util::bitboard::kattack[ssq];
            }
        }

        // Determine if two points are colinear
        template<typename Ts, typename Td>
        bool colinear(const Ts& src, const Td& dst){
            int ssq = util::transform::sq(src),
                dsq = util::transform::sq(dst);

            if(util::bitboard::lines[ssq][dsq]){ return true; }
            else                     { return false; }
        }

        // Determine direction of line connecting two points
        template<typename Ts, typename Td>
        ptype linetype(const Ts& src, const Td& dst){
            int ssq = util::transform::sq(src),
                dsq = util::transform::sq(dst);

            ptype res;

            if(
                (util::bitboard::dmasks[ssq] & util::bitboard::dmasks[dsq]) ||
                (util::bitboard::amasks[ssq] & util::bitboard::amasks[dsq])
            ){ res = bishop; }
            else if(
                (util::bitboard::rmasks[ssq] & util::bitboard::rmasks[dsq]) ||
                (util::bitboard::fmasks[ssq] & util::bitboard::fmasks[dsq])
            ){ res = rook; }
            else{ res = NOTYPE; }

            return res;
        }

        // Determine direction of line given as bitmap
        ptype linetype(const U64& src){
            square sq = util::transform::bitscan(src);
            if     (src == (src & util::bitboard::dmasks[sq])){ return bishop; }
            else if(src == (src & util::bitboard::amasks[sq])){ return bishop; }
            else if(src == (src & util::bitboard::rmasks[sq])){ return rook; }
            else if(src == (src & util::bitboard::fmasks[sq])){ return rook; }
            else                                              { return NOTYPE; }
        }

        // Return bitmap of axis (rank, file, diagonal, or antidiagonal) colinear to src, a bitmap of a line; if none return 0
        U64 axisalign(const U64& src){
            square sq = util::transform::bitscan(src);
            if     (src == (src & util::bitboard::dmasks[sq])){ return util::bitboard::dmasks[sq]; }
            else if(src == (src & util::bitboard::amasks[sq])){ return util::bitboard::amasks[sq]; }
            else if(src == (src & util::bitboard::rmasks[sq])){ return util::bitboard::rmasks[sq]; }
            else if(src == (src & util::bitboard::fmasks[sq])){ return util::bitboard::fmasks[sq]; }
            else                                              { return 0; }
        }

        // Bitmap of line between src and dst
        template<typename Ts, typename Td>
        U64 linebt(const Ts& src, const Td& dst, bool endp){
            U64 res = util::bitboard::lines[util::transform::sq(src)][util::transform::sq(dst)];
            if(endp){ res |= util::transform::mask(src) | util::transform::mask(dst); }
            return res;
        }

        std::string odisplay(const U64& _bb, char c){
            std::string viz = util::bitboard::bbviz;
            U64 bb = _bb;
            while(bb){
                square x = util::transform::bitscan(bb);
                int idx = util::bitboard::bbvizidx(x);
                viz[idx] = c;
                util::transform::lsbflip(bb);
            }
            return viz;
        }

        int bvizidx(square sq){ return 36 + 35*(7 - sq/8) + 4*(sq%8); }

        int bbvizidx(square sq){ return 2 + 25*(7 - sq/8) + 3*(sq%8); }

        namespace {
            template<square sq>
            struct rmask{ const static U64 value = util::constants::RANK_ << (sq & 56); };

            template<square sq>
            struct fmask{ const static U64 value = util::constants::FILE_ << (sq & 7); };

            template<square sq>
            struct dmask{
                const static int diag = 8*(sq & 7) - (sq & 56);
                const static int nort = -diag & ( diag >> 31);
                const static int sout =  diag & (-diag >> 31);
                const static U64 value = (util::constants::MAINDIAG_ >> sout) << nort;
            };

            template<square sq>
            struct amask{
                const static int diag = 56 - 8*(sq & 7) - (sq & 56);
                const static int nort = -diag & ( diag >> 31);
                const static int sout =  diag & (-diag >> 31);
                const static U64 value = (util::constants::ANTIDIAG_ >> sout) << nort;
            };

        }

        // Bitmaps of board axes
        const std::array<U64, 64> rmasks = {
            rmask<0>::value, rmask<1>::value, rmask<2>::value, rmask<3>::value, rmask<4>::value, rmask<5>::value, rmask<6>::value, rmask<7>::value,
            rmask<8>::value, rmask<9>::value, rmask<10>::value, rmask<11>::value, rmask<12>::value, rmask<13>::value, rmask<14>::value, rmask<15>::value,
            rmask<16>::value, rmask<17>::value, rmask<18>::value, rmask<19>::value, rmask<20>::value, rmask<21>::value, rmask<22>::value, rmask<23>::value,
            rmask<24>::value, rmask<25>::value, rmask<26>::value, rmask<27>::value, rmask<28>::value, rmask<29>::value, rmask<30>::value, rmask<31>::value,
            rmask<32>::value, rmask<33>::value, rmask<34>::value, rmask<35>::value, rmask<36>::value, rmask<37>::value, rmask<38>::value, rmask<39>::value,
            rmask<40>::value, rmask<41>::value, rmask<42>::value, rmask<43>::value, rmask<44>::value, rmask<45>::value, rmask<46>::value, rmask<47>::value,
            rmask<48>::value, rmask<49>::value, rmask<50>::value, rmask<51>::value, rmask<52>::value, rmask<53>::value, rmask<54>::value, rmask<55>::value,
            rmask<56>::value, rmask<57>::value, rmask<58>::value, rmask<59>::value, rmask<60>::value, rmask<61>::value, rmask<62>::value, rmask<63>::value
        };

        const std::array<U64, 64> fmasks = {
            fmask<0>::value, fmask<1>::value, fmask<2>::value, fmask<3>::value, fmask<4>::value, fmask<5>::value, fmask<6>::value, fmask<7>::value,
            fmask<8>::value, fmask<9>::value, fmask<10>::value, fmask<11>::value, fmask<12>::value, fmask<13>::value, fmask<14>::value, fmask<15>::value,
            fmask<16>::value, fmask<17>::value, fmask<18>::value, fmask<19>::value, fmask<20>::value, fmask<21>::value, fmask<22>::value, fmask<23>::value,
            fmask<24>::value, fmask<25>::value, fmask<26>::value, fmask<27>::value, fmask<28>::value, fmask<29>::value, fmask<30>::value, fmask<31>::value,
            fmask<32>::value, fmask<33>::value, fmask<34>::value, fmask<35>::value, fmask<36>::value, fmask<37>::value, fmask<38>::value, fmask<39>::value,
            fmask<40>::value, fmask<41>::value, fmask<42>::value, fmask<43>::value, fmask<44>::value, fmask<45>::value, fmask<46>::value, fmask<47>::value,
            fmask<48>::value, fmask<49>::value, fmask<50>::value, fmask<51>::value, fmask<52>::value, fmask<53>::value, fmask<54>::value, fmask<55>::value,
            fmask<56>::value, fmask<57>::value, fmask<58>::value, fmask<59>::value, fmask<60>::value, fmask<61>::value, fmask<62>::value, fmask<63>::value
        };

        const std::array<U64, 64> dmasks = {
            dmask<0>::value, dmask<1>::value, dmask<2>::value, dmask<3>::value, dmask<4>::value, dmask<5>::value, dmask<6>::value, dmask<7>::value,
            dmask<8>::value, dmask<9>::value, dmask<10>::value, dmask<11>::value, dmask<12>::value, dmask<13>::value, dmask<14>::value, dmask<15>::value,
            dmask<16>::value, dmask<17>::value, dmask<18>::value, dmask<19>::value, dmask<20>::value, dmask<21>::value, dmask<22>::value, dmask<23>::value,
            dmask<24>::value, dmask<25>::value, dmask<26>::value, dmask<27>::value, dmask<28>::value, dmask<29>::value, dmask<30>::value, dmask<31>::value,
            dmask<32>::value, dmask<33>::value, dmask<34>::value, dmask<35>::value, dmask<36>::value, dmask<37>::value, dmask<38>::value, dmask<39>::value,
            dmask<40>::value, dmask<41>::value, dmask<42>::value, dmask<43>::value, dmask<44>::value, dmask<45>::value, dmask<46>::value, dmask<47>::value,
            dmask<48>::value, dmask<49>::value, dmask<50>::value, dmask<51>::value, dmask<52>::value, dmask<53>::value, dmask<54>::value, dmask<55>::value,
            dmask<56>::value, dmask<57>::value, dmask<58>::value, dmask<59>::value, dmask<60>::value, dmask<61>::value, dmask<62>::value, dmask<63>::value
        };

        const std::array<U64, 64> amasks = {
            amask<0>::value, amask<1>::value, amask<2>::value, amask<3>::value, amask<4>::value, amask<5>::value, amask<6>::value, amask<7>::value,
            amask<8>::value, amask<9>::value, amask<10>::value, amask<11>::value, amask<12>::value, amask<13>::value, amask<14>::value, amask<15>::value,
            amask<16>::value, amask<17>::value, amask<18>::value, amask<19>::value, amask<20>::value, amask<21>::value, amask<22>::value, amask<23>::value,
            amask<24>::value, amask<25>::value, amask<26>::value, amask<27>::value, amask<28>::value, amask<29>::value, amask<30>::value, amask<31>::value,
            amask<32>::value, amask<33>::value, amask<34>::value, amask<35>::value, amask<36>::value, amask<37>::value, amask<38>::value, amask<39>::value,
            amask<40>::value, amask<41>::value, amask<42>::value, amask<43>::value, amask<44>::value, amask<45>::value, amask<46>::value, amask<47>::value,
            amask<48>::value, amask<49>::value, amask<50>::value, amask<51>::value, amask<52>::value, amask<53>::value, amask<54>::value, amask<55>::value,
            amask<56>::value, amask<57>::value, amask<58>::value, amask<59>::value, amask<60>::value, amask<61>::value, amask<62>::value, amask<63>::value
        };

        namespace {

            template<square ssq, int rs, int fs>
            struct shift{ const static U64 value = ((ssq/8 + rs)<0 || (ssq/8 + rs)>7 || (ssq%8 + fs)<0 || (ssq%8 + fs)>7) ? util::constants::ZERO_ : (util::constants::ONE_ << (8*(ssq/8 + rs) + (ssq%8 + fs))); };

            template<square sq>
            struct wpatk{ const static U64 value = (sq < 56) ? (shift<sq, 1, -1>::value | shift<sq, 1,  1>::value) : util::constants::ZERO_; };

            template<square sq>
            struct bpatk{ const static U64 value = (sq >= 8) ? (shift<sq, -1, -1>::value | shift<sq, -1, 1>::value) : util::constants::ZERO_; };

            template<square sq>
            struct natk{ const static U64 value = shift<sq, 1,2>::value | shift<sq, 1,-2>::value | shift<sq, -1,2>::value | shift<sq, -1,-2>::value | shift<sq, 2,1>::value | shift<sq, 2,-1>::value | shift<sq, -2,1>::value | shift<sq, -2,-1>::value; };

            template<square sq>
            struct batk{ const static U64 value = (util::constants::ONE_ << sq) ^ (util::bitboard::dmasks[sq] | util::bitboard::amasks[sq]); };

            template<square sq>
            struct ratk{ const static U64 value = (util::constants::ONE_ << sq) ^ (util::bitboard::rmasks[sq] | util::bitboard::fmasks[sq]); };

            template<square sq>
            struct qatk{ const static U64 value = batk<sq>::value | ratk<sq>::value; };

            template<square sq>
            struct katk{ const static U64 value = shift<sq, -1,-1>::value | shift<sq, -1,0>::value | shift<sq, -1,1>::value | shift<sq, 0,-1>::value | shift<sq, 0,1>::value | shift<sq, 1,-1>::value | shift<sq, 1,0>::value | shift<sq, 1,1>::value; };
        }

        const std::array<U64, 64> wpattack = {
            wpatk<0>::value, wpatk<1>::value, wpatk<2>::value, wpatk<3>::value, wpatk<4>::value, wpatk<5>::value, wpatk<6>::value, wpatk<7>::value,
            wpatk<8>::value, wpatk<9>::value, wpatk<10>::value, wpatk<11>::value, wpatk<12>::value, wpatk<13>::value, wpatk<14>::value, wpatk<15>::value,
            wpatk<16>::value, wpatk<17>::value, wpatk<18>::value, wpatk<19>::value, wpatk<20>::value, wpatk<21>::value, wpatk<22>::value, wpatk<23>::value,
            wpatk<24>::value, wpatk<25>::value, wpatk<26>::value, wpatk<27>::value, wpatk<28>::value, wpatk<29>::value, wpatk<30>::value, wpatk<31>::value,
            wpatk<32>::value, wpatk<33>::value, wpatk<34>::value, wpatk<35>::value, wpatk<36>::value, wpatk<37>::value, wpatk<38>::value, wpatk<39>::value,
            wpatk<40>::value, wpatk<41>::value, wpatk<42>::value, wpatk<43>::value, wpatk<44>::value, wpatk<45>::value, wpatk<46>::value, wpatk<47>::value,
            wpatk<48>::value, wpatk<49>::value, wpatk<50>::value, wpatk<51>::value, wpatk<52>::value, wpatk<53>::value, wpatk<54>::value, wpatk<55>::value,
            wpatk<56>::value, wpatk<57>::value, wpatk<58>::value, wpatk<59>::value, wpatk<60>::value, wpatk<61>::value, wpatk<62>::value, wpatk<63>::value
        };

        const std::array<U64, 64> bpattack = {
            bpatk<0>::value, bpatk<1>::value, bpatk<2>::value, bpatk<3>::value, bpatk<4>::value, bpatk<5>::value, bpatk<6>::value, bpatk<7>::value,
            bpatk<8>::value, bpatk<9>::value, bpatk<10>::value, bpatk<11>::value, bpatk<12>::value, bpatk<13>::value, bpatk<14>::value, bpatk<15>::value,
            bpatk<16>::value, bpatk<17>::value, bpatk<18>::value, bpatk<19>::value, bpatk<20>::value, bpatk<21>::value, bpatk<22>::value, bpatk<23>::value,
            bpatk<24>::value, bpatk<25>::value, bpatk<26>::value, bpatk<27>::value, bpatk<28>::value, bpatk<29>::value, bpatk<30>::value, bpatk<31>::value,
            bpatk<32>::value, bpatk<33>::value, bpatk<34>::value, bpatk<35>::value, bpatk<36>::value, bpatk<37>::value, bpatk<38>::value, bpatk<39>::value,
            bpatk<40>::value, bpatk<41>::value, bpatk<42>::value, bpatk<43>::value, bpatk<44>::value, bpatk<45>::value, bpatk<46>::value, bpatk<47>::value,
            bpatk<48>::value, bpatk<49>::value, bpatk<50>::value, bpatk<51>::value, bpatk<52>::value, bpatk<53>::value, bpatk<54>::value, bpatk<55>::value,
            bpatk<56>::value, bpatk<57>::value, bpatk<58>::value, bpatk<59>::value, bpatk<60>::value, bpatk<61>::value, bpatk<62>::value, bpatk<63>::value
        };

        const std::array<U64, 64> nattack = {
            natk<0>::value, natk<1>::value, natk<2>::value, natk<3>::value, natk<4>::value, natk<5>::value, natk<6>::value, natk<7>::value,
            natk<8>::value, natk<9>::value, natk<10>::value, natk<11>::value, natk<12>::value, natk<13>::value, natk<14>::value, natk<15>::value,
            natk<16>::value, natk<17>::value, natk<18>::value, natk<19>::value, natk<20>::value, natk<21>::value, natk<22>::value, natk<23>::value,
            natk<24>::value, natk<25>::value, natk<26>::value, natk<27>::value, natk<28>::value, natk<29>::value, natk<30>::value, natk<31>::value,
            natk<32>::value, natk<33>::value, natk<34>::value, natk<35>::value, natk<36>::value, natk<37>::value, natk<38>::value, natk<39>::value,
            natk<40>::value, natk<41>::value, natk<42>::value, natk<43>::value, natk<44>::value, natk<45>::value, natk<46>::value, natk<47>::value,
            natk<48>::value, natk<49>::value, natk<50>::value, natk<51>::value, natk<52>::value, natk<53>::value, natk<54>::value, natk<55>::value,
            natk<56>::value, natk<57>::value, natk<58>::value, natk<59>::value, natk<60>::value, natk<61>::value, natk<62>::value, natk<63>::value
        };

        const std::array<U64, 64> battack = {
            batk<0>::value, batk<1>::value, batk<2>::value, batk<3>::value, batk<4>::value, batk<5>::value, batk<6>::value, batk<7>::value,
            batk<8>::value, batk<9>::value, batk<10>::value, batk<11>::value, batk<12>::value, batk<13>::value, batk<14>::value, batk<15>::value,
            batk<16>::value, batk<17>::value, batk<18>::value, batk<19>::value, batk<20>::value, batk<21>::value, batk<22>::value, batk<23>::value,
            batk<24>::value, batk<25>::value, batk<26>::value, batk<27>::value, batk<28>::value, batk<29>::value, batk<30>::value, batk<31>::value,
            batk<32>::value, batk<33>::value, batk<34>::value, batk<35>::value, batk<36>::value, batk<37>::value, batk<38>::value, batk<39>::value,
            batk<40>::value, batk<41>::value, batk<42>::value, batk<43>::value, batk<44>::value, batk<45>::value, batk<46>::value, batk<47>::value,
            batk<48>::value, batk<49>::value, batk<50>::value, batk<51>::value, batk<52>::value, batk<53>::value, batk<54>::value, batk<55>::value,
            batk<56>::value, batk<57>::value, batk<58>::value, batk<59>::value, batk<60>::value, batk<61>::value, batk<62>::value, batk<63>::value
        };

        const std::array<U64, 64> rattack = {
            ratk<0>::value, ratk<1>::value, ratk<2>::value, ratk<3>::value, ratk<4>::value, ratk<5>::value, ratk<6>::value, ratk<7>::value,
            ratk<8>::value, ratk<9>::value, ratk<10>::value, ratk<11>::value, ratk<12>::value, ratk<13>::value, ratk<14>::value, ratk<15>::value,
            ratk<16>::value, ratk<17>::value, ratk<18>::value, ratk<19>::value, ratk<20>::value, ratk<21>::value, ratk<22>::value, ratk<23>::value,
            ratk<24>::value, ratk<25>::value, ratk<26>::value, ratk<27>::value, ratk<28>::value, ratk<29>::value, ratk<30>::value, ratk<31>::value,
            ratk<32>::value, ratk<33>::value, ratk<34>::value, ratk<35>::value, ratk<36>::value, ratk<37>::value, ratk<38>::value, ratk<39>::value,
            ratk<40>::value, ratk<41>::value, ratk<42>::value, ratk<43>::value, ratk<44>::value, ratk<45>::value, ratk<46>::value, ratk<47>::value,
            ratk<48>::value, ratk<49>::value, ratk<50>::value, ratk<51>::value, ratk<52>::value, ratk<53>::value, ratk<54>::value, ratk<55>::value,
            ratk<56>::value, ratk<57>::value, ratk<58>::value, ratk<59>::value, ratk<60>::value, ratk<61>::value, ratk<62>::value, ratk<63>::value
        };

        const std::array<U64, 64> qattack = {
            qatk<0>::value, qatk<1>::value, qatk<2>::value, qatk<3>::value, qatk<4>::value, qatk<5>::value, qatk<6>::value, qatk<7>::value,
            qatk<8>::value, qatk<9>::value, qatk<10>::value, qatk<11>::value, qatk<12>::value, qatk<13>::value, qatk<14>::value, qatk<15>::value,
            qatk<16>::value, qatk<17>::value, qatk<18>::value, qatk<19>::value, qatk<20>::value, qatk<21>::value, qatk<22>::value, qatk<23>::value,
            qatk<24>::value, qatk<25>::value, qatk<26>::value, qatk<27>::value, qatk<28>::value, qatk<29>::value, qatk<30>::value, qatk<31>::value,
            qatk<32>::value, qatk<33>::value, qatk<34>::value, qatk<35>::value, qatk<36>::value, qatk<37>::value, qatk<38>::value, qatk<39>::value,
            qatk<40>::value, qatk<41>::value, qatk<42>::value, qatk<43>::value, qatk<44>::value, qatk<45>::value, qatk<46>::value, qatk<47>::value,
            qatk<48>::value, qatk<49>::value, qatk<50>::value, qatk<51>::value, qatk<52>::value, qatk<53>::value, qatk<54>::value, qatk<55>::value,
            qatk<56>::value, qatk<57>::value, qatk<58>::value, qatk<59>::value, qatk<60>::value, qatk<61>::value, qatk<62>::value, qatk<63>::value
        };

        const std::array<U64, 64> kattack = {
            katk<0>::value, katk<1>::value, katk<2>::value, katk<3>::value, katk<4>::value, katk<5>::value, katk<6>::value, katk<7>::value,
            katk<8>::value, katk<9>::value, katk<10>::value, katk<11>::value, katk<12>::value, katk<13>::value, katk<14>::value, katk<15>::value,
            katk<16>::value, katk<17>::value, katk<18>::value, katk<19>::value, katk<20>::value, katk<21>::value, katk<22>::value, katk<23>::value,
            katk<24>::value, katk<25>::value, katk<26>::value, katk<27>::value, katk<28>::value, katk<29>::value, katk<30>::value, katk<31>::value,
            katk<32>::value, katk<33>::value, katk<34>::value, katk<35>::value, katk<36>::value, katk<37>::value, katk<38>::value, katk<39>::value,
            katk<40>::value, katk<41>::value, katk<42>::value, katk<43>::value, katk<44>::value, katk<45>::value, katk<46>::value, katk<47>::value,
            katk<48>::value, katk<49>::value, katk<50>::value, katk<51>::value, katk<52>::value, katk<53>::value, katk<54>::value, katk<55>::value,
            katk<56>::value, katk<57>::value, katk<58>::value, katk<59>::value, katk<60>::value, katk<61>::value, katk<62>::value, katk<63>::value
        };

        namespace {

            template<square s1, square s2>
            struct line{
                const static U64 value = (util::bitboard::battack[s1] & util::constants::ONE_ << s2)
                                       ? (util::bitboard::battack[s1] & util::bitboard::battack[s2] & ((util::constants::ALL_ << s1) ^ (util::constants::ALL_ << s2)))
                                       : (
                                            (util::bitboard::rattack[s1] & util::constants::ONE_ << s2)
                                          ? (util::bitboard::rattack[s1] & util::bitboard::rattack[s2] & ((util::constants::ALL_ << s1) ^ (util::constants::ALL_ << s2)))
                                          : util::constants::ZERO_
                                         );
            };

            template<ptype pt, square sq, direction dir>
            struct ray{
                const static U64 value = (pt == bishop ? util::bitboard::battack[sq] : (pt == rook ? util::bitboard::rattack[sq] : util::bitboard::qattack[sq]))
                                       & (dir%2 ? (dir%5 <= 1 ? util::bitboard::amasks[sq] : util::bitboard::dmasks[sq]) : (!(dir%4) ? util::bitboard::fmasks[sq] :  util::bitboard::rmasks[sq]))
                                       & ((dir > 1 & dir < 6) ? util::constants::ALL_ >> (63-sq) : util::constants::ALL_ << sq);
            };
        }

        const std::array<std::array<U64, 64>, 64> lines = {
            line< 0, 0>::value, line< 0, 1>::value, line< 0, 2>::value, line< 0, 3>::value, line< 0, 4>::value, line< 0, 5>::value, line< 0, 6>::value, line< 0, 7>::value,
            line< 0, 8>::value, line< 0, 9>::value, line< 0,10>::value, line< 0,11>::value, line< 0,12>::value, line< 0,13>::value, line< 0,14>::value, line< 0,15>::value,
            line< 0,16>::value, line< 0,17>::value, line< 0,18>::value, line< 0,19>::value, line< 0,20>::value, line< 0,21>::value, line< 0,22>::value, line< 0,23>::value,
            line< 0,24>::value, line< 0,25>::value, line< 0,26>::value, line< 0,27>::value, line< 0,28>::value, line< 0,29>::value, line< 0,30>::value, line< 0,31>::value,
            line< 0,32>::value, line< 0,33>::value, line< 0,34>::value, line< 0,35>::value, line< 0,36>::value, line< 0,37>::value, line< 0,38>::value, line< 0,39>::value,
            line< 0,40>::value, line< 0,41>::value, line< 0,42>::value, line< 0,43>::value, line< 0,44>::value, line< 0,45>::value, line< 0,46>::value, line< 0,47>::value,
            line< 0,48>::value, line< 0,49>::value, line< 0,50>::value, line< 0,51>::value, line< 0,52>::value, line< 0,53>::value, line< 0,54>::value, line< 0,55>::value,
            line< 0,56>::value, line< 0,57>::value, line< 0,58>::value, line< 0,59>::value, line< 0,60>::value, line< 0,61>::value, line< 0,62>::value, line< 0,63>::value,
            line< 1, 0>::value, line< 1, 1>::value, line< 1, 2>::value, line< 1, 3>::value, line< 1, 4>::value, line< 1, 5>::value, line< 1, 6>::value, line< 1, 7>::value,
            line< 1, 8>::value, line< 1, 9>::value, line< 1,10>::value, line< 1,11>::value, line< 1,12>::value, line< 1,13>::value, line< 1,14>::value, line< 1,15>::value,
            line< 1,16>::value, line< 1,17>::value, line< 1,18>::value, line< 1,19>::value, line< 1,20>::value, line< 1,21>::value, line< 1,22>::value, line< 1,23>::value,
            line< 1,24>::value, line< 1,25>::value, line< 1,26>::value, line< 1,27>::value, line< 1,28>::value, line< 1,29>::value, line< 1,30>::value, line< 1,31>::value,
            line< 1,32>::value, line< 1,33>::value, line< 1,34>::value, line< 1,35>::value, line< 1,36>::value, line< 1,37>::value, line< 1,38>::value, line< 1,39>::value,
            line< 1,40>::value, line< 1,41>::value, line< 1,42>::value, line< 1,43>::value, line< 1,44>::value, line< 1,45>::value, line< 1,46>::value, line< 1,47>::value,
            line< 1,48>::value, line< 1,49>::value, line< 1,50>::value, line< 1,51>::value, line< 1,52>::value, line< 1,53>::value, line< 1,54>::value, line< 1,55>::value,
            line< 1,56>::value, line< 1,57>::value, line< 1,58>::value, line< 1,59>::value, line< 1,60>::value, line< 1,61>::value, line< 1,62>::value, line< 1,63>::value,
            line< 2, 0>::value, line< 2, 1>::value, line< 2, 2>::value, line< 2, 3>::value, line< 2, 4>::value, line< 2, 5>::value, line< 2, 6>::value, line< 2, 7>::value,
            line< 2, 8>::value, line< 2, 9>::value, line< 2,10>::value, line< 2,11>::value, line< 2,12>::value, line< 2,13>::value, line< 2,14>::value, line< 2,15>::value,
            line< 2,16>::value, line< 2,17>::value, line< 2,18>::value, line< 2,19>::value, line< 2,20>::value, line< 2,21>::value, line< 2,22>::value, line< 2,23>::value,
            line< 2,24>::value, line< 2,25>::value, line< 2,26>::value, line< 2,27>::value, line< 2,28>::value, line< 2,29>::value, line< 2,30>::value, line< 2,31>::value,
            line< 2,32>::value, line< 2,33>::value, line< 2,34>::value, line< 2,35>::value, line< 2,36>::value, line< 2,37>::value, line< 2,38>::value, line< 2,39>::value,
            line< 2,40>::value, line< 2,41>::value, line< 2,42>::value, line< 2,43>::value, line< 2,44>::value, line< 2,45>::value, line< 2,46>::value, line< 2,47>::value,
            line< 2,48>::value, line< 2,49>::value, line< 2,50>::value, line< 2,51>::value, line< 2,52>::value, line< 2,53>::value, line< 2,54>::value, line< 2,55>::value,
            line< 2,56>::value, line< 2,57>::value, line< 2,58>::value, line< 2,59>::value, line< 2,60>::value, line< 2,61>::value, line< 2,62>::value, line< 2,63>::value,
            line< 3, 0>::value, line< 3, 1>::value, line< 3, 2>::value, line< 3, 3>::value, line< 3, 4>::value, line< 3, 5>::value, line< 3, 6>::value, line< 3, 7>::value,
            line< 3, 8>::value, line< 3, 9>::value, line< 3,10>::value, line< 3,11>::value, line< 3,12>::value, line< 3,13>::value, line< 3,14>::value, line< 3,15>::value,
            line< 3,16>::value, line< 3,17>::value, line< 3,18>::value, line< 3,19>::value, line< 3,20>::value, line< 3,21>::value, line< 3,22>::value, line< 3,23>::value,
            line< 3,24>::value, line< 3,25>::value, line< 3,26>::value, line< 3,27>::value, line< 3,28>::value, line< 3,29>::value, line< 3,30>::value, line< 3,31>::value,
            line< 3,32>::value, line< 3,33>::value, line< 3,34>::value, line< 3,35>::value, line< 3,36>::value, line< 3,37>::value, line< 3,38>::value, line< 3,39>::value,
            line< 3,40>::value, line< 3,41>::value, line< 3,42>::value, line< 3,43>::value, line< 3,44>::value, line< 3,45>::value, line< 3,46>::value, line< 3,47>::value,
            line< 3,48>::value, line< 3,49>::value, line< 3,50>::value, line< 3,51>::value, line< 3,52>::value, line< 3,53>::value, line< 3,54>::value, line< 3,55>::value,
            line< 3,56>::value, line< 3,57>::value, line< 3,58>::value, line< 3,59>::value, line< 3,60>::value, line< 3,61>::value, line< 3,62>::value, line< 3,63>::value,
            line< 4, 0>::value, line< 4, 1>::value, line< 4, 2>::value, line< 4, 3>::value, line< 4, 4>::value, line< 4, 5>::value, line< 4, 6>::value, line< 4, 7>::value,
            line< 4, 8>::value, line< 4, 9>::value, line< 4,10>::value, line< 4,11>::value, line< 4,12>::value, line< 4,13>::value, line< 4,14>::value, line< 4,15>::value,
            line< 4,16>::value, line< 4,17>::value, line< 4,18>::value, line< 4,19>::value, line< 4,20>::value, line< 4,21>::value, line< 4,22>::value, line< 4,23>::value,
            line< 4,24>::value, line< 4,25>::value, line< 4,26>::value, line< 4,27>::value, line< 4,28>::value, line< 4,29>::value, line< 4,30>::value, line< 4,31>::value,
            line< 4,32>::value, line< 4,33>::value, line< 4,34>::value, line< 4,35>::value, line< 4,36>::value, line< 4,37>::value, line< 4,38>::value, line< 4,39>::value,
            line< 4,40>::value, line< 4,41>::value, line< 4,42>::value, line< 4,43>::value, line< 4,44>::value, line< 4,45>::value, line< 4,46>::value, line< 4,47>::value,
            line< 4,48>::value, line< 4,49>::value, line< 4,50>::value, line< 4,51>::value, line< 4,52>::value, line< 4,53>::value, line< 4,54>::value, line< 4,55>::value,
            line< 4,56>::value, line< 4,57>::value, line< 4,58>::value, line< 4,59>::value, line< 4,60>::value, line< 4,61>::value, line< 4,62>::value, line< 4,63>::value,
            line< 5, 0>::value, line< 5, 1>::value, line< 5, 2>::value, line< 5, 3>::value, line< 5, 4>::value, line< 5, 5>::value, line< 5, 6>::value, line< 5, 7>::value,
            line< 5, 8>::value, line< 5, 9>::value, line< 5,10>::value, line< 5,11>::value, line< 5,12>::value, line< 5,13>::value, line< 5,14>::value, line< 5,15>::value,
            line< 5,16>::value, line< 5,17>::value, line< 5,18>::value, line< 5,19>::value, line< 5,20>::value, line< 5,21>::value, line< 5,22>::value, line< 5,23>::value,
            line< 5,24>::value, line< 5,25>::value, line< 5,26>::value, line< 5,27>::value, line< 5,28>::value, line< 5,29>::value, line< 5,30>::value, line< 5,31>::value,
            line< 5,32>::value, line< 5,33>::value, line< 5,34>::value, line< 5,35>::value, line< 5,36>::value, line< 5,37>::value, line< 5,38>::value, line< 5,39>::value,
            line< 5,40>::value, line< 5,41>::value, line< 5,42>::value, line< 5,43>::value, line< 5,44>::value, line< 5,45>::value, line< 5,46>::value, line< 5,47>::value,
            line< 5,48>::value, line< 5,49>::value, line< 5,50>::value, line< 5,51>::value, line< 5,52>::value, line< 5,53>::value, line< 5,54>::value, line< 5,55>::value,
            line< 5,56>::value, line< 5,57>::value, line< 5,58>::value, line< 5,59>::value, line< 5,60>::value, line< 5,61>::value, line< 5,62>::value, line< 5,63>::value,
            line< 6, 0>::value, line< 6, 1>::value, line< 6, 2>::value, line< 6, 3>::value, line< 6, 4>::value, line< 6, 5>::value, line< 6, 6>::value, line< 6, 7>::value,
            line< 6, 8>::value, line< 6, 9>::value, line< 6,10>::value, line< 6,11>::value, line< 6,12>::value, line< 6,13>::value, line< 6,14>::value, line< 6,15>::value,
            line< 6,16>::value, line< 6,17>::value, line< 6,18>::value, line< 6,19>::value, line< 6,20>::value, line< 6,21>::value, line< 6,22>::value, line< 6,23>::value,
            line< 6,24>::value, line< 6,25>::value, line< 6,26>::value, line< 6,27>::value, line< 6,28>::value, line< 6,29>::value, line< 6,30>::value, line< 6,31>::value,
            line< 6,32>::value, line< 6,33>::value, line< 6,34>::value, line< 6,35>::value, line< 6,36>::value, line< 6,37>::value, line< 6,38>::value, line< 6,39>::value,
            line< 6,40>::value, line< 6,41>::value, line< 6,42>::value, line< 6,43>::value, line< 6,44>::value, line< 6,45>::value, line< 6,46>::value, line< 6,47>::value,
            line< 6,48>::value, line< 6,49>::value, line< 6,50>::value, line< 6,51>::value, line< 6,52>::value, line< 6,53>::value, line< 6,54>::value, line< 6,55>::value,
            line< 6,56>::value, line< 6,57>::value, line< 6,58>::value, line< 6,59>::value, line< 6,60>::value, line< 6,61>::value, line< 6,62>::value, line< 6,63>::value,
            line< 7, 0>::value, line< 7, 1>::value, line< 7, 2>::value, line< 7, 3>::value, line< 7, 4>::value, line< 7, 5>::value, line< 7, 6>::value, line< 7, 7>::value,
            line< 7, 8>::value, line< 7, 9>::value, line< 7,10>::value, line< 7,11>::value, line< 7,12>::value, line< 7,13>::value, line< 7,14>::value, line< 7,15>::value,
            line< 7,16>::value, line< 7,17>::value, line< 7,18>::value, line< 7,19>::value, line< 7,20>::value, line< 7,21>::value, line< 7,22>::value, line< 7,23>::value,
            line< 7,24>::value, line< 7,25>::value, line< 7,26>::value, line< 7,27>::value, line< 7,28>::value, line< 7,29>::value, line< 7,30>::value, line< 7,31>::value,
            line< 7,32>::value, line< 7,33>::value, line< 7,34>::value, line< 7,35>::value, line< 7,36>::value, line< 7,37>::value, line< 7,38>::value, line< 7,39>::value,
            line< 7,40>::value, line< 7,41>::value, line< 7,42>::value, line< 7,43>::value, line< 7,44>::value, line< 7,45>::value, line< 7,46>::value, line< 7,47>::value,
            line< 7,48>::value, line< 7,49>::value, line< 7,50>::value, line< 7,51>::value, line< 7,52>::value, line< 7,53>::value, line< 7,54>::value, line< 7,55>::value,
            line< 7,56>::value, line< 7,57>::value, line< 7,58>::value, line< 7,59>::value, line< 7,60>::value, line< 7,61>::value, line< 7,62>::value, line< 7,63>::value,
            line< 8, 0>::value, line< 8, 1>::value, line< 8, 2>::value, line< 8, 3>::value, line< 8, 4>::value, line< 8, 5>::value, line< 8, 6>::value, line< 8, 7>::value,
            line< 8, 8>::value, line< 8, 9>::value, line< 8,10>::value, line< 8,11>::value, line< 8,12>::value, line< 8,13>::value, line< 8,14>::value, line< 8,15>::value,
            line< 8,16>::value, line< 8,17>::value, line< 8,18>::value, line< 8,19>::value, line< 8,20>::value, line< 8,21>::value, line< 8,22>::value, line< 8,23>::value,
            line< 8,24>::value, line< 8,25>::value, line< 8,26>::value, line< 8,27>::value, line< 8,28>::value, line< 8,29>::value, line< 8,30>::value, line< 8,31>::value,
            line< 8,32>::value, line< 8,33>::value, line< 8,34>::value, line< 8,35>::value, line< 8,36>::value, line< 8,37>::value, line< 8,38>::value, line< 8,39>::value,
            line< 8,40>::value, line< 8,41>::value, line< 8,42>::value, line< 8,43>::value, line< 8,44>::value, line< 8,45>::value, line< 8,46>::value, line< 8,47>::value,
            line< 8,48>::value, line< 8,49>::value, line< 8,50>::value, line< 8,51>::value, line< 8,52>::value, line< 8,53>::value, line< 8,54>::value, line< 8,55>::value,
            line< 8,56>::value, line< 8,57>::value, line< 8,58>::value, line< 8,59>::value, line< 8,60>::value, line< 8,61>::value, line< 8,62>::value, line< 8,63>::value,
            line< 9, 0>::value, line< 9, 1>::value, line< 9, 2>::value, line< 9, 3>::value, line< 9, 4>::value, line< 9, 5>::value, line< 9, 6>::value, line< 9, 7>::value,
            line< 9, 8>::value, line< 9, 9>::value, line< 9,10>::value, line< 9,11>::value, line< 9,12>::value, line< 9,13>::value, line< 9,14>::value, line< 9,15>::value,
            line< 9,16>::value, line< 9,17>::value, line< 9,18>::value, line< 9,19>::value, line< 9,20>::value, line< 9,21>::value, line< 9,22>::value, line< 9,23>::value,
            line< 9,24>::value, line< 9,25>::value, line< 9,26>::value, line< 9,27>::value, line< 9,28>::value, line< 9,29>::value, line< 9,30>::value, line< 9,31>::value,
            line< 9,32>::value, line< 9,33>::value, line< 9,34>::value, line< 9,35>::value, line< 9,36>::value, line< 9,37>::value, line< 9,38>::value, line< 9,39>::value,
            line< 9,40>::value, line< 9,41>::value, line< 9,42>::value, line< 9,43>::value, line< 9,44>::value, line< 9,45>::value, line< 9,46>::value, line< 9,47>::value,
            line< 9,48>::value, line< 9,49>::value, line< 9,50>::value, line< 9,51>::value, line< 9,52>::value, line< 9,53>::value, line< 9,54>::value, line< 9,55>::value,
            line< 9,56>::value, line< 9,57>::value, line< 9,58>::value, line< 9,59>::value, line< 9,60>::value, line< 9,61>::value, line< 9,62>::value, line< 9,63>::value,
            line<10, 0>::value, line<10, 1>::value, line<10, 2>::value, line<10, 3>::value, line<10, 4>::value, line<10, 5>::value, line<10, 6>::value, line<10, 7>::value,
            line<10, 8>::value, line<10, 9>::value, line<10,10>::value, line<10,11>::value, line<10,12>::value, line<10,13>::value, line<10,14>::value, line<10,15>::value,
            line<10,16>::value, line<10,17>::value, line<10,18>::value, line<10,19>::value, line<10,20>::value, line<10,21>::value, line<10,22>::value, line<10,23>::value,
            line<10,24>::value, line<10,25>::value, line<10,26>::value, line<10,27>::value, line<10,28>::value, line<10,29>::value, line<10,30>::value, line<10,31>::value,
            line<10,32>::value, line<10,33>::value, line<10,34>::value, line<10,35>::value, line<10,36>::value, line<10,37>::value, line<10,38>::value, line<10,39>::value,
            line<10,40>::value, line<10,41>::value, line<10,42>::value, line<10,43>::value, line<10,44>::value, line<10,45>::value, line<10,46>::value, line<10,47>::value,
            line<10,48>::value, line<10,49>::value, line<10,50>::value, line<10,51>::value, line<10,52>::value, line<10,53>::value, line<10,54>::value, line<10,55>::value,
            line<10,56>::value, line<10,57>::value, line<10,58>::value, line<10,59>::value, line<10,60>::value, line<10,61>::value, line<10,62>::value, line<10,63>::value,
            line<11, 0>::value, line<11, 1>::value, line<11, 2>::value, line<11, 3>::value, line<11, 4>::value, line<11, 5>::value, line<11, 6>::value, line<11, 7>::value,
            line<11, 8>::value, line<11, 9>::value, line<11,10>::value, line<11,11>::value, line<11,12>::value, line<11,13>::value, line<11,14>::value, line<11,15>::value,
            line<11,16>::value, line<11,17>::value, line<11,18>::value, line<11,19>::value, line<11,20>::value, line<11,21>::value, line<11,22>::value, line<11,23>::value,
            line<11,24>::value, line<11,25>::value, line<11,26>::value, line<11,27>::value, line<11,28>::value, line<11,29>::value, line<11,30>::value, line<11,31>::value,
            line<11,32>::value, line<11,33>::value, line<11,34>::value, line<11,35>::value, line<11,36>::value, line<11,37>::value, line<11,38>::value, line<11,39>::value,
            line<11,40>::value, line<11,41>::value, line<11,42>::value, line<11,43>::value, line<11,44>::value, line<11,45>::value, line<11,46>::value, line<11,47>::value,
            line<11,48>::value, line<11,49>::value, line<11,50>::value, line<11,51>::value, line<11,52>::value, line<11,53>::value, line<11,54>::value, line<11,55>::value,
            line<11,56>::value, line<11,57>::value, line<11,58>::value, line<11,59>::value, line<11,60>::value, line<11,61>::value, line<11,62>::value, line<11,63>::value,
            line<12, 0>::value, line<12, 1>::value, line<12, 2>::value, line<12, 3>::value, line<12, 4>::value, line<12, 5>::value, line<12, 6>::value, line<12, 7>::value,
            line<12, 8>::value, line<12, 9>::value, line<12,10>::value, line<12,11>::value, line<12,12>::value, line<12,13>::value, line<12,14>::value, line<12,15>::value,
            line<12,16>::value, line<12,17>::value, line<12,18>::value, line<12,19>::value, line<12,20>::value, line<12,21>::value, line<12,22>::value, line<12,23>::value,
            line<12,24>::value, line<12,25>::value, line<12,26>::value, line<12,27>::value, line<12,28>::value, line<12,29>::value, line<12,30>::value, line<12,31>::value,
            line<12,32>::value, line<12,33>::value, line<12,34>::value, line<12,35>::value, line<12,36>::value, line<12,37>::value, line<12,38>::value, line<12,39>::value,
            line<12,40>::value, line<12,41>::value, line<12,42>::value, line<12,43>::value, line<12,44>::value, line<12,45>::value, line<12,46>::value, line<12,47>::value,
            line<12,48>::value, line<12,49>::value, line<12,50>::value, line<12,51>::value, line<12,52>::value, line<12,53>::value, line<12,54>::value, line<12,55>::value,
            line<12,56>::value, line<12,57>::value, line<12,58>::value, line<12,59>::value, line<12,60>::value, line<12,61>::value, line<12,62>::value, line<12,63>::value,
            line<13, 0>::value, line<13, 1>::value, line<13, 2>::value, line<13, 3>::value, line<13, 4>::value, line<13, 5>::value, line<13, 6>::value, line<13, 7>::value,
            line<13, 8>::value, line<13, 9>::value, line<13,10>::value, line<13,11>::value, line<13,12>::value, line<13,13>::value, line<13,14>::value, line<13,15>::value,
            line<13,16>::value, line<13,17>::value, line<13,18>::value, line<13,19>::value, line<13,20>::value, line<13,21>::value, line<13,22>::value, line<13,23>::value,
            line<13,24>::value, line<13,25>::value, line<13,26>::value, line<13,27>::value, line<13,28>::value, line<13,29>::value, line<13,30>::value, line<13,31>::value,
            line<13,32>::value, line<13,33>::value, line<13,34>::value, line<13,35>::value, line<13,36>::value, line<13,37>::value, line<13,38>::value, line<13,39>::value,
            line<13,40>::value, line<13,41>::value, line<13,42>::value, line<13,43>::value, line<13,44>::value, line<13,45>::value, line<13,46>::value, line<13,47>::value,
            line<13,48>::value, line<13,49>::value, line<13,50>::value, line<13,51>::value, line<13,52>::value, line<13,53>::value, line<13,54>::value, line<13,55>::value,
            line<13,56>::value, line<13,57>::value, line<13,58>::value, line<13,59>::value, line<13,60>::value, line<13,61>::value, line<13,62>::value, line<13,63>::value,
            line<14, 0>::value, line<14, 1>::value, line<14, 2>::value, line<14, 3>::value, line<14, 4>::value, line<14, 5>::value, line<14, 6>::value, line<14, 7>::value,
            line<14, 8>::value, line<14, 9>::value, line<14,10>::value, line<14,11>::value, line<14,12>::value, line<14,13>::value, line<14,14>::value, line<14,15>::value,
            line<14,16>::value, line<14,17>::value, line<14,18>::value, line<14,19>::value, line<14,20>::value, line<14,21>::value, line<14,22>::value, line<14,23>::value,
            line<14,24>::value, line<14,25>::value, line<14,26>::value, line<14,27>::value, line<14,28>::value, line<14,29>::value, line<14,30>::value, line<14,31>::value,
            line<14,32>::value, line<14,33>::value, line<14,34>::value, line<14,35>::value, line<14,36>::value, line<14,37>::value, line<14,38>::value, line<14,39>::value,
            line<14,40>::value, line<14,41>::value, line<14,42>::value, line<14,43>::value, line<14,44>::value, line<14,45>::value, line<14,46>::value, line<14,47>::value,
            line<14,48>::value, line<14,49>::value, line<14,50>::value, line<14,51>::value, line<14,52>::value, line<14,53>::value, line<14,54>::value, line<14,55>::value,
            line<14,56>::value, line<14,57>::value, line<14,58>::value, line<14,59>::value, line<14,60>::value, line<14,61>::value, line<14,62>::value, line<14,63>::value,
            line<15, 0>::value, line<15, 1>::value, line<15, 2>::value, line<15, 3>::value, line<15, 4>::value, line<15, 5>::value, line<15, 6>::value, line<15, 7>::value,
            line<15, 8>::value, line<15, 9>::value, line<15,10>::value, line<15,11>::value, line<15,12>::value, line<15,13>::value, line<15,14>::value, line<15,15>::value,
            line<15,16>::value, line<15,17>::value, line<15,18>::value, line<15,19>::value, line<15,20>::value, line<15,21>::value, line<15,22>::value, line<15,23>::value,
            line<15,24>::value, line<15,25>::value, line<15,26>::value, line<15,27>::value, line<15,28>::value, line<15,29>::value, line<15,30>::value, line<15,31>::value,
            line<15,32>::value, line<15,33>::value, line<15,34>::value, line<15,35>::value, line<15,36>::value, line<15,37>::value, line<15,38>::value, line<15,39>::value,
            line<15,40>::value, line<15,41>::value, line<15,42>::value, line<15,43>::value, line<15,44>::value, line<15,45>::value, line<15,46>::value, line<15,47>::value,
            line<15,48>::value, line<15,49>::value, line<15,50>::value, line<15,51>::value, line<15,52>::value, line<15,53>::value, line<15,54>::value, line<15,55>::value,
            line<15,56>::value, line<15,57>::value, line<15,58>::value, line<15,59>::value, line<15,60>::value, line<15,61>::value, line<15,62>::value, line<15,63>::value,
            line<16, 0>::value, line<16, 1>::value, line<16, 2>::value, line<16, 3>::value, line<16, 4>::value, line<16, 5>::value, line<16, 6>::value, line<16, 7>::value,
            line<16, 8>::value, line<16, 9>::value, line<16,10>::value, line<16,11>::value, line<16,12>::value, line<16,13>::value, line<16,14>::value, line<16,15>::value,
            line<16,16>::value, line<16,17>::value, line<16,18>::value, line<16,19>::value, line<16,20>::value, line<16,21>::value, line<16,22>::value, line<16,23>::value,
            line<16,24>::value, line<16,25>::value, line<16,26>::value, line<16,27>::value, line<16,28>::value, line<16,29>::value, line<16,30>::value, line<16,31>::value,
            line<16,32>::value, line<16,33>::value, line<16,34>::value, line<16,35>::value, line<16,36>::value, line<16,37>::value, line<16,38>::value, line<16,39>::value,
            line<16,40>::value, line<16,41>::value, line<16,42>::value, line<16,43>::value, line<16,44>::value, line<16,45>::value, line<16,46>::value, line<16,47>::value,
            line<16,48>::value, line<16,49>::value, line<16,50>::value, line<16,51>::value, line<16,52>::value, line<16,53>::value, line<16,54>::value, line<16,55>::value,
            line<16,56>::value, line<16,57>::value, line<16,58>::value, line<16,59>::value, line<16,60>::value, line<16,61>::value, line<16,62>::value, line<16,63>::value,
            line<17, 0>::value, line<17, 1>::value, line<17, 2>::value, line<17, 3>::value, line<17, 4>::value, line<17, 5>::value, line<17, 6>::value, line<17, 7>::value,
            line<17, 8>::value, line<17, 9>::value, line<17,10>::value, line<17,11>::value, line<17,12>::value, line<17,13>::value, line<17,14>::value, line<17,15>::value,
            line<17,16>::value, line<17,17>::value, line<17,18>::value, line<17,19>::value, line<17,20>::value, line<17,21>::value, line<17,22>::value, line<17,23>::value,
            line<17,24>::value, line<17,25>::value, line<17,26>::value, line<17,27>::value, line<17,28>::value, line<17,29>::value, line<17,30>::value, line<17,31>::value,
            line<17,32>::value, line<17,33>::value, line<17,34>::value, line<17,35>::value, line<17,36>::value, line<17,37>::value, line<17,38>::value, line<17,39>::value,
            line<17,40>::value, line<17,41>::value, line<17,42>::value, line<17,43>::value, line<17,44>::value, line<17,45>::value, line<17,46>::value, line<17,47>::value,
            line<17,48>::value, line<17,49>::value, line<17,50>::value, line<17,51>::value, line<17,52>::value, line<17,53>::value, line<17,54>::value, line<17,55>::value,
            line<17,56>::value, line<17,57>::value, line<17,58>::value, line<17,59>::value, line<17,60>::value, line<17,61>::value, line<17,62>::value, line<17,63>::value,
            line<18, 0>::value, line<18, 1>::value, line<18, 2>::value, line<18, 3>::value, line<18, 4>::value, line<18, 5>::value, line<18, 6>::value, line<18, 7>::value,
            line<18, 8>::value, line<18, 9>::value, line<18,10>::value, line<18,11>::value, line<18,12>::value, line<18,13>::value, line<18,14>::value, line<18,15>::value,
            line<18,16>::value, line<18,17>::value, line<18,18>::value, line<18,19>::value, line<18,20>::value, line<18,21>::value, line<18,22>::value, line<18,23>::value,
            line<18,24>::value, line<18,25>::value, line<18,26>::value, line<18,27>::value, line<18,28>::value, line<18,29>::value, line<18,30>::value, line<18,31>::value,
            line<18,32>::value, line<18,33>::value, line<18,34>::value, line<18,35>::value, line<18,36>::value, line<18,37>::value, line<18,38>::value, line<18,39>::value,
            line<18,40>::value, line<18,41>::value, line<18,42>::value, line<18,43>::value, line<18,44>::value, line<18,45>::value, line<18,46>::value, line<18,47>::value,
            line<18,48>::value, line<18,49>::value, line<18,50>::value, line<18,51>::value, line<18,52>::value, line<18,53>::value, line<18,54>::value, line<18,55>::value,
            line<18,56>::value, line<18,57>::value, line<18,58>::value, line<18,59>::value, line<18,60>::value, line<18,61>::value, line<18,62>::value, line<18,63>::value,
            line<19, 0>::value, line<19, 1>::value, line<19, 2>::value, line<19, 3>::value, line<19, 4>::value, line<19, 5>::value, line<19, 6>::value, line<19, 7>::value,
            line<19, 8>::value, line<19, 9>::value, line<19,10>::value, line<19,11>::value, line<19,12>::value, line<19,13>::value, line<19,14>::value, line<19,15>::value,
            line<19,16>::value, line<19,17>::value, line<19,18>::value, line<19,19>::value, line<19,20>::value, line<19,21>::value, line<19,22>::value, line<19,23>::value,
            line<19,24>::value, line<19,25>::value, line<19,26>::value, line<19,27>::value, line<19,28>::value, line<19,29>::value, line<19,30>::value, line<19,31>::value,
            line<19,32>::value, line<19,33>::value, line<19,34>::value, line<19,35>::value, line<19,36>::value, line<19,37>::value, line<19,38>::value, line<19,39>::value,
            line<19,40>::value, line<19,41>::value, line<19,42>::value, line<19,43>::value, line<19,44>::value, line<19,45>::value, line<19,46>::value, line<19,47>::value,
            line<19,48>::value, line<19,49>::value, line<19,50>::value, line<19,51>::value, line<19,52>::value, line<19,53>::value, line<19,54>::value, line<19,55>::value,
            line<19,56>::value, line<19,57>::value, line<19,58>::value, line<19,59>::value, line<19,60>::value, line<19,61>::value, line<19,62>::value, line<19,63>::value,
            line<20, 0>::value, line<20, 1>::value, line<20, 2>::value, line<20, 3>::value, line<20, 4>::value, line<20, 5>::value, line<20, 6>::value, line<20, 7>::value,
            line<20, 8>::value, line<20, 9>::value, line<20,10>::value, line<20,11>::value, line<20,12>::value, line<20,13>::value, line<20,14>::value, line<20,15>::value,
            line<20,16>::value, line<20,17>::value, line<20,18>::value, line<20,19>::value, line<20,20>::value, line<20,21>::value, line<20,22>::value, line<20,23>::value,
            line<20,24>::value, line<20,25>::value, line<20,26>::value, line<20,27>::value, line<20,28>::value, line<20,29>::value, line<20,30>::value, line<20,31>::value,
            line<20,32>::value, line<20,33>::value, line<20,34>::value, line<20,35>::value, line<20,36>::value, line<20,37>::value, line<20,38>::value, line<20,39>::value,
            line<20,40>::value, line<20,41>::value, line<20,42>::value, line<20,43>::value, line<20,44>::value, line<20,45>::value, line<20,46>::value, line<20,47>::value,
            line<20,48>::value, line<20,49>::value, line<20,50>::value, line<20,51>::value, line<20,52>::value, line<20,53>::value, line<20,54>::value, line<20,55>::value,
            line<20,56>::value, line<20,57>::value, line<20,58>::value, line<20,59>::value, line<20,60>::value, line<20,61>::value, line<20,62>::value, line<20,63>::value,
            line<21, 0>::value, line<21, 1>::value, line<21, 2>::value, line<21, 3>::value, line<21, 4>::value, line<21, 5>::value, line<21, 6>::value, line<21, 7>::value,
            line<21, 8>::value, line<21, 9>::value, line<21,10>::value, line<21,11>::value, line<21,12>::value, line<21,13>::value, line<21,14>::value, line<21,15>::value,
            line<21,16>::value, line<21,17>::value, line<21,18>::value, line<21,19>::value, line<21,20>::value, line<21,21>::value, line<21,22>::value, line<21,23>::value,
            line<21,24>::value, line<21,25>::value, line<21,26>::value, line<21,27>::value, line<21,28>::value, line<21,29>::value, line<21,30>::value, line<21,31>::value,
            line<21,32>::value, line<21,33>::value, line<21,34>::value, line<21,35>::value, line<21,36>::value, line<21,37>::value, line<21,38>::value, line<21,39>::value,
            line<21,40>::value, line<21,41>::value, line<21,42>::value, line<21,43>::value, line<21,44>::value, line<21,45>::value, line<21,46>::value, line<21,47>::value,
            line<21,48>::value, line<21,49>::value, line<21,50>::value, line<21,51>::value, line<21,52>::value, line<21,53>::value, line<21,54>::value, line<21,55>::value,
            line<21,56>::value, line<21,57>::value, line<21,58>::value, line<21,59>::value, line<21,60>::value, line<21,61>::value, line<21,62>::value, line<21,63>::value,
            line<22, 0>::value, line<22, 1>::value, line<22, 2>::value, line<22, 3>::value, line<22, 4>::value, line<22, 5>::value, line<22, 6>::value, line<22, 7>::value,
            line<22, 8>::value, line<22, 9>::value, line<22,10>::value, line<22,11>::value, line<22,12>::value, line<22,13>::value, line<22,14>::value, line<22,15>::value,
            line<22,16>::value, line<22,17>::value, line<22,18>::value, line<22,19>::value, line<22,20>::value, line<22,21>::value, line<22,22>::value, line<22,23>::value,
            line<22,24>::value, line<22,25>::value, line<22,26>::value, line<22,27>::value, line<22,28>::value, line<22,29>::value, line<22,30>::value, line<22,31>::value,
            line<22,32>::value, line<22,33>::value, line<22,34>::value, line<22,35>::value, line<22,36>::value, line<22,37>::value, line<22,38>::value, line<22,39>::value,
            line<22,40>::value, line<22,41>::value, line<22,42>::value, line<22,43>::value, line<22,44>::value, line<22,45>::value, line<22,46>::value, line<22,47>::value,
            line<22,48>::value, line<22,49>::value, line<22,50>::value, line<22,51>::value, line<22,52>::value, line<22,53>::value, line<22,54>::value, line<22,55>::value,
            line<22,56>::value, line<22,57>::value, line<22,58>::value, line<22,59>::value, line<22,60>::value, line<22,61>::value, line<22,62>::value, line<22,63>::value,
            line<23, 0>::value, line<23, 1>::value, line<23, 2>::value, line<23, 3>::value, line<23, 4>::value, line<23, 5>::value, line<23, 6>::value, line<23, 7>::value,
            line<23, 8>::value, line<23, 9>::value, line<23,10>::value, line<23,11>::value, line<23,12>::value, line<23,13>::value, line<23,14>::value, line<23,15>::value,
            line<23,16>::value, line<23,17>::value, line<23,18>::value, line<23,19>::value, line<23,20>::value, line<23,21>::value, line<23,22>::value, line<23,23>::value,
            line<23,24>::value, line<23,25>::value, line<23,26>::value, line<23,27>::value, line<23,28>::value, line<23,29>::value, line<23,30>::value, line<23,31>::value,
            line<23,32>::value, line<23,33>::value, line<23,34>::value, line<23,35>::value, line<23,36>::value, line<23,37>::value, line<23,38>::value, line<23,39>::value,
            line<23,40>::value, line<23,41>::value, line<23,42>::value, line<23,43>::value, line<23,44>::value, line<23,45>::value, line<23,46>::value, line<23,47>::value,
            line<23,48>::value, line<23,49>::value, line<23,50>::value, line<23,51>::value, line<23,52>::value, line<23,53>::value, line<23,54>::value, line<23,55>::value,
            line<23,56>::value, line<23,57>::value, line<23,58>::value, line<23,59>::value, line<23,60>::value, line<23,61>::value, line<23,62>::value, line<23,63>::value,
            line<24, 0>::value, line<24, 1>::value, line<24, 2>::value, line<24, 3>::value, line<24, 4>::value, line<24, 5>::value, line<24, 6>::value, line<24, 7>::value,
            line<24, 8>::value, line<24, 9>::value, line<24,10>::value, line<24,11>::value, line<24,12>::value, line<24,13>::value, line<24,14>::value, line<24,15>::value,
            line<24,16>::value, line<24,17>::value, line<24,18>::value, line<24,19>::value, line<24,20>::value, line<24,21>::value, line<24,22>::value, line<24,23>::value,
            line<24,24>::value, line<24,25>::value, line<24,26>::value, line<24,27>::value, line<24,28>::value, line<24,29>::value, line<24,30>::value, line<24,31>::value,
            line<24,32>::value, line<24,33>::value, line<24,34>::value, line<24,35>::value, line<24,36>::value, line<24,37>::value, line<24,38>::value, line<24,39>::value,
            line<24,40>::value, line<24,41>::value, line<24,42>::value, line<24,43>::value, line<24,44>::value, line<24,45>::value, line<24,46>::value, line<24,47>::value,
            line<24,48>::value, line<24,49>::value, line<24,50>::value, line<24,51>::value, line<24,52>::value, line<24,53>::value, line<24,54>::value, line<24,55>::value,
            line<24,56>::value, line<24,57>::value, line<24,58>::value, line<24,59>::value, line<24,60>::value, line<24,61>::value, line<24,62>::value, line<24,63>::value,
            line<25, 0>::value, line<25, 1>::value, line<25, 2>::value, line<25, 3>::value, line<25, 4>::value, line<25, 5>::value, line<25, 6>::value, line<25, 7>::value,
            line<25, 8>::value, line<25, 9>::value, line<25,10>::value, line<25,11>::value, line<25,12>::value, line<25,13>::value, line<25,14>::value, line<25,15>::value,
            line<25,16>::value, line<25,17>::value, line<25,18>::value, line<25,19>::value, line<25,20>::value, line<25,21>::value, line<25,22>::value, line<25,23>::value,
            line<25,24>::value, line<25,25>::value, line<25,26>::value, line<25,27>::value, line<25,28>::value, line<25,29>::value, line<25,30>::value, line<25,31>::value,
            line<25,32>::value, line<25,33>::value, line<25,34>::value, line<25,35>::value, line<25,36>::value, line<25,37>::value, line<25,38>::value, line<25,39>::value,
            line<25,40>::value, line<25,41>::value, line<25,42>::value, line<25,43>::value, line<25,44>::value, line<25,45>::value, line<25,46>::value, line<25,47>::value,
            line<25,48>::value, line<25,49>::value, line<25,50>::value, line<25,51>::value, line<25,52>::value, line<25,53>::value, line<25,54>::value, line<25,55>::value,
            line<25,56>::value, line<25,57>::value, line<25,58>::value, line<25,59>::value, line<25,60>::value, line<25,61>::value, line<25,62>::value, line<25,63>::value,
            line<26, 0>::value, line<26, 1>::value, line<26, 2>::value, line<26, 3>::value, line<26, 4>::value, line<26, 5>::value, line<26, 6>::value, line<26, 7>::value,
            line<26, 8>::value, line<26, 9>::value, line<26,10>::value, line<26,11>::value, line<26,12>::value, line<26,13>::value, line<26,14>::value, line<26,15>::value,
            line<26,16>::value, line<26,17>::value, line<26,18>::value, line<26,19>::value, line<26,20>::value, line<26,21>::value, line<26,22>::value, line<26,23>::value,
            line<26,24>::value, line<26,25>::value, line<26,26>::value, line<26,27>::value, line<26,28>::value, line<26,29>::value, line<26,30>::value, line<26,31>::value,
            line<26,32>::value, line<26,33>::value, line<26,34>::value, line<26,35>::value, line<26,36>::value, line<26,37>::value, line<26,38>::value, line<26,39>::value,
            line<26,40>::value, line<26,41>::value, line<26,42>::value, line<26,43>::value, line<26,44>::value, line<26,45>::value, line<26,46>::value, line<26,47>::value,
            line<26,48>::value, line<26,49>::value, line<26,50>::value, line<26,51>::value, line<26,52>::value, line<26,53>::value, line<26,54>::value, line<26,55>::value,
            line<26,56>::value, line<26,57>::value, line<26,58>::value, line<26,59>::value, line<26,60>::value, line<26,61>::value, line<26,62>::value, line<26,63>::value,
            line<27, 0>::value, line<27, 1>::value, line<27, 2>::value, line<27, 3>::value, line<27, 4>::value, line<27, 5>::value, line<27, 6>::value, line<27, 7>::value,
            line<27, 8>::value, line<27, 9>::value, line<27,10>::value, line<27,11>::value, line<27,12>::value, line<27,13>::value, line<27,14>::value, line<27,15>::value,
            line<27,16>::value, line<27,17>::value, line<27,18>::value, line<27,19>::value, line<27,20>::value, line<27,21>::value, line<27,22>::value, line<27,23>::value,
            line<27,24>::value, line<27,25>::value, line<27,26>::value, line<27,27>::value, line<27,28>::value, line<27,29>::value, line<27,30>::value, line<27,31>::value,
            line<27,32>::value, line<27,33>::value, line<27,34>::value, line<27,35>::value, line<27,36>::value, line<27,37>::value, line<27,38>::value, line<27,39>::value,
            line<27,40>::value, line<27,41>::value, line<27,42>::value, line<27,43>::value, line<27,44>::value, line<27,45>::value, line<27,46>::value, line<27,47>::value,
            line<27,48>::value, line<27,49>::value, line<27,50>::value, line<27,51>::value, line<27,52>::value, line<27,53>::value, line<27,54>::value, line<27,55>::value,
            line<27,56>::value, line<27,57>::value, line<27,58>::value, line<27,59>::value, line<27,60>::value, line<27,61>::value, line<27,62>::value, line<27,63>::value,
            line<28, 0>::value, line<28, 1>::value, line<28, 2>::value, line<28, 3>::value, line<28, 4>::value, line<28, 5>::value, line<28, 6>::value, line<28, 7>::value,
            line<28, 8>::value, line<28, 9>::value, line<28,10>::value, line<28,11>::value, line<28,12>::value, line<28,13>::value, line<28,14>::value, line<28,15>::value,
            line<28,16>::value, line<28,17>::value, line<28,18>::value, line<28,19>::value, line<28,20>::value, line<28,21>::value, line<28,22>::value, line<28,23>::value,
            line<28,24>::value, line<28,25>::value, line<28,26>::value, line<28,27>::value, line<28,28>::value, line<28,29>::value, line<28,30>::value, line<28,31>::value,
            line<28,32>::value, line<28,33>::value, line<28,34>::value, line<28,35>::value, line<28,36>::value, line<28,37>::value, line<28,38>::value, line<28,39>::value,
            line<28,40>::value, line<28,41>::value, line<28,42>::value, line<28,43>::value, line<28,44>::value, line<28,45>::value, line<28,46>::value, line<28,47>::value,
            line<28,48>::value, line<28,49>::value, line<28,50>::value, line<28,51>::value, line<28,52>::value, line<28,53>::value, line<28,54>::value, line<28,55>::value,
            line<28,56>::value, line<28,57>::value, line<28,58>::value, line<28,59>::value, line<28,60>::value, line<28,61>::value, line<28,62>::value, line<28,63>::value,
            line<29, 0>::value, line<29, 1>::value, line<29, 2>::value, line<29, 3>::value, line<29, 4>::value, line<29, 5>::value, line<29, 6>::value, line<29, 7>::value,
            line<29, 8>::value, line<29, 9>::value, line<29,10>::value, line<29,11>::value, line<29,12>::value, line<29,13>::value, line<29,14>::value, line<29,15>::value,
            line<29,16>::value, line<29,17>::value, line<29,18>::value, line<29,19>::value, line<29,20>::value, line<29,21>::value, line<29,22>::value, line<29,23>::value,
            line<29,24>::value, line<29,25>::value, line<29,26>::value, line<29,27>::value, line<29,28>::value, line<29,29>::value, line<29,30>::value, line<29,31>::value,
            line<29,32>::value, line<29,33>::value, line<29,34>::value, line<29,35>::value, line<29,36>::value, line<29,37>::value, line<29,38>::value, line<29,39>::value,
            line<29,40>::value, line<29,41>::value, line<29,42>::value, line<29,43>::value, line<29,44>::value, line<29,45>::value, line<29,46>::value, line<29,47>::value,
            line<29,48>::value, line<29,49>::value, line<29,50>::value, line<29,51>::value, line<29,52>::value, line<29,53>::value, line<29,54>::value, line<29,55>::value,
            line<29,56>::value, line<29,57>::value, line<29,58>::value, line<29,59>::value, line<29,60>::value, line<29,61>::value, line<29,62>::value, line<29,63>::value,
            line<30, 0>::value, line<30, 1>::value, line<30, 2>::value, line<30, 3>::value, line<30, 4>::value, line<30, 5>::value, line<30, 6>::value, line<30, 7>::value,
            line<30, 8>::value, line<30, 9>::value, line<30,10>::value, line<30,11>::value, line<30,12>::value, line<30,13>::value, line<30,14>::value, line<30,15>::value,
            line<30,16>::value, line<30,17>::value, line<30,18>::value, line<30,19>::value, line<30,20>::value, line<30,21>::value, line<30,22>::value, line<30,23>::value,
            line<30,24>::value, line<30,25>::value, line<30,26>::value, line<30,27>::value, line<30,28>::value, line<30,29>::value, line<30,30>::value, line<30,31>::value,
            line<30,32>::value, line<30,33>::value, line<30,34>::value, line<30,35>::value, line<30,36>::value, line<30,37>::value, line<30,38>::value, line<30,39>::value,
            line<30,40>::value, line<30,41>::value, line<30,42>::value, line<30,43>::value, line<30,44>::value, line<30,45>::value, line<30,46>::value, line<30,47>::value,
            line<30,48>::value, line<30,49>::value, line<30,50>::value, line<30,51>::value, line<30,52>::value, line<30,53>::value, line<30,54>::value, line<30,55>::value,
            line<30,56>::value, line<30,57>::value, line<30,58>::value, line<30,59>::value, line<30,60>::value, line<30,61>::value, line<30,62>::value, line<30,63>::value,
            line<31, 0>::value, line<31, 1>::value, line<31, 2>::value, line<31, 3>::value, line<31, 4>::value, line<31, 5>::value, line<31, 6>::value, line<31, 7>::value,
            line<31, 8>::value, line<31, 9>::value, line<31,10>::value, line<31,11>::value, line<31,12>::value, line<31,13>::value, line<31,14>::value, line<31,15>::value,
            line<31,16>::value, line<31,17>::value, line<31,18>::value, line<31,19>::value, line<31,20>::value, line<31,21>::value, line<31,22>::value, line<31,23>::value,
            line<31,24>::value, line<31,25>::value, line<31,26>::value, line<31,27>::value, line<31,28>::value, line<31,29>::value, line<31,30>::value, line<31,31>::value,
            line<31,32>::value, line<31,33>::value, line<31,34>::value, line<31,35>::value, line<31,36>::value, line<31,37>::value, line<31,38>::value, line<31,39>::value,
            line<31,40>::value, line<31,41>::value, line<31,42>::value, line<31,43>::value, line<31,44>::value, line<31,45>::value, line<31,46>::value, line<31,47>::value,
            line<31,48>::value, line<31,49>::value, line<31,50>::value, line<31,51>::value, line<31,52>::value, line<31,53>::value, line<31,54>::value, line<31,55>::value,
            line<31,56>::value, line<31,57>::value, line<31,58>::value, line<31,59>::value, line<31,60>::value, line<31,61>::value, line<31,62>::value, line<31,63>::value,
            line<32, 0>::value, line<32, 1>::value, line<32, 2>::value, line<32, 3>::value, line<32, 4>::value, line<32, 5>::value, line<32, 6>::value, line<32, 7>::value,
            line<32, 8>::value, line<32, 9>::value, line<32,10>::value, line<32,11>::value, line<32,12>::value, line<32,13>::value, line<32,14>::value, line<32,15>::value,
            line<32,16>::value, line<32,17>::value, line<32,18>::value, line<32,19>::value, line<32,20>::value, line<32,21>::value, line<32,22>::value, line<32,23>::value,
            line<32,24>::value, line<32,25>::value, line<32,26>::value, line<32,27>::value, line<32,28>::value, line<32,29>::value, line<32,30>::value, line<32,31>::value,
            line<32,32>::value, line<32,33>::value, line<32,34>::value, line<32,35>::value, line<32,36>::value, line<32,37>::value, line<32,38>::value, line<32,39>::value,
            line<32,40>::value, line<32,41>::value, line<32,42>::value, line<32,43>::value, line<32,44>::value, line<32,45>::value, line<32,46>::value, line<32,47>::value,
            line<32,48>::value, line<32,49>::value, line<32,50>::value, line<32,51>::value, line<32,52>::value, line<32,53>::value, line<32,54>::value, line<32,55>::value,
            line<32,56>::value, line<32,57>::value, line<32,58>::value, line<32,59>::value, line<32,60>::value, line<32,61>::value, line<32,62>::value, line<32,63>::value,
            line<33, 0>::value, line<33, 1>::value, line<33, 2>::value, line<33, 3>::value, line<33, 4>::value, line<33, 5>::value, line<33, 6>::value, line<33, 7>::value,
            line<33, 8>::value, line<33, 9>::value, line<33,10>::value, line<33,11>::value, line<33,12>::value, line<33,13>::value, line<33,14>::value, line<33,15>::value,
            line<33,16>::value, line<33,17>::value, line<33,18>::value, line<33,19>::value, line<33,20>::value, line<33,21>::value, line<33,22>::value, line<33,23>::value,
            line<33,24>::value, line<33,25>::value, line<33,26>::value, line<33,27>::value, line<33,28>::value, line<33,29>::value, line<33,30>::value, line<33,31>::value,
            line<33,32>::value, line<33,33>::value, line<33,34>::value, line<33,35>::value, line<33,36>::value, line<33,37>::value, line<33,38>::value, line<33,39>::value,
            line<33,40>::value, line<33,41>::value, line<33,42>::value, line<33,43>::value, line<33,44>::value, line<33,45>::value, line<33,46>::value, line<33,47>::value,
            line<33,48>::value, line<33,49>::value, line<33,50>::value, line<33,51>::value, line<33,52>::value, line<33,53>::value, line<33,54>::value, line<33,55>::value,
            line<33,56>::value, line<33,57>::value, line<33,58>::value, line<33,59>::value, line<33,60>::value, line<33,61>::value, line<33,62>::value, line<33,63>::value,
            line<34, 0>::value, line<34, 1>::value, line<34, 2>::value, line<34, 3>::value, line<34, 4>::value, line<34, 5>::value, line<34, 6>::value, line<34, 7>::value,
            line<34, 8>::value, line<34, 9>::value, line<34,10>::value, line<34,11>::value, line<34,12>::value, line<34,13>::value, line<34,14>::value, line<34,15>::value,
            line<34,16>::value, line<34,17>::value, line<34,18>::value, line<34,19>::value, line<34,20>::value, line<34,21>::value, line<34,22>::value, line<34,23>::value,
            line<34,24>::value, line<34,25>::value, line<34,26>::value, line<34,27>::value, line<34,28>::value, line<34,29>::value, line<34,30>::value, line<34,31>::value,
            line<34,32>::value, line<34,33>::value, line<34,34>::value, line<34,35>::value, line<34,36>::value, line<34,37>::value, line<34,38>::value, line<34,39>::value,
            line<34,40>::value, line<34,41>::value, line<34,42>::value, line<34,43>::value, line<34,44>::value, line<34,45>::value, line<34,46>::value, line<34,47>::value,
            line<34,48>::value, line<34,49>::value, line<34,50>::value, line<34,51>::value, line<34,52>::value, line<34,53>::value, line<34,54>::value, line<34,55>::value,
            line<34,56>::value, line<34,57>::value, line<34,58>::value, line<34,59>::value, line<34,60>::value, line<34,61>::value, line<34,62>::value, line<34,63>::value,
            line<35, 0>::value, line<35, 1>::value, line<35, 2>::value, line<35, 3>::value, line<35, 4>::value, line<35, 5>::value, line<35, 6>::value, line<35, 7>::value,
            line<35, 8>::value, line<35, 9>::value, line<35,10>::value, line<35,11>::value, line<35,12>::value, line<35,13>::value, line<35,14>::value, line<35,15>::value,
            line<35,16>::value, line<35,17>::value, line<35,18>::value, line<35,19>::value, line<35,20>::value, line<35,21>::value, line<35,22>::value, line<35,23>::value,
            line<35,24>::value, line<35,25>::value, line<35,26>::value, line<35,27>::value, line<35,28>::value, line<35,29>::value, line<35,30>::value, line<35,31>::value,
            line<35,32>::value, line<35,33>::value, line<35,34>::value, line<35,35>::value, line<35,36>::value, line<35,37>::value, line<35,38>::value, line<35,39>::value,
            line<35,40>::value, line<35,41>::value, line<35,42>::value, line<35,43>::value, line<35,44>::value, line<35,45>::value, line<35,46>::value, line<35,47>::value,
            line<35,48>::value, line<35,49>::value, line<35,50>::value, line<35,51>::value, line<35,52>::value, line<35,53>::value, line<35,54>::value, line<35,55>::value,
            line<35,56>::value, line<35,57>::value, line<35,58>::value, line<35,59>::value, line<35,60>::value, line<35,61>::value, line<35,62>::value, line<35,63>::value,
            line<36, 0>::value, line<36, 1>::value, line<36, 2>::value, line<36, 3>::value, line<36, 4>::value, line<36, 5>::value, line<36, 6>::value, line<36, 7>::value,
            line<36, 8>::value, line<36, 9>::value, line<36,10>::value, line<36,11>::value, line<36,12>::value, line<36,13>::value, line<36,14>::value, line<36,15>::value,
            line<36,16>::value, line<36,17>::value, line<36,18>::value, line<36,19>::value, line<36,20>::value, line<36,21>::value, line<36,22>::value, line<36,23>::value,
            line<36,24>::value, line<36,25>::value, line<36,26>::value, line<36,27>::value, line<36,28>::value, line<36,29>::value, line<36,30>::value, line<36,31>::value,
            line<36,32>::value, line<36,33>::value, line<36,34>::value, line<36,35>::value, line<36,36>::value, line<36,37>::value, line<36,38>::value, line<36,39>::value,
            line<36,40>::value, line<36,41>::value, line<36,42>::value, line<36,43>::value, line<36,44>::value, line<36,45>::value, line<36,46>::value, line<36,47>::value,
            line<36,48>::value, line<36,49>::value, line<36,50>::value, line<36,51>::value, line<36,52>::value, line<36,53>::value, line<36,54>::value, line<36,55>::value,
            line<36,56>::value, line<36,57>::value, line<36,58>::value, line<36,59>::value, line<36,60>::value, line<36,61>::value, line<36,62>::value, line<36,63>::value,
            line<37, 0>::value, line<37, 1>::value, line<37, 2>::value, line<37, 3>::value, line<37, 4>::value, line<37, 5>::value, line<37, 6>::value, line<37, 7>::value,
            line<37, 8>::value, line<37, 9>::value, line<37,10>::value, line<37,11>::value, line<37,12>::value, line<37,13>::value, line<37,14>::value, line<37,15>::value,
            line<37,16>::value, line<37,17>::value, line<37,18>::value, line<37,19>::value, line<37,20>::value, line<37,21>::value, line<37,22>::value, line<37,23>::value,
            line<37,24>::value, line<37,25>::value, line<37,26>::value, line<37,27>::value, line<37,28>::value, line<37,29>::value, line<37,30>::value, line<37,31>::value,
            line<37,32>::value, line<37,33>::value, line<37,34>::value, line<37,35>::value, line<37,36>::value, line<37,37>::value, line<37,38>::value, line<37,39>::value,
            line<37,40>::value, line<37,41>::value, line<37,42>::value, line<37,43>::value, line<37,44>::value, line<37,45>::value, line<37,46>::value, line<37,47>::value,
            line<37,48>::value, line<37,49>::value, line<37,50>::value, line<37,51>::value, line<37,52>::value, line<37,53>::value, line<37,54>::value, line<37,55>::value,
            line<37,56>::value, line<37,57>::value, line<37,58>::value, line<37,59>::value, line<37,60>::value, line<37,61>::value, line<37,62>::value, line<37,63>::value,
            line<38, 0>::value, line<38, 1>::value, line<38, 2>::value, line<38, 3>::value, line<38, 4>::value, line<38, 5>::value, line<38, 6>::value, line<38, 7>::value,
            line<38, 8>::value, line<38, 9>::value, line<38,10>::value, line<38,11>::value, line<38,12>::value, line<38,13>::value, line<38,14>::value, line<38,15>::value,
            line<38,16>::value, line<38,17>::value, line<38,18>::value, line<38,19>::value, line<38,20>::value, line<38,21>::value, line<38,22>::value, line<38,23>::value,
            line<38,24>::value, line<38,25>::value, line<38,26>::value, line<38,27>::value, line<38,28>::value, line<38,29>::value, line<38,30>::value, line<38,31>::value,
            line<38,32>::value, line<38,33>::value, line<38,34>::value, line<38,35>::value, line<38,36>::value, line<38,37>::value, line<38,38>::value, line<38,39>::value,
            line<38,40>::value, line<38,41>::value, line<38,42>::value, line<38,43>::value, line<38,44>::value, line<38,45>::value, line<38,46>::value, line<38,47>::value,
            line<38,48>::value, line<38,49>::value, line<38,50>::value, line<38,51>::value, line<38,52>::value, line<38,53>::value, line<38,54>::value, line<38,55>::value,
            line<38,56>::value, line<38,57>::value, line<38,58>::value, line<38,59>::value, line<38,60>::value, line<38,61>::value, line<38,62>::value, line<38,63>::value,
            line<39, 0>::value, line<39, 1>::value, line<39, 2>::value, line<39, 3>::value, line<39, 4>::value, line<39, 5>::value, line<39, 6>::value, line<39, 7>::value,
            line<39, 8>::value, line<39, 9>::value, line<39,10>::value, line<39,11>::value, line<39,12>::value, line<39,13>::value, line<39,14>::value, line<39,15>::value,
            line<39,16>::value, line<39,17>::value, line<39,18>::value, line<39,19>::value, line<39,20>::value, line<39,21>::value, line<39,22>::value, line<39,23>::value,
            line<39,24>::value, line<39,25>::value, line<39,26>::value, line<39,27>::value, line<39,28>::value, line<39,29>::value, line<39,30>::value, line<39,31>::value,
            line<39,32>::value, line<39,33>::value, line<39,34>::value, line<39,35>::value, line<39,36>::value, line<39,37>::value, line<39,38>::value, line<39,39>::value,
            line<39,40>::value, line<39,41>::value, line<39,42>::value, line<39,43>::value, line<39,44>::value, line<39,45>::value, line<39,46>::value, line<39,47>::value,
            line<39,48>::value, line<39,49>::value, line<39,50>::value, line<39,51>::value, line<39,52>::value, line<39,53>::value, line<39,54>::value, line<39,55>::value,
            line<39,56>::value, line<39,57>::value, line<39,58>::value, line<39,59>::value, line<39,60>::value, line<39,61>::value, line<39,62>::value, line<39,63>::value,
            line<40, 0>::value, line<40, 1>::value, line<40, 2>::value, line<40, 3>::value, line<40, 4>::value, line<40, 5>::value, line<40, 6>::value, line<40, 7>::value,
            line<40, 8>::value, line<40, 9>::value, line<40,10>::value, line<40,11>::value, line<40,12>::value, line<40,13>::value, line<40,14>::value, line<40,15>::value,
            line<40,16>::value, line<40,17>::value, line<40,18>::value, line<40,19>::value, line<40,20>::value, line<40,21>::value, line<40,22>::value, line<40,23>::value,
            line<40,24>::value, line<40,25>::value, line<40,26>::value, line<40,27>::value, line<40,28>::value, line<40,29>::value, line<40,30>::value, line<40,31>::value,
            line<40,32>::value, line<40,33>::value, line<40,34>::value, line<40,35>::value, line<40,36>::value, line<40,37>::value, line<40,38>::value, line<40,39>::value,
            line<40,40>::value, line<40,41>::value, line<40,42>::value, line<40,43>::value, line<40,44>::value, line<40,45>::value, line<40,46>::value, line<40,47>::value,
            line<40,48>::value, line<40,49>::value, line<40,50>::value, line<40,51>::value, line<40,52>::value, line<40,53>::value, line<40,54>::value, line<40,55>::value,
            line<40,56>::value, line<40,57>::value, line<40,58>::value, line<40,59>::value, line<40,60>::value, line<40,61>::value, line<40,62>::value, line<40,63>::value,
            line<41, 0>::value, line<41, 1>::value, line<41, 2>::value, line<41, 3>::value, line<41, 4>::value, line<41, 5>::value, line<41, 6>::value, line<41, 7>::value,
            line<41, 8>::value, line<41, 9>::value, line<41,10>::value, line<41,11>::value, line<41,12>::value, line<41,13>::value, line<41,14>::value, line<41,15>::value,
            line<41,16>::value, line<41,17>::value, line<41,18>::value, line<41,19>::value, line<41,20>::value, line<41,21>::value, line<41,22>::value, line<41,23>::value,
            line<41,24>::value, line<41,25>::value, line<41,26>::value, line<41,27>::value, line<41,28>::value, line<41,29>::value, line<41,30>::value, line<41,31>::value,
            line<41,32>::value, line<41,33>::value, line<41,34>::value, line<41,35>::value, line<41,36>::value, line<41,37>::value, line<41,38>::value, line<41,39>::value,
            line<41,40>::value, line<41,41>::value, line<41,42>::value, line<41,43>::value, line<41,44>::value, line<41,45>::value, line<41,46>::value, line<41,47>::value,
            line<41,48>::value, line<41,49>::value, line<41,50>::value, line<41,51>::value, line<41,52>::value, line<41,53>::value, line<41,54>::value, line<41,55>::value,
            line<41,56>::value, line<41,57>::value, line<41,58>::value, line<41,59>::value, line<41,60>::value, line<41,61>::value, line<41,62>::value, line<41,63>::value,
            line<42, 0>::value, line<42, 1>::value, line<42, 2>::value, line<42, 3>::value, line<42, 4>::value, line<42, 5>::value, line<42, 6>::value, line<42, 7>::value,
            line<42, 8>::value, line<42, 9>::value, line<42,10>::value, line<42,11>::value, line<42,12>::value, line<42,13>::value, line<42,14>::value, line<42,15>::value,
            line<42,16>::value, line<42,17>::value, line<42,18>::value, line<42,19>::value, line<42,20>::value, line<42,21>::value, line<42,22>::value, line<42,23>::value,
            line<42,24>::value, line<42,25>::value, line<42,26>::value, line<42,27>::value, line<42,28>::value, line<42,29>::value, line<42,30>::value, line<42,31>::value,
            line<42,32>::value, line<42,33>::value, line<42,34>::value, line<42,35>::value, line<42,36>::value, line<42,37>::value, line<42,38>::value, line<42,39>::value,
            line<42,40>::value, line<42,41>::value, line<42,42>::value, line<42,43>::value, line<42,44>::value, line<42,45>::value, line<42,46>::value, line<42,47>::value,
            line<42,48>::value, line<42,49>::value, line<42,50>::value, line<42,51>::value, line<42,52>::value, line<42,53>::value, line<42,54>::value, line<42,55>::value,
            line<42,56>::value, line<42,57>::value, line<42,58>::value, line<42,59>::value, line<42,60>::value, line<42,61>::value, line<42,62>::value, line<42,63>::value,
            line<43, 0>::value, line<43, 1>::value, line<43, 2>::value, line<43, 3>::value, line<43, 4>::value, line<43, 5>::value, line<43, 6>::value, line<43, 7>::value,
            line<43, 8>::value, line<43, 9>::value, line<43,10>::value, line<43,11>::value, line<43,12>::value, line<43,13>::value, line<43,14>::value, line<43,15>::value,
            line<43,16>::value, line<43,17>::value, line<43,18>::value, line<43,19>::value, line<43,20>::value, line<43,21>::value, line<43,22>::value, line<43,23>::value,
            line<43,24>::value, line<43,25>::value, line<43,26>::value, line<43,27>::value, line<43,28>::value, line<43,29>::value, line<43,30>::value, line<43,31>::value,
            line<43,32>::value, line<43,33>::value, line<43,34>::value, line<43,35>::value, line<43,36>::value, line<43,37>::value, line<43,38>::value, line<43,39>::value,
            line<43,40>::value, line<43,41>::value, line<43,42>::value, line<43,43>::value, line<43,44>::value, line<43,45>::value, line<43,46>::value, line<43,47>::value,
            line<43,48>::value, line<43,49>::value, line<43,50>::value, line<43,51>::value, line<43,52>::value, line<43,53>::value, line<43,54>::value, line<43,55>::value,
            line<43,56>::value, line<43,57>::value, line<43,58>::value, line<43,59>::value, line<43,60>::value, line<43,61>::value, line<43,62>::value, line<43,63>::value,
            line<44, 0>::value, line<44, 1>::value, line<44, 2>::value, line<44, 3>::value, line<44, 4>::value, line<44, 5>::value, line<44, 6>::value, line<44, 7>::value,
            line<44, 8>::value, line<44, 9>::value, line<44,10>::value, line<44,11>::value, line<44,12>::value, line<44,13>::value, line<44,14>::value, line<44,15>::value,
            line<44,16>::value, line<44,17>::value, line<44,18>::value, line<44,19>::value, line<44,20>::value, line<44,21>::value, line<44,22>::value, line<44,23>::value,
            line<44,24>::value, line<44,25>::value, line<44,26>::value, line<44,27>::value, line<44,28>::value, line<44,29>::value, line<44,30>::value, line<44,31>::value,
            line<44,32>::value, line<44,33>::value, line<44,34>::value, line<44,35>::value, line<44,36>::value, line<44,37>::value, line<44,38>::value, line<44,39>::value,
            line<44,40>::value, line<44,41>::value, line<44,42>::value, line<44,43>::value, line<44,44>::value, line<44,45>::value, line<44,46>::value, line<44,47>::value,
            line<44,48>::value, line<44,49>::value, line<44,50>::value, line<44,51>::value, line<44,52>::value, line<44,53>::value, line<44,54>::value, line<44,55>::value,
            line<44,56>::value, line<44,57>::value, line<44,58>::value, line<44,59>::value, line<44,60>::value, line<44,61>::value, line<44,62>::value, line<44,63>::value,
            line<45, 0>::value, line<45, 1>::value, line<45, 2>::value, line<45, 3>::value, line<45, 4>::value, line<45, 5>::value, line<45, 6>::value, line<45, 7>::value,
            line<45, 8>::value, line<45, 9>::value, line<45,10>::value, line<45,11>::value, line<45,12>::value, line<45,13>::value, line<45,14>::value, line<45,15>::value,
            line<45,16>::value, line<45,17>::value, line<45,18>::value, line<45,19>::value, line<45,20>::value, line<45,21>::value, line<45,22>::value, line<45,23>::value,
            line<45,24>::value, line<45,25>::value, line<45,26>::value, line<45,27>::value, line<45,28>::value, line<45,29>::value, line<45,30>::value, line<45,31>::value,
            line<45,32>::value, line<45,33>::value, line<45,34>::value, line<45,35>::value, line<45,36>::value, line<45,37>::value, line<45,38>::value, line<45,39>::value,
            line<45,40>::value, line<45,41>::value, line<45,42>::value, line<45,43>::value, line<45,44>::value, line<45,45>::value, line<45,46>::value, line<45,47>::value,
            line<45,48>::value, line<45,49>::value, line<45,50>::value, line<45,51>::value, line<45,52>::value, line<45,53>::value, line<45,54>::value, line<45,55>::value,
            line<45,56>::value, line<45,57>::value, line<45,58>::value, line<45,59>::value, line<45,60>::value, line<45,61>::value, line<45,62>::value, line<45,63>::value,
            line<46, 0>::value, line<46, 1>::value, line<46, 2>::value, line<46, 3>::value, line<46, 4>::value, line<46, 5>::value, line<46, 6>::value, line<46, 7>::value,
            line<46, 8>::value, line<46, 9>::value, line<46,10>::value, line<46,11>::value, line<46,12>::value, line<46,13>::value, line<46,14>::value, line<46,15>::value,
            line<46,16>::value, line<46,17>::value, line<46,18>::value, line<46,19>::value, line<46,20>::value, line<46,21>::value, line<46,22>::value, line<46,23>::value,
            line<46,24>::value, line<46,25>::value, line<46,26>::value, line<46,27>::value, line<46,28>::value, line<46,29>::value, line<46,30>::value, line<46,31>::value,
            line<46,32>::value, line<46,33>::value, line<46,34>::value, line<46,35>::value, line<46,36>::value, line<46,37>::value, line<46,38>::value, line<46,39>::value,
            line<46,40>::value, line<46,41>::value, line<46,42>::value, line<46,43>::value, line<46,44>::value, line<46,45>::value, line<46,46>::value, line<46,47>::value,
            line<46,48>::value, line<46,49>::value, line<46,50>::value, line<46,51>::value, line<46,52>::value, line<46,53>::value, line<46,54>::value, line<46,55>::value,
            line<46,56>::value, line<46,57>::value, line<46,58>::value, line<46,59>::value, line<46,60>::value, line<46,61>::value, line<46,62>::value, line<46,63>::value,
            line<47, 0>::value, line<47, 1>::value, line<47, 2>::value, line<47, 3>::value, line<47, 4>::value, line<47, 5>::value, line<47, 6>::value, line<47, 7>::value,
            line<47, 8>::value, line<47, 9>::value, line<47,10>::value, line<47,11>::value, line<47,12>::value, line<47,13>::value, line<47,14>::value, line<47,15>::value,
            line<47,16>::value, line<47,17>::value, line<47,18>::value, line<47,19>::value, line<47,20>::value, line<47,21>::value, line<47,22>::value, line<47,23>::value,
            line<47,24>::value, line<47,25>::value, line<47,26>::value, line<47,27>::value, line<47,28>::value, line<47,29>::value, line<47,30>::value, line<47,31>::value,
            line<47,32>::value, line<47,33>::value, line<47,34>::value, line<47,35>::value, line<47,36>::value, line<47,37>::value, line<47,38>::value, line<47,39>::value,
            line<47,40>::value, line<47,41>::value, line<47,42>::value, line<47,43>::value, line<47,44>::value, line<47,45>::value, line<47,46>::value, line<47,47>::value,
            line<47,48>::value, line<47,49>::value, line<47,50>::value, line<47,51>::value, line<47,52>::value, line<47,53>::value, line<47,54>::value, line<47,55>::value,
            line<47,56>::value, line<47,57>::value, line<47,58>::value, line<47,59>::value, line<47,60>::value, line<47,61>::value, line<47,62>::value, line<47,63>::value,
            line<48, 0>::value, line<48, 1>::value, line<48, 2>::value, line<48, 3>::value, line<48, 4>::value, line<48, 5>::value, line<48, 6>::value, line<48, 7>::value,
            line<48, 8>::value, line<48, 9>::value, line<48,10>::value, line<48,11>::value, line<48,12>::value, line<48,13>::value, line<48,14>::value, line<48,15>::value,
            line<48,16>::value, line<48,17>::value, line<48,18>::value, line<48,19>::value, line<48,20>::value, line<48,21>::value, line<48,22>::value, line<48,23>::value,
            line<48,24>::value, line<48,25>::value, line<48,26>::value, line<48,27>::value, line<48,28>::value, line<48,29>::value, line<48,30>::value, line<48,31>::value,
            line<48,32>::value, line<48,33>::value, line<48,34>::value, line<48,35>::value, line<48,36>::value, line<48,37>::value, line<48,38>::value, line<48,39>::value,
            line<48,40>::value, line<48,41>::value, line<48,42>::value, line<48,43>::value, line<48,44>::value, line<48,45>::value, line<48,46>::value, line<48,47>::value,
            line<48,48>::value, line<48,49>::value, line<48,50>::value, line<48,51>::value, line<48,52>::value, line<48,53>::value, line<48,54>::value, line<48,55>::value,
            line<48,56>::value, line<48,57>::value, line<48,58>::value, line<48,59>::value, line<48,60>::value, line<48,61>::value, line<48,62>::value, line<48,63>::value,
            line<49, 0>::value, line<49, 1>::value, line<49, 2>::value, line<49, 3>::value, line<49, 4>::value, line<49, 5>::value, line<49, 6>::value, line<49, 7>::value,
            line<49, 8>::value, line<49, 9>::value, line<49,10>::value, line<49,11>::value, line<49,12>::value, line<49,13>::value, line<49,14>::value, line<49,15>::value,
            line<49,16>::value, line<49,17>::value, line<49,18>::value, line<49,19>::value, line<49,20>::value, line<49,21>::value, line<49,22>::value, line<49,23>::value,
            line<49,24>::value, line<49,25>::value, line<49,26>::value, line<49,27>::value, line<49,28>::value, line<49,29>::value, line<49,30>::value, line<49,31>::value,
            line<49,32>::value, line<49,33>::value, line<49,34>::value, line<49,35>::value, line<49,36>::value, line<49,37>::value, line<49,38>::value, line<49,39>::value,
            line<49,40>::value, line<49,41>::value, line<49,42>::value, line<49,43>::value, line<49,44>::value, line<49,45>::value, line<49,46>::value, line<49,47>::value,
            line<49,48>::value, line<49,49>::value, line<49,50>::value, line<49,51>::value, line<49,52>::value, line<49,53>::value, line<49,54>::value, line<49,55>::value,
            line<49,56>::value, line<49,57>::value, line<49,58>::value, line<49,59>::value, line<49,60>::value, line<49,61>::value, line<49,62>::value, line<49,63>::value,
            line<50, 0>::value, line<50, 1>::value, line<50, 2>::value, line<50, 3>::value, line<50, 4>::value, line<50, 5>::value, line<50, 6>::value, line<50, 7>::value,
            line<50, 8>::value, line<50, 9>::value, line<50,10>::value, line<50,11>::value, line<50,12>::value, line<50,13>::value, line<50,14>::value, line<50,15>::value,
            line<50,16>::value, line<50,17>::value, line<50,18>::value, line<50,19>::value, line<50,20>::value, line<50,21>::value, line<50,22>::value, line<50,23>::value,
            line<50,24>::value, line<50,25>::value, line<50,26>::value, line<50,27>::value, line<50,28>::value, line<50,29>::value, line<50,30>::value, line<50,31>::value,
            line<50,32>::value, line<50,33>::value, line<50,34>::value, line<50,35>::value, line<50,36>::value, line<50,37>::value, line<50,38>::value, line<50,39>::value,
            line<50,40>::value, line<50,41>::value, line<50,42>::value, line<50,43>::value, line<50,44>::value, line<50,45>::value, line<50,46>::value, line<50,47>::value,
            line<50,48>::value, line<50,49>::value, line<50,50>::value, line<50,51>::value, line<50,52>::value, line<50,53>::value, line<50,54>::value, line<50,55>::value,
            line<50,56>::value, line<50,57>::value, line<50,58>::value, line<50,59>::value, line<50,60>::value, line<50,61>::value, line<50,62>::value, line<50,63>::value,
            line<51, 0>::value, line<51, 1>::value, line<51, 2>::value, line<51, 3>::value, line<51, 4>::value, line<51, 5>::value, line<51, 6>::value, line<51, 7>::value,
            line<51, 8>::value, line<51, 9>::value, line<51,10>::value, line<51,11>::value, line<51,12>::value, line<51,13>::value, line<51,14>::value, line<51,15>::value,
            line<51,16>::value, line<51,17>::value, line<51,18>::value, line<51,19>::value, line<51,20>::value, line<51,21>::value, line<51,22>::value, line<51,23>::value,
            line<51,24>::value, line<51,25>::value, line<51,26>::value, line<51,27>::value, line<51,28>::value, line<51,29>::value, line<51,30>::value, line<51,31>::value,
            line<51,32>::value, line<51,33>::value, line<51,34>::value, line<51,35>::value, line<51,36>::value, line<51,37>::value, line<51,38>::value, line<51,39>::value,
            line<51,40>::value, line<51,41>::value, line<51,42>::value, line<51,43>::value, line<51,44>::value, line<51,45>::value, line<51,46>::value, line<51,47>::value,
            line<51,48>::value, line<51,49>::value, line<51,50>::value, line<51,51>::value, line<51,52>::value, line<51,53>::value, line<51,54>::value, line<51,55>::value,
            line<51,56>::value, line<51,57>::value, line<51,58>::value, line<51,59>::value, line<51,60>::value, line<51,61>::value, line<51,62>::value, line<51,63>::value,
            line<52, 0>::value, line<52, 1>::value, line<52, 2>::value, line<52, 3>::value, line<52, 4>::value, line<52, 5>::value, line<52, 6>::value, line<52, 7>::value,
            line<52, 8>::value, line<52, 9>::value, line<52,10>::value, line<52,11>::value, line<52,12>::value, line<52,13>::value, line<52,14>::value, line<52,15>::value,
            line<52,16>::value, line<52,17>::value, line<52,18>::value, line<52,19>::value, line<52,20>::value, line<52,21>::value, line<52,22>::value, line<52,23>::value,
            line<52,24>::value, line<52,25>::value, line<52,26>::value, line<52,27>::value, line<52,28>::value, line<52,29>::value, line<52,30>::value, line<52,31>::value,
            line<52,32>::value, line<52,33>::value, line<52,34>::value, line<52,35>::value, line<52,36>::value, line<52,37>::value, line<52,38>::value, line<52,39>::value,
            line<52,40>::value, line<52,41>::value, line<52,42>::value, line<52,43>::value, line<52,44>::value, line<52,45>::value, line<52,46>::value, line<52,47>::value,
            line<52,48>::value, line<52,49>::value, line<52,50>::value, line<52,51>::value, line<52,52>::value, line<52,53>::value, line<52,54>::value, line<52,55>::value,
            line<52,56>::value, line<52,57>::value, line<52,58>::value, line<52,59>::value, line<52,60>::value, line<52,61>::value, line<52,62>::value, line<52,63>::value,
            line<53, 0>::value, line<53, 1>::value, line<53, 2>::value, line<53, 3>::value, line<53, 4>::value, line<53, 5>::value, line<53, 6>::value, line<53, 7>::value,
            line<53, 8>::value, line<53, 9>::value, line<53,10>::value, line<53,11>::value, line<53,12>::value, line<53,13>::value, line<53,14>::value, line<53,15>::value,
            line<53,16>::value, line<53,17>::value, line<53,18>::value, line<53,19>::value, line<53,20>::value, line<53,21>::value, line<53,22>::value, line<53,23>::value,
            line<53,24>::value, line<53,25>::value, line<53,26>::value, line<53,27>::value, line<53,28>::value, line<53,29>::value, line<53,30>::value, line<53,31>::value,
            line<53,32>::value, line<53,33>::value, line<53,34>::value, line<53,35>::value, line<53,36>::value, line<53,37>::value, line<53,38>::value, line<53,39>::value,
            line<53,40>::value, line<53,41>::value, line<53,42>::value, line<53,43>::value, line<53,44>::value, line<53,45>::value, line<53,46>::value, line<53,47>::value,
            line<53,48>::value, line<53,49>::value, line<53,50>::value, line<53,51>::value, line<53,52>::value, line<53,53>::value, line<53,54>::value, line<53,55>::value,
            line<53,56>::value, line<53,57>::value, line<53,58>::value, line<53,59>::value, line<53,60>::value, line<53,61>::value, line<53,62>::value, line<53,63>::value,
            line<54, 0>::value, line<54, 1>::value, line<54, 2>::value, line<54, 3>::value, line<54, 4>::value, line<54, 5>::value, line<54, 6>::value, line<54, 7>::value,
            line<54, 8>::value, line<54, 9>::value, line<54,10>::value, line<54,11>::value, line<54,12>::value, line<54,13>::value, line<54,14>::value, line<54,15>::value,
            line<54,16>::value, line<54,17>::value, line<54,18>::value, line<54,19>::value, line<54,20>::value, line<54,21>::value, line<54,22>::value, line<54,23>::value,
            line<54,24>::value, line<54,25>::value, line<54,26>::value, line<54,27>::value, line<54,28>::value, line<54,29>::value, line<54,30>::value, line<54,31>::value,
            line<54,32>::value, line<54,33>::value, line<54,34>::value, line<54,35>::value, line<54,36>::value, line<54,37>::value, line<54,38>::value, line<54,39>::value,
            line<54,40>::value, line<54,41>::value, line<54,42>::value, line<54,43>::value, line<54,44>::value, line<54,45>::value, line<54,46>::value, line<54,47>::value,
            line<54,48>::value, line<54,49>::value, line<54,50>::value, line<54,51>::value, line<54,52>::value, line<54,53>::value, line<54,54>::value, line<54,55>::value,
            line<54,56>::value, line<54,57>::value, line<54,58>::value, line<54,59>::value, line<54,60>::value, line<54,61>::value, line<54,62>::value, line<54,63>::value,
            line<55, 0>::value, line<55, 1>::value, line<55, 2>::value, line<55, 3>::value, line<55, 4>::value, line<55, 5>::value, line<55, 6>::value, line<55, 7>::value,
            line<55, 8>::value, line<55, 9>::value, line<55,10>::value, line<55,11>::value, line<55,12>::value, line<55,13>::value, line<55,14>::value, line<55,15>::value,
            line<55,16>::value, line<55,17>::value, line<55,18>::value, line<55,19>::value, line<55,20>::value, line<55,21>::value, line<55,22>::value, line<55,23>::value,
            line<55,24>::value, line<55,25>::value, line<55,26>::value, line<55,27>::value, line<55,28>::value, line<55,29>::value, line<55,30>::value, line<55,31>::value,
            line<55,32>::value, line<55,33>::value, line<55,34>::value, line<55,35>::value, line<55,36>::value, line<55,37>::value, line<55,38>::value, line<55,39>::value,
            line<55,40>::value, line<55,41>::value, line<55,42>::value, line<55,43>::value, line<55,44>::value, line<55,45>::value, line<55,46>::value, line<55,47>::value,
            line<55,48>::value, line<55,49>::value, line<55,50>::value, line<55,51>::value, line<55,52>::value, line<55,53>::value, line<55,54>::value, line<55,55>::value,
            line<55,56>::value, line<55,57>::value, line<55,58>::value, line<55,59>::value, line<55,60>::value, line<55,61>::value, line<55,62>::value, line<55,63>::value,
            line<56, 0>::value, line<56, 1>::value, line<56, 2>::value, line<56, 3>::value, line<56, 4>::value, line<56, 5>::value, line<56, 6>::value, line<56, 7>::value,
            line<56, 8>::value, line<56, 9>::value, line<56,10>::value, line<56,11>::value, line<56,12>::value, line<56,13>::value, line<56,14>::value, line<56,15>::value,
            line<56,16>::value, line<56,17>::value, line<56,18>::value, line<56,19>::value, line<56,20>::value, line<56,21>::value, line<56,22>::value, line<56,23>::value,
            line<56,24>::value, line<56,25>::value, line<56,26>::value, line<56,27>::value, line<56,28>::value, line<56,29>::value, line<56,30>::value, line<56,31>::value,
            line<56,32>::value, line<56,33>::value, line<56,34>::value, line<56,35>::value, line<56,36>::value, line<56,37>::value, line<56,38>::value, line<56,39>::value,
            line<56,40>::value, line<56,41>::value, line<56,42>::value, line<56,43>::value, line<56,44>::value, line<56,45>::value, line<56,46>::value, line<56,47>::value,
            line<56,48>::value, line<56,49>::value, line<56,50>::value, line<56,51>::value, line<56,52>::value, line<56,53>::value, line<56,54>::value, line<56,55>::value,
            line<56,56>::value, line<56,57>::value, line<56,58>::value, line<56,59>::value, line<56,60>::value, line<56,61>::value, line<56,62>::value, line<56,63>::value,
            line<57, 0>::value, line<57, 1>::value, line<57, 2>::value, line<57, 3>::value, line<57, 4>::value, line<57, 5>::value, line<57, 6>::value, line<57, 7>::value,
            line<57, 8>::value, line<57, 9>::value, line<57,10>::value, line<57,11>::value, line<57,12>::value, line<57,13>::value, line<57,14>::value, line<57,15>::value,
            line<57,16>::value, line<57,17>::value, line<57,18>::value, line<57,19>::value, line<57,20>::value, line<57,21>::value, line<57,22>::value, line<57,23>::value,
            line<57,24>::value, line<57,25>::value, line<57,26>::value, line<57,27>::value, line<57,28>::value, line<57,29>::value, line<57,30>::value, line<57,31>::value,
            line<57,32>::value, line<57,33>::value, line<57,34>::value, line<57,35>::value, line<57,36>::value, line<57,37>::value, line<57,38>::value, line<57,39>::value,
            line<57,40>::value, line<57,41>::value, line<57,42>::value, line<57,43>::value, line<57,44>::value, line<57,45>::value, line<57,46>::value, line<57,47>::value,
            line<57,48>::value, line<57,49>::value, line<57,50>::value, line<57,51>::value, line<57,52>::value, line<57,53>::value, line<57,54>::value, line<57,55>::value,
            line<57,56>::value, line<57,57>::value, line<57,58>::value, line<57,59>::value, line<57,60>::value, line<57,61>::value, line<57,62>::value, line<57,63>::value,
            line<58, 0>::value, line<58, 1>::value, line<58, 2>::value, line<58, 3>::value, line<58, 4>::value, line<58, 5>::value, line<58, 6>::value, line<58, 7>::value,
            line<58, 8>::value, line<58, 9>::value, line<58,10>::value, line<58,11>::value, line<58,12>::value, line<58,13>::value, line<58,14>::value, line<58,15>::value,
            line<58,16>::value, line<58,17>::value, line<58,18>::value, line<58,19>::value, line<58,20>::value, line<58,21>::value, line<58,22>::value, line<58,23>::value,
            line<58,24>::value, line<58,25>::value, line<58,26>::value, line<58,27>::value, line<58,28>::value, line<58,29>::value, line<58,30>::value, line<58,31>::value,
            line<58,32>::value, line<58,33>::value, line<58,34>::value, line<58,35>::value, line<58,36>::value, line<58,37>::value, line<58,38>::value, line<58,39>::value,
            line<58,40>::value, line<58,41>::value, line<58,42>::value, line<58,43>::value, line<58,44>::value, line<58,45>::value, line<58,46>::value, line<58,47>::value,
            line<58,48>::value, line<58,49>::value, line<58,50>::value, line<58,51>::value, line<58,52>::value, line<58,53>::value, line<58,54>::value, line<58,55>::value,
            line<58,56>::value, line<58,57>::value, line<58,58>::value, line<58,59>::value, line<58,60>::value, line<58,61>::value, line<58,62>::value, line<58,63>::value,
            line<59, 0>::value, line<59, 1>::value, line<59, 2>::value, line<59, 3>::value, line<59, 4>::value, line<59, 5>::value, line<59, 6>::value, line<59, 7>::value,
            line<59, 8>::value, line<59, 9>::value, line<59,10>::value, line<59,11>::value, line<59,12>::value, line<59,13>::value, line<59,14>::value, line<59,15>::value,
            line<59,16>::value, line<59,17>::value, line<59,18>::value, line<59,19>::value, line<59,20>::value, line<59,21>::value, line<59,22>::value, line<59,23>::value,
            line<59,24>::value, line<59,25>::value, line<59,26>::value, line<59,27>::value, line<59,28>::value, line<59,29>::value, line<59,30>::value, line<59,31>::value,
            line<59,32>::value, line<59,33>::value, line<59,34>::value, line<59,35>::value, line<59,36>::value, line<59,37>::value, line<59,38>::value, line<59,39>::value,
            line<59,40>::value, line<59,41>::value, line<59,42>::value, line<59,43>::value, line<59,44>::value, line<59,45>::value, line<59,46>::value, line<59,47>::value,
            line<59,48>::value, line<59,49>::value, line<59,50>::value, line<59,51>::value, line<59,52>::value, line<59,53>::value, line<59,54>::value, line<59,55>::value,
            line<59,56>::value, line<59,57>::value, line<59,58>::value, line<59,59>::value, line<59,60>::value, line<59,61>::value, line<59,62>::value, line<59,63>::value,
            line<60, 0>::value, line<60, 1>::value, line<60, 2>::value, line<60, 3>::value, line<60, 4>::value, line<60, 5>::value, line<60, 6>::value, line<60, 7>::value,
            line<60, 8>::value, line<60, 9>::value, line<60,10>::value, line<60,11>::value, line<60,12>::value, line<60,13>::value, line<60,14>::value, line<60,15>::value,
            line<60,16>::value, line<60,17>::value, line<60,18>::value, line<60,19>::value, line<60,20>::value, line<60,21>::value, line<60,22>::value, line<60,23>::value,
            line<60,24>::value, line<60,25>::value, line<60,26>::value, line<60,27>::value, line<60,28>::value, line<60,29>::value, line<60,30>::value, line<60,31>::value,
            line<60,32>::value, line<60,33>::value, line<60,34>::value, line<60,35>::value, line<60,36>::value, line<60,37>::value, line<60,38>::value, line<60,39>::value,
            line<60,40>::value, line<60,41>::value, line<60,42>::value, line<60,43>::value, line<60,44>::value, line<60,45>::value, line<60,46>::value, line<60,47>::value,
            line<60,48>::value, line<60,49>::value, line<60,50>::value, line<60,51>::value, line<60,52>::value, line<60,53>::value, line<60,54>::value, line<60,55>::value,
            line<60,56>::value, line<60,57>::value, line<60,58>::value, line<60,59>::value, line<60,60>::value, line<60,61>::value, line<60,62>::value, line<60,63>::value,
            line<61, 0>::value, line<61, 1>::value, line<61, 2>::value, line<61, 3>::value, line<61, 4>::value, line<61, 5>::value, line<61, 6>::value, line<61, 7>::value,
            line<61, 8>::value, line<61, 9>::value, line<61,10>::value, line<61,11>::value, line<61,12>::value, line<61,13>::value, line<61,14>::value, line<61,15>::value,
            line<61,16>::value, line<61,17>::value, line<61,18>::value, line<61,19>::value, line<61,20>::value, line<61,21>::value, line<61,22>::value, line<61,23>::value,
            line<61,24>::value, line<61,25>::value, line<61,26>::value, line<61,27>::value, line<61,28>::value, line<61,29>::value, line<61,30>::value, line<61,31>::value,
            line<61,32>::value, line<61,33>::value, line<61,34>::value, line<61,35>::value, line<61,36>::value, line<61,37>::value, line<61,38>::value, line<61,39>::value,
            line<61,40>::value, line<61,41>::value, line<61,42>::value, line<61,43>::value, line<61,44>::value, line<61,45>::value, line<61,46>::value, line<61,47>::value,
            line<61,48>::value, line<61,49>::value, line<61,50>::value, line<61,51>::value, line<61,52>::value, line<61,53>::value, line<61,54>::value, line<61,55>::value,
            line<61,56>::value, line<61,57>::value, line<61,58>::value, line<61,59>::value, line<61,60>::value, line<61,61>::value, line<61,62>::value, line<61,63>::value,
            line<62, 0>::value, line<62, 1>::value, line<62, 2>::value, line<62, 3>::value, line<62, 4>::value, line<62, 5>::value, line<62, 6>::value, line<62, 7>::value,
            line<62, 8>::value, line<62, 9>::value, line<62,10>::value, line<62,11>::value, line<62,12>::value, line<62,13>::value, line<62,14>::value, line<62,15>::value,
            line<62,16>::value, line<62,17>::value, line<62,18>::value, line<62,19>::value, line<62,20>::value, line<62,21>::value, line<62,22>::value, line<62,23>::value,
            line<62,24>::value, line<62,25>::value, line<62,26>::value, line<62,27>::value, line<62,28>::value, line<62,29>::value, line<62,30>::value, line<62,31>::value,
            line<62,32>::value, line<62,33>::value, line<62,34>::value, line<62,35>::value, line<62,36>::value, line<62,37>::value, line<62,38>::value, line<62,39>::value,
            line<62,40>::value, line<62,41>::value, line<62,42>::value, line<62,43>::value, line<62,44>::value, line<62,45>::value, line<62,46>::value, line<62,47>::value,
            line<62,48>::value, line<62,49>::value, line<62,50>::value, line<62,51>::value, line<62,52>::value, line<62,53>::value, line<62,54>::value, line<62,55>::value,
            line<62,56>::value, line<62,57>::value, line<62,58>::value, line<62,59>::value, line<62,60>::value, line<62,61>::value, line<62,62>::value, line<62,63>::value,
            line<63, 0>::value, line<63, 1>::value, line<63, 2>::value, line<63, 3>::value, line<63, 4>::value, line<63, 5>::value, line<63, 6>::value, line<63, 7>::value,
            line<63, 8>::value, line<63, 9>::value, line<63,10>::value, line<63,11>::value, line<63,12>::value, line<63,13>::value, line<63,14>::value, line<63,15>::value,
            line<63,16>::value, line<63,17>::value, line<63,18>::value, line<63,19>::value, line<63,20>::value, line<63,21>::value, line<63,22>::value, line<63,23>::value,
            line<63,24>::value, line<63,25>::value, line<63,26>::value, line<63,27>::value, line<63,28>::value, line<63,29>::value, line<63,30>::value, line<63,31>::value,
            line<63,32>::value, line<63,33>::value, line<63,34>::value, line<63,35>::value, line<63,36>::value, line<63,37>::value, line<63,38>::value, line<63,39>::value,
            line<63,40>::value, line<63,41>::value, line<63,42>::value, line<63,43>::value, line<63,44>::value, line<63,45>::value, line<63,46>::value, line<63,47>::value,
            line<63,48>::value, line<63,49>::value, line<63,50>::value, line<63,51>::value, line<63,52>::value, line<63,53>::value, line<63,54>::value, line<63,55>::value,
            line<63,56>::value, line<63,57>::value, line<63,58>::value, line<63,59>::value, line<63,60>::value, line<63,61>::value, line<63,62>::value, line<63,63>::value
        };

        const std::array<std::array<std::array<U64, 8>, 64>, 3> rays = {
            ray<bishop, 0, N>::value, ray<bishop, 0,NW>::value, ray<bishop, 0, W>::value, ray<bishop, 0,SW>::value, ray<bishop, 0, S>::value, ray<bishop, 0,SE>::value, ray<bishop, 0, E>::value, ray<bishop, 0,NE>::value,
            ray<bishop, 1, N>::value, ray<bishop, 1,NW>::value, ray<bishop, 1, W>::value, ray<bishop, 1,SW>::value, ray<bishop, 1, S>::value, ray<bishop, 1,SE>::value, ray<bishop, 1, E>::value, ray<bishop, 1,NE>::value,
            ray<bishop, 2, N>::value, ray<bishop, 2,NW>::value, ray<bishop, 2, W>::value, ray<bishop, 2,SW>::value, ray<bishop, 2, S>::value, ray<bishop, 2,SE>::value, ray<bishop, 2, E>::value, ray<bishop, 2,NE>::value,
            ray<bishop, 3, N>::value, ray<bishop, 3,NW>::value, ray<bishop, 3, W>::value, ray<bishop, 3,SW>::value, ray<bishop, 3, S>::value, ray<bishop, 3,SE>::value, ray<bishop, 3, E>::value, ray<bishop, 3,NE>::value,
            ray<bishop, 4, N>::value, ray<bishop, 4,NW>::value, ray<bishop, 4, W>::value, ray<bishop, 4,SW>::value, ray<bishop, 4, S>::value, ray<bishop, 4,SE>::value, ray<bishop, 4, E>::value, ray<bishop, 4,NE>::value,
            ray<bishop, 5, N>::value, ray<bishop, 5,NW>::value, ray<bishop, 5, W>::value, ray<bishop, 5,SW>::value, ray<bishop, 5, S>::value, ray<bishop, 5,SE>::value, ray<bishop, 5, E>::value, ray<bishop, 5,NE>::value,
            ray<bishop, 6, N>::value, ray<bishop, 6,NW>::value, ray<bishop, 6, W>::value, ray<bishop, 6,SW>::value, ray<bishop, 6, S>::value, ray<bishop, 6,SE>::value, ray<bishop, 6, E>::value, ray<bishop, 6,NE>::value,
            ray<bishop, 7, N>::value, ray<bishop, 7,NW>::value, ray<bishop, 7, W>::value, ray<bishop, 7,SW>::value, ray<bishop, 7, S>::value, ray<bishop, 7,SE>::value, ray<bishop, 7, E>::value, ray<bishop, 7,NE>::value,
            ray<bishop, 8, N>::value, ray<bishop, 8,NW>::value, ray<bishop, 8, W>::value, ray<bishop, 8,SW>::value, ray<bishop, 8, S>::value, ray<bishop, 8,SE>::value, ray<bishop, 8, E>::value, ray<bishop, 8,NE>::value,
            ray<bishop, 9, N>::value, ray<bishop, 9,NW>::value, ray<bishop, 9, W>::value, ray<bishop, 9,SW>::value, ray<bishop, 9, S>::value, ray<bishop, 9,SE>::value, ray<bishop, 9, E>::value, ray<bishop, 9,NE>::value,
            ray<bishop,10, N>::value, ray<bishop,10,NW>::value, ray<bishop,10, W>::value, ray<bishop,10,SW>::value, ray<bishop,10, S>::value, ray<bishop,10,SE>::value, ray<bishop,10, E>::value, ray<bishop,10,NE>::value,
            ray<bishop,11, N>::value, ray<bishop,11,NW>::value, ray<bishop,11, W>::value, ray<bishop,11,SW>::value, ray<bishop,11, S>::value, ray<bishop,11,SE>::value, ray<bishop,11, E>::value, ray<bishop,11,NE>::value,
            ray<bishop,12, N>::value, ray<bishop,12,NW>::value, ray<bishop,12, W>::value, ray<bishop,12,SW>::value, ray<bishop,12, S>::value, ray<bishop,12,SE>::value, ray<bishop,12, E>::value, ray<bishop,12,NE>::value,
            ray<bishop,13, N>::value, ray<bishop,13,NW>::value, ray<bishop,13, W>::value, ray<bishop,13,SW>::value, ray<bishop,13, S>::value, ray<bishop,13,SE>::value, ray<bishop,13, E>::value, ray<bishop,13,NE>::value,
            ray<bishop,14, N>::value, ray<bishop,14,NW>::value, ray<bishop,14, W>::value, ray<bishop,14,SW>::value, ray<bishop,14, S>::value, ray<bishop,14,SE>::value, ray<bishop,14, E>::value, ray<bishop,14,NE>::value,
            ray<bishop,15, N>::value, ray<bishop,15,NW>::value, ray<bishop,15, W>::value, ray<bishop,15,SW>::value, ray<bishop,15, S>::value, ray<bishop,15,SE>::value, ray<bishop,15, E>::value, ray<bishop,15,NE>::value,
            ray<bishop,16, N>::value, ray<bishop,16,NW>::value, ray<bishop,16, W>::value, ray<bishop,16,SW>::value, ray<bishop,16, S>::value, ray<bishop,16,SE>::value, ray<bishop,16, E>::value, ray<bishop,16,NE>::value,
            ray<bishop,17, N>::value, ray<bishop,17,NW>::value, ray<bishop,17, W>::value, ray<bishop,17,SW>::value, ray<bishop,17, S>::value, ray<bishop,17,SE>::value, ray<bishop,17, E>::value, ray<bishop,17,NE>::value,
            ray<bishop,18, N>::value, ray<bishop,18,NW>::value, ray<bishop,18, W>::value, ray<bishop,18,SW>::value, ray<bishop,18, S>::value, ray<bishop,18,SE>::value, ray<bishop,18, E>::value, ray<bishop,18,NE>::value,
            ray<bishop,19, N>::value, ray<bishop,19,NW>::value, ray<bishop,19, W>::value, ray<bishop,19,SW>::value, ray<bishop,19, S>::value, ray<bishop,19,SE>::value, ray<bishop,19, E>::value, ray<bishop,19,NE>::value,
            ray<bishop,20, N>::value, ray<bishop,20,NW>::value, ray<bishop,20, W>::value, ray<bishop,20,SW>::value, ray<bishop,20, S>::value, ray<bishop,20,SE>::value, ray<bishop,20, E>::value, ray<bishop,20,NE>::value,
            ray<bishop,21, N>::value, ray<bishop,21,NW>::value, ray<bishop,21, W>::value, ray<bishop,21,SW>::value, ray<bishop,21, S>::value, ray<bishop,21,SE>::value, ray<bishop,21, E>::value, ray<bishop,21,NE>::value,
            ray<bishop,22, N>::value, ray<bishop,22,NW>::value, ray<bishop,22, W>::value, ray<bishop,22,SW>::value, ray<bishop,22, S>::value, ray<bishop,22,SE>::value, ray<bishop,22, E>::value, ray<bishop,22,NE>::value,
            ray<bishop,23, N>::value, ray<bishop,23,NW>::value, ray<bishop,23, W>::value, ray<bishop,23,SW>::value, ray<bishop,23, S>::value, ray<bishop,23,SE>::value, ray<bishop,23, E>::value, ray<bishop,23,NE>::value,
            ray<bishop,24, N>::value, ray<bishop,24,NW>::value, ray<bishop,24, W>::value, ray<bishop,24,SW>::value, ray<bishop,24, S>::value, ray<bishop,24,SE>::value, ray<bishop,24, E>::value, ray<bishop,24,NE>::value,
            ray<bishop,25, N>::value, ray<bishop,25,NW>::value, ray<bishop,25, W>::value, ray<bishop,25,SW>::value, ray<bishop,25, S>::value, ray<bishop,25,SE>::value, ray<bishop,25, E>::value, ray<bishop,25,NE>::value,
            ray<bishop,26, N>::value, ray<bishop,26,NW>::value, ray<bishop,26, W>::value, ray<bishop,26,SW>::value, ray<bishop,26, S>::value, ray<bishop,26,SE>::value, ray<bishop,26, E>::value, ray<bishop,26,NE>::value,
            ray<bishop,27, N>::value, ray<bishop,27,NW>::value, ray<bishop,27, W>::value, ray<bishop,27,SW>::value, ray<bishop,27, S>::value, ray<bishop,27,SE>::value, ray<bishop,27, E>::value, ray<bishop,27,NE>::value,
            ray<bishop,28, N>::value, ray<bishop,28,NW>::value, ray<bishop,28, W>::value, ray<bishop,28,SW>::value, ray<bishop,28, S>::value, ray<bishop,28,SE>::value, ray<bishop,28, E>::value, ray<bishop,28,NE>::value,
            ray<bishop,29, N>::value, ray<bishop,29,NW>::value, ray<bishop,29, W>::value, ray<bishop,29,SW>::value, ray<bishop,29, S>::value, ray<bishop,29,SE>::value, ray<bishop,29, E>::value, ray<bishop,29,NE>::value,
            ray<bishop,30, N>::value, ray<bishop,30,NW>::value, ray<bishop,30, W>::value, ray<bishop,30,SW>::value, ray<bishop,30, S>::value, ray<bishop,30,SE>::value, ray<bishop,30, E>::value, ray<bishop,30,NE>::value,
            ray<bishop,31, N>::value, ray<bishop,31,NW>::value, ray<bishop,31, W>::value, ray<bishop,31,SW>::value, ray<bishop,31, S>::value, ray<bishop,31,SE>::value, ray<bishop,31, E>::value, ray<bishop,31,NE>::value,
            ray<bishop,32, N>::value, ray<bishop,32,NW>::value, ray<bishop,32, W>::value, ray<bishop,32,SW>::value, ray<bishop,32, S>::value, ray<bishop,32,SE>::value, ray<bishop,32, E>::value, ray<bishop,32,NE>::value,
            ray<bishop,33, N>::value, ray<bishop,33,NW>::value, ray<bishop,33, W>::value, ray<bishop,33,SW>::value, ray<bishop,33, S>::value, ray<bishop,33,SE>::value, ray<bishop,33, E>::value, ray<bishop,33,NE>::value,
            ray<bishop,34, N>::value, ray<bishop,34,NW>::value, ray<bishop,34, W>::value, ray<bishop,34,SW>::value, ray<bishop,34, S>::value, ray<bishop,34,SE>::value, ray<bishop,34, E>::value, ray<bishop,34,NE>::value,
            ray<bishop,35, N>::value, ray<bishop,35,NW>::value, ray<bishop,35, W>::value, ray<bishop,35,SW>::value, ray<bishop,35, S>::value, ray<bishop,35,SE>::value, ray<bishop,35, E>::value, ray<bishop,35,NE>::value,
            ray<bishop,36, N>::value, ray<bishop,36,NW>::value, ray<bishop,36, W>::value, ray<bishop,36,SW>::value, ray<bishop,36, S>::value, ray<bishop,36,SE>::value, ray<bishop,36, E>::value, ray<bishop,36,NE>::value,
            ray<bishop,37, N>::value, ray<bishop,37,NW>::value, ray<bishop,37, W>::value, ray<bishop,37,SW>::value, ray<bishop,37, S>::value, ray<bishop,37,SE>::value, ray<bishop,37, E>::value, ray<bishop,37,NE>::value,
            ray<bishop,38, N>::value, ray<bishop,38,NW>::value, ray<bishop,38, W>::value, ray<bishop,38,SW>::value, ray<bishop,38, S>::value, ray<bishop,38,SE>::value, ray<bishop,38, E>::value, ray<bishop,38,NE>::value,
            ray<bishop,39, N>::value, ray<bishop,39,NW>::value, ray<bishop,39, W>::value, ray<bishop,39,SW>::value, ray<bishop,39, S>::value, ray<bishop,39,SE>::value, ray<bishop,39, E>::value, ray<bishop,39,NE>::value,
            ray<bishop,40, N>::value, ray<bishop,40,NW>::value, ray<bishop,40, W>::value, ray<bishop,40,SW>::value, ray<bishop,40, S>::value, ray<bishop,40,SE>::value, ray<bishop,40, E>::value, ray<bishop,40,NE>::value,
            ray<bishop,41, N>::value, ray<bishop,41,NW>::value, ray<bishop,41, W>::value, ray<bishop,41,SW>::value, ray<bishop,41, S>::value, ray<bishop,41,SE>::value, ray<bishop,41, E>::value, ray<bishop,41,NE>::value,
            ray<bishop,42, N>::value, ray<bishop,42,NW>::value, ray<bishop,42, W>::value, ray<bishop,42,SW>::value, ray<bishop,42, S>::value, ray<bishop,42,SE>::value, ray<bishop,42, E>::value, ray<bishop,42,NE>::value,
            ray<bishop,43, N>::value, ray<bishop,43,NW>::value, ray<bishop,43, W>::value, ray<bishop,43,SW>::value, ray<bishop,43, S>::value, ray<bishop,43,SE>::value, ray<bishop,43, E>::value, ray<bishop,43,NE>::value,
            ray<bishop,44, N>::value, ray<bishop,44,NW>::value, ray<bishop,44, W>::value, ray<bishop,44,SW>::value, ray<bishop,44, S>::value, ray<bishop,44,SE>::value, ray<bishop,44, E>::value, ray<bishop,44,NE>::value,
            ray<bishop,45, N>::value, ray<bishop,45,NW>::value, ray<bishop,45, W>::value, ray<bishop,45,SW>::value, ray<bishop,45, S>::value, ray<bishop,45,SE>::value, ray<bishop,45, E>::value, ray<bishop,45,NE>::value,
            ray<bishop,46, N>::value, ray<bishop,46,NW>::value, ray<bishop,46, W>::value, ray<bishop,46,SW>::value, ray<bishop,46, S>::value, ray<bishop,46,SE>::value, ray<bishop,46, E>::value, ray<bishop,46,NE>::value,
            ray<bishop,47, N>::value, ray<bishop,47,NW>::value, ray<bishop,47, W>::value, ray<bishop,47,SW>::value, ray<bishop,47, S>::value, ray<bishop,47,SE>::value, ray<bishop,47, E>::value, ray<bishop,47,NE>::value,
            ray<bishop,48, N>::value, ray<bishop,48,NW>::value, ray<bishop,48, W>::value, ray<bishop,48,SW>::value, ray<bishop,48, S>::value, ray<bishop,48,SE>::value, ray<bishop,48, E>::value, ray<bishop,48,NE>::value,
            ray<bishop,49, N>::value, ray<bishop,49,NW>::value, ray<bishop,49, W>::value, ray<bishop,49,SW>::value, ray<bishop,49, S>::value, ray<bishop,49,SE>::value, ray<bishop,49, E>::value, ray<bishop,49,NE>::value,
            ray<bishop,50, N>::value, ray<bishop,50,NW>::value, ray<bishop,50, W>::value, ray<bishop,50,SW>::value, ray<bishop,50, S>::value, ray<bishop,50,SE>::value, ray<bishop,50, E>::value, ray<bishop,50,NE>::value,
            ray<bishop,51, N>::value, ray<bishop,51,NW>::value, ray<bishop,51, W>::value, ray<bishop,51,SW>::value, ray<bishop,51, S>::value, ray<bishop,51,SE>::value, ray<bishop,51, E>::value, ray<bishop,51,NE>::value,
            ray<bishop,52, N>::value, ray<bishop,52,NW>::value, ray<bishop,52, W>::value, ray<bishop,52,SW>::value, ray<bishop,52, S>::value, ray<bishop,52,SE>::value, ray<bishop,52, E>::value, ray<bishop,52,NE>::value,
            ray<bishop,53, N>::value, ray<bishop,53,NW>::value, ray<bishop,53, W>::value, ray<bishop,53,SW>::value, ray<bishop,53, S>::value, ray<bishop,53,SE>::value, ray<bishop,53, E>::value, ray<bishop,53,NE>::value,
            ray<bishop,54, N>::value, ray<bishop,54,NW>::value, ray<bishop,54, W>::value, ray<bishop,54,SW>::value, ray<bishop,54, S>::value, ray<bishop,54,SE>::value, ray<bishop,54, E>::value, ray<bishop,54,NE>::value,
            ray<bishop,55, N>::value, ray<bishop,55,NW>::value, ray<bishop,55, W>::value, ray<bishop,55,SW>::value, ray<bishop,55, S>::value, ray<bishop,55,SE>::value, ray<bishop,55, E>::value, ray<bishop,55,NE>::value,
            ray<bishop,56, N>::value, ray<bishop,56,NW>::value, ray<bishop,56, W>::value, ray<bishop,56,SW>::value, ray<bishop,56, S>::value, ray<bishop,56,SE>::value, ray<bishop,56, E>::value, ray<bishop,56,NE>::value,
            ray<bishop,57, N>::value, ray<bishop,57,NW>::value, ray<bishop,57, W>::value, ray<bishop,57,SW>::value, ray<bishop,57, S>::value, ray<bishop,57,SE>::value, ray<bishop,57, E>::value, ray<bishop,57,NE>::value,
            ray<bishop,58, N>::value, ray<bishop,58,NW>::value, ray<bishop,58, W>::value, ray<bishop,58,SW>::value, ray<bishop,58, S>::value, ray<bishop,58,SE>::value, ray<bishop,58, E>::value, ray<bishop,58,NE>::value,
            ray<bishop,59, N>::value, ray<bishop,59,NW>::value, ray<bishop,59, W>::value, ray<bishop,59,SW>::value, ray<bishop,59, S>::value, ray<bishop,59,SE>::value, ray<bishop,59, E>::value, ray<bishop,59,NE>::value,
            ray<bishop,60, N>::value, ray<bishop,60,NW>::value, ray<bishop,60, W>::value, ray<bishop,60,SW>::value, ray<bishop,60, S>::value, ray<bishop,60,SE>::value, ray<bishop,60, E>::value, ray<bishop,60,NE>::value,
            ray<bishop,61, N>::value, ray<bishop,61,NW>::value, ray<bishop,61, W>::value, ray<bishop,61,SW>::value, ray<bishop,61, S>::value, ray<bishop,61,SE>::value, ray<bishop,61, E>::value, ray<bishop,61,NE>::value,
            ray<bishop,62, N>::value, ray<bishop,62,NW>::value, ray<bishop,62, W>::value, ray<bishop,62,SW>::value, ray<bishop,62, S>::value, ray<bishop,62,SE>::value, ray<bishop,62, E>::value, ray<bishop,62,NE>::value,
            ray<bishop,63, N>::value, ray<bishop,63,NW>::value, ray<bishop,63, W>::value, ray<bishop,63,SW>::value, ray<bishop,63, S>::value, ray<bishop,63,SE>::value, ray<bishop,63, E>::value, ray<bishop,63,NE>::value,
            ray<  rook, 0, N>::value, ray<  rook, 0,NW>::value, ray<  rook, 0, W>::value, ray<  rook, 0,SW>::value, ray<  rook, 0, S>::value, ray<  rook, 0,SE>::value, ray<  rook, 0, E>::value, ray<  rook, 0,NE>::value,
            ray<  rook, 1, N>::value, ray<  rook, 1,NW>::value, ray<  rook, 1, W>::value, ray<  rook, 1,SW>::value, ray<  rook, 1, S>::value, ray<  rook, 1,SE>::value, ray<  rook, 1, E>::value, ray<  rook, 1,NE>::value,
            ray<  rook, 2, N>::value, ray<  rook, 2,NW>::value, ray<  rook, 2, W>::value, ray<  rook, 2,SW>::value, ray<  rook, 2, S>::value, ray<  rook, 2,SE>::value, ray<  rook, 2, E>::value, ray<  rook, 2,NE>::value,
            ray<  rook, 3, N>::value, ray<  rook, 3,NW>::value, ray<  rook, 3, W>::value, ray<  rook, 3,SW>::value, ray<  rook, 3, S>::value, ray<  rook, 3,SE>::value, ray<  rook, 3, E>::value, ray<  rook, 3,NE>::value,
            ray<  rook, 4, N>::value, ray<  rook, 4,NW>::value, ray<  rook, 4, W>::value, ray<  rook, 4,SW>::value, ray<  rook, 4, S>::value, ray<  rook, 4,SE>::value, ray<  rook, 4, E>::value, ray<  rook, 4,NE>::value,
            ray<  rook, 5, N>::value, ray<  rook, 5,NW>::value, ray<  rook, 5, W>::value, ray<  rook, 5,SW>::value, ray<  rook, 5, S>::value, ray<  rook, 5,SE>::value, ray<  rook, 5, E>::value, ray<  rook, 5,NE>::value,
            ray<  rook, 6, N>::value, ray<  rook, 6,NW>::value, ray<  rook, 6, W>::value, ray<  rook, 6,SW>::value, ray<  rook, 6, S>::value, ray<  rook, 6,SE>::value, ray<  rook, 6, E>::value, ray<  rook, 6,NE>::value,
            ray<  rook, 7, N>::value, ray<  rook, 7,NW>::value, ray<  rook, 7, W>::value, ray<  rook, 7,SW>::value, ray<  rook, 7, S>::value, ray<  rook, 7,SE>::value, ray<  rook, 7, E>::value, ray<  rook, 7,NE>::value,
            ray<  rook, 8, N>::value, ray<  rook, 8,NW>::value, ray<  rook, 8, W>::value, ray<  rook, 8,SW>::value, ray<  rook, 8, S>::value, ray<  rook, 8,SE>::value, ray<  rook, 8, E>::value, ray<  rook, 8,NE>::value,
            ray<  rook, 9, N>::value, ray<  rook, 9,NW>::value, ray<  rook, 9, W>::value, ray<  rook, 9,SW>::value, ray<  rook, 9, S>::value, ray<  rook, 9,SE>::value, ray<  rook, 9, E>::value, ray<  rook, 9,NE>::value,
            ray<  rook,10, N>::value, ray<  rook,10,NW>::value, ray<  rook,10, W>::value, ray<  rook,10,SW>::value, ray<  rook,10, S>::value, ray<  rook,10,SE>::value, ray<  rook,10, E>::value, ray<  rook,10,NE>::value,
            ray<  rook,11, N>::value, ray<  rook,11,NW>::value, ray<  rook,11, W>::value, ray<  rook,11,SW>::value, ray<  rook,11, S>::value, ray<  rook,11,SE>::value, ray<  rook,11, E>::value, ray<  rook,11,NE>::value,
            ray<  rook,12, N>::value, ray<  rook,12,NW>::value, ray<  rook,12, W>::value, ray<  rook,12,SW>::value, ray<  rook,12, S>::value, ray<  rook,12,SE>::value, ray<  rook,12, E>::value, ray<  rook,12,NE>::value,
            ray<  rook,13, N>::value, ray<  rook,13,NW>::value, ray<  rook,13, W>::value, ray<  rook,13,SW>::value, ray<  rook,13, S>::value, ray<  rook,13,SE>::value, ray<  rook,13, E>::value, ray<  rook,13,NE>::value,
            ray<  rook,14, N>::value, ray<  rook,14,NW>::value, ray<  rook,14, W>::value, ray<  rook,14,SW>::value, ray<  rook,14, S>::value, ray<  rook,14,SE>::value, ray<  rook,14, E>::value, ray<  rook,14,NE>::value,
            ray<  rook,15, N>::value, ray<  rook,15,NW>::value, ray<  rook,15, W>::value, ray<  rook,15,SW>::value, ray<  rook,15, S>::value, ray<  rook,15,SE>::value, ray<  rook,15, E>::value, ray<  rook,15,NE>::value,
            ray<  rook,16, N>::value, ray<  rook,16,NW>::value, ray<  rook,16, W>::value, ray<  rook,16,SW>::value, ray<  rook,16, S>::value, ray<  rook,16,SE>::value, ray<  rook,16, E>::value, ray<  rook,16,NE>::value,
            ray<  rook,17, N>::value, ray<  rook,17,NW>::value, ray<  rook,17, W>::value, ray<  rook,17,SW>::value, ray<  rook,17, S>::value, ray<  rook,17,SE>::value, ray<  rook,17, E>::value, ray<  rook,17,NE>::value,
            ray<  rook,18, N>::value, ray<  rook,18,NW>::value, ray<  rook,18, W>::value, ray<  rook,18,SW>::value, ray<  rook,18, S>::value, ray<  rook,18,SE>::value, ray<  rook,18, E>::value, ray<  rook,18,NE>::value,
            ray<  rook,19, N>::value, ray<  rook,19,NW>::value, ray<  rook,19, W>::value, ray<  rook,19,SW>::value, ray<  rook,19, S>::value, ray<  rook,19,SE>::value, ray<  rook,19, E>::value, ray<  rook,19,NE>::value,
            ray<  rook,20, N>::value, ray<  rook,20,NW>::value, ray<  rook,20, W>::value, ray<  rook,20,SW>::value, ray<  rook,20, S>::value, ray<  rook,20,SE>::value, ray<  rook,20, E>::value, ray<  rook,20,NE>::value,
            ray<  rook,21, N>::value, ray<  rook,21,NW>::value, ray<  rook,21, W>::value, ray<  rook,21,SW>::value, ray<  rook,21, S>::value, ray<  rook,21,SE>::value, ray<  rook,21, E>::value, ray<  rook,21,NE>::value,
            ray<  rook,22, N>::value, ray<  rook,22,NW>::value, ray<  rook,22, W>::value, ray<  rook,22,SW>::value, ray<  rook,22, S>::value, ray<  rook,22,SE>::value, ray<  rook,22, E>::value, ray<  rook,22,NE>::value,
            ray<  rook,23, N>::value, ray<  rook,23,NW>::value, ray<  rook,23, W>::value, ray<  rook,23,SW>::value, ray<  rook,23, S>::value, ray<  rook,23,SE>::value, ray<  rook,23, E>::value, ray<  rook,23,NE>::value,
            ray<  rook,24, N>::value, ray<  rook,24,NW>::value, ray<  rook,24, W>::value, ray<  rook,24,SW>::value, ray<  rook,24, S>::value, ray<  rook,24,SE>::value, ray<  rook,24, E>::value, ray<  rook,24,NE>::value,
            ray<  rook,25, N>::value, ray<  rook,25,NW>::value, ray<  rook,25, W>::value, ray<  rook,25,SW>::value, ray<  rook,25, S>::value, ray<  rook,25,SE>::value, ray<  rook,25, E>::value, ray<  rook,25,NE>::value,
            ray<  rook,26, N>::value, ray<  rook,26,NW>::value, ray<  rook,26, W>::value, ray<  rook,26,SW>::value, ray<  rook,26, S>::value, ray<  rook,26,SE>::value, ray<  rook,26, E>::value, ray<  rook,26,NE>::value,
            ray<  rook,27, N>::value, ray<  rook,27,NW>::value, ray<  rook,27, W>::value, ray<  rook,27,SW>::value, ray<  rook,27, S>::value, ray<  rook,27,SE>::value, ray<  rook,27, E>::value, ray<  rook,27,NE>::value,
            ray<  rook,28, N>::value, ray<  rook,28,NW>::value, ray<  rook,28, W>::value, ray<  rook,28,SW>::value, ray<  rook,28, S>::value, ray<  rook,28,SE>::value, ray<  rook,28, E>::value, ray<  rook,28,NE>::value,
            ray<  rook,29, N>::value, ray<  rook,29,NW>::value, ray<  rook,29, W>::value, ray<  rook,29,SW>::value, ray<  rook,29, S>::value, ray<  rook,29,SE>::value, ray<  rook,29, E>::value, ray<  rook,29,NE>::value,
            ray<  rook,30, N>::value, ray<  rook,30,NW>::value, ray<  rook,30, W>::value, ray<  rook,30,SW>::value, ray<  rook,30, S>::value, ray<  rook,30,SE>::value, ray<  rook,30, E>::value, ray<  rook,30,NE>::value,
            ray<  rook,31, N>::value, ray<  rook,31,NW>::value, ray<  rook,31, W>::value, ray<  rook,31,SW>::value, ray<  rook,31, S>::value, ray<  rook,31,SE>::value, ray<  rook,31, E>::value, ray<  rook,31,NE>::value,
            ray<  rook,32, N>::value, ray<  rook,32,NW>::value, ray<  rook,32, W>::value, ray<  rook,32,SW>::value, ray<  rook,32, S>::value, ray<  rook,32,SE>::value, ray<  rook,32, E>::value, ray<  rook,32,NE>::value,
            ray<  rook,33, N>::value, ray<  rook,33,NW>::value, ray<  rook,33, W>::value, ray<  rook,33,SW>::value, ray<  rook,33, S>::value, ray<  rook,33,SE>::value, ray<  rook,33, E>::value, ray<  rook,33,NE>::value,
            ray<  rook,34, N>::value, ray<  rook,34,NW>::value, ray<  rook,34, W>::value, ray<  rook,34,SW>::value, ray<  rook,34, S>::value, ray<  rook,34,SE>::value, ray<  rook,34, E>::value, ray<  rook,34,NE>::value,
            ray<  rook,35, N>::value, ray<  rook,35,NW>::value, ray<  rook,35, W>::value, ray<  rook,35,SW>::value, ray<  rook,35, S>::value, ray<  rook,35,SE>::value, ray<  rook,35, E>::value, ray<  rook,35,NE>::value,
            ray<  rook,36, N>::value, ray<  rook,36,NW>::value, ray<  rook,36, W>::value, ray<  rook,36,SW>::value, ray<  rook,36, S>::value, ray<  rook,36,SE>::value, ray<  rook,36, E>::value, ray<  rook,36,NE>::value,
            ray<  rook,37, N>::value, ray<  rook,37,NW>::value, ray<  rook,37, W>::value, ray<  rook,37,SW>::value, ray<  rook,37, S>::value, ray<  rook,37,SE>::value, ray<  rook,37, E>::value, ray<  rook,37,NE>::value,
            ray<  rook,38, N>::value, ray<  rook,38,NW>::value, ray<  rook,38, W>::value, ray<  rook,38,SW>::value, ray<  rook,38, S>::value, ray<  rook,38,SE>::value, ray<  rook,38, E>::value, ray<  rook,38,NE>::value,
            ray<  rook,39, N>::value, ray<  rook,39,NW>::value, ray<  rook,39, W>::value, ray<  rook,39,SW>::value, ray<  rook,39, S>::value, ray<  rook,39,SE>::value, ray<  rook,39, E>::value, ray<  rook,39,NE>::value,
            ray<  rook,40, N>::value, ray<  rook,40,NW>::value, ray<  rook,40, W>::value, ray<  rook,40,SW>::value, ray<  rook,40, S>::value, ray<  rook,40,SE>::value, ray<  rook,40, E>::value, ray<  rook,40,NE>::value,
            ray<  rook,41, N>::value, ray<  rook,41,NW>::value, ray<  rook,41, W>::value, ray<  rook,41,SW>::value, ray<  rook,41, S>::value, ray<  rook,41,SE>::value, ray<  rook,41, E>::value, ray<  rook,41,NE>::value,
            ray<  rook,42, N>::value, ray<  rook,42,NW>::value, ray<  rook,42, W>::value, ray<  rook,42,SW>::value, ray<  rook,42, S>::value, ray<  rook,42,SE>::value, ray<  rook,42, E>::value, ray<  rook,42,NE>::value,
            ray<  rook,43, N>::value, ray<  rook,43,NW>::value, ray<  rook,43, W>::value, ray<  rook,43,SW>::value, ray<  rook,43, S>::value, ray<  rook,43,SE>::value, ray<  rook,43, E>::value, ray<  rook,43,NE>::value,
            ray<  rook,44, N>::value, ray<  rook,44,NW>::value, ray<  rook,44, W>::value, ray<  rook,44,SW>::value, ray<  rook,44, S>::value, ray<  rook,44,SE>::value, ray<  rook,44, E>::value, ray<  rook,44,NE>::value,
            ray<  rook,45, N>::value, ray<  rook,45,NW>::value, ray<  rook,45, W>::value, ray<  rook,45,SW>::value, ray<  rook,45, S>::value, ray<  rook,45,SE>::value, ray<  rook,45, E>::value, ray<  rook,45,NE>::value,
            ray<  rook,46, N>::value, ray<  rook,46,NW>::value, ray<  rook,46, W>::value, ray<  rook,46,SW>::value, ray<  rook,46, S>::value, ray<  rook,46,SE>::value, ray<  rook,46, E>::value, ray<  rook,46,NE>::value,
            ray<  rook,47, N>::value, ray<  rook,47,NW>::value, ray<  rook,47, W>::value, ray<  rook,47,SW>::value, ray<  rook,47, S>::value, ray<  rook,47,SE>::value, ray<  rook,47, E>::value, ray<  rook,47,NE>::value,
            ray<  rook,48, N>::value, ray<  rook,48,NW>::value, ray<  rook,48, W>::value, ray<  rook,48,SW>::value, ray<  rook,48, S>::value, ray<  rook,48,SE>::value, ray<  rook,48, E>::value, ray<  rook,48,NE>::value,
            ray<  rook,49, N>::value, ray<  rook,49,NW>::value, ray<  rook,49, W>::value, ray<  rook,49,SW>::value, ray<  rook,49, S>::value, ray<  rook,49,SE>::value, ray<  rook,49, E>::value, ray<  rook,49,NE>::value,
            ray<  rook,50, N>::value, ray<  rook,50,NW>::value, ray<  rook,50, W>::value, ray<  rook,50,SW>::value, ray<  rook,50, S>::value, ray<  rook,50,SE>::value, ray<  rook,50, E>::value, ray<  rook,50,NE>::value,
            ray<  rook,51, N>::value, ray<  rook,51,NW>::value, ray<  rook,51, W>::value, ray<  rook,51,SW>::value, ray<  rook,51, S>::value, ray<  rook,51,SE>::value, ray<  rook,51, E>::value, ray<  rook,51,NE>::value,
            ray<  rook,52, N>::value, ray<  rook,52,NW>::value, ray<  rook,52, W>::value, ray<  rook,52,SW>::value, ray<  rook,52, S>::value, ray<  rook,52,SE>::value, ray<  rook,52, E>::value, ray<  rook,52,NE>::value,
            ray<  rook,53, N>::value, ray<  rook,53,NW>::value, ray<  rook,53, W>::value, ray<  rook,53,SW>::value, ray<  rook,53, S>::value, ray<  rook,53,SE>::value, ray<  rook,53, E>::value, ray<  rook,53,NE>::value,
            ray<  rook,54, N>::value, ray<  rook,54,NW>::value, ray<  rook,54, W>::value, ray<  rook,54,SW>::value, ray<  rook,54, S>::value, ray<  rook,54,SE>::value, ray<  rook,54, E>::value, ray<  rook,54,NE>::value,
            ray<  rook,55, N>::value, ray<  rook,55,NW>::value, ray<  rook,55, W>::value, ray<  rook,55,SW>::value, ray<  rook,55, S>::value, ray<  rook,55,SE>::value, ray<  rook,55, E>::value, ray<  rook,55,NE>::value,
            ray<  rook,56, N>::value, ray<  rook,56,NW>::value, ray<  rook,56, W>::value, ray<  rook,56,SW>::value, ray<  rook,56, S>::value, ray<  rook,56,SE>::value, ray<  rook,56, E>::value, ray<  rook,56,NE>::value,
            ray<  rook,57, N>::value, ray<  rook,57,NW>::value, ray<  rook,57, W>::value, ray<  rook,57,SW>::value, ray<  rook,57, S>::value, ray<  rook,57,SE>::value, ray<  rook,57, E>::value, ray<  rook,57,NE>::value,
            ray<  rook,58, N>::value, ray<  rook,58,NW>::value, ray<  rook,58, W>::value, ray<  rook,58,SW>::value, ray<  rook,58, S>::value, ray<  rook,58,SE>::value, ray<  rook,58, E>::value, ray<  rook,58,NE>::value,
            ray<  rook,59, N>::value, ray<  rook,59,NW>::value, ray<  rook,59, W>::value, ray<  rook,59,SW>::value, ray<  rook,59, S>::value, ray<  rook,59,SE>::value, ray<  rook,59, E>::value, ray<  rook,59,NE>::value,
            ray<  rook,60, N>::value, ray<  rook,60,NW>::value, ray<  rook,60, W>::value, ray<  rook,60,SW>::value, ray<  rook,60, S>::value, ray<  rook,60,SE>::value, ray<  rook,60, E>::value, ray<  rook,60,NE>::value,
            ray<  rook,61, N>::value, ray<  rook,61,NW>::value, ray<  rook,61, W>::value, ray<  rook,61,SW>::value, ray<  rook,61, S>::value, ray<  rook,61,SE>::value, ray<  rook,61, E>::value, ray<  rook,61,NE>::value,
            ray<  rook,62, N>::value, ray<  rook,62,NW>::value, ray<  rook,62, W>::value, ray<  rook,62,SW>::value, ray<  rook,62, S>::value, ray<  rook,62,SE>::value, ray<  rook,62, E>::value, ray<  rook,62,NE>::value,
            ray<  rook,63, N>::value, ray<  rook,63,NW>::value, ray<  rook,63, W>::value, ray<  rook,63,SW>::value, ray<  rook,63, S>::value, ray<  rook,63,SE>::value, ray<  rook,63, E>::value, ray<  rook,63,NE>::value,
            ray< queen, 0, N>::value, ray< queen, 0,NW>::value, ray< queen, 0, W>::value, ray< queen, 0,SW>::value, ray< queen, 0, S>::value, ray< queen, 0,SE>::value, ray< queen, 0, E>::value, ray< queen, 0,NE>::value,
            ray< queen, 1, N>::value, ray< queen, 1,NW>::value, ray< queen, 1, W>::value, ray< queen, 1,SW>::value, ray< queen, 1, S>::value, ray< queen, 1,SE>::value, ray< queen, 1, E>::value, ray< queen, 1,NE>::value,
            ray< queen, 2, N>::value, ray< queen, 2,NW>::value, ray< queen, 2, W>::value, ray< queen, 2,SW>::value, ray< queen, 2, S>::value, ray< queen, 2,SE>::value, ray< queen, 2, E>::value, ray< queen, 2,NE>::value,
            ray< queen, 3, N>::value, ray< queen, 3,NW>::value, ray< queen, 3, W>::value, ray< queen, 3,SW>::value, ray< queen, 3, S>::value, ray< queen, 3,SE>::value, ray< queen, 3, E>::value, ray< queen, 3,NE>::value,
            ray< queen, 4, N>::value, ray< queen, 4,NW>::value, ray< queen, 4, W>::value, ray< queen, 4,SW>::value, ray< queen, 4, S>::value, ray< queen, 4,SE>::value, ray< queen, 4, E>::value, ray< queen, 4,NE>::value,
            ray< queen, 5, N>::value, ray< queen, 5,NW>::value, ray< queen, 5, W>::value, ray< queen, 5,SW>::value, ray< queen, 5, S>::value, ray< queen, 5,SE>::value, ray< queen, 5, E>::value, ray< queen, 5,NE>::value,
            ray< queen, 6, N>::value, ray< queen, 6,NW>::value, ray< queen, 6, W>::value, ray< queen, 6,SW>::value, ray< queen, 6, S>::value, ray< queen, 6,SE>::value, ray< queen, 6, E>::value, ray< queen, 6,NE>::value,
            ray< queen, 7, N>::value, ray< queen, 7,NW>::value, ray< queen, 7, W>::value, ray< queen, 7,SW>::value, ray< queen, 7, S>::value, ray< queen, 7,SE>::value, ray< queen, 7, E>::value, ray< queen, 7,NE>::value,
            ray< queen, 8, N>::value, ray< queen, 8,NW>::value, ray< queen, 8, W>::value, ray< queen, 8,SW>::value, ray< queen, 8, S>::value, ray< queen, 8,SE>::value, ray< queen, 8, E>::value, ray< queen, 8,NE>::value,
            ray< queen, 9, N>::value, ray< queen, 9,NW>::value, ray< queen, 9, W>::value, ray< queen, 9,SW>::value, ray< queen, 9, S>::value, ray< queen, 9,SE>::value, ray< queen, 9, E>::value, ray< queen, 9,NE>::value,
            ray< queen,10, N>::value, ray< queen,10,NW>::value, ray< queen,10, W>::value, ray< queen,10,SW>::value, ray< queen,10, S>::value, ray< queen,10,SE>::value, ray< queen,10, E>::value, ray< queen,10,NE>::value,
            ray< queen,11, N>::value, ray< queen,11,NW>::value, ray< queen,11, W>::value, ray< queen,11,SW>::value, ray< queen,11, S>::value, ray< queen,11,SE>::value, ray< queen,11, E>::value, ray< queen,11,NE>::value,
            ray< queen,12, N>::value, ray< queen,12,NW>::value, ray< queen,12, W>::value, ray< queen,12,SW>::value, ray< queen,12, S>::value, ray< queen,12,SE>::value, ray< queen,12, E>::value, ray< queen,12,NE>::value,
            ray< queen,13, N>::value, ray< queen,13,NW>::value, ray< queen,13, W>::value, ray< queen,13,SW>::value, ray< queen,13, S>::value, ray< queen,13,SE>::value, ray< queen,13, E>::value, ray< queen,13,NE>::value,
            ray< queen,14, N>::value, ray< queen,14,NW>::value, ray< queen,14, W>::value, ray< queen,14,SW>::value, ray< queen,14, S>::value, ray< queen,14,SE>::value, ray< queen,14, E>::value, ray< queen,14,NE>::value,
            ray< queen,15, N>::value, ray< queen,15,NW>::value, ray< queen,15, W>::value, ray< queen,15,SW>::value, ray< queen,15, S>::value, ray< queen,15,SE>::value, ray< queen,15, E>::value, ray< queen,15,NE>::value,
            ray< queen,16, N>::value, ray< queen,16,NW>::value, ray< queen,16, W>::value, ray< queen,16,SW>::value, ray< queen,16, S>::value, ray< queen,16,SE>::value, ray< queen,16, E>::value, ray< queen,16,NE>::value,
            ray< queen,17, N>::value, ray< queen,17,NW>::value, ray< queen,17, W>::value, ray< queen,17,SW>::value, ray< queen,17, S>::value, ray< queen,17,SE>::value, ray< queen,17, E>::value, ray< queen,17,NE>::value,
            ray< queen,18, N>::value, ray< queen,18,NW>::value, ray< queen,18, W>::value, ray< queen,18,SW>::value, ray< queen,18, S>::value, ray< queen,18,SE>::value, ray< queen,18, E>::value, ray< queen,18,NE>::value,
            ray< queen,19, N>::value, ray< queen,19,NW>::value, ray< queen,19, W>::value, ray< queen,19,SW>::value, ray< queen,19, S>::value, ray< queen,19,SE>::value, ray< queen,19, E>::value, ray< queen,19,NE>::value,
            ray< queen,20, N>::value, ray< queen,20,NW>::value, ray< queen,20, W>::value, ray< queen,20,SW>::value, ray< queen,20, S>::value, ray< queen,20,SE>::value, ray< queen,20, E>::value, ray< queen,20,NE>::value,
            ray< queen,21, N>::value, ray< queen,21,NW>::value, ray< queen,21, W>::value, ray< queen,21,SW>::value, ray< queen,21, S>::value, ray< queen,21,SE>::value, ray< queen,21, E>::value, ray< queen,21,NE>::value,
            ray< queen,22, N>::value, ray< queen,22,NW>::value, ray< queen,22, W>::value, ray< queen,22,SW>::value, ray< queen,22, S>::value, ray< queen,22,SE>::value, ray< queen,22, E>::value, ray< queen,22,NE>::value,
            ray< queen,23, N>::value, ray< queen,23,NW>::value, ray< queen,23, W>::value, ray< queen,23,SW>::value, ray< queen,23, S>::value, ray< queen,23,SE>::value, ray< queen,23, E>::value, ray< queen,23,NE>::value,
            ray< queen,24, N>::value, ray< queen,24,NW>::value, ray< queen,24, W>::value, ray< queen,24,SW>::value, ray< queen,24, S>::value, ray< queen,24,SE>::value, ray< queen,24, E>::value, ray< queen,24,NE>::value,
            ray< queen,25, N>::value, ray< queen,25,NW>::value, ray< queen,25, W>::value, ray< queen,25,SW>::value, ray< queen,25, S>::value, ray< queen,25,SE>::value, ray< queen,25, E>::value, ray< queen,25,NE>::value,
            ray< queen,26, N>::value, ray< queen,26,NW>::value, ray< queen,26, W>::value, ray< queen,26,SW>::value, ray< queen,26, S>::value, ray< queen,26,SE>::value, ray< queen,26, E>::value, ray< queen,26,NE>::value,
            ray< queen,27, N>::value, ray< queen,27,NW>::value, ray< queen,27, W>::value, ray< queen,27,SW>::value, ray< queen,27, S>::value, ray< queen,27,SE>::value, ray< queen,27, E>::value, ray< queen,27,NE>::value,
            ray< queen,28, N>::value, ray< queen,28,NW>::value, ray< queen,28, W>::value, ray< queen,28,SW>::value, ray< queen,28, S>::value, ray< queen,28,SE>::value, ray< queen,28, E>::value, ray< queen,28,NE>::value,
            ray< queen,29, N>::value, ray< queen,29,NW>::value, ray< queen,29, W>::value, ray< queen,29,SW>::value, ray< queen,29, S>::value, ray< queen,29,SE>::value, ray< queen,29, E>::value, ray< queen,29,NE>::value,
            ray< queen,30, N>::value, ray< queen,30,NW>::value, ray< queen,30, W>::value, ray< queen,30,SW>::value, ray< queen,30, S>::value, ray< queen,30,SE>::value, ray< queen,30, E>::value, ray< queen,30,NE>::value,
            ray< queen,31, N>::value, ray< queen,31,NW>::value, ray< queen,31, W>::value, ray< queen,31,SW>::value, ray< queen,31, S>::value, ray< queen,31,SE>::value, ray< queen,31, E>::value, ray< queen,31,NE>::value,
            ray< queen,32, N>::value, ray< queen,32,NW>::value, ray< queen,32, W>::value, ray< queen,32,SW>::value, ray< queen,32, S>::value, ray< queen,32,SE>::value, ray< queen,32, E>::value, ray< queen,32,NE>::value,
            ray< queen,33, N>::value, ray< queen,33,NW>::value, ray< queen,33, W>::value, ray< queen,33,SW>::value, ray< queen,33, S>::value, ray< queen,33,SE>::value, ray< queen,33, E>::value, ray< queen,33,NE>::value,
            ray< queen,34, N>::value, ray< queen,34,NW>::value, ray< queen,34, W>::value, ray< queen,34,SW>::value, ray< queen,34, S>::value, ray< queen,34,SE>::value, ray< queen,34, E>::value, ray< queen,34,NE>::value,
            ray< queen,35, N>::value, ray< queen,35,NW>::value, ray< queen,35, W>::value, ray< queen,35,SW>::value, ray< queen,35, S>::value, ray< queen,35,SE>::value, ray< queen,35, E>::value, ray< queen,35,NE>::value,
            ray< queen,36, N>::value, ray< queen,36,NW>::value, ray< queen,36, W>::value, ray< queen,36,SW>::value, ray< queen,36, S>::value, ray< queen,36,SE>::value, ray< queen,36, E>::value, ray< queen,36,NE>::value,
            ray< queen,37, N>::value, ray< queen,37,NW>::value, ray< queen,37, W>::value, ray< queen,37,SW>::value, ray< queen,37, S>::value, ray< queen,37,SE>::value, ray< queen,37, E>::value, ray< queen,37,NE>::value,
            ray< queen,38, N>::value, ray< queen,38,NW>::value, ray< queen,38, W>::value, ray< queen,38,SW>::value, ray< queen,38, S>::value, ray< queen,38,SE>::value, ray< queen,38, E>::value, ray< queen,38,NE>::value,
            ray< queen,39, N>::value, ray< queen,39,NW>::value, ray< queen,39, W>::value, ray< queen,39,SW>::value, ray< queen,39, S>::value, ray< queen,39,SE>::value, ray< queen,39, E>::value, ray< queen,39,NE>::value,
            ray< queen,40, N>::value, ray< queen,40,NW>::value, ray< queen,40, W>::value, ray< queen,40,SW>::value, ray< queen,40, S>::value, ray< queen,40,SE>::value, ray< queen,40, E>::value, ray< queen,40,NE>::value,
            ray< queen,41, N>::value, ray< queen,41,NW>::value, ray< queen,41, W>::value, ray< queen,41,SW>::value, ray< queen,41, S>::value, ray< queen,41,SE>::value, ray< queen,41, E>::value, ray< queen,41,NE>::value,
            ray< queen,42, N>::value, ray< queen,42,NW>::value, ray< queen,42, W>::value, ray< queen,42,SW>::value, ray< queen,42, S>::value, ray< queen,42,SE>::value, ray< queen,42, E>::value, ray< queen,42,NE>::value,
            ray< queen,43, N>::value, ray< queen,43,NW>::value, ray< queen,43, W>::value, ray< queen,43,SW>::value, ray< queen,43, S>::value, ray< queen,43,SE>::value, ray< queen,43, E>::value, ray< queen,43,NE>::value,
            ray< queen,44, N>::value, ray< queen,44,NW>::value, ray< queen,44, W>::value, ray< queen,44,SW>::value, ray< queen,44, S>::value, ray< queen,44,SE>::value, ray< queen,44, E>::value, ray< queen,44,NE>::value,
            ray< queen,45, N>::value, ray< queen,45,NW>::value, ray< queen,45, W>::value, ray< queen,45,SW>::value, ray< queen,45, S>::value, ray< queen,45,SE>::value, ray< queen,45, E>::value, ray< queen,45,NE>::value,
            ray< queen,46, N>::value, ray< queen,46,NW>::value, ray< queen,46, W>::value, ray< queen,46,SW>::value, ray< queen,46, S>::value, ray< queen,46,SE>::value, ray< queen,46, E>::value, ray< queen,46,NE>::value,
            ray< queen,47, N>::value, ray< queen,47,NW>::value, ray< queen,47, W>::value, ray< queen,47,SW>::value, ray< queen,47, S>::value, ray< queen,47,SE>::value, ray< queen,47, E>::value, ray< queen,47,NE>::value,
            ray< queen,48, N>::value, ray< queen,48,NW>::value, ray< queen,48, W>::value, ray< queen,48,SW>::value, ray< queen,48, S>::value, ray< queen,48,SE>::value, ray< queen,48, E>::value, ray< queen,48,NE>::value,
            ray< queen,49, N>::value, ray< queen,49,NW>::value, ray< queen,49, W>::value, ray< queen,49,SW>::value, ray< queen,49, S>::value, ray< queen,49,SE>::value, ray< queen,49, E>::value, ray< queen,49,NE>::value,
            ray< queen,50, N>::value, ray< queen,50,NW>::value, ray< queen,50, W>::value, ray< queen,50,SW>::value, ray< queen,50, S>::value, ray< queen,50,SE>::value, ray< queen,50, E>::value, ray< queen,50,NE>::value,
            ray< queen,51, N>::value, ray< queen,51,NW>::value, ray< queen,51, W>::value, ray< queen,51,SW>::value, ray< queen,51, S>::value, ray< queen,51,SE>::value, ray< queen,51, E>::value, ray< queen,51,NE>::value,
            ray< queen,52, N>::value, ray< queen,52,NW>::value, ray< queen,52, W>::value, ray< queen,52,SW>::value, ray< queen,52, S>::value, ray< queen,52,SE>::value, ray< queen,52, E>::value, ray< queen,52,NE>::value,
            ray< queen,53, N>::value, ray< queen,53,NW>::value, ray< queen,53, W>::value, ray< queen,53,SW>::value, ray< queen,53, S>::value, ray< queen,53,SE>::value, ray< queen,53, E>::value, ray< queen,53,NE>::value,
            ray< queen,54, N>::value, ray< queen,54,NW>::value, ray< queen,54, W>::value, ray< queen,54,SW>::value, ray< queen,54, S>::value, ray< queen,54,SE>::value, ray< queen,54, E>::value, ray< queen,54,NE>::value,
            ray< queen,55, N>::value, ray< queen,55,NW>::value, ray< queen,55, W>::value, ray< queen,55,SW>::value, ray< queen,55, S>::value, ray< queen,55,SE>::value, ray< queen,55, E>::value, ray< queen,55,NE>::value,
            ray< queen,56, N>::value, ray< queen,56,NW>::value, ray< queen,56, W>::value, ray< queen,56,SW>::value, ray< queen,56, S>::value, ray< queen,56,SE>::value, ray< queen,56, E>::value, ray< queen,56,NE>::value,
            ray< queen,57, N>::value, ray< queen,57,NW>::value, ray< queen,57, W>::value, ray< queen,57,SW>::value, ray< queen,57, S>::value, ray< queen,57,SE>::value, ray< queen,57, E>::value, ray< queen,57,NE>::value,
            ray< queen,58, N>::value, ray< queen,58,NW>::value, ray< queen,58, W>::value, ray< queen,58,SW>::value, ray< queen,58, S>::value, ray< queen,58,SE>::value, ray< queen,58, E>::value, ray< queen,58,NE>::value,
            ray< queen,59, N>::value, ray< queen,59,NW>::value, ray< queen,59, W>::value, ray< queen,59,SW>::value, ray< queen,59, S>::value, ray< queen,59,SE>::value, ray< queen,59, E>::value, ray< queen,59,NE>::value,
            ray< queen,60, N>::value, ray< queen,60,NW>::value, ray< queen,60, W>::value, ray< queen,60,SW>::value, ray< queen,60, S>::value, ray< queen,60,SE>::value, ray< queen,60, E>::value, ray< queen,60,NE>::value,
            ray< queen,61, N>::value, ray< queen,61,NW>::value, ray< queen,61, W>::value, ray< queen,61,SW>::value, ray< queen,61, S>::value, ray< queen,61,SE>::value, ray< queen,61, E>::value, ray< queen,61,NE>::value,
            ray< queen,62, N>::value, ray< queen,62,NW>::value, ray< queen,62, W>::value, ray< queen,62,SW>::value, ray< queen,62, S>::value, ray< queen,62,SE>::value, ray< queen,62, E>::value, ray< queen,62,NE>::value,
            ray< queen,63, N>::value, ray< queen,63,NW>::value, ray< queen,63, W>::value, ray< queen,63,SW>::value, ray< queen,63, S>::value, ray< queen,63,SE>::value, ray< queen,63, E>::value, ray< queen,63,NE>::value
        };

        const std::string bviz = std::string(1, util::constants::endl)
                                + "   " + util::constants::files[0]
                                + "   " + util::constants::files[1]
                                + "   " + util::constants::files[2]
                                + "   " + util::constants::files[3]
                                + "   " + util::constants::files[4]
                                + "   " + util::constants::files[5]
                                + "   " + util::constants::files[6]
                                + "   " + util::constants::files[7]
                                + util::constants::endl + util::repr::rank2c(7) + ' '
                                + bcell + bcell + bcell + bcell + bcell + bcell + bcell + bcell
                                + util::constants::endl + util::repr::rank2c(6) + ' '
                                + bcell + bcell + bcell + bcell + bcell + bcell + bcell + bcell
                                + util::constants::endl + util::repr::rank2c(5) + ' '
                                + bcell + bcell + bcell + bcell + bcell + bcell + bcell + bcell
                                + util::constants::endl + util::repr::rank2c(4) + ' '
                                + bcell + bcell + bcell + bcell + bcell + bcell + bcell + bcell
                                + util::constants::endl + util::repr::rank2c(3) + ' '
                                + bcell + bcell + bcell + bcell + bcell + bcell + bcell + bcell
                                + util::constants::endl + util::repr::rank2c(2) + ' '
                                + bcell + bcell + bcell + bcell + bcell + bcell + bcell + bcell
                                + util::constants::endl + util::repr::rank2c(1) + ' '
                                + bcell + bcell + bcell + bcell + bcell + bcell + bcell + bcell
                                + util::constants::endl + util::repr::rank2c(0) + ' '
                                + bcell + bcell + bcell + bcell + bcell + bcell + bcell + bcell
                                + util::constants::endl;

        const std::string bbviz = std::string(1, util::constants::endl)
                                + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + util::constants::endl
                                + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + util::constants::endl
                                + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + util::constants::endl
                                + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + util::constants::endl
                                + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + util::constants::endl
                                + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + util::constants::endl
                                + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + util::constants::endl
                                + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + util::constants::endl;
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

        char ptype2c(ptype pt, color c){
            switch(pt){
                case pawn:      return c<0 ? 'p' : 'P';
                case knight:    return c<0 ? 'n' : 'N';
                case bishop:    return c<0 ? 'b' : 'B';
                case rook:      return c<0 ? 'r' : 'R';
                case queen:     return c<0 ? 'q' : 'Q';
                default:        return c<0 ? 'k' : 'K';
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

template U64 util::bitboard::attackfrom(const square&, ptype, color=white);
template U64 util::bitboard::attackfrom(const U64&, ptype, color=white);
template U64 util::bitboard::attackfrom(const coords&, ptype, color=white);

template bool util::bitboard::colinear(const square&, const square&);
template bool util::bitboard::colinear(const U64&, const U64&);
template bool util::bitboard::colinear(const coords&, const coords&);
template bool util::bitboard::colinear(const square&, const U64&);
template bool util::bitboard::colinear(const square&, const coords&);
template bool util::bitboard::colinear(const U64&, const coords&);
template bool util::bitboard::colinear(const U64&, const square&);
template bool util::bitboard::colinear(const coords&, const square&);
template bool util::bitboard::colinear(const coords&, const U64&);

template ptype util::bitboard::linetype(const square&, const square&);
template ptype util::bitboard::linetype(const U64&, const U64&);
template ptype util::bitboard::linetype(const coords&, const coords&);
template ptype util::bitboard::linetype(const square&, const U64&);
template ptype util::bitboard::linetype(const square&, const coords&);
template ptype util::bitboard::linetype(const U64&, const coords&);
template ptype util::bitboard::linetype(const U64&, const square&);
template ptype util::bitboard::linetype(const coords&, const square&);
template ptype util::bitboard::linetype(const coords&, const U64&);

template U64 util::bitboard::linebt(const square&, const square&, bool=false);
template U64 util::bitboard::linebt(const U64&, const U64&, bool=false);
template U64 util::bitboard::linebt(const coords&, const coords&, bool=false);
template U64 util::bitboard::linebt(const square&, const U64&, bool=false);
template U64 util::bitboard::linebt(const square&, const coords&, bool=false);
template U64 util::bitboard::linebt(const U64&, const coords&, bool=false);
template U64 util::bitboard::linebt(const U64&, const square&, bool=false);
template U64 util::bitboard::linebt(const coords&, const square&, bool=false);
template U64 util::bitboard::linebt(const coords&, const U64&, bool=false);