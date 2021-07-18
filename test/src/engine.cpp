#include <csignal>

#include <atomic>
#include <future>
#include <thread>
#include <unordered_set>

#include "devutil.hpp"

/**************************************************
        VARIABLES, STRUCTS, & TYPEDEFS
**************************************************/

static std::atomic<bool> _RFQ_{false};

constexpr Tempus::Nanosecond zns{0};

// cmd options
std::string REPR{"/"},
            castles{"KQkq"},
            FENSTR;
std::vector<std::string> MOVETEXT;

// maximum search depth
long MAX_DEPTH{1};

// printing utils
typedef table<6, 20, true> idtb;
typedef table<6, 20, false> dtb;
typedef table<3, 20, true> itb;
typedef table<3, 20, false> tb;

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

void stop(int);
stats search(Board, int);
stats searchn(Board, int);
void perft(Board, bool, bool);
// void perftdiv(Board, bool);
void perft1(Board, bool, bool);
void print_header(bool, bool=true);
void print_footer(bool, bool=true);
template<int N>
std::string bitmask_index(std::string, std::bitset<N>);

/**************************************************
                CLASS DECLARATION
**************************************************/

class App{
    // User input string
    std::string cmd;
    std::stringstream sstr;
    const std::string prompt{"> "},
                      helpstr{
                        "Available commands:\n"
                        "\n\tmove\n\tundo\n\tperft\n\tnew\n\tshowboard\n\tsetboard\n\texit\n\n"
                        "Type 'help <command>' for more info on each command\n"
                      };

    Board board;
    std::vector<Board> boards;
    std::vector<ply> moves,
                     plies;
    const std::unordered_set<std::string> quit_cmds = {"exit", "quit", "x", "q"};
    std::map<std::string, std::function<void(App&)> > router;
    std::map<std::string, std::function<void(App&)> >::iterator invalid; 

    template<typename T>
    inline void print(T, bool=true);

    inline void print();

    void move();

    void history();

    void undo();

    void pft();

    void setboard();

    void newgame();

    void show();

    void fen();

    void clear();

    void help();

public:
    App();

    void run();

};

/**************************************************
                        MAIN
**************************************************/

int main(){
    std::cout << std::boolalpha;
    std::cout.imbue(std::locale(std::cout.getloc(), new comma_sep));
    
    std::signal(SIGINT, stop);

    App app;
    app.run();

    return 0;
}

/**************************************************
                FUNCTION DEFINITIONS
**************************************************/

void stop(int sig){ _RFQ_ = true; }

