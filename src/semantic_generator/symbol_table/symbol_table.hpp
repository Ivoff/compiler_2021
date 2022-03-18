#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <map>
#include <string>
#include <exception>
#include "../../misc/type.hpp"

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
    // par(simbolo, valor)
    std::map<std::string, std::pair<Symbol, std::string >> m_table;

    SymbolTable(){};
    void add_entry(Symbol symbol);
    bool find(std::string name);
    void print();
};

struct Scope {    
    SymbolTable* m_symbol_table;    
    Scope* m_parent_scope;

    Scope() {
        m_parent_scope = nullptr;
        m_symbol_table = new SymbolTable();        
    }

    Scope(Scope* parent_scope) {
        m_parent_scope = parent_scope;
        m_symbol_table = new SymbolTable();        
    }
};

#endif