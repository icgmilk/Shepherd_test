#include <stdlib.h>
#include <stdio.h>
#include "globals.c"
#include "lexer.c"

int main()
{
    lexer_t *lexer = lexer_init("test_suite/alias.c");

    reg_lexer_next_token(lexer->global_lexer);

    while (true) {
        printf("%s\n", lexer->global_lexer->cur_token->literal);
        reg_lexer_next_token(lexer->global_lexer);
    }

    lexer_free(lexer);
    return 0;
}
