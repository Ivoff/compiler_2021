#include <stdio.h>
#include <iostream>

#include "syntactical_analyser/parser.hpp"
#include "semantic_generator/semantic_analyser.hpp"
#include "semantic_generator/symbol_table/symbol_table.hpp"
#include "semantic_recursive/semantic_analyser_rec.hpp"

int main() {
    Parser* parser = new Parser("input");    
    
    try {        
        Scope* scopes = new Scope();
        CodeGenerator* code_generator = new CodeGenerator();

        RecursiveSemanticAnalyser* analyser = new RecursiveSemanticAnalyser(
            parser->parse(),
            scopes,
            code_generator
        );

        analyser->analise();
        parser->m_parse_tree->print_attr(parser->m_parse_tree->m_root, "");
        code_generator->print();        
    } 
    catch (std::runtime_error& ex) {
        std::cerr << "[Runtime error] " << ex.what() << std::endl;
        parser->m_parse_tree->print_attr(parser->m_parse_tree->m_root, "");
        std::exit(EXIT_SUCCESS);
    }    

    return 0;
}