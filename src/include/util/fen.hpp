#ifndef UTIL_FEN_H
#define UTIL_FEN_H

namespace util{
    namespace fen{
        /*
            UTILITY OBJECTS FOR FEN PARSING
        */
        const std::regex delim("/");

        // FEN string for new (standard) game of chess
        const std::string new_game("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

        ptype c2type(char p);
    }
}


#endif