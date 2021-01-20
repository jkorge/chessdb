#ifndef PARSEBUF_H
#define PARSEBUF_H

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

    ParseBuf(const string& file) : std::basic_streambuf<CharT, Traits>(), _fdev(file, "rb") {}
    ~ParseBuf(){ this->_fdev.close(); }

protected:

    string _buf;
    FileBase<CharT, Traits> _fdev;

    virtual void read();
    virtual int_type parse();

    void set_get(int_type);
    int_type uflow();
    int_type underflow();
    int sync();


};

/******************************
    ParseBuf Member Funcs
******************************/

template<typename CharT, typename Traits>
void ParseBuf<CharT, Traits>::read(){ this->_fdev.xsgetn(this->_buf, BUFSIZE); }

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::parse(){ return this->_buf.size(); }

template<typename CharT, typename Traits>
void ParseBuf<CharT, Traits>::set_get(ParseBuf<CharT, Traits>::int_type sz){ this->setg(&this->_buf[0], &this->_buf[0], &this->_buf[0] + sz); }

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::uflow(){
    int_type res = this->underflow();
    if(res != Traits::eof()){ this->sync(); }
    return res;
}

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::underflow(){
    // Check for eof, read from file, update get ptrs
    int_type size;
    if(!this->_fdev.eof()){ this->read(); }
    this->set_get(this->_buf.size());
    return (this->gptr() == this->egptr()) ? Traits::eof() : Traits::to_int_type(*this->gptr());
}

template<typename CharT, typename Traits>
int ParseBuf<CharT, Traits>::sync(){
    // Parse and clear buffer
    if(!this->_buf.empty()){ this->parse(); }
    this->_buf.clear();
    this->set_get(0);
    return 0;
}


#endif