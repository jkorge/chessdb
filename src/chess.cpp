#include "include/chess.hpp"

/**************************************************
                GLOBAL CONSTANTS
**************************************************/

namespace {
    template<square sq>
    struct rmask{ const static U64 value = RANK_ << (sq & 56); };

    template<square sq>
    struct fmask{ const static U64 value = FILE_ << (sq & 7); };

    template<square sq>
    struct dmask{
        const static int diag = 8*(sq & 7) - (sq & 56);
        const static int nort = -diag & ( diag >> 31);
        const static int sout =  diag & (-diag >> 31);
        const static U64 value = (MAINDIAG_ >> sout) << nort;
    };

    template<square sq>
    struct amask{
        const static int diag = 56 - 8*(sq & 7) - (sq & 56);
        const static int nort = -diag & ( diag >> 31);
        const static int sout =  diag & (-diag >> 31);
        const static U64 value = (ANTIDIAG_ >> sout) << nort;
    };
}

const std::array<U64, 64> rmasks = {
    rmask< 0>::value, rmask< 1>::value, rmask< 2>::value, rmask< 3>::value, rmask< 4>::value, rmask< 5>::value, rmask< 6>::value, rmask< 7>::value,
    rmask< 8>::value, rmask< 9>::value, rmask<10>::value, rmask<11>::value, rmask<12>::value, rmask<13>::value, rmask<14>::value, rmask<15>::value,
    rmask<16>::value, rmask<17>::value, rmask<18>::value, rmask<19>::value, rmask<20>::value, rmask<21>::value, rmask<22>::value, rmask<23>::value,
    rmask<24>::value, rmask<25>::value, rmask<26>::value, rmask<27>::value, rmask<28>::value, rmask<29>::value, rmask<30>::value, rmask<31>::value,
    rmask<32>::value, rmask<33>::value, rmask<34>::value, rmask<35>::value, rmask<36>::value, rmask<37>::value, rmask<38>::value, rmask<39>::value,
    rmask<40>::value, rmask<41>::value, rmask<42>::value, rmask<43>::value, rmask<44>::value, rmask<45>::value, rmask<46>::value, rmask<47>::value,
    rmask<48>::value, rmask<49>::value, rmask<50>::value, rmask<51>::value, rmask<52>::value, rmask<53>::value, rmask<54>::value, rmask<55>::value,
    rmask<56>::value, rmask<57>::value, rmask<58>::value, rmask<59>::value, rmask<60>::value, rmask<61>::value, rmask<62>::value, rmask<63>::value
};

const std::array<U64, 64> fmasks = {
    fmask< 0>::value, fmask< 1>::value, fmask< 2>::value, fmask< 3>::value, fmask< 4>::value, fmask< 5>::value, fmask< 6>::value, fmask< 7>::value,
    fmask< 8>::value, fmask< 9>::value, fmask<10>::value, fmask<11>::value, fmask<12>::value, fmask<13>::value, fmask<14>::value, fmask<15>::value,
    fmask<16>::value, fmask<17>::value, fmask<18>::value, fmask<19>::value, fmask<20>::value, fmask<21>::value, fmask<22>::value, fmask<23>::value,
    fmask<24>::value, fmask<25>::value, fmask<26>::value, fmask<27>::value, fmask<28>::value, fmask<29>::value, fmask<30>::value, fmask<31>::value,
    fmask<32>::value, fmask<33>::value, fmask<34>::value, fmask<35>::value, fmask<36>::value, fmask<37>::value, fmask<38>::value, fmask<39>::value,
    fmask<40>::value, fmask<41>::value, fmask<42>::value, fmask<43>::value, fmask<44>::value, fmask<45>::value, fmask<46>::value, fmask<47>::value,
    fmask<48>::value, fmask<49>::value, fmask<50>::value, fmask<51>::value, fmask<52>::value, fmask<53>::value, fmask<54>::value, fmask<55>::value,
    fmask<56>::value, fmask<57>::value, fmask<58>::value, fmask<59>::value, fmask<60>::value, fmask<61>::value, fmask<62>::value, fmask<63>::value
};

const std::array<U64, 64> dmasks = {
    dmask< 0>::value, dmask< 1>::value, dmask< 2>::value, dmask< 3>::value, dmask< 4>::value, dmask< 5>::value, dmask< 6>::value, dmask< 7>::value,
    dmask< 8>::value, dmask< 9>::value, dmask<10>::value, dmask<11>::value, dmask<12>::value, dmask<13>::value, dmask<14>::value, dmask<15>::value,
    dmask<16>::value, dmask<17>::value, dmask<18>::value, dmask<19>::value, dmask<20>::value, dmask<21>::value, dmask<22>::value, dmask<23>::value,
    dmask<24>::value, dmask<25>::value, dmask<26>::value, dmask<27>::value, dmask<28>::value, dmask<29>::value, dmask<30>::value, dmask<31>::value,
    dmask<32>::value, dmask<33>::value, dmask<34>::value, dmask<35>::value, dmask<36>::value, dmask<37>::value, dmask<38>::value, dmask<39>::value,
    dmask<40>::value, dmask<41>::value, dmask<42>::value, dmask<43>::value, dmask<44>::value, dmask<45>::value, dmask<46>::value, dmask<47>::value,
    dmask<48>::value, dmask<49>::value, dmask<50>::value, dmask<51>::value, dmask<52>::value, dmask<53>::value, dmask<54>::value, dmask<55>::value,
    dmask<56>::value, dmask<57>::value, dmask<58>::value, dmask<59>::value, dmask<60>::value, dmask<61>::value, dmask<62>::value, dmask<63>::value
};

const std::array<U64, 64> amasks = {
    amask< 0>::value, amask< 1>::value, amask< 2>::value, amask< 3>::value, amask< 4>::value, amask< 5>::value, amask< 6>::value, amask< 7>::value,
    amask< 8>::value, amask< 9>::value, amask<10>::value, amask<11>::value, amask<12>::value, amask<13>::value, amask<14>::value, amask<15>::value,
    amask<16>::value, amask<17>::value, amask<18>::value, amask<19>::value, amask<20>::value, amask<21>::value, amask<22>::value, amask<23>::value,
    amask<24>::value, amask<25>::value, amask<26>::value, amask<27>::value, amask<28>::value, amask<29>::value, amask<30>::value, amask<31>::value,
    amask<32>::value, amask<33>::value, amask<34>::value, amask<35>::value, amask<36>::value, amask<37>::value, amask<38>::value, amask<39>::value,
    amask<40>::value, amask<41>::value, amask<42>::value, amask<43>::value, amask<44>::value, amask<45>::value, amask<46>::value, amask<47>::value,
    amask<48>::value, amask<49>::value, amask<50>::value, amask<51>::value, amask<52>::value, amask<53>::value, amask<54>::value, amask<55>::value,
    amask<56>::value, amask<57>::value, amask<58>::value, amask<59>::value, amask<60>::value, amask<61>::value, amask<62>::value, amask<63>::value
};

namespace {
    template<square ssq, int rs, int fs>
    struct shift{ const static U64 value = ((ssq/8 + rs)<0 || (ssq/8 + rs)>7 || (ssq%8 + fs)<0 || (ssq%8 + fs)>7) ? ZERO_ : (ONE_ << (8*(ssq/8 + rs) + (ssq%8 + fs))); };

    template<square sq>
    struct wpatk{ const static U64 value = (sq < 56) ? (shift<sq, 1, -1>::value | shift<sq, 1,  1>::value) : ZERO_; };

    template<square sq>
    struct bpatk{ const static U64 value = (sq >= 8) ? (shift<sq, -1, -1>::value | shift<sq, -1, 1>::value) : ZERO_; };

    template<square sq>
    struct natk{ const static U64 value = shift<sq, 1,2>::value | shift<sq, 1,-2>::value | shift<sq, -1,2>::value | shift<sq, -1,-2>::value | shift<sq, 2,1>::value | shift<sq, 2,-1>::value | shift<sq, -2,1>::value | shift<sq, -2,-1>::value; };

    template<square sq>
    struct batk{ const static U64 value = (ONE_ << sq) ^ (dmasks[sq] | amasks[sq]); };

    template<square sq>
    struct ratk{ const static U64 value = (ONE_ << sq) ^ (rmasks[sq] | fmasks[sq]); };

    template<square sq>
    struct qatk{ const static U64 value = batk<sq>::value | ratk<sq>::value; };

    template<square sq>
    struct katk{ const static U64 value = shift<sq, -1,-1>::value | shift<sq, -1,0>::value | shift<sq, -1,1>::value | shift<sq, 0,-1>::value | shift<sq, 0,1>::value | shift<sq, 1,-1>::value | shift<sq, 1,0>::value | shift<sq, 1,1>::value; };
}

const std::array<U64, 64> wpattack = {
    wpatk< 0>::value, wpatk< 1>::value, wpatk< 2>::value, wpatk< 3>::value, wpatk< 4>::value, wpatk< 5>::value, wpatk< 6>::value, wpatk< 7>::value,
    wpatk< 8>::value, wpatk< 9>::value, wpatk<10>::value, wpatk<11>::value, wpatk<12>::value, wpatk<13>::value, wpatk<14>::value, wpatk<15>::value,
    wpatk<16>::value, wpatk<17>::value, wpatk<18>::value, wpatk<19>::value, wpatk<20>::value, wpatk<21>::value, wpatk<22>::value, wpatk<23>::value,
    wpatk<24>::value, wpatk<25>::value, wpatk<26>::value, wpatk<27>::value, wpatk<28>::value, wpatk<29>::value, wpatk<30>::value, wpatk<31>::value,
    wpatk<32>::value, wpatk<33>::value, wpatk<34>::value, wpatk<35>::value, wpatk<36>::value, wpatk<37>::value, wpatk<38>::value, wpatk<39>::value,
    wpatk<40>::value, wpatk<41>::value, wpatk<42>::value, wpatk<43>::value, wpatk<44>::value, wpatk<45>::value, wpatk<46>::value, wpatk<47>::value,
    wpatk<48>::value, wpatk<49>::value, wpatk<50>::value, wpatk<51>::value, wpatk<52>::value, wpatk<53>::value, wpatk<54>::value, wpatk<55>::value,
    wpatk<56>::value, wpatk<57>::value, wpatk<58>::value, wpatk<59>::value, wpatk<60>::value, wpatk<61>::value, wpatk<62>::value, wpatk<63>::value
};

const std::array<U64, 64> bpattack = {
    bpatk< 0>::value, bpatk< 1>::value, bpatk< 2>::value, bpatk< 3>::value, bpatk< 4>::value, bpatk< 5>::value, bpatk< 6>::value, bpatk< 7>::value,
    bpatk< 8>::value, bpatk< 9>::value, bpatk<10>::value, bpatk<11>::value, bpatk<12>::value, bpatk<13>::value, bpatk<14>::value, bpatk<15>::value,
    bpatk<16>::value, bpatk<17>::value, bpatk<18>::value, bpatk<19>::value, bpatk<20>::value, bpatk<21>::value, bpatk<22>::value, bpatk<23>::value,
    bpatk<24>::value, bpatk<25>::value, bpatk<26>::value, bpatk<27>::value, bpatk<28>::value, bpatk<29>::value, bpatk<30>::value, bpatk<31>::value,
    bpatk<32>::value, bpatk<33>::value, bpatk<34>::value, bpatk<35>::value, bpatk<36>::value, bpatk<37>::value, bpatk<38>::value, bpatk<39>::value,
    bpatk<40>::value, bpatk<41>::value, bpatk<42>::value, bpatk<43>::value, bpatk<44>::value, bpatk<45>::value, bpatk<46>::value, bpatk<47>::value,
    bpatk<48>::value, bpatk<49>::value, bpatk<50>::value, bpatk<51>::value, bpatk<52>::value, bpatk<53>::value, bpatk<54>::value, bpatk<55>::value,
    bpatk<56>::value, bpatk<57>::value, bpatk<58>::value, bpatk<59>::value, bpatk<60>::value, bpatk<61>::value, bpatk<62>::value, bpatk<63>::value
};

const std::array<U64, 64> nattack = {
    natk< 0>::value, natk< 1>::value, natk< 2>::value, natk< 3>::value, natk< 4>::value, natk< 5>::value, natk< 6>::value, natk< 7>::value,
    natk< 8>::value, natk< 9>::value, natk<10>::value, natk<11>::value, natk<12>::value, natk<13>::value, natk<14>::value, natk<15>::value,
    natk<16>::value, natk<17>::value, natk<18>::value, natk<19>::value, natk<20>::value, natk<21>::value, natk<22>::value, natk<23>::value,
    natk<24>::value, natk<25>::value, natk<26>::value, natk<27>::value, natk<28>::value, natk<29>::value, natk<30>::value, natk<31>::value,
    natk<32>::value, natk<33>::value, natk<34>::value, natk<35>::value, natk<36>::value, natk<37>::value, natk<38>::value, natk<39>::value,
    natk<40>::value, natk<41>::value, natk<42>::value, natk<43>::value, natk<44>::value, natk<45>::value, natk<46>::value, natk<47>::value,
    natk<48>::value, natk<49>::value, natk<50>::value, natk<51>::value, natk<52>::value, natk<53>::value, natk<54>::value, natk<55>::value,
    natk<56>::value, natk<57>::value, natk<58>::value, natk<59>::value, natk<60>::value, natk<61>::value, natk<62>::value, natk<63>::value
};

const std::array<U64, 64> battack = {
    batk< 0>::value, batk< 1>::value, batk< 2>::value, batk< 3>::value, batk< 4>::value, batk< 5>::value, batk< 6>::value, batk< 7>::value,
    batk< 8>::value, batk< 9>::value, batk<10>::value, batk<11>::value, batk<12>::value, batk<13>::value, batk<14>::value, batk<15>::value,
    batk<16>::value, batk<17>::value, batk<18>::value, batk<19>::value, batk<20>::value, batk<21>::value, batk<22>::value, batk<23>::value,
    batk<24>::value, batk<25>::value, batk<26>::value, batk<27>::value, batk<28>::value, batk<29>::value, batk<30>::value, batk<31>::value,
    batk<32>::value, batk<33>::value, batk<34>::value, batk<35>::value, batk<36>::value, batk<37>::value, batk<38>::value, batk<39>::value,
    batk<40>::value, batk<41>::value, batk<42>::value, batk<43>::value, batk<44>::value, batk<45>::value, batk<46>::value, batk<47>::value,
    batk<48>::value, batk<49>::value, batk<50>::value, batk<51>::value, batk<52>::value, batk<53>::value, batk<54>::value, batk<55>::value,
    batk<56>::value, batk<57>::value, batk<58>::value, batk<59>::value, batk<60>::value, batk<61>::value, batk<62>::value, batk<63>::value
};

const std::array<U64, 64> rattack = {
    ratk< 0>::value, ratk< 1>::value, ratk< 2>::value, ratk< 3>::value, ratk< 4>::value, ratk< 5>::value, ratk< 6>::value, ratk< 7>::value,
    ratk< 8>::value, ratk< 9>::value, ratk<10>::value, ratk<11>::value, ratk<12>::value, ratk<13>::value, ratk<14>::value, ratk<15>::value,
    ratk<16>::value, ratk<17>::value, ratk<18>::value, ratk<19>::value, ratk<20>::value, ratk<21>::value, ratk<22>::value, ratk<23>::value,
    ratk<24>::value, ratk<25>::value, ratk<26>::value, ratk<27>::value, ratk<28>::value, ratk<29>::value, ratk<30>::value, ratk<31>::value,
    ratk<32>::value, ratk<33>::value, ratk<34>::value, ratk<35>::value, ratk<36>::value, ratk<37>::value, ratk<38>::value, ratk<39>::value,
    ratk<40>::value, ratk<41>::value, ratk<42>::value, ratk<43>::value, ratk<44>::value, ratk<45>::value, ratk<46>::value, ratk<47>::value,
    ratk<48>::value, ratk<49>::value, ratk<50>::value, ratk<51>::value, ratk<52>::value, ratk<53>::value, ratk<54>::value, ratk<55>::value,
    ratk<56>::value, ratk<57>::value, ratk<58>::value, ratk<59>::value, ratk<60>::value, ratk<61>::value, ratk<62>::value, ratk<63>::value
};

const std::array<U64, 64> qattack = {
    qatk< 0>::value, qatk< 1>::value, qatk< 2>::value, qatk< 3>::value, qatk< 4>::value, qatk< 5>::value, qatk< 6>::value, qatk< 7>::value,
    qatk< 8>::value, qatk< 9>::value, qatk<10>::value, qatk<11>::value, qatk<12>::value, qatk<13>::value, qatk<14>::value, qatk<15>::value,
    qatk<16>::value, qatk<17>::value, qatk<18>::value, qatk<19>::value, qatk<20>::value, qatk<21>::value, qatk<22>::value, qatk<23>::value,
    qatk<24>::value, qatk<25>::value, qatk<26>::value, qatk<27>::value, qatk<28>::value, qatk<29>::value, qatk<30>::value, qatk<31>::value,
    qatk<32>::value, qatk<33>::value, qatk<34>::value, qatk<35>::value, qatk<36>::value, qatk<37>::value, qatk<38>::value, qatk<39>::value,
    qatk<40>::value, qatk<41>::value, qatk<42>::value, qatk<43>::value, qatk<44>::value, qatk<45>::value, qatk<46>::value, qatk<47>::value,
    qatk<48>::value, qatk<49>::value, qatk<50>::value, qatk<51>::value, qatk<52>::value, qatk<53>::value, qatk<54>::value, qatk<55>::value,
    qatk<56>::value, qatk<57>::value, qatk<58>::value, qatk<59>::value, qatk<60>::value, qatk<61>::value, qatk<62>::value, qatk<63>::value
};

const std::array<U64, 64> kattack = {
    katk< 0>::value, katk< 1>::value, katk< 2>::value, katk< 3>::value, katk< 4>::value, katk< 5>::value, katk< 6>::value, katk< 7>::value,
    katk< 8>::value, katk< 9>::value, katk<10>::value, katk<11>::value, katk<12>::value, katk<13>::value, katk<14>::value, katk<15>::value,
    katk<16>::value, katk<17>::value, katk<18>::value, katk<19>::value, katk<20>::value, katk<21>::value, katk<22>::value, katk<23>::value,
    katk<24>::value, katk<25>::value, katk<26>::value, katk<27>::value, katk<28>::value, katk<29>::value, katk<30>::value, katk<31>::value,
    katk<32>::value, katk<33>::value, katk<34>::value, katk<35>::value, katk<36>::value, katk<37>::value, katk<38>::value, katk<39>::value,
    katk<40>::value, katk<41>::value, katk<42>::value, katk<43>::value, katk<44>::value, katk<45>::value, katk<46>::value, katk<47>::value,
    katk<48>::value, katk<49>::value, katk<50>::value, katk<51>::value, katk<52>::value, katk<53>::value, katk<54>::value, katk<55>::value,
    katk<56>::value, katk<57>::value, katk<58>::value, katk<59>::value, katk<60>::value, katk<61>::value, katk<62>::value, katk<63>::value
};

namespace {

    template<square s1, square s2>
    struct _axis{
        const static U64 value = (battack[s1] & ONE_ << s2)
                               ? ((battack[s1] & battack[s2]) | (ONE_ << s1) | (ONE_ << s2))
                               : (
                                    (rattack[s1] & ONE_ << s2)
                                  ? ((rattack[s1] & rattack[s2]) | (ONE_ << s1) | (ONE_ << s2))
                                  : ZERO_
                                 );
    };

    template<square s1, square s2>
    struct _line{
        const static U64 value = (battack[s1] & ONE_ << s2)
                               ? (battack[s1] & battack[s2] & ((ALL_ << s1) ^ (ALL_ << s2)))
                               : (
                                    (rattack[s1] & ONE_ << s2)
                                  ? (rattack[s1] & rattack[s2] & ((ALL_ << s1) ^ (ALL_ << s2)))
                                  : ZERO_
                                 );
    };

    template<ptype pt, square sq, int dir>
    struct _ray{
        const static U64 value = (pt == bishop ? battack[sq] : (pt == rook ? rattack[sq] : qattack[sq]))
                               & (dir%2 ? ((dir-1)%4 ? amasks[sq] : dmasks[sq]) : (dir%4 ? fmasks[sq] :  rmasks[sq]))
                               & (dir>3 ? ALL_ >> (63-sq) : ALL_ << sq);
    };
}

const std::array<std::array<U64, 64>, 64> axes = {
    _axis< 0, 0>::value, _axis< 0, 1>::value, _axis< 0, 2>::value, _axis< 0, 3>::value, _axis< 0, 4>::value, _axis< 0, 5>::value, _axis< 0, 6>::value, _axis< 0, 7>::value,
    _axis< 0, 8>::value, _axis< 0, 9>::value, _axis< 0,10>::value, _axis< 0,11>::value, _axis< 0,12>::value, _axis< 0,13>::value, _axis< 0,14>::value, _axis< 0,15>::value,
    _axis< 0,16>::value, _axis< 0,17>::value, _axis< 0,18>::value, _axis< 0,19>::value, _axis< 0,20>::value, _axis< 0,21>::value, _axis< 0,22>::value, _axis< 0,23>::value,
    _axis< 0,24>::value, _axis< 0,25>::value, _axis< 0,26>::value, _axis< 0,27>::value, _axis< 0,28>::value, _axis< 0,29>::value, _axis< 0,30>::value, _axis< 0,31>::value,
    _axis< 0,32>::value, _axis< 0,33>::value, _axis< 0,34>::value, _axis< 0,35>::value, _axis< 0,36>::value, _axis< 0,37>::value, _axis< 0,38>::value, _axis< 0,39>::value,
    _axis< 0,40>::value, _axis< 0,41>::value, _axis< 0,42>::value, _axis< 0,43>::value, _axis< 0,44>::value, _axis< 0,45>::value, _axis< 0,46>::value, _axis< 0,47>::value,
    _axis< 0,48>::value, _axis< 0,49>::value, _axis< 0,50>::value, _axis< 0,51>::value, _axis< 0,52>::value, _axis< 0,53>::value, _axis< 0,54>::value, _axis< 0,55>::value,
    _axis< 0,56>::value, _axis< 0,57>::value, _axis< 0,58>::value, _axis< 0,59>::value, _axis< 0,60>::value, _axis< 0,61>::value, _axis< 0,62>::value, _axis< 0,63>::value,
    _axis< 1, 0>::value, _axis< 1, 1>::value, _axis< 1, 2>::value, _axis< 1, 3>::value, _axis< 1, 4>::value, _axis< 1, 5>::value, _axis< 1, 6>::value, _axis< 1, 7>::value,
    _axis< 1, 8>::value, _axis< 1, 9>::value, _axis< 1,10>::value, _axis< 1,11>::value, _axis< 1,12>::value, _axis< 1,13>::value, _axis< 1,14>::value, _axis< 1,15>::value,
    _axis< 1,16>::value, _axis< 1,17>::value, _axis< 1,18>::value, _axis< 1,19>::value, _axis< 1,20>::value, _axis< 1,21>::value, _axis< 1,22>::value, _axis< 1,23>::value,
    _axis< 1,24>::value, _axis< 1,25>::value, _axis< 1,26>::value, _axis< 1,27>::value, _axis< 1,28>::value, _axis< 1,29>::value, _axis< 1,30>::value, _axis< 1,31>::value,
    _axis< 1,32>::value, _axis< 1,33>::value, _axis< 1,34>::value, _axis< 1,35>::value, _axis< 1,36>::value, _axis< 1,37>::value, _axis< 1,38>::value, _axis< 1,39>::value,
    _axis< 1,40>::value, _axis< 1,41>::value, _axis< 1,42>::value, _axis< 1,43>::value, _axis< 1,44>::value, _axis< 1,45>::value, _axis< 1,46>::value, _axis< 1,47>::value,
    _axis< 1,48>::value, _axis< 1,49>::value, _axis< 1,50>::value, _axis< 1,51>::value, _axis< 1,52>::value, _axis< 1,53>::value, _axis< 1,54>::value, _axis< 1,55>::value,
    _axis< 1,56>::value, _axis< 1,57>::value, _axis< 1,58>::value, _axis< 1,59>::value, _axis< 1,60>::value, _axis< 1,61>::value, _axis< 1,62>::value, _axis< 1,63>::value,
    _axis< 2, 0>::value, _axis< 2, 1>::value, _axis< 2, 2>::value, _axis< 2, 3>::value, _axis< 2, 4>::value, _axis< 2, 5>::value, _axis< 2, 6>::value, _axis< 2, 7>::value,
    _axis< 2, 8>::value, _axis< 2, 9>::value, _axis< 2,10>::value, _axis< 2,11>::value, _axis< 2,12>::value, _axis< 2,13>::value, _axis< 2,14>::value, _axis< 2,15>::value,
    _axis< 2,16>::value, _axis< 2,17>::value, _axis< 2,18>::value, _axis< 2,19>::value, _axis< 2,20>::value, _axis< 2,21>::value, _axis< 2,22>::value, _axis< 2,23>::value,
    _axis< 2,24>::value, _axis< 2,25>::value, _axis< 2,26>::value, _axis< 2,27>::value, _axis< 2,28>::value, _axis< 2,29>::value, _axis< 2,30>::value, _axis< 2,31>::value,
    _axis< 2,32>::value, _axis< 2,33>::value, _axis< 2,34>::value, _axis< 2,35>::value, _axis< 2,36>::value, _axis< 2,37>::value, _axis< 2,38>::value, _axis< 2,39>::value,
    _axis< 2,40>::value, _axis< 2,41>::value, _axis< 2,42>::value, _axis< 2,43>::value, _axis< 2,44>::value, _axis< 2,45>::value, _axis< 2,46>::value, _axis< 2,47>::value,
    _axis< 2,48>::value, _axis< 2,49>::value, _axis< 2,50>::value, _axis< 2,51>::value, _axis< 2,52>::value, _axis< 2,53>::value, _axis< 2,54>::value, _axis< 2,55>::value,
    _axis< 2,56>::value, _axis< 2,57>::value, _axis< 2,58>::value, _axis< 2,59>::value, _axis< 2,60>::value, _axis< 2,61>::value, _axis< 2,62>::value, _axis< 2,63>::value,
    _axis< 3, 0>::value, _axis< 3, 1>::value, _axis< 3, 2>::value, _axis< 3, 3>::value, _axis< 3, 4>::value, _axis< 3, 5>::value, _axis< 3, 6>::value, _axis< 3, 7>::value,
    _axis< 3, 8>::value, _axis< 3, 9>::value, _axis< 3,10>::value, _axis< 3,11>::value, _axis< 3,12>::value, _axis< 3,13>::value, _axis< 3,14>::value, _axis< 3,15>::value,
    _axis< 3,16>::value, _axis< 3,17>::value, _axis< 3,18>::value, _axis< 3,19>::value, _axis< 3,20>::value, _axis< 3,21>::value, _axis< 3,22>::value, _axis< 3,23>::value,
    _axis< 3,24>::value, _axis< 3,25>::value, _axis< 3,26>::value, _axis< 3,27>::value, _axis< 3,28>::value, _axis< 3,29>::value, _axis< 3,30>::value, _axis< 3,31>::value,
    _axis< 3,32>::value, _axis< 3,33>::value, _axis< 3,34>::value, _axis< 3,35>::value, _axis< 3,36>::value, _axis< 3,37>::value, _axis< 3,38>::value, _axis< 3,39>::value,
    _axis< 3,40>::value, _axis< 3,41>::value, _axis< 3,42>::value, _axis< 3,43>::value, _axis< 3,44>::value, _axis< 3,45>::value, _axis< 3,46>::value, _axis< 3,47>::value,
    _axis< 3,48>::value, _axis< 3,49>::value, _axis< 3,50>::value, _axis< 3,51>::value, _axis< 3,52>::value, _axis< 3,53>::value, _axis< 3,54>::value, _axis< 3,55>::value,
    _axis< 3,56>::value, _axis< 3,57>::value, _axis< 3,58>::value, _axis< 3,59>::value, _axis< 3,60>::value, _axis< 3,61>::value, _axis< 3,62>::value, _axis< 3,63>::value,
    _axis< 4, 0>::value, _axis< 4, 1>::value, _axis< 4, 2>::value, _axis< 4, 3>::value, _axis< 4, 4>::value, _axis< 4, 5>::value, _axis< 4, 6>::value, _axis< 4, 7>::value,
    _axis< 4, 8>::value, _axis< 4, 9>::value, _axis< 4,10>::value, _axis< 4,11>::value, _axis< 4,12>::value, _axis< 4,13>::value, _axis< 4,14>::value, _axis< 4,15>::value,
    _axis< 4,16>::value, _axis< 4,17>::value, _axis< 4,18>::value, _axis< 4,19>::value, _axis< 4,20>::value, _axis< 4,21>::value, _axis< 4,22>::value, _axis< 4,23>::value,
    _axis< 4,24>::value, _axis< 4,25>::value, _axis< 4,26>::value, _axis< 4,27>::value, _axis< 4,28>::value, _axis< 4,29>::value, _axis< 4,30>::value, _axis< 4,31>::value,
    _axis< 4,32>::value, _axis< 4,33>::value, _axis< 4,34>::value, _axis< 4,35>::value, _axis< 4,36>::value, _axis< 4,37>::value, _axis< 4,38>::value, _axis< 4,39>::value,
    _axis< 4,40>::value, _axis< 4,41>::value, _axis< 4,42>::value, _axis< 4,43>::value, _axis< 4,44>::value, _axis< 4,45>::value, _axis< 4,46>::value, _axis< 4,47>::value,
    _axis< 4,48>::value, _axis< 4,49>::value, _axis< 4,50>::value, _axis< 4,51>::value, _axis< 4,52>::value, _axis< 4,53>::value, _axis< 4,54>::value, _axis< 4,55>::value,
    _axis< 4,56>::value, _axis< 4,57>::value, _axis< 4,58>::value, _axis< 4,59>::value, _axis< 4,60>::value, _axis< 4,61>::value, _axis< 4,62>::value, _axis< 4,63>::value,
    _axis< 5, 0>::value, _axis< 5, 1>::value, _axis< 5, 2>::value, _axis< 5, 3>::value, _axis< 5, 4>::value, _axis< 5, 5>::value, _axis< 5, 6>::value, _axis< 5, 7>::value,
    _axis< 5, 8>::value, _axis< 5, 9>::value, _axis< 5,10>::value, _axis< 5,11>::value, _axis< 5,12>::value, _axis< 5,13>::value, _axis< 5,14>::value, _axis< 5,15>::value,
    _axis< 5,16>::value, _axis< 5,17>::value, _axis< 5,18>::value, _axis< 5,19>::value, _axis< 5,20>::value, _axis< 5,21>::value, _axis< 5,22>::value, _axis< 5,23>::value,
    _axis< 5,24>::value, _axis< 5,25>::value, _axis< 5,26>::value, _axis< 5,27>::value, _axis< 5,28>::value, _axis< 5,29>::value, _axis< 5,30>::value, _axis< 5,31>::value,
    _axis< 5,32>::value, _axis< 5,33>::value, _axis< 5,34>::value, _axis< 5,35>::value, _axis< 5,36>::value, _axis< 5,37>::value, _axis< 5,38>::value, _axis< 5,39>::value,
    _axis< 5,40>::value, _axis< 5,41>::value, _axis< 5,42>::value, _axis< 5,43>::value, _axis< 5,44>::value, _axis< 5,45>::value, _axis< 5,46>::value, _axis< 5,47>::value,
    _axis< 5,48>::value, _axis< 5,49>::value, _axis< 5,50>::value, _axis< 5,51>::value, _axis< 5,52>::value, _axis< 5,53>::value, _axis< 5,54>::value, _axis< 5,55>::value,
    _axis< 5,56>::value, _axis< 5,57>::value, _axis< 5,58>::value, _axis< 5,59>::value, _axis< 5,60>::value, _axis< 5,61>::value, _axis< 5,62>::value, _axis< 5,63>::value,
    _axis< 6, 0>::value, _axis< 6, 1>::value, _axis< 6, 2>::value, _axis< 6, 3>::value, _axis< 6, 4>::value, _axis< 6, 5>::value, _axis< 6, 6>::value, _axis< 6, 7>::value,
    _axis< 6, 8>::value, _axis< 6, 9>::value, _axis< 6,10>::value, _axis< 6,11>::value, _axis< 6,12>::value, _axis< 6,13>::value, _axis< 6,14>::value, _axis< 6,15>::value,
    _axis< 6,16>::value, _axis< 6,17>::value, _axis< 6,18>::value, _axis< 6,19>::value, _axis< 6,20>::value, _axis< 6,21>::value, _axis< 6,22>::value, _axis< 6,23>::value,
    _axis< 6,24>::value, _axis< 6,25>::value, _axis< 6,26>::value, _axis< 6,27>::value, _axis< 6,28>::value, _axis< 6,29>::value, _axis< 6,30>::value, _axis< 6,31>::value,
    _axis< 6,32>::value, _axis< 6,33>::value, _axis< 6,34>::value, _axis< 6,35>::value, _axis< 6,36>::value, _axis< 6,37>::value, _axis< 6,38>::value, _axis< 6,39>::value,
    _axis< 6,40>::value, _axis< 6,41>::value, _axis< 6,42>::value, _axis< 6,43>::value, _axis< 6,44>::value, _axis< 6,45>::value, _axis< 6,46>::value, _axis< 6,47>::value,
    _axis< 6,48>::value, _axis< 6,49>::value, _axis< 6,50>::value, _axis< 6,51>::value, _axis< 6,52>::value, _axis< 6,53>::value, _axis< 6,54>::value, _axis< 6,55>::value,
    _axis< 6,56>::value, _axis< 6,57>::value, _axis< 6,58>::value, _axis< 6,59>::value, _axis< 6,60>::value, _axis< 6,61>::value, _axis< 6,62>::value, _axis< 6,63>::value,
    _axis< 7, 0>::value, _axis< 7, 1>::value, _axis< 7, 2>::value, _axis< 7, 3>::value, _axis< 7, 4>::value, _axis< 7, 5>::value, _axis< 7, 6>::value, _axis< 7, 7>::value,
    _axis< 7, 8>::value, _axis< 7, 9>::value, _axis< 7,10>::value, _axis< 7,11>::value, _axis< 7,12>::value, _axis< 7,13>::value, _axis< 7,14>::value, _axis< 7,15>::value,
    _axis< 7,16>::value, _axis< 7,17>::value, _axis< 7,18>::value, _axis< 7,19>::value, _axis< 7,20>::value, _axis< 7,21>::value, _axis< 7,22>::value, _axis< 7,23>::value,
    _axis< 7,24>::value, _axis< 7,25>::value, _axis< 7,26>::value, _axis< 7,27>::value, _axis< 7,28>::value, _axis< 7,29>::value, _axis< 7,30>::value, _axis< 7,31>::value,
    _axis< 7,32>::value, _axis< 7,33>::value, _axis< 7,34>::value, _axis< 7,35>::value, _axis< 7,36>::value, _axis< 7,37>::value, _axis< 7,38>::value, _axis< 7,39>::value,
    _axis< 7,40>::value, _axis< 7,41>::value, _axis< 7,42>::value, _axis< 7,43>::value, _axis< 7,44>::value, _axis< 7,45>::value, _axis< 7,46>::value, _axis< 7,47>::value,
    _axis< 7,48>::value, _axis< 7,49>::value, _axis< 7,50>::value, _axis< 7,51>::value, _axis< 7,52>::value, _axis< 7,53>::value, _axis< 7,54>::value, _axis< 7,55>::value,
    _axis< 7,56>::value, _axis< 7,57>::value, _axis< 7,58>::value, _axis< 7,59>::value, _axis< 7,60>::value, _axis< 7,61>::value, _axis< 7,62>::value, _axis< 7,63>::value,
    _axis< 8, 0>::value, _axis< 8, 1>::value, _axis< 8, 2>::value, _axis< 8, 3>::value, _axis< 8, 4>::value, _axis< 8, 5>::value, _axis< 8, 6>::value, _axis< 8, 7>::value,
    _axis< 8, 8>::value, _axis< 8, 9>::value, _axis< 8,10>::value, _axis< 8,11>::value, _axis< 8,12>::value, _axis< 8,13>::value, _axis< 8,14>::value, _axis< 8,15>::value,
    _axis< 8,16>::value, _axis< 8,17>::value, _axis< 8,18>::value, _axis< 8,19>::value, _axis< 8,20>::value, _axis< 8,21>::value, _axis< 8,22>::value, _axis< 8,23>::value,
    _axis< 8,24>::value, _axis< 8,25>::value, _axis< 8,26>::value, _axis< 8,27>::value, _axis< 8,28>::value, _axis< 8,29>::value, _axis< 8,30>::value, _axis< 8,31>::value,
    _axis< 8,32>::value, _axis< 8,33>::value, _axis< 8,34>::value, _axis< 8,35>::value, _axis< 8,36>::value, _axis< 8,37>::value, _axis< 8,38>::value, _axis< 8,39>::value,
    _axis< 8,40>::value, _axis< 8,41>::value, _axis< 8,42>::value, _axis< 8,43>::value, _axis< 8,44>::value, _axis< 8,45>::value, _axis< 8,46>::value, _axis< 8,47>::value,
    _axis< 8,48>::value, _axis< 8,49>::value, _axis< 8,50>::value, _axis< 8,51>::value, _axis< 8,52>::value, _axis< 8,53>::value, _axis< 8,54>::value, _axis< 8,55>::value,
    _axis< 8,56>::value, _axis< 8,57>::value, _axis< 8,58>::value, _axis< 8,59>::value, _axis< 8,60>::value, _axis< 8,61>::value, _axis< 8,62>::value, _axis< 8,63>::value,
    _axis< 9, 0>::value, _axis< 9, 1>::value, _axis< 9, 2>::value, _axis< 9, 3>::value, _axis< 9, 4>::value, _axis< 9, 5>::value, _axis< 9, 6>::value, _axis< 9, 7>::value,
    _axis< 9, 8>::value, _axis< 9, 9>::value, _axis< 9,10>::value, _axis< 9,11>::value, _axis< 9,12>::value, _axis< 9,13>::value, _axis< 9,14>::value, _axis< 9,15>::value,
    _axis< 9,16>::value, _axis< 9,17>::value, _axis< 9,18>::value, _axis< 9,19>::value, _axis< 9,20>::value, _axis< 9,21>::value, _axis< 9,22>::value, _axis< 9,23>::value,
    _axis< 9,24>::value, _axis< 9,25>::value, _axis< 9,26>::value, _axis< 9,27>::value, _axis< 9,28>::value, _axis< 9,29>::value, _axis< 9,30>::value, _axis< 9,31>::value,
    _axis< 9,32>::value, _axis< 9,33>::value, _axis< 9,34>::value, _axis< 9,35>::value, _axis< 9,36>::value, _axis< 9,37>::value, _axis< 9,38>::value, _axis< 9,39>::value,
    _axis< 9,40>::value, _axis< 9,41>::value, _axis< 9,42>::value, _axis< 9,43>::value, _axis< 9,44>::value, _axis< 9,45>::value, _axis< 9,46>::value, _axis< 9,47>::value,
    _axis< 9,48>::value, _axis< 9,49>::value, _axis< 9,50>::value, _axis< 9,51>::value, _axis< 9,52>::value, _axis< 9,53>::value, _axis< 9,54>::value, _axis< 9,55>::value,
    _axis< 9,56>::value, _axis< 9,57>::value, _axis< 9,58>::value, _axis< 9,59>::value, _axis< 9,60>::value, _axis< 9,61>::value, _axis< 9,62>::value, _axis< 9,63>::value,
    _axis<10, 0>::value, _axis<10, 1>::value, _axis<10, 2>::value, _axis<10, 3>::value, _axis<10, 4>::value, _axis<10, 5>::value, _axis<10, 6>::value, _axis<10, 7>::value,
    _axis<10, 8>::value, _axis<10, 9>::value, _axis<10,10>::value, _axis<10,11>::value, _axis<10,12>::value, _axis<10,13>::value, _axis<10,14>::value, _axis<10,15>::value,
    _axis<10,16>::value, _axis<10,17>::value, _axis<10,18>::value, _axis<10,19>::value, _axis<10,20>::value, _axis<10,21>::value, _axis<10,22>::value, _axis<10,23>::value,
    _axis<10,24>::value, _axis<10,25>::value, _axis<10,26>::value, _axis<10,27>::value, _axis<10,28>::value, _axis<10,29>::value, _axis<10,30>::value, _axis<10,31>::value,
    _axis<10,32>::value, _axis<10,33>::value, _axis<10,34>::value, _axis<10,35>::value, _axis<10,36>::value, _axis<10,37>::value, _axis<10,38>::value, _axis<10,39>::value,
    _axis<10,40>::value, _axis<10,41>::value, _axis<10,42>::value, _axis<10,43>::value, _axis<10,44>::value, _axis<10,45>::value, _axis<10,46>::value, _axis<10,47>::value,
    _axis<10,48>::value, _axis<10,49>::value, _axis<10,50>::value, _axis<10,51>::value, _axis<10,52>::value, _axis<10,53>::value, _axis<10,54>::value, _axis<10,55>::value,
    _axis<10,56>::value, _axis<10,57>::value, _axis<10,58>::value, _axis<10,59>::value, _axis<10,60>::value, _axis<10,61>::value, _axis<10,62>::value, _axis<10,63>::value,
    _axis<11, 0>::value, _axis<11, 1>::value, _axis<11, 2>::value, _axis<11, 3>::value, _axis<11, 4>::value, _axis<11, 5>::value, _axis<11, 6>::value, _axis<11, 7>::value,
    _axis<11, 8>::value, _axis<11, 9>::value, _axis<11,10>::value, _axis<11,11>::value, _axis<11,12>::value, _axis<11,13>::value, _axis<11,14>::value, _axis<11,15>::value,
    _axis<11,16>::value, _axis<11,17>::value, _axis<11,18>::value, _axis<11,19>::value, _axis<11,20>::value, _axis<11,21>::value, _axis<11,22>::value, _axis<11,23>::value,
    _axis<11,24>::value, _axis<11,25>::value, _axis<11,26>::value, _axis<11,27>::value, _axis<11,28>::value, _axis<11,29>::value, _axis<11,30>::value, _axis<11,31>::value,
    _axis<11,32>::value, _axis<11,33>::value, _axis<11,34>::value, _axis<11,35>::value, _axis<11,36>::value, _axis<11,37>::value, _axis<11,38>::value, _axis<11,39>::value,
    _axis<11,40>::value, _axis<11,41>::value, _axis<11,42>::value, _axis<11,43>::value, _axis<11,44>::value, _axis<11,45>::value, _axis<11,46>::value, _axis<11,47>::value,
    _axis<11,48>::value, _axis<11,49>::value, _axis<11,50>::value, _axis<11,51>::value, _axis<11,52>::value, _axis<11,53>::value, _axis<11,54>::value, _axis<11,55>::value,
    _axis<11,56>::value, _axis<11,57>::value, _axis<11,58>::value, _axis<11,59>::value, _axis<11,60>::value, _axis<11,61>::value, _axis<11,62>::value, _axis<11,63>::value,
    _axis<12, 0>::value, _axis<12, 1>::value, _axis<12, 2>::value, _axis<12, 3>::value, _axis<12, 4>::value, _axis<12, 5>::value, _axis<12, 6>::value, _axis<12, 7>::value,
    _axis<12, 8>::value, _axis<12, 9>::value, _axis<12,10>::value, _axis<12,11>::value, _axis<12,12>::value, _axis<12,13>::value, _axis<12,14>::value, _axis<12,15>::value,
    _axis<12,16>::value, _axis<12,17>::value, _axis<12,18>::value, _axis<12,19>::value, _axis<12,20>::value, _axis<12,21>::value, _axis<12,22>::value, _axis<12,23>::value,
    _axis<12,24>::value, _axis<12,25>::value, _axis<12,26>::value, _axis<12,27>::value, _axis<12,28>::value, _axis<12,29>::value, _axis<12,30>::value, _axis<12,31>::value,
    _axis<12,32>::value, _axis<12,33>::value, _axis<12,34>::value, _axis<12,35>::value, _axis<12,36>::value, _axis<12,37>::value, _axis<12,38>::value, _axis<12,39>::value,
    _axis<12,40>::value, _axis<12,41>::value, _axis<12,42>::value, _axis<12,43>::value, _axis<12,44>::value, _axis<12,45>::value, _axis<12,46>::value, _axis<12,47>::value,
    _axis<12,48>::value, _axis<12,49>::value, _axis<12,50>::value, _axis<12,51>::value, _axis<12,52>::value, _axis<12,53>::value, _axis<12,54>::value, _axis<12,55>::value,
    _axis<12,56>::value, _axis<12,57>::value, _axis<12,58>::value, _axis<12,59>::value, _axis<12,60>::value, _axis<12,61>::value, _axis<12,62>::value, _axis<12,63>::value,
    _axis<13, 0>::value, _axis<13, 1>::value, _axis<13, 2>::value, _axis<13, 3>::value, _axis<13, 4>::value, _axis<13, 5>::value, _axis<13, 6>::value, _axis<13, 7>::value,
    _axis<13, 8>::value, _axis<13, 9>::value, _axis<13,10>::value, _axis<13,11>::value, _axis<13,12>::value, _axis<13,13>::value, _axis<13,14>::value, _axis<13,15>::value,
    _axis<13,16>::value, _axis<13,17>::value, _axis<13,18>::value, _axis<13,19>::value, _axis<13,20>::value, _axis<13,21>::value, _axis<13,22>::value, _axis<13,23>::value,
    _axis<13,24>::value, _axis<13,25>::value, _axis<13,26>::value, _axis<13,27>::value, _axis<13,28>::value, _axis<13,29>::value, _axis<13,30>::value, _axis<13,31>::value,
    _axis<13,32>::value, _axis<13,33>::value, _axis<13,34>::value, _axis<13,35>::value, _axis<13,36>::value, _axis<13,37>::value, _axis<13,38>::value, _axis<13,39>::value,
    _axis<13,40>::value, _axis<13,41>::value, _axis<13,42>::value, _axis<13,43>::value, _axis<13,44>::value, _axis<13,45>::value, _axis<13,46>::value, _axis<13,47>::value,
    _axis<13,48>::value, _axis<13,49>::value, _axis<13,50>::value, _axis<13,51>::value, _axis<13,52>::value, _axis<13,53>::value, _axis<13,54>::value, _axis<13,55>::value,
    _axis<13,56>::value, _axis<13,57>::value, _axis<13,58>::value, _axis<13,59>::value, _axis<13,60>::value, _axis<13,61>::value, _axis<13,62>::value, _axis<13,63>::value,
    _axis<14, 0>::value, _axis<14, 1>::value, _axis<14, 2>::value, _axis<14, 3>::value, _axis<14, 4>::value, _axis<14, 5>::value, _axis<14, 6>::value, _axis<14, 7>::value,
    _axis<14, 8>::value, _axis<14, 9>::value, _axis<14,10>::value, _axis<14,11>::value, _axis<14,12>::value, _axis<14,13>::value, _axis<14,14>::value, _axis<14,15>::value,
    _axis<14,16>::value, _axis<14,17>::value, _axis<14,18>::value, _axis<14,19>::value, _axis<14,20>::value, _axis<14,21>::value, _axis<14,22>::value, _axis<14,23>::value,
    _axis<14,24>::value, _axis<14,25>::value, _axis<14,26>::value, _axis<14,27>::value, _axis<14,28>::value, _axis<14,29>::value, _axis<14,30>::value, _axis<14,31>::value,
    _axis<14,32>::value, _axis<14,33>::value, _axis<14,34>::value, _axis<14,35>::value, _axis<14,36>::value, _axis<14,37>::value, _axis<14,38>::value, _axis<14,39>::value,
    _axis<14,40>::value, _axis<14,41>::value, _axis<14,42>::value, _axis<14,43>::value, _axis<14,44>::value, _axis<14,45>::value, _axis<14,46>::value, _axis<14,47>::value,
    _axis<14,48>::value, _axis<14,49>::value, _axis<14,50>::value, _axis<14,51>::value, _axis<14,52>::value, _axis<14,53>::value, _axis<14,54>::value, _axis<14,55>::value,
    _axis<14,56>::value, _axis<14,57>::value, _axis<14,58>::value, _axis<14,59>::value, _axis<14,60>::value, _axis<14,61>::value, _axis<14,62>::value, _axis<14,63>::value,
    _axis<15, 0>::value, _axis<15, 1>::value, _axis<15, 2>::value, _axis<15, 3>::value, _axis<15, 4>::value, _axis<15, 5>::value, _axis<15, 6>::value, _axis<15, 7>::value,
    _axis<15, 8>::value, _axis<15, 9>::value, _axis<15,10>::value, _axis<15,11>::value, _axis<15,12>::value, _axis<15,13>::value, _axis<15,14>::value, _axis<15,15>::value,
    _axis<15,16>::value, _axis<15,17>::value, _axis<15,18>::value, _axis<15,19>::value, _axis<15,20>::value, _axis<15,21>::value, _axis<15,22>::value, _axis<15,23>::value,
    _axis<15,24>::value, _axis<15,25>::value, _axis<15,26>::value, _axis<15,27>::value, _axis<15,28>::value, _axis<15,29>::value, _axis<15,30>::value, _axis<15,31>::value,
    _axis<15,32>::value, _axis<15,33>::value, _axis<15,34>::value, _axis<15,35>::value, _axis<15,36>::value, _axis<15,37>::value, _axis<15,38>::value, _axis<15,39>::value,
    _axis<15,40>::value, _axis<15,41>::value, _axis<15,42>::value, _axis<15,43>::value, _axis<15,44>::value, _axis<15,45>::value, _axis<15,46>::value, _axis<15,47>::value,
    _axis<15,48>::value, _axis<15,49>::value, _axis<15,50>::value, _axis<15,51>::value, _axis<15,52>::value, _axis<15,53>::value, _axis<15,54>::value, _axis<15,55>::value,
    _axis<15,56>::value, _axis<15,57>::value, _axis<15,58>::value, _axis<15,59>::value, _axis<15,60>::value, _axis<15,61>::value, _axis<15,62>::value, _axis<15,63>::value,
    _axis<16, 0>::value, _axis<16, 1>::value, _axis<16, 2>::value, _axis<16, 3>::value, _axis<16, 4>::value, _axis<16, 5>::value, _axis<16, 6>::value, _axis<16, 7>::value,
    _axis<16, 8>::value, _axis<16, 9>::value, _axis<16,10>::value, _axis<16,11>::value, _axis<16,12>::value, _axis<16,13>::value, _axis<16,14>::value, _axis<16,15>::value,
    _axis<16,16>::value, _axis<16,17>::value, _axis<16,18>::value, _axis<16,19>::value, _axis<16,20>::value, _axis<16,21>::value, _axis<16,22>::value, _axis<16,23>::value,
    _axis<16,24>::value, _axis<16,25>::value, _axis<16,26>::value, _axis<16,27>::value, _axis<16,28>::value, _axis<16,29>::value, _axis<16,30>::value, _axis<16,31>::value,
    _axis<16,32>::value, _axis<16,33>::value, _axis<16,34>::value, _axis<16,35>::value, _axis<16,36>::value, _axis<16,37>::value, _axis<16,38>::value, _axis<16,39>::value,
    _axis<16,40>::value, _axis<16,41>::value, _axis<16,42>::value, _axis<16,43>::value, _axis<16,44>::value, _axis<16,45>::value, _axis<16,46>::value, _axis<16,47>::value,
    _axis<16,48>::value, _axis<16,49>::value, _axis<16,50>::value, _axis<16,51>::value, _axis<16,52>::value, _axis<16,53>::value, _axis<16,54>::value, _axis<16,55>::value,
    _axis<16,56>::value, _axis<16,57>::value, _axis<16,58>::value, _axis<16,59>::value, _axis<16,60>::value, _axis<16,61>::value, _axis<16,62>::value, _axis<16,63>::value,
    _axis<17, 0>::value, _axis<17, 1>::value, _axis<17, 2>::value, _axis<17, 3>::value, _axis<17, 4>::value, _axis<17, 5>::value, _axis<17, 6>::value, _axis<17, 7>::value,
    _axis<17, 8>::value, _axis<17, 9>::value, _axis<17,10>::value, _axis<17,11>::value, _axis<17,12>::value, _axis<17,13>::value, _axis<17,14>::value, _axis<17,15>::value,
    _axis<17,16>::value, _axis<17,17>::value, _axis<17,18>::value, _axis<17,19>::value, _axis<17,20>::value, _axis<17,21>::value, _axis<17,22>::value, _axis<17,23>::value,
    _axis<17,24>::value, _axis<17,25>::value, _axis<17,26>::value, _axis<17,27>::value, _axis<17,28>::value, _axis<17,29>::value, _axis<17,30>::value, _axis<17,31>::value,
    _axis<17,32>::value, _axis<17,33>::value, _axis<17,34>::value, _axis<17,35>::value, _axis<17,36>::value, _axis<17,37>::value, _axis<17,38>::value, _axis<17,39>::value,
    _axis<17,40>::value, _axis<17,41>::value, _axis<17,42>::value, _axis<17,43>::value, _axis<17,44>::value, _axis<17,45>::value, _axis<17,46>::value, _axis<17,47>::value,
    _axis<17,48>::value, _axis<17,49>::value, _axis<17,50>::value, _axis<17,51>::value, _axis<17,52>::value, _axis<17,53>::value, _axis<17,54>::value, _axis<17,55>::value,
    _axis<17,56>::value, _axis<17,57>::value, _axis<17,58>::value, _axis<17,59>::value, _axis<17,60>::value, _axis<17,61>::value, _axis<17,62>::value, _axis<17,63>::value,
    _axis<18, 0>::value, _axis<18, 1>::value, _axis<18, 2>::value, _axis<18, 3>::value, _axis<18, 4>::value, _axis<18, 5>::value, _axis<18, 6>::value, _axis<18, 7>::value,
    _axis<18, 8>::value, _axis<18, 9>::value, _axis<18,10>::value, _axis<18,11>::value, _axis<18,12>::value, _axis<18,13>::value, _axis<18,14>::value, _axis<18,15>::value,
    _axis<18,16>::value, _axis<18,17>::value, _axis<18,18>::value, _axis<18,19>::value, _axis<18,20>::value, _axis<18,21>::value, _axis<18,22>::value, _axis<18,23>::value,
    _axis<18,24>::value, _axis<18,25>::value, _axis<18,26>::value, _axis<18,27>::value, _axis<18,28>::value, _axis<18,29>::value, _axis<18,30>::value, _axis<18,31>::value,
    _axis<18,32>::value, _axis<18,33>::value, _axis<18,34>::value, _axis<18,35>::value, _axis<18,36>::value, _axis<18,37>::value, _axis<18,38>::value, _axis<18,39>::value,
    _axis<18,40>::value, _axis<18,41>::value, _axis<18,42>::value, _axis<18,43>::value, _axis<18,44>::value, _axis<18,45>::value, _axis<18,46>::value, _axis<18,47>::value,
    _axis<18,48>::value, _axis<18,49>::value, _axis<18,50>::value, _axis<18,51>::value, _axis<18,52>::value, _axis<18,53>::value, _axis<18,54>::value, _axis<18,55>::value,
    _axis<18,56>::value, _axis<18,57>::value, _axis<18,58>::value, _axis<18,59>::value, _axis<18,60>::value, _axis<18,61>::value, _axis<18,62>::value, _axis<18,63>::value,
    _axis<19, 0>::value, _axis<19, 1>::value, _axis<19, 2>::value, _axis<19, 3>::value, _axis<19, 4>::value, _axis<19, 5>::value, _axis<19, 6>::value, _axis<19, 7>::value,
    _axis<19, 8>::value, _axis<19, 9>::value, _axis<19,10>::value, _axis<19,11>::value, _axis<19,12>::value, _axis<19,13>::value, _axis<19,14>::value, _axis<19,15>::value,
    _axis<19,16>::value, _axis<19,17>::value, _axis<19,18>::value, _axis<19,19>::value, _axis<19,20>::value, _axis<19,21>::value, _axis<19,22>::value, _axis<19,23>::value,
    _axis<19,24>::value, _axis<19,25>::value, _axis<19,26>::value, _axis<19,27>::value, _axis<19,28>::value, _axis<19,29>::value, _axis<19,30>::value, _axis<19,31>::value,
    _axis<19,32>::value, _axis<19,33>::value, _axis<19,34>::value, _axis<19,35>::value, _axis<19,36>::value, _axis<19,37>::value, _axis<19,38>::value, _axis<19,39>::value,
    _axis<19,40>::value, _axis<19,41>::value, _axis<19,42>::value, _axis<19,43>::value, _axis<19,44>::value, _axis<19,45>::value, _axis<19,46>::value, _axis<19,47>::value,
    _axis<19,48>::value, _axis<19,49>::value, _axis<19,50>::value, _axis<19,51>::value, _axis<19,52>::value, _axis<19,53>::value, _axis<19,54>::value, _axis<19,55>::value,
    _axis<19,56>::value, _axis<19,57>::value, _axis<19,58>::value, _axis<19,59>::value, _axis<19,60>::value, _axis<19,61>::value, _axis<19,62>::value, _axis<19,63>::value,
    _axis<20, 0>::value, _axis<20, 1>::value, _axis<20, 2>::value, _axis<20, 3>::value, _axis<20, 4>::value, _axis<20, 5>::value, _axis<20, 6>::value, _axis<20, 7>::value,
    _axis<20, 8>::value, _axis<20, 9>::value, _axis<20,10>::value, _axis<20,11>::value, _axis<20,12>::value, _axis<20,13>::value, _axis<20,14>::value, _axis<20,15>::value,
    _axis<20,16>::value, _axis<20,17>::value, _axis<20,18>::value, _axis<20,19>::value, _axis<20,20>::value, _axis<20,21>::value, _axis<20,22>::value, _axis<20,23>::value,
    _axis<20,24>::value, _axis<20,25>::value, _axis<20,26>::value, _axis<20,27>::value, _axis<20,28>::value, _axis<20,29>::value, _axis<20,30>::value, _axis<20,31>::value,
    _axis<20,32>::value, _axis<20,33>::value, _axis<20,34>::value, _axis<20,35>::value, _axis<20,36>::value, _axis<20,37>::value, _axis<20,38>::value, _axis<20,39>::value,
    _axis<20,40>::value, _axis<20,41>::value, _axis<20,42>::value, _axis<20,43>::value, _axis<20,44>::value, _axis<20,45>::value, _axis<20,46>::value, _axis<20,47>::value,
    _axis<20,48>::value, _axis<20,49>::value, _axis<20,50>::value, _axis<20,51>::value, _axis<20,52>::value, _axis<20,53>::value, _axis<20,54>::value, _axis<20,55>::value,
    _axis<20,56>::value, _axis<20,57>::value, _axis<20,58>::value, _axis<20,59>::value, _axis<20,60>::value, _axis<20,61>::value, _axis<20,62>::value, _axis<20,63>::value,
    _axis<21, 0>::value, _axis<21, 1>::value, _axis<21, 2>::value, _axis<21, 3>::value, _axis<21, 4>::value, _axis<21, 5>::value, _axis<21, 6>::value, _axis<21, 7>::value,
    _axis<21, 8>::value, _axis<21, 9>::value, _axis<21,10>::value, _axis<21,11>::value, _axis<21,12>::value, _axis<21,13>::value, _axis<21,14>::value, _axis<21,15>::value,
    _axis<21,16>::value, _axis<21,17>::value, _axis<21,18>::value, _axis<21,19>::value, _axis<21,20>::value, _axis<21,21>::value, _axis<21,22>::value, _axis<21,23>::value,
    _axis<21,24>::value, _axis<21,25>::value, _axis<21,26>::value, _axis<21,27>::value, _axis<21,28>::value, _axis<21,29>::value, _axis<21,30>::value, _axis<21,31>::value,
    _axis<21,32>::value, _axis<21,33>::value, _axis<21,34>::value, _axis<21,35>::value, _axis<21,36>::value, _axis<21,37>::value, _axis<21,38>::value, _axis<21,39>::value,
    _axis<21,40>::value, _axis<21,41>::value, _axis<21,42>::value, _axis<21,43>::value, _axis<21,44>::value, _axis<21,45>::value, _axis<21,46>::value, _axis<21,47>::value,
    _axis<21,48>::value, _axis<21,49>::value, _axis<21,50>::value, _axis<21,51>::value, _axis<21,52>::value, _axis<21,53>::value, _axis<21,54>::value, _axis<21,55>::value,
    _axis<21,56>::value, _axis<21,57>::value, _axis<21,58>::value, _axis<21,59>::value, _axis<21,60>::value, _axis<21,61>::value, _axis<21,62>::value, _axis<21,63>::value,
    _axis<22, 0>::value, _axis<22, 1>::value, _axis<22, 2>::value, _axis<22, 3>::value, _axis<22, 4>::value, _axis<22, 5>::value, _axis<22, 6>::value, _axis<22, 7>::value,
    _axis<22, 8>::value, _axis<22, 9>::value, _axis<22,10>::value, _axis<22,11>::value, _axis<22,12>::value, _axis<22,13>::value, _axis<22,14>::value, _axis<22,15>::value,
    _axis<22,16>::value, _axis<22,17>::value, _axis<22,18>::value, _axis<22,19>::value, _axis<22,20>::value, _axis<22,21>::value, _axis<22,22>::value, _axis<22,23>::value,
    _axis<22,24>::value, _axis<22,25>::value, _axis<22,26>::value, _axis<22,27>::value, _axis<22,28>::value, _axis<22,29>::value, _axis<22,30>::value, _axis<22,31>::value,
    _axis<22,32>::value, _axis<22,33>::value, _axis<22,34>::value, _axis<22,35>::value, _axis<22,36>::value, _axis<22,37>::value, _axis<22,38>::value, _axis<22,39>::value,
    _axis<22,40>::value, _axis<22,41>::value, _axis<22,42>::value, _axis<22,43>::value, _axis<22,44>::value, _axis<22,45>::value, _axis<22,46>::value, _axis<22,47>::value,
    _axis<22,48>::value, _axis<22,49>::value, _axis<22,50>::value, _axis<22,51>::value, _axis<22,52>::value, _axis<22,53>::value, _axis<22,54>::value, _axis<22,55>::value,
    _axis<22,56>::value, _axis<22,57>::value, _axis<22,58>::value, _axis<22,59>::value, _axis<22,60>::value, _axis<22,61>::value, _axis<22,62>::value, _axis<22,63>::value,
    _axis<23, 0>::value, _axis<23, 1>::value, _axis<23, 2>::value, _axis<23, 3>::value, _axis<23, 4>::value, _axis<23, 5>::value, _axis<23, 6>::value, _axis<23, 7>::value,
    _axis<23, 8>::value, _axis<23, 9>::value, _axis<23,10>::value, _axis<23,11>::value, _axis<23,12>::value, _axis<23,13>::value, _axis<23,14>::value, _axis<23,15>::value,
    _axis<23,16>::value, _axis<23,17>::value, _axis<23,18>::value, _axis<23,19>::value, _axis<23,20>::value, _axis<23,21>::value, _axis<23,22>::value, _axis<23,23>::value,
    _axis<23,24>::value, _axis<23,25>::value, _axis<23,26>::value, _axis<23,27>::value, _axis<23,28>::value, _axis<23,29>::value, _axis<23,30>::value, _axis<23,31>::value,
    _axis<23,32>::value, _axis<23,33>::value, _axis<23,34>::value, _axis<23,35>::value, _axis<23,36>::value, _axis<23,37>::value, _axis<23,38>::value, _axis<23,39>::value,
    _axis<23,40>::value, _axis<23,41>::value, _axis<23,42>::value, _axis<23,43>::value, _axis<23,44>::value, _axis<23,45>::value, _axis<23,46>::value, _axis<23,47>::value,
    _axis<23,48>::value, _axis<23,49>::value, _axis<23,50>::value, _axis<23,51>::value, _axis<23,52>::value, _axis<23,53>::value, _axis<23,54>::value, _axis<23,55>::value,
    _axis<23,56>::value, _axis<23,57>::value, _axis<23,58>::value, _axis<23,59>::value, _axis<23,60>::value, _axis<23,61>::value, _axis<23,62>::value, _axis<23,63>::value,
    _axis<24, 0>::value, _axis<24, 1>::value, _axis<24, 2>::value, _axis<24, 3>::value, _axis<24, 4>::value, _axis<24, 5>::value, _axis<24, 6>::value, _axis<24, 7>::value,
    _axis<24, 8>::value, _axis<24, 9>::value, _axis<24,10>::value, _axis<24,11>::value, _axis<24,12>::value, _axis<24,13>::value, _axis<24,14>::value, _axis<24,15>::value,
    _axis<24,16>::value, _axis<24,17>::value, _axis<24,18>::value, _axis<24,19>::value, _axis<24,20>::value, _axis<24,21>::value, _axis<24,22>::value, _axis<24,23>::value,
    _axis<24,24>::value, _axis<24,25>::value, _axis<24,26>::value, _axis<24,27>::value, _axis<24,28>::value, _axis<24,29>::value, _axis<24,30>::value, _axis<24,31>::value,
    _axis<24,32>::value, _axis<24,33>::value, _axis<24,34>::value, _axis<24,35>::value, _axis<24,36>::value, _axis<24,37>::value, _axis<24,38>::value, _axis<24,39>::value,
    _axis<24,40>::value, _axis<24,41>::value, _axis<24,42>::value, _axis<24,43>::value, _axis<24,44>::value, _axis<24,45>::value, _axis<24,46>::value, _axis<24,47>::value,
    _axis<24,48>::value, _axis<24,49>::value, _axis<24,50>::value, _axis<24,51>::value, _axis<24,52>::value, _axis<24,53>::value, _axis<24,54>::value, _axis<24,55>::value,
    _axis<24,56>::value, _axis<24,57>::value, _axis<24,58>::value, _axis<24,59>::value, _axis<24,60>::value, _axis<24,61>::value, _axis<24,62>::value, _axis<24,63>::value,
    _axis<25, 0>::value, _axis<25, 1>::value, _axis<25, 2>::value, _axis<25, 3>::value, _axis<25, 4>::value, _axis<25, 5>::value, _axis<25, 6>::value, _axis<25, 7>::value,
    _axis<25, 8>::value, _axis<25, 9>::value, _axis<25,10>::value, _axis<25,11>::value, _axis<25,12>::value, _axis<25,13>::value, _axis<25,14>::value, _axis<25,15>::value,
    _axis<25,16>::value, _axis<25,17>::value, _axis<25,18>::value, _axis<25,19>::value, _axis<25,20>::value, _axis<25,21>::value, _axis<25,22>::value, _axis<25,23>::value,
    _axis<25,24>::value, _axis<25,25>::value, _axis<25,26>::value, _axis<25,27>::value, _axis<25,28>::value, _axis<25,29>::value, _axis<25,30>::value, _axis<25,31>::value,
    _axis<25,32>::value, _axis<25,33>::value, _axis<25,34>::value, _axis<25,35>::value, _axis<25,36>::value, _axis<25,37>::value, _axis<25,38>::value, _axis<25,39>::value,
    _axis<25,40>::value, _axis<25,41>::value, _axis<25,42>::value, _axis<25,43>::value, _axis<25,44>::value, _axis<25,45>::value, _axis<25,46>::value, _axis<25,47>::value,
    _axis<25,48>::value, _axis<25,49>::value, _axis<25,50>::value, _axis<25,51>::value, _axis<25,52>::value, _axis<25,53>::value, _axis<25,54>::value, _axis<25,55>::value,
    _axis<25,56>::value, _axis<25,57>::value, _axis<25,58>::value, _axis<25,59>::value, _axis<25,60>::value, _axis<25,61>::value, _axis<25,62>::value, _axis<25,63>::value,
    _axis<26, 0>::value, _axis<26, 1>::value, _axis<26, 2>::value, _axis<26, 3>::value, _axis<26, 4>::value, _axis<26, 5>::value, _axis<26, 6>::value, _axis<26, 7>::value,
    _axis<26, 8>::value, _axis<26, 9>::value, _axis<26,10>::value, _axis<26,11>::value, _axis<26,12>::value, _axis<26,13>::value, _axis<26,14>::value, _axis<26,15>::value,
    _axis<26,16>::value, _axis<26,17>::value, _axis<26,18>::value, _axis<26,19>::value, _axis<26,20>::value, _axis<26,21>::value, _axis<26,22>::value, _axis<26,23>::value,
    _axis<26,24>::value, _axis<26,25>::value, _axis<26,26>::value, _axis<26,27>::value, _axis<26,28>::value, _axis<26,29>::value, _axis<26,30>::value, _axis<26,31>::value,
    _axis<26,32>::value, _axis<26,33>::value, _axis<26,34>::value, _axis<26,35>::value, _axis<26,36>::value, _axis<26,37>::value, _axis<26,38>::value, _axis<26,39>::value,
    _axis<26,40>::value, _axis<26,41>::value, _axis<26,42>::value, _axis<26,43>::value, _axis<26,44>::value, _axis<26,45>::value, _axis<26,46>::value, _axis<26,47>::value,
    _axis<26,48>::value, _axis<26,49>::value, _axis<26,50>::value, _axis<26,51>::value, _axis<26,52>::value, _axis<26,53>::value, _axis<26,54>::value, _axis<26,55>::value,
    _axis<26,56>::value, _axis<26,57>::value, _axis<26,58>::value, _axis<26,59>::value, _axis<26,60>::value, _axis<26,61>::value, _axis<26,62>::value, _axis<26,63>::value,
    _axis<27, 0>::value, _axis<27, 1>::value, _axis<27, 2>::value, _axis<27, 3>::value, _axis<27, 4>::value, _axis<27, 5>::value, _axis<27, 6>::value, _axis<27, 7>::value,
    _axis<27, 8>::value, _axis<27, 9>::value, _axis<27,10>::value, _axis<27,11>::value, _axis<27,12>::value, _axis<27,13>::value, _axis<27,14>::value, _axis<27,15>::value,
    _axis<27,16>::value, _axis<27,17>::value, _axis<27,18>::value, _axis<27,19>::value, _axis<27,20>::value, _axis<27,21>::value, _axis<27,22>::value, _axis<27,23>::value,
    _axis<27,24>::value, _axis<27,25>::value, _axis<27,26>::value, _axis<27,27>::value, _axis<27,28>::value, _axis<27,29>::value, _axis<27,30>::value, _axis<27,31>::value,
    _axis<27,32>::value, _axis<27,33>::value, _axis<27,34>::value, _axis<27,35>::value, _axis<27,36>::value, _axis<27,37>::value, _axis<27,38>::value, _axis<27,39>::value,
    _axis<27,40>::value, _axis<27,41>::value, _axis<27,42>::value, _axis<27,43>::value, _axis<27,44>::value, _axis<27,45>::value, _axis<27,46>::value, _axis<27,47>::value,
    _axis<27,48>::value, _axis<27,49>::value, _axis<27,50>::value, _axis<27,51>::value, _axis<27,52>::value, _axis<27,53>::value, _axis<27,54>::value, _axis<27,55>::value,
    _axis<27,56>::value, _axis<27,57>::value, _axis<27,58>::value, _axis<27,59>::value, _axis<27,60>::value, _axis<27,61>::value, _axis<27,62>::value, _axis<27,63>::value,
    _axis<28, 0>::value, _axis<28, 1>::value, _axis<28, 2>::value, _axis<28, 3>::value, _axis<28, 4>::value, _axis<28, 5>::value, _axis<28, 6>::value, _axis<28, 7>::value,
    _axis<28, 8>::value, _axis<28, 9>::value, _axis<28,10>::value, _axis<28,11>::value, _axis<28,12>::value, _axis<28,13>::value, _axis<28,14>::value, _axis<28,15>::value,
    _axis<28,16>::value, _axis<28,17>::value, _axis<28,18>::value, _axis<28,19>::value, _axis<28,20>::value, _axis<28,21>::value, _axis<28,22>::value, _axis<28,23>::value,
    _axis<28,24>::value, _axis<28,25>::value, _axis<28,26>::value, _axis<28,27>::value, _axis<28,28>::value, _axis<28,29>::value, _axis<28,30>::value, _axis<28,31>::value,
    _axis<28,32>::value, _axis<28,33>::value, _axis<28,34>::value, _axis<28,35>::value, _axis<28,36>::value, _axis<28,37>::value, _axis<28,38>::value, _axis<28,39>::value,
    _axis<28,40>::value, _axis<28,41>::value, _axis<28,42>::value, _axis<28,43>::value, _axis<28,44>::value, _axis<28,45>::value, _axis<28,46>::value, _axis<28,47>::value,
    _axis<28,48>::value, _axis<28,49>::value, _axis<28,50>::value, _axis<28,51>::value, _axis<28,52>::value, _axis<28,53>::value, _axis<28,54>::value, _axis<28,55>::value,
    _axis<28,56>::value, _axis<28,57>::value, _axis<28,58>::value, _axis<28,59>::value, _axis<28,60>::value, _axis<28,61>::value, _axis<28,62>::value, _axis<28,63>::value,
    _axis<29, 0>::value, _axis<29, 1>::value, _axis<29, 2>::value, _axis<29, 3>::value, _axis<29, 4>::value, _axis<29, 5>::value, _axis<29, 6>::value, _axis<29, 7>::value,
    _axis<29, 8>::value, _axis<29, 9>::value, _axis<29,10>::value, _axis<29,11>::value, _axis<29,12>::value, _axis<29,13>::value, _axis<29,14>::value, _axis<29,15>::value,
    _axis<29,16>::value, _axis<29,17>::value, _axis<29,18>::value, _axis<29,19>::value, _axis<29,20>::value, _axis<29,21>::value, _axis<29,22>::value, _axis<29,23>::value,
    _axis<29,24>::value, _axis<29,25>::value, _axis<29,26>::value, _axis<29,27>::value, _axis<29,28>::value, _axis<29,29>::value, _axis<29,30>::value, _axis<29,31>::value,
    _axis<29,32>::value, _axis<29,33>::value, _axis<29,34>::value, _axis<29,35>::value, _axis<29,36>::value, _axis<29,37>::value, _axis<29,38>::value, _axis<29,39>::value,
    _axis<29,40>::value, _axis<29,41>::value, _axis<29,42>::value, _axis<29,43>::value, _axis<29,44>::value, _axis<29,45>::value, _axis<29,46>::value, _axis<29,47>::value,
    _axis<29,48>::value, _axis<29,49>::value, _axis<29,50>::value, _axis<29,51>::value, _axis<29,52>::value, _axis<29,53>::value, _axis<29,54>::value, _axis<29,55>::value,
    _axis<29,56>::value, _axis<29,57>::value, _axis<29,58>::value, _axis<29,59>::value, _axis<29,60>::value, _axis<29,61>::value, _axis<29,62>::value, _axis<29,63>::value,
    _axis<30, 0>::value, _axis<30, 1>::value, _axis<30, 2>::value, _axis<30, 3>::value, _axis<30, 4>::value, _axis<30, 5>::value, _axis<30, 6>::value, _axis<30, 7>::value,
    _axis<30, 8>::value, _axis<30, 9>::value, _axis<30,10>::value, _axis<30,11>::value, _axis<30,12>::value, _axis<30,13>::value, _axis<30,14>::value, _axis<30,15>::value,
    _axis<30,16>::value, _axis<30,17>::value, _axis<30,18>::value, _axis<30,19>::value, _axis<30,20>::value, _axis<30,21>::value, _axis<30,22>::value, _axis<30,23>::value,
    _axis<30,24>::value, _axis<30,25>::value, _axis<30,26>::value, _axis<30,27>::value, _axis<30,28>::value, _axis<30,29>::value, _axis<30,30>::value, _axis<30,31>::value,
    _axis<30,32>::value, _axis<30,33>::value, _axis<30,34>::value, _axis<30,35>::value, _axis<30,36>::value, _axis<30,37>::value, _axis<30,38>::value, _axis<30,39>::value,
    _axis<30,40>::value, _axis<30,41>::value, _axis<30,42>::value, _axis<30,43>::value, _axis<30,44>::value, _axis<30,45>::value, _axis<30,46>::value, _axis<30,47>::value,
    _axis<30,48>::value, _axis<30,49>::value, _axis<30,50>::value, _axis<30,51>::value, _axis<30,52>::value, _axis<30,53>::value, _axis<30,54>::value, _axis<30,55>::value,
    _axis<30,56>::value, _axis<30,57>::value, _axis<30,58>::value, _axis<30,59>::value, _axis<30,60>::value, _axis<30,61>::value, _axis<30,62>::value, _axis<30,63>::value,
    _axis<31, 0>::value, _axis<31, 1>::value, _axis<31, 2>::value, _axis<31, 3>::value, _axis<31, 4>::value, _axis<31, 5>::value, _axis<31, 6>::value, _axis<31, 7>::value,
    _axis<31, 8>::value, _axis<31, 9>::value, _axis<31,10>::value, _axis<31,11>::value, _axis<31,12>::value, _axis<31,13>::value, _axis<31,14>::value, _axis<31,15>::value,
    _axis<31,16>::value, _axis<31,17>::value, _axis<31,18>::value, _axis<31,19>::value, _axis<31,20>::value, _axis<31,21>::value, _axis<31,22>::value, _axis<31,23>::value,
    _axis<31,24>::value, _axis<31,25>::value, _axis<31,26>::value, _axis<31,27>::value, _axis<31,28>::value, _axis<31,29>::value, _axis<31,30>::value, _axis<31,31>::value,
    _axis<31,32>::value, _axis<31,33>::value, _axis<31,34>::value, _axis<31,35>::value, _axis<31,36>::value, _axis<31,37>::value, _axis<31,38>::value, _axis<31,39>::value,
    _axis<31,40>::value, _axis<31,41>::value, _axis<31,42>::value, _axis<31,43>::value, _axis<31,44>::value, _axis<31,45>::value, _axis<31,46>::value, _axis<31,47>::value,
    _axis<31,48>::value, _axis<31,49>::value, _axis<31,50>::value, _axis<31,51>::value, _axis<31,52>::value, _axis<31,53>::value, _axis<31,54>::value, _axis<31,55>::value,
    _axis<31,56>::value, _axis<31,57>::value, _axis<31,58>::value, _axis<31,59>::value, _axis<31,60>::value, _axis<31,61>::value, _axis<31,62>::value, _axis<31,63>::value,
    _axis<32, 0>::value, _axis<32, 1>::value, _axis<32, 2>::value, _axis<32, 3>::value, _axis<32, 4>::value, _axis<32, 5>::value, _axis<32, 6>::value, _axis<32, 7>::value,
    _axis<32, 8>::value, _axis<32, 9>::value, _axis<32,10>::value, _axis<32,11>::value, _axis<32,12>::value, _axis<32,13>::value, _axis<32,14>::value, _axis<32,15>::value,
    _axis<32,16>::value, _axis<32,17>::value, _axis<32,18>::value, _axis<32,19>::value, _axis<32,20>::value, _axis<32,21>::value, _axis<32,22>::value, _axis<32,23>::value,
    _axis<32,24>::value, _axis<32,25>::value, _axis<32,26>::value, _axis<32,27>::value, _axis<32,28>::value, _axis<32,29>::value, _axis<32,30>::value, _axis<32,31>::value,
    _axis<32,32>::value, _axis<32,33>::value, _axis<32,34>::value, _axis<32,35>::value, _axis<32,36>::value, _axis<32,37>::value, _axis<32,38>::value, _axis<32,39>::value,
    _axis<32,40>::value, _axis<32,41>::value, _axis<32,42>::value, _axis<32,43>::value, _axis<32,44>::value, _axis<32,45>::value, _axis<32,46>::value, _axis<32,47>::value,
    _axis<32,48>::value, _axis<32,49>::value, _axis<32,50>::value, _axis<32,51>::value, _axis<32,52>::value, _axis<32,53>::value, _axis<32,54>::value, _axis<32,55>::value,
    _axis<32,56>::value, _axis<32,57>::value, _axis<32,58>::value, _axis<32,59>::value, _axis<32,60>::value, _axis<32,61>::value, _axis<32,62>::value, _axis<32,63>::value,
    _axis<33, 0>::value, _axis<33, 1>::value, _axis<33, 2>::value, _axis<33, 3>::value, _axis<33, 4>::value, _axis<33, 5>::value, _axis<33, 6>::value, _axis<33, 7>::value,
    _axis<33, 8>::value, _axis<33, 9>::value, _axis<33,10>::value, _axis<33,11>::value, _axis<33,12>::value, _axis<33,13>::value, _axis<33,14>::value, _axis<33,15>::value,
    _axis<33,16>::value, _axis<33,17>::value, _axis<33,18>::value, _axis<33,19>::value, _axis<33,20>::value, _axis<33,21>::value, _axis<33,22>::value, _axis<33,23>::value,
    _axis<33,24>::value, _axis<33,25>::value, _axis<33,26>::value, _axis<33,27>::value, _axis<33,28>::value, _axis<33,29>::value, _axis<33,30>::value, _axis<33,31>::value,
    _axis<33,32>::value, _axis<33,33>::value, _axis<33,34>::value, _axis<33,35>::value, _axis<33,36>::value, _axis<33,37>::value, _axis<33,38>::value, _axis<33,39>::value,
    _axis<33,40>::value, _axis<33,41>::value, _axis<33,42>::value, _axis<33,43>::value, _axis<33,44>::value, _axis<33,45>::value, _axis<33,46>::value, _axis<33,47>::value,
    _axis<33,48>::value, _axis<33,49>::value, _axis<33,50>::value, _axis<33,51>::value, _axis<33,52>::value, _axis<33,53>::value, _axis<33,54>::value, _axis<33,55>::value,
    _axis<33,56>::value, _axis<33,57>::value, _axis<33,58>::value, _axis<33,59>::value, _axis<33,60>::value, _axis<33,61>::value, _axis<33,62>::value, _axis<33,63>::value,
    _axis<34, 0>::value, _axis<34, 1>::value, _axis<34, 2>::value, _axis<34, 3>::value, _axis<34, 4>::value, _axis<34, 5>::value, _axis<34, 6>::value, _axis<34, 7>::value,
    _axis<34, 8>::value, _axis<34, 9>::value, _axis<34,10>::value, _axis<34,11>::value, _axis<34,12>::value, _axis<34,13>::value, _axis<34,14>::value, _axis<34,15>::value,
    _axis<34,16>::value, _axis<34,17>::value, _axis<34,18>::value, _axis<34,19>::value, _axis<34,20>::value, _axis<34,21>::value, _axis<34,22>::value, _axis<34,23>::value,
    _axis<34,24>::value, _axis<34,25>::value, _axis<34,26>::value, _axis<34,27>::value, _axis<34,28>::value, _axis<34,29>::value, _axis<34,30>::value, _axis<34,31>::value,
    _axis<34,32>::value, _axis<34,33>::value, _axis<34,34>::value, _axis<34,35>::value, _axis<34,36>::value, _axis<34,37>::value, _axis<34,38>::value, _axis<34,39>::value,
    _axis<34,40>::value, _axis<34,41>::value, _axis<34,42>::value, _axis<34,43>::value, _axis<34,44>::value, _axis<34,45>::value, _axis<34,46>::value, _axis<34,47>::value,
    _axis<34,48>::value, _axis<34,49>::value, _axis<34,50>::value, _axis<34,51>::value, _axis<34,52>::value, _axis<34,53>::value, _axis<34,54>::value, _axis<34,55>::value,
    _axis<34,56>::value, _axis<34,57>::value, _axis<34,58>::value, _axis<34,59>::value, _axis<34,60>::value, _axis<34,61>::value, _axis<34,62>::value, _axis<34,63>::value,
    _axis<35, 0>::value, _axis<35, 1>::value, _axis<35, 2>::value, _axis<35, 3>::value, _axis<35, 4>::value, _axis<35, 5>::value, _axis<35, 6>::value, _axis<35, 7>::value,
    _axis<35, 8>::value, _axis<35, 9>::value, _axis<35,10>::value, _axis<35,11>::value, _axis<35,12>::value, _axis<35,13>::value, _axis<35,14>::value, _axis<35,15>::value,
    _axis<35,16>::value, _axis<35,17>::value, _axis<35,18>::value, _axis<35,19>::value, _axis<35,20>::value, _axis<35,21>::value, _axis<35,22>::value, _axis<35,23>::value,
    _axis<35,24>::value, _axis<35,25>::value, _axis<35,26>::value, _axis<35,27>::value, _axis<35,28>::value, _axis<35,29>::value, _axis<35,30>::value, _axis<35,31>::value,
    _axis<35,32>::value, _axis<35,33>::value, _axis<35,34>::value, _axis<35,35>::value, _axis<35,36>::value, _axis<35,37>::value, _axis<35,38>::value, _axis<35,39>::value,
    _axis<35,40>::value, _axis<35,41>::value, _axis<35,42>::value, _axis<35,43>::value, _axis<35,44>::value, _axis<35,45>::value, _axis<35,46>::value, _axis<35,47>::value,
    _axis<35,48>::value, _axis<35,49>::value, _axis<35,50>::value, _axis<35,51>::value, _axis<35,52>::value, _axis<35,53>::value, _axis<35,54>::value, _axis<35,55>::value,
    _axis<35,56>::value, _axis<35,57>::value, _axis<35,58>::value, _axis<35,59>::value, _axis<35,60>::value, _axis<35,61>::value, _axis<35,62>::value, _axis<35,63>::value,
    _axis<36, 0>::value, _axis<36, 1>::value, _axis<36, 2>::value, _axis<36, 3>::value, _axis<36, 4>::value, _axis<36, 5>::value, _axis<36, 6>::value, _axis<36, 7>::value,
    _axis<36, 8>::value, _axis<36, 9>::value, _axis<36,10>::value, _axis<36,11>::value, _axis<36,12>::value, _axis<36,13>::value, _axis<36,14>::value, _axis<36,15>::value,
    _axis<36,16>::value, _axis<36,17>::value, _axis<36,18>::value, _axis<36,19>::value, _axis<36,20>::value, _axis<36,21>::value, _axis<36,22>::value, _axis<36,23>::value,
    _axis<36,24>::value, _axis<36,25>::value, _axis<36,26>::value, _axis<36,27>::value, _axis<36,28>::value, _axis<36,29>::value, _axis<36,30>::value, _axis<36,31>::value,
    _axis<36,32>::value, _axis<36,33>::value, _axis<36,34>::value, _axis<36,35>::value, _axis<36,36>::value, _axis<36,37>::value, _axis<36,38>::value, _axis<36,39>::value,
    _axis<36,40>::value, _axis<36,41>::value, _axis<36,42>::value, _axis<36,43>::value, _axis<36,44>::value, _axis<36,45>::value, _axis<36,46>::value, _axis<36,47>::value,
    _axis<36,48>::value, _axis<36,49>::value, _axis<36,50>::value, _axis<36,51>::value, _axis<36,52>::value, _axis<36,53>::value, _axis<36,54>::value, _axis<36,55>::value,
    _axis<36,56>::value, _axis<36,57>::value, _axis<36,58>::value, _axis<36,59>::value, _axis<36,60>::value, _axis<36,61>::value, _axis<36,62>::value, _axis<36,63>::value,
    _axis<37, 0>::value, _axis<37, 1>::value, _axis<37, 2>::value, _axis<37, 3>::value, _axis<37, 4>::value, _axis<37, 5>::value, _axis<37, 6>::value, _axis<37, 7>::value,
    _axis<37, 8>::value, _axis<37, 9>::value, _axis<37,10>::value, _axis<37,11>::value, _axis<37,12>::value, _axis<37,13>::value, _axis<37,14>::value, _axis<37,15>::value,
    _axis<37,16>::value, _axis<37,17>::value, _axis<37,18>::value, _axis<37,19>::value, _axis<37,20>::value, _axis<37,21>::value, _axis<37,22>::value, _axis<37,23>::value,
    _axis<37,24>::value, _axis<37,25>::value, _axis<37,26>::value, _axis<37,27>::value, _axis<37,28>::value, _axis<37,29>::value, _axis<37,30>::value, _axis<37,31>::value,
    _axis<37,32>::value, _axis<37,33>::value, _axis<37,34>::value, _axis<37,35>::value, _axis<37,36>::value, _axis<37,37>::value, _axis<37,38>::value, _axis<37,39>::value,
    _axis<37,40>::value, _axis<37,41>::value, _axis<37,42>::value, _axis<37,43>::value, _axis<37,44>::value, _axis<37,45>::value, _axis<37,46>::value, _axis<37,47>::value,
    _axis<37,48>::value, _axis<37,49>::value, _axis<37,50>::value, _axis<37,51>::value, _axis<37,52>::value, _axis<37,53>::value, _axis<37,54>::value, _axis<37,55>::value,
    _axis<37,56>::value, _axis<37,57>::value, _axis<37,58>::value, _axis<37,59>::value, _axis<37,60>::value, _axis<37,61>::value, _axis<37,62>::value, _axis<37,63>::value,
    _axis<38, 0>::value, _axis<38, 1>::value, _axis<38, 2>::value, _axis<38, 3>::value, _axis<38, 4>::value, _axis<38, 5>::value, _axis<38, 6>::value, _axis<38, 7>::value,
    _axis<38, 8>::value, _axis<38, 9>::value, _axis<38,10>::value, _axis<38,11>::value, _axis<38,12>::value, _axis<38,13>::value, _axis<38,14>::value, _axis<38,15>::value,
    _axis<38,16>::value, _axis<38,17>::value, _axis<38,18>::value, _axis<38,19>::value, _axis<38,20>::value, _axis<38,21>::value, _axis<38,22>::value, _axis<38,23>::value,
    _axis<38,24>::value, _axis<38,25>::value, _axis<38,26>::value, _axis<38,27>::value, _axis<38,28>::value, _axis<38,29>::value, _axis<38,30>::value, _axis<38,31>::value,
    _axis<38,32>::value, _axis<38,33>::value, _axis<38,34>::value, _axis<38,35>::value, _axis<38,36>::value, _axis<38,37>::value, _axis<38,38>::value, _axis<38,39>::value,
    _axis<38,40>::value, _axis<38,41>::value, _axis<38,42>::value, _axis<38,43>::value, _axis<38,44>::value, _axis<38,45>::value, _axis<38,46>::value, _axis<38,47>::value,
    _axis<38,48>::value, _axis<38,49>::value, _axis<38,50>::value, _axis<38,51>::value, _axis<38,52>::value, _axis<38,53>::value, _axis<38,54>::value, _axis<38,55>::value,
    _axis<38,56>::value, _axis<38,57>::value, _axis<38,58>::value, _axis<38,59>::value, _axis<38,60>::value, _axis<38,61>::value, _axis<38,62>::value, _axis<38,63>::value,
    _axis<39, 0>::value, _axis<39, 1>::value, _axis<39, 2>::value, _axis<39, 3>::value, _axis<39, 4>::value, _axis<39, 5>::value, _axis<39, 6>::value, _axis<39, 7>::value,
    _axis<39, 8>::value, _axis<39, 9>::value, _axis<39,10>::value, _axis<39,11>::value, _axis<39,12>::value, _axis<39,13>::value, _axis<39,14>::value, _axis<39,15>::value,
    _axis<39,16>::value, _axis<39,17>::value, _axis<39,18>::value, _axis<39,19>::value, _axis<39,20>::value, _axis<39,21>::value, _axis<39,22>::value, _axis<39,23>::value,
    _axis<39,24>::value, _axis<39,25>::value, _axis<39,26>::value, _axis<39,27>::value, _axis<39,28>::value, _axis<39,29>::value, _axis<39,30>::value, _axis<39,31>::value,
    _axis<39,32>::value, _axis<39,33>::value, _axis<39,34>::value, _axis<39,35>::value, _axis<39,36>::value, _axis<39,37>::value, _axis<39,38>::value, _axis<39,39>::value,
    _axis<39,40>::value, _axis<39,41>::value, _axis<39,42>::value, _axis<39,43>::value, _axis<39,44>::value, _axis<39,45>::value, _axis<39,46>::value, _axis<39,47>::value,
    _axis<39,48>::value, _axis<39,49>::value, _axis<39,50>::value, _axis<39,51>::value, _axis<39,52>::value, _axis<39,53>::value, _axis<39,54>::value, _axis<39,55>::value,
    _axis<39,56>::value, _axis<39,57>::value, _axis<39,58>::value, _axis<39,59>::value, _axis<39,60>::value, _axis<39,61>::value, _axis<39,62>::value, _axis<39,63>::value,
    _axis<40, 0>::value, _axis<40, 1>::value, _axis<40, 2>::value, _axis<40, 3>::value, _axis<40, 4>::value, _axis<40, 5>::value, _axis<40, 6>::value, _axis<40, 7>::value,
    _axis<40, 8>::value, _axis<40, 9>::value, _axis<40,10>::value, _axis<40,11>::value, _axis<40,12>::value, _axis<40,13>::value, _axis<40,14>::value, _axis<40,15>::value,
    _axis<40,16>::value, _axis<40,17>::value, _axis<40,18>::value, _axis<40,19>::value, _axis<40,20>::value, _axis<40,21>::value, _axis<40,22>::value, _axis<40,23>::value,
    _axis<40,24>::value, _axis<40,25>::value, _axis<40,26>::value, _axis<40,27>::value, _axis<40,28>::value, _axis<40,29>::value, _axis<40,30>::value, _axis<40,31>::value,
    _axis<40,32>::value, _axis<40,33>::value, _axis<40,34>::value, _axis<40,35>::value, _axis<40,36>::value, _axis<40,37>::value, _axis<40,38>::value, _axis<40,39>::value,
    _axis<40,40>::value, _axis<40,41>::value, _axis<40,42>::value, _axis<40,43>::value, _axis<40,44>::value, _axis<40,45>::value, _axis<40,46>::value, _axis<40,47>::value,
    _axis<40,48>::value, _axis<40,49>::value, _axis<40,50>::value, _axis<40,51>::value, _axis<40,52>::value, _axis<40,53>::value, _axis<40,54>::value, _axis<40,55>::value,
    _axis<40,56>::value, _axis<40,57>::value, _axis<40,58>::value, _axis<40,59>::value, _axis<40,60>::value, _axis<40,61>::value, _axis<40,62>::value, _axis<40,63>::value,
    _axis<41, 0>::value, _axis<41, 1>::value, _axis<41, 2>::value, _axis<41, 3>::value, _axis<41, 4>::value, _axis<41, 5>::value, _axis<41, 6>::value, _axis<41, 7>::value,
    _axis<41, 8>::value, _axis<41, 9>::value, _axis<41,10>::value, _axis<41,11>::value, _axis<41,12>::value, _axis<41,13>::value, _axis<41,14>::value, _axis<41,15>::value,
    _axis<41,16>::value, _axis<41,17>::value, _axis<41,18>::value, _axis<41,19>::value, _axis<41,20>::value, _axis<41,21>::value, _axis<41,22>::value, _axis<41,23>::value,
    _axis<41,24>::value, _axis<41,25>::value, _axis<41,26>::value, _axis<41,27>::value, _axis<41,28>::value, _axis<41,29>::value, _axis<41,30>::value, _axis<41,31>::value,
    _axis<41,32>::value, _axis<41,33>::value, _axis<41,34>::value, _axis<41,35>::value, _axis<41,36>::value, _axis<41,37>::value, _axis<41,38>::value, _axis<41,39>::value,
    _axis<41,40>::value, _axis<41,41>::value, _axis<41,42>::value, _axis<41,43>::value, _axis<41,44>::value, _axis<41,45>::value, _axis<41,46>::value, _axis<41,47>::value,
    _axis<41,48>::value, _axis<41,49>::value, _axis<41,50>::value, _axis<41,51>::value, _axis<41,52>::value, _axis<41,53>::value, _axis<41,54>::value, _axis<41,55>::value,
    _axis<41,56>::value, _axis<41,57>::value, _axis<41,58>::value, _axis<41,59>::value, _axis<41,60>::value, _axis<41,61>::value, _axis<41,62>::value, _axis<41,63>::value,
    _axis<42, 0>::value, _axis<42, 1>::value, _axis<42, 2>::value, _axis<42, 3>::value, _axis<42, 4>::value, _axis<42, 5>::value, _axis<42, 6>::value, _axis<42, 7>::value,
    _axis<42, 8>::value, _axis<42, 9>::value, _axis<42,10>::value, _axis<42,11>::value, _axis<42,12>::value, _axis<42,13>::value, _axis<42,14>::value, _axis<42,15>::value,
    _axis<42,16>::value, _axis<42,17>::value, _axis<42,18>::value, _axis<42,19>::value, _axis<42,20>::value, _axis<42,21>::value, _axis<42,22>::value, _axis<42,23>::value,
    _axis<42,24>::value, _axis<42,25>::value, _axis<42,26>::value, _axis<42,27>::value, _axis<42,28>::value, _axis<42,29>::value, _axis<42,30>::value, _axis<42,31>::value,
    _axis<42,32>::value, _axis<42,33>::value, _axis<42,34>::value, _axis<42,35>::value, _axis<42,36>::value, _axis<42,37>::value, _axis<42,38>::value, _axis<42,39>::value,
    _axis<42,40>::value, _axis<42,41>::value, _axis<42,42>::value, _axis<42,43>::value, _axis<42,44>::value, _axis<42,45>::value, _axis<42,46>::value, _axis<42,47>::value,
    _axis<42,48>::value, _axis<42,49>::value, _axis<42,50>::value, _axis<42,51>::value, _axis<42,52>::value, _axis<42,53>::value, _axis<42,54>::value, _axis<42,55>::value,
    _axis<42,56>::value, _axis<42,57>::value, _axis<42,58>::value, _axis<42,59>::value, _axis<42,60>::value, _axis<42,61>::value, _axis<42,62>::value, _axis<42,63>::value,
    _axis<43, 0>::value, _axis<43, 1>::value, _axis<43, 2>::value, _axis<43, 3>::value, _axis<43, 4>::value, _axis<43, 5>::value, _axis<43, 6>::value, _axis<43, 7>::value,
    _axis<43, 8>::value, _axis<43, 9>::value, _axis<43,10>::value, _axis<43,11>::value, _axis<43,12>::value, _axis<43,13>::value, _axis<43,14>::value, _axis<43,15>::value,
    _axis<43,16>::value, _axis<43,17>::value, _axis<43,18>::value, _axis<43,19>::value, _axis<43,20>::value, _axis<43,21>::value, _axis<43,22>::value, _axis<43,23>::value,
    _axis<43,24>::value, _axis<43,25>::value, _axis<43,26>::value, _axis<43,27>::value, _axis<43,28>::value, _axis<43,29>::value, _axis<43,30>::value, _axis<43,31>::value,
    _axis<43,32>::value, _axis<43,33>::value, _axis<43,34>::value, _axis<43,35>::value, _axis<43,36>::value, _axis<43,37>::value, _axis<43,38>::value, _axis<43,39>::value,
    _axis<43,40>::value, _axis<43,41>::value, _axis<43,42>::value, _axis<43,43>::value, _axis<43,44>::value, _axis<43,45>::value, _axis<43,46>::value, _axis<43,47>::value,
    _axis<43,48>::value, _axis<43,49>::value, _axis<43,50>::value, _axis<43,51>::value, _axis<43,52>::value, _axis<43,53>::value, _axis<43,54>::value, _axis<43,55>::value,
    _axis<43,56>::value, _axis<43,57>::value, _axis<43,58>::value, _axis<43,59>::value, _axis<43,60>::value, _axis<43,61>::value, _axis<43,62>::value, _axis<43,63>::value,
    _axis<44, 0>::value, _axis<44, 1>::value, _axis<44, 2>::value, _axis<44, 3>::value, _axis<44, 4>::value, _axis<44, 5>::value, _axis<44, 6>::value, _axis<44, 7>::value,
    _axis<44, 8>::value, _axis<44, 9>::value, _axis<44,10>::value, _axis<44,11>::value, _axis<44,12>::value, _axis<44,13>::value, _axis<44,14>::value, _axis<44,15>::value,
    _axis<44,16>::value, _axis<44,17>::value, _axis<44,18>::value, _axis<44,19>::value, _axis<44,20>::value, _axis<44,21>::value, _axis<44,22>::value, _axis<44,23>::value,
    _axis<44,24>::value, _axis<44,25>::value, _axis<44,26>::value, _axis<44,27>::value, _axis<44,28>::value, _axis<44,29>::value, _axis<44,30>::value, _axis<44,31>::value,
    _axis<44,32>::value, _axis<44,33>::value, _axis<44,34>::value, _axis<44,35>::value, _axis<44,36>::value, _axis<44,37>::value, _axis<44,38>::value, _axis<44,39>::value,
    _axis<44,40>::value, _axis<44,41>::value, _axis<44,42>::value, _axis<44,43>::value, _axis<44,44>::value, _axis<44,45>::value, _axis<44,46>::value, _axis<44,47>::value,
    _axis<44,48>::value, _axis<44,49>::value, _axis<44,50>::value, _axis<44,51>::value, _axis<44,52>::value, _axis<44,53>::value, _axis<44,54>::value, _axis<44,55>::value,
    _axis<44,56>::value, _axis<44,57>::value, _axis<44,58>::value, _axis<44,59>::value, _axis<44,60>::value, _axis<44,61>::value, _axis<44,62>::value, _axis<44,63>::value,
    _axis<45, 0>::value, _axis<45, 1>::value, _axis<45, 2>::value, _axis<45, 3>::value, _axis<45, 4>::value, _axis<45, 5>::value, _axis<45, 6>::value, _axis<45, 7>::value,
    _axis<45, 8>::value, _axis<45, 9>::value, _axis<45,10>::value, _axis<45,11>::value, _axis<45,12>::value, _axis<45,13>::value, _axis<45,14>::value, _axis<45,15>::value,
    _axis<45,16>::value, _axis<45,17>::value, _axis<45,18>::value, _axis<45,19>::value, _axis<45,20>::value, _axis<45,21>::value, _axis<45,22>::value, _axis<45,23>::value,
    _axis<45,24>::value, _axis<45,25>::value, _axis<45,26>::value, _axis<45,27>::value, _axis<45,28>::value, _axis<45,29>::value, _axis<45,30>::value, _axis<45,31>::value,
    _axis<45,32>::value, _axis<45,33>::value, _axis<45,34>::value, _axis<45,35>::value, _axis<45,36>::value, _axis<45,37>::value, _axis<45,38>::value, _axis<45,39>::value,
    _axis<45,40>::value, _axis<45,41>::value, _axis<45,42>::value, _axis<45,43>::value, _axis<45,44>::value, _axis<45,45>::value, _axis<45,46>::value, _axis<45,47>::value,
    _axis<45,48>::value, _axis<45,49>::value, _axis<45,50>::value, _axis<45,51>::value, _axis<45,52>::value, _axis<45,53>::value, _axis<45,54>::value, _axis<45,55>::value,
    _axis<45,56>::value, _axis<45,57>::value, _axis<45,58>::value, _axis<45,59>::value, _axis<45,60>::value, _axis<45,61>::value, _axis<45,62>::value, _axis<45,63>::value,
    _axis<46, 0>::value, _axis<46, 1>::value, _axis<46, 2>::value, _axis<46, 3>::value, _axis<46, 4>::value, _axis<46, 5>::value, _axis<46, 6>::value, _axis<46, 7>::value,
    _axis<46, 8>::value, _axis<46, 9>::value, _axis<46,10>::value, _axis<46,11>::value, _axis<46,12>::value, _axis<46,13>::value, _axis<46,14>::value, _axis<46,15>::value,
    _axis<46,16>::value, _axis<46,17>::value, _axis<46,18>::value, _axis<46,19>::value, _axis<46,20>::value, _axis<46,21>::value, _axis<46,22>::value, _axis<46,23>::value,
    _axis<46,24>::value, _axis<46,25>::value, _axis<46,26>::value, _axis<46,27>::value, _axis<46,28>::value, _axis<46,29>::value, _axis<46,30>::value, _axis<46,31>::value,
    _axis<46,32>::value, _axis<46,33>::value, _axis<46,34>::value, _axis<46,35>::value, _axis<46,36>::value, _axis<46,37>::value, _axis<46,38>::value, _axis<46,39>::value,
    _axis<46,40>::value, _axis<46,41>::value, _axis<46,42>::value, _axis<46,43>::value, _axis<46,44>::value, _axis<46,45>::value, _axis<46,46>::value, _axis<46,47>::value,
    _axis<46,48>::value, _axis<46,49>::value, _axis<46,50>::value, _axis<46,51>::value, _axis<46,52>::value, _axis<46,53>::value, _axis<46,54>::value, _axis<46,55>::value,
    _axis<46,56>::value, _axis<46,57>::value, _axis<46,58>::value, _axis<46,59>::value, _axis<46,60>::value, _axis<46,61>::value, _axis<46,62>::value, _axis<46,63>::value,
    _axis<47, 0>::value, _axis<47, 1>::value, _axis<47, 2>::value, _axis<47, 3>::value, _axis<47, 4>::value, _axis<47, 5>::value, _axis<47, 6>::value, _axis<47, 7>::value,
    _axis<47, 8>::value, _axis<47, 9>::value, _axis<47,10>::value, _axis<47,11>::value, _axis<47,12>::value, _axis<47,13>::value, _axis<47,14>::value, _axis<47,15>::value,
    _axis<47,16>::value, _axis<47,17>::value, _axis<47,18>::value, _axis<47,19>::value, _axis<47,20>::value, _axis<47,21>::value, _axis<47,22>::value, _axis<47,23>::value,
    _axis<47,24>::value, _axis<47,25>::value, _axis<47,26>::value, _axis<47,27>::value, _axis<47,28>::value, _axis<47,29>::value, _axis<47,30>::value, _axis<47,31>::value,
    _axis<47,32>::value, _axis<47,33>::value, _axis<47,34>::value, _axis<47,35>::value, _axis<47,36>::value, _axis<47,37>::value, _axis<47,38>::value, _axis<47,39>::value,
    _axis<47,40>::value, _axis<47,41>::value, _axis<47,42>::value, _axis<47,43>::value, _axis<47,44>::value, _axis<47,45>::value, _axis<47,46>::value, _axis<47,47>::value,
    _axis<47,48>::value, _axis<47,49>::value, _axis<47,50>::value, _axis<47,51>::value, _axis<47,52>::value, _axis<47,53>::value, _axis<47,54>::value, _axis<47,55>::value,
    _axis<47,56>::value, _axis<47,57>::value, _axis<47,58>::value, _axis<47,59>::value, _axis<47,60>::value, _axis<47,61>::value, _axis<47,62>::value, _axis<47,63>::value,
    _axis<48, 0>::value, _axis<48, 1>::value, _axis<48, 2>::value, _axis<48, 3>::value, _axis<48, 4>::value, _axis<48, 5>::value, _axis<48, 6>::value, _axis<48, 7>::value,
    _axis<48, 8>::value, _axis<48, 9>::value, _axis<48,10>::value, _axis<48,11>::value, _axis<48,12>::value, _axis<48,13>::value, _axis<48,14>::value, _axis<48,15>::value,
    _axis<48,16>::value, _axis<48,17>::value, _axis<48,18>::value, _axis<48,19>::value, _axis<48,20>::value, _axis<48,21>::value, _axis<48,22>::value, _axis<48,23>::value,
    _axis<48,24>::value, _axis<48,25>::value, _axis<48,26>::value, _axis<48,27>::value, _axis<48,28>::value, _axis<48,29>::value, _axis<48,30>::value, _axis<48,31>::value,
    _axis<48,32>::value, _axis<48,33>::value, _axis<48,34>::value, _axis<48,35>::value, _axis<48,36>::value, _axis<48,37>::value, _axis<48,38>::value, _axis<48,39>::value,
    _axis<48,40>::value, _axis<48,41>::value, _axis<48,42>::value, _axis<48,43>::value, _axis<48,44>::value, _axis<48,45>::value, _axis<48,46>::value, _axis<48,47>::value,
    _axis<48,48>::value, _axis<48,49>::value, _axis<48,50>::value, _axis<48,51>::value, _axis<48,52>::value, _axis<48,53>::value, _axis<48,54>::value, _axis<48,55>::value,
    _axis<48,56>::value, _axis<48,57>::value, _axis<48,58>::value, _axis<48,59>::value, _axis<48,60>::value, _axis<48,61>::value, _axis<48,62>::value, _axis<48,63>::value,
    _axis<49, 0>::value, _axis<49, 1>::value, _axis<49, 2>::value, _axis<49, 3>::value, _axis<49, 4>::value, _axis<49, 5>::value, _axis<49, 6>::value, _axis<49, 7>::value,
    _axis<49, 8>::value, _axis<49, 9>::value, _axis<49,10>::value, _axis<49,11>::value, _axis<49,12>::value, _axis<49,13>::value, _axis<49,14>::value, _axis<49,15>::value,
    _axis<49,16>::value, _axis<49,17>::value, _axis<49,18>::value, _axis<49,19>::value, _axis<49,20>::value, _axis<49,21>::value, _axis<49,22>::value, _axis<49,23>::value,
    _axis<49,24>::value, _axis<49,25>::value, _axis<49,26>::value, _axis<49,27>::value, _axis<49,28>::value, _axis<49,29>::value, _axis<49,30>::value, _axis<49,31>::value,
    _axis<49,32>::value, _axis<49,33>::value, _axis<49,34>::value, _axis<49,35>::value, _axis<49,36>::value, _axis<49,37>::value, _axis<49,38>::value, _axis<49,39>::value,
    _axis<49,40>::value, _axis<49,41>::value, _axis<49,42>::value, _axis<49,43>::value, _axis<49,44>::value, _axis<49,45>::value, _axis<49,46>::value, _axis<49,47>::value,
    _axis<49,48>::value, _axis<49,49>::value, _axis<49,50>::value, _axis<49,51>::value, _axis<49,52>::value, _axis<49,53>::value, _axis<49,54>::value, _axis<49,55>::value,
    _axis<49,56>::value, _axis<49,57>::value, _axis<49,58>::value, _axis<49,59>::value, _axis<49,60>::value, _axis<49,61>::value, _axis<49,62>::value, _axis<49,63>::value,
    _axis<50, 0>::value, _axis<50, 1>::value, _axis<50, 2>::value, _axis<50, 3>::value, _axis<50, 4>::value, _axis<50, 5>::value, _axis<50, 6>::value, _axis<50, 7>::value,
    _axis<50, 8>::value, _axis<50, 9>::value, _axis<50,10>::value, _axis<50,11>::value, _axis<50,12>::value, _axis<50,13>::value, _axis<50,14>::value, _axis<50,15>::value,
    _axis<50,16>::value, _axis<50,17>::value, _axis<50,18>::value, _axis<50,19>::value, _axis<50,20>::value, _axis<50,21>::value, _axis<50,22>::value, _axis<50,23>::value,
    _axis<50,24>::value, _axis<50,25>::value, _axis<50,26>::value, _axis<50,27>::value, _axis<50,28>::value, _axis<50,29>::value, _axis<50,30>::value, _axis<50,31>::value,
    _axis<50,32>::value, _axis<50,33>::value, _axis<50,34>::value, _axis<50,35>::value, _axis<50,36>::value, _axis<50,37>::value, _axis<50,38>::value, _axis<50,39>::value,
    _axis<50,40>::value, _axis<50,41>::value, _axis<50,42>::value, _axis<50,43>::value, _axis<50,44>::value, _axis<50,45>::value, _axis<50,46>::value, _axis<50,47>::value,
    _axis<50,48>::value, _axis<50,49>::value, _axis<50,50>::value, _axis<50,51>::value, _axis<50,52>::value, _axis<50,53>::value, _axis<50,54>::value, _axis<50,55>::value,
    _axis<50,56>::value, _axis<50,57>::value, _axis<50,58>::value, _axis<50,59>::value, _axis<50,60>::value, _axis<50,61>::value, _axis<50,62>::value, _axis<50,63>::value,
    _axis<51, 0>::value, _axis<51, 1>::value, _axis<51, 2>::value, _axis<51, 3>::value, _axis<51, 4>::value, _axis<51, 5>::value, _axis<51, 6>::value, _axis<51, 7>::value,
    _axis<51, 8>::value, _axis<51, 9>::value, _axis<51,10>::value, _axis<51,11>::value, _axis<51,12>::value, _axis<51,13>::value, _axis<51,14>::value, _axis<51,15>::value,
    _axis<51,16>::value, _axis<51,17>::value, _axis<51,18>::value, _axis<51,19>::value, _axis<51,20>::value, _axis<51,21>::value, _axis<51,22>::value, _axis<51,23>::value,
    _axis<51,24>::value, _axis<51,25>::value, _axis<51,26>::value, _axis<51,27>::value, _axis<51,28>::value, _axis<51,29>::value, _axis<51,30>::value, _axis<51,31>::value,
    _axis<51,32>::value, _axis<51,33>::value, _axis<51,34>::value, _axis<51,35>::value, _axis<51,36>::value, _axis<51,37>::value, _axis<51,38>::value, _axis<51,39>::value,
    _axis<51,40>::value, _axis<51,41>::value, _axis<51,42>::value, _axis<51,43>::value, _axis<51,44>::value, _axis<51,45>::value, _axis<51,46>::value, _axis<51,47>::value,
    _axis<51,48>::value, _axis<51,49>::value, _axis<51,50>::value, _axis<51,51>::value, _axis<51,52>::value, _axis<51,53>::value, _axis<51,54>::value, _axis<51,55>::value,
    _axis<51,56>::value, _axis<51,57>::value, _axis<51,58>::value, _axis<51,59>::value, _axis<51,60>::value, _axis<51,61>::value, _axis<51,62>::value, _axis<51,63>::value,
    _axis<52, 0>::value, _axis<52, 1>::value, _axis<52, 2>::value, _axis<52, 3>::value, _axis<52, 4>::value, _axis<52, 5>::value, _axis<52, 6>::value, _axis<52, 7>::value,
    _axis<52, 8>::value, _axis<52, 9>::value, _axis<52,10>::value, _axis<52,11>::value, _axis<52,12>::value, _axis<52,13>::value, _axis<52,14>::value, _axis<52,15>::value,
    _axis<52,16>::value, _axis<52,17>::value, _axis<52,18>::value, _axis<52,19>::value, _axis<52,20>::value, _axis<52,21>::value, _axis<52,22>::value, _axis<52,23>::value,
    _axis<52,24>::value, _axis<52,25>::value, _axis<52,26>::value, _axis<52,27>::value, _axis<52,28>::value, _axis<52,29>::value, _axis<52,30>::value, _axis<52,31>::value,
    _axis<52,32>::value, _axis<52,33>::value, _axis<52,34>::value, _axis<52,35>::value, _axis<52,36>::value, _axis<52,37>::value, _axis<52,38>::value, _axis<52,39>::value,
    _axis<52,40>::value, _axis<52,41>::value, _axis<52,42>::value, _axis<52,43>::value, _axis<52,44>::value, _axis<52,45>::value, _axis<52,46>::value, _axis<52,47>::value,
    _axis<52,48>::value, _axis<52,49>::value, _axis<52,50>::value, _axis<52,51>::value, _axis<52,52>::value, _axis<52,53>::value, _axis<52,54>::value, _axis<52,55>::value,
    _axis<52,56>::value, _axis<52,57>::value, _axis<52,58>::value, _axis<52,59>::value, _axis<52,60>::value, _axis<52,61>::value, _axis<52,62>::value, _axis<52,63>::value,
    _axis<53, 0>::value, _axis<53, 1>::value, _axis<53, 2>::value, _axis<53, 3>::value, _axis<53, 4>::value, _axis<53, 5>::value, _axis<53, 6>::value, _axis<53, 7>::value,
    _axis<53, 8>::value, _axis<53, 9>::value, _axis<53,10>::value, _axis<53,11>::value, _axis<53,12>::value, _axis<53,13>::value, _axis<53,14>::value, _axis<53,15>::value,
    _axis<53,16>::value, _axis<53,17>::value, _axis<53,18>::value, _axis<53,19>::value, _axis<53,20>::value, _axis<53,21>::value, _axis<53,22>::value, _axis<53,23>::value,
    _axis<53,24>::value, _axis<53,25>::value, _axis<53,26>::value, _axis<53,27>::value, _axis<53,28>::value, _axis<53,29>::value, _axis<53,30>::value, _axis<53,31>::value,
    _axis<53,32>::value, _axis<53,33>::value, _axis<53,34>::value, _axis<53,35>::value, _axis<53,36>::value, _axis<53,37>::value, _axis<53,38>::value, _axis<53,39>::value,
    _axis<53,40>::value, _axis<53,41>::value, _axis<53,42>::value, _axis<53,43>::value, _axis<53,44>::value, _axis<53,45>::value, _axis<53,46>::value, _axis<53,47>::value,
    _axis<53,48>::value, _axis<53,49>::value, _axis<53,50>::value, _axis<53,51>::value, _axis<53,52>::value, _axis<53,53>::value, _axis<53,54>::value, _axis<53,55>::value,
    _axis<53,56>::value, _axis<53,57>::value, _axis<53,58>::value, _axis<53,59>::value, _axis<53,60>::value, _axis<53,61>::value, _axis<53,62>::value, _axis<53,63>::value,
    _axis<54, 0>::value, _axis<54, 1>::value, _axis<54, 2>::value, _axis<54, 3>::value, _axis<54, 4>::value, _axis<54, 5>::value, _axis<54, 6>::value, _axis<54, 7>::value,
    _axis<54, 8>::value, _axis<54, 9>::value, _axis<54,10>::value, _axis<54,11>::value, _axis<54,12>::value, _axis<54,13>::value, _axis<54,14>::value, _axis<54,15>::value,
    _axis<54,16>::value, _axis<54,17>::value, _axis<54,18>::value, _axis<54,19>::value, _axis<54,20>::value, _axis<54,21>::value, _axis<54,22>::value, _axis<54,23>::value,
    _axis<54,24>::value, _axis<54,25>::value, _axis<54,26>::value, _axis<54,27>::value, _axis<54,28>::value, _axis<54,29>::value, _axis<54,30>::value, _axis<54,31>::value,
    _axis<54,32>::value, _axis<54,33>::value, _axis<54,34>::value, _axis<54,35>::value, _axis<54,36>::value, _axis<54,37>::value, _axis<54,38>::value, _axis<54,39>::value,
    _axis<54,40>::value, _axis<54,41>::value, _axis<54,42>::value, _axis<54,43>::value, _axis<54,44>::value, _axis<54,45>::value, _axis<54,46>::value, _axis<54,47>::value,
    _axis<54,48>::value, _axis<54,49>::value, _axis<54,50>::value, _axis<54,51>::value, _axis<54,52>::value, _axis<54,53>::value, _axis<54,54>::value, _axis<54,55>::value,
    _axis<54,56>::value, _axis<54,57>::value, _axis<54,58>::value, _axis<54,59>::value, _axis<54,60>::value, _axis<54,61>::value, _axis<54,62>::value, _axis<54,63>::value,
    _axis<55, 0>::value, _axis<55, 1>::value, _axis<55, 2>::value, _axis<55, 3>::value, _axis<55, 4>::value, _axis<55, 5>::value, _axis<55, 6>::value, _axis<55, 7>::value,
    _axis<55, 8>::value, _axis<55, 9>::value, _axis<55,10>::value, _axis<55,11>::value, _axis<55,12>::value, _axis<55,13>::value, _axis<55,14>::value, _axis<55,15>::value,
    _axis<55,16>::value, _axis<55,17>::value, _axis<55,18>::value, _axis<55,19>::value, _axis<55,20>::value, _axis<55,21>::value, _axis<55,22>::value, _axis<55,23>::value,
    _axis<55,24>::value, _axis<55,25>::value, _axis<55,26>::value, _axis<55,27>::value, _axis<55,28>::value, _axis<55,29>::value, _axis<55,30>::value, _axis<55,31>::value,
    _axis<55,32>::value, _axis<55,33>::value, _axis<55,34>::value, _axis<55,35>::value, _axis<55,36>::value, _axis<55,37>::value, _axis<55,38>::value, _axis<55,39>::value,
    _axis<55,40>::value, _axis<55,41>::value, _axis<55,42>::value, _axis<55,43>::value, _axis<55,44>::value, _axis<55,45>::value, _axis<55,46>::value, _axis<55,47>::value,
    _axis<55,48>::value, _axis<55,49>::value, _axis<55,50>::value, _axis<55,51>::value, _axis<55,52>::value, _axis<55,53>::value, _axis<55,54>::value, _axis<55,55>::value,
    _axis<55,56>::value, _axis<55,57>::value, _axis<55,58>::value, _axis<55,59>::value, _axis<55,60>::value, _axis<55,61>::value, _axis<55,62>::value, _axis<55,63>::value,
    _axis<56, 0>::value, _axis<56, 1>::value, _axis<56, 2>::value, _axis<56, 3>::value, _axis<56, 4>::value, _axis<56, 5>::value, _axis<56, 6>::value, _axis<56, 7>::value,
    _axis<56, 8>::value, _axis<56, 9>::value, _axis<56,10>::value, _axis<56,11>::value, _axis<56,12>::value, _axis<56,13>::value, _axis<56,14>::value, _axis<56,15>::value,
    _axis<56,16>::value, _axis<56,17>::value, _axis<56,18>::value, _axis<56,19>::value, _axis<56,20>::value, _axis<56,21>::value, _axis<56,22>::value, _axis<56,23>::value,
    _axis<56,24>::value, _axis<56,25>::value, _axis<56,26>::value, _axis<56,27>::value, _axis<56,28>::value, _axis<56,29>::value, _axis<56,30>::value, _axis<56,31>::value,
    _axis<56,32>::value, _axis<56,33>::value, _axis<56,34>::value, _axis<56,35>::value, _axis<56,36>::value, _axis<56,37>::value, _axis<56,38>::value, _axis<56,39>::value,
    _axis<56,40>::value, _axis<56,41>::value, _axis<56,42>::value, _axis<56,43>::value, _axis<56,44>::value, _axis<56,45>::value, _axis<56,46>::value, _axis<56,47>::value,
    _axis<56,48>::value, _axis<56,49>::value, _axis<56,50>::value, _axis<56,51>::value, _axis<56,52>::value, _axis<56,53>::value, _axis<56,54>::value, _axis<56,55>::value,
    _axis<56,56>::value, _axis<56,57>::value, _axis<56,58>::value, _axis<56,59>::value, _axis<56,60>::value, _axis<56,61>::value, _axis<56,62>::value, _axis<56,63>::value,
    _axis<57, 0>::value, _axis<57, 1>::value, _axis<57, 2>::value, _axis<57, 3>::value, _axis<57, 4>::value, _axis<57, 5>::value, _axis<57, 6>::value, _axis<57, 7>::value,
    _axis<57, 8>::value, _axis<57, 9>::value, _axis<57,10>::value, _axis<57,11>::value, _axis<57,12>::value, _axis<57,13>::value, _axis<57,14>::value, _axis<57,15>::value,
    _axis<57,16>::value, _axis<57,17>::value, _axis<57,18>::value, _axis<57,19>::value, _axis<57,20>::value, _axis<57,21>::value, _axis<57,22>::value, _axis<57,23>::value,
    _axis<57,24>::value, _axis<57,25>::value, _axis<57,26>::value, _axis<57,27>::value, _axis<57,28>::value, _axis<57,29>::value, _axis<57,30>::value, _axis<57,31>::value,
    _axis<57,32>::value, _axis<57,33>::value, _axis<57,34>::value, _axis<57,35>::value, _axis<57,36>::value, _axis<57,37>::value, _axis<57,38>::value, _axis<57,39>::value,
    _axis<57,40>::value, _axis<57,41>::value, _axis<57,42>::value, _axis<57,43>::value, _axis<57,44>::value, _axis<57,45>::value, _axis<57,46>::value, _axis<57,47>::value,
    _axis<57,48>::value, _axis<57,49>::value, _axis<57,50>::value, _axis<57,51>::value, _axis<57,52>::value, _axis<57,53>::value, _axis<57,54>::value, _axis<57,55>::value,
    _axis<57,56>::value, _axis<57,57>::value, _axis<57,58>::value, _axis<57,59>::value, _axis<57,60>::value, _axis<57,61>::value, _axis<57,62>::value, _axis<57,63>::value,
    _axis<58, 0>::value, _axis<58, 1>::value, _axis<58, 2>::value, _axis<58, 3>::value, _axis<58, 4>::value, _axis<58, 5>::value, _axis<58, 6>::value, _axis<58, 7>::value,
    _axis<58, 8>::value, _axis<58, 9>::value, _axis<58,10>::value, _axis<58,11>::value, _axis<58,12>::value, _axis<58,13>::value, _axis<58,14>::value, _axis<58,15>::value,
    _axis<58,16>::value, _axis<58,17>::value, _axis<58,18>::value, _axis<58,19>::value, _axis<58,20>::value, _axis<58,21>::value, _axis<58,22>::value, _axis<58,23>::value,
    _axis<58,24>::value, _axis<58,25>::value, _axis<58,26>::value, _axis<58,27>::value, _axis<58,28>::value, _axis<58,29>::value, _axis<58,30>::value, _axis<58,31>::value,
    _axis<58,32>::value, _axis<58,33>::value, _axis<58,34>::value, _axis<58,35>::value, _axis<58,36>::value, _axis<58,37>::value, _axis<58,38>::value, _axis<58,39>::value,
    _axis<58,40>::value, _axis<58,41>::value, _axis<58,42>::value, _axis<58,43>::value, _axis<58,44>::value, _axis<58,45>::value, _axis<58,46>::value, _axis<58,47>::value,
    _axis<58,48>::value, _axis<58,49>::value, _axis<58,50>::value, _axis<58,51>::value, _axis<58,52>::value, _axis<58,53>::value, _axis<58,54>::value, _axis<58,55>::value,
    _axis<58,56>::value, _axis<58,57>::value, _axis<58,58>::value, _axis<58,59>::value, _axis<58,60>::value, _axis<58,61>::value, _axis<58,62>::value, _axis<58,63>::value,
    _axis<59, 0>::value, _axis<59, 1>::value, _axis<59, 2>::value, _axis<59, 3>::value, _axis<59, 4>::value, _axis<59, 5>::value, _axis<59, 6>::value, _axis<59, 7>::value,
    _axis<59, 8>::value, _axis<59, 9>::value, _axis<59,10>::value, _axis<59,11>::value, _axis<59,12>::value, _axis<59,13>::value, _axis<59,14>::value, _axis<59,15>::value,
    _axis<59,16>::value, _axis<59,17>::value, _axis<59,18>::value, _axis<59,19>::value, _axis<59,20>::value, _axis<59,21>::value, _axis<59,22>::value, _axis<59,23>::value,
    _axis<59,24>::value, _axis<59,25>::value, _axis<59,26>::value, _axis<59,27>::value, _axis<59,28>::value, _axis<59,29>::value, _axis<59,30>::value, _axis<59,31>::value,
    _axis<59,32>::value, _axis<59,33>::value, _axis<59,34>::value, _axis<59,35>::value, _axis<59,36>::value, _axis<59,37>::value, _axis<59,38>::value, _axis<59,39>::value,
    _axis<59,40>::value, _axis<59,41>::value, _axis<59,42>::value, _axis<59,43>::value, _axis<59,44>::value, _axis<59,45>::value, _axis<59,46>::value, _axis<59,47>::value,
    _axis<59,48>::value, _axis<59,49>::value, _axis<59,50>::value, _axis<59,51>::value, _axis<59,52>::value, _axis<59,53>::value, _axis<59,54>::value, _axis<59,55>::value,
    _axis<59,56>::value, _axis<59,57>::value, _axis<59,58>::value, _axis<59,59>::value, _axis<59,60>::value, _axis<59,61>::value, _axis<59,62>::value, _axis<59,63>::value,
    _axis<60, 0>::value, _axis<60, 1>::value, _axis<60, 2>::value, _axis<60, 3>::value, _axis<60, 4>::value, _axis<60, 5>::value, _axis<60, 6>::value, _axis<60, 7>::value,
    _axis<60, 8>::value, _axis<60, 9>::value, _axis<60,10>::value, _axis<60,11>::value, _axis<60,12>::value, _axis<60,13>::value, _axis<60,14>::value, _axis<60,15>::value,
    _axis<60,16>::value, _axis<60,17>::value, _axis<60,18>::value, _axis<60,19>::value, _axis<60,20>::value, _axis<60,21>::value, _axis<60,22>::value, _axis<60,23>::value,
    _axis<60,24>::value, _axis<60,25>::value, _axis<60,26>::value, _axis<60,27>::value, _axis<60,28>::value, _axis<60,29>::value, _axis<60,30>::value, _axis<60,31>::value,
    _axis<60,32>::value, _axis<60,33>::value, _axis<60,34>::value, _axis<60,35>::value, _axis<60,36>::value, _axis<60,37>::value, _axis<60,38>::value, _axis<60,39>::value,
    _axis<60,40>::value, _axis<60,41>::value, _axis<60,42>::value, _axis<60,43>::value, _axis<60,44>::value, _axis<60,45>::value, _axis<60,46>::value, _axis<60,47>::value,
    _axis<60,48>::value, _axis<60,49>::value, _axis<60,50>::value, _axis<60,51>::value, _axis<60,52>::value, _axis<60,53>::value, _axis<60,54>::value, _axis<60,55>::value,
    _axis<60,56>::value, _axis<60,57>::value, _axis<60,58>::value, _axis<60,59>::value, _axis<60,60>::value, _axis<60,61>::value, _axis<60,62>::value, _axis<60,63>::value,
    _axis<61, 0>::value, _axis<61, 1>::value, _axis<61, 2>::value, _axis<61, 3>::value, _axis<61, 4>::value, _axis<61, 5>::value, _axis<61, 6>::value, _axis<61, 7>::value,
    _axis<61, 8>::value, _axis<61, 9>::value, _axis<61,10>::value, _axis<61,11>::value, _axis<61,12>::value, _axis<61,13>::value, _axis<61,14>::value, _axis<61,15>::value,
    _axis<61,16>::value, _axis<61,17>::value, _axis<61,18>::value, _axis<61,19>::value, _axis<61,20>::value, _axis<61,21>::value, _axis<61,22>::value, _axis<61,23>::value,
    _axis<61,24>::value, _axis<61,25>::value, _axis<61,26>::value, _axis<61,27>::value, _axis<61,28>::value, _axis<61,29>::value, _axis<61,30>::value, _axis<61,31>::value,
    _axis<61,32>::value, _axis<61,33>::value, _axis<61,34>::value, _axis<61,35>::value, _axis<61,36>::value, _axis<61,37>::value, _axis<61,38>::value, _axis<61,39>::value,
    _axis<61,40>::value, _axis<61,41>::value, _axis<61,42>::value, _axis<61,43>::value, _axis<61,44>::value, _axis<61,45>::value, _axis<61,46>::value, _axis<61,47>::value,
    _axis<61,48>::value, _axis<61,49>::value, _axis<61,50>::value, _axis<61,51>::value, _axis<61,52>::value, _axis<61,53>::value, _axis<61,54>::value, _axis<61,55>::value,
    _axis<61,56>::value, _axis<61,57>::value, _axis<61,58>::value, _axis<61,59>::value, _axis<61,60>::value, _axis<61,61>::value, _axis<61,62>::value, _axis<61,63>::value,
    _axis<62, 0>::value, _axis<62, 1>::value, _axis<62, 2>::value, _axis<62, 3>::value, _axis<62, 4>::value, _axis<62, 5>::value, _axis<62, 6>::value, _axis<62, 7>::value,
    _axis<62, 8>::value, _axis<62, 9>::value, _axis<62,10>::value, _axis<62,11>::value, _axis<62,12>::value, _axis<62,13>::value, _axis<62,14>::value, _axis<62,15>::value,
    _axis<62,16>::value, _axis<62,17>::value, _axis<62,18>::value, _axis<62,19>::value, _axis<62,20>::value, _axis<62,21>::value, _axis<62,22>::value, _axis<62,23>::value,
    _axis<62,24>::value, _axis<62,25>::value, _axis<62,26>::value, _axis<62,27>::value, _axis<62,28>::value, _axis<62,29>::value, _axis<62,30>::value, _axis<62,31>::value,
    _axis<62,32>::value, _axis<62,33>::value, _axis<62,34>::value, _axis<62,35>::value, _axis<62,36>::value, _axis<62,37>::value, _axis<62,38>::value, _axis<62,39>::value,
    _axis<62,40>::value, _axis<62,41>::value, _axis<62,42>::value, _axis<62,43>::value, _axis<62,44>::value, _axis<62,45>::value, _axis<62,46>::value, _axis<62,47>::value,
    _axis<62,48>::value, _axis<62,49>::value, _axis<62,50>::value, _axis<62,51>::value, _axis<62,52>::value, _axis<62,53>::value, _axis<62,54>::value, _axis<62,55>::value,
    _axis<62,56>::value, _axis<62,57>::value, _axis<62,58>::value, _axis<62,59>::value, _axis<62,60>::value, _axis<62,61>::value, _axis<62,62>::value, _axis<62,63>::value,
    _axis<63, 0>::value, _axis<63, 1>::value, _axis<63, 2>::value, _axis<63, 3>::value, _axis<63, 4>::value, _axis<63, 5>::value, _axis<63, 6>::value, _axis<63, 7>::value,
    _axis<63, 8>::value, _axis<63, 9>::value, _axis<63,10>::value, _axis<63,11>::value, _axis<63,12>::value, _axis<63,13>::value, _axis<63,14>::value, _axis<63,15>::value,
    _axis<63,16>::value, _axis<63,17>::value, _axis<63,18>::value, _axis<63,19>::value, _axis<63,20>::value, _axis<63,21>::value, _axis<63,22>::value, _axis<63,23>::value,
    _axis<63,24>::value, _axis<63,25>::value, _axis<63,26>::value, _axis<63,27>::value, _axis<63,28>::value, _axis<63,29>::value, _axis<63,30>::value, _axis<63,31>::value,
    _axis<63,32>::value, _axis<63,33>::value, _axis<63,34>::value, _axis<63,35>::value, _axis<63,36>::value, _axis<63,37>::value, _axis<63,38>::value, _axis<63,39>::value,
    _axis<63,40>::value, _axis<63,41>::value, _axis<63,42>::value, _axis<63,43>::value, _axis<63,44>::value, _axis<63,45>::value, _axis<63,46>::value, _axis<63,47>::value,
    _axis<63,48>::value, _axis<63,49>::value, _axis<63,50>::value, _axis<63,51>::value, _axis<63,52>::value, _axis<63,53>::value, _axis<63,54>::value, _axis<63,55>::value,
    _axis<63,56>::value, _axis<63,57>::value, _axis<63,58>::value, _axis<63,59>::value, _axis<63,60>::value, _axis<63,61>::value, _axis<63,62>::value, _axis<63,63>::value
};

const std::array<std::array<U64, 64>, 64> lines = {
    _line< 0, 0>::value, _line< 0, 1>::value, _line< 0, 2>::value, _line< 0, 3>::value, _line< 0, 4>::value, _line< 0, 5>::value, _line< 0, 6>::value, _line< 0, 7>::value,
    _line< 0, 8>::value, _line< 0, 9>::value, _line< 0,10>::value, _line< 0,11>::value, _line< 0,12>::value, _line< 0,13>::value, _line< 0,14>::value, _line< 0,15>::value,
    _line< 0,16>::value, _line< 0,17>::value, _line< 0,18>::value, _line< 0,19>::value, _line< 0,20>::value, _line< 0,21>::value, _line< 0,22>::value, _line< 0,23>::value,
    _line< 0,24>::value, _line< 0,25>::value, _line< 0,26>::value, _line< 0,27>::value, _line< 0,28>::value, _line< 0,29>::value, _line< 0,30>::value, _line< 0,31>::value,
    _line< 0,32>::value, _line< 0,33>::value, _line< 0,34>::value, _line< 0,35>::value, _line< 0,36>::value, _line< 0,37>::value, _line< 0,38>::value, _line< 0,39>::value,
    _line< 0,40>::value, _line< 0,41>::value, _line< 0,42>::value, _line< 0,43>::value, _line< 0,44>::value, _line< 0,45>::value, _line< 0,46>::value, _line< 0,47>::value,
    _line< 0,48>::value, _line< 0,49>::value, _line< 0,50>::value, _line< 0,51>::value, _line< 0,52>::value, _line< 0,53>::value, _line< 0,54>::value, _line< 0,55>::value,
    _line< 0,56>::value, _line< 0,57>::value, _line< 0,58>::value, _line< 0,59>::value, _line< 0,60>::value, _line< 0,61>::value, _line< 0,62>::value, _line< 0,63>::value,
    _line< 1, 0>::value, _line< 1, 1>::value, _line< 1, 2>::value, _line< 1, 3>::value, _line< 1, 4>::value, _line< 1, 5>::value, _line< 1, 6>::value, _line< 1, 7>::value,
    _line< 1, 8>::value, _line< 1, 9>::value, _line< 1,10>::value, _line< 1,11>::value, _line< 1,12>::value, _line< 1,13>::value, _line< 1,14>::value, _line< 1,15>::value,
    _line< 1,16>::value, _line< 1,17>::value, _line< 1,18>::value, _line< 1,19>::value, _line< 1,20>::value, _line< 1,21>::value, _line< 1,22>::value, _line< 1,23>::value,
    _line< 1,24>::value, _line< 1,25>::value, _line< 1,26>::value, _line< 1,27>::value, _line< 1,28>::value, _line< 1,29>::value, _line< 1,30>::value, _line< 1,31>::value,
    _line< 1,32>::value, _line< 1,33>::value, _line< 1,34>::value, _line< 1,35>::value, _line< 1,36>::value, _line< 1,37>::value, _line< 1,38>::value, _line< 1,39>::value,
    _line< 1,40>::value, _line< 1,41>::value, _line< 1,42>::value, _line< 1,43>::value, _line< 1,44>::value, _line< 1,45>::value, _line< 1,46>::value, _line< 1,47>::value,
    _line< 1,48>::value, _line< 1,49>::value, _line< 1,50>::value, _line< 1,51>::value, _line< 1,52>::value, _line< 1,53>::value, _line< 1,54>::value, _line< 1,55>::value,
    _line< 1,56>::value, _line< 1,57>::value, _line< 1,58>::value, _line< 1,59>::value, _line< 1,60>::value, _line< 1,61>::value, _line< 1,62>::value, _line< 1,63>::value,
    _line< 2, 0>::value, _line< 2, 1>::value, _line< 2, 2>::value, _line< 2, 3>::value, _line< 2, 4>::value, _line< 2, 5>::value, _line< 2, 6>::value, _line< 2, 7>::value,
    _line< 2, 8>::value, _line< 2, 9>::value, _line< 2,10>::value, _line< 2,11>::value, _line< 2,12>::value, _line< 2,13>::value, _line< 2,14>::value, _line< 2,15>::value,
    _line< 2,16>::value, _line< 2,17>::value, _line< 2,18>::value, _line< 2,19>::value, _line< 2,20>::value, _line< 2,21>::value, _line< 2,22>::value, _line< 2,23>::value,
    _line< 2,24>::value, _line< 2,25>::value, _line< 2,26>::value, _line< 2,27>::value, _line< 2,28>::value, _line< 2,29>::value, _line< 2,30>::value, _line< 2,31>::value,
    _line< 2,32>::value, _line< 2,33>::value, _line< 2,34>::value, _line< 2,35>::value, _line< 2,36>::value, _line< 2,37>::value, _line< 2,38>::value, _line< 2,39>::value,
    _line< 2,40>::value, _line< 2,41>::value, _line< 2,42>::value, _line< 2,43>::value, _line< 2,44>::value, _line< 2,45>::value, _line< 2,46>::value, _line< 2,47>::value,
    _line< 2,48>::value, _line< 2,49>::value, _line< 2,50>::value, _line< 2,51>::value, _line< 2,52>::value, _line< 2,53>::value, _line< 2,54>::value, _line< 2,55>::value,
    _line< 2,56>::value, _line< 2,57>::value, _line< 2,58>::value, _line< 2,59>::value, _line< 2,60>::value, _line< 2,61>::value, _line< 2,62>::value, _line< 2,63>::value,
    _line< 3, 0>::value, _line< 3, 1>::value, _line< 3, 2>::value, _line< 3, 3>::value, _line< 3, 4>::value, _line< 3, 5>::value, _line< 3, 6>::value, _line< 3, 7>::value,
    _line< 3, 8>::value, _line< 3, 9>::value, _line< 3,10>::value, _line< 3,11>::value, _line< 3,12>::value, _line< 3,13>::value, _line< 3,14>::value, _line< 3,15>::value,
    _line< 3,16>::value, _line< 3,17>::value, _line< 3,18>::value, _line< 3,19>::value, _line< 3,20>::value, _line< 3,21>::value, _line< 3,22>::value, _line< 3,23>::value,
    _line< 3,24>::value, _line< 3,25>::value, _line< 3,26>::value, _line< 3,27>::value, _line< 3,28>::value, _line< 3,29>::value, _line< 3,30>::value, _line< 3,31>::value,
    _line< 3,32>::value, _line< 3,33>::value, _line< 3,34>::value, _line< 3,35>::value, _line< 3,36>::value, _line< 3,37>::value, _line< 3,38>::value, _line< 3,39>::value,
    _line< 3,40>::value, _line< 3,41>::value, _line< 3,42>::value, _line< 3,43>::value, _line< 3,44>::value, _line< 3,45>::value, _line< 3,46>::value, _line< 3,47>::value,
    _line< 3,48>::value, _line< 3,49>::value, _line< 3,50>::value, _line< 3,51>::value, _line< 3,52>::value, _line< 3,53>::value, _line< 3,54>::value, _line< 3,55>::value,
    _line< 3,56>::value, _line< 3,57>::value, _line< 3,58>::value, _line< 3,59>::value, _line< 3,60>::value, _line< 3,61>::value, _line< 3,62>::value, _line< 3,63>::value,
    _line< 4, 0>::value, _line< 4, 1>::value, _line< 4, 2>::value, _line< 4, 3>::value, _line< 4, 4>::value, _line< 4, 5>::value, _line< 4, 6>::value, _line< 4, 7>::value,
    _line< 4, 8>::value, _line< 4, 9>::value, _line< 4,10>::value, _line< 4,11>::value, _line< 4,12>::value, _line< 4,13>::value, _line< 4,14>::value, _line< 4,15>::value,
    _line< 4,16>::value, _line< 4,17>::value, _line< 4,18>::value, _line< 4,19>::value, _line< 4,20>::value, _line< 4,21>::value, _line< 4,22>::value, _line< 4,23>::value,
    _line< 4,24>::value, _line< 4,25>::value, _line< 4,26>::value, _line< 4,27>::value, _line< 4,28>::value, _line< 4,29>::value, _line< 4,30>::value, _line< 4,31>::value,
    _line< 4,32>::value, _line< 4,33>::value, _line< 4,34>::value, _line< 4,35>::value, _line< 4,36>::value, _line< 4,37>::value, _line< 4,38>::value, _line< 4,39>::value,
    _line< 4,40>::value, _line< 4,41>::value, _line< 4,42>::value, _line< 4,43>::value, _line< 4,44>::value, _line< 4,45>::value, _line< 4,46>::value, _line< 4,47>::value,
    _line< 4,48>::value, _line< 4,49>::value, _line< 4,50>::value, _line< 4,51>::value, _line< 4,52>::value, _line< 4,53>::value, _line< 4,54>::value, _line< 4,55>::value,
    _line< 4,56>::value, _line< 4,57>::value, _line< 4,58>::value, _line< 4,59>::value, _line< 4,60>::value, _line< 4,61>::value, _line< 4,62>::value, _line< 4,63>::value,
    _line< 5, 0>::value, _line< 5, 1>::value, _line< 5, 2>::value, _line< 5, 3>::value, _line< 5, 4>::value, _line< 5, 5>::value, _line< 5, 6>::value, _line< 5, 7>::value,
    _line< 5, 8>::value, _line< 5, 9>::value, _line< 5,10>::value, _line< 5,11>::value, _line< 5,12>::value, _line< 5,13>::value, _line< 5,14>::value, _line< 5,15>::value,
    _line< 5,16>::value, _line< 5,17>::value, _line< 5,18>::value, _line< 5,19>::value, _line< 5,20>::value, _line< 5,21>::value, _line< 5,22>::value, _line< 5,23>::value,
    _line< 5,24>::value, _line< 5,25>::value, _line< 5,26>::value, _line< 5,27>::value, _line< 5,28>::value, _line< 5,29>::value, _line< 5,30>::value, _line< 5,31>::value,
    _line< 5,32>::value, _line< 5,33>::value, _line< 5,34>::value, _line< 5,35>::value, _line< 5,36>::value, _line< 5,37>::value, _line< 5,38>::value, _line< 5,39>::value,
    _line< 5,40>::value, _line< 5,41>::value, _line< 5,42>::value, _line< 5,43>::value, _line< 5,44>::value, _line< 5,45>::value, _line< 5,46>::value, _line< 5,47>::value,
    _line< 5,48>::value, _line< 5,49>::value, _line< 5,50>::value, _line< 5,51>::value, _line< 5,52>::value, _line< 5,53>::value, _line< 5,54>::value, _line< 5,55>::value,
    _line< 5,56>::value, _line< 5,57>::value, _line< 5,58>::value, _line< 5,59>::value, _line< 5,60>::value, _line< 5,61>::value, _line< 5,62>::value, _line< 5,63>::value,
    _line< 6, 0>::value, _line< 6, 1>::value, _line< 6, 2>::value, _line< 6, 3>::value, _line< 6, 4>::value, _line< 6, 5>::value, _line< 6, 6>::value, _line< 6, 7>::value,
    _line< 6, 8>::value, _line< 6, 9>::value, _line< 6,10>::value, _line< 6,11>::value, _line< 6,12>::value, _line< 6,13>::value, _line< 6,14>::value, _line< 6,15>::value,
    _line< 6,16>::value, _line< 6,17>::value, _line< 6,18>::value, _line< 6,19>::value, _line< 6,20>::value, _line< 6,21>::value, _line< 6,22>::value, _line< 6,23>::value,
    _line< 6,24>::value, _line< 6,25>::value, _line< 6,26>::value, _line< 6,27>::value, _line< 6,28>::value, _line< 6,29>::value, _line< 6,30>::value, _line< 6,31>::value,
    _line< 6,32>::value, _line< 6,33>::value, _line< 6,34>::value, _line< 6,35>::value, _line< 6,36>::value, _line< 6,37>::value, _line< 6,38>::value, _line< 6,39>::value,
    _line< 6,40>::value, _line< 6,41>::value, _line< 6,42>::value, _line< 6,43>::value, _line< 6,44>::value, _line< 6,45>::value, _line< 6,46>::value, _line< 6,47>::value,
    _line< 6,48>::value, _line< 6,49>::value, _line< 6,50>::value, _line< 6,51>::value, _line< 6,52>::value, _line< 6,53>::value, _line< 6,54>::value, _line< 6,55>::value,
    _line< 6,56>::value, _line< 6,57>::value, _line< 6,58>::value, _line< 6,59>::value, _line< 6,60>::value, _line< 6,61>::value, _line< 6,62>::value, _line< 6,63>::value,
    _line< 7, 0>::value, _line< 7, 1>::value, _line< 7, 2>::value, _line< 7, 3>::value, _line< 7, 4>::value, _line< 7, 5>::value, _line< 7, 6>::value, _line< 7, 7>::value,
    _line< 7, 8>::value, _line< 7, 9>::value, _line< 7,10>::value, _line< 7,11>::value, _line< 7,12>::value, _line< 7,13>::value, _line< 7,14>::value, _line< 7,15>::value,
    _line< 7,16>::value, _line< 7,17>::value, _line< 7,18>::value, _line< 7,19>::value, _line< 7,20>::value, _line< 7,21>::value, _line< 7,22>::value, _line< 7,23>::value,
    _line< 7,24>::value, _line< 7,25>::value, _line< 7,26>::value, _line< 7,27>::value, _line< 7,28>::value, _line< 7,29>::value, _line< 7,30>::value, _line< 7,31>::value,
    _line< 7,32>::value, _line< 7,33>::value, _line< 7,34>::value, _line< 7,35>::value, _line< 7,36>::value, _line< 7,37>::value, _line< 7,38>::value, _line< 7,39>::value,
    _line< 7,40>::value, _line< 7,41>::value, _line< 7,42>::value, _line< 7,43>::value, _line< 7,44>::value, _line< 7,45>::value, _line< 7,46>::value, _line< 7,47>::value,
    _line< 7,48>::value, _line< 7,49>::value, _line< 7,50>::value, _line< 7,51>::value, _line< 7,52>::value, _line< 7,53>::value, _line< 7,54>::value, _line< 7,55>::value,
    _line< 7,56>::value, _line< 7,57>::value, _line< 7,58>::value, _line< 7,59>::value, _line< 7,60>::value, _line< 7,61>::value, _line< 7,62>::value, _line< 7,63>::value,
    _line< 8, 0>::value, _line< 8, 1>::value, _line< 8, 2>::value, _line< 8, 3>::value, _line< 8, 4>::value, _line< 8, 5>::value, _line< 8, 6>::value, _line< 8, 7>::value,
    _line< 8, 8>::value, _line< 8, 9>::value, _line< 8,10>::value, _line< 8,11>::value, _line< 8,12>::value, _line< 8,13>::value, _line< 8,14>::value, _line< 8,15>::value,
    _line< 8,16>::value, _line< 8,17>::value, _line< 8,18>::value, _line< 8,19>::value, _line< 8,20>::value, _line< 8,21>::value, _line< 8,22>::value, _line< 8,23>::value,
    _line< 8,24>::value, _line< 8,25>::value, _line< 8,26>::value, _line< 8,27>::value, _line< 8,28>::value, _line< 8,29>::value, _line< 8,30>::value, _line< 8,31>::value,
    _line< 8,32>::value, _line< 8,33>::value, _line< 8,34>::value, _line< 8,35>::value, _line< 8,36>::value, _line< 8,37>::value, _line< 8,38>::value, _line< 8,39>::value,
    _line< 8,40>::value, _line< 8,41>::value, _line< 8,42>::value, _line< 8,43>::value, _line< 8,44>::value, _line< 8,45>::value, _line< 8,46>::value, _line< 8,47>::value,
    _line< 8,48>::value, _line< 8,49>::value, _line< 8,50>::value, _line< 8,51>::value, _line< 8,52>::value, _line< 8,53>::value, _line< 8,54>::value, _line< 8,55>::value,
    _line< 8,56>::value, _line< 8,57>::value, _line< 8,58>::value, _line< 8,59>::value, _line< 8,60>::value, _line< 8,61>::value, _line< 8,62>::value, _line< 8,63>::value,
    _line< 9, 0>::value, _line< 9, 1>::value, _line< 9, 2>::value, _line< 9, 3>::value, _line< 9, 4>::value, _line< 9, 5>::value, _line< 9, 6>::value, _line< 9, 7>::value,
    _line< 9, 8>::value, _line< 9, 9>::value, _line< 9,10>::value, _line< 9,11>::value, _line< 9,12>::value, _line< 9,13>::value, _line< 9,14>::value, _line< 9,15>::value,
    _line< 9,16>::value, _line< 9,17>::value, _line< 9,18>::value, _line< 9,19>::value, _line< 9,20>::value, _line< 9,21>::value, _line< 9,22>::value, _line< 9,23>::value,
    _line< 9,24>::value, _line< 9,25>::value, _line< 9,26>::value, _line< 9,27>::value, _line< 9,28>::value, _line< 9,29>::value, _line< 9,30>::value, _line< 9,31>::value,
    _line< 9,32>::value, _line< 9,33>::value, _line< 9,34>::value, _line< 9,35>::value, _line< 9,36>::value, _line< 9,37>::value, _line< 9,38>::value, _line< 9,39>::value,
    _line< 9,40>::value, _line< 9,41>::value, _line< 9,42>::value, _line< 9,43>::value, _line< 9,44>::value, _line< 9,45>::value, _line< 9,46>::value, _line< 9,47>::value,
    _line< 9,48>::value, _line< 9,49>::value, _line< 9,50>::value, _line< 9,51>::value, _line< 9,52>::value, _line< 9,53>::value, _line< 9,54>::value, _line< 9,55>::value,
    _line< 9,56>::value, _line< 9,57>::value, _line< 9,58>::value, _line< 9,59>::value, _line< 9,60>::value, _line< 9,61>::value, _line< 9,62>::value, _line< 9,63>::value,
    _line<10, 0>::value, _line<10, 1>::value, _line<10, 2>::value, _line<10, 3>::value, _line<10, 4>::value, _line<10, 5>::value, _line<10, 6>::value, _line<10, 7>::value,
    _line<10, 8>::value, _line<10, 9>::value, _line<10,10>::value, _line<10,11>::value, _line<10,12>::value, _line<10,13>::value, _line<10,14>::value, _line<10,15>::value,
    _line<10,16>::value, _line<10,17>::value, _line<10,18>::value, _line<10,19>::value, _line<10,20>::value, _line<10,21>::value, _line<10,22>::value, _line<10,23>::value,
    _line<10,24>::value, _line<10,25>::value, _line<10,26>::value, _line<10,27>::value, _line<10,28>::value, _line<10,29>::value, _line<10,30>::value, _line<10,31>::value,
    _line<10,32>::value, _line<10,33>::value, _line<10,34>::value, _line<10,35>::value, _line<10,36>::value, _line<10,37>::value, _line<10,38>::value, _line<10,39>::value,
    _line<10,40>::value, _line<10,41>::value, _line<10,42>::value, _line<10,43>::value, _line<10,44>::value, _line<10,45>::value, _line<10,46>::value, _line<10,47>::value,
    _line<10,48>::value, _line<10,49>::value, _line<10,50>::value, _line<10,51>::value, _line<10,52>::value, _line<10,53>::value, _line<10,54>::value, _line<10,55>::value,
    _line<10,56>::value, _line<10,57>::value, _line<10,58>::value, _line<10,59>::value, _line<10,60>::value, _line<10,61>::value, _line<10,62>::value, _line<10,63>::value,
    _line<11, 0>::value, _line<11, 1>::value, _line<11, 2>::value, _line<11, 3>::value, _line<11, 4>::value, _line<11, 5>::value, _line<11, 6>::value, _line<11, 7>::value,
    _line<11, 8>::value, _line<11, 9>::value, _line<11,10>::value, _line<11,11>::value, _line<11,12>::value, _line<11,13>::value, _line<11,14>::value, _line<11,15>::value,
    _line<11,16>::value, _line<11,17>::value, _line<11,18>::value, _line<11,19>::value, _line<11,20>::value, _line<11,21>::value, _line<11,22>::value, _line<11,23>::value,
    _line<11,24>::value, _line<11,25>::value, _line<11,26>::value, _line<11,27>::value, _line<11,28>::value, _line<11,29>::value, _line<11,30>::value, _line<11,31>::value,
    _line<11,32>::value, _line<11,33>::value, _line<11,34>::value, _line<11,35>::value, _line<11,36>::value, _line<11,37>::value, _line<11,38>::value, _line<11,39>::value,
    _line<11,40>::value, _line<11,41>::value, _line<11,42>::value, _line<11,43>::value, _line<11,44>::value, _line<11,45>::value, _line<11,46>::value, _line<11,47>::value,
    _line<11,48>::value, _line<11,49>::value, _line<11,50>::value, _line<11,51>::value, _line<11,52>::value, _line<11,53>::value, _line<11,54>::value, _line<11,55>::value,
    _line<11,56>::value, _line<11,57>::value, _line<11,58>::value, _line<11,59>::value, _line<11,60>::value, _line<11,61>::value, _line<11,62>::value, _line<11,63>::value,
    _line<12, 0>::value, _line<12, 1>::value, _line<12, 2>::value, _line<12, 3>::value, _line<12, 4>::value, _line<12, 5>::value, _line<12, 6>::value, _line<12, 7>::value,
    _line<12, 8>::value, _line<12, 9>::value, _line<12,10>::value, _line<12,11>::value, _line<12,12>::value, _line<12,13>::value, _line<12,14>::value, _line<12,15>::value,
    _line<12,16>::value, _line<12,17>::value, _line<12,18>::value, _line<12,19>::value, _line<12,20>::value, _line<12,21>::value, _line<12,22>::value, _line<12,23>::value,
    _line<12,24>::value, _line<12,25>::value, _line<12,26>::value, _line<12,27>::value, _line<12,28>::value, _line<12,29>::value, _line<12,30>::value, _line<12,31>::value,
    _line<12,32>::value, _line<12,33>::value, _line<12,34>::value, _line<12,35>::value, _line<12,36>::value, _line<12,37>::value, _line<12,38>::value, _line<12,39>::value,
    _line<12,40>::value, _line<12,41>::value, _line<12,42>::value, _line<12,43>::value, _line<12,44>::value, _line<12,45>::value, _line<12,46>::value, _line<12,47>::value,
    _line<12,48>::value, _line<12,49>::value, _line<12,50>::value, _line<12,51>::value, _line<12,52>::value, _line<12,53>::value, _line<12,54>::value, _line<12,55>::value,
    _line<12,56>::value, _line<12,57>::value, _line<12,58>::value, _line<12,59>::value, _line<12,60>::value, _line<12,61>::value, _line<12,62>::value, _line<12,63>::value,
    _line<13, 0>::value, _line<13, 1>::value, _line<13, 2>::value, _line<13, 3>::value, _line<13, 4>::value, _line<13, 5>::value, _line<13, 6>::value, _line<13, 7>::value,
    _line<13, 8>::value, _line<13, 9>::value, _line<13,10>::value, _line<13,11>::value, _line<13,12>::value, _line<13,13>::value, _line<13,14>::value, _line<13,15>::value,
    _line<13,16>::value, _line<13,17>::value, _line<13,18>::value, _line<13,19>::value, _line<13,20>::value, _line<13,21>::value, _line<13,22>::value, _line<13,23>::value,
    _line<13,24>::value, _line<13,25>::value, _line<13,26>::value, _line<13,27>::value, _line<13,28>::value, _line<13,29>::value, _line<13,30>::value, _line<13,31>::value,
    _line<13,32>::value, _line<13,33>::value, _line<13,34>::value, _line<13,35>::value, _line<13,36>::value, _line<13,37>::value, _line<13,38>::value, _line<13,39>::value,
    _line<13,40>::value, _line<13,41>::value, _line<13,42>::value, _line<13,43>::value, _line<13,44>::value, _line<13,45>::value, _line<13,46>::value, _line<13,47>::value,
    _line<13,48>::value, _line<13,49>::value, _line<13,50>::value, _line<13,51>::value, _line<13,52>::value, _line<13,53>::value, _line<13,54>::value, _line<13,55>::value,
    _line<13,56>::value, _line<13,57>::value, _line<13,58>::value, _line<13,59>::value, _line<13,60>::value, _line<13,61>::value, _line<13,62>::value, _line<13,63>::value,
    _line<14, 0>::value, _line<14, 1>::value, _line<14, 2>::value, _line<14, 3>::value, _line<14, 4>::value, _line<14, 5>::value, _line<14, 6>::value, _line<14, 7>::value,
    _line<14, 8>::value, _line<14, 9>::value, _line<14,10>::value, _line<14,11>::value, _line<14,12>::value, _line<14,13>::value, _line<14,14>::value, _line<14,15>::value,
    _line<14,16>::value, _line<14,17>::value, _line<14,18>::value, _line<14,19>::value, _line<14,20>::value, _line<14,21>::value, _line<14,22>::value, _line<14,23>::value,
    _line<14,24>::value, _line<14,25>::value, _line<14,26>::value, _line<14,27>::value, _line<14,28>::value, _line<14,29>::value, _line<14,30>::value, _line<14,31>::value,
    _line<14,32>::value, _line<14,33>::value, _line<14,34>::value, _line<14,35>::value, _line<14,36>::value, _line<14,37>::value, _line<14,38>::value, _line<14,39>::value,
    _line<14,40>::value, _line<14,41>::value, _line<14,42>::value, _line<14,43>::value, _line<14,44>::value, _line<14,45>::value, _line<14,46>::value, _line<14,47>::value,
    _line<14,48>::value, _line<14,49>::value, _line<14,50>::value, _line<14,51>::value, _line<14,52>::value, _line<14,53>::value, _line<14,54>::value, _line<14,55>::value,
    _line<14,56>::value, _line<14,57>::value, _line<14,58>::value, _line<14,59>::value, _line<14,60>::value, _line<14,61>::value, _line<14,62>::value, _line<14,63>::value,
    _line<15, 0>::value, _line<15, 1>::value, _line<15, 2>::value, _line<15, 3>::value, _line<15, 4>::value, _line<15, 5>::value, _line<15, 6>::value, _line<15, 7>::value,
    _line<15, 8>::value, _line<15, 9>::value, _line<15,10>::value, _line<15,11>::value, _line<15,12>::value, _line<15,13>::value, _line<15,14>::value, _line<15,15>::value,
    _line<15,16>::value, _line<15,17>::value, _line<15,18>::value, _line<15,19>::value, _line<15,20>::value, _line<15,21>::value, _line<15,22>::value, _line<15,23>::value,
    _line<15,24>::value, _line<15,25>::value, _line<15,26>::value, _line<15,27>::value, _line<15,28>::value, _line<15,29>::value, _line<15,30>::value, _line<15,31>::value,
    _line<15,32>::value, _line<15,33>::value, _line<15,34>::value, _line<15,35>::value, _line<15,36>::value, _line<15,37>::value, _line<15,38>::value, _line<15,39>::value,
    _line<15,40>::value, _line<15,41>::value, _line<15,42>::value, _line<15,43>::value, _line<15,44>::value, _line<15,45>::value, _line<15,46>::value, _line<15,47>::value,
    _line<15,48>::value, _line<15,49>::value, _line<15,50>::value, _line<15,51>::value, _line<15,52>::value, _line<15,53>::value, _line<15,54>::value, _line<15,55>::value,
    _line<15,56>::value, _line<15,57>::value, _line<15,58>::value, _line<15,59>::value, _line<15,60>::value, _line<15,61>::value, _line<15,62>::value, _line<15,63>::value,
    _line<16, 0>::value, _line<16, 1>::value, _line<16, 2>::value, _line<16, 3>::value, _line<16, 4>::value, _line<16, 5>::value, _line<16, 6>::value, _line<16, 7>::value,
    _line<16, 8>::value, _line<16, 9>::value, _line<16,10>::value, _line<16,11>::value, _line<16,12>::value, _line<16,13>::value, _line<16,14>::value, _line<16,15>::value,
    _line<16,16>::value, _line<16,17>::value, _line<16,18>::value, _line<16,19>::value, _line<16,20>::value, _line<16,21>::value, _line<16,22>::value, _line<16,23>::value,
    _line<16,24>::value, _line<16,25>::value, _line<16,26>::value, _line<16,27>::value, _line<16,28>::value, _line<16,29>::value, _line<16,30>::value, _line<16,31>::value,
    _line<16,32>::value, _line<16,33>::value, _line<16,34>::value, _line<16,35>::value, _line<16,36>::value, _line<16,37>::value, _line<16,38>::value, _line<16,39>::value,
    _line<16,40>::value, _line<16,41>::value, _line<16,42>::value, _line<16,43>::value, _line<16,44>::value, _line<16,45>::value, _line<16,46>::value, _line<16,47>::value,
    _line<16,48>::value, _line<16,49>::value, _line<16,50>::value, _line<16,51>::value, _line<16,52>::value, _line<16,53>::value, _line<16,54>::value, _line<16,55>::value,
    _line<16,56>::value, _line<16,57>::value, _line<16,58>::value, _line<16,59>::value, _line<16,60>::value, _line<16,61>::value, _line<16,62>::value, _line<16,63>::value,
    _line<17, 0>::value, _line<17, 1>::value, _line<17, 2>::value, _line<17, 3>::value, _line<17, 4>::value, _line<17, 5>::value, _line<17, 6>::value, _line<17, 7>::value,
    _line<17, 8>::value, _line<17, 9>::value, _line<17,10>::value, _line<17,11>::value, _line<17,12>::value, _line<17,13>::value, _line<17,14>::value, _line<17,15>::value,
    _line<17,16>::value, _line<17,17>::value, _line<17,18>::value, _line<17,19>::value, _line<17,20>::value, _line<17,21>::value, _line<17,22>::value, _line<17,23>::value,
    _line<17,24>::value, _line<17,25>::value, _line<17,26>::value, _line<17,27>::value, _line<17,28>::value, _line<17,29>::value, _line<17,30>::value, _line<17,31>::value,
    _line<17,32>::value, _line<17,33>::value, _line<17,34>::value, _line<17,35>::value, _line<17,36>::value, _line<17,37>::value, _line<17,38>::value, _line<17,39>::value,
    _line<17,40>::value, _line<17,41>::value, _line<17,42>::value, _line<17,43>::value, _line<17,44>::value, _line<17,45>::value, _line<17,46>::value, _line<17,47>::value,
    _line<17,48>::value, _line<17,49>::value, _line<17,50>::value, _line<17,51>::value, _line<17,52>::value, _line<17,53>::value, _line<17,54>::value, _line<17,55>::value,
    _line<17,56>::value, _line<17,57>::value, _line<17,58>::value, _line<17,59>::value, _line<17,60>::value, _line<17,61>::value, _line<17,62>::value, _line<17,63>::value,
    _line<18, 0>::value, _line<18, 1>::value, _line<18, 2>::value, _line<18, 3>::value, _line<18, 4>::value, _line<18, 5>::value, _line<18, 6>::value, _line<18, 7>::value,
    _line<18, 8>::value, _line<18, 9>::value, _line<18,10>::value, _line<18,11>::value, _line<18,12>::value, _line<18,13>::value, _line<18,14>::value, _line<18,15>::value,
    _line<18,16>::value, _line<18,17>::value, _line<18,18>::value, _line<18,19>::value, _line<18,20>::value, _line<18,21>::value, _line<18,22>::value, _line<18,23>::value,
    _line<18,24>::value, _line<18,25>::value, _line<18,26>::value, _line<18,27>::value, _line<18,28>::value, _line<18,29>::value, _line<18,30>::value, _line<18,31>::value,
    _line<18,32>::value, _line<18,33>::value, _line<18,34>::value, _line<18,35>::value, _line<18,36>::value, _line<18,37>::value, _line<18,38>::value, _line<18,39>::value,
    _line<18,40>::value, _line<18,41>::value, _line<18,42>::value, _line<18,43>::value, _line<18,44>::value, _line<18,45>::value, _line<18,46>::value, _line<18,47>::value,
    _line<18,48>::value, _line<18,49>::value, _line<18,50>::value, _line<18,51>::value, _line<18,52>::value, _line<18,53>::value, _line<18,54>::value, _line<18,55>::value,
    _line<18,56>::value, _line<18,57>::value, _line<18,58>::value, _line<18,59>::value, _line<18,60>::value, _line<18,61>::value, _line<18,62>::value, _line<18,63>::value,
    _line<19, 0>::value, _line<19, 1>::value, _line<19, 2>::value, _line<19, 3>::value, _line<19, 4>::value, _line<19, 5>::value, _line<19, 6>::value, _line<19, 7>::value,
    _line<19, 8>::value, _line<19, 9>::value, _line<19,10>::value, _line<19,11>::value, _line<19,12>::value, _line<19,13>::value, _line<19,14>::value, _line<19,15>::value,
    _line<19,16>::value, _line<19,17>::value, _line<19,18>::value, _line<19,19>::value, _line<19,20>::value, _line<19,21>::value, _line<19,22>::value, _line<19,23>::value,
    _line<19,24>::value, _line<19,25>::value, _line<19,26>::value, _line<19,27>::value, _line<19,28>::value, _line<19,29>::value, _line<19,30>::value, _line<19,31>::value,
    _line<19,32>::value, _line<19,33>::value, _line<19,34>::value, _line<19,35>::value, _line<19,36>::value, _line<19,37>::value, _line<19,38>::value, _line<19,39>::value,
    _line<19,40>::value, _line<19,41>::value, _line<19,42>::value, _line<19,43>::value, _line<19,44>::value, _line<19,45>::value, _line<19,46>::value, _line<19,47>::value,
    _line<19,48>::value, _line<19,49>::value, _line<19,50>::value, _line<19,51>::value, _line<19,52>::value, _line<19,53>::value, _line<19,54>::value, _line<19,55>::value,
    _line<19,56>::value, _line<19,57>::value, _line<19,58>::value, _line<19,59>::value, _line<19,60>::value, _line<19,61>::value, _line<19,62>::value, _line<19,63>::value,
    _line<20, 0>::value, _line<20, 1>::value, _line<20, 2>::value, _line<20, 3>::value, _line<20, 4>::value, _line<20, 5>::value, _line<20, 6>::value, _line<20, 7>::value,
    _line<20, 8>::value, _line<20, 9>::value, _line<20,10>::value, _line<20,11>::value, _line<20,12>::value, _line<20,13>::value, _line<20,14>::value, _line<20,15>::value,
    _line<20,16>::value, _line<20,17>::value, _line<20,18>::value, _line<20,19>::value, _line<20,20>::value, _line<20,21>::value, _line<20,22>::value, _line<20,23>::value,
    _line<20,24>::value, _line<20,25>::value, _line<20,26>::value, _line<20,27>::value, _line<20,28>::value, _line<20,29>::value, _line<20,30>::value, _line<20,31>::value,
    _line<20,32>::value, _line<20,33>::value, _line<20,34>::value, _line<20,35>::value, _line<20,36>::value, _line<20,37>::value, _line<20,38>::value, _line<20,39>::value,
    _line<20,40>::value, _line<20,41>::value, _line<20,42>::value, _line<20,43>::value, _line<20,44>::value, _line<20,45>::value, _line<20,46>::value, _line<20,47>::value,
    _line<20,48>::value, _line<20,49>::value, _line<20,50>::value, _line<20,51>::value, _line<20,52>::value, _line<20,53>::value, _line<20,54>::value, _line<20,55>::value,
    _line<20,56>::value, _line<20,57>::value, _line<20,58>::value, _line<20,59>::value, _line<20,60>::value, _line<20,61>::value, _line<20,62>::value, _line<20,63>::value,
    _line<21, 0>::value, _line<21, 1>::value, _line<21, 2>::value, _line<21, 3>::value, _line<21, 4>::value, _line<21, 5>::value, _line<21, 6>::value, _line<21, 7>::value,
    _line<21, 8>::value, _line<21, 9>::value, _line<21,10>::value, _line<21,11>::value, _line<21,12>::value, _line<21,13>::value, _line<21,14>::value, _line<21,15>::value,
    _line<21,16>::value, _line<21,17>::value, _line<21,18>::value, _line<21,19>::value, _line<21,20>::value, _line<21,21>::value, _line<21,22>::value, _line<21,23>::value,
    _line<21,24>::value, _line<21,25>::value, _line<21,26>::value, _line<21,27>::value, _line<21,28>::value, _line<21,29>::value, _line<21,30>::value, _line<21,31>::value,
    _line<21,32>::value, _line<21,33>::value, _line<21,34>::value, _line<21,35>::value, _line<21,36>::value, _line<21,37>::value, _line<21,38>::value, _line<21,39>::value,
    _line<21,40>::value, _line<21,41>::value, _line<21,42>::value, _line<21,43>::value, _line<21,44>::value, _line<21,45>::value, _line<21,46>::value, _line<21,47>::value,
    _line<21,48>::value, _line<21,49>::value, _line<21,50>::value, _line<21,51>::value, _line<21,52>::value, _line<21,53>::value, _line<21,54>::value, _line<21,55>::value,
    _line<21,56>::value, _line<21,57>::value, _line<21,58>::value, _line<21,59>::value, _line<21,60>::value, _line<21,61>::value, _line<21,62>::value, _line<21,63>::value,
    _line<22, 0>::value, _line<22, 1>::value, _line<22, 2>::value, _line<22, 3>::value, _line<22, 4>::value, _line<22, 5>::value, _line<22, 6>::value, _line<22, 7>::value,
    _line<22, 8>::value, _line<22, 9>::value, _line<22,10>::value, _line<22,11>::value, _line<22,12>::value, _line<22,13>::value, _line<22,14>::value, _line<22,15>::value,
    _line<22,16>::value, _line<22,17>::value, _line<22,18>::value, _line<22,19>::value, _line<22,20>::value, _line<22,21>::value, _line<22,22>::value, _line<22,23>::value,
    _line<22,24>::value, _line<22,25>::value, _line<22,26>::value, _line<22,27>::value, _line<22,28>::value, _line<22,29>::value, _line<22,30>::value, _line<22,31>::value,
    _line<22,32>::value, _line<22,33>::value, _line<22,34>::value, _line<22,35>::value, _line<22,36>::value, _line<22,37>::value, _line<22,38>::value, _line<22,39>::value,
    _line<22,40>::value, _line<22,41>::value, _line<22,42>::value, _line<22,43>::value, _line<22,44>::value, _line<22,45>::value, _line<22,46>::value, _line<22,47>::value,
    _line<22,48>::value, _line<22,49>::value, _line<22,50>::value, _line<22,51>::value, _line<22,52>::value, _line<22,53>::value, _line<22,54>::value, _line<22,55>::value,
    _line<22,56>::value, _line<22,57>::value, _line<22,58>::value, _line<22,59>::value, _line<22,60>::value, _line<22,61>::value, _line<22,62>::value, _line<22,63>::value,
    _line<23, 0>::value, _line<23, 1>::value, _line<23, 2>::value, _line<23, 3>::value, _line<23, 4>::value, _line<23, 5>::value, _line<23, 6>::value, _line<23, 7>::value,
    _line<23, 8>::value, _line<23, 9>::value, _line<23,10>::value, _line<23,11>::value, _line<23,12>::value, _line<23,13>::value, _line<23,14>::value, _line<23,15>::value,
    _line<23,16>::value, _line<23,17>::value, _line<23,18>::value, _line<23,19>::value, _line<23,20>::value, _line<23,21>::value, _line<23,22>::value, _line<23,23>::value,
    _line<23,24>::value, _line<23,25>::value, _line<23,26>::value, _line<23,27>::value, _line<23,28>::value, _line<23,29>::value, _line<23,30>::value, _line<23,31>::value,
    _line<23,32>::value, _line<23,33>::value, _line<23,34>::value, _line<23,35>::value, _line<23,36>::value, _line<23,37>::value, _line<23,38>::value, _line<23,39>::value,
    _line<23,40>::value, _line<23,41>::value, _line<23,42>::value, _line<23,43>::value, _line<23,44>::value, _line<23,45>::value, _line<23,46>::value, _line<23,47>::value,
    _line<23,48>::value, _line<23,49>::value, _line<23,50>::value, _line<23,51>::value, _line<23,52>::value, _line<23,53>::value, _line<23,54>::value, _line<23,55>::value,
    _line<23,56>::value, _line<23,57>::value, _line<23,58>::value, _line<23,59>::value, _line<23,60>::value, _line<23,61>::value, _line<23,62>::value, _line<23,63>::value,
    _line<24, 0>::value, _line<24, 1>::value, _line<24, 2>::value, _line<24, 3>::value, _line<24, 4>::value, _line<24, 5>::value, _line<24, 6>::value, _line<24, 7>::value,
    _line<24, 8>::value, _line<24, 9>::value, _line<24,10>::value, _line<24,11>::value, _line<24,12>::value, _line<24,13>::value, _line<24,14>::value, _line<24,15>::value,
    _line<24,16>::value, _line<24,17>::value, _line<24,18>::value, _line<24,19>::value, _line<24,20>::value, _line<24,21>::value, _line<24,22>::value, _line<24,23>::value,
    _line<24,24>::value, _line<24,25>::value, _line<24,26>::value, _line<24,27>::value, _line<24,28>::value, _line<24,29>::value, _line<24,30>::value, _line<24,31>::value,
    _line<24,32>::value, _line<24,33>::value, _line<24,34>::value, _line<24,35>::value, _line<24,36>::value, _line<24,37>::value, _line<24,38>::value, _line<24,39>::value,
    _line<24,40>::value, _line<24,41>::value, _line<24,42>::value, _line<24,43>::value, _line<24,44>::value, _line<24,45>::value, _line<24,46>::value, _line<24,47>::value,
    _line<24,48>::value, _line<24,49>::value, _line<24,50>::value, _line<24,51>::value, _line<24,52>::value, _line<24,53>::value, _line<24,54>::value, _line<24,55>::value,
    _line<24,56>::value, _line<24,57>::value, _line<24,58>::value, _line<24,59>::value, _line<24,60>::value, _line<24,61>::value, _line<24,62>::value, _line<24,63>::value,
    _line<25, 0>::value, _line<25, 1>::value, _line<25, 2>::value, _line<25, 3>::value, _line<25, 4>::value, _line<25, 5>::value, _line<25, 6>::value, _line<25, 7>::value,
    _line<25, 8>::value, _line<25, 9>::value, _line<25,10>::value, _line<25,11>::value, _line<25,12>::value, _line<25,13>::value, _line<25,14>::value, _line<25,15>::value,
    _line<25,16>::value, _line<25,17>::value, _line<25,18>::value, _line<25,19>::value, _line<25,20>::value, _line<25,21>::value, _line<25,22>::value, _line<25,23>::value,
    _line<25,24>::value, _line<25,25>::value, _line<25,26>::value, _line<25,27>::value, _line<25,28>::value, _line<25,29>::value, _line<25,30>::value, _line<25,31>::value,
    _line<25,32>::value, _line<25,33>::value, _line<25,34>::value, _line<25,35>::value, _line<25,36>::value, _line<25,37>::value, _line<25,38>::value, _line<25,39>::value,
    _line<25,40>::value, _line<25,41>::value, _line<25,42>::value, _line<25,43>::value, _line<25,44>::value, _line<25,45>::value, _line<25,46>::value, _line<25,47>::value,
    _line<25,48>::value, _line<25,49>::value, _line<25,50>::value, _line<25,51>::value, _line<25,52>::value, _line<25,53>::value, _line<25,54>::value, _line<25,55>::value,
    _line<25,56>::value, _line<25,57>::value, _line<25,58>::value, _line<25,59>::value, _line<25,60>::value, _line<25,61>::value, _line<25,62>::value, _line<25,63>::value,
    _line<26, 0>::value, _line<26, 1>::value, _line<26, 2>::value, _line<26, 3>::value, _line<26, 4>::value, _line<26, 5>::value, _line<26, 6>::value, _line<26, 7>::value,
    _line<26, 8>::value, _line<26, 9>::value, _line<26,10>::value, _line<26,11>::value, _line<26,12>::value, _line<26,13>::value, _line<26,14>::value, _line<26,15>::value,
    _line<26,16>::value, _line<26,17>::value, _line<26,18>::value, _line<26,19>::value, _line<26,20>::value, _line<26,21>::value, _line<26,22>::value, _line<26,23>::value,
    _line<26,24>::value, _line<26,25>::value, _line<26,26>::value, _line<26,27>::value, _line<26,28>::value, _line<26,29>::value, _line<26,30>::value, _line<26,31>::value,
    _line<26,32>::value, _line<26,33>::value, _line<26,34>::value, _line<26,35>::value, _line<26,36>::value, _line<26,37>::value, _line<26,38>::value, _line<26,39>::value,
    _line<26,40>::value, _line<26,41>::value, _line<26,42>::value, _line<26,43>::value, _line<26,44>::value, _line<26,45>::value, _line<26,46>::value, _line<26,47>::value,
    _line<26,48>::value, _line<26,49>::value, _line<26,50>::value, _line<26,51>::value, _line<26,52>::value, _line<26,53>::value, _line<26,54>::value, _line<26,55>::value,
    _line<26,56>::value, _line<26,57>::value, _line<26,58>::value, _line<26,59>::value, _line<26,60>::value, _line<26,61>::value, _line<26,62>::value, _line<26,63>::value,
    _line<27, 0>::value, _line<27, 1>::value, _line<27, 2>::value, _line<27, 3>::value, _line<27, 4>::value, _line<27, 5>::value, _line<27, 6>::value, _line<27, 7>::value,
    _line<27, 8>::value, _line<27, 9>::value, _line<27,10>::value, _line<27,11>::value, _line<27,12>::value, _line<27,13>::value, _line<27,14>::value, _line<27,15>::value,
    _line<27,16>::value, _line<27,17>::value, _line<27,18>::value, _line<27,19>::value, _line<27,20>::value, _line<27,21>::value, _line<27,22>::value, _line<27,23>::value,
    _line<27,24>::value, _line<27,25>::value, _line<27,26>::value, _line<27,27>::value, _line<27,28>::value, _line<27,29>::value, _line<27,30>::value, _line<27,31>::value,
    _line<27,32>::value, _line<27,33>::value, _line<27,34>::value, _line<27,35>::value, _line<27,36>::value, _line<27,37>::value, _line<27,38>::value, _line<27,39>::value,
    _line<27,40>::value, _line<27,41>::value, _line<27,42>::value, _line<27,43>::value, _line<27,44>::value, _line<27,45>::value, _line<27,46>::value, _line<27,47>::value,
    _line<27,48>::value, _line<27,49>::value, _line<27,50>::value, _line<27,51>::value, _line<27,52>::value, _line<27,53>::value, _line<27,54>::value, _line<27,55>::value,
    _line<27,56>::value, _line<27,57>::value, _line<27,58>::value, _line<27,59>::value, _line<27,60>::value, _line<27,61>::value, _line<27,62>::value, _line<27,63>::value,
    _line<28, 0>::value, _line<28, 1>::value, _line<28, 2>::value, _line<28, 3>::value, _line<28, 4>::value, _line<28, 5>::value, _line<28, 6>::value, _line<28, 7>::value,
    _line<28, 8>::value, _line<28, 9>::value, _line<28,10>::value, _line<28,11>::value, _line<28,12>::value, _line<28,13>::value, _line<28,14>::value, _line<28,15>::value,
    _line<28,16>::value, _line<28,17>::value, _line<28,18>::value, _line<28,19>::value, _line<28,20>::value, _line<28,21>::value, _line<28,22>::value, _line<28,23>::value,
    _line<28,24>::value, _line<28,25>::value, _line<28,26>::value, _line<28,27>::value, _line<28,28>::value, _line<28,29>::value, _line<28,30>::value, _line<28,31>::value,
    _line<28,32>::value, _line<28,33>::value, _line<28,34>::value, _line<28,35>::value, _line<28,36>::value, _line<28,37>::value, _line<28,38>::value, _line<28,39>::value,
    _line<28,40>::value, _line<28,41>::value, _line<28,42>::value, _line<28,43>::value, _line<28,44>::value, _line<28,45>::value, _line<28,46>::value, _line<28,47>::value,
    _line<28,48>::value, _line<28,49>::value, _line<28,50>::value, _line<28,51>::value, _line<28,52>::value, _line<28,53>::value, _line<28,54>::value, _line<28,55>::value,
    _line<28,56>::value, _line<28,57>::value, _line<28,58>::value, _line<28,59>::value, _line<28,60>::value, _line<28,61>::value, _line<28,62>::value, _line<28,63>::value,
    _line<29, 0>::value, _line<29, 1>::value, _line<29, 2>::value, _line<29, 3>::value, _line<29, 4>::value, _line<29, 5>::value, _line<29, 6>::value, _line<29, 7>::value,
    _line<29, 8>::value, _line<29, 9>::value, _line<29,10>::value, _line<29,11>::value, _line<29,12>::value, _line<29,13>::value, _line<29,14>::value, _line<29,15>::value,
    _line<29,16>::value, _line<29,17>::value, _line<29,18>::value, _line<29,19>::value, _line<29,20>::value, _line<29,21>::value, _line<29,22>::value, _line<29,23>::value,
    _line<29,24>::value, _line<29,25>::value, _line<29,26>::value, _line<29,27>::value, _line<29,28>::value, _line<29,29>::value, _line<29,30>::value, _line<29,31>::value,
    _line<29,32>::value, _line<29,33>::value, _line<29,34>::value, _line<29,35>::value, _line<29,36>::value, _line<29,37>::value, _line<29,38>::value, _line<29,39>::value,
    _line<29,40>::value, _line<29,41>::value, _line<29,42>::value, _line<29,43>::value, _line<29,44>::value, _line<29,45>::value, _line<29,46>::value, _line<29,47>::value,
    _line<29,48>::value, _line<29,49>::value, _line<29,50>::value, _line<29,51>::value, _line<29,52>::value, _line<29,53>::value, _line<29,54>::value, _line<29,55>::value,
    _line<29,56>::value, _line<29,57>::value, _line<29,58>::value, _line<29,59>::value, _line<29,60>::value, _line<29,61>::value, _line<29,62>::value, _line<29,63>::value,
    _line<30, 0>::value, _line<30, 1>::value, _line<30, 2>::value, _line<30, 3>::value, _line<30, 4>::value, _line<30, 5>::value, _line<30, 6>::value, _line<30, 7>::value,
    _line<30, 8>::value, _line<30, 9>::value, _line<30,10>::value, _line<30,11>::value, _line<30,12>::value, _line<30,13>::value, _line<30,14>::value, _line<30,15>::value,
    _line<30,16>::value, _line<30,17>::value, _line<30,18>::value, _line<30,19>::value, _line<30,20>::value, _line<30,21>::value, _line<30,22>::value, _line<30,23>::value,
    _line<30,24>::value, _line<30,25>::value, _line<30,26>::value, _line<30,27>::value, _line<30,28>::value, _line<30,29>::value, _line<30,30>::value, _line<30,31>::value,
    _line<30,32>::value, _line<30,33>::value, _line<30,34>::value, _line<30,35>::value, _line<30,36>::value, _line<30,37>::value, _line<30,38>::value, _line<30,39>::value,
    _line<30,40>::value, _line<30,41>::value, _line<30,42>::value, _line<30,43>::value, _line<30,44>::value, _line<30,45>::value, _line<30,46>::value, _line<30,47>::value,
    _line<30,48>::value, _line<30,49>::value, _line<30,50>::value, _line<30,51>::value, _line<30,52>::value, _line<30,53>::value, _line<30,54>::value, _line<30,55>::value,
    _line<30,56>::value, _line<30,57>::value, _line<30,58>::value, _line<30,59>::value, _line<30,60>::value, _line<30,61>::value, _line<30,62>::value, _line<30,63>::value,
    _line<31, 0>::value, _line<31, 1>::value, _line<31, 2>::value, _line<31, 3>::value, _line<31, 4>::value, _line<31, 5>::value, _line<31, 6>::value, _line<31, 7>::value,
    _line<31, 8>::value, _line<31, 9>::value, _line<31,10>::value, _line<31,11>::value, _line<31,12>::value, _line<31,13>::value, _line<31,14>::value, _line<31,15>::value,
    _line<31,16>::value, _line<31,17>::value, _line<31,18>::value, _line<31,19>::value, _line<31,20>::value, _line<31,21>::value, _line<31,22>::value, _line<31,23>::value,
    _line<31,24>::value, _line<31,25>::value, _line<31,26>::value, _line<31,27>::value, _line<31,28>::value, _line<31,29>::value, _line<31,30>::value, _line<31,31>::value,
    _line<31,32>::value, _line<31,33>::value, _line<31,34>::value, _line<31,35>::value, _line<31,36>::value, _line<31,37>::value, _line<31,38>::value, _line<31,39>::value,
    _line<31,40>::value, _line<31,41>::value, _line<31,42>::value, _line<31,43>::value, _line<31,44>::value, _line<31,45>::value, _line<31,46>::value, _line<31,47>::value,
    _line<31,48>::value, _line<31,49>::value, _line<31,50>::value, _line<31,51>::value, _line<31,52>::value, _line<31,53>::value, _line<31,54>::value, _line<31,55>::value,
    _line<31,56>::value, _line<31,57>::value, _line<31,58>::value, _line<31,59>::value, _line<31,60>::value, _line<31,61>::value, _line<31,62>::value, _line<31,63>::value,
    _line<32, 0>::value, _line<32, 1>::value, _line<32, 2>::value, _line<32, 3>::value, _line<32, 4>::value, _line<32, 5>::value, _line<32, 6>::value, _line<32, 7>::value,
    _line<32, 8>::value, _line<32, 9>::value, _line<32,10>::value, _line<32,11>::value, _line<32,12>::value, _line<32,13>::value, _line<32,14>::value, _line<32,15>::value,
    _line<32,16>::value, _line<32,17>::value, _line<32,18>::value, _line<32,19>::value, _line<32,20>::value, _line<32,21>::value, _line<32,22>::value, _line<32,23>::value,
    _line<32,24>::value, _line<32,25>::value, _line<32,26>::value, _line<32,27>::value, _line<32,28>::value, _line<32,29>::value, _line<32,30>::value, _line<32,31>::value,
    _line<32,32>::value, _line<32,33>::value, _line<32,34>::value, _line<32,35>::value, _line<32,36>::value, _line<32,37>::value, _line<32,38>::value, _line<32,39>::value,
    _line<32,40>::value, _line<32,41>::value, _line<32,42>::value, _line<32,43>::value, _line<32,44>::value, _line<32,45>::value, _line<32,46>::value, _line<32,47>::value,
    _line<32,48>::value, _line<32,49>::value, _line<32,50>::value, _line<32,51>::value, _line<32,52>::value, _line<32,53>::value, _line<32,54>::value, _line<32,55>::value,
    _line<32,56>::value, _line<32,57>::value, _line<32,58>::value, _line<32,59>::value, _line<32,60>::value, _line<32,61>::value, _line<32,62>::value, _line<32,63>::value,
    _line<33, 0>::value, _line<33, 1>::value, _line<33, 2>::value, _line<33, 3>::value, _line<33, 4>::value, _line<33, 5>::value, _line<33, 6>::value, _line<33, 7>::value,
    _line<33, 8>::value, _line<33, 9>::value, _line<33,10>::value, _line<33,11>::value, _line<33,12>::value, _line<33,13>::value, _line<33,14>::value, _line<33,15>::value,
    _line<33,16>::value, _line<33,17>::value, _line<33,18>::value, _line<33,19>::value, _line<33,20>::value, _line<33,21>::value, _line<33,22>::value, _line<33,23>::value,
    _line<33,24>::value, _line<33,25>::value, _line<33,26>::value, _line<33,27>::value, _line<33,28>::value, _line<33,29>::value, _line<33,30>::value, _line<33,31>::value,
    _line<33,32>::value, _line<33,33>::value, _line<33,34>::value, _line<33,35>::value, _line<33,36>::value, _line<33,37>::value, _line<33,38>::value, _line<33,39>::value,
    _line<33,40>::value, _line<33,41>::value, _line<33,42>::value, _line<33,43>::value, _line<33,44>::value, _line<33,45>::value, _line<33,46>::value, _line<33,47>::value,
    _line<33,48>::value, _line<33,49>::value, _line<33,50>::value, _line<33,51>::value, _line<33,52>::value, _line<33,53>::value, _line<33,54>::value, _line<33,55>::value,
    _line<33,56>::value, _line<33,57>::value, _line<33,58>::value, _line<33,59>::value, _line<33,60>::value, _line<33,61>::value, _line<33,62>::value, _line<33,63>::value,
    _line<34, 0>::value, _line<34, 1>::value, _line<34, 2>::value, _line<34, 3>::value, _line<34, 4>::value, _line<34, 5>::value, _line<34, 6>::value, _line<34, 7>::value,
    _line<34, 8>::value, _line<34, 9>::value, _line<34,10>::value, _line<34,11>::value, _line<34,12>::value, _line<34,13>::value, _line<34,14>::value, _line<34,15>::value,
    _line<34,16>::value, _line<34,17>::value, _line<34,18>::value, _line<34,19>::value, _line<34,20>::value, _line<34,21>::value, _line<34,22>::value, _line<34,23>::value,
    _line<34,24>::value, _line<34,25>::value, _line<34,26>::value, _line<34,27>::value, _line<34,28>::value, _line<34,29>::value, _line<34,30>::value, _line<34,31>::value,
    _line<34,32>::value, _line<34,33>::value, _line<34,34>::value, _line<34,35>::value, _line<34,36>::value, _line<34,37>::value, _line<34,38>::value, _line<34,39>::value,
    _line<34,40>::value, _line<34,41>::value, _line<34,42>::value, _line<34,43>::value, _line<34,44>::value, _line<34,45>::value, _line<34,46>::value, _line<34,47>::value,
    _line<34,48>::value, _line<34,49>::value, _line<34,50>::value, _line<34,51>::value, _line<34,52>::value, _line<34,53>::value, _line<34,54>::value, _line<34,55>::value,
    _line<34,56>::value, _line<34,57>::value, _line<34,58>::value, _line<34,59>::value, _line<34,60>::value, _line<34,61>::value, _line<34,62>::value, _line<34,63>::value,
    _line<35, 0>::value, _line<35, 1>::value, _line<35, 2>::value, _line<35, 3>::value, _line<35, 4>::value, _line<35, 5>::value, _line<35, 6>::value, _line<35, 7>::value,
    _line<35, 8>::value, _line<35, 9>::value, _line<35,10>::value, _line<35,11>::value, _line<35,12>::value, _line<35,13>::value, _line<35,14>::value, _line<35,15>::value,
    _line<35,16>::value, _line<35,17>::value, _line<35,18>::value, _line<35,19>::value, _line<35,20>::value, _line<35,21>::value, _line<35,22>::value, _line<35,23>::value,
    _line<35,24>::value, _line<35,25>::value, _line<35,26>::value, _line<35,27>::value, _line<35,28>::value, _line<35,29>::value, _line<35,30>::value, _line<35,31>::value,
    _line<35,32>::value, _line<35,33>::value, _line<35,34>::value, _line<35,35>::value, _line<35,36>::value, _line<35,37>::value, _line<35,38>::value, _line<35,39>::value,
    _line<35,40>::value, _line<35,41>::value, _line<35,42>::value, _line<35,43>::value, _line<35,44>::value, _line<35,45>::value, _line<35,46>::value, _line<35,47>::value,
    _line<35,48>::value, _line<35,49>::value, _line<35,50>::value, _line<35,51>::value, _line<35,52>::value, _line<35,53>::value, _line<35,54>::value, _line<35,55>::value,
    _line<35,56>::value, _line<35,57>::value, _line<35,58>::value, _line<35,59>::value, _line<35,60>::value, _line<35,61>::value, _line<35,62>::value, _line<35,63>::value,
    _line<36, 0>::value, _line<36, 1>::value, _line<36, 2>::value, _line<36, 3>::value, _line<36, 4>::value, _line<36, 5>::value, _line<36, 6>::value, _line<36, 7>::value,
    _line<36, 8>::value, _line<36, 9>::value, _line<36,10>::value, _line<36,11>::value, _line<36,12>::value, _line<36,13>::value, _line<36,14>::value, _line<36,15>::value,
    _line<36,16>::value, _line<36,17>::value, _line<36,18>::value, _line<36,19>::value, _line<36,20>::value, _line<36,21>::value, _line<36,22>::value, _line<36,23>::value,
    _line<36,24>::value, _line<36,25>::value, _line<36,26>::value, _line<36,27>::value, _line<36,28>::value, _line<36,29>::value, _line<36,30>::value, _line<36,31>::value,
    _line<36,32>::value, _line<36,33>::value, _line<36,34>::value, _line<36,35>::value, _line<36,36>::value, _line<36,37>::value, _line<36,38>::value, _line<36,39>::value,
    _line<36,40>::value, _line<36,41>::value, _line<36,42>::value, _line<36,43>::value, _line<36,44>::value, _line<36,45>::value, _line<36,46>::value, _line<36,47>::value,
    _line<36,48>::value, _line<36,49>::value, _line<36,50>::value, _line<36,51>::value, _line<36,52>::value, _line<36,53>::value, _line<36,54>::value, _line<36,55>::value,
    _line<36,56>::value, _line<36,57>::value, _line<36,58>::value, _line<36,59>::value, _line<36,60>::value, _line<36,61>::value, _line<36,62>::value, _line<36,63>::value,
    _line<37, 0>::value, _line<37, 1>::value, _line<37, 2>::value, _line<37, 3>::value, _line<37, 4>::value, _line<37, 5>::value, _line<37, 6>::value, _line<37, 7>::value,
    _line<37, 8>::value, _line<37, 9>::value, _line<37,10>::value, _line<37,11>::value, _line<37,12>::value, _line<37,13>::value, _line<37,14>::value, _line<37,15>::value,
    _line<37,16>::value, _line<37,17>::value, _line<37,18>::value, _line<37,19>::value, _line<37,20>::value, _line<37,21>::value, _line<37,22>::value, _line<37,23>::value,
    _line<37,24>::value, _line<37,25>::value, _line<37,26>::value, _line<37,27>::value, _line<37,28>::value, _line<37,29>::value, _line<37,30>::value, _line<37,31>::value,
    _line<37,32>::value, _line<37,33>::value, _line<37,34>::value, _line<37,35>::value, _line<37,36>::value, _line<37,37>::value, _line<37,38>::value, _line<37,39>::value,
    _line<37,40>::value, _line<37,41>::value, _line<37,42>::value, _line<37,43>::value, _line<37,44>::value, _line<37,45>::value, _line<37,46>::value, _line<37,47>::value,
    _line<37,48>::value, _line<37,49>::value, _line<37,50>::value, _line<37,51>::value, _line<37,52>::value, _line<37,53>::value, _line<37,54>::value, _line<37,55>::value,
    _line<37,56>::value, _line<37,57>::value, _line<37,58>::value, _line<37,59>::value, _line<37,60>::value, _line<37,61>::value, _line<37,62>::value, _line<37,63>::value,
    _line<38, 0>::value, _line<38, 1>::value, _line<38, 2>::value, _line<38, 3>::value, _line<38, 4>::value, _line<38, 5>::value, _line<38, 6>::value, _line<38, 7>::value,
    _line<38, 8>::value, _line<38, 9>::value, _line<38,10>::value, _line<38,11>::value, _line<38,12>::value, _line<38,13>::value, _line<38,14>::value, _line<38,15>::value,
    _line<38,16>::value, _line<38,17>::value, _line<38,18>::value, _line<38,19>::value, _line<38,20>::value, _line<38,21>::value, _line<38,22>::value, _line<38,23>::value,
    _line<38,24>::value, _line<38,25>::value, _line<38,26>::value, _line<38,27>::value, _line<38,28>::value, _line<38,29>::value, _line<38,30>::value, _line<38,31>::value,
    _line<38,32>::value, _line<38,33>::value, _line<38,34>::value, _line<38,35>::value, _line<38,36>::value, _line<38,37>::value, _line<38,38>::value, _line<38,39>::value,
    _line<38,40>::value, _line<38,41>::value, _line<38,42>::value, _line<38,43>::value, _line<38,44>::value, _line<38,45>::value, _line<38,46>::value, _line<38,47>::value,
    _line<38,48>::value, _line<38,49>::value, _line<38,50>::value, _line<38,51>::value, _line<38,52>::value, _line<38,53>::value, _line<38,54>::value, _line<38,55>::value,
    _line<38,56>::value, _line<38,57>::value, _line<38,58>::value, _line<38,59>::value, _line<38,60>::value, _line<38,61>::value, _line<38,62>::value, _line<38,63>::value,
    _line<39, 0>::value, _line<39, 1>::value, _line<39, 2>::value, _line<39, 3>::value, _line<39, 4>::value, _line<39, 5>::value, _line<39, 6>::value, _line<39, 7>::value,
    _line<39, 8>::value, _line<39, 9>::value, _line<39,10>::value, _line<39,11>::value, _line<39,12>::value, _line<39,13>::value, _line<39,14>::value, _line<39,15>::value,
    _line<39,16>::value, _line<39,17>::value, _line<39,18>::value, _line<39,19>::value, _line<39,20>::value, _line<39,21>::value, _line<39,22>::value, _line<39,23>::value,
    _line<39,24>::value, _line<39,25>::value, _line<39,26>::value, _line<39,27>::value, _line<39,28>::value, _line<39,29>::value, _line<39,30>::value, _line<39,31>::value,
    _line<39,32>::value, _line<39,33>::value, _line<39,34>::value, _line<39,35>::value, _line<39,36>::value, _line<39,37>::value, _line<39,38>::value, _line<39,39>::value,
    _line<39,40>::value, _line<39,41>::value, _line<39,42>::value, _line<39,43>::value, _line<39,44>::value, _line<39,45>::value, _line<39,46>::value, _line<39,47>::value,
    _line<39,48>::value, _line<39,49>::value, _line<39,50>::value, _line<39,51>::value, _line<39,52>::value, _line<39,53>::value, _line<39,54>::value, _line<39,55>::value,
    _line<39,56>::value, _line<39,57>::value, _line<39,58>::value, _line<39,59>::value, _line<39,60>::value, _line<39,61>::value, _line<39,62>::value, _line<39,63>::value,
    _line<40, 0>::value, _line<40, 1>::value, _line<40, 2>::value, _line<40, 3>::value, _line<40, 4>::value, _line<40, 5>::value, _line<40, 6>::value, _line<40, 7>::value,
    _line<40, 8>::value, _line<40, 9>::value, _line<40,10>::value, _line<40,11>::value, _line<40,12>::value, _line<40,13>::value, _line<40,14>::value, _line<40,15>::value,
    _line<40,16>::value, _line<40,17>::value, _line<40,18>::value, _line<40,19>::value, _line<40,20>::value, _line<40,21>::value, _line<40,22>::value, _line<40,23>::value,
    _line<40,24>::value, _line<40,25>::value, _line<40,26>::value, _line<40,27>::value, _line<40,28>::value, _line<40,29>::value, _line<40,30>::value, _line<40,31>::value,
    _line<40,32>::value, _line<40,33>::value, _line<40,34>::value, _line<40,35>::value, _line<40,36>::value, _line<40,37>::value, _line<40,38>::value, _line<40,39>::value,
    _line<40,40>::value, _line<40,41>::value, _line<40,42>::value, _line<40,43>::value, _line<40,44>::value, _line<40,45>::value, _line<40,46>::value, _line<40,47>::value,
    _line<40,48>::value, _line<40,49>::value, _line<40,50>::value, _line<40,51>::value, _line<40,52>::value, _line<40,53>::value, _line<40,54>::value, _line<40,55>::value,
    _line<40,56>::value, _line<40,57>::value, _line<40,58>::value, _line<40,59>::value, _line<40,60>::value, _line<40,61>::value, _line<40,62>::value, _line<40,63>::value,
    _line<41, 0>::value, _line<41, 1>::value, _line<41, 2>::value, _line<41, 3>::value, _line<41, 4>::value, _line<41, 5>::value, _line<41, 6>::value, _line<41, 7>::value,
    _line<41, 8>::value, _line<41, 9>::value, _line<41,10>::value, _line<41,11>::value, _line<41,12>::value, _line<41,13>::value, _line<41,14>::value, _line<41,15>::value,
    _line<41,16>::value, _line<41,17>::value, _line<41,18>::value, _line<41,19>::value, _line<41,20>::value, _line<41,21>::value, _line<41,22>::value, _line<41,23>::value,
    _line<41,24>::value, _line<41,25>::value, _line<41,26>::value, _line<41,27>::value, _line<41,28>::value, _line<41,29>::value, _line<41,30>::value, _line<41,31>::value,
    _line<41,32>::value, _line<41,33>::value, _line<41,34>::value, _line<41,35>::value, _line<41,36>::value, _line<41,37>::value, _line<41,38>::value, _line<41,39>::value,
    _line<41,40>::value, _line<41,41>::value, _line<41,42>::value, _line<41,43>::value, _line<41,44>::value, _line<41,45>::value, _line<41,46>::value, _line<41,47>::value,
    _line<41,48>::value, _line<41,49>::value, _line<41,50>::value, _line<41,51>::value, _line<41,52>::value, _line<41,53>::value, _line<41,54>::value, _line<41,55>::value,
    _line<41,56>::value, _line<41,57>::value, _line<41,58>::value, _line<41,59>::value, _line<41,60>::value, _line<41,61>::value, _line<41,62>::value, _line<41,63>::value,
    _line<42, 0>::value, _line<42, 1>::value, _line<42, 2>::value, _line<42, 3>::value, _line<42, 4>::value, _line<42, 5>::value, _line<42, 6>::value, _line<42, 7>::value,
    _line<42, 8>::value, _line<42, 9>::value, _line<42,10>::value, _line<42,11>::value, _line<42,12>::value, _line<42,13>::value, _line<42,14>::value, _line<42,15>::value,
    _line<42,16>::value, _line<42,17>::value, _line<42,18>::value, _line<42,19>::value, _line<42,20>::value, _line<42,21>::value, _line<42,22>::value, _line<42,23>::value,
    _line<42,24>::value, _line<42,25>::value, _line<42,26>::value, _line<42,27>::value, _line<42,28>::value, _line<42,29>::value, _line<42,30>::value, _line<42,31>::value,
    _line<42,32>::value, _line<42,33>::value, _line<42,34>::value, _line<42,35>::value, _line<42,36>::value, _line<42,37>::value, _line<42,38>::value, _line<42,39>::value,
    _line<42,40>::value, _line<42,41>::value, _line<42,42>::value, _line<42,43>::value, _line<42,44>::value, _line<42,45>::value, _line<42,46>::value, _line<42,47>::value,
    _line<42,48>::value, _line<42,49>::value, _line<42,50>::value, _line<42,51>::value, _line<42,52>::value, _line<42,53>::value, _line<42,54>::value, _line<42,55>::value,
    _line<42,56>::value, _line<42,57>::value, _line<42,58>::value, _line<42,59>::value, _line<42,60>::value, _line<42,61>::value, _line<42,62>::value, _line<42,63>::value,
    _line<43, 0>::value, _line<43, 1>::value, _line<43, 2>::value, _line<43, 3>::value, _line<43, 4>::value, _line<43, 5>::value, _line<43, 6>::value, _line<43, 7>::value,
    _line<43, 8>::value, _line<43, 9>::value, _line<43,10>::value, _line<43,11>::value, _line<43,12>::value, _line<43,13>::value, _line<43,14>::value, _line<43,15>::value,
    _line<43,16>::value, _line<43,17>::value, _line<43,18>::value, _line<43,19>::value, _line<43,20>::value, _line<43,21>::value, _line<43,22>::value, _line<43,23>::value,
    _line<43,24>::value, _line<43,25>::value, _line<43,26>::value, _line<43,27>::value, _line<43,28>::value, _line<43,29>::value, _line<43,30>::value, _line<43,31>::value,
    _line<43,32>::value, _line<43,33>::value, _line<43,34>::value, _line<43,35>::value, _line<43,36>::value, _line<43,37>::value, _line<43,38>::value, _line<43,39>::value,
    _line<43,40>::value, _line<43,41>::value, _line<43,42>::value, _line<43,43>::value, _line<43,44>::value, _line<43,45>::value, _line<43,46>::value, _line<43,47>::value,
    _line<43,48>::value, _line<43,49>::value, _line<43,50>::value, _line<43,51>::value, _line<43,52>::value, _line<43,53>::value, _line<43,54>::value, _line<43,55>::value,
    _line<43,56>::value, _line<43,57>::value, _line<43,58>::value, _line<43,59>::value, _line<43,60>::value, _line<43,61>::value, _line<43,62>::value, _line<43,63>::value,
    _line<44, 0>::value, _line<44, 1>::value, _line<44, 2>::value, _line<44, 3>::value, _line<44, 4>::value, _line<44, 5>::value, _line<44, 6>::value, _line<44, 7>::value,
    _line<44, 8>::value, _line<44, 9>::value, _line<44,10>::value, _line<44,11>::value, _line<44,12>::value, _line<44,13>::value, _line<44,14>::value, _line<44,15>::value,
    _line<44,16>::value, _line<44,17>::value, _line<44,18>::value, _line<44,19>::value, _line<44,20>::value, _line<44,21>::value, _line<44,22>::value, _line<44,23>::value,
    _line<44,24>::value, _line<44,25>::value, _line<44,26>::value, _line<44,27>::value, _line<44,28>::value, _line<44,29>::value, _line<44,30>::value, _line<44,31>::value,
    _line<44,32>::value, _line<44,33>::value, _line<44,34>::value, _line<44,35>::value, _line<44,36>::value, _line<44,37>::value, _line<44,38>::value, _line<44,39>::value,
    _line<44,40>::value, _line<44,41>::value, _line<44,42>::value, _line<44,43>::value, _line<44,44>::value, _line<44,45>::value, _line<44,46>::value, _line<44,47>::value,
    _line<44,48>::value, _line<44,49>::value, _line<44,50>::value, _line<44,51>::value, _line<44,52>::value, _line<44,53>::value, _line<44,54>::value, _line<44,55>::value,
    _line<44,56>::value, _line<44,57>::value, _line<44,58>::value, _line<44,59>::value, _line<44,60>::value, _line<44,61>::value, _line<44,62>::value, _line<44,63>::value,
    _line<45, 0>::value, _line<45, 1>::value, _line<45, 2>::value, _line<45, 3>::value, _line<45, 4>::value, _line<45, 5>::value, _line<45, 6>::value, _line<45, 7>::value,
    _line<45, 8>::value, _line<45, 9>::value, _line<45,10>::value, _line<45,11>::value, _line<45,12>::value, _line<45,13>::value, _line<45,14>::value, _line<45,15>::value,
    _line<45,16>::value, _line<45,17>::value, _line<45,18>::value, _line<45,19>::value, _line<45,20>::value, _line<45,21>::value, _line<45,22>::value, _line<45,23>::value,
    _line<45,24>::value, _line<45,25>::value, _line<45,26>::value, _line<45,27>::value, _line<45,28>::value, _line<45,29>::value, _line<45,30>::value, _line<45,31>::value,
    _line<45,32>::value, _line<45,33>::value, _line<45,34>::value, _line<45,35>::value, _line<45,36>::value, _line<45,37>::value, _line<45,38>::value, _line<45,39>::value,
    _line<45,40>::value, _line<45,41>::value, _line<45,42>::value, _line<45,43>::value, _line<45,44>::value, _line<45,45>::value, _line<45,46>::value, _line<45,47>::value,
    _line<45,48>::value, _line<45,49>::value, _line<45,50>::value, _line<45,51>::value, _line<45,52>::value, _line<45,53>::value, _line<45,54>::value, _line<45,55>::value,
    _line<45,56>::value, _line<45,57>::value, _line<45,58>::value, _line<45,59>::value, _line<45,60>::value, _line<45,61>::value, _line<45,62>::value, _line<45,63>::value,
    _line<46, 0>::value, _line<46, 1>::value, _line<46, 2>::value, _line<46, 3>::value, _line<46, 4>::value, _line<46, 5>::value, _line<46, 6>::value, _line<46, 7>::value,
    _line<46, 8>::value, _line<46, 9>::value, _line<46,10>::value, _line<46,11>::value, _line<46,12>::value, _line<46,13>::value, _line<46,14>::value, _line<46,15>::value,
    _line<46,16>::value, _line<46,17>::value, _line<46,18>::value, _line<46,19>::value, _line<46,20>::value, _line<46,21>::value, _line<46,22>::value, _line<46,23>::value,
    _line<46,24>::value, _line<46,25>::value, _line<46,26>::value, _line<46,27>::value, _line<46,28>::value, _line<46,29>::value, _line<46,30>::value, _line<46,31>::value,
    _line<46,32>::value, _line<46,33>::value, _line<46,34>::value, _line<46,35>::value, _line<46,36>::value, _line<46,37>::value, _line<46,38>::value, _line<46,39>::value,
    _line<46,40>::value, _line<46,41>::value, _line<46,42>::value, _line<46,43>::value, _line<46,44>::value, _line<46,45>::value, _line<46,46>::value, _line<46,47>::value,
    _line<46,48>::value, _line<46,49>::value, _line<46,50>::value, _line<46,51>::value, _line<46,52>::value, _line<46,53>::value, _line<46,54>::value, _line<46,55>::value,
    _line<46,56>::value, _line<46,57>::value, _line<46,58>::value, _line<46,59>::value, _line<46,60>::value, _line<46,61>::value, _line<46,62>::value, _line<46,63>::value,
    _line<47, 0>::value, _line<47, 1>::value, _line<47, 2>::value, _line<47, 3>::value, _line<47, 4>::value, _line<47, 5>::value, _line<47, 6>::value, _line<47, 7>::value,
    _line<47, 8>::value, _line<47, 9>::value, _line<47,10>::value, _line<47,11>::value, _line<47,12>::value, _line<47,13>::value, _line<47,14>::value, _line<47,15>::value,
    _line<47,16>::value, _line<47,17>::value, _line<47,18>::value, _line<47,19>::value, _line<47,20>::value, _line<47,21>::value, _line<47,22>::value, _line<47,23>::value,
    _line<47,24>::value, _line<47,25>::value, _line<47,26>::value, _line<47,27>::value, _line<47,28>::value, _line<47,29>::value, _line<47,30>::value, _line<47,31>::value,
    _line<47,32>::value, _line<47,33>::value, _line<47,34>::value, _line<47,35>::value, _line<47,36>::value, _line<47,37>::value, _line<47,38>::value, _line<47,39>::value,
    _line<47,40>::value, _line<47,41>::value, _line<47,42>::value, _line<47,43>::value, _line<47,44>::value, _line<47,45>::value, _line<47,46>::value, _line<47,47>::value,
    _line<47,48>::value, _line<47,49>::value, _line<47,50>::value, _line<47,51>::value, _line<47,52>::value, _line<47,53>::value, _line<47,54>::value, _line<47,55>::value,
    _line<47,56>::value, _line<47,57>::value, _line<47,58>::value, _line<47,59>::value, _line<47,60>::value, _line<47,61>::value, _line<47,62>::value, _line<47,63>::value,
    _line<48, 0>::value, _line<48, 1>::value, _line<48, 2>::value, _line<48, 3>::value, _line<48, 4>::value, _line<48, 5>::value, _line<48, 6>::value, _line<48, 7>::value,
    _line<48, 8>::value, _line<48, 9>::value, _line<48,10>::value, _line<48,11>::value, _line<48,12>::value, _line<48,13>::value, _line<48,14>::value, _line<48,15>::value,
    _line<48,16>::value, _line<48,17>::value, _line<48,18>::value, _line<48,19>::value, _line<48,20>::value, _line<48,21>::value, _line<48,22>::value, _line<48,23>::value,
    _line<48,24>::value, _line<48,25>::value, _line<48,26>::value, _line<48,27>::value, _line<48,28>::value, _line<48,29>::value, _line<48,30>::value, _line<48,31>::value,
    _line<48,32>::value, _line<48,33>::value, _line<48,34>::value, _line<48,35>::value, _line<48,36>::value, _line<48,37>::value, _line<48,38>::value, _line<48,39>::value,
    _line<48,40>::value, _line<48,41>::value, _line<48,42>::value, _line<48,43>::value, _line<48,44>::value, _line<48,45>::value, _line<48,46>::value, _line<48,47>::value,
    _line<48,48>::value, _line<48,49>::value, _line<48,50>::value, _line<48,51>::value, _line<48,52>::value, _line<48,53>::value, _line<48,54>::value, _line<48,55>::value,
    _line<48,56>::value, _line<48,57>::value, _line<48,58>::value, _line<48,59>::value, _line<48,60>::value, _line<48,61>::value, _line<48,62>::value, _line<48,63>::value,
    _line<49, 0>::value, _line<49, 1>::value, _line<49, 2>::value, _line<49, 3>::value, _line<49, 4>::value, _line<49, 5>::value, _line<49, 6>::value, _line<49, 7>::value,
    _line<49, 8>::value, _line<49, 9>::value, _line<49,10>::value, _line<49,11>::value, _line<49,12>::value, _line<49,13>::value, _line<49,14>::value, _line<49,15>::value,
    _line<49,16>::value, _line<49,17>::value, _line<49,18>::value, _line<49,19>::value, _line<49,20>::value, _line<49,21>::value, _line<49,22>::value, _line<49,23>::value,
    _line<49,24>::value, _line<49,25>::value, _line<49,26>::value, _line<49,27>::value, _line<49,28>::value, _line<49,29>::value, _line<49,30>::value, _line<49,31>::value,
    _line<49,32>::value, _line<49,33>::value, _line<49,34>::value, _line<49,35>::value, _line<49,36>::value, _line<49,37>::value, _line<49,38>::value, _line<49,39>::value,
    _line<49,40>::value, _line<49,41>::value, _line<49,42>::value, _line<49,43>::value, _line<49,44>::value, _line<49,45>::value, _line<49,46>::value, _line<49,47>::value,
    _line<49,48>::value, _line<49,49>::value, _line<49,50>::value, _line<49,51>::value, _line<49,52>::value, _line<49,53>::value, _line<49,54>::value, _line<49,55>::value,
    _line<49,56>::value, _line<49,57>::value, _line<49,58>::value, _line<49,59>::value, _line<49,60>::value, _line<49,61>::value, _line<49,62>::value, _line<49,63>::value,
    _line<50, 0>::value, _line<50, 1>::value, _line<50, 2>::value, _line<50, 3>::value, _line<50, 4>::value, _line<50, 5>::value, _line<50, 6>::value, _line<50, 7>::value,
    _line<50, 8>::value, _line<50, 9>::value, _line<50,10>::value, _line<50,11>::value, _line<50,12>::value, _line<50,13>::value, _line<50,14>::value, _line<50,15>::value,
    _line<50,16>::value, _line<50,17>::value, _line<50,18>::value, _line<50,19>::value, _line<50,20>::value, _line<50,21>::value, _line<50,22>::value, _line<50,23>::value,
    _line<50,24>::value, _line<50,25>::value, _line<50,26>::value, _line<50,27>::value, _line<50,28>::value, _line<50,29>::value, _line<50,30>::value, _line<50,31>::value,
    _line<50,32>::value, _line<50,33>::value, _line<50,34>::value, _line<50,35>::value, _line<50,36>::value, _line<50,37>::value, _line<50,38>::value, _line<50,39>::value,
    _line<50,40>::value, _line<50,41>::value, _line<50,42>::value, _line<50,43>::value, _line<50,44>::value, _line<50,45>::value, _line<50,46>::value, _line<50,47>::value,
    _line<50,48>::value, _line<50,49>::value, _line<50,50>::value, _line<50,51>::value, _line<50,52>::value, _line<50,53>::value, _line<50,54>::value, _line<50,55>::value,
    _line<50,56>::value, _line<50,57>::value, _line<50,58>::value, _line<50,59>::value, _line<50,60>::value, _line<50,61>::value, _line<50,62>::value, _line<50,63>::value,
    _line<51, 0>::value, _line<51, 1>::value, _line<51, 2>::value, _line<51, 3>::value, _line<51, 4>::value, _line<51, 5>::value, _line<51, 6>::value, _line<51, 7>::value,
    _line<51, 8>::value, _line<51, 9>::value, _line<51,10>::value, _line<51,11>::value, _line<51,12>::value, _line<51,13>::value, _line<51,14>::value, _line<51,15>::value,
    _line<51,16>::value, _line<51,17>::value, _line<51,18>::value, _line<51,19>::value, _line<51,20>::value, _line<51,21>::value, _line<51,22>::value, _line<51,23>::value,
    _line<51,24>::value, _line<51,25>::value, _line<51,26>::value, _line<51,27>::value, _line<51,28>::value, _line<51,29>::value, _line<51,30>::value, _line<51,31>::value,
    _line<51,32>::value, _line<51,33>::value, _line<51,34>::value, _line<51,35>::value, _line<51,36>::value, _line<51,37>::value, _line<51,38>::value, _line<51,39>::value,
    _line<51,40>::value, _line<51,41>::value, _line<51,42>::value, _line<51,43>::value, _line<51,44>::value, _line<51,45>::value, _line<51,46>::value, _line<51,47>::value,
    _line<51,48>::value, _line<51,49>::value, _line<51,50>::value, _line<51,51>::value, _line<51,52>::value, _line<51,53>::value, _line<51,54>::value, _line<51,55>::value,
    _line<51,56>::value, _line<51,57>::value, _line<51,58>::value, _line<51,59>::value, _line<51,60>::value, _line<51,61>::value, _line<51,62>::value, _line<51,63>::value,
    _line<52, 0>::value, _line<52, 1>::value, _line<52, 2>::value, _line<52, 3>::value, _line<52, 4>::value, _line<52, 5>::value, _line<52, 6>::value, _line<52, 7>::value,
    _line<52, 8>::value, _line<52, 9>::value, _line<52,10>::value, _line<52,11>::value, _line<52,12>::value, _line<52,13>::value, _line<52,14>::value, _line<52,15>::value,
    _line<52,16>::value, _line<52,17>::value, _line<52,18>::value, _line<52,19>::value, _line<52,20>::value, _line<52,21>::value, _line<52,22>::value, _line<52,23>::value,
    _line<52,24>::value, _line<52,25>::value, _line<52,26>::value, _line<52,27>::value, _line<52,28>::value, _line<52,29>::value, _line<52,30>::value, _line<52,31>::value,
    _line<52,32>::value, _line<52,33>::value, _line<52,34>::value, _line<52,35>::value, _line<52,36>::value, _line<52,37>::value, _line<52,38>::value, _line<52,39>::value,
    _line<52,40>::value, _line<52,41>::value, _line<52,42>::value, _line<52,43>::value, _line<52,44>::value, _line<52,45>::value, _line<52,46>::value, _line<52,47>::value,
    _line<52,48>::value, _line<52,49>::value, _line<52,50>::value, _line<52,51>::value, _line<52,52>::value, _line<52,53>::value, _line<52,54>::value, _line<52,55>::value,
    _line<52,56>::value, _line<52,57>::value, _line<52,58>::value, _line<52,59>::value, _line<52,60>::value, _line<52,61>::value, _line<52,62>::value, _line<52,63>::value,
    _line<53, 0>::value, _line<53, 1>::value, _line<53, 2>::value, _line<53, 3>::value, _line<53, 4>::value, _line<53, 5>::value, _line<53, 6>::value, _line<53, 7>::value,
    _line<53, 8>::value, _line<53, 9>::value, _line<53,10>::value, _line<53,11>::value, _line<53,12>::value, _line<53,13>::value, _line<53,14>::value, _line<53,15>::value,
    _line<53,16>::value, _line<53,17>::value, _line<53,18>::value, _line<53,19>::value, _line<53,20>::value, _line<53,21>::value, _line<53,22>::value, _line<53,23>::value,
    _line<53,24>::value, _line<53,25>::value, _line<53,26>::value, _line<53,27>::value, _line<53,28>::value, _line<53,29>::value, _line<53,30>::value, _line<53,31>::value,
    _line<53,32>::value, _line<53,33>::value, _line<53,34>::value, _line<53,35>::value, _line<53,36>::value, _line<53,37>::value, _line<53,38>::value, _line<53,39>::value,
    _line<53,40>::value, _line<53,41>::value, _line<53,42>::value, _line<53,43>::value, _line<53,44>::value, _line<53,45>::value, _line<53,46>::value, _line<53,47>::value,
    _line<53,48>::value, _line<53,49>::value, _line<53,50>::value, _line<53,51>::value, _line<53,52>::value, _line<53,53>::value, _line<53,54>::value, _line<53,55>::value,
    _line<53,56>::value, _line<53,57>::value, _line<53,58>::value, _line<53,59>::value, _line<53,60>::value, _line<53,61>::value, _line<53,62>::value, _line<53,63>::value,
    _line<54, 0>::value, _line<54, 1>::value, _line<54, 2>::value, _line<54, 3>::value, _line<54, 4>::value, _line<54, 5>::value, _line<54, 6>::value, _line<54, 7>::value,
    _line<54, 8>::value, _line<54, 9>::value, _line<54,10>::value, _line<54,11>::value, _line<54,12>::value, _line<54,13>::value, _line<54,14>::value, _line<54,15>::value,
    _line<54,16>::value, _line<54,17>::value, _line<54,18>::value, _line<54,19>::value, _line<54,20>::value, _line<54,21>::value, _line<54,22>::value, _line<54,23>::value,
    _line<54,24>::value, _line<54,25>::value, _line<54,26>::value, _line<54,27>::value, _line<54,28>::value, _line<54,29>::value, _line<54,30>::value, _line<54,31>::value,
    _line<54,32>::value, _line<54,33>::value, _line<54,34>::value, _line<54,35>::value, _line<54,36>::value, _line<54,37>::value, _line<54,38>::value, _line<54,39>::value,
    _line<54,40>::value, _line<54,41>::value, _line<54,42>::value, _line<54,43>::value, _line<54,44>::value, _line<54,45>::value, _line<54,46>::value, _line<54,47>::value,
    _line<54,48>::value, _line<54,49>::value, _line<54,50>::value, _line<54,51>::value, _line<54,52>::value, _line<54,53>::value, _line<54,54>::value, _line<54,55>::value,
    _line<54,56>::value, _line<54,57>::value, _line<54,58>::value, _line<54,59>::value, _line<54,60>::value, _line<54,61>::value, _line<54,62>::value, _line<54,63>::value,
    _line<55, 0>::value, _line<55, 1>::value, _line<55, 2>::value, _line<55, 3>::value, _line<55, 4>::value, _line<55, 5>::value, _line<55, 6>::value, _line<55, 7>::value,
    _line<55, 8>::value, _line<55, 9>::value, _line<55,10>::value, _line<55,11>::value, _line<55,12>::value, _line<55,13>::value, _line<55,14>::value, _line<55,15>::value,
    _line<55,16>::value, _line<55,17>::value, _line<55,18>::value, _line<55,19>::value, _line<55,20>::value, _line<55,21>::value, _line<55,22>::value, _line<55,23>::value,
    _line<55,24>::value, _line<55,25>::value, _line<55,26>::value, _line<55,27>::value, _line<55,28>::value, _line<55,29>::value, _line<55,30>::value, _line<55,31>::value,
    _line<55,32>::value, _line<55,33>::value, _line<55,34>::value, _line<55,35>::value, _line<55,36>::value, _line<55,37>::value, _line<55,38>::value, _line<55,39>::value,
    _line<55,40>::value, _line<55,41>::value, _line<55,42>::value, _line<55,43>::value, _line<55,44>::value, _line<55,45>::value, _line<55,46>::value, _line<55,47>::value,
    _line<55,48>::value, _line<55,49>::value, _line<55,50>::value, _line<55,51>::value, _line<55,52>::value, _line<55,53>::value, _line<55,54>::value, _line<55,55>::value,
    _line<55,56>::value, _line<55,57>::value, _line<55,58>::value, _line<55,59>::value, _line<55,60>::value, _line<55,61>::value, _line<55,62>::value, _line<55,63>::value,
    _line<56, 0>::value, _line<56, 1>::value, _line<56, 2>::value, _line<56, 3>::value, _line<56, 4>::value, _line<56, 5>::value, _line<56, 6>::value, _line<56, 7>::value,
    _line<56, 8>::value, _line<56, 9>::value, _line<56,10>::value, _line<56,11>::value, _line<56,12>::value, _line<56,13>::value, _line<56,14>::value, _line<56,15>::value,
    _line<56,16>::value, _line<56,17>::value, _line<56,18>::value, _line<56,19>::value, _line<56,20>::value, _line<56,21>::value, _line<56,22>::value, _line<56,23>::value,
    _line<56,24>::value, _line<56,25>::value, _line<56,26>::value, _line<56,27>::value, _line<56,28>::value, _line<56,29>::value, _line<56,30>::value, _line<56,31>::value,
    _line<56,32>::value, _line<56,33>::value, _line<56,34>::value, _line<56,35>::value, _line<56,36>::value, _line<56,37>::value, _line<56,38>::value, _line<56,39>::value,
    _line<56,40>::value, _line<56,41>::value, _line<56,42>::value, _line<56,43>::value, _line<56,44>::value, _line<56,45>::value, _line<56,46>::value, _line<56,47>::value,
    _line<56,48>::value, _line<56,49>::value, _line<56,50>::value, _line<56,51>::value, _line<56,52>::value, _line<56,53>::value, _line<56,54>::value, _line<56,55>::value,
    _line<56,56>::value, _line<56,57>::value, _line<56,58>::value, _line<56,59>::value, _line<56,60>::value, _line<56,61>::value, _line<56,62>::value, _line<56,63>::value,
    _line<57, 0>::value, _line<57, 1>::value, _line<57, 2>::value, _line<57, 3>::value, _line<57, 4>::value, _line<57, 5>::value, _line<57, 6>::value, _line<57, 7>::value,
    _line<57, 8>::value, _line<57, 9>::value, _line<57,10>::value, _line<57,11>::value, _line<57,12>::value, _line<57,13>::value, _line<57,14>::value, _line<57,15>::value,
    _line<57,16>::value, _line<57,17>::value, _line<57,18>::value, _line<57,19>::value, _line<57,20>::value, _line<57,21>::value, _line<57,22>::value, _line<57,23>::value,
    _line<57,24>::value, _line<57,25>::value, _line<57,26>::value, _line<57,27>::value, _line<57,28>::value, _line<57,29>::value, _line<57,30>::value, _line<57,31>::value,
    _line<57,32>::value, _line<57,33>::value, _line<57,34>::value, _line<57,35>::value, _line<57,36>::value, _line<57,37>::value, _line<57,38>::value, _line<57,39>::value,
    _line<57,40>::value, _line<57,41>::value, _line<57,42>::value, _line<57,43>::value, _line<57,44>::value, _line<57,45>::value, _line<57,46>::value, _line<57,47>::value,
    _line<57,48>::value, _line<57,49>::value, _line<57,50>::value, _line<57,51>::value, _line<57,52>::value, _line<57,53>::value, _line<57,54>::value, _line<57,55>::value,
    _line<57,56>::value, _line<57,57>::value, _line<57,58>::value, _line<57,59>::value, _line<57,60>::value, _line<57,61>::value, _line<57,62>::value, _line<57,63>::value,
    _line<58, 0>::value, _line<58, 1>::value, _line<58, 2>::value, _line<58, 3>::value, _line<58, 4>::value, _line<58, 5>::value, _line<58, 6>::value, _line<58, 7>::value,
    _line<58, 8>::value, _line<58, 9>::value, _line<58,10>::value, _line<58,11>::value, _line<58,12>::value, _line<58,13>::value, _line<58,14>::value, _line<58,15>::value,
    _line<58,16>::value, _line<58,17>::value, _line<58,18>::value, _line<58,19>::value, _line<58,20>::value, _line<58,21>::value, _line<58,22>::value, _line<58,23>::value,
    _line<58,24>::value, _line<58,25>::value, _line<58,26>::value, _line<58,27>::value, _line<58,28>::value, _line<58,29>::value, _line<58,30>::value, _line<58,31>::value,
    _line<58,32>::value, _line<58,33>::value, _line<58,34>::value, _line<58,35>::value, _line<58,36>::value, _line<58,37>::value, _line<58,38>::value, _line<58,39>::value,
    _line<58,40>::value, _line<58,41>::value, _line<58,42>::value, _line<58,43>::value, _line<58,44>::value, _line<58,45>::value, _line<58,46>::value, _line<58,47>::value,
    _line<58,48>::value, _line<58,49>::value, _line<58,50>::value, _line<58,51>::value, _line<58,52>::value, _line<58,53>::value, _line<58,54>::value, _line<58,55>::value,
    _line<58,56>::value, _line<58,57>::value, _line<58,58>::value, _line<58,59>::value, _line<58,60>::value, _line<58,61>::value, _line<58,62>::value, _line<58,63>::value,
    _line<59, 0>::value, _line<59, 1>::value, _line<59, 2>::value, _line<59, 3>::value, _line<59, 4>::value, _line<59, 5>::value, _line<59, 6>::value, _line<59, 7>::value,
    _line<59, 8>::value, _line<59, 9>::value, _line<59,10>::value, _line<59,11>::value, _line<59,12>::value, _line<59,13>::value, _line<59,14>::value, _line<59,15>::value,
    _line<59,16>::value, _line<59,17>::value, _line<59,18>::value, _line<59,19>::value, _line<59,20>::value, _line<59,21>::value, _line<59,22>::value, _line<59,23>::value,
    _line<59,24>::value, _line<59,25>::value, _line<59,26>::value, _line<59,27>::value, _line<59,28>::value, _line<59,29>::value, _line<59,30>::value, _line<59,31>::value,
    _line<59,32>::value, _line<59,33>::value, _line<59,34>::value, _line<59,35>::value, _line<59,36>::value, _line<59,37>::value, _line<59,38>::value, _line<59,39>::value,
    _line<59,40>::value, _line<59,41>::value, _line<59,42>::value, _line<59,43>::value, _line<59,44>::value, _line<59,45>::value, _line<59,46>::value, _line<59,47>::value,
    _line<59,48>::value, _line<59,49>::value, _line<59,50>::value, _line<59,51>::value, _line<59,52>::value, _line<59,53>::value, _line<59,54>::value, _line<59,55>::value,
    _line<59,56>::value, _line<59,57>::value, _line<59,58>::value, _line<59,59>::value, _line<59,60>::value, _line<59,61>::value, _line<59,62>::value, _line<59,63>::value,
    _line<60, 0>::value, _line<60, 1>::value, _line<60, 2>::value, _line<60, 3>::value, _line<60, 4>::value, _line<60, 5>::value, _line<60, 6>::value, _line<60, 7>::value,
    _line<60, 8>::value, _line<60, 9>::value, _line<60,10>::value, _line<60,11>::value, _line<60,12>::value, _line<60,13>::value, _line<60,14>::value, _line<60,15>::value,
    _line<60,16>::value, _line<60,17>::value, _line<60,18>::value, _line<60,19>::value, _line<60,20>::value, _line<60,21>::value, _line<60,22>::value, _line<60,23>::value,
    _line<60,24>::value, _line<60,25>::value, _line<60,26>::value, _line<60,27>::value, _line<60,28>::value, _line<60,29>::value, _line<60,30>::value, _line<60,31>::value,
    _line<60,32>::value, _line<60,33>::value, _line<60,34>::value, _line<60,35>::value, _line<60,36>::value, _line<60,37>::value, _line<60,38>::value, _line<60,39>::value,
    _line<60,40>::value, _line<60,41>::value, _line<60,42>::value, _line<60,43>::value, _line<60,44>::value, _line<60,45>::value, _line<60,46>::value, _line<60,47>::value,
    _line<60,48>::value, _line<60,49>::value, _line<60,50>::value, _line<60,51>::value, _line<60,52>::value, _line<60,53>::value, _line<60,54>::value, _line<60,55>::value,
    _line<60,56>::value, _line<60,57>::value, _line<60,58>::value, _line<60,59>::value, _line<60,60>::value, _line<60,61>::value, _line<60,62>::value, _line<60,63>::value,
    _line<61, 0>::value, _line<61, 1>::value, _line<61, 2>::value, _line<61, 3>::value, _line<61, 4>::value, _line<61, 5>::value, _line<61, 6>::value, _line<61, 7>::value,
    _line<61, 8>::value, _line<61, 9>::value, _line<61,10>::value, _line<61,11>::value, _line<61,12>::value, _line<61,13>::value, _line<61,14>::value, _line<61,15>::value,
    _line<61,16>::value, _line<61,17>::value, _line<61,18>::value, _line<61,19>::value, _line<61,20>::value, _line<61,21>::value, _line<61,22>::value, _line<61,23>::value,
    _line<61,24>::value, _line<61,25>::value, _line<61,26>::value, _line<61,27>::value, _line<61,28>::value, _line<61,29>::value, _line<61,30>::value, _line<61,31>::value,
    _line<61,32>::value, _line<61,33>::value, _line<61,34>::value, _line<61,35>::value, _line<61,36>::value, _line<61,37>::value, _line<61,38>::value, _line<61,39>::value,
    _line<61,40>::value, _line<61,41>::value, _line<61,42>::value, _line<61,43>::value, _line<61,44>::value, _line<61,45>::value, _line<61,46>::value, _line<61,47>::value,
    _line<61,48>::value, _line<61,49>::value, _line<61,50>::value, _line<61,51>::value, _line<61,52>::value, _line<61,53>::value, _line<61,54>::value, _line<61,55>::value,
    _line<61,56>::value, _line<61,57>::value, _line<61,58>::value, _line<61,59>::value, _line<61,60>::value, _line<61,61>::value, _line<61,62>::value, _line<61,63>::value,
    _line<62, 0>::value, _line<62, 1>::value, _line<62, 2>::value, _line<62, 3>::value, _line<62, 4>::value, _line<62, 5>::value, _line<62, 6>::value, _line<62, 7>::value,
    _line<62, 8>::value, _line<62, 9>::value, _line<62,10>::value, _line<62,11>::value, _line<62,12>::value, _line<62,13>::value, _line<62,14>::value, _line<62,15>::value,
    _line<62,16>::value, _line<62,17>::value, _line<62,18>::value, _line<62,19>::value, _line<62,20>::value, _line<62,21>::value, _line<62,22>::value, _line<62,23>::value,
    _line<62,24>::value, _line<62,25>::value, _line<62,26>::value, _line<62,27>::value, _line<62,28>::value, _line<62,29>::value, _line<62,30>::value, _line<62,31>::value,
    _line<62,32>::value, _line<62,33>::value, _line<62,34>::value, _line<62,35>::value, _line<62,36>::value, _line<62,37>::value, _line<62,38>::value, _line<62,39>::value,
    _line<62,40>::value, _line<62,41>::value, _line<62,42>::value, _line<62,43>::value, _line<62,44>::value, _line<62,45>::value, _line<62,46>::value, _line<62,47>::value,
    _line<62,48>::value, _line<62,49>::value, _line<62,50>::value, _line<62,51>::value, _line<62,52>::value, _line<62,53>::value, _line<62,54>::value, _line<62,55>::value,
    _line<62,56>::value, _line<62,57>::value, _line<62,58>::value, _line<62,59>::value, _line<62,60>::value, _line<62,61>::value, _line<62,62>::value, _line<62,63>::value,
    _line<63, 0>::value, _line<63, 1>::value, _line<63, 2>::value, _line<63, 3>::value, _line<63, 4>::value, _line<63, 5>::value, _line<63, 6>::value, _line<63, 7>::value,
    _line<63, 8>::value, _line<63, 9>::value, _line<63,10>::value, _line<63,11>::value, _line<63,12>::value, _line<63,13>::value, _line<63,14>::value, _line<63,15>::value,
    _line<63,16>::value, _line<63,17>::value, _line<63,18>::value, _line<63,19>::value, _line<63,20>::value, _line<63,21>::value, _line<63,22>::value, _line<63,23>::value,
    _line<63,24>::value, _line<63,25>::value, _line<63,26>::value, _line<63,27>::value, _line<63,28>::value, _line<63,29>::value, _line<63,30>::value, _line<63,31>::value,
    _line<63,32>::value, _line<63,33>::value, _line<63,34>::value, _line<63,35>::value, _line<63,36>::value, _line<63,37>::value, _line<63,38>::value, _line<63,39>::value,
    _line<63,40>::value, _line<63,41>::value, _line<63,42>::value, _line<63,43>::value, _line<63,44>::value, _line<63,45>::value, _line<63,46>::value, _line<63,47>::value,
    _line<63,48>::value, _line<63,49>::value, _line<63,50>::value, _line<63,51>::value, _line<63,52>::value, _line<63,53>::value, _line<63,54>::value, _line<63,55>::value,
    _line<63,56>::value, _line<63,57>::value, _line<63,58>::value, _line<63,59>::value, _line<63,60>::value, _line<63,61>::value, _line<63,62>::value, _line<63,63>::value
};

const std::array<std::array<std::array<U64, 8>, 64>, 3> rays = {
    _ray<bishop, 0, 0>::value, _ray<bishop, 0,1>::value, _ray<bishop, 0, 2>::value, _ray<bishop, 0,3>::value, _ray<bishop, 0,4>::value, _ray<bishop, 0,5>::value, _ray<bishop, 0, 6>::value, _ray<bishop, 0,7>::value,
    _ray<bishop, 1, 0>::value, _ray<bishop, 1,1>::value, _ray<bishop, 1, 2>::value, _ray<bishop, 1,3>::value, _ray<bishop, 1,4>::value, _ray<bishop, 1,5>::value, _ray<bishop, 1, 6>::value, _ray<bishop, 1,7>::value,
    _ray<bishop, 2, 0>::value, _ray<bishop, 2,1>::value, _ray<bishop, 2, 2>::value, _ray<bishop, 2,3>::value, _ray<bishop, 2,4>::value, _ray<bishop, 2,5>::value, _ray<bishop, 2, 6>::value, _ray<bishop, 2,7>::value,
    _ray<bishop, 3, 0>::value, _ray<bishop, 3,1>::value, _ray<bishop, 3, 2>::value, _ray<bishop, 3,3>::value, _ray<bishop, 3,4>::value, _ray<bishop, 3,5>::value, _ray<bishop, 3, 6>::value, _ray<bishop, 3,7>::value,
    _ray<bishop, 4, 0>::value, _ray<bishop, 4,1>::value, _ray<bishop, 4, 2>::value, _ray<bishop, 4,3>::value, _ray<bishop, 4,4>::value, _ray<bishop, 4,5>::value, _ray<bishop, 4, 6>::value, _ray<bishop, 4,7>::value,
    _ray<bishop, 5, 0>::value, _ray<bishop, 5,1>::value, _ray<bishop, 5, 2>::value, _ray<bishop, 5,3>::value, _ray<bishop, 5,4>::value, _ray<bishop, 5,5>::value, _ray<bishop, 5, 6>::value, _ray<bishop, 5,7>::value,
    _ray<bishop, 6, 0>::value, _ray<bishop, 6,1>::value, _ray<bishop, 6, 2>::value, _ray<bishop, 6,3>::value, _ray<bishop, 6,4>::value, _ray<bishop, 6,5>::value, _ray<bishop, 6, 6>::value, _ray<bishop, 6,7>::value,
    _ray<bishop, 7, 0>::value, _ray<bishop, 7,1>::value, _ray<bishop, 7, 2>::value, _ray<bishop, 7,3>::value, _ray<bishop, 7,4>::value, _ray<bishop, 7,5>::value, _ray<bishop, 7, 6>::value, _ray<bishop, 7,7>::value,
    _ray<bishop, 8, 0>::value, _ray<bishop, 8,1>::value, _ray<bishop, 8, 2>::value, _ray<bishop, 8,3>::value, _ray<bishop, 8,4>::value, _ray<bishop, 8,5>::value, _ray<bishop, 8, 6>::value, _ray<bishop, 8,7>::value,
    _ray<bishop, 9, 0>::value, _ray<bishop, 9,1>::value, _ray<bishop, 9, 2>::value, _ray<bishop, 9,3>::value, _ray<bishop, 9,4>::value, _ray<bishop, 9,5>::value, _ray<bishop, 9, 6>::value, _ray<bishop, 9,7>::value,
    _ray<bishop,10, 0>::value, _ray<bishop,10,1>::value, _ray<bishop,10, 2>::value, _ray<bishop,10,3>::value, _ray<bishop,10,4>::value, _ray<bishop,10,5>::value, _ray<bishop,10, 6>::value, _ray<bishop,10,7>::value,
    _ray<bishop,11, 0>::value, _ray<bishop,11,1>::value, _ray<bishop,11, 2>::value, _ray<bishop,11,3>::value, _ray<bishop,11,4>::value, _ray<bishop,11,5>::value, _ray<bishop,11, 6>::value, _ray<bishop,11,7>::value,
    _ray<bishop,12, 0>::value, _ray<bishop,12,1>::value, _ray<bishop,12, 2>::value, _ray<bishop,12,3>::value, _ray<bishop,12,4>::value, _ray<bishop,12,5>::value, _ray<bishop,12, 6>::value, _ray<bishop,12,7>::value,
    _ray<bishop,13, 0>::value, _ray<bishop,13,1>::value, _ray<bishop,13, 2>::value, _ray<bishop,13,3>::value, _ray<bishop,13,4>::value, _ray<bishop,13,5>::value, _ray<bishop,13, 6>::value, _ray<bishop,13,7>::value,
    _ray<bishop,14, 0>::value, _ray<bishop,14,1>::value, _ray<bishop,14, 2>::value, _ray<bishop,14,3>::value, _ray<bishop,14,4>::value, _ray<bishop,14,5>::value, _ray<bishop,14, 6>::value, _ray<bishop,14,7>::value,
    _ray<bishop,15, 0>::value, _ray<bishop,15,1>::value, _ray<bishop,15, 2>::value, _ray<bishop,15,3>::value, _ray<bishop,15,4>::value, _ray<bishop,15,5>::value, _ray<bishop,15, 6>::value, _ray<bishop,15,7>::value,
    _ray<bishop,16, 0>::value, _ray<bishop,16,1>::value, _ray<bishop,16, 2>::value, _ray<bishop,16,3>::value, _ray<bishop,16,4>::value, _ray<bishop,16,5>::value, _ray<bishop,16, 6>::value, _ray<bishop,16,7>::value,
    _ray<bishop,17, 0>::value, _ray<bishop,17,1>::value, _ray<bishop,17, 2>::value, _ray<bishop,17,3>::value, _ray<bishop,17,4>::value, _ray<bishop,17,5>::value, _ray<bishop,17, 6>::value, _ray<bishop,17,7>::value,
    _ray<bishop,18, 0>::value, _ray<bishop,18,1>::value, _ray<bishop,18, 2>::value, _ray<bishop,18,3>::value, _ray<bishop,18,4>::value, _ray<bishop,18,5>::value, _ray<bishop,18, 6>::value, _ray<bishop,18,7>::value,
    _ray<bishop,19, 0>::value, _ray<bishop,19,1>::value, _ray<bishop,19, 2>::value, _ray<bishop,19,3>::value, _ray<bishop,19,4>::value, _ray<bishop,19,5>::value, _ray<bishop,19, 6>::value, _ray<bishop,19,7>::value,
    _ray<bishop,20, 0>::value, _ray<bishop,20,1>::value, _ray<bishop,20, 2>::value, _ray<bishop,20,3>::value, _ray<bishop,20,4>::value, _ray<bishop,20,5>::value, _ray<bishop,20, 6>::value, _ray<bishop,20,7>::value,
    _ray<bishop,21, 0>::value, _ray<bishop,21,1>::value, _ray<bishop,21, 2>::value, _ray<bishop,21,3>::value, _ray<bishop,21,4>::value, _ray<bishop,21,5>::value, _ray<bishop,21, 6>::value, _ray<bishop,21,7>::value,
    _ray<bishop,22, 0>::value, _ray<bishop,22,1>::value, _ray<bishop,22, 2>::value, _ray<bishop,22,3>::value, _ray<bishop,22,4>::value, _ray<bishop,22,5>::value, _ray<bishop,22, 6>::value, _ray<bishop,22,7>::value,
    _ray<bishop,23, 0>::value, _ray<bishop,23,1>::value, _ray<bishop,23, 2>::value, _ray<bishop,23,3>::value, _ray<bishop,23,4>::value, _ray<bishop,23,5>::value, _ray<bishop,23, 6>::value, _ray<bishop,23,7>::value,
    _ray<bishop,24, 0>::value, _ray<bishop,24,1>::value, _ray<bishop,24, 2>::value, _ray<bishop,24,3>::value, _ray<bishop,24,4>::value, _ray<bishop,24,5>::value, _ray<bishop,24, 6>::value, _ray<bishop,24,7>::value,
    _ray<bishop,25, 0>::value, _ray<bishop,25,1>::value, _ray<bishop,25, 2>::value, _ray<bishop,25,3>::value, _ray<bishop,25,4>::value, _ray<bishop,25,5>::value, _ray<bishop,25, 6>::value, _ray<bishop,25,7>::value,
    _ray<bishop,26, 0>::value, _ray<bishop,26,1>::value, _ray<bishop,26, 2>::value, _ray<bishop,26,3>::value, _ray<bishop,26,4>::value, _ray<bishop,26,5>::value, _ray<bishop,26, 6>::value, _ray<bishop,26,7>::value,
    _ray<bishop,27, 0>::value, _ray<bishop,27,1>::value, _ray<bishop,27, 2>::value, _ray<bishop,27,3>::value, _ray<bishop,27,4>::value, _ray<bishop,27,5>::value, _ray<bishop,27, 6>::value, _ray<bishop,27,7>::value,
    _ray<bishop,28, 0>::value, _ray<bishop,28,1>::value, _ray<bishop,28, 2>::value, _ray<bishop,28,3>::value, _ray<bishop,28,4>::value, _ray<bishop,28,5>::value, _ray<bishop,28, 6>::value, _ray<bishop,28,7>::value,
    _ray<bishop,29, 0>::value, _ray<bishop,29,1>::value, _ray<bishop,29, 2>::value, _ray<bishop,29,3>::value, _ray<bishop,29,4>::value, _ray<bishop,29,5>::value, _ray<bishop,29, 6>::value, _ray<bishop,29,7>::value,
    _ray<bishop,30, 0>::value, _ray<bishop,30,1>::value, _ray<bishop,30, 2>::value, _ray<bishop,30,3>::value, _ray<bishop,30,4>::value, _ray<bishop,30,5>::value, _ray<bishop,30, 6>::value, _ray<bishop,30,7>::value,
    _ray<bishop,31, 0>::value, _ray<bishop,31,1>::value, _ray<bishop,31, 2>::value, _ray<bishop,31,3>::value, _ray<bishop,31,4>::value, _ray<bishop,31,5>::value, _ray<bishop,31, 6>::value, _ray<bishop,31,7>::value,
    _ray<bishop,32, 0>::value, _ray<bishop,32,1>::value, _ray<bishop,32, 2>::value, _ray<bishop,32,3>::value, _ray<bishop,32,4>::value, _ray<bishop,32,5>::value, _ray<bishop,32, 6>::value, _ray<bishop,32,7>::value,
    _ray<bishop,33, 0>::value, _ray<bishop,33,1>::value, _ray<bishop,33, 2>::value, _ray<bishop,33,3>::value, _ray<bishop,33,4>::value, _ray<bishop,33,5>::value, _ray<bishop,33, 6>::value, _ray<bishop,33,7>::value,
    _ray<bishop,34, 0>::value, _ray<bishop,34,1>::value, _ray<bishop,34, 2>::value, _ray<bishop,34,3>::value, _ray<bishop,34,4>::value, _ray<bishop,34,5>::value, _ray<bishop,34, 6>::value, _ray<bishop,34,7>::value,
    _ray<bishop,35, 0>::value, _ray<bishop,35,1>::value, _ray<bishop,35, 2>::value, _ray<bishop,35,3>::value, _ray<bishop,35,4>::value, _ray<bishop,35,5>::value, _ray<bishop,35, 6>::value, _ray<bishop,35,7>::value,
    _ray<bishop,36, 0>::value, _ray<bishop,36,1>::value, _ray<bishop,36, 2>::value, _ray<bishop,36,3>::value, _ray<bishop,36,4>::value, _ray<bishop,36,5>::value, _ray<bishop,36, 6>::value, _ray<bishop,36,7>::value,
    _ray<bishop,37, 0>::value, _ray<bishop,37,1>::value, _ray<bishop,37, 2>::value, _ray<bishop,37,3>::value, _ray<bishop,37,4>::value, _ray<bishop,37,5>::value, _ray<bishop,37, 6>::value, _ray<bishop,37,7>::value,
    _ray<bishop,38, 0>::value, _ray<bishop,38,1>::value, _ray<bishop,38, 2>::value, _ray<bishop,38,3>::value, _ray<bishop,38,4>::value, _ray<bishop,38,5>::value, _ray<bishop,38, 6>::value, _ray<bishop,38,7>::value,
    _ray<bishop,39, 0>::value, _ray<bishop,39,1>::value, _ray<bishop,39, 2>::value, _ray<bishop,39,3>::value, _ray<bishop,39,4>::value, _ray<bishop,39,5>::value, _ray<bishop,39, 6>::value, _ray<bishop,39,7>::value,
    _ray<bishop,40, 0>::value, _ray<bishop,40,1>::value, _ray<bishop,40, 2>::value, _ray<bishop,40,3>::value, _ray<bishop,40,4>::value, _ray<bishop,40,5>::value, _ray<bishop,40, 6>::value, _ray<bishop,40,7>::value,
    _ray<bishop,41, 0>::value, _ray<bishop,41,1>::value, _ray<bishop,41, 2>::value, _ray<bishop,41,3>::value, _ray<bishop,41,4>::value, _ray<bishop,41,5>::value, _ray<bishop,41, 6>::value, _ray<bishop,41,7>::value,
    _ray<bishop,42, 0>::value, _ray<bishop,42,1>::value, _ray<bishop,42, 2>::value, _ray<bishop,42,3>::value, _ray<bishop,42,4>::value, _ray<bishop,42,5>::value, _ray<bishop,42, 6>::value, _ray<bishop,42,7>::value,
    _ray<bishop,43, 0>::value, _ray<bishop,43,1>::value, _ray<bishop,43, 2>::value, _ray<bishop,43,3>::value, _ray<bishop,43,4>::value, _ray<bishop,43,5>::value, _ray<bishop,43, 6>::value, _ray<bishop,43,7>::value,
    _ray<bishop,44, 0>::value, _ray<bishop,44,1>::value, _ray<bishop,44, 2>::value, _ray<bishop,44,3>::value, _ray<bishop,44,4>::value, _ray<bishop,44,5>::value, _ray<bishop,44, 6>::value, _ray<bishop,44,7>::value,
    _ray<bishop,45, 0>::value, _ray<bishop,45,1>::value, _ray<bishop,45, 2>::value, _ray<bishop,45,3>::value, _ray<bishop,45,4>::value, _ray<bishop,45,5>::value, _ray<bishop,45, 6>::value, _ray<bishop,45,7>::value,
    _ray<bishop,46, 0>::value, _ray<bishop,46,1>::value, _ray<bishop,46, 2>::value, _ray<bishop,46,3>::value, _ray<bishop,46,4>::value, _ray<bishop,46,5>::value, _ray<bishop,46, 6>::value, _ray<bishop,46,7>::value,
    _ray<bishop,47, 0>::value, _ray<bishop,47,1>::value, _ray<bishop,47, 2>::value, _ray<bishop,47,3>::value, _ray<bishop,47,4>::value, _ray<bishop,47,5>::value, _ray<bishop,47, 6>::value, _ray<bishop,47,7>::value,
    _ray<bishop,48, 0>::value, _ray<bishop,48,1>::value, _ray<bishop,48, 2>::value, _ray<bishop,48,3>::value, _ray<bishop,48,4>::value, _ray<bishop,48,5>::value, _ray<bishop,48, 6>::value, _ray<bishop,48,7>::value,
    _ray<bishop,49, 0>::value, _ray<bishop,49,1>::value, _ray<bishop,49, 2>::value, _ray<bishop,49,3>::value, _ray<bishop,49,4>::value, _ray<bishop,49,5>::value, _ray<bishop,49, 6>::value, _ray<bishop,49,7>::value,
    _ray<bishop,50, 0>::value, _ray<bishop,50,1>::value, _ray<bishop,50, 2>::value, _ray<bishop,50,3>::value, _ray<bishop,50,4>::value, _ray<bishop,50,5>::value, _ray<bishop,50, 6>::value, _ray<bishop,50,7>::value,
    _ray<bishop,51, 0>::value, _ray<bishop,51,1>::value, _ray<bishop,51, 2>::value, _ray<bishop,51,3>::value, _ray<bishop,51,4>::value, _ray<bishop,51,5>::value, _ray<bishop,51, 6>::value, _ray<bishop,51,7>::value,
    _ray<bishop,52, 0>::value, _ray<bishop,52,1>::value, _ray<bishop,52, 2>::value, _ray<bishop,52,3>::value, _ray<bishop,52,4>::value, _ray<bishop,52,5>::value, _ray<bishop,52, 6>::value, _ray<bishop,52,7>::value,
    _ray<bishop,53, 0>::value, _ray<bishop,53,1>::value, _ray<bishop,53, 2>::value, _ray<bishop,53,3>::value, _ray<bishop,53,4>::value, _ray<bishop,53,5>::value, _ray<bishop,53, 6>::value, _ray<bishop,53,7>::value,
    _ray<bishop,54, 0>::value, _ray<bishop,54,1>::value, _ray<bishop,54, 2>::value, _ray<bishop,54,3>::value, _ray<bishop,54,4>::value, _ray<bishop,54,5>::value, _ray<bishop,54, 6>::value, _ray<bishop,54,7>::value,
    _ray<bishop,55, 0>::value, _ray<bishop,55,1>::value, _ray<bishop,55, 2>::value, _ray<bishop,55,3>::value, _ray<bishop,55,4>::value, _ray<bishop,55,5>::value, _ray<bishop,55, 6>::value, _ray<bishop,55,7>::value,
    _ray<bishop,56, 0>::value, _ray<bishop,56,1>::value, _ray<bishop,56, 2>::value, _ray<bishop,56,3>::value, _ray<bishop,56,4>::value, _ray<bishop,56,5>::value, _ray<bishop,56, 6>::value, _ray<bishop,56,7>::value,
    _ray<bishop,57, 0>::value, _ray<bishop,57,1>::value, _ray<bishop,57, 2>::value, _ray<bishop,57,3>::value, _ray<bishop,57,4>::value, _ray<bishop,57,5>::value, _ray<bishop,57, 6>::value, _ray<bishop,57,7>::value,
    _ray<bishop,58, 0>::value, _ray<bishop,58,1>::value, _ray<bishop,58, 2>::value, _ray<bishop,58,3>::value, _ray<bishop,58,4>::value, _ray<bishop,58,5>::value, _ray<bishop,58, 6>::value, _ray<bishop,58,7>::value,
    _ray<bishop,59, 0>::value, _ray<bishop,59,1>::value, _ray<bishop,59, 2>::value, _ray<bishop,59,3>::value, _ray<bishop,59,4>::value, _ray<bishop,59,5>::value, _ray<bishop,59, 6>::value, _ray<bishop,59,7>::value,
    _ray<bishop,60, 0>::value, _ray<bishop,60,1>::value, _ray<bishop,60, 2>::value, _ray<bishop,60,3>::value, _ray<bishop,60,4>::value, _ray<bishop,60,5>::value, _ray<bishop,60, 6>::value, _ray<bishop,60,7>::value,
    _ray<bishop,61, 0>::value, _ray<bishop,61,1>::value, _ray<bishop,61, 2>::value, _ray<bishop,61,3>::value, _ray<bishop,61,4>::value, _ray<bishop,61,5>::value, _ray<bishop,61, 6>::value, _ray<bishop,61,7>::value,
    _ray<bishop,62, 0>::value, _ray<bishop,62,1>::value, _ray<bishop,62, 2>::value, _ray<bishop,62,3>::value, _ray<bishop,62,4>::value, _ray<bishop,62,5>::value, _ray<bishop,62, 6>::value, _ray<bishop,62,7>::value,
    _ray<bishop,63, 0>::value, _ray<bishop,63,1>::value, _ray<bishop,63, 2>::value, _ray<bishop,63,3>::value, _ray<bishop,63,4>::value, _ray<bishop,63,5>::value, _ray<bishop,63, 6>::value, _ray<bishop,63,7>::value,
    _ray<  rook, 0, 0>::value, _ray<  rook, 0,1>::value, _ray<  rook, 0, 2>::value, _ray<  rook, 0,3>::value, _ray<  rook, 0,4>::value, _ray<  rook, 0,5>::value, _ray<  rook, 0, 6>::value, _ray<  rook, 0,7>::value,
    _ray<  rook, 1, 0>::value, _ray<  rook, 1,1>::value, _ray<  rook, 1, 2>::value, _ray<  rook, 1,3>::value, _ray<  rook, 1,4>::value, _ray<  rook, 1,5>::value, _ray<  rook, 1, 6>::value, _ray<  rook, 1,7>::value,
    _ray<  rook, 2, 0>::value, _ray<  rook, 2,1>::value, _ray<  rook, 2, 2>::value, _ray<  rook, 2,3>::value, _ray<  rook, 2,4>::value, _ray<  rook, 2,5>::value, _ray<  rook, 2, 6>::value, _ray<  rook, 2,7>::value,
    _ray<  rook, 3, 0>::value, _ray<  rook, 3,1>::value, _ray<  rook, 3, 2>::value, _ray<  rook, 3,3>::value, _ray<  rook, 3,4>::value, _ray<  rook, 3,5>::value, _ray<  rook, 3, 6>::value, _ray<  rook, 3,7>::value,
    _ray<  rook, 4, 0>::value, _ray<  rook, 4,1>::value, _ray<  rook, 4, 2>::value, _ray<  rook, 4,3>::value, _ray<  rook, 4,4>::value, _ray<  rook, 4,5>::value, _ray<  rook, 4, 6>::value, _ray<  rook, 4,7>::value,
    _ray<  rook, 5, 0>::value, _ray<  rook, 5,1>::value, _ray<  rook, 5, 2>::value, _ray<  rook, 5,3>::value, _ray<  rook, 5,4>::value, _ray<  rook, 5,5>::value, _ray<  rook, 5, 6>::value, _ray<  rook, 5,7>::value,
    _ray<  rook, 6, 0>::value, _ray<  rook, 6,1>::value, _ray<  rook, 6, 2>::value, _ray<  rook, 6,3>::value, _ray<  rook, 6,4>::value, _ray<  rook, 6,5>::value, _ray<  rook, 6, 6>::value, _ray<  rook, 6,7>::value,
    _ray<  rook, 7, 0>::value, _ray<  rook, 7,1>::value, _ray<  rook, 7, 2>::value, _ray<  rook, 7,3>::value, _ray<  rook, 7,4>::value, _ray<  rook, 7,5>::value, _ray<  rook, 7, 6>::value, _ray<  rook, 7,7>::value,
    _ray<  rook, 8, 0>::value, _ray<  rook, 8,1>::value, _ray<  rook, 8, 2>::value, _ray<  rook, 8,3>::value, _ray<  rook, 8,4>::value, _ray<  rook, 8,5>::value, _ray<  rook, 8, 6>::value, _ray<  rook, 8,7>::value,
    _ray<  rook, 9, 0>::value, _ray<  rook, 9,1>::value, _ray<  rook, 9, 2>::value, _ray<  rook, 9,3>::value, _ray<  rook, 9,4>::value, _ray<  rook, 9,5>::value, _ray<  rook, 9, 6>::value, _ray<  rook, 9,7>::value,
    _ray<  rook,10, 0>::value, _ray<  rook,10,1>::value, _ray<  rook,10, 2>::value, _ray<  rook,10,3>::value, _ray<  rook,10,4>::value, _ray<  rook,10,5>::value, _ray<  rook,10, 6>::value, _ray<  rook,10,7>::value,
    _ray<  rook,11, 0>::value, _ray<  rook,11,1>::value, _ray<  rook,11, 2>::value, _ray<  rook,11,3>::value, _ray<  rook,11,4>::value, _ray<  rook,11,5>::value, _ray<  rook,11, 6>::value, _ray<  rook,11,7>::value,
    _ray<  rook,12, 0>::value, _ray<  rook,12,1>::value, _ray<  rook,12, 2>::value, _ray<  rook,12,3>::value, _ray<  rook,12,4>::value, _ray<  rook,12,5>::value, _ray<  rook,12, 6>::value, _ray<  rook,12,7>::value,
    _ray<  rook,13, 0>::value, _ray<  rook,13,1>::value, _ray<  rook,13, 2>::value, _ray<  rook,13,3>::value, _ray<  rook,13,4>::value, _ray<  rook,13,5>::value, _ray<  rook,13, 6>::value, _ray<  rook,13,7>::value,
    _ray<  rook,14, 0>::value, _ray<  rook,14,1>::value, _ray<  rook,14, 2>::value, _ray<  rook,14,3>::value, _ray<  rook,14,4>::value, _ray<  rook,14,5>::value, _ray<  rook,14, 6>::value, _ray<  rook,14,7>::value,
    _ray<  rook,15, 0>::value, _ray<  rook,15,1>::value, _ray<  rook,15, 2>::value, _ray<  rook,15,3>::value, _ray<  rook,15,4>::value, _ray<  rook,15,5>::value, _ray<  rook,15, 6>::value, _ray<  rook,15,7>::value,
    _ray<  rook,16, 0>::value, _ray<  rook,16,1>::value, _ray<  rook,16, 2>::value, _ray<  rook,16,3>::value, _ray<  rook,16,4>::value, _ray<  rook,16,5>::value, _ray<  rook,16, 6>::value, _ray<  rook,16,7>::value,
    _ray<  rook,17, 0>::value, _ray<  rook,17,1>::value, _ray<  rook,17, 2>::value, _ray<  rook,17,3>::value, _ray<  rook,17,4>::value, _ray<  rook,17,5>::value, _ray<  rook,17, 6>::value, _ray<  rook,17,7>::value,
    _ray<  rook,18, 0>::value, _ray<  rook,18,1>::value, _ray<  rook,18, 2>::value, _ray<  rook,18,3>::value, _ray<  rook,18,4>::value, _ray<  rook,18,5>::value, _ray<  rook,18, 6>::value, _ray<  rook,18,7>::value,
    _ray<  rook,19, 0>::value, _ray<  rook,19,1>::value, _ray<  rook,19, 2>::value, _ray<  rook,19,3>::value, _ray<  rook,19,4>::value, _ray<  rook,19,5>::value, _ray<  rook,19, 6>::value, _ray<  rook,19,7>::value,
    _ray<  rook,20, 0>::value, _ray<  rook,20,1>::value, _ray<  rook,20, 2>::value, _ray<  rook,20,3>::value, _ray<  rook,20,4>::value, _ray<  rook,20,5>::value, _ray<  rook,20, 6>::value, _ray<  rook,20,7>::value,
    _ray<  rook,21, 0>::value, _ray<  rook,21,1>::value, _ray<  rook,21, 2>::value, _ray<  rook,21,3>::value, _ray<  rook,21,4>::value, _ray<  rook,21,5>::value, _ray<  rook,21, 6>::value, _ray<  rook,21,7>::value,
    _ray<  rook,22, 0>::value, _ray<  rook,22,1>::value, _ray<  rook,22, 2>::value, _ray<  rook,22,3>::value, _ray<  rook,22,4>::value, _ray<  rook,22,5>::value, _ray<  rook,22, 6>::value, _ray<  rook,22,7>::value,
    _ray<  rook,23, 0>::value, _ray<  rook,23,1>::value, _ray<  rook,23, 2>::value, _ray<  rook,23,3>::value, _ray<  rook,23,4>::value, _ray<  rook,23,5>::value, _ray<  rook,23, 6>::value, _ray<  rook,23,7>::value,
    _ray<  rook,24, 0>::value, _ray<  rook,24,1>::value, _ray<  rook,24, 2>::value, _ray<  rook,24,3>::value, _ray<  rook,24,4>::value, _ray<  rook,24,5>::value, _ray<  rook,24, 6>::value, _ray<  rook,24,7>::value,
    _ray<  rook,25, 0>::value, _ray<  rook,25,1>::value, _ray<  rook,25, 2>::value, _ray<  rook,25,3>::value, _ray<  rook,25,4>::value, _ray<  rook,25,5>::value, _ray<  rook,25, 6>::value, _ray<  rook,25,7>::value,
    _ray<  rook,26, 0>::value, _ray<  rook,26,1>::value, _ray<  rook,26, 2>::value, _ray<  rook,26,3>::value, _ray<  rook,26,4>::value, _ray<  rook,26,5>::value, _ray<  rook,26, 6>::value, _ray<  rook,26,7>::value,
    _ray<  rook,27, 0>::value, _ray<  rook,27,1>::value, _ray<  rook,27, 2>::value, _ray<  rook,27,3>::value, _ray<  rook,27,4>::value, _ray<  rook,27,5>::value, _ray<  rook,27, 6>::value, _ray<  rook,27,7>::value,
    _ray<  rook,28, 0>::value, _ray<  rook,28,1>::value, _ray<  rook,28, 2>::value, _ray<  rook,28,3>::value, _ray<  rook,28,4>::value, _ray<  rook,28,5>::value, _ray<  rook,28, 6>::value, _ray<  rook,28,7>::value,
    _ray<  rook,29, 0>::value, _ray<  rook,29,1>::value, _ray<  rook,29, 2>::value, _ray<  rook,29,3>::value, _ray<  rook,29,4>::value, _ray<  rook,29,5>::value, _ray<  rook,29, 6>::value, _ray<  rook,29,7>::value,
    _ray<  rook,30, 0>::value, _ray<  rook,30,1>::value, _ray<  rook,30, 2>::value, _ray<  rook,30,3>::value, _ray<  rook,30,4>::value, _ray<  rook,30,5>::value, _ray<  rook,30, 6>::value, _ray<  rook,30,7>::value,
    _ray<  rook,31, 0>::value, _ray<  rook,31,1>::value, _ray<  rook,31, 2>::value, _ray<  rook,31,3>::value, _ray<  rook,31,4>::value, _ray<  rook,31,5>::value, _ray<  rook,31, 6>::value, _ray<  rook,31,7>::value,
    _ray<  rook,32, 0>::value, _ray<  rook,32,1>::value, _ray<  rook,32, 2>::value, _ray<  rook,32,3>::value, _ray<  rook,32,4>::value, _ray<  rook,32,5>::value, _ray<  rook,32, 6>::value, _ray<  rook,32,7>::value,
    _ray<  rook,33, 0>::value, _ray<  rook,33,1>::value, _ray<  rook,33, 2>::value, _ray<  rook,33,3>::value, _ray<  rook,33,4>::value, _ray<  rook,33,5>::value, _ray<  rook,33, 6>::value, _ray<  rook,33,7>::value,
    _ray<  rook,34, 0>::value, _ray<  rook,34,1>::value, _ray<  rook,34, 2>::value, _ray<  rook,34,3>::value, _ray<  rook,34,4>::value, _ray<  rook,34,5>::value, _ray<  rook,34, 6>::value, _ray<  rook,34,7>::value,
    _ray<  rook,35, 0>::value, _ray<  rook,35,1>::value, _ray<  rook,35, 2>::value, _ray<  rook,35,3>::value, _ray<  rook,35,4>::value, _ray<  rook,35,5>::value, _ray<  rook,35, 6>::value, _ray<  rook,35,7>::value,
    _ray<  rook,36, 0>::value, _ray<  rook,36,1>::value, _ray<  rook,36, 2>::value, _ray<  rook,36,3>::value, _ray<  rook,36,4>::value, _ray<  rook,36,5>::value, _ray<  rook,36, 6>::value, _ray<  rook,36,7>::value,
    _ray<  rook,37, 0>::value, _ray<  rook,37,1>::value, _ray<  rook,37, 2>::value, _ray<  rook,37,3>::value, _ray<  rook,37,4>::value, _ray<  rook,37,5>::value, _ray<  rook,37, 6>::value, _ray<  rook,37,7>::value,
    _ray<  rook,38, 0>::value, _ray<  rook,38,1>::value, _ray<  rook,38, 2>::value, _ray<  rook,38,3>::value, _ray<  rook,38,4>::value, _ray<  rook,38,5>::value, _ray<  rook,38, 6>::value, _ray<  rook,38,7>::value,
    _ray<  rook,39, 0>::value, _ray<  rook,39,1>::value, _ray<  rook,39, 2>::value, _ray<  rook,39,3>::value, _ray<  rook,39,4>::value, _ray<  rook,39,5>::value, _ray<  rook,39, 6>::value, _ray<  rook,39,7>::value,
    _ray<  rook,40, 0>::value, _ray<  rook,40,1>::value, _ray<  rook,40, 2>::value, _ray<  rook,40,3>::value, _ray<  rook,40,4>::value, _ray<  rook,40,5>::value, _ray<  rook,40, 6>::value, _ray<  rook,40,7>::value,
    _ray<  rook,41, 0>::value, _ray<  rook,41,1>::value, _ray<  rook,41, 2>::value, _ray<  rook,41,3>::value, _ray<  rook,41,4>::value, _ray<  rook,41,5>::value, _ray<  rook,41, 6>::value, _ray<  rook,41,7>::value,
    _ray<  rook,42, 0>::value, _ray<  rook,42,1>::value, _ray<  rook,42, 2>::value, _ray<  rook,42,3>::value, _ray<  rook,42,4>::value, _ray<  rook,42,5>::value, _ray<  rook,42, 6>::value, _ray<  rook,42,7>::value,
    _ray<  rook,43, 0>::value, _ray<  rook,43,1>::value, _ray<  rook,43, 2>::value, _ray<  rook,43,3>::value, _ray<  rook,43,4>::value, _ray<  rook,43,5>::value, _ray<  rook,43, 6>::value, _ray<  rook,43,7>::value,
    _ray<  rook,44, 0>::value, _ray<  rook,44,1>::value, _ray<  rook,44, 2>::value, _ray<  rook,44,3>::value, _ray<  rook,44,4>::value, _ray<  rook,44,5>::value, _ray<  rook,44, 6>::value, _ray<  rook,44,7>::value,
    _ray<  rook,45, 0>::value, _ray<  rook,45,1>::value, _ray<  rook,45, 2>::value, _ray<  rook,45,3>::value, _ray<  rook,45,4>::value, _ray<  rook,45,5>::value, _ray<  rook,45, 6>::value, _ray<  rook,45,7>::value,
    _ray<  rook,46, 0>::value, _ray<  rook,46,1>::value, _ray<  rook,46, 2>::value, _ray<  rook,46,3>::value, _ray<  rook,46,4>::value, _ray<  rook,46,5>::value, _ray<  rook,46, 6>::value, _ray<  rook,46,7>::value,
    _ray<  rook,47, 0>::value, _ray<  rook,47,1>::value, _ray<  rook,47, 2>::value, _ray<  rook,47,3>::value, _ray<  rook,47,4>::value, _ray<  rook,47,5>::value, _ray<  rook,47, 6>::value, _ray<  rook,47,7>::value,
    _ray<  rook,48, 0>::value, _ray<  rook,48,1>::value, _ray<  rook,48, 2>::value, _ray<  rook,48,3>::value, _ray<  rook,48,4>::value, _ray<  rook,48,5>::value, _ray<  rook,48, 6>::value, _ray<  rook,48,7>::value,
    _ray<  rook,49, 0>::value, _ray<  rook,49,1>::value, _ray<  rook,49, 2>::value, _ray<  rook,49,3>::value, _ray<  rook,49,4>::value, _ray<  rook,49,5>::value, _ray<  rook,49, 6>::value, _ray<  rook,49,7>::value,
    _ray<  rook,50, 0>::value, _ray<  rook,50,1>::value, _ray<  rook,50, 2>::value, _ray<  rook,50,3>::value, _ray<  rook,50,4>::value, _ray<  rook,50,5>::value, _ray<  rook,50, 6>::value, _ray<  rook,50,7>::value,
    _ray<  rook,51, 0>::value, _ray<  rook,51,1>::value, _ray<  rook,51, 2>::value, _ray<  rook,51,3>::value, _ray<  rook,51,4>::value, _ray<  rook,51,5>::value, _ray<  rook,51, 6>::value, _ray<  rook,51,7>::value,
    _ray<  rook,52, 0>::value, _ray<  rook,52,1>::value, _ray<  rook,52, 2>::value, _ray<  rook,52,3>::value, _ray<  rook,52,4>::value, _ray<  rook,52,5>::value, _ray<  rook,52, 6>::value, _ray<  rook,52,7>::value,
    _ray<  rook,53, 0>::value, _ray<  rook,53,1>::value, _ray<  rook,53, 2>::value, _ray<  rook,53,3>::value, _ray<  rook,53,4>::value, _ray<  rook,53,5>::value, _ray<  rook,53, 6>::value, _ray<  rook,53,7>::value,
    _ray<  rook,54, 0>::value, _ray<  rook,54,1>::value, _ray<  rook,54, 2>::value, _ray<  rook,54,3>::value, _ray<  rook,54,4>::value, _ray<  rook,54,5>::value, _ray<  rook,54, 6>::value, _ray<  rook,54,7>::value,
    _ray<  rook,55, 0>::value, _ray<  rook,55,1>::value, _ray<  rook,55, 2>::value, _ray<  rook,55,3>::value, _ray<  rook,55,4>::value, _ray<  rook,55,5>::value, _ray<  rook,55, 6>::value, _ray<  rook,55,7>::value,
    _ray<  rook,56, 0>::value, _ray<  rook,56,1>::value, _ray<  rook,56, 2>::value, _ray<  rook,56,3>::value, _ray<  rook,56,4>::value, _ray<  rook,56,5>::value, _ray<  rook,56, 6>::value, _ray<  rook,56,7>::value,
    _ray<  rook,57, 0>::value, _ray<  rook,57,1>::value, _ray<  rook,57, 2>::value, _ray<  rook,57,3>::value, _ray<  rook,57,4>::value, _ray<  rook,57,5>::value, _ray<  rook,57, 6>::value, _ray<  rook,57,7>::value,
    _ray<  rook,58, 0>::value, _ray<  rook,58,1>::value, _ray<  rook,58, 2>::value, _ray<  rook,58,3>::value, _ray<  rook,58,4>::value, _ray<  rook,58,5>::value, _ray<  rook,58, 6>::value, _ray<  rook,58,7>::value,
    _ray<  rook,59, 0>::value, _ray<  rook,59,1>::value, _ray<  rook,59, 2>::value, _ray<  rook,59,3>::value, _ray<  rook,59,4>::value, _ray<  rook,59,5>::value, _ray<  rook,59, 6>::value, _ray<  rook,59,7>::value,
    _ray<  rook,60, 0>::value, _ray<  rook,60,1>::value, _ray<  rook,60, 2>::value, _ray<  rook,60,3>::value, _ray<  rook,60,4>::value, _ray<  rook,60,5>::value, _ray<  rook,60, 6>::value, _ray<  rook,60,7>::value,
    _ray<  rook,61, 0>::value, _ray<  rook,61,1>::value, _ray<  rook,61, 2>::value, _ray<  rook,61,3>::value, _ray<  rook,61,4>::value, _ray<  rook,61,5>::value, _ray<  rook,61, 6>::value, _ray<  rook,61,7>::value,
    _ray<  rook,62, 0>::value, _ray<  rook,62,1>::value, _ray<  rook,62, 2>::value, _ray<  rook,62,3>::value, _ray<  rook,62,4>::value, _ray<  rook,62,5>::value, _ray<  rook,62, 6>::value, _ray<  rook,62,7>::value,
    _ray<  rook,63, 0>::value, _ray<  rook,63,1>::value, _ray<  rook,63, 2>::value, _ray<  rook,63,3>::value, _ray<  rook,63,4>::value, _ray<  rook,63,5>::value, _ray<  rook,63, 6>::value, _ray<  rook,63,7>::value,
    _ray< queen, 0, 0>::value, _ray< queen, 0,1>::value, _ray< queen, 0, 2>::value, _ray< queen, 0,3>::value, _ray< queen, 0,4>::value, _ray< queen, 0,5>::value, _ray< queen, 0, 6>::value, _ray< queen, 0,7>::value,
    _ray< queen, 1, 0>::value, _ray< queen, 1,1>::value, _ray< queen, 1, 2>::value, _ray< queen, 1,3>::value, _ray< queen, 1,4>::value, _ray< queen, 1,5>::value, _ray< queen, 1, 6>::value, _ray< queen, 1,7>::value,
    _ray< queen, 2, 0>::value, _ray< queen, 2,1>::value, _ray< queen, 2, 2>::value, _ray< queen, 2,3>::value, _ray< queen, 2,4>::value, _ray< queen, 2,5>::value, _ray< queen, 2, 6>::value, _ray< queen, 2,7>::value,
    _ray< queen, 3, 0>::value, _ray< queen, 3,1>::value, _ray< queen, 3, 2>::value, _ray< queen, 3,3>::value, _ray< queen, 3,4>::value, _ray< queen, 3,5>::value, _ray< queen, 3, 6>::value, _ray< queen, 3,7>::value,
    _ray< queen, 4, 0>::value, _ray< queen, 4,1>::value, _ray< queen, 4, 2>::value, _ray< queen, 4,3>::value, _ray< queen, 4,4>::value, _ray< queen, 4,5>::value, _ray< queen, 4, 6>::value, _ray< queen, 4,7>::value,
    _ray< queen, 5, 0>::value, _ray< queen, 5,1>::value, _ray< queen, 5, 2>::value, _ray< queen, 5,3>::value, _ray< queen, 5,4>::value, _ray< queen, 5,5>::value, _ray< queen, 5, 6>::value, _ray< queen, 5,7>::value,
    _ray< queen, 6, 0>::value, _ray< queen, 6,1>::value, _ray< queen, 6, 2>::value, _ray< queen, 6,3>::value, _ray< queen, 6,4>::value, _ray< queen, 6,5>::value, _ray< queen, 6, 6>::value, _ray< queen, 6,7>::value,
    _ray< queen, 7, 0>::value, _ray< queen, 7,1>::value, _ray< queen, 7, 2>::value, _ray< queen, 7,3>::value, _ray< queen, 7,4>::value, _ray< queen, 7,5>::value, _ray< queen, 7, 6>::value, _ray< queen, 7,7>::value,
    _ray< queen, 8, 0>::value, _ray< queen, 8,1>::value, _ray< queen, 8, 2>::value, _ray< queen, 8,3>::value, _ray< queen, 8,4>::value, _ray< queen, 8,5>::value, _ray< queen, 8, 6>::value, _ray< queen, 8,7>::value,
    _ray< queen, 9, 0>::value, _ray< queen, 9,1>::value, _ray< queen, 9, 2>::value, _ray< queen, 9,3>::value, _ray< queen, 9,4>::value, _ray< queen, 9,5>::value, _ray< queen, 9, 6>::value, _ray< queen, 9,7>::value,
    _ray< queen,10, 0>::value, _ray< queen,10,1>::value, _ray< queen,10, 2>::value, _ray< queen,10,3>::value, _ray< queen,10,4>::value, _ray< queen,10,5>::value, _ray< queen,10, 6>::value, _ray< queen,10,7>::value,
    _ray< queen,11, 0>::value, _ray< queen,11,1>::value, _ray< queen,11, 2>::value, _ray< queen,11,3>::value, _ray< queen,11,4>::value, _ray< queen,11,5>::value, _ray< queen,11, 6>::value, _ray< queen,11,7>::value,
    _ray< queen,12, 0>::value, _ray< queen,12,1>::value, _ray< queen,12, 2>::value, _ray< queen,12,3>::value, _ray< queen,12,4>::value, _ray< queen,12,5>::value, _ray< queen,12, 6>::value, _ray< queen,12,7>::value,
    _ray< queen,13, 0>::value, _ray< queen,13,1>::value, _ray< queen,13, 2>::value, _ray< queen,13,3>::value, _ray< queen,13,4>::value, _ray< queen,13,5>::value, _ray< queen,13, 6>::value, _ray< queen,13,7>::value,
    _ray< queen,14, 0>::value, _ray< queen,14,1>::value, _ray< queen,14, 2>::value, _ray< queen,14,3>::value, _ray< queen,14,4>::value, _ray< queen,14,5>::value, _ray< queen,14, 6>::value, _ray< queen,14,7>::value,
    _ray< queen,15, 0>::value, _ray< queen,15,1>::value, _ray< queen,15, 2>::value, _ray< queen,15,3>::value, _ray< queen,15,4>::value, _ray< queen,15,5>::value, _ray< queen,15, 6>::value, _ray< queen,15,7>::value,
    _ray< queen,16, 0>::value, _ray< queen,16,1>::value, _ray< queen,16, 2>::value, _ray< queen,16,3>::value, _ray< queen,16,4>::value, _ray< queen,16,5>::value, _ray< queen,16, 6>::value, _ray< queen,16,7>::value,
    _ray< queen,17, 0>::value, _ray< queen,17,1>::value, _ray< queen,17, 2>::value, _ray< queen,17,3>::value, _ray< queen,17,4>::value, _ray< queen,17,5>::value, _ray< queen,17, 6>::value, _ray< queen,17,7>::value,
    _ray< queen,18, 0>::value, _ray< queen,18,1>::value, _ray< queen,18, 2>::value, _ray< queen,18,3>::value, _ray< queen,18,4>::value, _ray< queen,18,5>::value, _ray< queen,18, 6>::value, _ray< queen,18,7>::value,
    _ray< queen,19, 0>::value, _ray< queen,19,1>::value, _ray< queen,19, 2>::value, _ray< queen,19,3>::value, _ray< queen,19,4>::value, _ray< queen,19,5>::value, _ray< queen,19, 6>::value, _ray< queen,19,7>::value,
    _ray< queen,20, 0>::value, _ray< queen,20,1>::value, _ray< queen,20, 2>::value, _ray< queen,20,3>::value, _ray< queen,20,4>::value, _ray< queen,20,5>::value, _ray< queen,20, 6>::value, _ray< queen,20,7>::value,
    _ray< queen,21, 0>::value, _ray< queen,21,1>::value, _ray< queen,21, 2>::value, _ray< queen,21,3>::value, _ray< queen,21,4>::value, _ray< queen,21,5>::value, _ray< queen,21, 6>::value, _ray< queen,21,7>::value,
    _ray< queen,22, 0>::value, _ray< queen,22,1>::value, _ray< queen,22, 2>::value, _ray< queen,22,3>::value, _ray< queen,22,4>::value, _ray< queen,22,5>::value, _ray< queen,22, 6>::value, _ray< queen,22,7>::value,
    _ray< queen,23, 0>::value, _ray< queen,23,1>::value, _ray< queen,23, 2>::value, _ray< queen,23,3>::value, _ray< queen,23,4>::value, _ray< queen,23,5>::value, _ray< queen,23, 6>::value, _ray< queen,23,7>::value,
    _ray< queen,24, 0>::value, _ray< queen,24,1>::value, _ray< queen,24, 2>::value, _ray< queen,24,3>::value, _ray< queen,24,4>::value, _ray< queen,24,5>::value, _ray< queen,24, 6>::value, _ray< queen,24,7>::value,
    _ray< queen,25, 0>::value, _ray< queen,25,1>::value, _ray< queen,25, 2>::value, _ray< queen,25,3>::value, _ray< queen,25,4>::value, _ray< queen,25,5>::value, _ray< queen,25, 6>::value, _ray< queen,25,7>::value,
    _ray< queen,26, 0>::value, _ray< queen,26,1>::value, _ray< queen,26, 2>::value, _ray< queen,26,3>::value, _ray< queen,26,4>::value, _ray< queen,26,5>::value, _ray< queen,26, 6>::value, _ray< queen,26,7>::value,
    _ray< queen,27, 0>::value, _ray< queen,27,1>::value, _ray< queen,27, 2>::value, _ray< queen,27,3>::value, _ray< queen,27,4>::value, _ray< queen,27,5>::value, _ray< queen,27, 6>::value, _ray< queen,27,7>::value,
    _ray< queen,28, 0>::value, _ray< queen,28,1>::value, _ray< queen,28, 2>::value, _ray< queen,28,3>::value, _ray< queen,28,4>::value, _ray< queen,28,5>::value, _ray< queen,28, 6>::value, _ray< queen,28,7>::value,
    _ray< queen,29, 0>::value, _ray< queen,29,1>::value, _ray< queen,29, 2>::value, _ray< queen,29,3>::value, _ray< queen,29,4>::value, _ray< queen,29,5>::value, _ray< queen,29, 6>::value, _ray< queen,29,7>::value,
    _ray< queen,30, 0>::value, _ray< queen,30,1>::value, _ray< queen,30, 2>::value, _ray< queen,30,3>::value, _ray< queen,30,4>::value, _ray< queen,30,5>::value, _ray< queen,30, 6>::value, _ray< queen,30,7>::value,
    _ray< queen,31, 0>::value, _ray< queen,31,1>::value, _ray< queen,31, 2>::value, _ray< queen,31,3>::value, _ray< queen,31,4>::value, _ray< queen,31,5>::value, _ray< queen,31, 6>::value, _ray< queen,31,7>::value,
    _ray< queen,32, 0>::value, _ray< queen,32,1>::value, _ray< queen,32, 2>::value, _ray< queen,32,3>::value, _ray< queen,32,4>::value, _ray< queen,32,5>::value, _ray< queen,32, 6>::value, _ray< queen,32,7>::value,
    _ray< queen,33, 0>::value, _ray< queen,33,1>::value, _ray< queen,33, 2>::value, _ray< queen,33,3>::value, _ray< queen,33,4>::value, _ray< queen,33,5>::value, _ray< queen,33, 6>::value, _ray< queen,33,7>::value,
    _ray< queen,34, 0>::value, _ray< queen,34,1>::value, _ray< queen,34, 2>::value, _ray< queen,34,3>::value, _ray< queen,34,4>::value, _ray< queen,34,5>::value, _ray< queen,34, 6>::value, _ray< queen,34,7>::value,
    _ray< queen,35, 0>::value, _ray< queen,35,1>::value, _ray< queen,35, 2>::value, _ray< queen,35,3>::value, _ray< queen,35,4>::value, _ray< queen,35,5>::value, _ray< queen,35, 6>::value, _ray< queen,35,7>::value,
    _ray< queen,36, 0>::value, _ray< queen,36,1>::value, _ray< queen,36, 2>::value, _ray< queen,36,3>::value, _ray< queen,36,4>::value, _ray< queen,36,5>::value, _ray< queen,36, 6>::value, _ray< queen,36,7>::value,
    _ray< queen,37, 0>::value, _ray< queen,37,1>::value, _ray< queen,37, 2>::value, _ray< queen,37,3>::value, _ray< queen,37,4>::value, _ray< queen,37,5>::value, _ray< queen,37, 6>::value, _ray< queen,37,7>::value,
    _ray< queen,38, 0>::value, _ray< queen,38,1>::value, _ray< queen,38, 2>::value, _ray< queen,38,3>::value, _ray< queen,38,4>::value, _ray< queen,38,5>::value, _ray< queen,38, 6>::value, _ray< queen,38,7>::value,
    _ray< queen,39, 0>::value, _ray< queen,39,1>::value, _ray< queen,39, 2>::value, _ray< queen,39,3>::value, _ray< queen,39,4>::value, _ray< queen,39,5>::value, _ray< queen,39, 6>::value, _ray< queen,39,7>::value,
    _ray< queen,40, 0>::value, _ray< queen,40,1>::value, _ray< queen,40, 2>::value, _ray< queen,40,3>::value, _ray< queen,40,4>::value, _ray< queen,40,5>::value, _ray< queen,40, 6>::value, _ray< queen,40,7>::value,
    _ray< queen,41, 0>::value, _ray< queen,41,1>::value, _ray< queen,41, 2>::value, _ray< queen,41,3>::value, _ray< queen,41,4>::value, _ray< queen,41,5>::value, _ray< queen,41, 6>::value, _ray< queen,41,7>::value,
    _ray< queen,42, 0>::value, _ray< queen,42,1>::value, _ray< queen,42, 2>::value, _ray< queen,42,3>::value, _ray< queen,42,4>::value, _ray< queen,42,5>::value, _ray< queen,42, 6>::value, _ray< queen,42,7>::value,
    _ray< queen,43, 0>::value, _ray< queen,43,1>::value, _ray< queen,43, 2>::value, _ray< queen,43,3>::value, _ray< queen,43,4>::value, _ray< queen,43,5>::value, _ray< queen,43, 6>::value, _ray< queen,43,7>::value,
    _ray< queen,44, 0>::value, _ray< queen,44,1>::value, _ray< queen,44, 2>::value, _ray< queen,44,3>::value, _ray< queen,44,4>::value, _ray< queen,44,5>::value, _ray< queen,44, 6>::value, _ray< queen,44,7>::value,
    _ray< queen,45, 0>::value, _ray< queen,45,1>::value, _ray< queen,45, 2>::value, _ray< queen,45,3>::value, _ray< queen,45,4>::value, _ray< queen,45,5>::value, _ray< queen,45, 6>::value, _ray< queen,45,7>::value,
    _ray< queen,46, 0>::value, _ray< queen,46,1>::value, _ray< queen,46, 2>::value, _ray< queen,46,3>::value, _ray< queen,46,4>::value, _ray< queen,46,5>::value, _ray< queen,46, 6>::value, _ray< queen,46,7>::value,
    _ray< queen,47, 0>::value, _ray< queen,47,1>::value, _ray< queen,47, 2>::value, _ray< queen,47,3>::value, _ray< queen,47,4>::value, _ray< queen,47,5>::value, _ray< queen,47, 6>::value, _ray< queen,47,7>::value,
    _ray< queen,48, 0>::value, _ray< queen,48,1>::value, _ray< queen,48, 2>::value, _ray< queen,48,3>::value, _ray< queen,48,4>::value, _ray< queen,48,5>::value, _ray< queen,48, 6>::value, _ray< queen,48,7>::value,
    _ray< queen,49, 0>::value, _ray< queen,49,1>::value, _ray< queen,49, 2>::value, _ray< queen,49,3>::value, _ray< queen,49,4>::value, _ray< queen,49,5>::value, _ray< queen,49, 6>::value, _ray< queen,49,7>::value,
    _ray< queen,50, 0>::value, _ray< queen,50,1>::value, _ray< queen,50, 2>::value, _ray< queen,50,3>::value, _ray< queen,50,4>::value, _ray< queen,50,5>::value, _ray< queen,50, 6>::value, _ray< queen,50,7>::value,
    _ray< queen,51, 0>::value, _ray< queen,51,1>::value, _ray< queen,51, 2>::value, _ray< queen,51,3>::value, _ray< queen,51,4>::value, _ray< queen,51,5>::value, _ray< queen,51, 6>::value, _ray< queen,51,7>::value,
    _ray< queen,52, 0>::value, _ray< queen,52,1>::value, _ray< queen,52, 2>::value, _ray< queen,52,3>::value, _ray< queen,52,4>::value, _ray< queen,52,5>::value, _ray< queen,52, 6>::value, _ray< queen,52,7>::value,
    _ray< queen,53, 0>::value, _ray< queen,53,1>::value, _ray< queen,53, 2>::value, _ray< queen,53,3>::value, _ray< queen,53,4>::value, _ray< queen,53,5>::value, _ray< queen,53, 6>::value, _ray< queen,53,7>::value,
    _ray< queen,54, 0>::value, _ray< queen,54,1>::value, _ray< queen,54, 2>::value, _ray< queen,54,3>::value, _ray< queen,54,4>::value, _ray< queen,54,5>::value, _ray< queen,54, 6>::value, _ray< queen,54,7>::value,
    _ray< queen,55, 0>::value, _ray< queen,55,1>::value, _ray< queen,55, 2>::value, _ray< queen,55,3>::value, _ray< queen,55,4>::value, _ray< queen,55,5>::value, _ray< queen,55, 6>::value, _ray< queen,55,7>::value,
    _ray< queen,56, 0>::value, _ray< queen,56,1>::value, _ray< queen,56, 2>::value, _ray< queen,56,3>::value, _ray< queen,56,4>::value, _ray< queen,56,5>::value, _ray< queen,56, 6>::value, _ray< queen,56,7>::value,
    _ray< queen,57, 0>::value, _ray< queen,57,1>::value, _ray< queen,57, 2>::value, _ray< queen,57,3>::value, _ray< queen,57,4>::value, _ray< queen,57,5>::value, _ray< queen,57, 6>::value, _ray< queen,57,7>::value,
    _ray< queen,58, 0>::value, _ray< queen,58,1>::value, _ray< queen,58, 2>::value, _ray< queen,58,3>::value, _ray< queen,58,4>::value, _ray< queen,58,5>::value, _ray< queen,58, 6>::value, _ray< queen,58,7>::value,
    _ray< queen,59, 0>::value, _ray< queen,59,1>::value, _ray< queen,59, 2>::value, _ray< queen,59,3>::value, _ray< queen,59,4>::value, _ray< queen,59,5>::value, _ray< queen,59, 6>::value, _ray< queen,59,7>::value,
    _ray< queen,60, 0>::value, _ray< queen,60,1>::value, _ray< queen,60, 2>::value, _ray< queen,60,3>::value, _ray< queen,60,4>::value, _ray< queen,60,5>::value, _ray< queen,60, 6>::value, _ray< queen,60,7>::value,
    _ray< queen,61, 0>::value, _ray< queen,61,1>::value, _ray< queen,61, 2>::value, _ray< queen,61,3>::value, _ray< queen,61,4>::value, _ray< queen,61,5>::value, _ray< queen,61, 6>::value, _ray< queen,61,7>::value,
    _ray< queen,62, 0>::value, _ray< queen,62,1>::value, _ray< queen,62, 2>::value, _ray< queen,62,3>::value, _ray< queen,62,4>::value, _ray< queen,62,5>::value, _ray< queen,62, 6>::value, _ray< queen,62,7>::value,
    _ray< queen,63, 0>::value, _ray< queen,63,1>::value, _ray< queen,63, 2>::value, _ray< queen,63,3>::value, _ray< queen,63,4>::value, _ray< queen,63,5>::value, _ray< queen,63, 6>::value, _ray< queen,63,7>::value
};

namespace {
    // Flip least and most significant 1-bits
    U64 exflip(U64 x){ return (x & (x-1)) & (x ^ mask(bitscanr(x))); }

    U64 batkm(square sq){ return ~(ONE_ << sq) & (exflip(dmasks[sq]) | exflip(amasks[sq])); }

    U64 ratkm(square sq){ return ~(ONE_ << sq) & (exflip(rmasks[sq]) | exflip(fmasks[sq])); }
}

const std::array<U64, 64> battackm = {
    batkm( 0), batkm( 1), batkm( 2), batkm( 3), batkm( 4), batkm( 5), batkm( 6), batkm( 7),
    batkm( 8), batkm( 9), batkm(10), batkm(11), batkm(12), batkm(13), batkm(14), batkm(15),
    batkm(16), batkm(17), batkm(18), batkm(19), batkm(20), batkm(21), batkm(22), batkm(23),
    batkm(24), batkm(25), batkm(26), batkm(27), batkm(28), batkm(29), batkm(30), batkm(31),
    batkm(32), batkm(33), batkm(34), batkm(35), batkm(36), batkm(37), batkm(38), batkm(39),
    batkm(40), batkm(41), batkm(42), batkm(43), batkm(44), batkm(45), batkm(46), batkm(47),
    batkm(48), batkm(49), batkm(50), batkm(51), batkm(52), batkm(53), batkm(54), batkm(55),
    batkm(56), batkm(57), batkm(58), batkm(59), batkm(60), batkm(61), batkm(62), batkm(63)
};

const std::array<U64, 64> rattackm = {
    ratkm( 0), ratkm( 1), ratkm( 2), ratkm( 3), ratkm( 4), ratkm( 5), ratkm( 6), ratkm( 7),
    ratkm( 8), ratkm( 9), ratkm(10), ratkm(11), ratkm(12), ratkm(13), ratkm(14), ratkm(15),
    ratkm(16), ratkm(17), ratkm(18), ratkm(19), ratkm(20), ratkm(21), ratkm(22), ratkm(23),
    ratkm(24), ratkm(25), ratkm(26), ratkm(27), ratkm(28), ratkm(29), ratkm(30), ratkm(31),
    ratkm(32), ratkm(33), ratkm(34), ratkm(35), ratkm(36), ratkm(37), ratkm(38), ratkm(39),
    ratkm(40), ratkm(41), ratkm(42), ratkm(43), ratkm(44), ratkm(45), ratkm(46), ratkm(47),
    ratkm(48), ratkm(49), ratkm(50), ratkm(51), ratkm(52), ratkm(53), ratkm(54), ratkm(55),
    ratkm(56), ratkm(57), ratkm(58), ratkm(59), ratkm(60), ratkm(61), ratkm(62), ratkm(63)
};

const std::string pieces = "KQRBN";

const std::string files = "abcdefgh";

const std::string ranks = "12345678";

const std::string endl(1, '\n');

const std::string bviz = endl
                        + "   " + files[0]
                        + "   " + files[1]
                        + "   " + files[2]
                        + "   " + files[3]
                        + "   " + files[4]
                        + "   " + files[5]
                        + "   " + files[6]
                        + "   " + files[7]
                        + endl + ranks[7] + ' '
                        + bcell + bcell + bcell + bcell + bcell + bcell + bcell + bcell
                        + endl + ranks[6] + ' '
                        + bcell + bcell + bcell + bcell + bcell + bcell + bcell + bcell
                        + endl + ranks[5] + ' '
                        + bcell + bcell + bcell + bcell + bcell + bcell + bcell + bcell
                        + endl + ranks[4] + ' '
                        + bcell + bcell + bcell + bcell + bcell + bcell + bcell + bcell
                        + endl + ranks[3] + ' '
                        + bcell + bcell + bcell + bcell + bcell + bcell + bcell + bcell
                        + endl + ranks[2] + ' '
                        + bcell + bcell + bcell + bcell + bcell + bcell + bcell + bcell
                        + endl + ranks[1] + ' '
                        + bcell + bcell + bcell + bcell + bcell + bcell + bcell + bcell
                        + endl + ranks[0] + ' '
                        + bcell + bcell + bcell + bcell + bcell + bcell + bcell + bcell
                        + endl;

const std::string bbviz = endl
                        + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + endl
                        + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + endl
                        + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + endl
                        + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + endl
                        + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + endl
                        + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + endl
                        + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + endl
                        + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + bbcell + endl;

std::map<std::string, pgntag> s2tag = {
    {"event", event},
    {"site", site},
    {"date", date},
    {"round", roundn},
    {"white", playerw},
    {"black", playerb},
    {"result", result},
    {"eco", eco},
    {"fen", fenstr},
    {"mode", mode},
    {"timecontrol", time_control},
    {"termination", termination},
    {"whiteelo", white_elo},
    {"blackelo", black_elo},
    {"whiteuscf", white_uscf},
    {"blackuscf", black_uscf}
};

std::map<pgntag, std::string> tag2s = {
    {event, "event"},
    {site, "site"},
    {date, "date"},
    {roundn, "round"},
    {playerw, "white"},
    {playerb, "black"},
    {result, "result"},
    {eco, "eco"},
    {fenstr, "fen"},
    {mode, "mode"},
    {time_control, "timecontrol"},
    {termination, "termination"},
    {white_elo, "whiteelo"},
    {black_elo, "blackelo"},
    {white_uscf, "whiteuscf"},
    {black_uscf, "blackuscf"}
};

// Track if magic bitboards have been initialized
bool initialized = false;

// Arrays for magic numbers
Magic BMagics[64];

Magic RMagics[64];

// Arrays for all possible sliding attack patterns
U64 BishopAttacks[5248];

U64 RookAttacks[102400];

// PRNG for producing magic numbers
std::random_device rd;
std::mt19937_64 gen(rd());
std::uniform_int_distribution<U64> dist;

/**************************************************
                FUNCTION DEFINITIONS
**************************************************/

// Define here to prevent compilier from attempting to inline
game::~game() = default;

bool ispiece(char c){ return pieces.find(c) != std::string::npos; }

bool isfile(char c){ return files.find(c) != std::string::npos; }

// Convert to string
std::string ptype2s(ptype pt){
    switch(pt){
        case pawn:   return "pawn";
        case knight: return "knight";
        case bishop: return "bishop";
        case rook:   return "rook";
        case queen:  return "queen";
        case king:   return "king";
        default:     return "none";
    }
}

std::string color2s(color c){
    switch(c){
        case white: return "white";
        case black: return "black";
        default:    return "none";
    }
}

std::string coord2s(square x){ return std::string(1, file2c(x%8)) + rank2c(x/8); }

std::string coord2s(U64 src){ return coord2s(bitscan(src)); }

square s2coord(std::string str){ return 8*c2rank(str[1]) + c2file(str[0]); }

std::string pgndict2s(const pgndict& pd){
    std::string res;
    for(pgndict::const_iterator it=pd.begin(); it!=pd.end(); ++it){
        res += tag2s[it->first] + ": " + it->second + '\n';
    }
    return res;
}

std::string bb2s(U64 bb, char c){
    std::string viz = bbviz;
    U64 _bb = bb;
    while(_bb){
        square x = lsbpop(_bb);
        int idx = bbvizidx(x);
        viz[idx] = c;
    }
    return viz;
}

U64 line(const square src, const square dst, bool endp){
    U64 res = lines[src][dst];
    if(endp){ res |= (1ULL << src) | (1ULL << dst); }
    return res;
}

U64 slide_atk(const square src, const ptype pt, const U64 occ){
    U64 res = 0ULL;
    for(int i=0; i<8; ++i){
        // Ray rooted @ src, directed along axis i
        U64 r = ray(src, pt, i);
        if(not r){ continue; }
        // Nearest (to src), occupied square on the ray
        square x = i>3 ? bitscanr(r & occ) : bitscan(r & occ);
        if(x == -1 || x == 64){ res |= r; continue; }
        // Squares on ray bounded by (src, x]
        res |= r ^ ray(x, pt, i);
    }
    return res;
}

U64 attack(const square src, const ptype pt, const color c){
    switch(pt){
        case pawn:
            switch(c){
                case white: return wpattack[src];
                case black: return bpattack[src];
                default:    return wpattack[src] & bpattack[src];
            }
        case knight:        return nattack[src];
        case bishop:        return battack[src];
        case rook:          return rattack[src];
        case queen:         return qattack[src];
        default:            return kattack[src];
    }
}

// Sliding attack masks (same as `attack` but exludes edge squares)
U64 attackm(square src, ptype pt){
    switch(pt){
        case bishop: return battackm[src];
        case rook:   return rattackm[src];
        default:     return battackm[src] | rattackm[src];
    }
}

// Bitboard of pieces giving check
U64 search_pins(const bitboard bb){
    /*
        A pin is found if the following are all true:
            1. There exists an attack line between candidate piece and opposing king
            2. There are no pieces with the same color as candidate on that line
            3. There is only one piece with the same color as the opposing king on that line
    */

    U64 pins = 0ULL;

    for(int i=white; i>=black; i-=2){
        color c = static_cast<color>(i);
        U64 kloc = bb(king, !c),
            same = bb(c),
            oppt = bb(!c);
        square ksq = bitscan(kloc);

        for(int j=bishop; j<=queen; ++j){
            ptype pt = static_cast<ptype>(j);
            U64 plocs = bb(pt, c);

            while(plocs){
                square x = lsbpop(plocs);
                U64 atk = kloc & attack(x, pt),
                    lbt = line(x, ksq);
                if(atk && popcnt(lbt & oppt) == 1 && !popcnt(lbt & same)){ pins |= lbt & oppt; }
            }
        }
    }
    return pins;
}

// Bitboard of pinned pieces
U64 search_checks(const bitboard bb, const color c){
    // color c = color of player EXECUTING check

    U64 checkers = 0ULL;
    const U64 kloc = bb(king, !c);

    for(int i=pawn; i<king; ++i){

        ptype pt = static_cast<ptype>(i);
        U64 plocs = bb(pt, c);

        while(plocs){

            square x = lsbpop(plocs);
            U64 atk = 0ULL;
            if(pt <= knight){ atk |= attack(x, pt, c); }
            if(pt == bishop || pt == queen){ atk |= BMagics[x].attacks[BMagics[x].index(bb())]; }
            if(pt == rook   || pt == queen){ atk |= RMagics[x].attacks[RMagics[x].index(bb())]; }

            if(atk & kloc){ checkers |= mask(x); }
        }
    }

    return checkers;
}

// RNG for magic bitboard initialization
U64 random_magic(){
    U64 res = ALL_;
    for(int i=0; i<3; ++i){ res &= dist(gen); }
    return res;
}

// Intialize magic bitboards
void magic_init(const ptype pt, U64* table, Magic* magics){

    U64 occupancy[4096], reference[4096], b;
    int epoch[4096] = {}, cnt = 0, size = 0;

    for(square sq=0; sq<64; ++sq){

        Magic& m = magics[sq];
        m.mask  = attackm(sq, pt);
        m.shift = 64 - popcnt(m.mask);

        m.attacks = !sq ? table : magics[sq - 1].attacks + size;

        b = size = 0;
        do{
            occupancy[size] = b;
            reference[size] = slide_atk(sq, pt, b);
            ++size;
            b = (b - m.mask) & m.mask;
        } while(b);

        for(int i=0; i<size; ){
            for(m.magic=0; popcnt((m.magic * m.mask) >> 56) < 6; ){ m.magic = random_magic(); }

            for (++cnt, i=0; i<size; ++i){
                int idx = m.index(occupancy[i]);

                if(epoch[idx] < cnt){
                    epoch[idx] = cnt;
                    m.attacks[idx] = reference[i];
                }
                else
                if(m.attacks[idx] != reference[i]){ break; }
            }
        }
    }
}

/**************************************************
                    BOARD CLASS
**************************************************/
namespace { ply missing; }

Board::Board(){
    if(!initialized){
        magic_init(bishop, BishopAttacks, BMagics);
        magic_init(rook, RookAttacks, RMagics);
        initialized = true;
    }
    this->reset();
}

void Board::reset(){
    this->board = newgame;
    this->pins = 0ULL;
    this->enpas = 0ULL;
    this->checkers = 0ULL;
    this->next = white;
    this->check = NOCOLOR;
    this->half = 0;
    this->full = 1;
    this->cancas = castle<white> | castle<black>;
}

void Board::clear(){
    this->remove();
    this->pins = 0ULL;
    this->enpas = 0ULL;
    this->checkers = 0ULL;
    this->next = NOCOLOR;
    this->check = NOCOLOR;
    this->half = 0;
    this->full = 0;
    this->cancas = 0b00000000;
}

color Board::lookupc(const U64 src) const{
    if     (this->board(white) & src){ return white;   }
    else if(this->board(black) & src){ return black;   }
    else                             { return NOCOLOR; }
}

ptype Board::lookupt(const U64 src) const{
    if     (this->board(pawn)   & src){ return pawn;   }
    else if(this->board(knight) & src){ return knight; }
    else if(this->board(bishop) & src){ return bishop; }
    else if(this->board(rook)   & src){ return rook;   }
    else if(this->board(queen)  & src){ return queen;  }
    else if(this->board(king)   & src){ return king;   }
    else                              { return NOTYPE; }
}

void Board::remove(const U64 src, const ptype pt, const color c){
    this->board(pt, c) &= ~src;
    this->board(c)     &= ~src;
    this->board()      &= ~src;
}

void Board::remove(const U64 src){
    for(int i=0; i<15; ++i){
        this->board[i] &= ~src;
    }
}

void Board::remove(){

    this->board.fill(0ULL);
}

void Board::place(const U64 dst, const ptype pt, const color c){
    this->board(pt, c) |= dst;
    this->board(c)     |= dst;
    this->board()      |= dst;
}

void Board::move(const U64 src, const U64 dst, const ptype pt, const color c){
    this->remove(src, pt, c);
    this->place(dst, pt, c);
}

bool Board::clearbt(const square sq1, const square sq2) const{
    if(attack(sq1, king) & (1ULL << sq2)){ return true; }
    else{
        U64 lbt = line(sq1, sq2);
        switch(lbt){
            case 0:  return false;
            default: return !(lbt & this->board());
        }
    }
}

template<color c>
void Board::apply(const ply p){

    constexpr color nc = !c;

    // Remove captured material
    if(p.capture){
        if(p.dst & this->enpas){ this->remove(push(p.dst, nc), pawn, nc); }
        else                   { this->remove(p.dst); }

        if(p.dst & this->board(rook, nc)){
             // Rook capture => update castling availability
            this->cancas &= (p.dst & rook_castle_src<nc, 1>) ? ~castle<nc, 1> : ~castle<nc, -1>;
        }
    }

    // set enpas to 0 unless p == pawn double push
    if(p.type == pawn && p.dst == double_push(p.src, c)){ this->enpas = push(p.src, c); }
    else{ this->enpas = 0; }

    // Move piece from src to dst
    this->move(p.src, p.dst, p.type, c);

    if(p.castle){
        // Move rook if castling
        (p.castle < 0) ? this->move(rook_castle_src<c, -1>, rook_castle_dst<c, -1>, rook, c)
                       : this->move(rook_castle_src<c,  1>, rook_castle_dst<c,  1>, rook, c);
        this->cancas &= ~castle<c>;
    }

    if(p.promo){
        // Promote pawn
        this->remove(p.dst, pawn, c);
        this->place(p.dst, p.promo, c);
    }

    // Update castling availability
    if(this->cancas & castle<c>){
        if(p.type == king){ this->cancas &= ~castle<c>; }
        else
        if(p.type == rook){
            if(p.src == rook_castle_src<c,  1>){ this->cancas &= ~castle<c,  1>; }
            else
            if(p.src == rook_castle_src<c, -1>){ this->cancas &= ~castle<c, -1>; }
        }
    }

    // Check for pins
    this->pins = search_pins(this->board);
    this->next = nc;

    // Update bitboard of pieces giving check
    this->checkers = p.check ? search_checks(this->board, c) : 0ULL;
    this->check = p.check ? nc : NOCOLOR;

    // Move counters
    if(c<0){ ++this->full; }
    if(p.type != pawn && !p.capture){ ++this->half;   }
    else                            { this->half = 0; }
}

void Board::update(const ply p){
    if(p == missing){ return; }
    p.c > 0 ? this->apply<white>(p) : this->apply<black>(p);
}

template<ptype pt, color c>
U64 Board::pseudo(const square sq) const{
    switch(pt){
        case king:
        case pawn:
        case knight: return attack(sq, pt, c);
        case bishop: return BMagics[sq].value(this->board());
        case rook:   return RMagics[sq].value(this->board());
        default:     return BMagics[sq].value(this->board()) | RMagics[sq].value(this->board());
    }
}

template<ptype pt, color c>
U64 Board::pseudo() const{
    U64 bb = this->board(pt, c),
        atk = 0;
    while(bb){ atk |= this->pseudo<pt, c>(lsbpop(bb)); }
    return atk;
}

template<color c>
U64 Board::pseudo() const{
    U64 atk = 0;
    for(int i=pawn; i<=king; ++i){
        ptype pt = static_cast<ptype>(i);
        atk |= pt == pawn   ? this->pseudo<  pawn, c>()
             : pt == knight ? this->pseudo<knight, c>()
             : pt == bishop ? this->pseudo<bishop, c>()
             : pt == rook   ? this->pseudo<  rook, c>()
             : pt == queen  ? this->pseudo< queen, c>()
             :                this->pseudo<  king, c>();
    }
    return atk;
}

template<color c>
U64 Board::legal_pawn(const square src, const U64 bsrc, const square kloc, const U64 occ) const{

    constexpr color nc = !c;
    constexpr U64 dpr = rank(c>0 ? 2 : 5);
    U64 res = 0;

    // Push
    res |= push(bsrc, c) & ~occ;
    // Double Push
    res |= push(res & dpr, c) & ~occ;
    // Captures
    res |= attack(src, pawn, c) & (this->board(nc) | (this->next == c ? this->enpas : 0));

    if(res & this->enpas){
        // Remove en passant discovered checks (king in check after executing ep capture)
        U64 _o = (occ ^ bsrc ^ push(this->enpas, nc)) | this->enpas;
        if(
            RMagics[kloc].value(_o) & (this->board(rook,   nc) | this->board(queen, nc)) ||
            BMagics[kloc].value(_o) & (this->board(bishop, nc) | this->board(queen, nc))
        ){ res &= ~this->enpas; }
    }
    return res;
}

template<color c>
U64 Board::legal_king(const square src, const U64 bsrc, const U64 occ) const{

    constexpr color nc = !c;

    U64 res = attack(src, king),
        atk;
    if(!(res &= ~this->board(c))){ return 0; }

    atk = this->pseudo<nc>();
    res &= ~atk;

    // Castles
    if(res && this->check != c && this->cancas & castle<c>){
        bool ks = this->cancas & castle<c,  1>,
             qs = this->cancas & castle<c, -1>;
        // Kingside - both sqaures clear and safe
        if(ks && ksl<c> == (ksl<c> & ~occ & ~atk)){ res |= bsrc << 2; }
        // Queenside - all 3 squares clear; 2 squares (traversed by king) safe
        if(qs && qslc<c> == (qslc<c> & ~occ) && qsls<c> == (qsls<c> & ~atk)){ res |= bsrc >> 2; }
    }
    return res;
}

template<color c>
U64 Board::check_filter(const ptype pt, const square kloc) const{

    U64 filter = 0,
        chkrs = this->checkers;

    int N = popcnt(chkrs);

    while(chkrs){
        square sq = lsbpop(chkrs);
        if(pt != king && N == 1){
            // Block or Capture
            filter |= line(sq, kloc, true);
            // En Passant capture
            if(pt == pawn && this->enpas == push(mask(sq), c)){ filter |= this->enpas; }
        }
        else
        if(pt == king && this->lookupt(mask(sq)) > knight){ filter |= axis(sq, kloc) & ~mask(sq); }
    }
    return filter;
}

template<color c>
U64 Board::legal_bb(const square src, const ptype pt) const{

    U64 res = 0;
    const U64 occ = this->board(), bsrc = mask(src);
    const square kloc = bitscan(this->board(king, c));

    switch(pt){

        case pawn:   res |= this->legal_pawn<c>(src, bsrc, kloc, occ); break;

        case knight: res |= attack(src, pt); break;

        case bishop: res |= BMagics[src].value(occ); break;

        case rook:   res |= RMagics[src].value(occ); break;

        case queen:  res |= (BMagics[src].value(occ) | RMagics[src].value(occ)); break;

        default:     res |= this->legal_king<c>(src, bsrc, occ);
    }

    // Pins
    if(bsrc & this->pins){ res &= axis(src, kloc); }

    // Check
    if(this->check == c){
        U64 filter = this->check_filter<c>(pt, kloc);
        res &= pt == king ? ~filter : filter;
    }

    // Squares occupied by same color material
    res &= ~this->board(c);

    return res;
}

U64 Board::legal(const color c) const{
    U64 res = 0;
    for(int j=pawn; j<=king; ++j){ res |= this->legal(static_cast<ptype>(j), c); }
    return res;
}

U64 Board::legal(const ptype pt, const color c) const{
    U64 res = 0, bb = this->board(pt, c);
    while(bb){ res |= this->legal(lsbpop(bb), pt, c); }
    return res;
}

U64 Board::legal(const square src, const ptype pt, const color c) const{
    return c>0 ? this->legal_bb<white>(src, pt)
               : this->legal_bb<black>(src, pt);
}

std::vector<ply> Board::legal_plies(const color c){

    std::vector<ply> plies;
    plies.reserve(256);
    ply p;
    p.c = c;
    const State _bstate = this->state;

    for(int i=pawn; i<=king; ++i){

        // for each piece type...
        p.type = static_cast<ptype>(i);
        U64 bb = this->board(p.type, c), atk;

        while(bb){

            // origin square
            square sq = lsbpop(bb);
            p.src = mask(sq);

            // legal move bitboard
            atk = this->legal(sq, p.type, c);

            // iterate over legal moves
            while(atk){

                // target square
                p.dst = mask(lsbpop(atk));

                // capture
                p.capture = p.dst & (!p.type ? (this->board(!c) | this->enpas) : this->board(!c));

                // castle
                if(p.type == king){ p.castle = (p.dst == p.src << 2) ? 1 : ((p.dst == p.src >> 2) ? -1 : 0); }
                else{ p.castle = 0; }

                // promotions
                bool pmo = !p.type && p.dst & back_rank(c);
                ptype start = pmo ? knight: pawn,
                      stop = pmo ? king : knight;

                for(int i=start; i<stop; ++i){
                    p.promo = static_cast<ptype>(i);

                    // check/mate
                    this->update(p);
                    this->checkers = search_checks(this->board, c);
                    if(this->checkers){
                        this->check = this->next;
                        p.check = true;
                        p.mate = !this->legal(this->next);
                    }
                    else{ p.check = p.mate = false; }
                    this->state = _bstate;
                    plies.emplace_back(p);
                }
            }
        }
    }

    return plies;
}

std::string Board::ply2san(const ply p) const{
    std::string res;
    if(p.castle){
        res = p.castle == -1 ? "O-O-O" : "O-O";
        if(p.mate) { res += '#'; }
        else
        if(p.check){ res += '+'; }
        return res;
    }

    // flags indicating need to disambiguate on rank and file, respectively
    bool dbr = false,
         dbf = p.type == pawn && p.capture;

    int srcf = bitscan(p.src)%8,
        srcr = bitscan(p.src)/8,
        dstf = bitscan(p.dst)%8,
        dstr = bitscan(p.dst)/8;

    // Other pieces of same type and color
    U64 bb = this->board(p.type, p.c) & ~p.src;

    while(bb){
        // Seek pieces which can reach dst (ie. seek need for disambiugation)
        square x = lsbpop(bb);
        if(this->legal(x, p.type, p.c) & p.dst){
            if(srcf != x%8){ dbf = true; }
            else
            if(srcr != x/8){ dbr = true; }
        }
    }

    // piece type and disambiguating characters
    if(p.type > pawn){ res += ptype2c(p.type); }
    if(dbf)          { res += file2c(srcf); }
    if(dbr)          { res += rank2c(srcr); }
    if(p.capture)    { res += 'x'; }

    // target square
    res += file2c(dstf);
    res += rank2c(dstr);

    // pawn promotion
    if(p.promo){ res += '='; res += ptype2c(p.promo); }

    // mate or check
    if(p.mate) { res += '#'; }
    else
    if(p.check){ res += '+'; }

    return res;
}

std::string Board::to_string(){
    std::string viz = bviz;
    U64 bb = this->board();

    while(bb){
        square x = lsbpop(bb);
        U64 src = 1ULL << x;
        int idx = bvizidx(x);
        viz[idx+1] = color2c(this->lookupc(src));
        viz[idx+2] = ptype2c(this->lookupt(src));
    }
    return viz;
}

template U64 Board::legal_pawn<white>(const square, const U64, const square, const U64) const;
template U64 Board::legal_pawn<black>(const square, const U64, const square, const U64) const;

template U64 Board::legal_king<white>(const square, const U64, const U64) const;
template U64 Board::legal_king<black>(const square, const U64, const U64) const;

template U64 Board::check_filter<white>(const ptype, const square) const;
template U64 Board::check_filter<black>(const ptype, const square) const;

template U64 Board::legal_bb<white>(const square, const ptype) const;
template U64 Board::legal_bb<black>(const square, const ptype) const;

template U64 Board::pseudo<white>() const;
template U64 Board::pseudo<black>() const;

template U64 Board::pseudo<  pawn, white>() const;
template U64 Board::pseudo<  pawn, black>() const;
template U64 Board::pseudo<knight, white>() const;
template U64 Board::pseudo<knight, black>() const;
template U64 Board::pseudo<bishop, white>() const;
template U64 Board::pseudo<bishop, black>() const;
template U64 Board::pseudo<  rook, white>() const;
template U64 Board::pseudo<  rook, black>() const;
template U64 Board::pseudo< queen, white>() const;
template U64 Board::pseudo< queen, black>() const;
template U64 Board::pseudo<  king, white>() const;
template U64 Board::pseudo<  king, black>() const;

template U64 Board::pseudo<  pawn, white>(const square) const;
template U64 Board::pseudo<  pawn, black>(const square) const;
template U64 Board::pseudo<knight, white>(const square) const;
template U64 Board::pseudo<knight, black>(const square) const;
template U64 Board::pseudo<bishop, white>(const square) const;
template U64 Board::pseudo<bishop, black>(const square) const;
template U64 Board::pseudo<  rook, white>(const square) const;
template U64 Board::pseudo<  rook, black>(const square) const;
template U64 Board::pseudo< queen, white>(const square) const;
template U64 Board::pseudo< queen, black>(const square) const;
template U64 Board::pseudo<  king, white>(const square) const;
template U64 Board::pseudo<  king, black>(const square) const;

template void Board::apply<white>(const ply);
template void Board::apply<black>(const ply);

/**************************************************
            DISAMBIGUATION FUNCTIONS
**************************************************/

namespace disamb{

    U64 pgn(const U64 src, const U64 dst, ptype pt, color c, const Board& board, bool capture){
        /*
            cand:   Candidate pieces (intersect with `src` if src is non-zero)
            pins:   Candidate pieces which are also pinned
            kloc:   Bitboard of same-color king
            dkln:   Line between `dst` and `kloc`, not including kloc
            occ:    Occupancy of the entire board
            res:    Result placeholder
        */
        U64 cand = board.board(pt, c) & (src ? src : ALL_),
            pins = cand & board.pins,
            kloc = board.board(king, c),
            dkln = line(bitscan(dst), bitscan(kloc), true) & ~kloc,
            occ  = board.board(),
            res;

        switch(pt){
            // Pawn movements
            case pawn: {
                while(pins){
                    // Remove immovable pins
                    square p = lsbpop(pins);
                    if(!(line(p, bitscan(kloc), true) & dkln)){ cand &= ~mask(p); }
                }
                // Find first pawn that can reach target square
                if(capture && (res = cand & attack(bitscan(dst), pt, !c))){ return res; }
                else
                if(cand & (res = push(dst, !c)))                          { return res; }
                else
                if(cand & (res = double_push(dst, !c)))                   { return res; }
                break;
            }

            // Back-row piece movements
            default: {
                while(cand){
                    square prosq = lsbpop(cand);
                    U64 pros = mask(prosq);
                    /*
                        1. Piece can move to target square
                        2. Piece has clear line of sight to target square OR does not need it (ie. is a knight)
                        3. Piece is not pinned OR `dst` is colinear with both king and prospective piece
                    */
                    if(
                        (dst & attack(prosq, pt, c)) &&
                        (pt == knight || !(occ & line(prosq, bitscan(dst)))) &&
                        (!(pros & board.pins) || line(prosq, bitscan(kloc), true) & dkln)
                    ){ return pros; }
                }
            }
        }

        // Disambiguation failed = return empty bitboard
        return 0ULL;
    }

    ply uci(std::string move, Board& board){
        // Separate src and dst
        std::string src_str = move.substr(0,2),
                    dst_str = move.substr(2);

        // Square numbers and bitboards
        square srcsq = s2coord(src_str),
               dstsq = s2coord(dst_str);
        U64 src = mask(srcsq), dst = mask(dstsq);

        // Construct ply
        ptype pt = board.lookupt(src),
              pmo = (!pt && !std::isdigit(dst_str.back())) ? fen::c2ptype(dst_str.back()) : pawn;
        int cas = (pt == king && std::abs(srcsq - dstsq) == 2) ? ((srcsq - dstsq) / 2) : 0;
        bool cap = board.board() & dst;
        ply p = {src, dst, pt, pmo, board.next, cas, cap, false, false};

        // Copy state, update, seek check/checkmate, restore state
        const State _bstate = board.state;
        board.update(p);
        board.checkers = search_checks(board.board, p.c);
        if(board.checkers){
            board.check = board.next;
            p.check = true;
            p.mate = !board.legal(board.next);
        }
        board.state = _bstate;

        return p;
    }
}

/**************************************************
            FORSYTH-EDWARDS NOTATION
**************************************************/
namespace fen{

    const std::regex delim("/");

    const std::string new_game("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    ptype c2ptype(char p){
        switch(p){
            case 'p':
            case 'P': return pawn;
            case 'n':
            case 'N': return knight;
            case 'b':
            case 'B': return bishop;
            case 'r':
            case 'R': return rook;
            case 'q':
            case 'Q': return queen;
            default:  return king;
        }
    }

    char ptype2c(ptype pt, color c){
        switch(pt){
            case pawn:      return c<0 ? 'p' : 'P';
            case knight:    return c<0 ? 'n' : 'N';
            case bishop:    return c<0 ? 'b' : 'B';
            case rook:      return c<0 ? 'r' : 'R';
            case queen:     return c<0 ? 'q' : 'Q';
            default:        return c<0 ? 'k' : 'K';
        }
    }

    // Arrange pieces on board according to FEN string
    void arrange(std::string fs, Board& board){

        int rank = 7, file =0;
        std::regex_token_iterator<std::string::iterator> boardtok(fs.begin(), fs.end(), delim, -1);

        while(boardtok != std::regex_token_iterator<std::string::iterator>()){
            std::string rs = *boardtok++;

            for(std::string::iterator it=rs.begin(); it!=rs.end(); ++it){
                if(std::isdigit(*it)){ file += (*it - '0'); }
                else{
                    color c = std::islower(*it) ? black : white;
                    ptype pt = c2ptype(*it);
                    U64 loc = mask(8*rank + file);

                    board.place(loc, pt, c);

                    ++file;
                }
            }
            // Next token => next rank, first file
            rank--; file = 0;
        }
    }

    // Configure board members to match FEN string
    void parse(std::string fs, Board& board){

        if(!fs.compare(new_game)){
            // FEN matches setup for new game - no processing needed
            board.reset();
        }
        else{
            board.clear();
            std::regex ws("\\s");
            std::regex_token_iterator<std::string::iterator> fentok(fs.begin(), fs.end(), ws, -1);

            // First field in FEN string contains board arrangment
            arrange(*fentok++, board);

            // Next player to move
            board.next = (*fentok++) == 'b' ? black : white;

            // Castling availability
            std::string cas = *fentok++;
            BYTE i = 1;
            for(const char* c=castles; c!=std::end(castles); ++c){
                if(cas.find(*c) != std::string::npos){ board.cancas |= i; }
                i <<= 1;
            }

            // En Passant
            std::string enpas = *fentok++;
            if(enpas != "-"){ board.enpas = mask(8*c2rank(enpas[0]) + c2file(enpas[1])); }
            else            { board.enpas = 0ULL; }

            // Move counters
            board.half = std::stoi(*fentok++);
            board.full = std::stoi(*fentok++);

            // Check for pins
            board.pins = search_pins(board.board);

            // Check for...check
            board.checkers = search_checks(board.board, !board.next);
            if(board.checkers){ board.check = board.next; }

        }
    }

    std::string from_board(const Board& board){
        std::string fstr;

        // First token - arrangement of material
        U64 occ = board.board();
        for(int r=7; r>=0; --r){
            int empty = 0;
            for(int f=0; f<8; ++f){
                U64 loc = mask(8*r + f);
                if(!(loc & occ)){ ++empty; }
                else{
                    if(empty){ fstr += std::to_string(empty); empty = 0; }
                    fstr += ptype2c(board.lookupt(loc), board.lookupc(loc));;
                }
            }
            if(empty){ fstr += std::to_string(empty); }
            if(r){ fstr += '/'; }
        }
        fstr += ' ';

        // Next to move
        fstr += color2c(board.next);
        fstr += ' ';

        // Castling availability
        if(!board.cancas){ fstr += '-'; }
        else{
            std::bitset<4> cancas(board.cancas);
            if(cancas.test(3)){ fstr += 'K'; }
            if(cancas.test(2)){ fstr += 'Q'; }
            if(cancas.test(1)){ fstr += 'k'; }
            if(cancas.test(0)){ fstr += 'q'; }
        }
        fstr += ' ';

        // En passant availability
        fstr += board.enpas ? coord2s(board.enpas) : std::string(1, '-');
        fstr += ' ';

        // Half-move counter
        fstr += std::to_string(board.half);
        fstr += ' ';

        // Full-move counter
        fstr += std::to_string(board.full);

        return fstr;        
    }
}
