#ifndef SEMANTIC_ACTIONS_TABLE_HPP
#define SEMANTIC_ACTIONS_TABLE_HPP

#include <map>
#include <string>
#include <exception>

#include "../../syntactical_analyser/parse_tree/parse_tree.hpp"
#include "../symbol_table/symbol_table.hpp"

struct SemanticActions {
    SymbolTable* m_symbol_table;

    void action_tipo_var(Node* cur_node);
    void action_dc_v(Node* cur_node);
    void action_variaveis(Node* cur_node);
    void action_mais_var(Node* cur_node);

    SemanticActions(SymbolTable* sym_table);
};

typedef void(SemanticActions::*action_function)(Node*);

struct SemanticActionsTable {    
    SymbolTable* m_symbol_table;
    SemanticActions* m_actions;
    std::map<std::string, action_function> m_actions_table {
        {"real", &SemanticActions::action_tipo_var},
        {"integer", &SemanticActions::action_tipo_var},
        {":", &SemanticActions::action_dc_v},
        {"<mais_var>", &SemanticActions::action_variaveis},
        {",", &SemanticActions::action_mais_var}
    };

    SemanticActionsTable(SymbolTable* symbol_table);
    void execute_action(std::string head, Node* cur_node);        
};

#endif