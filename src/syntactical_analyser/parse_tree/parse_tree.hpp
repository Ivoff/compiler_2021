#ifndef PARSE_TREE_HPP
#define PARSE_TREE_HPP

#include <vector>
#include <string>
#include <exception>
#include <stack>
#include <map>

#include "../../lexical_analyser/token.hpp"
#include "../../misc/type.hpp"

struct Attribute {    
    EType m_type;
    int m_int{0};
    double m_real{0.0};
    std::string m_str;
    
    Attribute(){};
    
    Attribute(EType type, int int_attr) {
        m_int = int_attr;
        m_type = type;
    }

    Attribute(EType type, double real_attr) {
        m_real = real_attr;
        m_type = type;
    }

    Attribute(EType type, std::string str_attr) {
        m_str = str_attr;
        m_type = type;
        // só pra garantir, porque arrumar vai dar trabalho de mais
        m_int = 0;
        m_real = 0;
    };

    std::string to_string() {
        if (m_type == EType::INTEGER && m_int) {
            return std::to_string(m_int);
        } 
        else if (m_type == EType::REAL && m_real)   {
            return std::to_string(m_real);
        } 
        else {
            return m_str;
        }
    }

    std::string type_to_string() {
        if (m_type == EType::INTEGER) {
            return "integer";
        } 
        else if (m_type == EType::REAL) {
            return "real";
        } 
        else {
            return "string";
        }
    }
};

struct Node {        
    std::string m_head;
    bool m_checked {false};
    Node* m_parent {nullptr};
    Token* m_terminal {nullptr};
    std::map<std::string, Attribute> m_attributes;
    std::vector<Node*>* m_node_list {nullptr}; // not a list LOL
    
    Node(std::string head);    
    Node* sibling(int index);
    Node* child(int index);
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
    static void print_attr(Node* current_node, std::string level);
    void update_tree_info();
    void update_tree_info_post_order();    
};

#endif
