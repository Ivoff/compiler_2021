#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <string>
#include <unordered_set>
#include <list>

#include "token.hpp"

struct Scanner {
    char* m_input_buffer;
    char m_current_char;
    int m_buffer_pos;
    int m_current_line;    
    int m_state;
    size_t m_file_size;        
    std::unordered_set<std::string> m_reserved_words {
        "program",
        "begin",
        "end",
        "real",
        "integer",
        "read",
        "write"
    };

    Scanner(std::string file_path);        
    Token* next_token();

    private:
        bool is_digit(char input);
        bool is_letter(char input);
        bool is_space(char input);
        bool is_newline(char input);
        bool is_eof();        
        bool is_reserved(std::string input);
        char next_char();
        char regress();
        void add_reserved_words(std::list<std::string>);
};

#endif