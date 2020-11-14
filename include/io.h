#ifndef IO_H
#define IO_H

#include <iostream>
#include <fstream>
#include <vector>

#include "types.h"
#include "util.h"
#include "encode.h"
#include "decode.h"
#include "pgn.h"


class IO{

private:
    PGN pgn_parser;

public:
    void write_byte(std::fstream&, BYTE*);
    void read_byte(std::fstream&, BYTE*);
    void read_ply(std::fstream&, BYTE*);
    void write_eply(std::fstream&, eply*);
    void write_eturn(std::fstream&, eturn*);
    void write_egame(std::fstream&, egame&);
    void write_tags(std::fstream&, pgndict&);
    void load_pgn_file(char *filename, std::vector<game>&, std::vector<pgndict>&);
};

void IO::write_byte(std::fstream &fout, BYTE* b){ fout.write((char*) b, 1); }

void IO::read_byte(std::fstream &fin, BYTE* b){ fin.read((char*) b, 1); }

void IO::read_ply(std::fstream &fin, BYTE* b){ fin.read((char*) b, 2); }

void IO::write_eply(std::fstream &fout, eply *p){
    this->write_byte(fout, &(p->name));
    this->write_byte(fout, &(p->action));
}

void IO::write_eturn(std::fstream &fout, eturn *t){
    this->write_eply(fout, &(t->white));
    this->write_eply(fout, &(t->black));
}

void IO::write_egame(std::fstream &fout, egame &g){
    for(int i=0;i<g.length();i++){
        this->write_eturn(fout, &(g.turns.at(i)));
    }
}

void IO::load_pgn_file(char *filename, std::vector<game> &games, std::vector<pgndict> &tags){

    std::cout << "Opening file " << filename <<std::endl;
    std::fstream fin(filename, std::ios::in);
    std::cout << "File opened" << std::endl;

    pgn_parser.parse_file(fin, games, tags);
    
    fin.close();
}


#endif