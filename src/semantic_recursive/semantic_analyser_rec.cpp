#include "semantic_analyser_rec.hpp"

RecursiveSemanticAnalyser::RecursiveSemanticAnalyser(ParseTree* parse_tree, SymbolTable* symbol_table, CodeGenerator* code_generator) {
    m_parse_tree = parse_tree;
    m_code_generator = code_generator;
    m_symbol_table = symbol_table;

    if (!m_parse_tree->m_tree_stack.empty()) 
    {
        printf("Pilha da árvore não vazia\n");
        printf("Deve ter algo de errado na análise sintática\n");
        exit(EXIT_SUCCESS);
    }
    m_parse_tree->m_current_node = m_parse_tree->m_root;
}

void RecursiveSemanticAnalyser::pfalsa(Node* cur_node)
{
    if (cur_node->child(0)->m_head == "&")
    {
        cur_node->m_attributes["end_loc"] = cur_node->m_attributes["inh"];
        return;
    }

    auto comandos_node = cur_node->child(1);

    comandos(comandos_node);
    cur_node->m_attributes["end_loc"] = comandos_node->m_attributes["end_loc"];

    return;
}

void RecursiveSemanticAnalyser::relacao(Node* cur_node)
{
    cur_node->m_attributes["lexval"] = Attribute(EType::STRING, cur_node->child(0)->m_terminal->lexem_to_str());
    return;
}

void RecursiveSemanticAnalyser::condicao(Node* cur_node)
{
    auto expressao1_node = cur_node->child(0);
    auto relacao_node = cur_node->child(1);
    auto expressao2_node = cur_node->child(2);

    expressao(expressao1_node);
    cur_node->m_attributes["left_op"] = expressao1_node->m_attributes["syn"];
    relacao(relacao_node);
    cur_node->m_attributes["rel_op"] = relacao_node->m_attributes["lexval"];
    expressao(expressao2_node);
    cur_node->m_attributes["right_op"] = expressao2_node->m_attributes["syn"];
    
    auto syn_type = cur_node->m_attributes["left_op"].m_type == cur_node->m_attributes["right_op"].m_type ? cur_node->m_attributes["right_op"].m_type : EType::REAL;
    cur_node->m_attributes["syn"] = Attribute(syn_type, m_code_generator->gen_temp());
    
    m_code_generator->add_code(
        cur_node->m_attributes["rel_op"].to_string(),
        cur_node->m_attributes["left_op"].to_string(),
        cur_node->m_attributes["right_op"].to_string(),
        cur_node->m_attributes["syn"].to_string()
    );

    int three_addr_loc = m_code_generator->add_code(
        "JF",
        cur_node->m_attributes["syn"].to_string(),
        "",
        ""
    );

    cur_node->m_attributes["jump_loc"] = Attribute(EType::INTEGER, three_addr_loc);    

    return;
}

void RecursiveSemanticAnalyser::outros_termos(Node* cur_node)
{
    if (cur_node->child(0)->m_head == "&")
    {
        cur_node->m_attributes["syn"] = cur_node->m_attributes["inh"];
        return;
    }

    auto op_ad_node = cur_node->child(0);
    auto termo_node = cur_node->child(1);
    auto outros_termos1_node = cur_node->child(2);

    op_ad(op_ad_node);
    cur_node->m_attributes["op"] = Attribute(EType::STRING, op_ad_node->m_attributes["lexval"].to_string());
    termo(termo_node);
    outros_termos1_node->m_attributes["inh"] = termo_node->m_attributes["syn"];
    outros_termos(outros_termos1_node);
    cur_node->m_attributes["syn"] = Attribute(cur_node->m_attributes["inh"].m_type, m_code_generator->gen_temp());

    m_code_generator->add_code(
        cur_node->m_attributes["op"].to_string(),
        cur_node->m_attributes["inh"].to_string(),
        outros_termos1_node->m_attributes["syn"].to_string(),
        cur_node->m_attributes["syn"].to_string()
    );

    return;
}

void RecursiveSemanticAnalyser::op_ad(Node* cur_node)
{
    cur_node->m_attributes["lexval"] = Attribute(EType::STRING, cur_node->child(0)->m_terminal->lexem_to_str());
    return;
}

void RecursiveSemanticAnalyser::mais_comandos(Node* cur_node)
{
    if (cur_node->child(0)->m_head == ";")
    {
        comandos(cur_node->child(1));
        cur_node->m_attributes["end_loc"] = cur_node->child(1)->m_attributes["end_loc"];
    }

    cur_node->m_attributes["end_loc"] = cur_node->m_attributes["inh"];

    return;
}

