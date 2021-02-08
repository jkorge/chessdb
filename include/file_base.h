#ifndef FILE_BASE_H
#define FILE_BASE_H

#include <string>
#include <cstdio>

/******************************
    FileBase
******************************/

// Wrapper for C-style file handles
template<typename CharT, typename Traits=std::char_traits<CharT> >
class FileBase{

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;

public:

    string _fname, _mode;
    std::FILE* _M_file {NULL};

    FileBase(const string& fname, const string& mode) : _fname(fname), _mode(mode){ this->open(); }
    ~FileBase(){ this->close(); }
    void open();
    void close();
    bool eof();

    void xsgetn(string&, std::streamsize);
    void xsgetn(string&, std::streamsize, char_type);
    void xsgetn(string&, std::streamsize, string&);

    void xsputn(string&);
    void xsputn(string&, std::streamsize);
    void xsputn(string&, std::streamsize, char_type);
    void xsputn(string&, std::streamsize, string&);
};

/******************************
    FileBase Member Funcs
******************************/

template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::open(){ if(this->_M_file == 0){ this->_M_file = std::fopen(this->_fname.data(), this->_mode.data()); } }

template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::close(){ std::fclose(this->_M_file); }

template<typename CharT, typename Traits>
bool FileBase<CharT, Traits>::eof(){ return std::feof(this->_M_file); }

/*
    READ FUNCS
*/

// Reads up to n bytes or EOF (whichever comes first)
template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::xsgetn(
    typename FileBase<CharT, Traits>::string& buf,
    std::streamsize n
){
    char_type tmp[n];
    std::fread(tmp, sizeof(char_type), n, this->_M_file);
    buf = tmp;
}

// Reads up to n bytes, EOF, or delim (whichever comes first)
template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::xsgetn(
    typename FileBase<CharT, Traits>::string& buf,
    std::streamsize n,
    typename FileBase<CharT, Traits>::char_type delim
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
    typename FileBase<CharT, Traits>::string& delim
){

    int_type c;
    char_type ch;
    string s(delim.size(), ' ');

    if(std::fgets(s.data(), delim.size()+1, this->_M_file) != NULL){
        for(int i=delim.size(); i<n; ++i){
            if(s == delim){
                s.clear();
                break;
            }
            else{
                if((c = std::fgetc(this->_M_file)) == Traits::eof()){ break; }
                else if((ch = Traits::to_char_type(c)) == '\r'){ continue; }
                else{
                    buf.push_back(s[0]);
                    s.erase(0, 1);
                    s.push_back(ch);
                }
            }
        }
        buf += s;
    }
}

/*
    WRITE FUNCS
*/

// Write entire contents of buffer
template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::xsputn(typename FileBase<CharT, Traits>::string& buf){ std::fputs(buf.data(), this->_M_file); }

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
    typename FileBase<CharT, Traits>::char_type delim
){
    for(int i=0; i<n; ++i){
        if(buf[i] == delim){ break; }
        else                { int_type c = std::fputc(buf[i], this->_M_file); }
    }
}

// Write up to n bytes or (string) delim (whichever comes first)
template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::xsputn(
    typename FileBase<CharT, Traits>::string& buf,
    std::streamsize n,
    typename FileBase<CharT, Traits>::string& delim
){

    int_type sz = delim.size();
    for(int i=0; i<n; ++i){
        if(buf.substr(i, sz) == delim){ break; }
        else                          { int_type c = std::fputc(buf[i], this->_M_file); }
    }
}

#endif