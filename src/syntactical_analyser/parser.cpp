#include "parser.hpp"
#include <stdio.h>

Parser::Parser(std::string file_path) {
    m_file_path = file_path;
    
    m_scanner = new Scanner(m_file_path);    
    m_parse_tree = new ParseTree();    
    
    m_parse_table = ParseTable();

    m_cur_token = nullptr;

    m_parse_stack.push("EOF");
    m_parse_stack.push("<programa>");
}

bool Parser::is_terminal(std::string input) {    
    return !(input[0] == '<' && input[input.length()-1] == '>' && input.length() > 2);
}

std::string Parser::sintax_error(std::string expected) {

    std::string got_lexem;
    if (m_cur_token->m_id == ETokenId::NUMBER_INT) {
        got_lexem = std::to_string(m_cur_token->m_int_attr);
    } 
    else if (m_cur_token->m_id == ETokenId::NUMBER_REAL) {
        got_lexem = std::to_string(m_cur_token->m_real_attr);
    }
    else {
        got_lexem = m_cur_token->m_str_attr;
    }

    std::string output = "\nErro de sintaxe: \n\tEsperado \"" + expected + "\", reconhecido \"" + m_cur_token->m_parse_key + "\"";    
    std::string detail = "\n\tLexema reconhecido: \""+ got_lexem + "\"\n\n";
    
    std::string parse_stack = "Parse stack:\n";
    // std::string tree_stack;
    
    while(!m_parse_stack.empty()) {
        parse_stack += "["+m_parse_stack.top()+"]";
        parse_stack += "\n";
        m_parse_stack.pop();
    }
    parse_stack += "\n";

    // while(!m_parse_tree->m_tree_stack.empty()) {
    //     tree_stack += "["+m_parse_tree->m_tree_stack.top()->m_head+"] ";
    //     if (m_parse_tree->m_tree_stack.top()->m_terminal != nullptr) {
    //         tree_stack += "["+m_parse_tree->m_tree_stack.top()->m_terminal->lexem_to_str()+"]";
    //     }
    //     tree_stack += "\n";
    //     m_parse_tree->m_tree_stack.pop();
    // }
    // tree_stack += "\n";
    
    return output + detail + parse_stack;
}

ParseTree* Parser::parse() {    
    m_cur_token = m_scanner->next_token();    
    while(m_parse_stack.top() != "EOF") {        
        std::string top = m_parse_stack.top();
        if (is_terminal(top)) {
            if (top == m_cur_token->m_parse_key) {
                m_parse_stack.pop();
                m_parse_tree->m_current_node->m_head = m_cur_token->m_parse_key;
                m_parse_tree->m_current_node->m_terminal = m_cur_token;

                m_cur_token = m_scanner->next_token();
                
                // Ao chegar a um terminal retoma a caminhada da árvore a partir
                // do último nó ignorado                
                if (!m_parse_tree->m_tree_stack.empty()) {
                    m_parse_tree->m_current_node = m_parse_tree->m_tree_stack.top();
                    m_parse_tree->m_tree_stack.pop();
                }                    
            }
            else if (top == "&") {
                m_parse_stack.pop();
                m_parse_tree->m_current_node->m_head = "&";
                m_parse_tree->m_current_node->m_terminal = nullptr;

                if (!m_parse_tree->m_tree_stack.empty()) {            
                    m_parse_tree->m_current_node = m_parse_tree->m_tree_stack.top();                
                    m_parse_tree->m_tree_stack.pop();
                }
            }
            else
                throw std::runtime_error(sintax_error(top));
        }
        else {
            std::vector<std::string> prod_rul = m_parse_table.m_table[top][m_cur_token->m_parse_key];
            if (prod_rul.size()) {
                m_parse_stack.pop();
                m_parse_tree->insert_node(top);
                for (int i = prod_rul.size()-1; i >= 0; i -= 1) {
                    m_parse_stack.push(prod_rul[i]);
                    m_parse_tree->insert_node(new Node(""));
                }
                m_parse_tree->update_tree_info();
            }
            else
            {                
                throw std::runtime_error(sintax_error(top));
            }                
        }        
    }

    return m_parse_tree;
}