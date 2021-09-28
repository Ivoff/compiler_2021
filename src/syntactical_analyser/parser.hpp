#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <stack>
#include <stdio.h>

#include "../lexical_analyser/scanner.hpp"
#include "parse_table/parse_table.hpp"
#include "parse_tree/parse_tree.hpp"

struct Parser {            
    std::string m_file_path;
    Scanner* m_scanner;
    ParseTable m_parse_table;
    ParseTree* m_parse_tree;    
    Token* m_cur_token;
    std::stack<std::string> m_parse_stack;
    
    Parser(std::string file_path);
    void parse();
    
    private:
        std::string sintax_error(std::string expected);
        bool is_terminal(std::string input);
};

#endif
