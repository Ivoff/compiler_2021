#ifndef SCANNER_CPP
#define SCANNER_CPP

#include <stdio.h>
#include <ctype.h>
#include <exception>
#include <cmath>

#include "scanner.hpp"

Scanner::Scanner(std::string file_path) {
    m_file_size = 0;
    m_buffer_pos = 0;
    m_current_line = 1;
    m_state = 0;
    char curr_char;

    FILE* handle = fopen(file_path.c_str(), "r");
    while((curr_char = getc(handle)) != EOF) {
        m_file_size += 1;
    }
    m_file_size += 1;
    m_input_buffer = (char* )calloc(sizeof(char), m_file_size);
    fseek(handle, 0, SEEK_SET);
    for(int i = 0; (curr_char = getc(handle)) != EOF && i < m_file_size; i += 1) {
        m_input_buffer[i] = curr_char;
    }    
};

bool Scanner::is_digit(char input) {
    return isdigit(input);
};

bool Scanner::is_letter(char input) {
    return isalpha(input);
};

bool Scanner::is_space(char input) {
    return input != '\n' && isspace(input);
};

bool Scanner::is_newline(char input) {
    return input == '\n';
};

bool Scanner::is_reserved(std::string input) {    
    return m_reserved_set.find(input) != m_reserved_set.end();    
};

bool Scanner::is_symbol(char input) {
    return m_symbols_set.find(input) != m_symbols_set.end();
};

bool Scanner::is_symbol(std::string input) {
    return input.length() == 1 && is_symbol(input[0]);
};

bool Scanner::is_cond(std::string input) {
    return m_cond_map.find(input) != m_cond_map.end();
}

bool Scanner::is_op(std::string input) {
    return m_op_map.find(input) != m_op_map.end();
}

bool Scanner::is_op(char input) {
    std::string aux;
    aux += input;
    return m_op_map.find(aux) != m_op_map.end();
}

bool Scanner::is_eof() {
    return m_buffer_pos == m_file_size;
}

std::string Scanner::error_message(std::string lexem) {
    return std::to_string(m_current_line) + ": " +
            "Início do lexema \"" + lexem + "\" " +
            "não reconhecido";
}

char Scanner::next_char() {
    return m_input_buffer[m_buffer_pos++];
};

char Scanner::regress() {
    return m_input_buffer[--m_buffer_pos];
}

double Scanner::str_to_real(std::string input) {
    size_t dot_index = (char)input.find('.');
    int acc = 1;
    int fraction_part = 0;
    int integer_part = 0;
    int denominator;

    for(int i = input.size()-1; i > dot_index; i -= 1) {
        fraction_part += (input[i]-'0') * acc;
        acc *= 10;
    }

    acc = 1;
    for(int i = dot_index-1; i >= 0; i -= 1) {
        integer_part += (input[i]-'0') * acc;
        acc *= 10;
    }

    return integer_part + fraction_part / std::pow(10, (int)std::log10(fraction_part) + 1);
}

int Scanner::str_to_int(std::string input) {    
    int acc = 1;
    int integer_part = 0;

    for(int i = input.size()-1; i >= 0; i -= 1) {
        integer_part += (input[i]-'0') * acc;
        acc *= 10;
    }

    return integer_part;
}

void Scanner::split_sci_not(const std::string& input, int e_pos, double& coef, int& exp) {
    if (e_pos != std::string::npos) {
        coef = input.find('.') != std::string::npos ? 
            str_to_real(input.substr(0, e_pos)) : 
            str_to_int(input.substr(0, e_pos));

        exp = str_to_int(input.substr(e_pos + 2));
    }
}

