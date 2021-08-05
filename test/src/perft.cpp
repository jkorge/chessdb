#include "devutil.hpp"

typedef table<3,35,true> tbl;
typedef header<20> hdr;

struct linestat {
    ply p;
    uint64_t nodes;
    uint64_t time;
    linestat(const ply _p, const uint64_t _n, const uint64_t _t) : p(_p), nodes(_n), time(_t) {}
};

typedef std::vector<linestat> stats;

stats RESULTS;

U64 count(Board board){

    U64 res = 0,
        brnk = back_rank(board.next),
        bb, b;

    for(int i=pawn; i<=king; ++i){
        ptype pt = static_cast<ptype>(i);
        bb = board.board(pt, board.next);
        while(bb){
            b = board.legal(lsbpop(bb), pt, board.next);
            res += popcnt(b);
            if(!pt && (b &= brnk)){ res += 3*popcnt(b); }
        }
    }
    return res;
}

template<bool root>
uint64_t perft(Board board, int depth){

    uint64_t cnt, nodes = 0, t0, t1;

    for(const ply& p : board.legal_plies(board.next)){

        if(root){ t0 = Tempus::time(); }

        if(root && depth <= 1){
            cnt = 1;
            ++nodes;
        }
        else{
            board.update(p);
            cnt = (depth == 2) ? count(board) : perft<false>(board, depth - 1);
            nodes += cnt;
            board.undo();
        }

        if(root){
            t1 = Tempus::time();
            RESULTS.emplace_back(p, cnt, t1-t0);
        }
    }

    return nodes;
}

int main(int argc, char** argv){

    std::cout << std::boolalpha;
    std::cout.imbue(std::locale(std::cout.getloc(), new comma_sep));

    int depth = 1;
    for(int i=1; i<argc; ++i){
        if(!std::strcmp("-n", argv[i])){ depth = std::stoi(argv[++i]); }
    }

    Board board;
    uint64_t res = perft<true>(board, depth);
    tbl::header("Ply", "N", "Time");
    uint64_t T = 0;
    for(int i=0; i<RESULTS.size(); ++i){
        tbl::row(board.ply2san(RESULTS[i].p), RESULTS[i].nodes, Tempus::strtime(RESULTS[i].time));
        T += RESULTS[i].time;
    }
    tbl::header("/", res, Tempus::strtime(T));

    return 0;
}
