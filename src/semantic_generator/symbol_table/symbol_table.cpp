#include "symbol_table.hpp"

void SymbolTable::add_entry(Symbol symbol) 
{    
    if (m_table.count(symbol.m_name)) 
    {
        throw std::runtime_error("Semantic Error: variable \"" + symbol.m_name + "\" is already defined");
    }
    else {
        m_table.insert({symbol.m_name, std::make_pair(symbol, "")});
    }
}

void SymbolTable::print() {
    for(auto it = m_table.begin(); it != m_table.end(); ++it)
    {
        printf("%s %s: %s\n", it->second.first.type_to_string().c_str(), it->first.c_str(), it->second.second.c_str());
    }
}