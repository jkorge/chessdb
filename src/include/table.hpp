#include <iostream>
#include <iomanip>
#include <string>

// printing utils
struct comma_sep : std::numpunct<char> { string_type do_grouping() const{ return "\3"; } };

// Print a bunch of '-'
inline std::string bar(int length){ return std::string(length, '-'); }

template<int W>
struct header{
    static std::string bar_unit;

    template<typename T>
    static void print(T val){
        std::cout << '\n'
                  << header<W>::bar_unit << '\n'
                  << std::setw(W) << std::left
                  << val << '\n'
                  << header<W>::bar_unit << '\n';
    }
};

template<int W>
std::string header<W>::bar_unit = bar(W);

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