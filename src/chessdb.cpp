#include <iostream>
#include <fstream>
#include <utility>
#include <map>
#include <string>

#include "decode.h"
#include "encode.h"
#include "util.h"
#include "io.h"
#include "types.h"

using namespace std;

int main() {

    IO io;
    vector<game> games;
    vector<pgndict> tags;

    char datafile[]{"data\\otbgames\\OTB.pgn"};
    io.load_pgn_file(datafile, games, tags);

    // Encoder enc;
    // egame res;
    // char outfile[]{"data\\otbgames\\OTB_encoded.bin"};
    // cout << "Encoding data and saving to " << outfile << endl;
    // fstream fout(outfile, ios::out | ios::binary);
    // for(int i=0;i<games.size();i++){
    //     enc.encode_game(games[i], &res);
    //     io.write_egame(fout, res);
    // }
    // fout.close();


    // fstream fin(outfile, ios::in | ios::binary);
    // fstream fout2("data\\otb_samples\\results.txt", ios::out);
    // Decoder dec;
    // BYTE data_in[2]{0,0};
    // eply enc_ply;
    // ply dec_ply;
    // while(!fin.eof()){

    //     io.read_ply(fin, data_in);

    //     enc_ply = eply{data_in[0], data_in[1]};
    //     dec.decode_ply(enc_ply, &dec_ply);

    //     fout2 << "Encoded Ply: ";
    //     fout2 << (bs8) enc_ply.name << " " << (bs8) enc_ply.action << " " << endl;

    //     fout2 << "Decoded Ply: ";
    //     fout2 << util::pname2s[dec_ply.piece.name] << " moves "
    //          << dec_ply.dst[1] << " files "
    //          << dec_ply.dst[0] << " ranks."
    //          << endl << endl;
    // }
    // fout2.close();


    return 0;
};