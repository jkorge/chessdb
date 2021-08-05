#include <csignal>

#include <atomic>
#include <bitset>
#include <future>
#include <iomanip>
#include <iostream>
#include <locale>
#include <string>
#include <thread>
#include <unordered_set>

#include "chessdb.hpp"

/**************************************************
                FUNCTION DECLARATIONS
**************************************************/

// Print a bunch of '-'
inline std::string bar(int length){ return std::string(length, '-'); }

// Print a visualization of a Board
inline void bprint(Board& board){ std::cout << board.to_string() << '\n'; }

// SIGINT handler
void stop(int);

// Select chars from string where bits in bitset are == 1
template<int N>
std::string bitmask_index(const std::string, std::bitset<N>);

/**************************************************
        VARIABLES, STRUCTS, & TYPEDEFS
**************************************************/

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

    stats& operator+=(const stats& other){
        this->operator[](0) += other.at(0);
        this->operator[](1) += other.at(1);
        this->operator[](2) += other.at(2);
        this->operator[](3) += other.at(3);
        this->operator[](4) += other.at(4);
        return *this;
    }
} stats;

// printing utils
struct comma_sep : std::numpunct<char> { string_type do_grouping() const{ return "\3"; } };

template<int N, int W, bool idx=false>
struct table{
    static std::string bar_unit;
    static int index;

    static void _row(){ std::cout << "|\n"; }
    static void rrow(){ std::cout << "|\r"; }

    template<typename T, typename... Ts>
    static void row(T val, Ts... args){
        if(idx){ std::cout << '|' << std::setw(4) << std::left << table<N, W, idx>::index++; }
        table<N, W>::_row(val, args...);
    }

    template<typename T, typename... Ts>
    static void _row(T val, Ts... args){
        std::cout << '|' << std::setw(W) << std::left << val;
        table<N, W>::_row(args...);
    }

    template<typename T, typename... Ts>
    static void header(T val, Ts... args){
        table<N, W, idx>::sep();
        if(idx){ std::cout << '|' << std::setw(4) << std::left << ""; }
        table<N, W, idx>::_row(val, args...);
        table<N, W, idx>::sep();
        table<N, W, idx>::index = 0;
    }

    template<typename T, typename... Ts>
    static void rrow(T val, Ts... args){
        std::cout << '|' << std::setw(W) << std::left << val;
        table<N, W>::rrow(args...);
    }

    static void prow(const ply& p){
        table<N, W>::row(
            color2s(p.c),
            ptype2s(p.type),
            ptype2s(p.promo),
            coord2s(p.src),
            coord2s(p.dst),
            (p.castle ? (p.castle>0 ? "ks" : "qs") : "__"),
            p.capture,
            p.check,
            p.mate
        );
    }

    static void prrow(const ply& p){
        table<N, W>::rrow(
            color2s(p.c),
            ptype2s(p.type),
            ptype2s(p.promo),
            coord2s(p.src),
            coord2s(p.dst),
            (p.castle ? (p.castle>0 ? "ks" : "qs") : "__"),
            p.capture,
            p.check,
            p.mate
        );
    }

    static void sep(){
        std::string tmp(1, '|');
        for(int i=0; i<N; ++i){ tmp += table<N, W, idx>::bar_unit; }
        tmp += bar(idx ? N+4 : N-1) + '|';
        std::cout << tmp << '\n';
    }

    static void sep(int n){
        std::string tmp(1, '|');
        for(int i=0; i<n; ++i){ tmp += table<N, W, idx>::bar_unit; }
        tmp += bar(idx ? n+4 : n-1) + '|';
        std::cout << tmp << '\n';
    }
};

template<int N, int W, bool idx>
std::string table<N, W, idx>::bar_unit = bar(W);

template<int N, int W, bool idx>
int table<N, W, idx>::index = 1;

typedef table<6, 20, true> idtb;
typedef table<6, 20, false> dtb;
typedef table<3, 20, true> itb;
typedef table<3, 20, false> tb;

// SIGINT sets this to true and stops perft recursions
static std::atomic<bool> _RFQ_{false};

// 0ns duration - used with `std::future::wait_for` to check if future object is ready
constexpr Tempus::Nanosecond zns{0};


/**************************************************
                CLASS DECLARATION
**************************************************/
// Recursive engines for perft
uint64_t search(Board, int);
stats searchn(Board, int);

class App{

    // User input string
    std::string cmd;
    std::stringstream sstr;

    // Strings for prompt prefix and help (printed on startup)
    const std::string prompt{"> "},
                      helpstr{
                        "Available commands:\n"
                        "\n\tmove\n\tundo\n\tperft\n\tthreads\n\tnew\n\tshowboard\n\tsetboard\n\texit\n\n"
                        "Type 'help <command>' for more info on each command\n"
                      };

    // Commands for exiting
    const std::unordered_set<std::string> quit_cmds = {"exit", "quit", "x", "q"};

    // Board object for all operations
    Board board;

    // Container for perft results
    stats results = {0};

    // Move history
    std::vector<State> states;
    std::vector<ply> moves,
                     plies;

    // Map commands to functions
    std::map<std::string, std::function<void(App&)> > router;

    // Default-initialized iterator points to one-past-the-end (iterator it == invalid => command not available)
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

    void set_threads();

    void help();

    // Perft entrypoints
    void perft(Board, int, bool, bool);
    void perft1(Board, bool, bool);

    // Print table header/footer for perft results
    void print_header(bool, bool=true);
    void print_footer(bool, bool=true);

    // Parse a SAN ply
    ply pply(std::string, color);
    ply pcastle(bool, color, bool, bool);
    ply prest(const std::string, color, bool, bool);

    template<bool root>
    uint64_t perftq(Board, int, bool);

public:
    App();

    void run();

};