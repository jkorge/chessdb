#include <cstdlib>

#include "devutil.hpp"

const int NCOLS = 5;

typedef table<20> tbl;

int main(int argc, char** argv){
    std::string src_file = "/Users/jkorg/Desktop/chess/data/otb/OTB.pgn",
                dst_file = "OTB.cdb";

    int batch_size = 100000;

    for(int i=1; i<argc; ++i){
        if(!std::strcmp(argv[i], "-f")){ src_file = argv[++i]; }
        else
        if(!std::strcmp(argv[i], "-t")){ dst_file = argv[++i]; }
        else
        if(!std::strcmp(argv[i], "-b")){ batch_size = std::stoi(argv[++i]); }
    }

    PGNStream pstr(src_file);
    ChessDBStream cstr(dst_file);

    std::vector<game> batch_games;
    batch_games.reserve(batch_size);

    tbl::row("Batch No.", "Batch Size", "Time", "Avg/Game", "Cumulative");
    tbl::sep(NCOLS);

    int batch = 0, N = 0;
    unsigned long long Tf, Ti = Tempus::time(), T0=Tempus::time();

    while(pstr){
        // Load game
        pstr >> batch_games;

        if(batch_games.size() == batch_size){
            // Dump batch
            for(int j=0; j<batch_size; ++j){ cstr.insert(batch_games[j]); }
            Tf = Tempus::time();
            N += batch_size;
            tbl::row(++batch, batch_size, Tempus::strtime(Tf-Ti), Tempus::strtime((Tf-Ti) / batch_size), Tempus::strtime(Tf-T0));
            batch_games.clear();
            batch_games.reserve(batch_size);
            Ti = Tempus::time();
        }
    }
    if(batch_games.size()){
        for(int j=0; j<batch_games.size(); ++j){ cstr.insert(batch_games[j]); }
        Tf = Tempus::time();
        N += batch_games.size();
        tbl::row(++batch, batch_games.size(), Tempus::strtime(Tf-Ti), Tempus::strtime((Tf-Ti) / batch_games.size()), Tempus::strtime(Tf-T0));
        batch_games.clear();
    }

    tbl::sep(NCOLS);
    std::cout << "\n\n";

    tbl::row("Num. Games", "Num. Batches", "Avg/Batch", "Avg/Game", "Total");
    tbl::sep(NCOLS);
    unsigned long long Ttot = Tf - T0;
    tbl::row(N, batch, Tempus::strtime(Ttot/batch), Tempus::strtime(Ttot/N), Tempus::strtime(Ttot));
    tbl::sep(NCOLS);

    return 0;
}
