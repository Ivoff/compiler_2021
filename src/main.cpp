#include <stdio.h>
#include <iostream>

#include "syntactical_analyser/parser.hpp"
#include "semantic_generator/semantic_analyser.hpp"
#include "semantic_generator/symbol_table/symbol_table.hpp"
#include "semantic_generator/code_generator/code_generator.hpp"

int main() {

    // Scanner scanner = Scanner("input");    
    // Token* cur_token;
    // while((cur_token = scanner.next_token()) != nullptr) {
    //     printf("%s\n", cur_token->to_string().c_str());
    // }    

    Parser* parser = new Parser("input");    
    try {
        SymbolTable* symbol_table = new SymbolTable();
        CodeGenerator* code_generator = new CodeGenerator();
        
        SemanticAnalyser* analyser = new SemanticAnalyser(
            parser->parse(), 
            new SemanticActionsTable(symbol_table, code_generator)
        );

        // parser->m_parse_tree->print(parser->m_parse_tree->m_root, "");
        analyser->analise();
        code_generator->print();   
    } 
    catch (std::runtime_error& ex) {
        std::cerr << "[Runtime error]" << ex.what() << std::endl;
        parser->m_parse_tree->print(parser->m_parse_tree->m_root, "");
        exit(EXIT_SUCCESS);
    }    

    return 0;
}