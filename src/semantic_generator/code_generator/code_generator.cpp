#include "code_generator.hpp"

void CodeGenerator::add_code(std::string op, std::string argument1, std::string argument2, std::string result) {
    m_out_buffer += op + "; " + argument1 + "; " + argument2 + "; " + result + "\n";
}

void CodeGenerator::add_code(std::string op, double argument1, double argument2, std::string result) {
    m_out_buffer += op + "; " + std::to_string(argument1) + "; " + std::to_string(argument2) + "; " + result + "\n";
};

void CodeGenerator::add_code(std::string op, int argument1, int argument2, std::string result) {
    m_out_buffer += op + "; " + std::to_string(argument1) + "; " + std::to_string(argument2) + "; " + result + "\n";
};

void CodeGenerator::add_code(std::string op, double argument1, int argument2, std::string result) {
    m_out_buffer += op + "; " + std::to_string(argument1) + "; " + std::to_string(argument2) + "; " + result + "\n";
};

void CodeGenerator::add_code(std::string op, int argument1, double argument2, std::string result) {
    m_out_buffer += op + "; " + std::to_string(argument1) + "; " + std::to_string(argument2) + "; " + result + "\n";
};


void CodeGenerator::print() {
    printf("%s\n", m_out_buffer.c_str());
}

std::string CodeGenerator::gen_temp() {
    return "t"+std::to_string(m_temp_var_acc++);
}