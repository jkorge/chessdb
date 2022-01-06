#include "include/parsestream.hpp"

/**************************************************
                      FILEBASE
**************************************************/

template<typename CharT, typename Traits>
FileBase<CharT, Traits>::FileBase() {}

template<typename CharT, typename Traits>
FileBase<CharT, Traits>::FileBase(const string& fname, const string& mode) : _fname(fname), _mode(mode){ this->open(); }

template<typename CharT, typename Traits>
FileBase<CharT, Traits>::FileBase(std::FILE*&& _fp) : _M_file(_fp) {}

template<typename CharT, typename Traits>
FileBase<CharT, Traits>::~FileBase() = default;

template<>
void FileBase<char>::open(){
    if(this->_M_file == 0){
        this->_M_file = fopen64(this->_fname.data(), this->_mode.data());
        std::setvbuf(this->_M_file, NULL, _IONBF, 0);
    }
}

template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::close(){ if(this->_M_file){ std::fclose(this->_M_file); } }

template<typename CharT, typename Traits>
bool FileBase<CharT, Traits>::eof(){ return std::feof(this->_M_file); }

template<typename CharT, typename Traits>
int FileBase<CharT, Traits>::seek(uint64_t offset, int origin){ return fseeko64(this->_M_file, offset, origin); }

template<typename CharT, typename Traits>
uint64_t FileBase<CharT, Traits>::tell(){ return ftello64(this->_M_file); }

/*
    READ FUNCS
*/

// Reads up to n bytes or EOF (whichever comes first)
template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::xsgetn(
    typename FileBase<CharT, Traits>::string& buf,
    std::streamsize n
){
    char_type* tmp = new char_type[n];
    n = std::fread(tmp, sizeof(char_type), n, this->_M_file);
    buf.append(tmp, n);
    delete[] tmp;
}

// Reads up to n bytes, EOF, or delim (whichever comes first)
template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::xsgetn(
    typename FileBase<CharT, Traits>::string& buf,
    std::streamsize n,
    const typename FileBase<CharT, Traits>::char_type delim
){ this->xsgetn(buf, n, string{delim}); }

// Reads up to n bytes, EOF, or delim (whichever comes first)
// delim is a multi-char string
template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::xsgetn(
    typename FileBase<CharT, Traits>::string& buf,
    std::streamsize n,
    const typename FileBase<CharT, Traits>::string delim
){
    /*
        Local vars
    */
    std::streamsize d = delim.size();
    uint64_t pos = this->tell();

    /*
        Read
    */
    string tmp(n, ' ');
    if(!std::fread(tmp.data(), sizeof(char_type), n, this->_M_file)){ return; }

    /*
        Remove CR
    */
    tmp.erase(std::remove(tmp.begin(), tmp.end(), '\r'), tmp.end());
    n -= tmp.size();

    /*
        Find delim and cut string
    */
    int idx = tmp.find(delim);
    if(idx == string::npos){
        idx = tmp.find_last_not_of(' ');
        if(idx != string::npos){ ++idx; }
    }
    tmp = tmp.substr(0, idx);
    if(n){
        // Count CR removed from tmp and increase delim counter by 1 for each LF
        n = 0;
        for(typename std::allocator<CharT>::size_type j=0; j<  tmp.size(); ++j){ if(tmp[j]   == '\n'){ ++n; } }
        for(typename std::allocator<CharT>::size_type i=0; i<delim.size(); ++i){ if(delim[i] == '\n'){ ++d; } }
    }

    /*
        Move file pointer back to just after delim
    */
    this->seek(pos + tmp.size() + n + d);
    buf += tmp;
}

/*
    WRITE FUNCS
*/

// Write entire contents of buffer
template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::xsputn(typename FileBase<CharT, Traits>::string& buf){ std::fwrite(buf.data(), sizeof(char_type), buf.size(), this->_M_file); }

// Write up to n bytes
template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::xsputn(
    typename FileBase<CharT, Traits>::string& buf,
    std::streamsize n
){ std::fwrite(buf.data(), sizeof(char_type), n, this->_M_file); }

