#include "semantic_actions_table.hpp"

SemanticActions::SemanticActions(SymbolTable* sym_table, CodeGenerator* code_gen) {
    m_symbol_table = sym_table;
    m_code_gen = code_gen;
};

void SemanticActions::action_tipo_var(Node* cur_node) {    
    /**
     * <tipo_var> -> real {<tipo_var>.syn = "real"}
     * <tipo_var> -> integer {<tipo_var>.syn = "real"}
     */    
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
    if (cur_node->m_head == ":" && cur_node->m_parent->m_head == "<dc_v>") {                
        cur_node->m_parent->m_node_list->at(2)->m_attributes["inh"] = Attribute(Attribute::STRING, cur_node->m_parent->m_node_list->at(0)->m_attributes["syn"].m_str);
    }
};

void SemanticActions::action_variaveis(Node* cur_node) {
    /*
    <variaveis> -> ident {
                        addEntry(ident.syn, <variaveis>.inh);
                        addCode("ALME", "0.0", "", ident.syn);
                        <mais_var>.inh = <variaveis>.inh
                     }
                <mais_var>     
     */    
    if (cur_node->m_head == "<mais_var>" && cur_node->m_parent->m_head == "<variaveis>") {                
        std::string ident_syn = cur_node->m_parent->m_node_list->at(0)->m_terminal->lexem_to_str();        
        std::string variaveis_inh = cur_node->m_parent->m_attributes["inh"].m_str;                        
        
        m_symbol_table->add_entry(ident_syn, variaveis_inh);
        if (variaveis_inh == "real")
            m_code_gen->add_code("ALME", "0.0", "", ident_syn);
        else
            m_code_gen->add_code("ALME", "0", "", ident_syn);

        cur_node->m_attributes["inh"] = Attribute(Attribute::STRING, variaveis_inh);
    }
};

void SemanticActions::action_mais_var(Node* cur_node) {
    /*
    <mais_var> -> , {<variaveis>.inh = <mais_var>.inh} <variaveis>
    */    
    if (cur_node->m_head == "," && cur_node->m_parent->m_head == "<mais_var>") {                
        cur_node->m_parent->m_node_list->at(1)->m_attributes["inh"] = Attribute(Attribute::STRING, cur_node->m_parent->m_attributes["inh"].m_str);
    }
};

void SemanticActions::action_comando(Node* cur_node) {    
    std::string op = cur_node->m_parent->m_node_list->at(0)->m_head;
    if (cur_node->m_head == ")" && (op == "read" || op == "write")) {        
        std::string ident_syn = cur_node->m_parent->m_node_list->at(2)->m_terminal->lexem_to_str();        
        m_code_gen->add_code(op, "", "", ident_syn);
    }
    // TODO: fazer assignement
};

void SemanticActions::action_fator(Node* cur_node) {    
    if (cur_node->m_parent->m_head == "<fator>") {        
        Node* fator_node = cur_node->m_parent;
        if (cur_node->m_head == "ident") {
            fator_node->m_attributes["syn"] = Attribute(Attribute::STRING, cur_node->m_terminal->lexem_to_str());            
        }
        else if (cur_node->m_head == "numero_int") {
            fator_node->m_attributes["syn"] = Attribute(Attribute::STRING, cur_node->m_terminal->lexem_to_str());
        }
        else if (cur_node->m_head == "numero_real") {
            fator_node->m_attributes["syn"] = Attribute(Attribute::STRING, cur_node->m_terminal->lexem_to_str());
        }
        else if(cur_node->m_head == ")") {
            //TODO: fazer <expressao>
        }
    }
};

void SemanticActions::action_termo_POST_op_un(Node* cur_node) {
    /*
    <termo> -> <op_un> { <fator>.inh = <op_un>.val }
    */
    if (cur_node->m_head == "<fator>" && cur_node->m_parent->m_head == "<termo>") {        
        int aux = cur_node->m_parent->m_node_list->at(0)->m_node_list->at(0)->m_head == "-" ? -1 : 1;
        cur_node->m_attributes["inh"] = Attribute(Attribute::INTEGER, aux);
    }
};

void SemanticActions::action_termo_POST_fator(Node* cur_node) {    
    if (cur_node->m_head == "<mais_fatores>" && cur_node->m_parent->m_head == "<termo>") {        
        Node* fator_node = cur_node->sibling(1);
        
        cur_node->m_attributes["inh"] = Attribute(
            Attribute::STRING, 
            fator_node->m_attributes["inh"].m_int > 0 ? 
                fator_node->m_attributes["syn"].to_string() : 
                "(-"+fator_node->m_attributes["syn"].to_string()+")"
        );
         
        m_rightmost_actions.push(
            std::make_pair(
                std::make_pair(
                    &SemanticActions::action_termo_POST_mais_fatores, 
                    cur_node->m_parent
                ), 
                "<outros_termos>"
            )
        );
    }
};

void SemanticActions::action_op_mul(Node* cur_node) {    
    if ((cur_node->m_head == "*" || cur_node->m_head == "/") && cur_node->m_parent->m_head == "<op_mul>") {        
        cur_node->m_parent->m_attributes["lexval"] = Attribute(Attribute::STRING, cur_node->m_terminal->lexem_to_str());
    }
};

