#ifndef LEXER_C
#define LEXER_C

typedef enum {
    T_start,
    T_identifier
} token_type_t;

typedef struct token_t token_t;

struct token_t {
    token_type_t typ;
    token_t *next;
};

typedef struct {
    int capacity;
    int size;
    token_t *data;
} token_arena_t;

token_arena_t *arena_init(int capacity) {
    token_arena_t *arena = malloc(sizeof(token_arena_t));
    arena->capacity = capacity;
    arena->size = 0;
    arena->data = calloc(capacity, sizeof(token_t));
    return arena;
}

token_t *alloc_token(token_arena_t *arena, int size) {
    token_t *data = &arena->data[arena->size], *cur = data;
    for (int i = 1; i < size; i++) {
        cur->next = &arena->data[arena->size + i];
        cur = cur->next;
    }
    cur->next = 0;
    arena->size += size;
    return data;
}

void arena_free(token_arena_t **arena) {
    token_t *data = arena[0]->data;
    free(data);
    free(arena[0]);
}

#endif
