#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <string.h>

enum class ETokenId {
    IDENT = 0x100,
    NUMBER_INT,
    NUMBER_REAL,
    EQUAL_OP,
    DIFF_OP,
    LESS_OP,
    GREATER_OP,
    LESS_EQUAL_OP,
    GREATER_EQUAL_OP,
    ASSIGNMENT_OP,
    ADD_OP,
    SUB_OP,
    MULT_OP,
    DIV_OP,
    IF,
    THEN,
    ELSE,
    SYMBOL,
    RESERVED,
    NULLABLE
};

struct Token {
    ETokenId m_id;
    union {
        bool m_has_not_attr;
        int m_int_attr;
        float m_float_attr;
        double m_double_attr;
        char* m_str_attr;
    };

    Token(ETokenId id) {
        m_id = id;    
        m_has_not_attr = true;    
    }

    Token(ETokenId id, int int_attr) {
        m_id = id;
        m_int_attr = int_attr;
    }

    Token(ETokenId id, float float_attr) {
        m_id = id;
        m_float_attr = float_attr;
    }

    Token(ETokenId id, double double_attr) {
        m_id = id;
        m_double_attr = double_attr;
    }

    Token(ETokenId id, std::string str_attr) {        
        m_id = id;
        m_str_attr = (char*)malloc(sizeof(char)*str_attr.size());
        strcpy(m_str_attr, str_attr.c_str());
    }

    std::string to_string() {        
        char output_str[250];
        sprintf(
            output_str, 
            "[%d, %s]",
            m_id, m_str_attr
        );
        return std::string(output_str);
    }
};

#endif