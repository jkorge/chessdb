#ifndef PARSEBUF_H
#define PARSEBUF_H

#include <streambuf>
#include "filebase.hpp"

constexpr std::streamsize BUFSIZE = 4096;

/*
    Streambuf subclass which reads from file and parses what it finds
*/

template<typename CharT, typename Traits=std::char_traits<CharT> >
class ParseBuf : virtual public std::basic_streambuf<CharT, Traits> {

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;

    template<typename _CharT, typename _Traits>
    friend class ParseStream;

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
    
public:
    ParseBuf() : std::basic_streambuf<CharT, Traits>() {}
    ParseBuf(const string& file, const string& mode) : std::basic_streambuf<CharT, Traits>(), _fdev(file, mode) {}
    ~ParseBuf() { this->close(); }

};

#endif
