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

bool SymbolTable::find(std::string name)
{
    return m_table.count(name) != 0;
}

void SymbolTable::print() {
    
    std::string aux = "";
    
    for(auto it = m_table.begin(); it != m_table.end(); ++it)
    {        
        for(auto args_it = m_arguments.begin(); args_it != m_arguments.end(); ++args_it)
        {
            if (args_it->m_name == it->first)
            {
                aux = "[arg] ";
                break;
            }
        }
        printf("\t%s%s %s: %s\n", aux.c_str(), it->second.first.type_to_string().c_str(), it->first.c_str(), it->second.second.c_str());
        aux = "";
    }    
}

SymbolTable* Scope::get_scope(std::string scope_id)
{
    if (m_symbol_tables.count(scope_id))
    {
        return m_symbol_tables[scope_id];
    }
    else
    {
        throw std::runtime_error("Semantic Error[get_scope]: scope \"" + scope_id + "\" doesn't exist");
    }

    return nullptr;
}

SymbolTable* Scope::new_scope(std::string scope_id)
{
    if (!m_symbol_tables.count(scope_id))
    {
        m_symbol_tables[scope_id] = new SymbolTable();
        return m_symbol_tables[scope_id];
    }
    else 
    {
        throw std::runtime_error("Semantic Error[new_scope]: scope already \"" + scope_id + "\" exist");
    }

    return nullptr;
}

SymbolTable* Scope::add_to_scope(std::string scope_id, Symbol symbol)
{
    if (m_symbol_tables.count(scope_id))
    {
        m_symbol_tables[scope_id]->add_entry(symbol);
        return m_symbol_tables[scope_id];
    }
    else
    {
        throw std::runtime_error("Semantic Error[add_to_scope]: scope \"" + scope_id + "\" doesn't exist");
    }

    return nullptr;
}

SymbolTable* Scope::add_args(std::string scope_id, Symbol symbol)
{
    if (m_symbol_tables.count(scope_id))
    {
        m_symbol_tables[scope_id]->m_arguments.push_back(symbol);
        return m_symbol_tables[scope_id];
    }
    else
    {
        throw std::runtime_error("Semantic Error[add_args]: scope \"" + scope_id + "\" doesn't exist");
    }

    return nullptr;
}

bool Scope::find(std::string scope_id, std::string symbol)
{
    if (m_symbol_tables.count(scope_id))
    {                
        return m_symbol_tables[scope_id]->find(symbol);
    }
    else
    {
        throw std::runtime_error("Semantic Error[find]: scope \"" + scope_id + "\" doesn't exist");
    }

    return false;    
}

void Scope::erase_scope(std::string scope_id)
{
    SymbolTable* cur_table = m_symbol_tables[scope_id];    
    auto symb_table = cur_table->m_table;    

    for(auto it = symb_table.begin(); it != symb_table.end(); ++it)
    {
        it->second.second = "";
    }   
}

void Scope::print()
{
    for(auto it = m_symbol_tables.begin(); it != m_symbol_tables.end(); ++it)
    {
        printf("%s\n", it->first.c_str());
        it->second->print();        
    }
}