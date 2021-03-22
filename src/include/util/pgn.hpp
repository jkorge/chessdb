#ifndef UTIL_PGN_H
#define UTIL_PGN_H

namespace util{
    namespace pgn{
        /*
            UTILITY OBJECTS FOR PGN PARSING
        */

        // Regex for game results (1-0, 0-1, 1/2-1/2, *)
        const std::regex game_result("\\s*((1/)*[0-9]-[0-9](/2)*|\\*)\\s*");

        // Int values for each outcome
        const std::map<std::string, int> game_result_values{
            {"1-0", 1},
            {"0-1", -1},
            {"1/2-1/2", 2},
            {"*", 0}
        };

        // Regex for elided ply (".." or "--")
        const std::regex missing_ply("(-{2}|\\.{2})");

        // Delimiter for parsing moves in SAN
        const std::regex move_num_r("(\\d{1,}\\.)");

        // Regex for movetext that opens with an elided white ply (e.g. "1... Rd8")
        const std::regex black_starts("\\d{1,}\\.{3}");

        // Delimiters for parsing PGN tags
        constexpr char tag_delims[]{"[]"};
        constexpr char key_delim = ' ';
        constexpr char value_delim = '\"';

        // Regex for newline NOT preceded by dot or space
        extern std::regex nds;

        // Get pgntag from string
        extern std::map<std::string, pgntag> s2tag;

        // Iterator to end of s2tag
        extern std::map<std::string, pgntag>::iterator stend;

        // Get string from pgntag
        extern std::map<pgntag, std::string> tag2s;

        // Get string from pgndict ("key: value\n...")
        std::string pgndict2s(const pgndict&);
    }
}

#endif