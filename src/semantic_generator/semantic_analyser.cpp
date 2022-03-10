#include "semantic_analyser.hpp"

SemanticAnalyser::SemanticAnalyser(ParseTree* parse_tree, SemanticActionsTable* action_table) {
    m_parse_tree = parse_tree;
    m_action_table = action_table;
    if (!m_parse_tree->m_tree_stack.empty()) {
        printf("Pilha da árvore não vazia\n");
        printf("Deve ter algo de errado na análise sintática\n");
        exit(EXIT_SUCCESS);
    }
    m_parse_tree->m_current_node = m_parse_tree->m_root;
}

void SemanticAnalyser::analise() {    
    if (m_parse_tree->m_root != nullptr) {
        std::stack<Node*> aux1;
        aux1.push(m_parse_tree->m_root);
        while(!aux1.empty()) {
            auto cur_node = aux1.top();                        
            aux1.pop();            
            if (cur_node->m_node_list->size()) {
                for (int i = cur_node->m_node_list->size()-1; i >= 0; i -= 1) {
                    aux1.push(cur_node->m_node_list->at(i));
                }
            }
            // printf("%s\n", cur_node->m_head.c_str());

            m_action_table->execute_rightmost_actions(cur_node->m_head);
            m_action_table->execute_action(cur_node->m_head, cur_node);            
        }
    }

    m_parse_tree->print_attr(m_parse_tree->m_root, "");
}