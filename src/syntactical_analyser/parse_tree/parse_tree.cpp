#include "parse_tree.hpp"

Node::Node(std::string head) {
    m_head = head;    
    m_node_list = new std::vector<Node*>();
};

/**
 * @brief index from father POV
 * 
 * @param index 
 * @return Node* 
 */
Node* Node::sibling(int index) {
    return m_parent->m_node_list->at(index);
}

/**
 * @brief To append a newly created node as a child from m_current_node
 * 
 * @param new_node 
 */
void ParseTree::insert_node(Node* new_node) {
    if (m_root == nullptr) {        
        m_root = new_node;
        new_node->m_parent = nullptr;
        m_current_node = m_root;
    } 
    else if (m_current_node != nullptr){
        m_current_node->m_node_list->emplace_back(new_node);
        new_node->m_parent = m_current_node;
    } else {
        throw std::runtime_error("Node atual null");
    }
};

/**
 * @brief To fill in an already created node.
 * 
 * @param head 
 */
void ParseTree::insert_node(std::string head) {
    if (m_root == nullptr) {        
        m_root = new Node(head);
        m_root->m_parent = nullptr;
        m_current_node = m_root;
    } 
    else if (m_current_node != nullptr){
        m_current_node->m_head = head;        
    } else {
        throw std::runtime_error("Node atual null");
    }
}

void ParseTree::print_attr(Node* current_node, std::string level) {
    if (current_node->m_terminal != nullptr) 
    {
        printf("%s%s -> %s\n", level.c_str(), current_node->m_head.c_str(), current_node->m_terminal->lexem_to_str().c_str());        
    }
    else {
        printf("%s%s\n", level.c_str(), current_node->m_head.c_str());
        if (!current_node->m_attributes.empty()) 
        {
            for(auto itr =  current_node->m_attributes.begin(); itr != current_node->m_attributes.end(); ++itr) 
            {
                printf("%s[attr] %s.%s = %s (%s)\n", level.c_str(), current_node->m_head.c_str(), itr->first.c_str(), itr->second.to_string().c_str(), itr->second.type_to_string().c_str());
            }            
        }
    }
            
    for (auto el : *current_node->m_node_list) {
        print_attr(el, level + "|- ");        
    }    
}

void ParseTree::print(Node* current_node, std::string level) {    
    if (current_node->m_terminal != nullptr) {
        printf("%s%s -> %s\n", level.c_str(), current_node->m_head.c_str(), current_node->m_terminal->lexem_to_str().c_str());        
    }
    else
        printf("%s%s\n", level.c_str(), current_node->m_head.c_str());
        
    if (m_root->m_node_list->size() > 0) {
        for (auto el : *current_node->m_node_list) {
            print(el, level + "|- ");
        }
    }
}

void ParseTree::update_tree_info() {
    if (m_current_node->m_node_list->size() > 1) {
        for (auto i = m_current_node->m_node_list->size()-1; i > 0; i -= 1) {
            m_tree_stack.push(m_current_node->m_node_list->at(i));
        }
        m_current_node = m_current_node->m_node_list->at(0);
    } else if (m_current_node->m_node_list->size()  == 1) {
        //Se tamanho da lista de nós é 1 então current node é child
        m_current_node = m_current_node->m_node_list->at(0);
    }
}

void ParseTree::update_tree_info_post_order() {
    if (m_current_node->m_node_list->size() > 1) {
        for (auto i = m_current_node->m_node_list->size()-1; i > 0; i -= 1) {
            m_tree_stack.push(m_current_node->m_node_list->at(i));
        }
        if (m_current_node->m_node_list->at(0)->m_node_list->size()) {
            m_tree_stack.push(m_current_node->m_node_list->at(0));
            m_current_node = m_current_node->m_node_list->at(0);
        } 
        else {
            m_current_node = m_current_node->m_node_list->at(0);
        }        
    } 
    else if (m_current_node->m_node_list->size() == 1) {
        //Se tamanho da lista de nós é 1 então current node é child
        m_current_node = m_current_node->m_node_list->at(0);
    }
}