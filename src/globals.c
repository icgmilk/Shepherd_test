#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "defs.h"

int FILE_MAP_LEN = 0;
char *file_names[MAX_FILE];
char *sources[MAX_FILE];

int file_map_add_entry(char *file_path, char **source_ref, int *len_ref) {
    char buffer[MAX_LINE_LEN], source = calloc(MAX_SOURCE, sizeof(char));
    int length = 0;
    FILE *f = fopen(file_path, "rb");

    if (!f) {
        printf("[%s] Error: Failed to read file\n", file_path);
        exit(1);
    }

    for (;;) {
        if (!fgets(buffer, MAX_LINE_LEN, f))
            break;

        strcpy(source + length, buffer);
        length += strlen(buffer);
    }
    fclose(f);
    
    file_names[FILE_MAP_LEN] = calloc(strlen(file_path) + 1, sizeof(char));
    strcpy(file_names[FILE_MAP_LEN], file_path);
    sources[FILE_MAP_LEN++] = source;

    source_ref[0] = source;
    len_ref[0] = length;

    return FILE_MAP_LEN - 1;
}

void init_globals() {
}

void error(char *str, location_t *location, ...)
{
    /* sprintf logic copied from c.c, not boundary check */
    char ERR_MSG_BUF[100];
    int *var_args = &str + 8;
    int si = 0, bi = 0, pi = 0;

    while (str[si]) {
        if (str[si] != '%') {
            ERR_MSG_BUF[bi] = str[si];
            bi++;
            si++;
        } else {
            int w = 0, zp = 0, pp = 0;

            si++;
            if (str[si] == '#') {
                pp = 1;
                si++;
            }
            if (str[si] == '0') {
                zp = 1;
                si++;
            }
            if (str[si] >= '1' && str[si] <= '9') {
                w = str[si] - '0';
                si++;
                if (str[si] >= '0' && str[si] <= '9') {
                    w = w * 10;
                    w += str[si] - '0';
                    si++;
                }
            }
            switch (str[si]) {
            case 37: /* % */
                ERR_MSG_BUF[bi++] = '%';
                si++;
                continue;
            case 99: /* c */
                ERR_MSG_BUF[bi++] = var_args[pi];
                break;
            case 115: /* s */
                strcpy(ERR_MSG_BUF + bi, var_args[pi]);
                bi += strlen(var_args[pi]);
                break;
            case 111: /* o */
                bi += __format(ERR_MSG_BUF + bi, var_args[pi], w, zp, 8, pp);
                break;
            case 100: /* d */
                bi += __format(ERR_MSG_BUF + bi, var_args[pi], w, zp, 10, 0);
                break;
            case 120: /* x */
                bi += __format(ERR_MSG_BUF + bi, var_args[pi], w, zp, 16, pp);
                break;
            default:
                abort();
                break;
            }
            pi++;
            si++;
        }
    }
    ERR_MSG_BUF[bi] = 0;

    printf("[%s:%d:%d] Error: %s\n", file_names[location->file_idx], location->line, location->col, ERR_MSG_BUF);
    exit(1);
}
