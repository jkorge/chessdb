#include "include/chessdb.hpp"

template<typename CharT, typename Traits>
int ChessDB<CharT, Traits>::cnt = 0;

/******************************
    ChessDB Member Funcs
******************************/
template<typename CharT, typename Traits>
ChessDB<CharT, Traits>::ChessDB(const string& file, bool exists, logging::LEVEL lvl, bool flog)
    : ParseBuf<CharT,Traits>(file, exists ? "rb+" : "wb+"),
      logger(ChessDB::cnt, lvl, flog)
{
    if(exists){ this->load(); }
    else      { this->create(); }
    ++this->cnt;
}

template<typename CharT, typename Traits>
ChessDB<CharT, Traits>::~ChessDB(){
    // Dump remaining stats
    this->detach();
    // Close file
    this->logger.debug("Closing");
    this->close();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::load(){
    this->logger.debug("Loading DB");
    this->load_header();
    this->load_index();
    this->load_tag_enum();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::create(){
    this->logger.debug("Initializing DB");
    this->write_header();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::detach(){
    this->logger.debug("Disconnecting from DB");
    this->encode = false;
    this->write_header();
    /*
        WRITE INDEX BEFORE TAG ENUM
    */
    this->write_index();
    this->write_tag_enum();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::seek_game(int i){ this->_fdev.seek(this->index[i].first); }

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::seek_index(){ this->_fdev.seek(this->IDXPOS); }

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::seek_tags(){
    this->seek_index();
    this->_fdev.seek(this->NGAMES * util::constants::IDXSZ, SEEK_CUR);
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

    // Update index pos
    this->IDXPOS += util::constants::ATGSZ + util::constants::NPYSZ + nplies*util::constants::EPYSZ;

    return this->_buf.size();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::encode_tag(const ChessDB<CharT, Traits>::string& tag){
    // Add tag to enumeration, append to buffer
    uint32_t val = this->enum_extend(tag);
    this->_buf.append((char_type*)&val, util::constants::TAGSZ);
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
    this->_buf.append((char_type*)&ep, util::constants::EPYSZ);
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::write_tag_enum(){
    this->logger.debug("Saving Tags");
    this->seek_tags();
    this->sync();

    std::stringstream bufstr;
    std::copy(this->tags.begin(), this->tags.end(), std::ostream_iterator<std::string>(bufstr, util::constants::endl.data()));
    this->_buf = std::move(*bufstr.rdbuf()).str();

    this->write();
    this->sync();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::write_index(){
    this->logger.debug("Saving Index");
    this->seek_index();
    this->sync();

    for(int i=0; i<this->index.size(); ++i){
        this->_buf.append((char_type*)&this->index[i].first, sizeof(long));
        this->_buf.append((char_type*)&this->index[i].second, util::constants::NPYSZ);
    }

    this->write();
    this->sync();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::write_header(){
    this->logger.debug("Updating Header");
    this->_fdev.seek(0);
    this->sync();

    // Time, NGAMES, NTAGS, IDXPOS
    unsigned long long tme = Tempus::time();
    this->_buf.append((char_type*)&tme, util::constants::TMESZ);
    this->_buf.append((char_type*)&this->NGAMES, util::constants::TAGSZ);
    this->_buf.append((char_type*)&this->NTAGS, util::constants::TAGSZ);
    this->_buf.append((char_type*)&this->IDXPOS, util::constants::TAGSZ);

    this->logger.debug("NGAMES:", this->NGAMES);
    this->logger.debug("NTAGS:", this->NTAGS);
    this->logger.debug("IDXPOS:", this->IDXPOS);

    this->write();
    this->sync();
}

/*
    READ FUNCS
*/

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::read(){
    // Tags
    this->_fdev.xsgetn(this->_buf, util::constants::ATGSZ);
    // Plies
    uint16_t npl = this->read_nplies();
    this->_fdev.xsgetn(this->_buf, npl * util::constants::EPYSZ);
}

template<typename CharT, typename Traits>
typename ChessDB<CharT, Traits>::int_type
ChessDB<CharT, Traits>::rparse(){
    this->rg.tags.reset();
    this->rg.plies.clear();
    int loc = 0,
        N = this->_buf.size();

    for(pgndict::iterator it=this->rg.tags.begin(); it!=this->rg.tags.end(); ++it){
        it->second = this->tags[*(uint32_t*)(this->_buf.data() + loc)];
        loc += util::constants::TAGSZ;
    }

    std::vector<eply> eplies;
    while(loc < N){
        eplies.emplace_back(*(eply*)(this->_buf.data() + loc));
        loc += util::constants::EPYSZ;
    }

    this->rg.plies = this->dec.decode_game(eplies, this->rg.tags[fenstr]);

    return this->rg.plies.size();
}

template<typename CharT, typename Traits>
uint16_t ChessDB<CharT, Traits>::read_nplies(){
    std::string tmp;
    this->_fdev.xsgetn(tmp, util::constants::NPYSZ);
    return *(uint16_t*)tmp.data();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::load_header(){
    this->logger.debug("Reading Header");
    this->_fdev.seek(0);
    this->sync();

    this->_fdev.xsgetn(this->_buf, util::constants::HDRSZ);

    // Ignore timestamp
    // unsigned long long last_read_t = *(unsigned long long*)this->_buf.data();

    // Extract num games
    this->NGAMES = *(uint32_t*)(this->_buf.data() + util::constants::TMESZ);

    // Extract num tags
    this->NTAGS = *(uint32_t*)(this->_buf.data() + util::constants::TMESZ + util::constants::TAGSZ);

    // Extract index pos
    this->IDXPOS = *(uint32_t*)(this->_buf.data() + util::constants::TMESZ + 2*util::constants::TAGSZ);

    this->logger.debug("NGAMES:", this->NGAMES);
    this->logger.debug("NTAGS:", this->NTAGS);
    this->logger.debug("IDXPOS:", this->IDXPOS);

    this->sync();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::load_tag_enum(){
    this->logger.debug("Reading Tags");
    this->seek_tags();
    this->sync();

    // Read tags (from current position to EOF)
    long loc = this->_fdev.tell();
    this->_fdev.seek(0, SEEK_END);
    long end = this->_fdev.tell();
    this->_fdev.seek(loc, SEEK_SET);
    this->_fdev.xsgetn(this->_buf, end-loc);

    // Move buffer to stringstream and split on \n
    std::stringstream bufstr;
    bufstr.str(std::move(this->_buf));
    this->tags.resize(this->NTAGS);
    for(int i=0; i<this->NTAGS; ++i){
        std::getline(bufstr, this->tags[i], util::constants::nln);
        this->tag_enumerations[this->tags[i]] = i;
    }

    this->sync();
}

template<typename CharT, typename Traits>
void ChessDB<CharT, Traits>::load_index(){
    this->logger.debug("Reading Index");
    this->seek_index();
    this->sync();

    std::size_t sz = this->NGAMES * util::constants::IDXSZ;
    this->_fdev.xsgetn(this->_buf, sz);

    this->index.reserve(this->NGAMES);
    for(unsigned int i=0; i<this->NGAMES; ++i){
        int loc = i*util::constants::IDXSZ;
        long pos = *(long*)(this->_buf.data() + loc);
        uint16_t npl = *(uint16_t*)(this->_buf.data() + loc + sizeof(long));
        index.emplace(i, std::make_pair(pos, npl));
    }
    this->sync();
}

/******************************
    ChessDB Member Funcs
******************************/

template<typename CharT, typename Traits>
ChessDBStream<CharT, Traits>::ChessDBStream(const std::basic_string<CharT>& file, logging::LEVEL lvl, bool flog)
    : ParseStream<CharT, Traits>(),
      _pbuf(file, std::filesystem::exists(file), lvl, flog) { this->rdbuf(&this->_pbuf); }

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
    this->_pbuf.seek_index();
    *this << g;
}

template<typename CharT, typename Traits>
game ChessDBStream<CharT, Traits>::select(int i){
    this->_pbuf.seek_game(i);
    game res;
    *this >> res;
    return res;
}

template<typename CharT, typename Traits>
int ChessDBStream<CharT, Traits>::size(){ return this->_pbuf.NGAMES; }


template class ChessDB<char>;
template class ChessDBStream<char>;