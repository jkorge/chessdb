#ifndef PARSEBUF_H
#define PARSEBUF_H

#include <iostream>
#include <unordered_map>
#include "file_base.h"

constexpr int BUFSIZE = 4096;

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

    ParseBuf() : std::basic_streambuf<CharT, Traits>() {}
    ParseBuf(const string& file, const string& mode) : std::basic_streambuf<CharT, Traits>(), _fdev(file, mode) {}
    virtual ~ParseBuf() { this->close(); }

protected:

    string _buf;
    FileBase<CharT, Traits> _fdev;

    void close();
    int sync();

    virtual void read();
    virtual int_type rparse();
    std::streamsize xsgetn(char_type*, std::streamsize);
    int_type uflow();
    int_type underflow();

    virtual void write();
    virtual int_type wparse();
    std::streamsize xsputn(const char_type* s, std::streamsize count);
    int_type overflow(int_type=Traits::eof());

    template<typename _CharT, typename _Traits>
    friend class ParseStream;

};

/******************************
    ParseBuf Member Funcs
******************************/

template<typename CharT, typename Traits>
void ParseBuf<CharT, Traits>::close(){ this->_fdev.close(); }

template<typename CharT, typename Traits>
int ParseBuf<CharT, Traits>::sync(){
    this->_buf.clear();
    return 0;
}

/*
    READ FUNCS
*/

template<typename CharT, typename Traits>
void ParseBuf<CharT, Traits>::read(){ this->_fdev.xsgetn(this->_buf, BUFSIZE); }

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::rparse(){ return this->_buf.size(); }

template<typename CharT, typename Traits>
std::streamsize ParseBuf<CharT, Traits>::xsgetn(ParseBuf<CharT, Traits>::char_type* s, std::streamsize n){
    if(this->uflow() == Traits::eof()){ return 0; }
    else                              { return this->_buf.size(); }
}

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::uflow(){ return this->underflow(); }

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::underflow(){
    if(!this->_fdev.eof()){
        this->sync();
        this->read();
        this->rparse();
        this->setg(&this->_buf[0], &this->_buf[0], &this->_buf[0]);
        return 0;
    }
    else{ return Traits::eof(); }
}


/*
    WRITE FUNCS
*/

template<typename CharT, typename Traits>
void ParseBuf<CharT, Traits>::write(){ this->_fdev.xsputn(this->_buf); }

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::wparse(){ return this->_buf.size(); }

template<typename CharT, typename Traits>
std::streamsize ParseBuf<CharT,Traits>::xsputn(const ParseBuf<CharT, Traits>::char_type* s, std::streamsize count){
    this->_buf.append(s, count);
    return this->overflow();
}

template<typename CharT, typename Traits>
typename ParseBuf<CharT, Traits>::int_type
ParseBuf<CharT, Traits>::overflow(ParseBuf<CharT, Traits>::int_type ch){
    if(ch != Traits::eof()){ this->_buf.push_back(ch); }
    else{
        this->wparse();
        this->write();
        this->sync();
        this->setp(&this->_buf[0], &this->_buf[0]);
    }
    return this->_buf.size();
}


/******************************
    ParseStream
******************************/

template<typename CharT, typename Traits=std::char_traits<CharT> >
class ParseStream : public std::basic_iostream<CharT, Traits>{

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;

    // Underlying buffer
    ParseBuf<CharT, Traits> _pbuf;

public:

    ParseStream() : std::basic_iostream<CharT, Traits>() {}
    ParseStream(const std::basic_string<CharT>& file) : std::basic_iostream<CharT, Traits>(&_pbuf), _pbuf(file) {}

    ParseStream<CharT, Traits>& read(char_type*, std::streamsize);
    ParseStream<CharT, Traits>& write(const char_type*, std::streamsize);

};

/******************************
    ParseStream Member Funcs
******************************/

template<typename CharT, typename Traits>
ParseStream<CharT, Traits>& ParseStream<CharT, Traits>::read(ParseStream<CharT, Traits>::char_type* s, std::streamsize n){
    typename std::basic_istream<CharT, Traits>::sentry sty(*this, true);
    if(sty){
        std::streamsize count = this->rdbuf()->sgetn(s, n);
        if(!count){ this->setstate(std::ios_base::eofbit); }
    }
    return *this;
}

template<typename CharT, typename Traits>
ParseStream<CharT, Traits>& ParseStream<CharT, Traits>::write(const ParseStream<CharT, Traits>::char_type* s, std::streamsize n){
    typename std::basic_ostream<CharT, Traits>::sentry sty(*this);
    if(sty){ this->rdbuf()->sputn(s, n); }
    return *this;
}


#endif