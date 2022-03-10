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
        
        std::string lexem = cur_node->m_terminal->lexem_to_str();
        Node* fator_node = cur_node->m_parent;

        if (cur_node->m_head == "ident") 
        {
            if (m_symbol_table->m_table.count(lexem)) 
            {
                auto attr_type = m_symbol_table->m_table[lexem].first.m_type == Symbol::INTEGER ? Attribute::INTEGER : Attribute::REAL;
                fator_node->m_attributes["syn"] = Attribute(attr_type, lexem);
            } 
            else 
            {
                // erro
                printf("variável %s não existe\n", lexem.c_str());
                std::exit(0);
            }                        
        }
        else if (cur_node->m_head == "numero_int") 
        {
            fator_node->m_attributes["syn"] = Attribute(Attribute::INTEGER, cur_node->m_terminal->lexem_to_str());
        }
        else if (cur_node->m_head == "numero_real") 
        {
            fator_node->m_attributes["syn"] = Attribute(Attribute::REAL, cur_node->m_terminal->lexem_to_str());
        }
        else if(cur_node->m_head == ")") 
        {
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
            fator_node->m_attributes["syn"].m_type, 
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
    if (cur_node->m_head == "<mais_fatores>" && cur_node->m_parent->m_head == "<mais_fatores>") 
    {
        Node* fator_node = cur_node->sibling(1);        

        cur_node->m_attributes["inh"] = Attribute(fator_node->m_attributes["syn"].m_type, fator_node->m_attributes["syn"].to_string());

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
    
    if (cur_node->m_head == "<mais_fatores>" && cur_node->m_node_list->at(0)->m_head == "&") 
    {
        cur_node->m_attributes["syn"] = cur_node->m_attributes["inh"];
    }
};

void SemanticActions::action_mais_fatores(Node* cur_node) {        
    if (cur_node->m_head == "<mais_fatores>" && cur_node->m_parent->m_head == "<mais_fatores>") 
    {
        Node* mais_fatores_parent = cur_node->m_parent;
        Node* mais_fatores_child = cur_node;        

        auto left_operand = mais_fatores_parent->m_attributes["inh"]; // atributo herdado de <fator> pelo não terminal <mais_fatores> imediatamente acima
        auto right_operand = mais_fatores_child->m_attributes["syn"]; // atributo herdado da subárvore imediatamente abaixo de <mais_fatores>, podendo ter uma variável temporária ou um lexema

        // atributo da variável temporária
        Attribute::EAttrTipo temp_attr;

        if (left_operand.m_type != right_operand.m_type)
        {
            temp_attr = Attribute::REAL;
        } 
        else {
            temp_attr = left_operand.m_type;
        }

        mais_fatores_parent->m_attributes["syn"] = Attribute(temp_attr, m_code_gen->gen_temp());
        std::string op = mais_fatores_parent->m_node_list->at(0)->m_attributes["lexval"].to_string();

        m_code_gen->add_code(
            op,
            left_operand.to_string(),
            right_operand.to_string(), 
            mais_fatores_parent->m_attributes["syn"].to_string()
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

        m_rightmost_actions.push(
            std::make_pair(
                std::make_pair(
                    &SemanticActions::action_expressao_POST_outros_termos, 
                    cur_node->m_parent
                ), 
                "<mais_comandos>"
            )
        );
    }
};

void SemanticActions::action_outros_termos_POST_op_ad(Node* cur_node) {
    if (cur_node->m_head == "<termo>" && cur_node->m_parent->m_head == "<outros_termos>") {
        auto op_ad = cur_node->sibling(0);
        auto outros_termos = cur_node->m_parent;

        outros_termos->m_attributes["op"] = op_ad->m_attributes["lexval"];
    }
}

void SemanticActions::action_outros_termos_POST_termo(Node* cur_node) {    
    if (cur_node->m_head == "<outros_termos>" && cur_node->m_parent->m_head == "<outros_termos>") {        
        auto termo = cur_node->sibling(1);
        cur_node->m_attributes["inh"] = termo->m_attributes["syn"];


        m_rightmost_actions.push(
            std::make_pair(
                std::make_pair(
                    &SemanticActions::action_outros_termos, 
                    cur_node
                ), 
                "<mais_comandos>"
            )
        );
    }

    if (cur_node->m_head == "<outros_termos>" && cur_node->m_node_list->at(0)->m_head == "&") {
        cur_node->m_attributes["syn"] = cur_node->m_attributes["inh"];
    }
};

void SemanticActions::action_expressao_POST_outros_termos(Node* cur_node) {    
    if(cur_node->m_head == "<expressao>") {        
        cur_node->m_attributes["syn"] = cur_node->m_node_list->at(1)->m_attributes["syn"];
    }
}

void SemanticActions::action_outros_termos(Node* cur_node) {    
    if (cur_node->m_head == "<outros_termos>" && cur_node->m_parent->m_head == "<outros_termos>") {        
        Node* outros_termos_parent = cur_node->m_parent;
        Node* outros_termos_child = cur_node;

        auto left_operand = outros_termos_parent->m_attributes["inh"];
        auto right_operand = outros_termos_child->m_attributes["syn"];

        Attribute::EAttrTipo temp_attr;

        if (left_operand.m_type != right_operand.m_type)
        {
            temp_attr = Attribute::REAL;
        } 
        else {
            temp_attr = left_operand.m_type;
        }

        outros_termos_parent->m_attributes["syn"] = Attribute(temp_attr, m_code_gen->gen_temp());
        std::string op = outros_termos_parent->m_node_list->at(0)->m_attributes["lexval"].to_string();

        m_code_gen->add_code(
            op,
            left_operand.to_string(),
            right_operand.to_string(), 
            outros_termos_parent->m_attributes["syn"].to_string()
        ); 
    }
}

void SemanticActions::action_comandos_POST_comando(Node* cur_node) {
    if (cur_node->sibling(0)->m_node_list->at(0)->m_head == "ident") {
        ParseTree::print_attr(cur_node->sibling(0), "");
        m_code_gen->print();
        std::exit(0);
    }    
}

/*
 *================================================================= NOT ACTIONS =====================================================================
 */

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
    }
};