#ifndef SCANNER_CPP
#define SCANNER_CPP

#include <stdio.h>
#include <ctype.h>

#include "scanner.hpp"
#include "../utils/exceptions.hpp"

Scanner::Scanner(std::string file_path) {
    m_file_size = 0;
    m_buffer_pos = 0;
    m_current_line = 1;
    m_state = 0;
    char curr_char;

    FILE* handle = fopen(file_path.c_str(), "r");
    while((curr_char = getc(handle)) != EOF) {                
        m_file_size += 1;
    }    
    m_input_buffer = (char* )calloc(sizeof(char), m_file_size);
    fseek(handle, 0, SEEK_SET);
    for(int i = 0; (curr_char = getc(handle)) != EOF && i < m_file_size; i += 1) {
        m_input_buffer[i] = curr_char;
    }    

    printf("bytes read: %d\n", m_file_size);
    printf("contents from file:\n%s\n", m_input_buffer);    
};

bool Scanner::is_digit(char input) {
    return isdigit(input);
};

bool Scanner::is_letter(char input) {
    return isalpha(input);
};

bool Scanner::is_space(char input) {
    return input != '\n' && isspace(input);
};

bool Scanner::is_newline(char input) {
    return input == '\n';
};

bool Scanner::is_reserved(std::string input) {
    return m_reserved_words.find(input) != m_reserved_words.end();
};

bool Scanner::is_eof() {
    return m_buffer_pos == m_file_size;
}

char Scanner::next_char() {
    return m_input_buffer[m_buffer_pos++];
};

char Scanner::regress() {
    return m_input_buffer[--m_buffer_pos];
}
Token* Scanner::next_token() {
    if (is_eof()) {
        return nullptr;
    }
    char curr_char;
    while(true) {
        curr_char = next_char();
        std::string lexem;
        switch(m_state) {
            case 0:                
                if (is_eof()) {
                    break;                    
                }
                if (is_space(curr_char)) {
                    m_state = 0;          
                    break;
                }      
                else if (is_newline(curr_char)) {
                    m_current_line++;
                    m_state = 0;
                    break;
                }
                else if (is_letter(curr_char)) {
                    
                }
            break;
        }
    }
};

#endif
