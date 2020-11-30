#ifndef FILE_BASE_H
#define FILE_BASE_H

#include <string>
#include <cstdio>

#include "types.h"


/******************************
    FileBase
******************************/

// Wrapper for C-style file handles
template<typename CharT, typename Traits=std::char_traits<CharT> >
class FileBase{
public:

    typedef typename Traits::int_type int_type;
    typedef typename Traits::char_type char_type;
    typedef typename std::basic_string<char_type> string;

    bool eof{false};
    string _fname, _mode;
    std::FILE* _M_file {NULL};

    FileBase(string fname, string mode) : _fname(fname), _mode(mode){ this->open(); }
    ~FileBase(){ this->close(); }
    void open();
    int close();
    void xsgetn(string&, std::streamsize);
    void xsgetn(string&, std::streamsize, char_type);
    void xsgetn(string&, std::streamsize, string&);
};

/******************************
    FileBase Member Funcs
******************************/

// TODO: Check file was successfully opened/closed
template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::open(){ if(this->_M_file == 0){ this->_M_file = std::fopen(this->_fname.data(), this->_mode.data()); } }

template<typename CharT, typename Traits>
int FileBase<CharT, Traits>::close(){ return std::fclose(this->_M_file); }

// Read into string buf. Reads up to n bytes or EOF (whichever comes first)
template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::xsgetn(string& buf, std::streamsize n){ std::fread(buf.data(), 1, n, this->_M_file); }

// Read into string buf. Reads up to n bytes, EOF, or delim (whichever comes first)
template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::xsgetn(string& buf, std::streamsize n, char_type delim){
    int_type c;
    for(short i=0; i<n; i++){
        if((c = std::fgetc(this->_M_file)) == EOF){ this->eof = true; break; }
        else if(c == delim){ break; }
        else{ buf.push_back(Traits::to_char_type(c)); }
    }
}

// Read into string buf. Reads up to n bytes, EOF, or delim (whichever comes first)
// delim is a multi-char string
template<typename CharT, typename Traits>
void FileBase<CharT, Traits>::xsgetn(string& buf, std::streamsize n, string& delim){

    int_type c, eob;

    for(short i=0; i<n; i++){

        c = std::fgetc(this->_M_file);

        if(c == Traits::eof()){ this->eof = true; break; }
        else if(c == Traits::to_int_type(delim[0])){
            // Read delim-sized block into buf
            buf.push_back(Traits::to_char_type(c));
            for(short j=1; j<delim.size(); j++){
                if((c = std::fgetc(this->_M_file)) == Traits::eof()){ this->eof = true; break; }
                else{ buf.push_back(Traits::to_char_type(c)); }
            }
            // If delim, erase and break
            eob = buf.size() - delim.size();
            if(!buf.compare(eob, delim.size(), delim)){
                buf.erase(eob);
                break;
            }
        }
        else{ buf.push_back(Traits::to_char_type(c)); }
    }
}


#endif