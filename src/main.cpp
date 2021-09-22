#include <stdio.h>
#include "lexical_analyser/scanner.hpp"

int main() {

    Scanner scanner = Scanner("input");

    Token* cur_token;
    while((cur_token = scanner.next_token()) != nullptr) {
        printf("%s\n", cur_token->to_string().c_str());
    }

    return 0;
}