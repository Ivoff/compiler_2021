#ifndef SEMANTIC_ACTIONS_TABLE_HPP
#define SEMANTIC_ACTIONS_TABLE_HPP

#include <map>
#include <string>
#include <exception>
#include <queue>

#include "../../syntactical_analyser/parse_tree/parse_tree.hpp"
#include "../symbol_table/symbol_table.hpp"
#include "../code_generator/code_generator.hpp"

struct SemanticActions {
    typedef void(SemanticActions::*m_action_function)(Node*);

    SymbolTable* m_symbol_table;
    CodeGenerator* m_code_gen;
    std::queue<
        std::pair<
            void(SemanticActions::*)(Node*), 
            Node*
        >
    > m_rightmost_actions;

    void action_tipo_var(Node* cur_node);
    void action_dc_v(Node* cur_node);
    void action_variaveis(Node* cur_node);
    void action_mais_var(Node* cur_node);
    void action_comando(Node* cur_node);
    void action_fator(Node* cur_node);
    // void action_op_un(Node* cur_node);
    void action_termo_POST_op_un(Node* cur_node);
    void action_termo_POST_fator(Node* cur_node);
    void action_termo_POST_mais_fatores(Node* cur_node);
    void action_op_mul(Node* cur_node);
    void action_mais_fatores_POST_fator(Node* cur_node);
    void action_mais_fatores(Node* cur_node);
    void action_op_ad(Node* cur_node);
    void action_outros_termos_POST_termo(Node* cur_node);
    void action_expressao_POST_termo(Node* cur_node);
    void action_expressao_POST_outros_termos(Node* cur_node);
    void action_outros_termos(Node* cur_node);

    SemanticActions(SymbolTable* sym_table, CodeGenerator* code_gen);
};

typedef void(SemanticActions::*action_function)(Node*);

struct SemanticActionsTable {    
    SymbolTable* m_symbol_table;
    SemanticActions* m_actions;
    CodeGenerator* m_code_gen;

    std::multimap<std::string, action_function> m_actions_table {
        {"real", &SemanticActions::action_tipo_var},
        {"integer", &SemanticActions::action_tipo_var},
        {":", &SemanticActions::action_dc_v},
        {"<mais_var>", &SemanticActions::action_variaveis},
        {",", &SemanticActions::action_mais_var},
        {")", &SemanticActions::action_comando},
        {"ident", &SemanticActions::action_fator},
        {"numero_int", &SemanticActions::action_fator},
        {"numero_real", &SemanticActions::action_fator},        
        // {")", &SemanticActions::action_fator},
        // {"-", &SemanticActions::action_op_un},
        // {"&", &SemanticActions::action_op_un},
        {"<fator>", &SemanticActions::action_termo_POST_op_un},
        {"<mais_fatores>", &SemanticActions::action_termo_POST_fator},
        {"*", &SemanticActions::action_op_mul},
        {"/", &SemanticActions::action_op_mul},
        {"<mais_fatores>", &SemanticActions::action_mais_fatores_POST_fator},
        {"&", &SemanticActions::action_mais_fatores},
        {"+", &SemanticActions::action_op_ad},        
        {"<outros_termos>", &SemanticActions::action_expressao_POST_termo},
        {"<outros_termos>", &SemanticActions::action_outros_termos_POST_termo},
        {"&", &SemanticActions::action_outros_termos}
    };    

    SemanticActionsTable(SymbolTable* symbol_table, CodeGenerator* code_gen);
    void execute_action(std::string head, Node* cur_node);
    void execute_rightmost_actions();
};

#endif