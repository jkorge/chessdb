#ifndef UTIL_REPR_H
#define UTIL_REPR_H

/**************************
    DATA REPRESENTATIONS
**************************/

namespace util{
    namespace repr{

        // Get file from char
        int c2file(char);

        // Get char from file
        char file2c(int);
        
        // Get rank from char
        int c2rank(char);

        // Get char from rank
        char rank2c(int);

        // Get ptype from char
        ptype c2ptype(char);

        // Get char from ptype
        char ptype2c(ptype);

        // Get string from ptype
        std::string ptype2s(ptype);

        // Get string from pname
        std::string pname2s(pname);

        // Get string from color
        std::string color2s(color);

        // Get char from color
        char color2c(color);

        // Get string from coordinates; Returns string "(r,f)" where r = rank and f = file
        std::string coord2s_(const coords&);

        template<typename T>
        std::string coord2s(const T&);
    }
}

#endif