void SemanticActions::action_mais_fatores_POST_fator(Node* cur_node) {    
    if (cur_node->m_head == "<mais_fatores>" && cur_node->m_parent->m_head == "<mais_fatores>") {        
        Node* fator_node = cur_node->sibling(1);        
        
        cur_node->m_attributes["inh"] = Attribute(Attribute::STRING,fator_node->m_attributes["syn"].to_string());

        m_rightmost_actions.push(
            std::make_pair(
                std::make_pair(
                    &SemanticActions::action_mais_fatores, 
                    cur_node
                ), 
                "<outros_termos>"
            )
        );
    } 
    
    if (cur_node->m_head == "<mais_fatores>" && cur_node->m_node_list->at(0)->m_head == "&") {
        cur_node->m_attributes["syn"] = cur_node->m_attributes["inh"];
    }
};

void SemanticActions::action_mais_fatores(Node* cur_node) {        
    if (cur_node->m_head == "<mais_fatores>" && cur_node->m_parent->m_head == "<mais_fatores>") {
        Node* mais_fatores_parent = cur_node->m_parent;
        Node* mais_fatores_child = cur_node;        
        
        /* <mais_fatores> -> <op_mul><fator><mais_fatores> {
                <mais_fatores>.syn = geraTemp();
                code(<mais_fatores>.op, <mais_fatores>.inh, <mais_fatores₁>.syn, <mais_fatores>.syn);
        }*/
        mais_fatores_parent->m_attributes["syn"] = Attribute(Attribute::STRING, m_code_gen->gen_temp());
        std::string op = mais_fatores_parent->m_node_list->at(0)->m_attributes["lexval"].m_str;        

        m_code_gen->add_code(
            op,
            mais_fatores_parent->m_attributes["inh"].to_string(),
            mais_fatores_child->m_attributes["syn"].to_string(), 
            mais_fatores_parent->m_attributes["syn"].m_str
        );        
    }    
};

void SemanticActions::action_termo_POST_mais_fatores(Node* cur_node) {        
    if (cur_node->m_head == "<termo>") {        
        cur_node->m_attributes["syn"] = cur_node->m_node_list->at(2)->m_attributes["syn"];
    }
};

void SemanticActions::action_op_ad(Node* cur_node) {    
    if ((cur_node->m_head == "+" || cur_node->m_head == "-") && cur_node->m_parent->m_head == "<op_ad>") {        
        cur_node->m_parent->m_attributes["lexval"] = Attribute(Attribute::STRING, cur_node->m_terminal->lexem_to_str());
    }
};

void SemanticActions::action_expressao_POST_termo(Node* cur_node) {
    if (cur_node->m_head == "<outros_termos>" && cur_node->m_parent->m_head == "<expressao>") {
        cur_node->m_attributes["inh"] = cur_node->sibling(0)->m_attributes["syn"];
    }
};

void SemanticActions::action_outros_termos_POST_op_ad(Node* cur_node) {
    if (cur_node->m_head == "<termo>" && cur_node->m_parent->m_head == "<outros_termos>") {
        auto op_ad = cur_node->sibling(0);
        auto outros_termos = cur_node->m_parent;

        outros_termos->m_attributes["op"] = op_ad->m_attributes["lexval"];

        ParseTree::print_attr(outros_termos, "");
        std::exit(0);
    }
}

void SemanticActions::action_outros_termos_POST_termo(Node* cur_node) {    
    if (cur_node->m_head == "<outros_termos>" && cur_node->m_parent->m_head == "<outros_termos>") {        
        cur_node->m_attributes["inh"] = Attribute(Attribute::STRING, m_code_gen->gen_temp());
        m_code_gen->add_code(
            cur_node->m_parent->m_node_list->at(0)->m_attributes["lexval"].m_str,
            cur_node->m_parent->m_attributes["inh"].to_string(),
            cur_node->m_parent->m_node_list->at(1)->m_attributes["syn"].to_string(),
            cur_node->m_attributes["inh"].to_string()
        );

    }
};

void SemanticActions::action_expressao_POST_outros_termos(Node* cur_node) {    
    if(cur_node->m_head == "<expressao>") {        
        cur_node->m_attributes["syn"] = cur_node->m_node_list->at(1)->m_attributes["syn"];
    }
}

void SemanticActions::action_outros_termos(Node* cur_node) {    
    if (cur_node->m_head == "&" && cur_node->m_parent->m_head == "<outros_termos>") {        
        Node* outros_termos_parent = cur_node->m_parent->m_parent;
        Node* outros_termos_child = cur_node->m_parent;
                        
        if (outros_termos_parent->m_head != "<outros_termos>") {
            return;
        }
    }
}

SemanticActionsTable::SemanticActionsTable(SymbolTable* sym_table, CodeGenerator* code_gen) {
    m_symbol_table = sym_table;
    m_actions = new SemanticActions(sym_table, code_gen);
    m_code_gen = code_gen;
};

void SemanticActionsTable::execute_action(std::string head, Node* cur_node) {
    if (m_actions_table.count(head)) {        
        auto result = m_actions_table.equal_range(head);
        for(auto it = result.first; it != result.second; it ++) {
            (m_actions->*(it->second))(cur_node);
        }        
    } 
    else {
        // printf("ação gramatical não encontrada\n");
    }
};

void SemanticActionsTable::execute_rightmost_actions(std::string cur_head) {
    while(!m_actions->m_rightmost_actions.empty()) {
        auto top = m_actions->m_rightmost_actions.top();        
        if (top.second == cur_head) {
            m_actions->m_rightmost_actions.pop();
            (m_actions->*(top.first.first))(top.first.second);                        
        } else {
            break;
        }
        // std::pair<void(SemanticActions::*)(Node*), Node*> fun_node =  m_actions->m_rightmost_actions.front();
        // (m_actions->*(fun_node.first))(fun_node.second);
        // m_actions->m_rightmost_actions.pop();
    }
};