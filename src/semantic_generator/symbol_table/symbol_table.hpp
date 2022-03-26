#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <map>
#include <string>
#include <exception>
#include <vector>
#include "../../misc/type.hpp"
#include "../code_generator/code_generator.hpp"

struct Symbol 
{
    std::string m_name;
    EType m_type;

    Symbol(){};
    Symbol(std::string name, EType type) {
        m_name = name;
        m_type = type;
    }

    Symbol(std::string name, std::string type) {
        m_name = name;
        if (type == "real")
            m_type = EType::REAL;
        else
            m_type = EType::INTEGER;
    }

    std::string type_to_string() {
        if (m_type == EType::INTEGER)
            return "integer";
        else
            return "real";
    }    
};

struct SymbolTable {
    // map (identificador_simbolo, par)
    // par(simbolo, valor)
    std::map<std::string, std::pair<Symbol, std::string >> m_table;
    std::vector<Symbol> m_arguments;

    SymbolTable(){};
    void add_entry(Symbol symbol);
    bool find(std::string name);
    void print();    
};

struct Scope {
    // <ident, tabela de símbolos>
    std::map<std::string, SymbolTable*> m_symbol_tables;

    Scope() {}

    SymbolTable* get_scope(std::string scope_id);
    SymbolTable* new_scope(std::string scope_id);
    SymbolTable* add_to_scope(std::string scope_id, Symbol symbol);
    SymbolTable* add_args(std::string scope_id, Symbol symbol);    
    void erase_scope(std::string scope_id, CodeGenerator* code_gen);
    bool find(std::string scope_id, std::string symbol);
    void print();
};

#endif