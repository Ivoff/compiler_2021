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
        m_table.insert({name, std::make_pair(Symbol(name, aux_type), "")});
    }
}

void SymbolTable::print_table() {
    for(auto it = m_table.begin(); it != m_table.end(); ++it)
    {
        printf("%s %s: %s\n", it->second.first.type_to_string().c_str(), it->first.c_str(), it->second.second.c_str());
    }
}