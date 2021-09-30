#include "symbol_table.hpp"

void SymbolTable::add_entry(std::string name, std::string type) {
    Symbol::EType aux_type;

    if (type == "real")
        aux_type = Symbol::REAL;
    else
        aux_type = Symbol::INTEGER;        
    
    if (m_table.count(name)) {
        throw std::runtime_error("Semantic Error: variable \"" + name + "\" is already defined");
    }
    else {
        m_table.insert({name, Symbol(name, aux_type )});
    }
}