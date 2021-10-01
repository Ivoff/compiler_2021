#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <string>

struct CodeGenerator {
    std::string m_out_buffer;
    int m_temp_var_acc {1};

    CodeGenerator(){};
    void add_code(std::string op, std::string argument1, std::string argument2, std::string result);
    void add_code(std::string op, double argument1, double argument2, std::string result);
    void add_code(std::string op, int argument1, int argument2, std::string result);
    void add_code(std::string op, double argument1, int argument2, std::string result);
    void add_code(std::string op, int argument1, double argument2, std::string result);
    std::string gen_temp();
    void print();
};

#endif