// recurse over legal plies from board position @ root up to MAX_DEPTH
stats search(Board root, int lvl){
    if(_RFQ_){ return {0}; }

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

// search + track extra stats
stats searchn(Board root, int lvl){
    if(_RFQ_){ return {0}; }

    std::vector<ply> plies = root.legal_plies(root.next);
    if(lvl == MAX_DEPTH){
        stats ret = {(uint64_t)plies.size(), 0ULL, 0ULL, 0ULL, 0ULL};
        for(int i=0; i<plies.size(); ++i){
            ret[1] += plies[i].capture;
            ret[2] += plies[i].check;
            ret[3] += plies[i].mate;
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

void perft(Board root, bool det, bool div){
    /*
        det - Toggles aggregation of detailed stats (number of captures, checks, and checkmates)
        div - Toggles displaying perftdiv (divide results according to legal moves available from root)
    */

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
    RESULTS[4] = Tempus::time();
    for(int i=0, j=0; i<N; ++i){
        std::promise<stats> prms;
        futures[i] = prms.get_future();

        if(i >= std::thread::hardware_concurrency()){
            // Wait for a thread to finish
            bool c = false;
            int k = 0;
            do{
                c = std::future_status::ready == futures[k++].wait_for(zns);
                k %= i;
            } while(!c);
        }

        // Construct in-place at back of vector
        threads.emplace_back([prms=std::move(prms), det=det](Board root) mutable{
            uint64_t t0 = Tempus::time();
            stats res = det ? searchn(root, 2) : search(root, 2);
            res[4] = Tempus::time() - t0;
            prms.set_value(res);
        }, successors[i]);
        threads.back().detach();
    }

    // Await results from futures
    for(int i=0; i<N; ++i){ fulfilled[i] = futures[i].get(); }
    RESULTS[4] = Tempus::time() - RESULTS[4];

    if(_RFQ_){ _RFQ_ = false; return; }

    // Print results to console
    print_header(det, div);
    for(int i=0; i<N; ++i){
        if(det){
            // Accumulate results
            RESULTS[0] += fulfilled[i][0];
            RESULTS[1] += fulfilled[i][1];
            RESULTS[2] += fulfilled[i][2];
            RESULTS[3] += fulfilled[i][3];

            // Print
            if(div){ idtb::row(root.ply2san(plies[i]), fulfilled[i][0], fulfilled[i][1], fulfilled[i][2], fulfilled[i][3], Tempus::strtime(fulfilled[i][4])); }
        }
        else{
            // Accumulate results
            RESULTS[0] += fulfilled[i][0];

            // Print
            if(div){ itb::row(root.ply2san(plies[i]), fulfilled[i][0], Tempus::strtime(fulfilled[i][4])); }
        }
    }
    print_footer(det, div);
}

// perftdiv, single-threaded (only goes to depth 1)
void perft1(Board root, bool det, bool div){

    std::vector<uint64_t> times;
    uint64_t t0 = Tempus::time();

    // First level down from root
    std::vector<ply> plies = root.legal_plies(root.next);
    RESULTS[0] = plies.size();
    if(det){
        for(int i=0; i<RESULTS[0]; ++i){
            RESULTS[1] += plies[i].capture;
            RESULTS[2] += plies[i].check;
            RESULTS[3] += plies[i].mate;
            times.emplace_back(Tempus::time() - t0);
        }
    }
    RESULTS[4] = Tempus::time() - t0;

    // Print results to console
    print_header(det, div);
    if(div){
        for(int i=0; i<RESULTS[0]; ++i){
            if(det){ idtb::row(root.ply2san(plies[i]), 1, (int)plies[i].capture, (int)plies[i].check, (int)plies[i].mate, Tempus::strtime(times[i])); }
            else   { itb::row(root.ply2san(plies[i]), 1, Tempus::strtime(Tempus::time() - t0)); }
        }
    }
    print_footer(det, div);
}

void print_header(bool det, bool div){
    if(div){
        if(det){ idtb::header("Ply", "N", "Captures", "Checks", "Checkmates", "Time"); }
        else   { itb::header("Ply", "N", "Time"); }
    }
    else{
        if(det){ dtb::header("Ply", "N", "Captures", "Checks", "Checkmates", "Time"); }
        else   { tb::header("Ply", "N", "Time"); }
    }
}

void print_footer(bool det, bool div){
    if(div){
        if(det){ idtb::header(REPR, RESULTS[0], RESULTS[1], RESULTS[2], RESULTS[3], Tempus::strtime(RESULTS[4])); }
        else   { itb::header(REPR, RESULTS[0], Tempus::strtime(RESULTS[4])); }
    }
    else{
        if(det){ dtb::row(REPR, RESULTS[0], RESULTS[1], RESULTS[2], RESULTS[3], Tempus::strtime(RESULTS[4])); dtb::sep(); }
        else   { tb::row(REPR, RESULTS[0], Tempus::strtime(RESULTS[4])); tb::sep(); }
    }
}

template<int N>
std::string bitmask_index(std::string txt, std::bitset<N> msk){
    std::string res;
    for(int i=0,j=N-1; i<N && j>=0; ++i, --j){ if(msk.test(j)){ res += txt[i]; } }
    return res;
}

/**************************************************
                CLASS DEFINITIONS
**************************************************/

template<typename T>
inline void App::print(T val, bool newln){ std::cout << val << (newln ? '\n' : '\0'); }

inline void App::print(){ std::cout << std::endl; }

void App::move(){
    // SAN move => ply object
    std::string san;
    this->sstr >> san;
    ply p = pply(san, this->board.next, this->board);

    // Check legality
    this->plies = this->board.legal_plies(board.next);
    if(std::find(plies.begin(), plies.end(), p) == plies.end()){
        std::cout << "Illegal move (" << san << ")\n";
    }
    else{

        // Add ply and current board to move history
        this->moves.push_back(p);
        this->boards.push_back(board);

        // Update
        this->board.update(p);

        if(!this->board.legal_plies(board.next).size()){ this->print(color2s(!board.next) + " wins"); }
    }
}

void App::history(){
    if(!this->moves.size()){ this->print(""); }
    else{
        std::string res = (this->boards[0].next == black) ? "... " : "";
        for(int i=0; i<this->moves.size(); ++i){
            if(this->boards[i].next == white){ res += std::to_string(i/2 + 1) + "."; }
            res += this->boards[i].ply2san(this->moves[i]) + " ";
        }
        this->print(res);
    }
}

void App::undo(){
    // Ensure there are older states to move to
    if(!boards.size()){ std::cout << "No move history\n"; }
    else{
        // Default undo last move
        int rem = 1;

        // Get number of moves to undo (limited by move history)
        if(!this->sstr.eof()){ this->sstr >> rem; }
        if(rem < 0){
            // Undo all
            // this->board = this->boards.at(0);
            this->clear();
        }
        else{
            if(rem > this->boards.size()){ rem = this->boards.size() - 1; }
            // Replace board with copy from history
            // this->board = *(this->boards.end() - rem);

            // Remove historical records (no redo)
            for(; rem; --rem){
                this->boards.pop_back();
                this->moves.pop_back();
            }
        }
    }
}

void App::pft(){

    // Get depth (default 1)
    if(!this->sstr.eof()){ this->sstr >> MAX_DEPTH; }
    else                 { MAX_DEPTH = 1; }

    // Check if detailed stats requested
    bool det = this->cmd.back() != 'q';
    bool div = this->cmd.substr(5, 3) == "div";

    if(MAX_DEPTH == 1){ perft1(board, det, div); }
    else              { perft(board, det, div);  }

    RESULTS.fill(0);
}

void App::setboard(){
    // Read FEN string
    std::string fstr;
    std::getline(this->sstr, fstr);
    fstr.erase(0,1);

    // Update board
    fen::parse(fstr, this->board);

    // Clear history
    this->clear();
}

void App::newgame(){
    this->board.reset();
    this->clear();
}

void App::show(){
    // Board
    bprint(this->board);
    // Next to move
    this->print(std::string(1, '[') + color2s(this->board.next) + ']');
    // Castling availability
    std::string txt = "Castling: ";
    if(board.cancas){ txt += bitmask_index<4>(castles, std::bitset<4>(board.cancas)); }
    else            { txt += '-'; }
    this->print(txt);
    // En Passant availability
    txt = "En Passant: ";
    if(board.enpas){ txt += coord2s(board.enpas); }
    else           { txt += '-'; }
    this->print(txt);
    this->print();
}

void App::fen(){
    this->print(fen::from_board(this->board));
}

void App::clear(){
    this->boards.clear();
    this->moves.clear();
}

void App::help(){
    std::string h;
    if(this->sstr.eof()){ h = this->helpstr; }
    else{
        this->sstr >> h;
        if(h == "move"){
            h = "Move material on the board according to a ply written in Standard Algebraic Notation\n"
                 "https://en.wikipedia.org/wiki/Algebraic_notation_(chess)\n"
                 "  Usage: move <ply>\n"
                 "  Example: move e4\n"
                 "           move Nxg6\n"
                 "           move c8=Q+";
        }
        else
        if(h == "undo"){
            h = "Undo the last N moves\n"
                 "Omit N to undo only the most recent move\n"
                 "Use a negative number to undo all moves\n"
                 "Has no effect if no moves have been made since the last call to `new` or `setboard`\n"
                 "  Usage: undo <N>\n"
                 "  Example: undo 3  (undo last 3 moves)\n"
                 "           undo    (undo last move)\n"
                 "           undo -1 (undo all moves)";
        }
        else
        if(h == "perft" || h == "perftq"){
            h = "Searches the game tree from the current position out to N plies and reports stats for all possible outcomes\n"
                 "Use perftq to suppress stats (except the total number of outcomes)"
                 "N > 5 is NOT recommended\n"
                 "  Usage: <perft/perftq> <N>";
        }
        else
        if(h == "new"){
            h = "Reset the board for a new game. Clears move history\n"
                 "  Usage: new";
        }
        else
        if(h == "setboard"){
            h = "Arrange material on the board according to a Forsyth-Edwards Notation record\n"
                 "  Usage: setboard <FEN>\n"
                 "  Example: setboard r1b1k2r/ppp2p1P/2n1p3/3pq3/N2n4/3B2P1/PPPN3P/R2QK2R w KQkq - 1 15";
        }
        else
        if(h == "showboard"){
            h = "Display the current board configuration\n"
                 "  Usage: show";
        }
        else
        if(h == "fen"){
            h = "Get a Forsyth-Edwards Notation record for the current board";
        }
        else
        if(h == "exit" || h == "quit" || h == "x" || h == "q"){
            h = "Close the program";
        }
    }
    this->print(std::string(1, '\n') + h);
    this->print();
}

App::App(){

    this->router["move"]      = &this->move;
    this->router["undo"]      = &this->undo;
    this->router["perft"]     = &this->pft;
    this->router["perftq"]    = &this->pft;
    this->router["perftdiv"]  = &this->pft;
    this->router["perftdivq"] = &this->pft;
    this->router["new"]       = &this->newgame;
    this->router["setboard"]  = &this->setboard;
    this->router["showboard"] = &this->show;
    this->router["fen"]       = &this->fen;
    this->router["history"]   = &this->history;
    this->router["help"]      = &this->help;

    this->invalid = this->router.end();

    this->print(this->helpstr);
    bprint(this->board);
}

void App::run(){
    while(true){

        // Read user input
        this->print(this->prompt, false);
        std::getline(std::cin, this->cmd);
        if(!this->cmd.size()){ continue; }

        // Separate command from args
        this->sstr.clear();
        this->sstr.str(this->cmd);
        this->sstr >> this->cmd;

        // Route to function (or end loop)
        if(this->quit_cmds.find(this->cmd) != this->quit_cmds.end()){ break; }
        else
        if(this->router.find(this->cmd) == this->invalid){
            this->print("Invalid command '", false);
            this->print(this->cmd, false);
            this->print("'");
            continue;
        }
        this->router[this->cmd](*this);
    }
}