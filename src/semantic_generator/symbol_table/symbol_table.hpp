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
    std::map<std::string, Symbol> m_table;

    SymbolTable(){};
    void add_entry(std::string name, std::string type);
};

#endif