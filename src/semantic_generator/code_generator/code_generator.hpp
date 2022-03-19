#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <string>
#include <vector>
#include <array>

struct CodeGenerator {
    std::string m_out_buffer;
    std::vector<std::array<std::string, 4>> m_instructions;
    int m_temp_var_acc {1};
    int m_cur_line {0};

    CodeGenerator(){};
    int add_code(std::string op, std::string argument1, std::string argument2, std::string result);
    int add_code(std::string op, double argument1, double argument2, std::string result);
    int add_code(std::string op, int argument1, int argument2, std::string result);
    int add_code(std::string op, double argument1, int argument2, std::string result);
    int add_code(std::string op, int argument1, double argument2, std::string result);
    std::string gen_temp();    
    void print();
    void edit(int line, int pos, std::string val);
};

#endif