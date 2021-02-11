#ifndef PARSEBUF_H
#define PARSEBUF_H

#include <unordered_map>
#include "file_base.h"

#ifndef BUFSIZE
#define BUFSIZE 4096
#endif

/******************************
    ParseBuf
******************************/

// Streambuf subclass which reads from file and parses what it finds
template<typename CharT, typename Traits=std::char_traits<CharT> >
class ParseBuf : virtual public std::basic_streambuf<CharT, Traits> {

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;
    
public:

    ParseBuf(const string& file, const string& mode) : std::basic_streambuf<CharT, Traits>(), _fdev(file, mode) { this->_mode = mode.find('r') != string::npos ? 0 : 1; }

protected:

    string _read_buf, _write_buf;
    int_type _mode;
    FileBase<CharT, Traits> _fdev;

    virtual void close();

    virtual void read();
    virtual int_type read_parse();
    int read_sync();
    void set_get(int_type);
    int_type uflow();
    int_type underflow();

    virtual void write();
    virtual int_type write_parse();
    int write_sync();
    void set_put(int_type);
    int_type sputc(const char_type);
    std::streamsize xsputn(const char_type* s, std::streamsize count);
    int_type overflow(int_type);

};

/******************************
    ParseBuf Member Funcs
******************************/

template<typename CharT, typename Traits>
void ParseBuf<CharT, Traits>::close(){ this->_fdev.close(); }

/*
    READ FUNCS
*/

template<typename CharT, typename Traits>
void ParseBuf<CharT, Traits>::read(){ this->_fdev.xsgetn(this->_read_buf, BUFSIZE); }

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::read_parse(){ return this->_read_buf.size(); }

template<typename CharT, typename Traits>
void ParseBuf<CharT, Traits>::set_get(ParseBuf<CharT, Traits>::int_type sz){ this->setg(&this->_read_buf[0], &this->_read_buf[0], &this->_read_buf[0] + sz); }

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::uflow(){
    int_type res = this->underflow();
    if(res != Traits::eof()){ this->read_sync(); }
    return res;
}

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::underflow(){
    // Check for eof, read from file, update get ptrs
    if(!this->_fdev.eof()){ this->read(); }
    this->set_get(this->_read_buf.size());
    return (this->gptr() == this->egptr()) ? Traits::eof() : Traits::to_int_type(*this->gptr());
}

template<typename CharT, typename Traits>
int ParseBuf<CharT, Traits>::read_sync(){
    // Parse and clear buffer
    if(!this->_read_buf.empty()){ this->read_parse(); }
    this->_read_buf.clear();
    this->set_get(0);
    return 0;
}


/*
    WRITE FUNCS
*/

// ostream.write() -> streambuf.xsputn() -> streambuf.overflow()

template<typename CharT, typename Traits>
void ParseBuf<CharT, Traits>::write(){ this->_fdev.xsputn(this->_write_buf); }

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::write_parse(){ return this->_write_buf.size(); }

template<typename CharT, typename Traits>
void ParseBuf<CharT, Traits>::set_put(ParseBuf<CharT, Traits>::int_type sz){ this->setp(&this->_write_buf[0], &this->_write_buf[0] + sz); }

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::sputc(const ParseBuf<CharT, Traits>::char_type ch){ return this->overflow(ch); }

template<typename CharT, typename Traits>
std::streamsize ParseBuf<CharT,Traits>::xsputn(const ParseBuf<CharT, Traits>::char_type* s, std::streamsize count){
    this->_write_buf.append(s, count);
    std::streamsize res = this->_write_buf.size();
    this->write_sync();
    return res;
}

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::overflow(ParseBuf<CharT, Traits>::int_type ch){
    // Add to buffer, update put ptrs
    if(ch != Traits::eof()){ this->_write_buf.push_back(ch); }
    this->set_put(this->_write_buf.size());
    return (this->pptr() == this->epptr()) ? Traits::eof() : Traits::to_int_type(*this->pptr());
}

template<typename CharT, typename Traits>
int ParseBuf<CharT, Traits>::write_sync(){
    // Parse, write to file, and clear buffer
    if(!this->_write_buf.empty()){
        this->write_parse();
        this->write();
    }
    this->_write_buf.clear();
    this->set_put(0);
    return 0;
}

#endif