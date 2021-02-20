#ifndef CHESSDB_H
#define CHESSDB_H

#include <filesystem>
#include "tempus.h"
#include "parsebuf.h"
#include "types.h"
#include "util.h"
#include "encode.h"
#include "decode.h"

std::size_t PLYSZ{sizeof(ply)},                     // ply struct
            EPYSZ{sizeof(eply)},                    // eply typedef
            TAGSZ{sizeof(uint32_t)},                // Tag enumeration value
            NPYSZ{sizeof(uint16_t)},                // nplies record
            ATGSZ{16 * TAGSZ},                      // Tags in each game
            TMESZ{sizeof(unsigned long long)},      // Time (nsec since epoch)
            HDRSZ{TMESZ + 2*TAGSZ};                 // File header (Timestamp + number of games + number of enumerated tags)



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
        this->_fdev.seek(TMESZ);
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
    ChessDB Member Funcs
******************************/

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::load(){
    this->_fdev.seek(0);

    // Load meta data
    this->load_header();
    this->load_index();
    this->load_tag_enum();

    // Set stream pos to end of game data
    // this->seek_end();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::create(){
    this->encode = false;
    this->write_timestamp();

    // Placeholder 0 for num games and num tags
    uint32_t nil = 0;
    this->sputn((char_type*)&nil, TAGSZ);
    this->sputn((char_type*)&nil, TAGSZ);
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::seek(int i){ this->_fdev.seek(this->index[i].first); }

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::seek_end(){
    if(this->index.size()){
        this->_fdev.seek(
            this->index[this->NGAMES-1].first +                 // Start byte of last game
            ATGSZ + NPYSZ +                                     // Tag-width + nplies-width
            (this->index[this->NGAMES-1].second * EPYSZ)        // nplies * eply-width
        );
    }
    else{ this->_fdev.seek(HDRSZ); }
}

/*
    WRITE FUNCS
*/

template<typename CharT, typename Traits>
typename ChessDB<CharT, Traits>::int_type
ChessDB<CharT, Traits>::wparse(){
    if(this->encode){ return this->encode_game(); }
    else            { return 0;                   } // Unformatted output (e.g. timestamps, integer sizes)
}

template<typename CharT, typename Traits>
typename ChessDB<CharT, Traits>::int_type
ChessDB<CharT, Traits>::encode_game(){

    const game g = *reinterpret_cast<game*>(this->_buf.data());
    this->sync();

    // Tags
    for(pgndict::const_iterator tagit=g.tags.cbegin(); tagit!=g.tags.cend(); ++tagit){
        this->encode_tag(tagit->second);
    }

    // Num plies
    uint16_t nplies = g.plies.size();
    this->_buf.append((char_type*)&nplies, sizeof(nplies));

    // Plies
    for(std::vector<ply>::const_iterator movit=g.plies.cbegin(); movit!=g.plies.cend(); ++movit){
        this->encode_ply(*movit);
    }

    // Increment game count and update index
    this->index.insert({++this->NGAMES - 1, {this->_fdev.tell(), nplies}});

    return this->_buf.size();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::encode_tag(const ChessDB<CharT, Traits>::string& tag){
    // Add tag to enumeration, append to buffer
    uint32_t val = this->enum_extend(tag);
    this->_buf.append((char_type*)&val, TAGSZ);
}

template<typename CharT, typename Traits>
uint32_t ChessDB<CharT, Traits>::enum_extend(const ChessDB<CharT, Traits>::string& tag){
    if(this->tag_enumerations.find(tag) == this->tag_enumerations.end()){
        this->tag_enumerations.emplace(tag, ++this->NTAGS - 1);
        this->tags.emplace_back(tag);
    }
    return this->tag_enumerations[tag];
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::encode_ply(const ply& p){
    // Encode ply, append to buffer
    eply ep = this->enc.encode_ply(p);
    this->_buf.append((char_type*)&ep, EPYSZ);
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::write_tag_enum(){
    this->seek_end();
    this->sync();

    this->_buf = this->tags[0] + util::endl;
    for(unsigned int i=1; i<this->NTAGS; ++i){
        string t = this->tags[i] + util::endl;
        this->_buf.append(t.data(), t.size());
    }

    this->write();
    this->sync();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::write_timestamp(){
    this->encode = false;
    this->_fdev.seek(0);
    unsigned long long tme = Tempus::time();
    this->sputn((char_type*)&tme, TMESZ);
}

/*
    READ FUNCS
*/

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::read(){
    // Tags
    this->_fdev.xsgetn(this->_buf, ATGSZ);
    // Plies
    this->_fdev.xsgetn(this->_buf, this->read_nplies() * EPYSZ);
}

template<typename CharT, typename Traits>
typename ChessDB<CharT, Traits>::int_type
ChessDB<CharT, Traits>::rparse(){
    this->rg.tags.reset();
    this->rg.plies.clear();
    int loc = 0,
        N = this->_buf.size();

    // std::cout << "Tags" << std::endl;
    for(pgndict::iterator it=this->rg.tags.begin(); it!=this->rg.tags.end(); ++it){
        it->second = this->tags[*(uint32_t*)(this->_buf.data() + loc)];
        loc += TAGSZ;
    }

    // std::cout << "Plies" << std::endl;
    std::vector<eply> eplies;
    while(loc < N){
        eplies.emplace_back(*(eply*)(this->_buf.data() + loc));
        loc += EPYSZ;
    }

    // std::cout << "Decode" << std::endl;
    this->rg.plies = this->dec.decode_game(eplies, this->rg.tags[fenstr]);

    // std::cout << "Done" << std::endl;
    return this->rg.plies.size();
}

template<typename CharT, typename Traits>
uint16_t ChessDB<CharT, Traits>::read_nplies(){
    std::string tmp;
    this->_fdev.xsgetn(tmp, NPYSZ);
    return *(uint16_t*)tmp.data();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::load_header(){
    this->_fdev.xsgetn(this->_buf, HDRSZ);

    // Ignore timestamp
    // std::cout << Tempus::strtime(*(unsigned long long*)this->_buf.data()) << std::endl;

    // Extract num games
    this->NGAMES = *(uint32_t*)(this->_buf.data() + TMESZ);

    // Extract num tags
    this->NTAGS = *(uint32_t*)(this->_buf.data() + TMESZ + TAGSZ);

    // Overwrite timestamp
    this->sync();
    this->write_timestamp();
    
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::load_tag_enum(){
    this->seek_end();

    // Skip null string at start of enum
    this->_fdev.xsgetn(this->_buf, 1024, util::endl);
    for(unsigned int i=1; i<this->NTAGS; ++i){
        this->sync();
        this->_fdev.xsgetn(this->_buf, 1024, util::endl);
        this->tag_enumerations[this->_buf] = i;
        this->tags.emplace_back(this->_buf);
    }

    if(this->BAKTAG.empty()){ this->BAKTAG = this->tags; }
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::load_index(){
    this->_fdev.seek(HDRSZ);

    for(unsigned int i=0; i<this->NGAMES; ++i){

        long pos = this->_fdev.tell();
        this->_fdev.seek(ATGSZ, SEEK_CUR);
        uint16_t nplies = this->read_nplies();

        this->index.insert({i, {pos, nplies}});
        this->_fdev.seek(nplies * EPYSZ, SEEK_CUR);
    }
}

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

/******************************
    ChessDB Member Funcs
******************************/

template<typename CharT, typename Traits>
ChessDBStream<CharT, Traits>& ChessDBStream<CharT, Traits>::operator<<(const game& g){
    this->_pbuf.encode = true;
    this->write((char_type*)&g, sizeof(g));
    return *this;
}

template<typename CharT, typename Traits>
ChessDBStream<CharT, Traits>& ChessDBStream<CharT, Traits>::operator>>(game& g){
    this->read(nullptr, 0);
    g = this->_pbuf.rg;
    return *this;
}

template<typename CharT, typename Traits>
void ChessDBStream<CharT, Traits>::insert(const game& g){
    this->_pbuf.seek_end();
    *this << g;
}

template<typename CharT, typename Traits>
game ChessDBStream<CharT, Traits>::select(int i){
    this->_pbuf.seek(i);
    game res;
    *this >> res;
    return res;
}

template<typename CharT, typename Traits>
int ChessDBStream<CharT, Traits>::size(){ return this->_pbuf.NGAMES; }


#endif