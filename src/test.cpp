#include <iostream>
#include <fstream>
#include <utility>
#include <map>
#include <string>
#include <cstdlib>
#include <regex>

#include "types.h"
#include "util.h"
#include "encode.h"
#include "pgn.h"

using namespace std;

#ifdef _WIN32
    #include <windows.h>
    void sleep(unsigned int ms){ Sleep(ms); }
#else
    #include <unistd.h>
    void sleep(unsigned int ms){ usleep(ms * 1000); } // usleep takes microsecs
#endif

static ChessBoard board;

void read_movetext(char *file, string &dst){

    fstream fin(file, ios::in);
    string line;
    int N;
    while(getline(fin, line)){
        if(!line.length()){ continue; }
        else if(!line.compare(0,1,"[")){ continue; }
        else{
            N = line.length();
            while(N){
                // Add space at EOL if not already there and doesn't end in '.'
                if(line.compare(N-1, 1, " ") & line.compare(N-1, 1, ".")){ line+= " "; }
                dst += line;
                if(!getline(fin, line)){ break; }
                N = line.length();
            }
            break;
        }
    }
    fin.close();
}

void encode_decode_ply(ostream &out, ply &prc_p){

    static Encoder enc;
    static Decoder dec;
    static ply dec_p;
    static eply enc_p;

    enc.encode_ply(prc_p, &enc_p);
    dec.decode_ply(enc_p, &dec_p);

    out << "Processed Ply: ";
    out << util::ptype2s[prc_p.piece.type] << " to " << util::file2c[prc_p.dst[1]] << prc_p.dst[0]+1 << ". ";
    if(prc_p.castle){ out << (prc_p.castle>0 ? "Kingside " : "Queenside ") << "Castle. "; }
    if(prc_p.capture){ out << "Capture. "; }
    if(prc_p.check){ out << "Check. "; }
    if(prc_p.mate){ out << "Mate. "; }
    if(prc_p.promo){ out << "Promote pawn to " << util::ptype2s[prc_p.promo] << ". "; }
    out << endl;

    out << "Encoded Ply: ";
    out << (bs8) enc_p.name << " " << (bs8) enc_p.action << " " << endl;

    out << "Decoded Ply: ";
    out << util::pname2s[dec_p.piece.name] << " moves " << dec_p.dst[1] << " files " << dec_p.dst[0] << " ranks." << endl;
    out << endl;

}

void board_update_ply(ostream &out, ply &p, ChessBoard &board){
    board.update(p);
    board.print_board(out);
    sleep(1000);
}

void run_test_turn(ostream &out, turn t, string turn_string, bool test_encode_decode){

    static char *wtok, *btok;
    wtok = strtok(&turn_string[0], " ");
    btok = strtok(NULL, " ");

    for(int k=0;k<2;k++){
        if(t[k].piece.name == NONAME){ continue; }

        out << ((k%2) ? btok : wtok) << endl;

        if(test_encode_decode){ encode_decode_ply(out, (k%2) ? t.black : t.white); }
        else{ board_update_ply(out, k%2 ? t.black : t.white, board); }
    }
}

void run_test(ostream &out, string movetext, game &g, bool test_encode_decode){

    int i = 0;
    string turn_string;

    regex move_num_r("(\\d{1,}\\.)");
    regex_token_iterator<string::iterator> end;
    regex_token_iterator<string::iterator> rtok(movetext.begin(), movetext.end(), move_num_r, -1);

    out << movetext << endl << endl;

    if(!test_encode_decode){ board.print_board(out); }

    while(rtok!=end){

        turn_string = *rtok++;
        if(!turn_string.length()){ continue; }
        run_test_turn(out, g[i++], turn_string, test_encode_decode);
        out << endl;
    }
}

int main() {

    char file[] = "data/otb_samples.pgn";
    string movetext;
    read_movetext(file, movetext);

    PGN pgn_parser;
    game g = pgn_parser.parse_game(movetext);

    // fstream fout("data\\output_sample.txt", ios::out);
    run_test(cout, movetext, g, true);
    return 0;
};