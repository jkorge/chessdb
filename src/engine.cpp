#include "include/engine.hpp"

#ifdef _WIN32
std::string _cls("cls");
#else
std::string _cls("clear");
#endif

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

template<int N>
std::string bitmask_index(const std::string txt, std::bitset<N> msk){
    std::string res;
    for(int i=0,j=N-1; i<N && j>=0; ++i, --j){ if(msk.test(j)){ res += txt[i]; } }
    return res;
}

template<>
inline void _count<uint64_t>(uint64_t& res, Board& board, int depth, const ply& p){
    // Count legal moves
    U64 brnk = back_rank(board.next),
        bb, b;

    for(int i=pawn; i<=king; ++i){
        ptype pt = static_cast<ptype>(i);
        bb = board.board(pt, board.next);
        while(bb){
            b = board.legal(lsbpop(bb), pt, board.next);
            res += popcnt(b);
            // Add three for each pawn-promoting move (already counted the move once; +3 for remaining promotions)
            if(!pt && (b &= brnk)){ res += 3*popcnt(b); }
        }
    }
}

template<>
inline void _count<stats>(stats& res, Board& board, int depth, const ply& p){
    ++res[0];
    res[1] += p.capture;
    res[2] += p.check;
    res[3] += p.mate;
}

template<typename T>
inline void _recurse(T& res, Board& board, int depth, const ply& p){
    board.update(p);
    res += search<T>(board, depth);
    board.undo();
}

template<>
inline void _search(stats& res, Board& board, int depth, statsFunction f){
    for(const ply& p : board.legal_plies(board.next)){ f(res, board, depth, p); }
}

template<>
inline void _search(uint64_t& res, Board& board, int depth, intFunction f){
    if(depth <= 1){ f(res, board, depth, ply()); }
    else          { for(const ply& p : board.legal_plies(board.next)){ f(res, board, depth, p); } }
}

template<typename T>
T search(Board board, int depth){
    if(_RFQ_){ return {0}; }
    T res = {0};
    --depth;
    _search(res, board, depth, (depth <= 1) ? _count<T> : _recurse<T>);
    return res;
}

// String constants
const std::string REPR{"/"}, castles{"KQkq"};

// Start with max available number of threads
int NTHREADS = std::thread::hardware_concurrency();

/**************************************************
                CLASS DEFINITIONS
**************************************************/

/*
    PRIVATE
*/

template<typename T>
inline void App::print(T val, bool newln){

    std::cout << val << (newln ? '\n' : '\0');
}

inline void App::print(){

    std::cout << std::endl;
}

void App::move(){

    // SAN move => ply object
    std::string san;
    this->sstr >> san;
    ply p = this->pply(san, this->board.next);

    // Check legality
    this->plies = this->board.legal_plies(board.next);
    if(std::find(plies.begin(), plies.end(), p) == plies.end()){
        std::cout << "Illegal move (" << san << ")\n";
    }
    else{

        // Add ply and current board to move history
        this->moves.push_back(p);
        this->states.push_back(board.state);

        // Update
        this->board.update(p);

        if(!this->board.legal_plies(board.next).size()){ this->print(color2s(!board.next) + " wins"); }
    }
}

void App::history(){
    if(!this->sstr.eof()){
        std::string sub;
        this->sstr >> sub;
        if(sub == "erase" && this->moves.size()){ this->clear(); }
    }
    else
    if(!this->moves.size()){ this->print(""); }
    else{
        std::string res = (this->states[0].next == black) ? "... " : "";
        const State _bstate = this->board.state;
        for(unsigned int i=0; i<this->moves.size(); ++i){
            if(this->states[i].next == white){ res += std::to_string(i/2 + 1) + "."; }
            this->board.state = this->states[i];
            res += this->board.ply2san(this->moves[i]) + " ";
        }
        this->print(res);
        this->board.state = _bstate;
    }
}

void App::undo(){
    // Ensure there are older states to move to
    if(!states.size()){ std::cout << "No move history\n"; }
    else{
        // Default undo last move
        int rem = 1;

        // Get number of moves to undo (limited by move history)
        if(!this->sstr.eof()){ this->sstr >> rem; }
        if(rem < 0){
            // Undo all
            this->board.state = this->states.at(0);
            this->clear();
        }
        else{
            if(rem > this->states.size()){ rem = this->states.size() - 1; }
            // Replace board state with copy from history
            this->board.state = *(this->states.end() - rem);

            // Remove historical records (no redo)
            for(; rem; --rem){
                this->states.pop_back();
                this->moves.pop_back();
            }
        }
    }
}

void App::pft(){

    // Get depth (default 1)
    int depth = 1;
    if(!this->sstr.eof()){ this->sstr >> depth; }

    // Check if detailed stats requested
    bool det = this->cmd.back() != 'q';
    bool div = this->cmd.substr(5, 3) == "div";

    if(depth == 1){ this->perft1(board, det, div); }
    else          { this->perft(board, depth, det, div);  }

    this->results.fill(0);
}

