#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <string>
#include <unordered_set>
#include <list>
#include <map>

#include "token.hpp"

struct Scanner {
    char* m_input_buffer;
    char m_current_char;
    int m_buffer_pos;
    int m_current_line;    
    int m_state;
    size_t m_file_size;   
         
    std::unordered_set<std::string> m_reserved_set {
        "program",
        "begin",
        "end",
        "real",
        "integer",
        "procedure",
        "read",
        "write"
    };

    std::unordered_set<char> m_symbols_set {
        ',',
        ';',
        '$',
        '.',
        ':',
        '(',
        ')',
    };

    std::map<std::string, ETokenId> m_op_map {
        {"=", ETokenId::EQUAL_OP},
        {"<>", ETokenId::DIFF_OP},
        {"<", ETokenId::LESS_OP},
        {">", ETokenId::GREATER_OP},
        {"<=", ETokenId::LESS_EQUAL_OP},
        {">=", ETokenId::GREATER_EQUAL_OP},
        {":=", ETokenId::ASSIGNMENT_OP},
        {"+", ETokenId::ADD_OP},
        {"-", ETokenId::SUB_OP},
        {"*", ETokenId::MULT_OP},
        {"/", ETokenId::DIV_OP}
    };

    std::map<std::string, ETokenId> m_cond_map {
        {"if", ETokenId::IF},
        {"else", ETokenId::ELSE},
        {"then", ETokenId::THEN},
        {"while", ETokenId::WHILE},
        {"do", ETokenId::DO}
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
        bool is_symbol(char input);
        bool is_symbol(std::string input);
        bool is_cond(std::string input);
        bool is_op(std::string input);
        bool is_op(char input);
        char next_char();
        char regress();
        double str_to_real(std::string input);
        int str_to_int(std::string input);
        void split_sci_not(const std::string& input, int e_pos, double& coef, int& exp);
        std::string error_message(std::string lexem);
        void add_reserved_words(std::list<std::string>);        
};

#endif