#ifndef PARSESTREAM_H
#define PARSESTREAM_H

#include <iostream>
#include <filesystem>
#include "parsebuf.hpp"

/*
    IOStream subclass which uses a ParseBuf as its buffer
*/

namespace {
    std::string _get_mode(const std::string& file){ return std::filesystem::exists(file) ? "rb+" : "wb+"; }
    std::wstring _get_mode(const std::wstring& file){ return std::filesystem::exists(file) ? L"rb+" : L"wb+"; }

    template<typename CharT>
    std::basic_string<CharT> get_mode(const std::basic_string<CharT>& file){ return _get_mode(file); }
}

template<typename CharT, typename Traits=std::char_traits<CharT> >
class ParseStream : public std::basic_iostream<CharT, Traits>{

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;

    // Underlying buffer
    ParseBuf<CharT, Traits> _pbuf;

public:

    ParseStream() : std::basic_iostream<CharT, Traits>() {}
    ParseStream(const std::basic_string<CharT>& file) : std::basic_iostream<CharT, Traits>(&_pbuf), _pbuf(file, get_mode(file)) {}

    ParseStream<CharT, Traits>& read(char_type*, std::streamsize);
    ParseStream<CharT, Traits>& write(const char_type*, std::streamsize);

};

#endif