#ifndef SEMANTIC_ANALYSER_REC_HPP
#define SEMANTIC_ANALYSER_REC_HPP

#include "../syntactical_analyser/parser.hpp"
#include "../semantic_generator/symbol_table/symbol_table.hpp"
#include "../semantic_generator/code_generator/code_generator.hpp"

struct RecursiveSemanticAnalyser {
    ParseTree* m_parse_tree;
    CodeGenerator* m_code_generator;
    SymbolTable* m_symbol_table;
    Scope* m_scopes;

    RecursiveSemanticAnalyser(ParseTree* parse_tree, Scope* scopes, CodeGenerator* code_generator);
    void analise();
    void programa(Node* cur_node);
    void corpo(Node* cur_node);
    void dc(Node* cur_node);
    void dc_v(Node* cur_node);    
    void tipo_var(Node* cur_node);
    void variaveis(Node* cur_node);
    void mais_dc(Node* cur_node);
    void comandos(Node* cur_node);
    void mais_var(Node* cur_node);
    void comando(Node* cur_node);
    void mais_comandos(Node* cur_node);
    void expressao(Node* cur_node);
    void termo(Node* cur_node);
    void outros_termos(Node* cur_node);
    void op_ad(Node* cur_node);
    void op_un(Node* cur_node);
    void fator(Node* cur_node);
    void mais_fatores(Node* cur_node);
    void op_mul(Node* cur_node);
    void condicao(Node* cur_node);
    void pfalsa(Node* cur_node);
    void relacao(Node* cur_node);
    void dc_p(Node* cur_node);
    void parametros(Node* cur_node);
    void lista_par(Node* cur_node);
    void mais_par(Node* cur_node);
    void corpo_p(Node* cur_node);
    void dc_loc(Node* cur_node);
    void mais_dcloc(Node* cur_node);
    void restoIdent(Node* cur_node);
    void lista_arg(Node* cur_node);
    void argumentos(Node* cur_node);
    void mais_ident(Node* cur_node);
};

#endif