void App::setboard(){
    // Read FEN string
    std::string fstr;
    std::getline(this->sstr, fstr);
    if(fstr.empty()){ this->print("No FEN string provided. Board unchanged"); return; }
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

void App::set_threads(){
    if(this->sstr.eof()){ return; }
    else                { this->sstr >> NTHREADS; }
}

void App::clear(){
    this->states.clear();
    this->moves.clear();
}

void App::cls(){

    std::system(_cls.data());
}

void App::help(){
    std::string h;
    if(this->sstr.eof()){ h = this->helpstr; }
    else{
        this->sstr >> h;
        if(h == "new"){
            h = "Reset the board for a new game. Clears move history\n"
                "  Usage: new";
        }
        else
        if(h == "showboard"){
            h = "Display the current board configuration\n"
                "  Usage: showboard";
        }
        else
        if(h == "setboard"){
            h = "Arrange material on the board according to a Forsyth-Edwards Notation record\n"
                "  Usage: setboard <FEN>\n"
                "  Example: setboard r1b1k2r/ppp2p1P/2n1p3/3pq3/N2n4/3B2P1/PPPN3P/R2QK2R w KQkq - 1 15";
        }
        else
        if(h == "fen"){
            h = "Display a Forsyth-Edwards Notation record for the current board";
        }
        else
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
        if(h == "history"){
            h = "Display a history of moves in Standard Algebraic Notation\n"
                "Use the subcommand `erase` to wipe the current history (does not affect current position)\n"
                "  Usage: history OR history erase\n";
        }
        else
        if(h == "perft" || h == "perftq"){
            h = "Searches the game tree from the current position out to N plies and reports stats for all possible outcomes\n"
                "Use perftq to suppress stats (except the total number of outcomes)"
                "N > 5 is NOT recommended\n"
                "  Usage: <perft/perftq> <N>";
        }
        else
        if(h == "threads"){
            h = "Set the number of threads to use for perft\n"
                "   Example: threads 10 (use 10 threads)\n"
                "            threads 1  (single threaded)";
        }
        else
        if(h == "cls"){
            h = "Clears the screen\n";
        }
        else
        if(h == "exit" || h == "quit" || h == "x" || h == "q"){
            h = "Close the program";
        }
    }
    this->print(std::string(1, '\n') + h);
    this->print();
}

void App::perft(Board root, int depth, bool det, bool div){
    /*
        det - Toggles aggregation of detailed stats (number of captures, checks, and checkmates)
        div - Toggles displaying perftdiv (divide results according to legal moves available from root)
    */

    // Lambda which runs the tree search and sets the value of the associated promise object
    auto func = [det](Board board, int depth, std::promise<stats>&& prms){
        stats res;
        uint64_t t0 = Tempus::time();
        if(det){ res    = search<stats   >(board, depth); }
        else   { res[0] = search<uint64_t>(board, depth); }
        res[4] = Tempus::time() - t0;
        prms.set_value(res);
    };

    // First level down from root
    std::vector<ply> plies = root.legal_plies(root.next);
    std::vector<Board> successors(plies.size(), root);
    for(unsigned int i=0; i<plies.size(); ++i){ successors[i].update(plies[i]); }

    int N = successors.size();

    // Vectors for thread objects and their results
    std::vector<std::thread> threads;
    std::vector<bool> running(N, false);
    std::vector<std::future<stats> > futures(N);
    std::vector<stats> fulfilled(N);
    int k = 0;

    // Create threads
    this->results[4] = Tempus::time();
    for(int i=0; i<N; ++i){
        std::promise<stats> prms;
        futures[i] = prms.get_future();

        if(NTHREADS > 1){
            if(i >= NTHREADS){
                // Wait for a thread to finish
                do{
                    k = 0;
                    for(unsigned int j=0; j<running.size(); ++j){
                        running[j] = running[j] && std::future_status::ready != futures[j].wait_for(zns);
                        k += running[j];
                    }
                } while(k >= NTHREADS);
            }

            // Construct in-place at back of vector
            running[i] = true;
            threads.emplace_back(func, successors[i], depth, std::move(prms));
            threads.back().detach();
        }
        else{ func(successors[i], depth, std::move(prms)); }
    }

    // Await results from futures
    for(int i=0; i<N; ++i){ fulfilled[i] = futures[i].get(); }
    this->results[4] = Tempus::time() - this->results[4];

    if(_RFQ_){ _RFQ_ = false; return; }

    // Print results to console
    this->print_header(det, div);
    for(int i=0; i<N; ++i){
        if(det){
            // Accumulate results
            this->results[0] += fulfilled[i][0];
            this->results[1] += fulfilled[i][1];
            this->results[2] += fulfilled[i][2];
            this->results[3] += fulfilled[i][3];

            // Print
            if(div){ idtb::row(root.ply2san(plies[i]), fulfilled[i][0], fulfilled[i][1], fulfilled[i][2], fulfilled[i][3], Tempus::strtime(fulfilled[i][4])); }
        }
        else{
            // Accumulate results
            this->results[0] += fulfilled[i][0];

            // Print
            if(div){ itb::row(root.ply2san(plies[i]), fulfilled[i][0], Tempus::strtime(fulfilled[i][4])); }
        }
    }
    this->print_footer(det, div);
}

// perft to depth 1 - no threading needed
void App::perft1(Board root, bool det, bool div){

    std::vector<uint64_t> times;
    uint64_t t0 = Tempus::time();

    // First level down from root
    std::vector<ply> plies = root.legal_plies(root.next);
    this->results[0] = plies.size();
    if(det){
        for(const ply& p : plies){
            this->results[1] += p.capture;
            this->results[2] += p.check;
            this->results[3] += p.mate;
            times.emplace_back(Tempus::time() - t0);
        }
    }
    this->results[4] = Tempus::time() - t0;

    // Print results to console
    this->print_header(det, div);
    if(div){
        for(unsigned int i=0; i<this->results[0]; ++i){
            if(det){ idtb::row(root.ply2san(plies[i]), 1, (int)plies[i].capture, (int)plies[i].check, (int)plies[i].mate, Tempus::strtime(times[i])); }
            else   { itb::row(root.ply2san(plies[i]), 1, Tempus::strtime(Tempus::time() - t0)); }
        }
    }
    this->print_footer(det, div);
}

void App::print_header(bool det, bool div){
    if(div){
        if(det){ idtb::header("Ply", "N", "Captures", "Checks", "Checkmates", "Time"); }
        else   { itb::header("Ply", "N", "Time"); }
    }
    else{
        if(det){ dtb::header("Ply", "N", "Captures", "Checks", "Checkmates", "Time"); }
        else   { tb::header("Ply", "N", "Time"); }
    }
}

void App::print_footer(bool det, bool div){
    if(div){
        if(det){ idtb::header(REPR, this->results[0], this->results[1], this->results[2], this->results[3], Tempus::strtime(this->results[4])); }
        else   { itb::header(REPR, this->results[0], Tempus::strtime(this->results[4])); }
    }
    else{
        if(det){ dtb::row(REPR, this->results[0], this->results[1], this->results[2], this->results[3], Tempus::strtime(this->results[4])); dtb::sep(); }
        else   { tb::row(REPR, this->results[0], Tempus::strtime(this->results[4])); tb::sep(); }
    }
}

ply App::pply(std::string p, color c){
    // Parse and remove flags
    bool check = p.back() == '+',
         mate = p.back() == '#';
    if(check || mate){ p.erase(p.size()-1, 1); }
    if(mate){ check = true; }

    bool kingside{!p.compare("O-O")},
         queenside{!p.compare("O-O-O")};

    // Shortcut for castles
    if(kingside | queenside){ return this->pcastle(queenside, c, check, mate); }
    else                    { return this->prest(p, c, check, mate); }
}

ply App::pcastle(bool qs, color c, bool check, bool mate){
    U64 src = this->board.board(king, c),
        dst = qs ? (src >> 2) : (src << 2);
    return {src, dst, king, pawn, c, qs ? -1 : 1, false, check, mate};
}

ply App::prest(const std::string p, color c, bool check, bool mate){
    int f = 0, r = 0;
    square srcsq = 0, dstsq = 0;
    bool capture = false;
    ptype pt = pawn, promo = pawn;

    for(typename std::string::const_reverse_iterator it=p.rbegin(); it!=p.rend(); ++it){
        char ch = *it;
        if(isfile(ch))      { (!f++ ? dstsq : srcsq) += c2file(ch);     }
        else
        if(std::isdigit(ch)){ (!r++ ? dstsq : srcsq) += c2rank(ch) * 8; }
        else
        if(ispiece(ch))     { pt = c2ptype(ch); }
        else
        if(ch == 'x')       { capture = true; }
        else
        if(ch == '=')       { promo = pt; pt = pawn; }
    }
    
    U64 dst = mask(dstsq),
        src = 0;

    if(pt == king)   { src = this->board.board(king ,c); }
    else
    if(r==2 && f==2) { src = mask(srcsq);   }
    else{
        if(f==2)     { src = fileof(srcsq); }
        else
        if(r==2)     { src = rankof(srcsq); }
        
        src = disamb::pgn(src, dst, pt, c, this->board, capture);
    }

    return {src, dst, pt, promo, c, 0, capture, check, mate};
}

/*
    PUBLIC
*/

App::App(){

    this->router["move"]      = &App::move;
    this->router["undo"]      = &App::undo;
    this->router["perft"]     = &App::pft;
    this->router["perftq"]    = &App::pft;
    this->router["perftdiv"]  = &App::pft;
    this->router["perftdivq"] = &App::pft;
    this->router["new"]       = &App::newgame;
    this->router["setboard"]  = &App::setboard;
    this->router["showboard"] = &App::show;
    this->router["fen"]       = &App::fen;
    this->router["history"]   = &App::history;
    this->router["threads"]   = &App::set_threads;
    this->router["cls"]       = &App::cls;
    this->router["help"]      = &App::help;

    this->invalid = this->router.end();

    this->print(this->helpstr);
    bprint(this->board);
}

App::~App() = default;

void App::run(){
    while(true){
        if(_RFQ_){ break; }

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