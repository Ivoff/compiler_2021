#ifndef PARSE_TABLE_HPP
#define PARSE_TABLE_HPP

#include <unordered_map>
#include <string>
#include <vector>

struct ParseTable {
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> m_table {
        {
            "<programa>",
            {
                {
                    "program",
                    {"program", "ident", "<corpo>", "."}
                }                
            }        
        },
        {
            "<corpo>",
            {
                {
                    "real",
                    {"<dc>", "begin", "<comandos>", "end"}
                },
                {
                    "integer",
                    {"<dc>", "begin", "<comandos>", "end"}
                },
                {
                    "begin",
                    {"<dc>", "begin", "<comandos>", "end"}
                }
            }
        },
        {
            "<dc>",
            {
                {
                    "real",
                    {"<dc_v>", "<mais_dc>"}
                },
                {
                    "integer",
                    {"<dc_v>", "<mais_dc>"}
                },
                {
                    ";",
                    {"&"}
                },
                {
                    "begin",
                    {"&"}
                }
            }
        },
        {
            "<mais_dc>",
            {
                {
                    ";",
                    {";", "<dc>"}
                },                
                {
                    "begin",
                    {"&"}
                }
            }
        },
        {
            "<dc_v>",
            {
                {
                    "real",                
                    {"<tipo_var>", ":", "<variaveis>"}
                },
                {
                    "integer",                
                    {"<tipo_var>", ":", "<variaveis>"}                   
                }
            }
        },
        {
            "<tipo_var>",
            {
                {
                    "real",
                    {"real"}
                },
                {
                    "integer",
                    {"integer"}
                }
            }
        },
        {
            "<variaveis>",
            {
                {
                    "ident",
                    {"ident", "<mais_var>"}
                }
            }
        },
        {
            "<mais_var>",
            {
                {
                    ";",
                    {"&"}
                },
                {
                    ",",
                    {",", "<variaveis>"}
                },
                {
                    "begin",
                    {"&"}
                }
            }
        },
        {
            "<comandos>",
            {
                {
                    "read",                
                    {"<comando>", "<mais_comandos>"}
                },
                {
                    "write",
                    {"<comando>", "<mais_comandos>"}
                },
                {
                    "if",
                    {"<comando>", "<mais_comandos>"}
                },
                {
                    "ident",
                    {"<comando>", "<mais_comandos>"}
                }            
            }
        },
        {
            "<mais_comandos>",
            {
                {
                    "end",
                    {"&"}
                },
                {
                    ";",
                    {";", "<comandos>"}
                },
                {
                    "$",
                    {"&"}
                },
                {
                    "else",
                    {"&"}
                }
            }
        },
        {
            "<comando>",
            {
                {
                    "read",
                    {"read", "(", "ident", ")"}
                },
                {
                    "write",
                    {"write", "(", "ident", ")"}
                },
                {
                    "if",
                    {"if", "<condicao>", "then", "<comando>", "<pfalsa>", "$"}
                },
                {
                    "ident",
                    {"ident", ":=", "<expressao>"}
                }
            }
        },
        {
            "<condicao>",
            {
                {
                    "(",
                    {"<expressao>", "<relacao>", "<expressao>"}
                },
                {
                    "ident",
                    {"<expressao>", "<relacao>", "<expressao>"}
                },
                {
                    "-",
                    {"<expressao>", "<relacao>", "<expressao>"}
                },
                {
                    "numero_int",
                    {"<expressao>", "<relacao>", "<expressao>"}
                },
                {
                    "numero_real",
                    {"<expressao>", "<relacao>", "<expressao>"}
                }
            }
        },
        {
            "<relacao>",
            {
                {
                    "=",
                    {"="}
                },
                {
                    "<>",
                    {"<>"}
                },
                {
                    ">=",
                    {">="}
                },
                {
                    "<=",
                    {"<="}
                },
                {
                    ">",
                    {">"}
                },
                {
                    "<",
                    {"<"}
                }
            }
        },
        {
            "<expressao>",
            {
                {
                    "(",
                    {"<termo>", "<outros_termos>"}
                },
                {
                    "ident",
                    {"<termo>", "<outros_termos>"}
                },
                {
                    "-",
                    {"<termo>", "<outros_termos>"}
                },
                {
                    "numero_int",
                    {"<termo>", "<outros_termos>"}
                },
                {
                    "numero_real",
                    {"<termo>", "<outros_termos>"}
                }
            }
        },
        {
            "<termo>",
            {
                {
                    "(",
                    {"<op_un>", "<fator>", "<mais_fatores>"}
                },
                {
                    "ident",
                    {"<op_un>", "<fator>", "<mais_fatores>"}
                },
                {
                    "-",
                    {"<op_un>", "<fator>", "<mais_fatores>"}
                },
                {
                    "numero_int",
                    {"<op_un>", "<fator>", "<mais_fatores>"}
                },
                {
                    "numero_real",
                    {"<op_un>", "<fator>", "<mais_fatores>"}
                }
            }
        },
        {
            "<op_un>",
            {
                {
                    "(",
                    {"&"}
                },
                {
                    "ident",
                    {"&"}
                },
                {
                    "-",
                    {"-"}
                },
                {
                    "numero_int",
                    {"&"}
                },
                {
                    "numero_real",
                    {"&"}
                },
                {
                    "(",
                    {"(", "<expressao>", ")"}
                },
                {
                    "ident",
                    {"ident"}
                },
                {
                    "numero_int",
                    {"numero_int"}
                },
                {
                    "numero_real",
                    {"numero_real"}
                }
            }
        },
        {
            "<fator>",
            {
                {
                    "(",
                    {"(", "<expressao>", ")"}
                },
                {
                    "ident",
                    {"ident"}
                },
                {
                    "numero_int",
                    {"numero_int"}
                },
                {
                    "numero_real",
                    {"numero_real"}
                }
            }            
        },
        {
            "<outros_termos>",
            {
                {
                    "end",
                    {"&"}
                },
                {
                    ")",
                    {"&"}
                },
                {
                    "then",
                    {"&"}
                },
                {
                    "$",
                    {"&"}
                },
                {
                    "=",
                    {"&"}
                },
                {
                    "<>",
                    {"&"}
                },
                {
                    ">=",
                    {"&"}
                },
                {
                    "<=",
                    {"&"}
                },
                {
                    ">",
                    {"&"}
                },
                {
                    "<",
                    {"&"}
                },
                {
                    "-",
                    {"<op_ad>", "<termo>", "<outros_termos>"}
                },
                {
                    "+",
                    {"<op_ad>", "<termo>", "<outros_termos>"}
                },
                {
                    "else",
                    {"&"}
                },
                {
                    ";",
                    {"&"}
                }
            }
        },
        {
            "<op_ad>",
            {
                {
                    "-",
                    {"-"}
                },
                {
                    "+",
                    {"+"}
                }
            }
        },
        {
            "<mais_fatores>",
            {
                {
                    "end",
                    {"&"}
                },
                {
                    ")",
                    {"&"}
                },
                {
                    ";",
                    {"&"}
                },
                {
                    "then",
                    {"&"}
                },
                {
                    "$",
                    {"&"}
                },
                {
                    "=",
                    {"&"}
                },
                {
                    "<>",
                    {"&"}
                },
                {
                    ">=",
                    {"&"}
                },
                {
                    "<=",
                    {"&"}
                },
                {
                    ">",
                    {"&"}
                },
                {
                    "<",
                    {"&"}
                },
                {
                    "-",
                    {"&"}
                },
                {
                    "+",
                    {"&"}
                },
                {
                    "*",
                    {"<op_mul>", "<fator>", "<mais_fatores>"}
                },
                {
                    "/",
                    {"<op_mul>", "<fator>", "<mais_fatores>"}
                },
                {
                    "else",
                    {"&"}
                }
            }
        },
        {
            "<op_mul>",
            {
                {
                    "*",
                    {"*"}
                },
                {
                    "/",
                    {"/"}
                }
            }
        },
        {
            "<pfalsa>",
            {
                {
                    "$",
                    {"&"}
                },
                {
                    "else",
                    {"else", "<comandos>"}
                }
            }
        }
    };
    ParseTable(){};
};

#endif