#include "semantic_analyser_rec.hpp"

RecursiveSemanticAnalyser::RecursiveSemanticAnalyser(ParseTree* parse_tree, Scope* scopes, CodeGenerator* code_generator) {
    m_parse_tree = parse_tree;
    m_code_generator = code_generator;
    m_scopes = scopes;

    if (!m_parse_tree->m_tree_stack.empty()) 
    {
        printf("Pilha da árvore não vazia\n");
        printf("Deve ter algo de errado na análise sintática\n");
        exit(EXIT_SUCCESS);
    }
    m_parse_tree->m_current_node = m_parse_tree->m_root;
}

std::string RecursiveSemanticAnalyser::add_prefix(std::string scope_id, std::string op)
{
    if (op[0]-'0' <= 9)
    {
        return op;
    }

    return scope_id + "_" + op;
}

/*
============================================================================ACOES SEMANTICAS====================================================================================== 
 */

void RecursiveSemanticAnalyser::mais_ident(Node* cur_node)
{
    if (cur_node->child(0)->m_head == ",")
    {
        auto argumentos_node = cur_node->child(1);

        argumentos_node->m_attributes["counter"] = cur_node->m_attributes["counter"];
        argumentos_node->m_attributes["outer_scope"] = cur_node->m_attributes["outer_scope"];
        argumentos_node->m_attributes["inner_scope"] = cur_node->m_attributes["inner_scope"];
        argumentos(argumentos_node);

    }

    return;
}

void RecursiveSemanticAnalyser::argumentos(Node* cur_node)
{
    auto ident = cur_node->child(0)->m_terminal->lexem_to_str();
    auto outer_scope = cur_node->m_attributes["outer_scope"].to_string();
    auto inner_scope = cur_node->m_attributes["inner_scope"].to_string();
    int counter = cur_node->m_attributes["counter"].m_int;

    auto mais_ident_node = cur_node->child(1);

    if (counter > m_scopes->get_scope(inner_scope)->m_arguments.size()-1)
    {        
        char buffer[512];
        sprintf(buffer, "[ERRO] muitos argumentos na chamada da função %s\nRecursiveSemanticAnalyser::argumentos:45", inner_scope.c_str());
        std::string error_msg = std::string(buffer);
        throw std::runtime_error(error_msg);
    }

    if (m_scopes->find(outer_scope, ident))
    {
        auto arg_name = m_scopes->get_scope(inner_scope)->m_arguments[counter].m_name;                
        bool cond = m_scopes->get_scope(outer_scope)->m_table[ident].first.m_type == m_scopes->get_scope(inner_scope)->m_arguments[counter].m_type;
        cond = cond || (m_scopes->get_scope(inner_scope)->m_arguments[counter].m_type == EType::REAL);
        if (cond)
        {
            m_code_generator->add_code(":=", add_prefix(outer_scope,ident), "", add_prefix(inner_scope,arg_name));
            m_scopes->get_scope(inner_scope)->m_table[arg_name].second = add_prefix(outer_scope,ident);
            counter += 1;
            mais_ident_node->m_attributes["counter"] = Attribute(EType::INTEGER, counter);
            mais_ident_node->m_attributes["outer_scope"] = cur_node->m_attributes["outer_scope"];
            mais_ident_node->m_attributes["inner_scope"] = cur_node->m_attributes["inner_scope"];
            mais_ident(mais_ident_node);
            cur_node->m_attributes["end_loc"] = Attribute(EType::INTEGER, m_code_generator->m_cur_line);
        }
        else 
        {            
            char buffer[512];
            sprintf(buffer, "[ERRO] variável %s não corresponde ao tipo %s\nRecursiveSemanticAnalyser::argumentos:69", ident.c_str(), m_scopes->get_scope(inner_scope)->m_arguments[counter].type_to_string().c_str());
            std::string error_msg = std::string(buffer);
            throw std::runtime_error(error_msg);
        }
    }
    else 
    {
        char buffer[512];
        sprintf(buffer, "[ERRO] variável %s não existe\nRecursiveSemanticAnalyser::argumentos:77", ident.c_str());
        std::string error_msg = std::string(buffer);
        throw std::runtime_error(error_msg);        
    }

    return;
}

