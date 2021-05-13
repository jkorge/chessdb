#ifndef ENCODE_H
#define ENCODE_H

#include <filesystem>

#include "tempus.hpp"
#include "parsestream.hpp"
#include "chess.hpp"

constexpr BYTE CAPTURE_   = 1<<5,
               CHECK_     = 1<<6,
               MATE_      = 1<<7,
               QABISHOP_  = 0,
               QAROOK_    = 1<<7;

constexpr std::size_t PLYSZ{sizeof(ply)},                     // ply struct
                      EPYSZ{sizeof(eply)},                    // eply typedef

                      NPYSZ{sizeof(uint16_t)},                // nplies record
                      TAGSZ{sizeof(uint32_t)},                // Tag enumerations
                      TMESZ{sizeof(uint64_t)},                // Time (nsec since epoch)
                      FBTSZ{sizeof(uint64_t)},                // Size of file position indicator (ie. byte number)

                      ATGSZ{16 * TAGSZ},                      // Tags in each game
                      HDRSZ{2*TMESZ + 2*TAGSZ},               // File header
                      IDXSZ{FBTSZ + NPYSZ};                   // Size of each entry in index

constexpr char tag_delims[]{"[]"},
               key_delim = ' ',
               value_delim = '\"';

// Size requirements for precomputed plies (PGN)
constexpr std::size_t psz = 924,
                      nsz = 4896,
                      bsz = 8640,
                      rsz = 11904,
                      qsz = 15264,
                      ksz = 2526;

// Delimiter for parsing moves in SAN
const std::regex move_num_r("(\\d{1,}\\.)");

// Regex for movetext that opens with an elided white ply (e.g. "1... Rd8")
const std::regex elided("\\d{1,}\\.{3}");

namespace encode{
    std::vector<eply> Game(const game&);

    eply Ply(const ply, Board&);

    BYTE piece(const ply, Board&);

    BYTE action(const ply);

    BYTE pawn_action(const coords, const coords, bool, ptype);

    BYTE knight_action(const coords, const coords);

    BYTE bishop_action(const coords, const coords);

    BYTE rook_action(const coords, const coords);

    BYTE queen_action(const coords, const coords);

    BYTE king_action(const coords, const coords);
}

namespace decode{
    std::vector<ply> Game(const std::vector<eply>&, const std::string="");

    ply Ply(eply, Board&);

    coords pawn_action(BYTE, color);

    coords knight_action(BYTE);

    coords bishop_action(BYTE);

    coords rook_action(BYTE);

    coords queen_action(BYTE, BYTE);

    coords king_action(BYTE);
}

/**************************************************
                      PGN
**************************************************/
template<typename CharT, typename Traits=std::char_traits<CharT> >
class PGN : virtual public ParseBuf<CharT, Traits>{

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;

    template<typename CharT_, typename Traits_>
    friend class PGNStream;

    const string _delim = {"\n\n"};
    std::vector<string> _tokens;

    Board board;

protected:
    std::vector<ply> _plies;
    pgndict _tags;
public:
    std::map<ptype, std::vector<ply> > ply_map;
    std::map<std::string, ply*> san_map;

private:
    int tend;

    // ParseBuf overrides
    void read();
    int_type rparse();

    void brp(U64, U64, ptype, bool, bool, bool);

    // Parsing functions
    void tags();
    void movetext();
    void pplies(color=white);

public:
    PGN(const string&);
    ~PGN();
};


/**************************************************
                      PGNSTREAM
**************************************************/

template<typename CharT, typename Traits=std::char_traits<CharT> >
class PGNStream : public ParseStream<CharT, Traits>{

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;

    // Underlying buffer
    PGN<CharT, Traits> _pbuf;

public:

    PGNStream(const std::basic_string<CharT>&);
    PGNStream(const CharT*);

    PGNStream<CharT, Traits>& operator>>(game&);
    PGNStream<CharT, Traits>& operator>>(std::vector<game>& games);

    game next();
    void close();
};

/**************************************************
                      CHESSDB
**************************************************/

template<typename CharT, typename Traits=std::char_traits<CharT> >
class ChessDB : virtual public ParseBuf<CharT, Traits>{
    
    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;

    template<typename CharT_, typename Traits_>
    friend class ChessDBStream;

public:
    std::unordered_map<string, uint32_t> tag_enumerations{{"", 0}};
    std::vector<string> tags{""};

    // Map game index to starting byte and num plies
    std::unordered_map<int32_t, std::pair<uint64_t, uint16_t> > index;

protected:
    game rg;
    uint32_t NTAGS{1},
             NGAMES{0};
    uint64_t IDXPOS{HDRSZ};

    bool encode{false};

public:
    ChessDB(const string&, bool);
    ~ChessDB();

protected:
    void load();
    void create();
    void detach();
    void seek_game(int);
    void seek_index();
    void seek_tags();

private:
    // Write funcs
    int_type wparse();
    int_type encode_game();
    uint32_t enum_extend(const string&);
    void write_tag_enum();
    void write_index();
    void write_timestamp();
    void write_header();

    // Read funcs
    void read();
    int_type rparse();
    void load_header();
    void load_index();
    void load_tag_enum();

};

/**************************************************
                  CHESSDBSTREAM
**************************************************/

template<typename CharT, typename Traits=std::char_traits<CharT> >
class ChessDBStream : public ParseStream<CharT, Traits>{

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;

    // Underlying buffer
    ChessDB<CharT, Traits> _pbuf;

public:
    
    ChessDBStream(const std::basic_string<CharT>&);

    ChessDBStream<CharT, Traits>& operator<<(const game&);
    ChessDBStream<CharT, Traits>& operator>>(game&);

    void insert(const game&);
    game select(int);
    int nplies(int i);

    int size();
};

#endif