Token* Scanner::next_token() {
    if (is_eof()) {
        return nullptr;
    }

    char curr_char;    
    std::string lexem;
    while(true) {
        curr_char = next_char();        
        switch(m_state) {
            case 0:                
                if (is_eof()) {
                    return nullptr;                                
                }
                if (is_space(curr_char)) {
                    m_state = 0;          
                    break;
                }      
                else if (is_newline(curr_char)) {
                    m_current_line++;
                    m_state = 0;
                    break;
                }
                else if (is_letter(curr_char)) {
                    lexem += curr_char;
                    m_state = 1;
                    break;
                }
                else if(is_digit(curr_char)) {
                    lexem += curr_char;
                    m_state = 3;
                    break;
                } 
                else if (is_symbol(curr_char)) {
                    lexem += curr_char;
                    if (curr_char == ':') {                        
                        m_state = 12;
                        break;
                    }
                    else {
                        m_state = 11;
                        break;
                    }
                }
                else if (is_op(curr_char)) {
                    lexem += curr_char;
                    if (curr_char != '<' && curr_char != '>') {
                        m_state = 13;
                        break;
                    } 
                    else if (curr_char == '>') {
                        m_state = 14;
                        break;
                    }
                    else if (curr_char == '<') {
                        m_state = 16;
                        break;
                    } 
                    else {
                        throw std::runtime_error("Tem algo de errado no reconhecimento de operadores");
                    }
                }

                lexem += curr_char;
                throw std::runtime_error(error_message(lexem));
            break;
            case 1:
                if (is_letter(curr_char) || is_digit(curr_char) || curr_char == '_') {
                    lexem += curr_char;
                    m_state = 1;
                    break;
                } else {                    
                    m_state = 0;
                    regress();
                    
                    if (is_reserved(lexem))
                        return new Token(ETokenId::RESERVED, lexem, lexem);                    
                    
                    if (is_cond(lexem))
                        return new Token(m_cond_map[lexem], lexem, lexem);                                        
                    
                    else
                        return new Token(ETokenId::IDENT, lexem, "ident");                   
                }
            break;
            case 3:
                if (is_digit(curr_char)) {
                    lexem += curr_char;
                    m_state = 3;
                    break;
                }
                else if (curr_char == '.') {
                    lexem += curr_char;
                    m_state = 4;
                    break;
                }
                else if (curr_char == 'E') {
                    lexem += curr_char;
                    m_state = 8;
                    break;
                } else {
                    m_state = 0;
                    regress();
                    return new Token(ETokenId::NUMBER_INT, str_to_int(lexem), "numero_int");
                }
            break;
            case 4:
                if (is_digit(curr_char)) {
                    lexem += curr_char;
                    m_state = 4;
                    break;
                }
                else if (curr_char == 'E') {
                    lexem += curr_char;
                    m_state = 5;
                    break;
                } 
                else {
                    m_state = 0;
                    regress();
                    return new Token(ETokenId::NUMBER_REAL, str_to_real(lexem), "numero_real");
                }
            break;
            case 5:
                if (curr_char == '+' || curr_char == '-') {
                    lexem += curr_char;
                    m_state = 6;
                    break;
                }
                
                throw std::runtime_error(error_message(lexem+curr_char));
            break;
            case 6:
                if (is_digit(curr_char)) {
                    lexem += curr_char;
                    m_state = 6;
                    break;
                } else {
                    m_state = 0;
                    regress();

                    int e_pos = lexem.find('E');
                    if (e_pos != std::string::npos) {                        
                        double coef;
                        int exp;

                        split_sci_not(lexem, e_pos, coef, exp);

                        if (lexem[e_pos + 1] == '+') {
                            return new Token(ETokenId::NUMBER_REAL, std::pow(coef, exp), "numero_real");
                        } 
                        else {
                            return new Token(ETokenId::NUMBER_REAL, std::pow(coef, -exp), "numero_real");
                        }
                    }
                    
                    return new Token(ETokenId::NUMBER_REAL, str_to_real(lexem), "numero_real");
                }
            break;
            case 8:
                if (curr_char == '+' || curr_char == '-') {                    
                    lexem += curr_char;
                    m_state = 9;
                    break;
                }

                throw std::runtime_error(error_message(lexem+curr_char));
            break;
            case 9:
                if (is_digit(curr_char)) {
                    lexem += curr_char;
                    m_state = 9;
                    break;
                } else {
                    m_state = 0;
                    regress();

                    int e_pos = lexem.find('E');
                    if (e_pos != std::string::npos) {
                        double coef;
                        int exp;

                        split_sci_not(lexem, e_pos, coef, exp);

                        if (lexem[e_pos + 1] == '+') {                                              
                            return new Token(ETokenId::NUMBER_INT, (int)std::pow(coef, exp), "numero_int");
                        } 
                        else {
                            return new Token(ETokenId::NUMBER_INT, (int)std::pow(coef, -exp), "numero_int");
                        }
                    }

                    return new Token(ETokenId::NUMBER_INT, str_to_int(lexem), "numero_int");
                }                
            break;
            case 12:                
                if (curr_char == '=') {
                    lexem += '=';
                    m_state = 13;                    
                    break;
                } else {
                    m_state = 0;
                    regress();                    
                    
                    return new Token(ETokenId::SYMBOL, lexem, lexem);
                }                
            break;
            case 13:
                regress();
                m_state = 0;

                if (is_op(lexem))
                    return new Token(m_op_map[lexem], lexem, lexem);                

                throw std::runtime_error(error_message(lexem));
            break;
            case 14:                
                if (curr_char == '=') {
                    lexem += curr_char;
                    m_state = 13;
                    break;
                } else {
                    m_state = 0;
                    regress();

                    return new Token(m_op_map[lexem], lexem, lexem);
                }
            break;
            case 16:
                if (curr_char == '=' || curr_char == '>') {                    
                    lexem += curr_char;
                    m_state = 13;
                    break;
                } else {
                    m_state = 0;
                    regress();

                    return new Token(m_op_map[lexem], lexem, lexem);                  
                }
            break;
            case 11:
                m_state = 0;
                regress();

                if (is_symbol(lexem))
                    return new Token(ETokenId::SYMBOL, lexem, lexem);
                else
                    throw std::runtime_error(error_message(lexem));
            break;
        }
    }
};

#endif