// Writes up to n bytes or (char) delim (whichever comes first)
template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::xsputn(
    typename FileBase<CharT, Traits>::string& buf,
    std::streamsize n,
    const typename FileBase<CharT, Traits>::char_type delim
){
    for(int i=0; i<n; ++i){
        if(buf[i] == delim){ break; }
        else               { std::fputc(buf[i], this->_M_file); }
    }
}

// Write up to n bytes or (string) delim (whichever comes first)
template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::xsputn(
    typename FileBase<CharT, Traits>::string& buf,
    std::streamsize n,
    const typename FileBase<CharT, Traits>::string delim
){

    int_type sz = delim.size();
    for(int i=0; i<n; ++i){
        if(buf.substr(i, sz) == delim){ break; }
        else                          { std::fputc(buf[i], this->_M_file); }
    }
}

/**************************************************
                      PARSEBUF
**************************************************/

template<typename CharT, typename Traits>
ParseBuf<CharT, Traits>::ParseBuf() : std::basic_streambuf<CharT, Traits>() {}

template<typename CharT, typename Traits>
ParseBuf<CharT, Traits>::ParseBuf(const string& file, const string& mode) : std::basic_streambuf<CharT, Traits>(), _fdev(file, mode) {}

template<typename CharT, typename Traits>
ParseBuf<CharT, Traits>::ParseBuf(std::FILE*&& _fp) : std::basic_streambuf<CharT, Traits>(), _fdev(std::forward<std::FILE*>(_fp)) {}

template<typename CharT, typename Traits>
ParseBuf<CharT, Traits>::~ParseBuf() = default;

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
    int_type res = Traits::eof();
    if(!this->_fdev.eof()){
        this->sync();
        this->read();
        this->rparse();
        this->setg(&this->_buf[0], &this->_buf[0], &this->_buf[0]);
        res = this->_fdev.eof() ? Traits::eof() : 0;
    }
    return res;
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

/**************************************************
                      PARSESTREAM
**************************************************/

namespace {
    std::string _get_mode(const std::string& file){ return std::filesystem::exists(file) ? "rb+" : "wb+"; }
    // std::wstring _get_mode(const std::wstring& file){ return std::filesystem::exists(file) ? L"rb+" : L"wb+"; }

    template<typename CharT>
    std::basic_string<CharT> get_mode(const std::basic_string<CharT>& file){ return _get_mode(file); }
}

template<typename CharT, typename Traits>
ParseStream<CharT, Traits>::ParseStream() : std::basic_iostream<CharT, Traits>() {}

template<typename CharT, typename Traits>
ParseStream<CharT, Traits>::ParseStream(const string& file) : std::basic_iostream<CharT, Traits>(&_pbuf), _pbuf(file, get_mode(file)) {}

template<typename CharT, typename Traits>
ParseStream<CharT, Traits>::ParseStream(std::FILE*&& _fp) : std::basic_iostream<CharT, Traits>(&_pbuf), _pbuf(std::forward<std::FILE*>(_fp)) {}

template<typename CharT, typename Traits>
ParseStream<CharT, Traits>::~ParseStream() = default;

template<typename CharT, typename Traits>
ParseStream<CharT, Traits>& ParseStream<CharT, Traits>::read(ParseStream<CharT, Traits>::char_type* s, std::streamsize n){
    typename std::basic_istream<CharT, Traits>::sentry sty(*this, true);
    if(sty){
        std::streamsize count = this->rdbuf()->sgetn(s, n);
        if(count == 0){ this->setstate(std::ios_base::eofbit); }
    }
    return *this;
}

template<typename CharT, typename Traits>
ParseStream<CharT, Traits>& ParseStream<CharT, Traits>::write(const ParseStream<CharT, Traits>::char_type* s, std::streamsize n){
    typename std::basic_ostream<CharT, Traits>::sentry sty(*this);
    if(sty){ this->rdbuf()->sputn(s, n); }
    return *this;
}

////////////////////////////////////////////////////////////////////////////

template class ParseBuf<char>;
// template class ParseBuf<wchar_t>;

template class ParseStream<char>;
// template class ParseStream<wchar_t>;

template class FileBase<char>;
// template class FileBase<wchar_t>;