void RecursiveSemanticAnalyser::mais_fatores(Node* cur_node)
{
    if (cur_node->child(0)->m_head == "&")
    {
        cur_node->m_attributes["syn"] = cur_node->m_attributes["inh"];
        return;
    }

    auto op_mul_node = cur_node->child(0);
    auto fator_node = cur_node->child(1);
    auto mais_fatores1_node = cur_node->child(2);        

    op_mul(op_mul_node);
    cur_node->m_attributes["op"] = Attribute(EType::STRING, op_mul_node->m_attributes["lexval"].to_string());
    fator(fator_node);
    mais_fatores1_node->m_attributes["inh"] = fator_node->m_attributes["syn"];    
    mais_fatores(mais_fatores1_node);
    cur_node->m_attributes["syn"] = Attribute(cur_node->m_attributes["inh"].m_type, m_code_generator->gen_temp());

    m_code_generator->add_code(
        cur_node->m_attributes["op"].to_string(),
        cur_node->m_attributes["inh"].to_string(),
        mais_fatores1_node->m_attributes["syn"].to_string(),
        cur_node->m_attributes["syn"].to_string()
    );

    return;
}

void RecursiveSemanticAnalyser::op_mul(Node* cur_node)
{    
    cur_node->m_attributes["lexval"] = Attribute(EType::STRING, cur_node->child(0)->m_terminal->lexem_to_str());
    return;
}

void RecursiveSemanticAnalyser::fator(Node* cur_node)
{
    auto first_node = cur_node->child(0);    

    if (first_node->m_head == "ident") 
    {
        auto ident = first_node->m_terminal->lexem_to_str();

        if (m_symbol_table->find(ident))
        {            
            cur_node->m_attributes["syn"] = Attribute(m_symbol_table->m_table[ident].first.m_type, ident);
        }            
        else
        {
            printf("[ERRO] variável %s não existe\n", ident);
            std::exit(0);
        }

    }
    else if (first_node->m_head == "numero_int")
    {        
        auto fator_inh = cur_node->m_attributes["inh"].to_string();
        cur_node->m_attributes["syn"] = Attribute(EType::INTEGER, fator_inh+first_node->m_terminal->lexem_to_str());
    }
    else if (first_node->m_head == "numero_real")
    {
        auto fator_inh = cur_node->m_attributes["inh"].to_string();
        cur_node->m_attributes["syn"] = Attribute(EType::REAL, fator_inh+first_node->m_terminal->lexem_to_str());
    }
    else
    {
        auto expressao_node = cur_node->child(1);
        expressao(expressao_node);
        cur_node->m_attributes["syn"] = expressao_node->m_attributes["syn"];
    }
}

void RecursiveSemanticAnalyser::op_un(Node* cur_node)
{
    if (cur_node->child(0)->m_head == "-" && cur_node->child(0)->m_terminal != nullptr)
    {        
        cur_node->m_attributes["val"] = Attribute(EType::STRING, "-");
    }
    else
    {        
        cur_node->m_attributes["val"] = Attribute(EType::STRING, "");
    }

    return;
}

void RecursiveSemanticAnalyser::termo(Node* cur_node)
{
    auto op_un_node = cur_node->child(0);
    auto fator_node = cur_node->child(1);
    auto mais_fatores_node = cur_node->child(2);

    op_un(op_un_node);
    fator_node->m_attributes["inh"] = op_un_node->m_attributes["val"];
    fator(fator_node);
    mais_fatores_node->m_attributes["inh"] = fator_node->m_attributes["syn"];
    mais_fatores(mais_fatores_node);
    cur_node->m_attributes["syn"] = mais_fatores_node->m_attributes["syn"];

    return;
}

void RecursiveSemanticAnalyser::expressao(Node* cur_node)
{
    auto termo_node = cur_node->child(0);
    auto outros_termos_node = cur_node->child(1);

    termo(termo_node);
    outros_termos_node->m_attributes["inh"] = termo_node->m_attributes["syn"];
    outros_termos(outros_termos_node);
    cur_node->m_attributes["syn"] = outros_termos_node->m_attributes["syn"];

    return;
}

