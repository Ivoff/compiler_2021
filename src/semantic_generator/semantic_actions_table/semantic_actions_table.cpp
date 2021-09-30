#include "semantic_actions_table.hpp"

SemanticActions::SemanticActions(SymbolTable* sym_table) {
    m_symbol_table = sym_table;
};

void SemanticActions::action_tipo_var(Node* cur_node) {    
    /**
     * <tipo_var> -> real {<tipo_var>.syn = "real"}
     * <tipo_var> -> integer {<tipo_var>.syn = "real"}
     */
    printf("action_tipo_var\n");    
    if ((cur_node->m_head == "real" || cur_node->m_head == "integer") && cur_node->m_parent->m_head == "<tipo_var>") {
        cur_node->m_parent->m_attributes["syn"] = Attribute(Attribute::STRING, cur_node->m_terminal->lexem_to_str());
    }
};

void SemanticActions::action_dc_v(Node* cur_node) {
    /*
    <dc_v> ->  <tipo_var> {<variaveis>.inh = <tipo_var>.syn}
            : 
            <variaveis>
    */
    printf("action_dc_v\n");
    if (cur_node->m_head == ":" && cur_node->m_parent->m_head == "<dc_v>") {
        printf("%s\n", cur_node->m_parent->m_node_list->at(0)->m_attributes["syn"].m_str.c_str());
        printf("%s\n", cur_node->m_parent->m_node_list->at(2)->m_node_list->at(1)->m_head.c_str());
        cur_node->m_parent->m_node_list->at(2)->m_attributes["inh"] = Attribute(Attribute::STRING, cur_node->m_parent->m_node_list->at(0)->m_attributes["syn"].m_str);
    }
}

void SemanticActions::action_variaveis(Node* cur_node) {
    /*
    <variaveis> -> ident {
                        addEntry(ident.syn, <variaveis>.inh);
                        <mais_var>.inh = <variaveis>.inh
                     }
               <mais_var>     
     */
    printf("action_variaveis\n");
    if (cur_node->m_head == "<mais_var>" && cur_node->m_parent->m_head == "<variaveis>") {        
        std::string ident_syn = cur_node->m_parent->m_node_list->at(0)->m_terminal->lexem_to_str();        
        std::string variaveis_inh = cur_node->m_parent->m_attributes["inh"].m_str;
        printf("%s\n", cur_node->m_parent->m_parent->m_attributes["inh"].m_str.c_str());

        printf("add_entry(%s, %s)\n", ident_syn.c_str(), variaveis_inh.c_str());
        
        m_symbol_table->add_entry(ident_syn, variaveis_inh);        
        cur_node->m_attributes["inh"] = Attribute(Attribute::STRING, variaveis_inh);
    }
}

void SemanticActions::action_mais_var(Node* cur_node) {
    printf("action_mais_var\n");
    if (cur_node->m_head == "," && cur_node->m_parent->m_head == "<mais_var>") {        
        cur_node->m_parent->m_node_list->at(1)->m_attributes["inh"] = Attribute(Attribute::STRING, cur_node->m_parent->m_attributes["inh"].m_str);
    }
}

SemanticActionsTable::SemanticActionsTable(SymbolTable* sym_table) {
    m_symbol_table = sym_table;
    m_actions = new SemanticActions(sym_table);
}

void SemanticActionsTable::execute_action(std::string head, Node* cur_node) {
    if (m_actions_table.count(head)) {                
        (m_actions->*(m_actions_table.find(head)->second))(cur_node);
    } 
    else {
        // printf("ação gramatical não encontrada\n");
    }
}