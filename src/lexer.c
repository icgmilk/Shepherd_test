#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

typedef enum {
    T_numeric,
    T_identifier,
    T_comma,  /* , */
    T_string, /* null-terminated string */
    T_char,
    T_open_bracket,  /* ( */
    T_close_bracket, /* ) */
    T_open_curly,    /* { */
    T_close_curly,   /* } */
    T_open_square,   /* [ */
    T_close_square,  /* ] */
    T_asterisk,      /* '*' */
    T_divide,        /* / */
    T_mod,           /* % */
    T_bit_or,        /* | */
    T_bit_xor,       /* ^ */
    T_bit_not,       /* ~ */
    T_log_and,       /* && */
    T_log_or,        /* || */
    T_log_not,       /* ! */
    T_lt,            /* < */
    T_gt,            /* > */
    T_le,            /* <= */
    T_ge,            /* >= */
    T_lshift,        /* << */
    T_rshift,        /* >> */
    T_dot,           /* . */
    T_arrow,         /* -> */
    T_plus,          /* + */
    T_minus,         /* - */
    T_minuseq,       /* -= */
    T_pluseq,        /* += */
    T_oreq,          /* |= */
    T_andeq,         /* &= */
    T_eq,            /* == */
    T_noteq,         /* != */
    T_assign,        /* = */
    T_increment,     /* ++ */
    T_decrement,     /* -- */
    T_question,      /* ? */
    T_colon,         /* : */
    T_semicolon,     /* ; */
    T_eof,           /* end-of-file (EOF) */
    T_ampersand,     /* & */
    T_return,
    T_if,
    T_else,
    T_while,
    T_for,
    T_do,
    T_typedef,
    T_enum,
    T_struct,
    T_sizeof,
    T_elipsis, /* ... */
    T_switch,
    T_case,
    T_break,
    T_default,
    T_continue,
    /* C pre-processor directives */
    T_cppd_hash,
    T_cppd_hashhash,
    T_cppd_include,
    T_cppd_define,
    T_cppd_undef,
    T_cppd_error,
    T_cppd_if,
    T_cppd_elif,
    T_cppd_else,
    T_cppd_endif,
    T_cppd_ifdef,
    T_cppd_ifndef
} token_type_t;

typedef struct token_t token_t;