void RecursiveSemanticAnalyser::comando(Node* cur_node)
{
    auto first_node = cur_node->child(0);
    
    if (first_node->m_head == "read" || first_node->m_head == "write")
    {
        auto ident = cur_node->child(2)->m_terminal->lexem_to_str();
        
        if (m_symbol_table->find(ident))
        {
            int three_addr_loc = m_code_generator->add_code(
                first_node->m_head, 
                first_node->m_head[0] == 'r' ? "" : ident, 
                "", 
                first_node->m_head[0] == 'r' ? ident : ""
            );

            cur_node->m_attributes["end_loc"] = Attribute(EType::INTEGER, three_addr_loc);
        }
        else
        {
            printf("[ERRO] variável %s não existe\n", ident);
            std::exit(0);
        }        
    }
    else if (first_node->m_head == "ident")
    {
        auto expressao_node = cur_node->child(2);
        auto ident = cur_node->child(0)->m_terminal->lexem_to_str();

        expressao(expressao_node);

        if (m_symbol_table->find(ident))
        {
            if (m_symbol_table->m_table[ident].first.m_type == expressao_node->m_attributes["syn"].m_type || expressao_node->m_attributes["syn"].m_type == EType::INTEGER)
            {
                m_symbol_table->m_table[ident].second = expressao_node->m_attributes["syn"].to_string();                
                int three_addr_loc = m_code_generator->add_code(
                    ":=", 
                    m_symbol_table->m_table[ident].second, 
                    "", 
                    ident
                );

                cur_node->m_attributes["end_loc"] = Attribute(EType::INTEGER, three_addr_loc);
            }
            else
            {
                printf("[ERRO] variável %s não corresponde ao tipo da expressao\n", ident);
                std::exit(0);
            }
        }
        else
        {
            printf("[ERRO] variável %s não existe\n", ident);
            std::exit(0);
        }
    }
    else if (first_node->m_head == "if")
    {
        auto condicao_node = cur_node->child(1);
        auto comandos_node = cur_node->child(3);
        auto pfalsa_node = cur_node->child(4);

        condicao(condicao_node);
        comandos(comandos_node);
        
        if (pfalsa_node->child(0)->m_head == "else")
        {
            condicao_node->m_attributes["goto_loc"] = Attribute(EType::INTEGER, m_code_generator->add_code("goto", "", "", ""));
        }            

        pfalsa_node->m_attributes["inh"] = Attribute(EType::INTEGER, m_code_generator->m_cur_line);
        m_code_generator->edit(condicao_node->m_attributes["jump_loc"].m_int, 2, std::to_string(m_code_generator->m_cur_line));
        pfalsa(pfalsa_node);

        if (pfalsa_node->child(0)->m_head == "else")
        {
            m_code_generator->edit(condicao_node->m_attributes["goto_loc"].m_int, 1, std::to_string(m_code_generator->m_cur_line));
        }            
    }

    return;
}

void RecursiveSemanticAnalyser::comandos(Node* cur_node)
{
    auto comando_node = cur_node->child(0);
    auto mais_comandos_node = cur_node->child(1);

    comando(comando_node);
    mais_comandos_node->m_attributes["inh"] = comando_node->m_attributes["end_loc"];
    mais_comandos(mais_comandos_node);
    cur_node->m_attributes["end_loc"] = mais_comandos_node->m_attributes["end_loc"];

    return;
}

void RecursiveSemanticAnalyser::mais_dc(Node* cur_node)  
{
    if (cur_node->child(0)->m_head == ";")
        dc(cur_node->child(1));
    else
        return;
}

void RecursiveSemanticAnalyser::mais_var(Node* cur_node) 
{
    if (cur_node->child(0)->m_head == ",") 
    {
        cur_node->child(1)->m_attributes["inh"] = cur_node->m_attributes["inh"];
        variaveis(cur_node->child(1));
    } 
    else
        return;
}

void RecursiveSemanticAnalyser::variaveis(Node* cur_node) 
{
    auto ident = cur_node->child(0)->m_terminal->lexem_to_str();
    auto type = cur_node->m_attributes["inh"].to_string();
    
    m_symbol_table->add_entry(Symbol(ident, type));
    
    if (type == "real")
        m_code_generator->add_code("ALME", "0.0", "", ident);
    else
        m_code_generator->add_code("ALME", "0", "", ident);

    auto mais_var_node = cur_node->child(1);
    mais_var_node->m_attributes["inh"] = cur_node->m_attributes["inh"];
    mais_var(mais_var_node);

    return;
}

void RecursiveSemanticAnalyser::tipo_var(Node* cur_node) 
{       
    auto tipo_node = cur_node->child(0);

    if (tipo_node->m_head == "integer")    
        cur_node->m_attributes["syn"] = Attribute(EType::STRING, tipo_node->m_terminal->lexem_to_str());    
    else
        cur_node->m_attributes["syn"] = Attribute(EType::STRING, tipo_node->m_terminal->lexem_to_str());

    return;
}

void RecursiveSemanticAnalyser::dc_v(Node* cur_node) 
{
    auto tipo_var_node = cur_node->child(0);
    auto variaveis_node = cur_node->child(2);

    tipo_var(tipo_var_node);
    variaveis_node->m_attributes["inh"] = tipo_var_node->m_attributes["syn"];
    variaveis(variaveis_node);

    return;
}

void RecursiveSemanticAnalyser::dc(Node* cur_node) 
{
    if (cur_node->m_node_list->size() > 1) 
    {
        auto dc_v_node = cur_node->child(0);
        auto mais_dc_node = cur_node->child(1);

        dc_v(dc_v_node);
        mais_dc(mais_dc_node);
    }

    return;
}

void RecursiveSemanticAnalyser::corpo(Node* cur_node) 
{
    auto dc_node = cur_node->child(0);
    auto comandos_node = cur_node->child(2);

    dc(dc_node);
    comandos(comandos_node);
    m_code_generator->add_code("PARA", "", "", "");
}

void RecursiveSemanticAnalyser::programa(Node* cur_node) 
{
    auto corpo_node = cur_node->child(2);
    
    corpo(corpo_node);    
}

void RecursiveSemanticAnalyser::analise() 
{    
    programa(m_parse_tree->m_root);
}