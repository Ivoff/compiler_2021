#ifndef SCANNER_CPP
#define SCANNER_CPP

#include <stdio.h>
#include <ctype.h>

#include "scanner.hpp"

Scanner::Scanner(std::string file_path) {
    m_file_size = 0;
    m_buffer_pos = 0;
    m_current_line = 0;
    m_state = 0;
    char curr_char;

    FILE* handle = fopen(file_path.c_str(), "r");
    while((curr_char = getc(handle)) != EOF) {        
        input_buffer.emplace_back(curr_char);
        m_file_size += 1;
    }

    printf("bytes read: %d\n", m_file_size);
    scan();
};

bool Scanner::is_digit(char input) {
    return isdigit(input);
};

bool Scanner::is_letter(char input) {
    return isalpha(input);
};

bool Scanner::is_space(char input) {
    return isspace(input);
};

void Scanner::scan() {

};

#endif