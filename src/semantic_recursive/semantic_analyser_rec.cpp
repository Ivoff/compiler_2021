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
        // mais_dc(mais_dc_node);
    }

    return;
}

void RecursiveSemanticAnalyser::corpo(Node* cur_node) 
{
    auto dc_node = cur_node->child(0);
    auto comandos_node = cur_node->child(2);

    dc(dc_node);
    // comandos(comandos_node);
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