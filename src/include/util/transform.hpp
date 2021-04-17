#ifndef UTIL_TRANS_H
#define UTIL_TRANS_H

/**************************
    DATA TRANSFORMATIONS
**************************/

namespace util{
    namespace transform{

        // Char to bool
        template<typename CharT>
        bool ispiece(CharT);

        template<typename CharT>
        bool isfile(CharT);

        // Change case
        template<typename CharT>
        void uppercase(CharT*, CharT*);

        template<typename CharT>
        void uppercase(std::basic_string<CharT>&);

        template<typename CharT>
        void lowercase(CharT*, CharT*);

        template<typename CharT>
        void lowercase(std::basic_string<CharT>&);

        // Coordinate conversions
        template<typename T>
        square sq(const T);

        template<typename T>
        U64 mask(const T);

        template<typename T>
        coords rf(const T);

        // Index of least significant 1-bit (aka square number)
        inline square bitscan(const U64 src){ return _tzcnt_u64(src); }

        // Index of most significant 1-bit
        inline square bitscanr(const U64 src){ return 63 - _lzcnt_u64(src); }

        // Flip least-significant bit
        inline void lsbflip(U64& x){ x &= x-1; }

    }
}

#endif