void RecursiveSemanticAnalyser::lista_arg(Node* cur_node)
{
    if (cur_node->child(0)->m_head == "&")
    {
        cur_node->m_attributes["end_loc"] = Attribute(EType::INTEGER, m_code_generator->m_cur_line);
        return;
    }
    else if (cur_node->child(0)->m_head == "(")
    {
        auto argumentos_node = cur_node->child(1);

        argumentos_node->m_attributes["outer_scope"] = cur_node->m_attributes["outer_scope"];
        argumentos_node->m_attributes["inner_scope"] = cur_node->m_attributes["inner_scope"];
        argumentos_node->m_attributes["counter"] = Attribute(EType::INTEGER, 0);
        argumentos(argumentos_node);
        cur_node->m_attributes["end_loc"] = Attribute(EType::INTEGER, m_code_generator->m_cur_line);
    }

    return;
}

void RecursiveSemanticAnalyser::restoIdent(Node* cur_node)
{
    auto first_node = cur_node->child(0);

    if (first_node->m_head == ":=")
    {
        auto expressao_node = cur_node->child(1);
        auto scope = cur_node->m_attributes["scope"].to_string();
        auto ident = cur_node->m_attributes["ident"].to_string();

        expressao_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
        expressao(expressao_node);        

        
        if (m_scopes->find(scope, ident))
        {
            if (m_scopes->get_scope(scope)->m_table[ident].first.m_type == expressao_node->m_attributes["syn"].m_type || expressao_node->m_attributes["syn"].m_type == EType::INTEGER)
            {
                m_scopes->get_scope(scope)->m_table[ident].second = expressao_node->m_attributes["syn"].to_string();                
                int three_addr_loc = m_code_generator->add_code(
                    ":=", 
                    add_prefix(scope, m_scopes->get_scope(scope)->m_table[ident].second), 
                    "", 
                    add_prefix(scope,ident)
                );

                cur_node->m_attributes["end_loc"] = Attribute(EType::INTEGER, three_addr_loc);
            }
            else
            {
                char buffer[512];
                sprintf(buffer, "[ERRO] variável %s não corresponde ao tipo da expressao\nRecursiveSemanticAnalyser::restoIdent:137", ident.c_str());
                std::string error_msg = std::string(buffer);
                throw std::runtime_error(error_msg);
            }
        }
        else
        {
            char buffer[512];
            sprintf(buffer, "[ERRO] variável %s não existe\nRecursiveSemanticAnalyser::restoIdent:145", ident.c_str());
            std::string error_msg = std::string(buffer);
            throw std::runtime_error(error_msg);
        }
    }
    else if (first_node->m_head == "<lista_arg>")
    {
        auto lista_arg_node = cur_node->child(0);        
        
        if (cur_node->m_attributes["scope"].to_string() == cur_node->m_attributes["ident"].to_string())
        {
            char buffer[512];
            sprintf(buffer, "[ERRO] recursão encontrada na chamada da função %s dentro da função %s\n", cur_node->m_attributes["ident"].to_string().c_str(), cur_node->m_attributes["scope"].to_string().c_str());
            std::string error_msg = std::string(buffer);
            throw std::runtime_error(error_msg);
        }
        
        lista_arg_node->m_attributes["outer_scope"] = cur_node->m_attributes["scope"];
        lista_arg_node->m_attributes["inner_scope"] = cur_node->m_attributes["ident"];
        lista_arg(lista_arg_node);
        m_scopes->erase_scope(lista_arg_node->m_attributes["inner_scope"].to_string());
        cur_node->m_attributes["end_loc"] = Attribute(EType::INTEGER, m_code_generator->m_cur_line);
    }

    return;
}

void RecursiveSemanticAnalyser::mais_dcloc(Node* cur_node)
{
    if (cur_node->child(0)->m_head != "&")
    {
        auto dc_loc_node = cur_node->child(1);        

        dc_loc_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
        dc_loc(dc_loc_node);
    }

    return;
}

void RecursiveSemanticAnalyser::dc_loc(Node* cur_node)
{
    if (cur_node->child(0)->m_head == "<dc_v>")
    {
        auto dc_v_node = cur_node->child(0);
        auto mais_dcloc_node = cur_node->child(1);

        dc_v_node->m_attributes["inh"] = cur_node->m_attributes["scope"];
        dc_v(dc_v_node);
        mais_dcloc_node->m_attributes["scope"] = cur_node->m_attributes["scope"];;
        mais_dcloc(mais_dcloc_node);
    }

    return;
}

