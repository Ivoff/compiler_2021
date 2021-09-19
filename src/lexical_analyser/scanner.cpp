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
        m_file_size += 1;
    }    
    input_buffer = (char* )calloc(sizeof(char), m_file_size);
    fseek(handle, 0, SEEK_SET);
    for(int i = 0; (curr_char = getc(handle)) != EOF && i < m_file_size; i += 1) {
        input_buffer[i] = curr_char;
    }    

    printf("bytes read: %d\n", m_file_size);
    printf("contents from file:\n%s\n", input_buffer);
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