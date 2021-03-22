#ifndef FILEBASE_H
#define FILEBASE_H

#include <cstdio>
#include <string>

/*
    Wrapper for C file IO
*/

template<typename CharT, typename Traits=std::char_traits<CharT> >
class FileBase{

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;

public:

    string _fname, _mode;
    std::FILE* _M_file {NULL};

    FileBase() {}
    FileBase(const string& fname, const string& mode) : _fname(fname), _mode(mode){ this->open(); }
    ~FileBase(){ this->close(); }

    void open();
    void close();
    bool eof();
    int seek(long, int=SEEK_SET);
    long tell();

    void xsgetn(string&, std::streamsize);
    void xsgetn(string&, std::streamsize, char_type);
    void xsgetn(string&, std::streamsize, string&);

    void xsputn(string&);
    void xsputn(string&, std::streamsize);
    void xsputn(string&, std::streamsize, char_type);
    void xsputn(string&, std::streamsize, string&);
};

#endif