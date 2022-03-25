#include "code_generator.hpp"

int CodeGenerator::add_code(std::string op, std::string argument1, std::string argument2, std::string result) {    
    m_instructions.push_back({op, argument1, argument2, result});
    update_op_max_length(op, argument1, argument2, result);
    return m_cur_line++;
}

int CodeGenerator::add_code(std::string op, double argument1, double argument2, std::string result) {
    m_instructions.push_back({op, std::to_string(argument1), std::to_string(argument2), result});
    update_op_max_length(op, std::to_string(argument1), std::to_string(argument2), result);
    return m_cur_line++;
};

int CodeGenerator::add_code(std::string op, int argument1, int argument2, std::string result) {
    m_instructions.push_back({op, std::to_string(argument1), std::to_string(argument2), result});
    update_op_max_length(op, std::to_string(argument1), std::to_string(argument2), result);
    return m_cur_line++;
};

int CodeGenerator::add_code(std::string op, double argument1, int argument2, std::string result) {
    m_instructions.push_back({op, std::to_string(argument1), std::to_string(argument2), result});
    update_op_max_length(op, std::to_string(argument1), std::to_string(argument2), result);
    return m_cur_line++;
};

int CodeGenerator::add_code(std::string op, int argument1, double argument2, std::string result) {
    m_instructions.push_back({op, std::to_string(argument1), std::to_string(argument2), result});
    update_op_max_length(op, std::to_string(argument1), std::to_string(argument2), result);
    return m_cur_line++;
};


void CodeGenerator::print() {
    int count = 0;
    std::string print_format = "%03d| %"+std::to_string(m_op_max_length[0])+"s| %"+std::to_string(m_op_max_length[1])+"s| %"+std::to_string(m_op_max_length[2])+"s| %"+std::to_string(m_op_max_length[3])+"s\n";
    for(auto itr = m_instructions.begin(); itr != m_instructions.end(); itr++)
    {
        printf(print_format.c_str(), count++, itr->at(0).c_str(), itr->at(1).c_str(), itr->at(2).c_str(), itr->at(3).c_str());
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

void CodeGenerator::update_op_max_length(std::string op0, std::string op1, std::string op2,std::string op3)
{
    m_op_max_length[0] = std::max(m_op_max_length[0], (int)op0.size());
    m_op_max_length[1] = std::max(m_op_max_length[1], (int)op1.size());
    m_op_max_length[2] = std::max(m_op_max_length[2], (int)op2.size());
    m_op_max_length[3] = std::max(m_op_max_length[3], (int)op3.size());        
}