#ifndef UTIL_BITBOARD_H
#define UTIL_BITBOARD_H

namespace util{
    namespace bitboard{

        template<typename T>
        U64 attackfrom(const T, ptype, color=NOCOLOR);

        template<typename Ts, typename Td>
        bool colinear(const Ts, const Td);

        template<typename Ts, typename Td>
        ptype linetype(const Ts, const Td);

        ptype linetype(const U64);

        U64 axisalign(const U64);

        template<typename Ts, typename Td>
        U64 linebt(const Ts, const Td, bool=false);

        // String visualization of bitboard
        std::string odisplay(const U64, char='1');

        int bvizidx(square);
        int bbvizidx(square);

        extern const std::array<U64, 64> rmasks;
        extern const std::array<U64, 64> fmasks;
        extern const std::array<U64, 64> dmasks;
        extern const std::array<U64, 64> amasks;

        extern const std::array<U64, 64> wpattack;
        extern const std::array<U64, 64> bpattack;
        extern const std::array<U64, 64> nattack;
        extern const std::array<U64, 64> battack;
        extern const std::array<U64, 64> rattack;
        extern const std::array<U64, 64> qattack;
        extern const std::array<U64, 64> kattack;

        extern const  std::array<std::array<U64, 64>, 64> lines;
        extern const  std::array<std::array<std::array<U64, 8>, 64>, 3> rays;

        constexpr char bcell[]{"[  ]"};
        constexpr char bbcell[]{" . "};
        extern const std::string bviz;
        extern const std::string bbviz;
    }
}

#endif