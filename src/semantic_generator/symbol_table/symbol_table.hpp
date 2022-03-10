#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <map>
#include <string>
#include <exception>

struct Symbol {
    enum EType {
        INTEGER = 0x300,
        REAL
    };

    std::string m_name;
    EType m_type;

    Symbol(){};
    Symbol(std::string name, Symbol::EType type) {
        m_name = name;
        m_type = type;
    }
};

struct SymbolTable {
    // par(simbolo, valor)
    std::map<std::string, std::pair<Symbol, std::string >> m_table;

    SymbolTable(){};
    void add_entry(std::string name, std::string type);
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