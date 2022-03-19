#include "code_generator.hpp"

int CodeGenerator::add_code(std::string op, std::string argument1, std::string argument2, std::string result) {    
    m_instructions.push_back({op, argument1, argument2, result});    
    return m_cur_line++;
}

int CodeGenerator::add_code(std::string op, double argument1, double argument2, std::string result) {
    m_instructions.push_back({op, std::to_string(argument1), std::to_string(argument2), result});
    return m_cur_line++;
};

int CodeGenerator::add_code(std::string op, int argument1, int argument2, std::string result) {
    m_instructions.push_back({op, std::to_string(argument1), std::to_string(argument2), result});
    return m_cur_line++;
};

int CodeGenerator::add_code(std::string op, double argument1, int argument2, std::string result) {
    m_instructions.push_back({op, std::to_string(argument1), std::to_string(argument2), result});
    return m_cur_line++;
};

int CodeGenerator::add_code(std::string op, int argument1, double argument2, std::string result) {
    m_instructions.push_back({op, std::to_string(argument1), std::to_string(argument2), result});
    return m_cur_line++;
};


void CodeGenerator::print() {
    int count = 0;
    for(auto itr = m_instructions.begin(); itr != m_instructions.end(); itr++)
    {
        printf("%03d; %s; %s; %s; %s\n", count++, itr->at(0).c_str(), itr->at(1).c_str(), itr->at(2).c_str(), itr->at(3).c_str());
    }    
}

std::string CodeGenerator::gen_temp() {
    return "t"+std::to_string(m_temp_var_acc++);
}

void CodeGenerator::edit(int line, int pos, std::string val)
{
    try
    {
        m_instructions[line][pos] = val;
    }
    catch(std::runtime_error& ex)
    {
        throw ex;
    }    
}