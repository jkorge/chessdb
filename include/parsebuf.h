#ifndef PARSEBUF_H
#define PARSEBUF_H

#include <iostream>
#include "file_base.h"
#include "types.h"

#define BUFSIZE 2048

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

    ParseBuf(string file) : std::basic_streambuf<CharT, Traits>(), _fdev(file, "r") {}
    ~ParseBuf(){ this->_fdev.close(); }

protected:

    string _buf;
    FileBase<CharT, Traits> _fdev;

    virtual void read();
    void set_get(int_type);
    char_type next();
    char_type xnext();
    int_type uflow();
    int_type underflow();
    virtual int_type parse();
    int_type sync();


};

/******************************
    ParseBuf Member Funcs
******************************/

template<typename CharT, typename Traits>
void ParseBuf<CharT, Traits>::read(){ this->_fdev.xsgetn(this->_buf, BUFSIZE); }

template<typename CharT, typename Traits>
void ParseBuf<CharT, Traits>::set_get(ParseBuf<CharT, Traits>::int_type sz){ this->setg(&this->_buf[0], &this->_buf[0], &this->_buf[0] + sz); }

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::char_type
ParseBuf<CharT, Traits>::next(){ return Traits::to_char_type(this->sgetc()); }     // Does NOT advance get ptr

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::char_type
ParseBuf<CharT, Traits>::xnext(){ return Traits::to_char_type(this->sbumpc()); }

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::uflow(){
    int_type res = this->underflow();
    if(not Traits::eq_int_type(res, Traits::eof())){ this->sync(); }
    return res;
}

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::underflow(){
    int_type size;
    if(this->gptr() >= this->egptr()){
        // Read from file
        this->read();
        if(this->_fdev.eof){ return Traits::eof(); }
        this->set_get(this->_buf.size());
        
        // check for eof; parse data if not eof
        size = Traits::eq_int_type(this->sgetc(), Traits::eof()) ? 0 : this->parse();
        this->set_get(size);
    }

    return (this->gptr() == this->egptr()) ? Traits::eof() : Traits::to_int_type(*this->gptr());
}

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::parse(){ return this->_buf.size(); }

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::sync(){
    this->_buf.clear();
    this->set_get(0);
    return 0;
}


#endif