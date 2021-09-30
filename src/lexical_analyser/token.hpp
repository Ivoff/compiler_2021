#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <string.h>
#include <unordered_map>

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
    std::string m_parse_key;
    union {
        bool m_has_not_attr;
        int m_int_attr;
        float m_float_attr;
        double m_real_attr;
        char* m_str_attr;
    };
    
    Token(){};

    Token(ETokenId id, std::string parse_key) {
        m_id = id;
        m_has_not_attr = true;
        m_parse_key = parse_key;
    }

    Token(ETokenId id, int int_attr, std::string parse_key) {
        m_id = id;
        m_int_attr = int_attr;
        m_parse_key = parse_key;
    }

    Token(ETokenId id, float float_attr, std::string parse_key) {
        m_id = id;
        m_float_attr = float_attr;
        m_parse_key = parse_key;
    }

    Token(ETokenId id, double double_attr, std::string parse_key) {
        m_id = id;
        m_real_attr = double_attr;
        m_parse_key = parse_key;
    }

    Token(ETokenId id, std::string str_attr, std::string parse_key) {        
        m_id = id;
        m_str_attr = (char*)malloc(sizeof(char)*str_attr.size());
        strcpy(m_str_attr, str_attr.c_str());
        m_parse_key = parse_key;
    }

    std::string id_to_str() {
        std::unordered_map<ETokenId, std::string> mapper {
            {ETokenId::IDENT, "IDENT"},
            {ETokenId::NUMBER_INT, "NUMBER_INT"},
            {ETokenId::NUMBER_REAL, "NUMBER_REAL"},
            {ETokenId::EQUAL_OP, "EQUAL_OP"},
            {ETokenId::DIFF_OP, "DIFF_OP"},
            {ETokenId::LESS_OP, "LESS_OP"},
            {ETokenId::GREATER_OP, "GREATER_OP"},
            {ETokenId::LESS_EQUAL_OP, "LESS_EQUAL_OP"},
            {ETokenId::GREATER_EQUAL_OP, "GREATER_EQUAL_OP"},
            {ETokenId::ASSIGNMENT_OP, "ASSIGNMENT_OP"},
            {ETokenId::ADD_OP, "ADD_OP"},
            {ETokenId::SUB_OP, "SUB_OP"},
            {ETokenId::MULT_OP, "MULT_OP"},
            {ETokenId::DIV_OP, "DIV_OP"},
            {ETokenId::IF, "IF"},
            {ETokenId::THEN, "THEN"},
            {ETokenId::ELSE, "ELSE"},
            {ETokenId::SYMBOL, "SYMBOL"},
            {ETokenId::RESERVED, "RESERVED"},
            {ETokenId::NULLABLE, "NULLABLE"}
        };

        return mapper[m_id];
    };

    std::string to_string() {
        char output_str[300];
        
        if (m_id == ETokenId::NUMBER_INT) {
            sprintf(
                output_str, 
                "[%s, %d, %s]",
                id_to_str().c_str(), m_int_attr, m_parse_key.c_str()
            );
        }
        else if (m_id == ETokenId::NUMBER_REAL) {
            sprintf(
                output_str, 
                "[%s, %lf, %s]",
                id_to_str().c_str(), m_real_attr, m_parse_key.c_str()
            );
        } 
        else {
            sprintf(
                output_str, 
                "[%s, %s, %s]",
                id_to_str().c_str(), m_str_attr, m_parse_key.c_str()
            );
        }

        return std::string(output_str);
    }

    std::string lexem_to_str() {
        if (m_id == ETokenId::NUMBER_INT) {
            return std::to_string(m_int_attr);
        }
        else if (m_id == ETokenId::NUMBER_REAL) {
            return std::to_string(m_real_attr);
        } 
        else {
            return std::string(m_str_attr);
        }
    }
};

#endif