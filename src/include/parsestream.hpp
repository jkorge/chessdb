#ifndef PARSESTREAM_H
#define PARSESTREAM_H

#include <cstdio>

#include <algorithm>
#include <filesystem>
#include <string>
#include <streambuf>

constexpr std::streamsize BUFSIZE = 4096;

/*
    Wrapper for C file IO
*/

template<typename CharT, typename Traits=std::char_traits<CharT> >
class FileBase{

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<CharT> string;

public:

    string _fname, _mode;
    std::FILE* _M_file {NULL};

    FileBase();
    FileBase(const string&, const string&);
    FileBase(std::FILE*&&);
    ~FileBase();

    void open();
    void close();
    bool eof();
    int seek(uint64_t, int=SEEK_SET);
    uint64_t tell();

    void xsgetn(string&, std::streamsize);
    void xsgetn(string&, std::streamsize, const char_type);
    void xsgetn(string&, std::streamsize, const string);

    void xsputn(string&);
    void xsputn(string&, std::streamsize);
    void xsputn(string&, std::streamsize, const char_type);
    void xsputn(string&, std::streamsize, const string);
};

/*
    Streambuf subclass which reads from file and parses what it finds
*/

template<typename CharT, typename Traits=std::char_traits<CharT> >
class ParseBuf : virtual public std::basic_streambuf<CharT, Traits> {

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<CharT> string;

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
    ParseBuf();
    ParseBuf(const string&, const string&);
    ParseBuf(std::FILE*&&);
    ~ParseBuf();

};

/*
    IOStream subclass which uses a ParseBuf as its buffer
*/

template<typename CharT, typename Traits=std::char_traits<CharT> >
class ParseStream : public std::basic_iostream<CharT, Traits>{

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<CharT> string;

    // Underlying buffer
    ParseBuf<CharT, Traits> _pbuf;

public:

    ParseStream();
    ParseStream(const string&);
    ParseStream(std::FILE*&&);
    ~ParseStream();

    ParseStream<CharT, Traits>& read(char_type* = nullptr, std::streamsize=0);
    ParseStream<CharT, Traits>& write(const char_type*, std::streamsize);

};

#endif