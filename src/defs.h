#ifndef DEFS_H
#define DEFS_H

#define MAX_REGIONAL_LEXERS_SIZE 32
#define MAX_LINE_LEN 256
#define MAX_SOURCE 200000
#define MAX_TOKEN_LEN 256
#define MAX_FILE 32
#define MAX_PATH_LEN 32

typedef struct {
    int file_idx;
    int line;
    int col;
} location_t;

int a = 0;

#endif
