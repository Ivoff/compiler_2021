#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class ETokenId {
    IDENT = 0,
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

union UAttribute { 
    bool has_not_attr;
    int int_attr;
    float float_attr;
    double double_attr;
    std::string str_attr;
    ~UAttribute() {};
    UAttribute() {};
};

struct Token {
    ETokenId m_id;
    UAttribute m_attr;

    Token(ETokenId id) {
        m_id = id;    
        m_attr.has_not_attr = true;    
    }

    Token(ETokenId id, int int_attr) {
        m_id = id;
        m_attr.int_attr = int_attr;
    }

    Token(ETokenId id, float float_attr) {
        m_id = id;
        m_attr.float_attr = float_attr;
    }

    Token(ETokenId id, double double_attr) {
        m_id = id;
        m_attr.double_attr = double_attr;
    }

    Token(ETokenId id, std::string str_attr) {
        m_id = id;
        m_attr.str_attr = str_attr;
    }        
};

#endif