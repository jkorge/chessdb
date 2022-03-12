#include <csignal>

#include <atomic>
#include <bitset>
#include <future>
#include <iostream>
#include <locale>
#include <string>
#include <thread>
#include <unordered_set>

#include "chessdb.hpp"
#include "table.hpp"

/**************************************************
        VARIABLES, STRUCTS, & TYPEDEFS
**************************************************/

// template params => num. columns, column width, include index
typedef table<6, 20, true> idtb;
typedef table<6, 20, false> dtb;
typedef table<3, 20, true> itb;
typedef table<3, 20, false> tb;

// Array subclass for containing perft results
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

// SIGINT sets this to true and stops perft recursions
static std::atomic<bool> _RFQ_{false};

// 0ns duration - used with `std::future::wait_for` to check if future object is ready
constexpr Tempus::Nanosecond zns{0};

/**************************************************
                FUNCTION DECLARATIONS
**************************************************/

// Print a visualization of a Board
inline void bprint(Board& board){ std::cout << board.to_string() << '\n'; }

// SIGINT handler
void stop(int);

// Select chars from string where bits in bitset are == 1
template<int N>
std::string bitmask_index(const std::string, std::bitset<N>);

// Recursive engine for perft
template<typename T>
T search(Board, int);

template<typename T>
void _count(T& res, Board& board, int depth, const ply& p);

template<typename T, typename Function>
void _search(T& res, Board& board, int depth, Function f);

typedef void (*statsFunction)(stats&, Board&, int, const ply&);
typedef void (*intFunction)(uint64_t&, Board&, int, const ply&);

/**************************************************
                CLASS DECLARATION
**************************************************/

class App{

    // User input string
    std::string cmd;
    std::stringstream sstr;

    // Strings for prompt prefix and help (printed on startup)
    const std::string prompt{"> "},
                      helpstr{
                        "Available commands:\n"
                        "\n\tnew\n\tshowboard\n\tsetboard\n\tfen\n\tmove\n\tundo\n\thistory\n\tperft\n\tthreads\n\tclear/cls\n\texit\n\n"
                        "Type 'help <command>' for more info on each command\n"
                      };

    // Commands for exiting
    const std::unordered_set<std::string> quit_cmds = {"exit", "quit", "x", "q"};

    // Board object
    Board board;

    // Container for perft results
    stats results = {0};

    // Move history
    std::vector<State> states;
    std::vector<ply> moves, plies;

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

    void cls();

    void set_threads();

    void help();

    // Perft functions
    void perft(Board, int, bool, bool);
    void perft1(Board, bool, bool);

    // Print table header/footer for perft results
    void print_header(bool, bool=true);
    void print_footer(bool, bool=true);

    // Parse a SAN ply
    ply pply(std::string, color);
    ply pcastle(bool, color, bool, bool);
    ply prest(const std::string, color, bool, bool);

public:
    App();
    ~App();

    void run();

};