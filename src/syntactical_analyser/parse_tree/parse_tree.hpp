#ifndef PARSE_TREE_HPP
#define PARSE_TREE_HPP

#include <vector>
#include <string>
#include <exception>
#include <stack>

#include "../../lexical_analyser/token.hpp"

struct Node {        
    std::string m_head;
    Node* m_parent {nullptr};
    Token* m_terminal {nullptr};
    std::vector<Node*>* m_node_list {nullptr}; // not a list LOL
    
    Node(std::string head);
    Node(std::string head, std::vector<Node*>* node_list);
    Node(std::string head, Token* terminal);    
};

struct ParseTree {
    Node* m_root {nullptr};
    Node* m_current_node {nullptr};
    std::stack<Node*> m_tree_stack;
    
    ParseTree(){};    
    void insert_node(Node* current_node, Node* new_node);
    void insert_node(Node* new_node);
    void insert_node(std::string head);
    void print(Node* current_node, std::string level);
    void update_tree_info();
    
};

#endif
