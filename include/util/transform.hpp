#ifndef UTIL_TRANS_H
#define UTIL_TRANS_H

/**************************
    DATA TRANSFORMATIONS
**************************/

namespace util{
    namespace transform{

        // Char to bool
        template<typename CharT>
        bool ispiece(CharT c);

        template<typename CharT>
        bool isfile(CharT c);

        // Change case
        template<typename CharT>
        void uppercase(CharT *start, CharT *end);

        template<typename CharT>
        void uppercase(std::basic_string<CharT>& s);

        template<typename CharT>
        void lowercase(CharT *start, CharT *end);

        template<typename CharT>
        void lowercase(std::basic_string<CharT>& s);

        // Flip least-significant bit
        void lsbflip(U64& x);

        // Coordinate conversions
        template<typename T>
        square sq(const T& src);

        template<typename T>
        U64 mask(const T& src);

        template<typename T>
        coords rf(const T& src);

        template<typename T>
        square bitscan(const T& src);

        square bitscan(const U64& src);

    }
}

#endif