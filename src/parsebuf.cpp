#include "include/parsebuf.hpp"

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

// Does nothing - derived classes should override
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

// Does nothing - derived classes should override
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

template class ParseBuf<char>;
template class ParseBuf<wchar_t>;