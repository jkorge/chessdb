#include <cstdlib>

#include "devutil.hpp"
#include "logging.hpp"
#include "tempus.hpp"
#include "pgn.hpp"
#include "chessdb.hpp"

typedef header<20> hdr;
int pfreq{20};

void prog(float i, float N, char end='\0'){ std::cout << int(100 * (i/N)) << '%' << end; }

std::vector<game> load_pgn(std::string file, int start, int stop=-1){
    hdr::print("Loading PGN");

    // Processors
    PGNStream pstr(file);

    // Containers
    std::vector<game> games;
    game g;
    unsigned long long t0 = Tempus::time();
    double T;

    for(int i=0; i<start; ++i){ pstr >> g; }

    if(stop>0){
        float N = stop - start;
        int p = N / pfreq;
        for(int i=start; i<stop; ++i){
            pstr >> g;
            games.emplace_back(g);
            if(i && not (i%p)){ prog(i+1 - start, N, '\r'); }
            if(pstr.eof()){ break; }
        }
        prog(N, N);
    }
    else{
        // Read to end of file
        int i = 0;
        while(!pstr.eof()){
            pstr >> g;
            games.emplace_back(g);
            std::cout << ++i << '\r';
        }
        prog(1.0, 1.0);
    }
    T = Tempus::time() - t0;
    std::cout << " (" << (T/1.0e9) << " s)" << "\n\n";
    return games;
}

void write_game_data(std::string file, const std::vector<game>& games){
    hdr::print("Encoding DB");

    ChessDBStream cstr(file);
    float N = games.size(), T;
    int i = 0,
        p = N / pfreq;
    unsigned long long t0 = Tempus::time();

    for(std::vector<game>::const_iterator it=games.begin(); it!=games.end(); ++it){
        cstr.insert(*it);
        if(++i && not (i%p)){ prog(i, N, '\r'); }
    }
    prog(N, N);
    T = Tempus::time() - t0;
    std::cout << " (" << (T/1.0e9) << " s)" << "\n\n";
}

std::vector<game> load_db(std::string& file){
    hdr::print("Loading DB");

    ChessDBStream cstr(file);
    std::vector<game> games;
    float N = cstr.size(), T;
    int p = N / pfreq;
    unsigned long long t0 = Tempus::time();

    for(int i=0; i<N; ++i){
        games.emplace_back(cstr.select(i));
        if(i && not (i%p)){ prog(i+1, N, '\r'); }
    }
    prog(N, N);
    T = Tempus::time() - t0;
    std::cout << " (" << (T/1.0e9) << " s)" << "\n\n";

    return games;
}

bool error_check(const std::vector<game>& games, const std::vector<game>& lgames){
    hdr::print("Error Check");
    ChessBoard board;
    Fen fen;

    if(games != lgames){
        int N = games.size(),
            L = lgames.size();
        if(N != L){
            if(N > L){ std::cout << "DB File Contained " << L << "/" << N << " games\n"; }
            else     { std::cout << "Found " << L << "games in DB file. Expected " << N << '\n'; }
        }
        else{
            for(int i=0; i<lgames.size(); ++i){
                if(!games[i].tags.at(fenstr).empty()){ fen.parse(games[i].tags.at(fenstr), board); }
                else{ board.newgame(); }
                for(int j=0; j<lgames[i].plies.size(); ++j){

                    if(lgames[i].plies[j] != games[i].plies[j]){
                        // Print details where files disagree
                        std::cout << "Game " << i << " ply " << j << ":\n" << board.display() << '\n';

                        board.update(games[i].plies[j]);
                        std::cout << board.display() << "\nsrc: ";
                        // printply(games[i].plies[j]);
                        pprint(games[i].plies[j], board);

                        std::cout << "dst: ";
                        // printply(lgames[i].plies[j]);
                        pprint(lgames[i].plies[j], board);
                        std::cout << '\n';
                    }
                    else{ board.update(games[i].plies[j]); }
                }
            }
        }
        std::cout << '\n';
        return false;
    }
    else{
        std::cout << "No errors found\n\n";
        return true;
    }
}

bool create(std::string& src_file, std::string& dst_file, int start, int stop){

    // Load games from PGN file
    std::vector<game> games = load_pgn(src_file, start, stop);
    
    // Encode and write to db file
    write_game_data(dst_file, games);

    // Load db
    std::vector<game> lgames = load_db(dst_file);

    // Compare games loaded from db to those from original PGN file
    return error_check(games, lgames);
}

void load(std::string& dst_file){ std::vector<game> lgames = load_db(dst_file); }

int main(int argc, char** argv){

    // IO Manip. stdout
    std::cout << std::boolalpha << std::fixed;
    std::cout << std::setprecision(2);

    // Num games to load
    int start = 0,
        stop = 100;

    // Filenames
    std::string src_file = "/Users/jkorg/Desktop/chess/data/otb/OTB.pgn",
                dst_file = "testdb.cdb";

    for(int i=1; i<argc; ++i){
        if(!std::strcmp(argv[i], "-s")){ start = std::stoi(argv[++i]); }
        if(!std::strcmp(argv[i], "-t")){ stop  = std::stoi(argv[++i]); }
        if(!std::strcmp(argv[i], "-p")){ pfreq = std::stoi(argv[++i]); }
        if(!std::strcmp(argv[i], "-f")){ src_file = argv[++i]; }
        if(!std::strcmp(argv[i], "-d")){ dst_file = argv[++i]; }
    }

    std::cout << "Reading games " << start << " to " << stop
              << " from " << src_file << " and saving to " << dst_file << '\n';

    if(create(src_file, dst_file, start, stop)){ load(dst_file); }

    std::system((std::string("del ") + dst_file).data());
    hdr::print("Done");

    return 0;
}
