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

        // Flip least-significant bit
        void lsbflip(U64&);

        // Coordinate conversions
        template<typename T>
        square sq(const T&);

        template<typename T>
        U64 mask(const T&);

        template<typename T>
        coords rf(const T&);

        template<typename T>
        square bitscan(const T&);

        template<typename T>
        square bitscanr(const T&);

        square bitscan(const U64&);

        square bitscanr(const U64&);

    }
}

#endif