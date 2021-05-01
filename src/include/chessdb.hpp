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
                      TAGSZ{sizeof(uint32_t)},                // Tag enumeration value
                      NPYSZ{sizeof(uint16_t)},                // nplies record
                      ATGSZ{16 * TAGSZ},                      // Tags in each game
                      TMESZ{sizeof(uint64_t)},                // Time (nsec since epoch)
                      HDRSZ{TMESZ + 3*TAGSZ},                 // File header (Timestamp + number of games + number of enumerated tags + starting byte of index)
                      IDXSZ{sizeof(int32_t) + NPYSZ};         // Size of each entry in index

constexpr char tag_delims[]{"[]"},
               key_delim = ' ',
               value_delim = '\"';

// Delimiter for parsing moves in SAN
const std::regex move_num_r("(\\d{1,}\\.)");

// Regex for movetext that opens with an elided white ply (e.g. "1... Rd8")
const std::regex elided("\\d{1,}\\.{3}");

/**************************************************
                      ENCODER
**************************************************/

class Encoder {

    Board board;

    ply missing;                // default construct for missing or elided plies
    eply emissing{UINT16_MAX};  // use `11111111 11111111` for missing plies

public:

    std::vector<eply> encode_game(const game&);

    eply encode_ply(const ply, Board&);

    BYTE encode_piece(const ply, Board&);

    BYTE encode_action(const ply);

    BYTE encode_mat(ptype, color);

    BYTE encode_capture(bool);

    BYTE encode_check(bool);

    BYTE encode_mate(bool);

    BYTE encode_pawn_promotion(ptype, bool=true);

    BYTE encode_queen_axis(const coords, const coords);

    BYTE pawn_action(const coords, const coords, bool, ptype);

    BYTE knight_action(const coords, const coords);

    BYTE bishop_action(const coords, const coords);

    BYTE rook_action(const coords, const coords);

    BYTE queen_action(const coords, const coords);

    BYTE king_action(const coords, const coords);
};

/**************************************************
                      DECODER
**************************************************/

class Decoder{

    Board board;

    ply missing;                // default construct for missing or elided plies
    eply emissing{UINT16_MAX};  // use `11111111 11111111` for missing plies

public:
    std::vector<ply> decode_game(const std::vector<eply>&, const std::string="");

    ply decode_ply(eply, Board&);

    ptype decode_type(BYTE);

    color decode_color(BYTE);

    bool decode_capture(BYTE);

    bool decode_check(BYTE);

    bool decode_mate(BYTE);

    ptype decode_pawn_promotion(BYTE);

    ptype decode_queen_axis(BYTE);

    coords pawn_action(BYTE, color);

    coords knight_action(BYTE);

    coords bishop_action(BYTE);

    coords rook_action(BYTE);

    coords queen_action(BYTE, ptype);

    coords king_action(BYTE);
};

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

private:
    int tend;

    // ParseBuf overrides
    void read();
    int_type rparse();

    // Parsing functions
    void tags();
    void movetext();
    void pplies(color=white);
    ply pply(string, color);
    ply pcastle(bool, color, bool, bool);
    ply prest(const string, color, bool, bool);

public:
    PGN(const string&);
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

    Encoder enc;
    Decoder dec;
public:
    std::unordered_map<string, uint32_t> tag_enumerations{{"", 0}};
    std::vector<string> tags{""};

    // Map game index to starting byte and num plies
    std::unordered_map<int32_t, std::pair<int32_t, uint16_t> > index;

protected:
    game rg;
    uint32_t NTAGS{1},
             NGAMES{0},
             IDXPOS{HDRSZ};

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

    int size();
};

#endif