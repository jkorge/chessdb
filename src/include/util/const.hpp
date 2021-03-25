#ifndef UTIL_CONST_H
#define UTIL_CONST_H

/**************************
    CONSTANTS
**************************/

namespace util{
    namespace constants{

        constexpr U64 DEBRUIJN64_   = 0x03f79d71b4cb0a89;
        constexpr U64 MAINDIAG_     = 0x8040201008040201;
        constexpr U64 ANTIDIAG_     = 0x0102040810204080;
        constexpr U64 ALL_          = 0xffffffffffffffff;
        constexpr U64 RANK_         = 0xff;
        constexpr U64 FILE_         = 0x0101010101010101;
        constexpr U64 ONE_          = 0x01;
        constexpr U64 ZERO_         = 0x00;
        constexpr U64 A1            = 0x01;
        constexpr U64 H8            = 0x8000000000000000;

        constexpr BYTE CAPTURE_   = 1<<5;
        constexpr BYTE CHECK_     = 1<<6;
        constexpr BYTE MATE_      = 1<<7;
        constexpr BYTE QABISHOP_  = 0;
        constexpr BYTE QAROOK_    = 1<<5;

        constexpr int i64[64] = {
             0, 47,  1, 56, 48, 27,  2, 60,
            57, 49, 41, 37, 28, 16,  3, 61,
            54, 58, 35, 52, 50, 42, 21, 44,
            38, 32, 29, 23, 17, 11,  4, 62,
            46, 55, 26, 59, 40, 36, 15, 53,
            34, 51, 20, 43, 31, 22, 10, 45,
            25, 39, 14, 33, 19, 30,  9, 24,
            13, 18,  8, 12,  7,  6,  5, 63
        };

        // Whitespace regex
        const std::regex wsr("\\s");

        // Whitespace string
        const std::string ws(1, ' ');

        // Newline string
        constexpr char endl = '\n';

        // Starting positions for a standard game of chess
        constexpr U64 white_pawns = 0x000000000000ff00,
                      white_knights = 0x0000000000000042,
                      white_bishops = 0x0000000000000024,
                      white_rooks = 0x0000000000000081,
                      white_queen = 0x0000000000000008,
                      white_king = 0x0000000000000010;

        constexpr U64 black_pawns = white_pawns << 40,
                      black_knights = white_knights << 56,
                      black_bishops =  white_bishops << 56,
                      black_rooks =  white_rooks << 56,
                      black_queen =  white_queen << 56,
                      black_king =  white_king << 56;

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

        // Piece and file identifiers
        const std::string pieces = "KQRBN";
        const std::string files = "abcdefgh";
        const std::string ranks = "12345678";

        // Iterators to end of regex_token_iterator results
        static rtok rtokend;
        static crtok crtokend;

        constexpr std::size_t PLYSZ{sizeof(ply)},                     // ply struct
                              EPYSZ{sizeof(eply)},                    // eply typedef
                              TAGSZ{sizeof(uint32_t)},                // Tag enumeration value
                              NPYSZ{sizeof(uint16_t)},                // nplies record
                              ATGSZ{16 * TAGSZ},                      // Tags in each game
                              TMESZ{sizeof(unsigned long long)},      // Time (nsec since epoch)
                              HDRSZ{TMESZ + 2*TAGSZ};                 // File header (Timestamp + number of games + number of enumerated tags)
    }
}

#endif