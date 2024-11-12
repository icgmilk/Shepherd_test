#include <stdio.h>
#include <stdlib.h>
#include "globals.c"
#include "lexer.c"

int main()
{
    init_globals();
    int token_count = 0;
    lexer_t *lexer = lexer_init("test_suite/alias.c");
    token_type_t typ;

    typ = lexer_next_token(lexer);

    while (typ != T_eof) {
        token_t *tk = lexer_cur_token(lexer);
        printf("[%d]: %s\n", token_count++, tk->literal);
        typ = lexer_next_token(lexer);
    }

    lexer_free(lexer);
    free_globals();
    return 0;
}
