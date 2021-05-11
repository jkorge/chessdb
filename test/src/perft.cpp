#include <future>
#include <thread>

#include "devutil.hpp"

/**************************************************
        VARIABLES, STRUCTS, & TYPEDEFS
**************************************************/

// cmd options
bool DET{true};
std::string REPR{"/"},
            FENSTR;
std::vector<std::string> MOVETEXT;

// maximum search depth
long MAX_DEPTH{1};

// formatting specs
constexpr std::size_t sz{20};
typedef table<6, 20, true> tbl;
typedef table<3, 20, true> tab;

// Array typedef to contain results
typedef struct stats : std::array<uint64_t, 5>{
    stats operator+(const stats& other) const{
        return {
            this->at(0) + other.at(0),
            this->at(1) + other.at(1),
            this->at(2) + other.at(2),
            this->at(3) + other.at(3),
            this->at(4) + other.at(4)
        };
    }

    void operator+=(const stats& other){
        this->operator[](0) += other.at(0);
        this->operator[](1) += other.at(1);
        this->operator[](2) += other.at(2);
        this->operator[](3) += other.at(3);
        this->operator[](4) += other.at(4);
    }
} stats;

// container for results
stats RESULTS = {0};

/**************************************************
                FUNCTION DECLARATIONS
**************************************************/

stats search(Board, int);
stats searchn(Board, int);
void perft(Board);
void perft1(Board);
Board setup();
void print_header();
void print_footer();

/**************************************************
                        MAIN
**************************************************/

int main(int argc, char** argv){
    std::cout << std::boolalpha;
    std::cout.imbue(std::locale(std::cout.getloc(), new comma_sep));

    // argparse
    for(int i=1; i<argc; ++i){
        if(!std::strcmp(argv[i], "-d")){ MAX_DEPTH = std::stoi(argv[++i]); }
        else
        if(!std::strcmp(argv[i], "-m")){
            REPR.clear();
            while(i<argc-1 && argv[i+1][0] != '-'){ MOVETEXT.emplace_back(argv[++i]); }
        }
        else
        if(!std::strcmp(argv[i], "-f")){ FENSTR = argv[++i]; }
        else
        if(!std::strcmp(argv[i], "-q")){ DET = false; }
    }

    Board root = setup();

    if(MAX_DEPTH > 1){ perft(root); }
    else             { perft1(root); }

    return 0;
}

/**************************************************
                FUNCTION DEFINITIONS
**************************************************/

// recurse over legal plies from board position @ root up to MAX_DEPTH
stats search(Board root, int lvl){
    std::vector<ply> plies = root.legal_plies(root.next);
    if(lvl == MAX_DEPTH){ return {(uint64_t)plies.size(), 0ULL, 0ULL, 0ULL, 0ULL}; }

    std::vector<Board> successors(plies.size(), root);
    stats n = {0};
    for(int i=0; i<plies.size(); ++i){
        successors[i].update(plies[i]);
        n[0] += search(successors[i], lvl + 1)[0];
    }

    return n;
}

// search_tree + track extra stats
stats searchn(Board root, int lvl){

    std::vector<ply> plies = root.legal_plies(root.next);
    if(lvl == MAX_DEPTH){
        stats ret = {(uint64_t)plies.size(), 0ULL, 0ULL, 0ULL, 0ULL};
        for(int i=0; i<plies.size(); ++i){
            if(plies[i].capture){ ++ret[1]; }
            if(plies[i].check)  { ++ret[2]; }
            if(plies[i].mate)   { ++ret[3]; }
        }
        return ret;
    }

    std::vector<Board> successors(plies.size(), root);
    stats res = {0};
    for(int i=0; i<plies.size(); ++i){
        successors[i].update(plies[i]);
        res += searchn(successors[i], lvl + 1);
    }
    return res;
}

