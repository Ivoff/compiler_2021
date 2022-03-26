#include <stdio.h>
#include <iostream>

#include "syntactical_analyser/parser.hpp"
#include "semantic_generator/semantic_analyser.hpp"
#include "semantic_generator/symbol_table/symbol_table.hpp"
#include "semantic_recursive/semantic_analyser_rec.hpp"

int main() {
    Parser* parser = new Parser("input");    
    Scope* scopes = new Scope();    

    RecursiveSemanticAnalyser* analyser = new RecursiveSemanticAnalyser(
        parser->parse(),
        scopes            
    );

    try 
    {
        analyser->analise();        
        analyser->print_procedures();
    } 
    catch (std::runtime_error& ex) 
    {
        std::cerr << "[Compilation error] " << ex.what() << std::endl;
        parser->m_parse_tree->print_attr(parser->m_parse_tree->m_root, "");
        scopes->print();
        analyser->print_procedures();
        std::exit(EXIT_SUCCESS);
    }    

    return 0;
}