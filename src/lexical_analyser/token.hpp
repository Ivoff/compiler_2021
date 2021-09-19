#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum token_id {
    IDENT,
    INT,
    REAL,
    REL_OP,
    ARITH_OP,
    SYMBOL
};

struct Token {
    int m_id;
    union {
        int m_int_attr;
        float m_float_attr;
        double m_double_attr;        
        std::string m_str_attr;        
    };

    Token(int id, int int_attr) {
        m_id = id;
        m_int_attr = int_attr;
    }

    Token(int id, std::string str_attr) {
        m_id = id;
        m_str_attr = str_attr;
    }

    Token(int id, float float_attr) {
        m_id = id;
        m_float_attr = float_attr;
    }

    Token(int id, double double_attr) {
        m_id = id;
        m_double_attr = double_attr;
    }
};

#endif