struct token_t {
    token_type_t typ;
    location_t loc;
    char literal[MAX_TOKEN_LEN];
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

void arena_free(token_arena_t *arena) {
    token_t *data = arena->data;
    free(data);
    free(arena);
}

typedef enum {
    LM_source,
    LM_token
} lexer_mode_t;

typedef struct {
    token_arena_t *arena;
    int file_idx;
    lexer_mode_t mode;
    char source[MAX_LINE_LEN];
    int source_len;
    int pos;
    int line;
    int col;
    token_t *cur_token;
    bool skip_backslash_newline;
} regional_lexer_t;

regional_lexer_t *reg_lexer_source_init(token_arena_t *arena, char *source, int source_len, int file_idx) {
    regional_lexer_t *lexer = malloc(sizeof(regional_lexer_t));
    lexer->arena = arena;
    lexer->file_idx = file_idx;
    lexer->mode = LM_source;
    strcpy(lexer->source, source);
    lexer->source_len = source_len;
    lexer->pos = 0;
    lexer->line = 1;
    lexer->col = 1;
    lexer->cur_token = NULL;
    lexer->skip_backslash_newline = true;
    return lexer;
}

/* Utility Functions for identifying characters */
bool is_white_space(char ch) {
    return ch == ' ' || ch == '\t';
}

bool is_newline(char ch) {
    return ch == '\n' || ch == '\r';
}

bool is_alpha(char ch) {
    return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z';
}

bool is_digit(char ch) {
    return ch >= '0' && ch <= '9';
}

bool is_alnum(char ch) {
    return is_alpha(ch) || is_digit(ch);
}

bool is_identifier_start(char ch) {
    return is_alpha(ch) || ch == '_';
}

bool is_identifier(char ch) {
    return is_alnum(ch) || ch == '_';
}

/* Copies location data to loc_ref */
location_t *reg_lexer_cur_loc(regional_lexer_t *lexer, location_t *loc_ref) {
    if (!loc_ref)
        return loc_ref;

    switch (lexer->mode) {
        case LM_source: {
            loc_ref->file_idx = lexer->file_idx;
            loc_ref->col = lexer->col;
            loc_ref->line = lexer->line;
            return loc_ref;
        }
        case LM_token: {
            printf("TODO at reg_lexer_cur_loc\n");
            exit(1);
            break;
        }
    }

    return NULL;
}

/* TODO: Ineger comparison seems to be flawed, implement fail-safe algorithm later */
char reg_lexer_peek_char(regional_lexer_t *lexer, int offset) {
    return lexer->source[lexer->pos + offset];
}

void reg_lexer_read_char(regional_lexer_t *lexer, int len) {
    lexer->pos += len;
    lexer->col += len;
}

void reg_lexer_skip_white_space(regional_lexer_t *lexer) {
    char ch;

    while (true) {
        ch = reg_lexer_peek_char(lexer, 0);

        if (lexer->skip_backslash_newline && is_newline(ch)) {
            lexer->line += 1;
            lexer->col = 1;
            lexer->pos += 1;
            continue;
        }

        if (is_white_space(ch) || lexer->skip_backslash_newline && ch == '\\') {
            lexer->pos += 1;
            lexer->col += 1;
            continue;
        }

        break;
    }
}

void reg_lexer_make_token(regional_lexer_t *lexer, token_type_t typ, int len);
void reg_lexer_make_identifier_token(regional_lexer_t *lexer, int len);

void reg_lexer_next_token(regional_lexer_t *lexer) {
    char ch, token_str[MAX_TOKEN_LEN];
    int len;

    reg_lexer_skip_white_space(lexer);
    ch = reg_lexer_peek_char(lexer, 0);

    if (ch == '#') {
        if (reg_lexer_peek_char(lexer, 1) == '#') {
            reg_lexer_make_token(lexer, T_cppd_hashhash, 2);
            return;
        }

        reg_lexer_make_token(lexer, T_cppd_hash, 1);
        return;
    }

    if (is_identifier_start(ch)) {
        int len = 0;

        do {
            len++;
            ch = reg_lexer_peek_char(lexer, len);
        } while (is_identifier(ch));

        reg_lexer_make_identifier_token(lexer, len);
        return;
    }

    if (is_digit(ch)) {
        int len = 0;

        /* TODO: Handle hexadecimal numeric */
        do {
            len++;
            ch = reg_lexer_peek_char(lexer, len);
        } while (is_digit(ch));

        reg_lexer_make_token(lexer, T_numeric, len);
        return;
    }
    
    location_t loc;
    error("Unexpected character: %c", reg_lexer_cur_loc(lexer, &loc), ch);
}

void reg_lexer_make_token(regional_lexer_t *lexer, token_type_t typ, int len) {
    token_t *token = alloc_token(lexer->arena, 1);
    token->loc.line = lexer->line;
    token->loc.col = lexer->col;
    token->typ = typ;
    strncpy(token->literal, lexer->source + lexer->pos, len);
    lexer->cur_token = token;
    reg_lexer_read_char(lexer, len);
}

void reg_lexer_make_identifier_token(regional_lexer_t *lexer, int len) {
    token_t *token = alloc_token(lexer->arena, 1);
    token->loc.line = lexer->line;
    token->loc.col = lexer->col;
    strncpy(token->literal, lexer->source + lexer->pos, len);

    if (!strcmp(token->literal, "if"))
        token->typ = T_if;
    else if (!strcmp(token->literal, "while"))
        token->typ = T_while;
    else if (!strcmp(token->literal, "for"))
        token->typ = T_for;
    else if (!strcmp(token->literal, "do"))
        token->typ = T_do;
    else if (!strcmp(token->literal, "else"))
        token->typ = T_else;
    else if (!strcmp(token->literal, "return"))
        token->typ = T_return;
    else if (!strcmp(token->literal, "typedef"))
        token->typ = T_typedef;
    else if (!strcmp(token->literal, "enum"))
        token->typ = T_enum;
    else if (!strcmp(token->literal, "struct"))
        token->typ = T_struct;
    else if (!strcmp(token->literal, "sizeof"))
        token->typ = T_sizeof;
    else if (!strcmp(token->literal, "switch"))
        token->typ = T_switch;
    else if (!strcmp(token->literal, "case"))
        token->typ = T_case;
    else if (!strcmp(token->literal, "break"))
        token->typ = T_break;
    else if (!strcmp(token->literal, "default"))
        token->typ = T_default;
    else if (!strcmp(token->literal, "continue"))
        token->typ = T_continue;

    lexer->cur_token = token;
    reg_lexer_read_char(lexer, len);
}

void reg_lexer_free(regional_lexer_t *lexer) {
    /* free(lexer->source); */
    free(lexer);
}

typedef struct {
    int len;
    regional_lexer_t *lexers[MAX_REGIONAL_LEXERS_SIZE];
} regional_lexer_stack_t;

regional_lexer_stack_t *lexer_stack_init() {
    regional_lexer_stack_t *stack = malloc(sizeof(regional_lexer_stack_t));
    stack->len = 0;
    return stack;
}

void lexer_stack_push(regional_lexer_stack_t *stack, regional_lexer_t *lexer) {
    if (stack->len + 1 >= MAX_REGIONAL_LEXERS_SIZE)
        return;

    stack->lexers[stack->len++] = lexer;
}

void lexer_stack_pop(regional_lexer_stack_t *stack) {
    if (stack->len == 0)
        return;

    stack->len--;
    regional_lexer_t *lexer = stack->lexers[stack->len];
    reg_lexer_free(lexer);
}

regional_lexer_t *lexer_stack_top(regional_lexer_stack_t *stack) {
    if (stack->len == 0)
        return NULL;

    return stack->lexers[stack->len - 1];
}

typedef struct {
    token_arena_t *arena;
    regional_lexer_t *global_lexer;
    regional_lexer_stack_t *regional_lexers;
} lexer_t;

lexer_t *lexer_init(char *entry_file_path) {
    char *source;
    int length;
    int file_idx = file_map_add_entry(entry_file_path, &source, &length);

    lexer_t *lexer = malloc(sizeof(lexer_t));
    lexer->arena = arena_init(1024);
    lexer->global_lexer = reg_lexer_source_init(lexer->arena, source, length, file_idx);
    lexer->regional_lexers = lexer_stack_init();
    return lexer;
}

void lexer_free(lexer_t *lexer) {
    arena_free(lexer->arena);
    reg_lexer_free(lexer->global_lexer);
    free(lexer);
}
