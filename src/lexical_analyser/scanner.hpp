#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <string>

#include "token.hpp"

struct Scanner {
    char* input_buffer;
    int m_buffer_pos;
    int m_current_line;    
    int m_state;
    size_t m_file_size;

    Scanner(std::string file_path);        
    Token next_token();

    private:
        bool is_digit(char input);
        bool is_letter(char input);
        bool is_space(char input);    
        void scan();
};

#endif