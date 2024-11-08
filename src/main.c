#include <stdio.h>
#include "lexer.c"

int main()
{
    token_arena_t *arena = arena_init(1024);
    token_t *a = alloc_token(arena, 2);
    printf("%d\n", a[0].next);
    printf("%d\n", a[1].next);
    arena_free(&arena);
    
    printf("Hello World\n");
    return 0;
}
