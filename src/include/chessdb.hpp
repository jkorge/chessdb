#ifndef CHESSDB_H
#define CHESSDB_H

#include <filesystem>

#include "tempus.hpp"
#include "util.hpp"
#include "parsestream.hpp"
#include "encode.hpp"
#include "decode.hpp"



/******************************
    ChessDB
******************************/

template<typename CharT, typename Traits=std::char_traits<CharT> >
class ChessDB : virtual public ParseBuf<CharT, Traits>{
    
    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;

    Encoder enc;
    Decoder dec;

public:
    bool encode{false};

    uint32_t NTAGS{1},
             NGAMES{0};

    std::unordered_map<string, uint32_t> tag_enumerations{{"", 0}};
    std::vector<string> tags{""},
                        BAKTAG;

    // Map game index to starting byte and num plies
    std::unordered_map<long, std::pair<long, uint16_t> > index;

    game rg;

    ChessDB(const string& file, bool exists) : ParseBuf<CharT,Traits>(file, exists ? "rb+" : "wb+") {
        if(exists){ this->load(); }
        else      { this->create(); }
    }
    ~ChessDB() {
        this->encode = false;

        // Tag enumeration
        this->write_tag_enum();

        // Update placeholders for num games and num tags in header
        this->_fdev.seek(util::constants::TMESZ);
        uint32_t ttags = this->tags.size();
        this->sputn((char_type*)&this->NGAMES, sizeof(this->NGAMES));
        this->sputn((char_type*)&ttags, sizeof(ttags));

        // Close file
        this->close();
    }

    void load();
    void create();
    void seek(int);
    void seek_end();

    // Write funcs
    int_type wparse();
    int_type encode_game();
    void encode_tag(const string&);
    void encode_ply(const ply&);
    uint32_t enum_extend(const string&);
    void write_tag_enum();
    void write_timestamp();

    // Read funcs
    void read();
    int_type rparse();
    uint16_t read_nplies();
    void load_header();
    void load_index();
    void load_tag_enum();
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
    ChessDBStream(const std::basic_string<CharT>& file) : ParseStream<CharT, Traits>(), _pbuf(file, std::filesystem::exists(file)) { this->rdbuf(&this->_pbuf); }
    ChessDBStream<CharT, Traits>& operator<<(const game&);
    ChessDBStream<CharT, Traits>& operator>>(game&);
    void insert(const game&);
    game select(int);
    int size();
};


#endif