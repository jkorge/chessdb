#include <iostream>
#include <string>

#include "board.h"
#include "fen.h"


int main(){

    // std::string test{"8/pQRq2pk/4p2p/3r1p2/3P4/4P3/PP3PPP/6K1 b - - 0 1"};
    std::string test{"4k2r/pp1bqp2/4p2p/3p4/3Q4/3B4/PP1B2R1/4R1K1 w - - 0 1"};
    FENCE fen_parser;
    ChessBoard board;
    std::cout << test << std::endl << std::endl;
    fen_parser.parse_fen_string(test, board);
    board.print_board();
    return 0;
}