void RecursiveSemanticAnalyser::corpo_p(Node* cur_node)
{
    auto dc_loc_node = cur_node->child(0);
    auto comandos_node = cur_node->child(2);

    dc_loc_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    dc_loc(dc_loc_node);
    m_code_generator->add_code("PROCEDURE", cur_node->m_attributes["scope"].to_string(), "", "");
    comandos_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    comandos(comandos_node);

    return;
}

void RecursiveSemanticAnalyser::mais_par(Node* cur_node)
{
    if (cur_node->child(0)->m_head == ";")
    {
        auto lista_par_node = cur_node->child(1);

        lista_par_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
        lista_par(lista_par_node);        
    }

    return;
}

void RecursiveSemanticAnalyser::lista_par(Node* cur_node)
{
    auto tipo_var_node = cur_node->child(0);
    auto variaveis_node = cur_node->child(2);
    auto mais_par_node = cur_node->child(3);

    tipo_var(tipo_var_node);
    variaveis_node->m_attributes["inh"] = tipo_var_node->m_attributes["syn"];
    variaveis_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    variaveis_node->m_attributes["args"] = Attribute(EType::STRING, "true");
    variaveis(variaveis_node);
    mais_par_node->m_attributes["scope"] = variaveis_node->m_attributes["scope"];
    mais_par(mais_par_node);

    return;
}

void RecursiveSemanticAnalyser::parametros(Node* cur_node)
{
    if (cur_node->child(0)->m_head == "(")
    {
        auto lista_par_node = cur_node->child(1);

        lista_par_node->m_attributes["scope"] = cur_node->m_attributes["inh"];
        lista_par(lista_par_node);
    }
    
    return;
}

void RecursiveSemanticAnalyser::dc_p(Node* cur_node)
{
    auto ident = cur_node->child(1)->m_terminal->lexem_to_str();
    auto parametros_node = cur_node->child(2);
    auto corpo_p_node = cur_node->child(3);

    cur_node->m_attributes["scope"] = Attribute(EType::STRING, ident);
    m_scopes->new_scope(ident);
    parametros_node->m_attributes["inh"] = cur_node->m_attributes["scope"];
    parametros(parametros_node);
    corpo_p_node->m_attributes["scope"] = parametros_node->m_attributes["inh"];
    corpo_p(corpo_p_node);

    return;
}

