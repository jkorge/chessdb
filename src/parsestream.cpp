#include "include/parsestream.hpp"

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

template class ParseStream<char>;
template class ParseStream<wchar_t>;