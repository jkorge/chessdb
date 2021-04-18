#include <memory>

#include "tempus.hpp"
#include "devutil.hpp"
#include "encode.hpp"
#include "decode.hpp"

Encoder enc;

/*
    GRAPH STRUCTS
        Node contains board and vector of Edge
        Edge sub-classes shared_ptr<Node> and, additionally, contains an eply
*/

struct Edge;
struct Node{
    ChessBoard board;
    std::vector<Edge> edges;
    Node() {}
    Node(ChessBoard b) : board(b) {}
};

typedef std::shared_ptr<Node> node;

struct Edge : node{
    eply ep;
    Edge() : std::shared_ptr<Node>() {}
    Edge(node n, eply via) : node(n), ep(via) {}
    Edge(node n, ply via) : node(n), ep(enc.encode_ply(via)) {}
};

/*
    GAME TREE SEARCH
        DFS search through possible game states
*/
void search_tree(Node&, int=1);

/*
    UTIL VARS
*/
// Trackers
int N_TERMINAL{0},
    N_CAPTURES{0},
    N_CHECKS{0},
    N_MATES{0},
    N_TERMINAL_TOTAL{0},
    N_CAPTURES_TOTAL{0},
    N_CHECKS_TOTAL{0},
    N_MATES_TOTAL{0},
    MAX_DEPTH{1};

// perft vs perftdiv
bool SHOW_SUBSETS{false};

// SAN plies from cli
std::vector<std::string> MOVETEXT;

// FEN string from cli
std::string REPR{"/"}, FENSTR;

// Printing sizes
constexpr int NCOLS{6};
constexpr std::size_t sz{20};

// Table struct typedef
typedef table<sz> tbl;

/******************************************************
*******************************************************
******************************************************/

int main(int argc, char** argv){

    // Setup stdout
    std::cout << std::boolalpha << '\n';
    std::cout.imbue(std::locale(std::cout.getloc(), new comma_sep));

    // cmd args
    for(int i=1; i<argc; ++i){
        if(!std::strcmp(argv[i], "-d")){ MAX_DEPTH = std::stoi(argv[++i]); }
        else
        if(!std::strcmp(argv[i], "-x")){ SHOW_SUBSETS = true; }
        else
        if(!std::strcmp(argv[i], "-m")){
            REPR.clear();
            while(i<argc-1 && argv[i+1][0] != '-'){ MOVETEXT.emplace_back(argv[++i]); }
        }
        else
        if(!std::strcmp(argv[i], "-f")){ FENSTR = argv[++i]; }
    }

    // New Game <=> Root Node
    Node root;

    // Advance game based on cmd args
    if(!FENSTR.empty()){
        Fen fen;
        fen.parse(FENSTR, root.board);
    }
    for(int i=0; i<MOVETEXT.size(); ++i){
        if(white == root.board.next){ REPR += std::to_string(i/2 + 1) + "."; }
        REPR += MOVETEXT[i] + util::constants::ws;
        root.board.update(pply(MOVETEXT[i], root.board.next, root.board));
    }

    if(REPR.size() > sz){ REPR = REPR.substr(0, sz-3) + "..."; }

    unsigned long long t0, t1, T = 0;

    if(!FENSTR.empty() || REPR != "/"){ bprint(root.board); }
    tbl::row("Ply", "N", "Captures", "Checks", "Mates", "Time");
    tbl::sep(NCOLS);

    if(SHOW_SUBSETS){

        // Legal moves from search point
        std::vector<ply> plies = root.board.legal_plies(root.board.next, true);

        for(int i=0; i<plies.size(); ++i){

            // Update copy of board
            ChessBoard b = root.board;
            plies[i].name = b.update(plies[i]);

            // Search from new successor
            if(MAX_DEPTH > 1){
                Node n(b);

                t0 = Tempus::time();
                search_tree(n, 2);
                t1 = Tempus::time();

                // Print and reset for next iter
                T += t1 - t0;
                tbl::row(root.board.ply2san(plies[i]), N_TERMINAL, N_CAPTURES, N_CHECKS, N_MATES, Tempus::strtime(t1 - t0));

                N_TERMINAL_TOTAL += N_TERMINAL;
                N_CAPTURES_TOTAL += N_CAPTURES;
                N_CHECKS_TOTAL += N_CHECKS;
                N_MATES_TOTAL += N_MATES;

                N_TERMINAL = N_CAPTURES = N_CHECKS = N_MATES = 0;
            }
            else{

                if(plies[i].capture){ ++N_CAPTURES; }
                if(plies[i].check)  { ++N_CHECKS; }
                if(plies[i].mate)   { ++N_MATES; }

                tbl::row(root.board.ply2san(plies[i]), N_TERMINAL, N_CAPTURES, N_CHECKS, N_MATES, Tempus::strtime(0));

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

/******************************************************
*******************************************************
******************************************************/

void search_tree(Node& root, int lvl){

    std::vector<ply> plies = root.board.legal_plies(root.board.next, true);
    for(int i=0; i<plies.size(); ++i){

        ChessBoard b = root.board;
        plies[i].name = b.update(plies[i]);

        if(lvl == MAX_DEPTH){
            ++N_TERMINAL;
            if(plies[i].capture){ ++N_CAPTURES; }
            if(plies[i].check)  { ++N_CHECKS; }
            if(plies[i].mate)   { ++N_MATES; }
        }
        else{ Node n(b); search_tree(n, lvl + 1); }
    }
}