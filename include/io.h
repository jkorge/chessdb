#ifndef IO_H
#define IO_H

#include <iostream>
#include <fstream>

#include "types.h"
#include "util.h"


class IO{

public:
    void write_bytes(std::fstream&, BYTE*, ushort int=1);
    void read_bytes(std::fstream&, BYTE*, ushort int=1);
    void open_text_file(str,int=0);
};

void IO::write_bytes(std::fstream &fout, BYTE* b, ushort n){ fout.write((char*) b, n); }

void IO::read_bytes(std::fstream &fin, BYTE* b, ushort n){ fin.read((char*) b, n); }

std::fstream& IO::open(str filename, uint8 mode){
    std::fstream fin(filename, mode);
    return fin;
}

void IO::close(std::fstream &file){
    file.close();
}


#endif