void RecursiveSemanticAnalyser::pfalsa(Node* cur_node)
{
    if (cur_node->child(0)->m_head == "&")
    {
        cur_node->m_attributes["end_loc"] = Attribute(EType::INTEGER, m_code_generator->m_cur_line);
        return;
    }

    auto comandos_node = cur_node->child(1);

    comandos_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    comandos(comandos_node);
    cur_node->m_attributes["end_loc"] = Attribute(EType::INTEGER, m_code_generator->m_cur_line);

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

    expressao1_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    expressao(expressao1_node);
    cur_node->m_attributes["left_op"] = expressao1_node->m_attributes["syn"];
    relacao(relacao_node);
    cur_node->m_attributes["rel_op"] = relacao_node->m_attributes["lexval"];
    expressao2_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    expressao(expressao2_node);
    cur_node->m_attributes["right_op"] = expressao2_node->m_attributes["syn"];
    
    auto syn_type = cur_node->m_attributes["left_op"].m_type == cur_node->m_attributes["right_op"].m_type ? cur_node->m_attributes["right_op"].m_type : EType::REAL;
    cur_node->m_attributes["syn"] = Attribute(syn_type, m_code_generator->gen_temp());
    
    m_code_generator->add_code(
        cur_node->m_attributes["rel_op"].to_string(),
        add_prefix(cur_node->m_attributes["scope"].to_string(), cur_node->m_attributes["left_op"].to_string()),
        add_prefix(cur_node->m_attributes["scope"].to_string(), cur_node->m_attributes["right_op"].to_string()),
        add_prefix(cur_node->m_attributes["scope"].to_string(), cur_node->m_attributes["syn"].to_string())
    );

    int three_addr_loc = m_code_generator->add_code(
        "JF",
        add_prefix(cur_node->m_attributes["scope"].to_string(), cur_node->m_attributes["syn"].to_string()),
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
    termo_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    termo(termo_node);    
    outros_termos1_node->m_attributes["inh"] = termo_node->m_attributes["syn"];    
    outros_termos1_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    outros_termos(outros_termos1_node);
    cur_node->m_attributes["syn"] = Attribute(cur_node->m_attributes["inh"].m_type, m_code_generator->gen_temp());

    m_code_generator->add_code(
        cur_node->m_attributes["op"].to_string(),
        add_prefix(cur_node->m_attributes["scope"].to_string(), cur_node->m_attributes["inh"].to_string()),
        add_prefix(cur_node->m_attributes["scope"].to_string(), outros_termos1_node->m_attributes["syn"].to_string()),
        add_prefix(cur_node->m_attributes["scope"].to_string(), cur_node->m_attributes["syn"].to_string())
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
        auto comandos_node = cur_node->child(1);

        comandos_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
        comandos(comandos_node);
        cur_node->m_attributes["end_loc"] = Attribute(EType::INTEGER, m_code_generator->m_cur_line);
    }

    cur_node->m_attributes["end_loc"] = Attribute(EType::INTEGER, m_code_generator->m_cur_line);

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
    fator_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    fator(fator_node);
    mais_fatores1_node->m_attributes["inh"] = fator_node->m_attributes["syn"];
    mais_fatores1_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    mais_fatores(mais_fatores1_node);
    cur_node->m_attributes["syn"] = Attribute(cur_node->m_attributes["inh"].m_type, m_code_generator->gen_temp());

    m_code_generator->add_code(
        cur_node->m_attributes["op"].to_string(),
        add_prefix(cur_node->m_attributes["scope"].to_string(), cur_node->m_attributes["inh"].to_string()),
        add_prefix(cur_node->m_attributes["scope"].to_string(), mais_fatores1_node->m_attributes["syn"].to_string()),
        add_prefix(cur_node->m_attributes["scope"].to_string(), cur_node->m_attributes["syn"].to_string())
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
    auto scope = cur_node->m_attributes["scope"].to_string();

    if (first_node->m_head == "ident") 
    {
        auto ident = first_node->m_terminal->lexem_to_str();
        
        if (m_scopes->m_symbol_tables[scope]->find(ident))
        {
            cur_node->m_attributes["syn"] = Attribute(m_scopes->m_symbol_tables[scope]->m_table[ident].first.m_type, ident);
        }            
        else
        {            
            char buffer[512];
            sprintf(buffer, "[ERRO] variável %s não existe\nRecursiveSemanticAnalyser::fator:428", ident.c_str());            
            std::string error_msg = std::string(buffer);
            throw std::runtime_error(error_msg);            
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

        expressao_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
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
    fator_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    fator(fator_node);
    mais_fatores_node->m_attributes["inh"] = fator_node->m_attributes["syn"];
    mais_fatores_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    mais_fatores(mais_fatores_node);
    cur_node->m_attributes["syn"] = mais_fatores_node->m_attributes["syn"];

    return;
}

void RecursiveSemanticAnalyser::expressao(Node* cur_node)
{
    auto termo_node = cur_node->child(0);
    auto outros_termos_node = cur_node->child(1);

    termo_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    termo(termo_node);    
    outros_termos_node->m_attributes["inh"] = termo_node->m_attributes["syn"];
    outros_termos_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    outros_termos(outros_termos_node);
    cur_node->m_attributes["syn"] = outros_termos_node->m_attributes["syn"];

    return;
}

void RecursiveSemanticAnalyser::comando(Node* cur_node)
{
    auto first_node = cur_node->child(0);
    auto scope = cur_node->m_attributes["scope"].to_string();
    
    if (first_node->m_head == "read" || first_node->m_head == "write")
    {
        auto ident = cur_node->child(2)->m_terminal->lexem_to_str();
        
        if (m_scopes->m_symbol_tables[scope]->find(ident))
        {
            int three_addr_loc = m_code_generator->add_code(
                first_node->m_head, 
                first_node->m_head[0] == 'r' ? "" : add_prefix(scope, ident),
                "", 
                first_node->m_head[0] == 'r' ? add_prefix(scope, ident) : ""
            );

            cur_node->m_attributes["end_loc"] = Attribute(EType::INTEGER, three_addr_loc);
        }
        else
        {
            char buffer[512];
            sprintf(buffer, "[ERRO] variável %s não existe\nRecursiveSemanticAnalyser::comando:524", ident.c_str());            
            std::string error_msg = std::string(buffer);
            throw std::runtime_error(error_msg);             
        }        
    }
    else if (first_node->m_head == "ident")
    {        
        auto restoIdent_node = cur_node->child(1);
        auto ident = cur_node->child(0)->m_terminal->lexem_to_str();

        restoIdent_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
        restoIdent_node->m_attributes["ident"] = Attribute(EType::STRING, ident);
        restoIdent(restoIdent_node);        
        cur_node->m_attributes["end_loc"] = Attribute(EType::INTEGER, m_code_generator->m_cur_line);
    }
    else if (first_node->m_head == "if")
    {
        auto condicao_node = cur_node->child(1);
        auto comandos_node = cur_node->child(3);
        auto pfalsa_node = cur_node->child(4);

        condicao_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
        condicao(condicao_node);
        comandos_node->m_attributes["scope"] = cur_node->m_attributes["scope"];;
        comandos(comandos_node);
        
        if (pfalsa_node->child(0)->m_head == "else")
        {
            condicao_node->m_attributes["goto_loc"] = Attribute(EType::INTEGER, m_code_generator->add_code("goto", "", "", ""));
        }            

        pfalsa_node->m_attributes["inh"] = Attribute(EType::INTEGER, m_code_generator->m_cur_line);
        pfalsa_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
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

    comando_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    comando(comando_node);    
    mais_comandos_node->m_attributes["inh"] = Attribute(EType::INTEGER, m_code_generator->m_cur_line);
    mais_comandos_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    mais_comandos(mais_comandos_node);
    cur_node->m_attributes["end_loc"] = Attribute(EType::INTEGER, m_code_generator->m_cur_line);

    return;
}

void RecursiveSemanticAnalyser::mais_dc(Node* cur_node)  
{
    if (cur_node->child(0)->m_head == ";")
    {
        cur_node->child(1)->m_attributes["scope"] = cur_node->m_attributes["scope"];
        dc(cur_node->child(1));
    }
    else
        return;
}

void RecursiveSemanticAnalyser::mais_var(Node* cur_node) 
{
    if (cur_node->child(0)->m_head == ",") 
    {
        cur_node->child(1)->m_attributes["inh"] = cur_node->m_attributes["inh"];
        cur_node->child(1)->m_attributes["scope"] = cur_node->m_attributes["scope"];
        cur_node->child(1)->m_attributes["args"] = cur_node->m_attributes["args"];
        variaveis(cur_node->child(1));
    } 
    else
        return;
}

void RecursiveSemanticAnalyser::variaveis(Node* cur_node) 
{
    auto ident = cur_node->child(0)->m_terminal->lexem_to_str();
    auto type = cur_node->m_attributes["inh"].to_string();
    auto scope = cur_node->m_attributes["scope"].to_string();

    m_scopes->add_to_scope(scope, Symbol(ident, type));
    if (scope != "main" && cur_node->m_attributes["args"].to_string() == "true")
    {
        m_scopes->add_args(scope, Symbol(ident, type));
    }
    
    if (type == "real")
        m_code_generator->add_code("ALME", "0.0", "", add_prefix(scope, ident));
    else
        m_code_generator->add_code("ALME", "0", "", add_prefix(scope, ident));

    auto mais_var_node = cur_node->child(1);
    mais_var_node->m_attributes["inh"] = cur_node->m_attributes["inh"];
    mais_var_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
    mais_var_node->m_attributes["args"] = cur_node->m_attributes["args"];
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
    variaveis_node->m_attributes["scope"] = cur_node->m_attributes["inh"];
    variaveis(variaveis_node);

    return;
}

void RecursiveSemanticAnalyser::dc(Node* cur_node) 
{
    if (cur_node->m_node_list->size() >= 1) 
    {                        
        auto first_node = cur_node->child(0);

        if (first_node->m_head == "<dc_v>")
        {
            auto dc_v_node = cur_node->child(0);
            auto mais_dc_node = cur_node->child(1);
            
            dc_v_node->m_attributes["inh"] = cur_node->m_attributes["scope"];
            dc_v(dc_v_node);
            mais_dc_node->m_attributes["scope"] = cur_node->m_attributes["scope"];
            mais_dc(mais_dc_node);
        }
        else if(first_node->m_head == "<dc_p>")
        {
            auto dc_p_node = cur_node->child(0);            
            dc_p(dc_p_node);
        }
    }

    return;
}

void RecursiveSemanticAnalyser::corpo(Node* cur_node) 
{
    auto dc_node = cur_node->child(0);
    auto comandos_node = cur_node->child(2);

    m_scopes->new_scope("main");
    dc_node->m_attributes["scope"] = Attribute(EType::STRING, "main");
    dc(dc_node);
    m_code_generator->add_code("PROCEDURE", "main", "", "");
    comandos_node->m_attributes["scope"] = dc_node->m_attributes["scope"];
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