void perft(Board root){

    // First level down from root
    std::vector<ply> plies = root.legal_plies(root.next);
    std::vector<Board> successors(plies.size(), root);
    for(int i=0; i<plies.size(); ++i){ successors[i].update(plies[i]); }

    int N = successors.size();

    // Vectors for thread objects and their results
    std::vector<std::thread> threads;
    std::vector<std::future<stats> > futures(N);
    std::vector<stats> fulfilled(N);

    // Create threads
    for(int i=0; i<N; ++i){
        std::promise<stats> prms;
        futures[i] = prms.get_future();

        // Construct in-place at back of vector
        threads.emplace_back([prms=std::move(prms)](Board root) mutable{
            uint64_t t0 = Tempus::time();
            stats res = DET ? searchn(root, 2) : search(root, 2);
            res[4] = Tempus::time() - t0;
            prms.set_value(res);
        }, successors[i]);
    }

    // Await results from futures
    for(int i=0; i<N; ++i){
        fulfilled[i] = futures[i].get();
        threads[i].join();
    }

    // Print results to console
    print_header();
    for(int i=0; i<N; ++i){
        if(DET){
            // Accumulate results
            RESULTS[0] += fulfilled[i][0];
            RESULTS[1] += fulfilled[i][1];
            RESULTS[2] += fulfilled[i][2];
            RESULTS[3] += fulfilled[i][3];
            RESULTS[4]  = std::max(RESULTS[4], fulfilled[i][4]);

            // Print
            tbl::row(root.ply2san(plies[i]), fulfilled[i][0], fulfilled[i][1], fulfilled[i][2], fulfilled[i][3], Tempus::strtime(fulfilled[i][4]));
        }
        else{
            // Accumulate results
            RESULTS[0] += fulfilled[i][0];
            RESULTS[4]  = std::max(RESULTS[4], fulfilled[i][4]);

            // Print
            tab::row(root.ply2san(plies[i]), fulfilled[i][0], Tempus::strtime(fulfilled[i][4]));
        }
    }
    print_footer();
}

void perft1(Board root){

    std::vector<uint64_t> times;
    uint64_t t0 = Tempus::time();

    // First level down from root
    std::vector<ply> plies = root.legal_plies(root.next);
    RESULTS[0] = plies.size();
    if(DET){
        for(int i=0; i<RESULTS[0]; ++i){
            RESULTS[1] += plies[i].capture;
            RESULTS[2] += plies[i].check;
            RESULTS[3] += plies[i].mate;
            times.emplace_back(Tempus::time() - t0);
        }
    }
    RESULTS[4] = Tempus::time() - t0;

    // Print results to console
    print_header();
    for(int i=0; i<RESULTS[0]; ++i){
        if(DET){ tbl::row(root.ply2san(plies[i]), 1, (int)plies[i].capture, (int)plies[i].check, (int)plies[i].mate, Tempus::strtime(times[i])); }
        else   { tab::row(root.ply2san(plies[i]), 1, Tempus::strtime(Tempus::time() - t0)); }
    }
    print_footer();
}

Board setup(){

    Board root;

    // Advance board state according to cmd args
    if(!FENSTR.empty()){ fen::parse(FENSTR, root); }
    for(int i=0; i<MOVETEXT.size(); ++i){
        if(white == root.next){ REPR += std::to_string(i/2 + 1) + "."; }
        REPR += MOVETEXT[i] + " ";
        root.update(pply(MOVETEXT[i], root.next, root));
    }

    // Trim REPR to fit in table cell
    if(REPR.size() > sz){ REPR = REPR.substr(0, sz-3) + "..."; }

    // Print board if not standard new game
    if(!FENSTR.empty() || REPR != "/"){ bprint(root); }

    return root;
}

void print_header(){
    if(DET){ tbl::header("Ply", "N", "Captures", "Checks", "Checkmates", "Time"); }
    else   { tab::header("Ply", "N", "Time"); }
}

void print_footer(){
    if(DET){ tbl::header(REPR, RESULTS[0], RESULTS[1], RESULTS[2], RESULTS[3], Tempus::strtime(RESULTS[4])); }
    else   { tab::header(REPR, RESULTS[0], Tempus::strtime(RESULTS[4])); }
}