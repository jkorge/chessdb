#include <fstream>

#include "devutil.hpp"

// cmd options
bool DIV{false};
std::vector<std::string> MOVETEXT;
std::string REPR{"/"}, FENSTR;

// tracking leaf node properties
long N_TERMINAL{0},
    N_CAPTURES{0},
    N_CHECKS{0},
    N_MATES{0},
    N_TERMINAL_TOTAL{0},
    N_CAPTURES_TOTAL{0},
    N_CHECKS_TOTAL{0},
    N_MATES_TOTAL{0},
    MAX_DEPTH{1};

// formatting specs
constexpr std::size_t sz{20};
constexpr int NCOLS{6};
typedef table<sz> tbl;

// recurse over legal plies from board position @ root up to MAX_DEPTH
void search_tree(Board root, int lvl=1){

    std::vector<ply> plies = root.legal_plies(root.next);
    std::vector<Board> successors(plies.size(), root);
    for(int i=0; i<plies.size(); ++i){
        if(lvl == MAX_DEPTH){
            ++N_TERMINAL;
            if(plies[i].capture){ ++N_CAPTURES; }
            if(plies[i].check)  { ++N_CHECKS; }
            if(plies[i].mate)   { ++N_MATES; }
        }
        else{
            successors[i].update(plies[i]);
            search_tree(successors[i], lvl + 1);
        }
    }
}

int main(int argc, char** argv){
    std::cout << std::boolalpha;
    std::cout.imbue(std::locale(std::cout.getloc(), new comma_sep));

    // argparse
    for(int i=1; i<argc; ++i){
        if(!std::strcmp(argv[i], "-d")){ MAX_DEPTH = std::stoi(argv[++i]); }
        else
        if(!std::strcmp(argv[i], "-x")){ DIV = true; }
        else
        if(!std::strcmp(argv[i], "-m")){
            REPR.clear();
            while(i<argc-1 && argv[i+1][0] != '-'){ MOVETEXT.emplace_back(argv[++i]); }
        }
        else
        if(!std::strcmp(argv[i], "-f")){ FENSTR = argv[++i]; }
    }

    Board root;

    // Advance board state according to cmd args
    if(!FENSTR.empty()){ fen::parse(FENSTR, root); }
    for(int i=0; i<MOVETEXT.size(); ++i){
        if(white == root.next){ REPR += std::to_string(i/2 + 1) + "."; }
        REPR += MOVETEXT[i] + " ";
        root.update(pply(MOVETEXT[i], root.next, root));
    }

    if(REPR.size() > sz){ REPR = REPR.substr(0, sz-3) + "..."; }
    if(!FENSTR.empty() || REPR != "/"){ std::cout << root.to_string() << '\n'; }

    uint64_t t0, t1, T = 0;
    tbl::row("Ply", "N", "Captures", "Checks", "Checkmates", "Time");
    tbl::sep(NCOLS);

    if(DIV){

        std::vector<ply> plies = root.legal_plies(root.next);
        std::vector<Board> successors(plies.size(), root);

        for(int i=0; i<plies.size(); ++i){

            if(MAX_DEPTH > 1){

                successors[i].update(plies[i]);

                t0 = Tempus::time();
                search_tree(successors[i], 2);
                t1 = Tempus::time();

                T += t1 - t0;
                tbl::row(root.ply2san(plies[i]), N_TERMINAL, N_CAPTURES, N_CHECKS, N_MATES, Tempus::strtime(t1 - t0));

                N_TERMINAL_TOTAL += N_TERMINAL;
                N_CAPTURES_TOTAL += N_CAPTURES;
                N_CHECKS_TOTAL += N_CHECKS;
                N_MATES_TOTAL += N_MATES;

                N_TERMINAL = N_CAPTURES = N_CHECKS = N_MATES = 0;
            }
            else{
                ++N_TERMINAL;
                if(plies[i].capture){ ++N_CAPTURES; }
                if(plies[i].check)  { ++N_CHECKS; }
                if(plies[i].mate)   { ++N_MATES; }

                tbl::row(root.ply2san(plies[i]), N_TERMINAL, N_CAPTURES, N_CHECKS, N_MATES, Tempus::strtime(0));

                N_TERMINAL_TOTAL += 1;
                N_CAPTURES_TOTAL += plies[i].capture;
                N_CHECKS_TOTAL += plies[i].check;
                N_MATES_TOTAL += plies[i].mate;
                N_TERMINAL = N_CAPTURES = N_CHECKS = N_MATES = 0;
            }
        }
        tbl::sep(NCOLS);
        tbl::row(REPR, N_TERMINAL_TOTAL, N_CAPTURES_TOTAL, N_CHECKS_TOTAL, N_MATES_TOTAL, Tempus::strtime(T));
        tbl::sep(NCOLS);
    }
    else{
        t0 = Tempus::time();
        search_tree(root);
        t1 = Tempus::time();

        tbl::row(REPR, N_TERMINAL, N_CAPTURES, N_CHECKS, N_MATES, Tempus::strtime(t1 - t0));
        tbl::sep(NCOLS);
    }

    return 0;
}