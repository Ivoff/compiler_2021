#ifndef SEMANTIC_ANALYSER_HPP
#define SEMANTIC_ANALYSER_HPP

#include "../syntactical_analyser/parser.hpp"
#include "semantic_actions_table/semantic_actions_table.hpp"

struct SemanticAnalyser {
    ParseTree* m_parse_tree;
    SemanticActionsTable* m_action_table;

    SemanticAnalyser(ParseTree* parse_tree, SemanticActionsTable* action_table);    
    void analise();
};

#endif