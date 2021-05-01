#include "include/parsestream.hpp"

/**************************************************
                      FILEBASE
**************************************************/

std::string wtomb(const wchar_t* wide){
    std::mbstate_t state = std::mbstate_t();
    std::size_t sz = std::wcsrtombs(nullptr, &wide, 0, &state);
    std::string mb;
    std::wcsrtombs(&mb[0], &wide, sz+1, &state);
    return mb;
}

template<>
void FileBase<char>::open(){
    if(this->_M_file == 0){
        this->_M_file = std::fopen(this->_fname.data(), this->_mode.data());
        std::setvbuf(this->_M_file, NULL, _IONBF, 0);
    }
}

template<>
void FileBase<wchar_t>::open(){

    // Wide filename/mode => Narrow (multi-byte) filename/mode
    const wchar_t *_fname_loc = this->_fname.data(),
                  *_mode_loc = this->_mode.data();
    std::string _mbfname = wtomb(_fname_loc),
                _mbmode = wtomb(_mode_loc);

    if(this->_M_file == 0){
        this->_M_file = std::fopen(_mbfname.data(), _mbmode.data());
        std::setvbuf(this->_M_file, NULL, _IONBF, 0);
    }
}

template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::close(){ if(this->_M_file){ std::fclose(this->_M_file); } }

template<typename CharT, typename Traits>
bool FileBase<CharT, Traits>::eof(){ return std::feof(this->_M_file); }

template<typename CharT, typename Traits>
int FileBase<CharT, Traits>::seek(int32_t offset, int origin){ return std::fseek(this->_M_file, offset, origin); }

template<typename CharT, typename Traits>
int32_t FileBase<CharT, Traits>::tell(){ return std::ftell(this->_M_file); }

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
){
    int_type c;
    for(int i=0; i<n; ++i){
        if((c = std::fgetc(this->_M_file)) == Traits::eof()){ break; }
        else if(c == delim){ break; }
        else if(c == '\r'){ continue; }
        else{ buf.push_back(Traits::to_char_type(c)); }
    }
}

// Reads up to n bytes, EOF, or delim (whichever comes first)
// delim is a multi-char string
template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::xsgetn(
    typename FileBase<CharT, Traits>::string& buf,
    std::streamsize n,
    const typename FileBase<CharT, Traits>::string delim
){

    int_type c, N = delim.size();
    char_type ch;
    string s(N, ' ');

    if(std::fread(s.data(), sizeof(char_type), N, this->_M_file)){

        for(int i=N; i<n; ++i){

            if(s == delim)
                { return; }

            else
            if((c = std::fgetc(this->_M_file)) == Traits::eof())
                { break; }

            else
            if((ch = Traits::to_char_type(c)) == '\r')
                { continue; }

            else
            {
                buf.push_back(s[0]);
                s = s.substr(1, N-1) + ch;
            }
        }

        buf.append(s);
    }
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

/**************************************************
                      PARSESTREAM
**************************************************/

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
// template class ParseStream<wchar_t>;
template class ParseBuf<char>;
// template class ParseBuf<wchar_t>;
template class FileBase<char>;
// template class FileBase<wchar_t>;