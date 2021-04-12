#ifndef CHESSDB_H
#define CHESSDB_H

#include <filesystem>

#include "tempus.hpp"
#include "logging.hpp"
#include "util.hpp"
#include "parsestream.hpp"
#include "encode.hpp"
#include "decode.hpp"



/******************************
    ChessDB
******************************/

template<typename CharT, typename Traits=std::char_traits<CharT> >
class ChessDB : virtual public ParseBuf<CharT, Traits>{

    static int cnt;
    
    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;

    Encoder enc;
    Decoder dec;

public:
    bool encode{false};

    uint32_t NTAGS{1},
             NGAMES{0},
             IDXPOS{util::constants::HDRSZ};

    std::unordered_map<string, uint32_t> tag_enumerations{{"", 0}};
    std::vector<string> tags{""};

    // Map game index to starting byte and num plies
    std::unordered_map<long, std::pair<long, uint16_t> > index;

    game rg;

    logging::Logger logger;

    ChessDB(const string&, bool, logging::LEVEL=logging::NONE, bool=false);
    ~ChessDB();

    void load();
    void create();
    void detach();
    void seek_game(int);
    void seek_index();
    void seek_tags();

    // Write funcs
    int_type wparse();
    int_type encode_game();
    void encode_tag(const string&);
    void encode_ply(const ply&);
    uint32_t enum_extend(const string&);
    void write_tag_enum();
    void write_index();
    void write_timestamp();
    void write_header();

    // Read funcs
    void read();
    int_type rparse();
    uint16_t read_nplies();
    void load_header();
    void load_index();
    void load_tag_enum();

    template<typename CharT_, typename Traits_>
    friend class ChessDBStream;
};

/******************************
    ChessDBStream
******************************/

template<typename CharT, typename Traits=std::char_traits<CharT> >
class ChessDBStream : public ParseStream<CharT, Traits>{

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;

    // Underlying buffer
    ChessDB<CharT, Traits> _pbuf;

public:
    
    ChessDBStream(const std::basic_string<CharT>&, logging::LEVEL=logging::NONE, bool=false);
    ChessDBStream<CharT, Traits>& operator<<(const game&);
    ChessDBStream<CharT, Traits>& operator>>(game&);
    void insert(const game&);
    game select(int);
    int size();
};


#endif