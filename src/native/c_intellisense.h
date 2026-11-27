/*
 * RedPanda-C++ inspired lightweight C99 IntelliSense Engine (Pure C only — no C++)
 * Features:
 *  - Complete C89/C99 standard library catalog (functions with exact signatures, types, macros, headers)
 *  - Header-aware prioritization (#include <...>) + #include <header.h> completion
 *  - Scope-aware single-pass C parser: local variables, for-loop variables, function parameters,
 *    global variables, user functions, #define object/function macros, struct/union fields,
 *    typedef aliases, and enum constants
 *  - Member completion on '.' and '->' (including array indexing a[i]. and chained a->b.)
 *  - Live function signature CallTips with active-argument highlight range
 *  - Color-coded XPM completion icons for Scintilla
 */
#ifndef CREV_C_INTELLISENSE_H
#define CREV_C_INTELLISENSE_H

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Completion item kinds (also Scintilla XPM image IDs 1..7) */
enum {
    CI_VAR    = 1, /* Local / global variable or parameter */
    CI_FUNC   = 2, /* Function or function-like macro */
    CI_MEMBER = 3, /* Struct / union member field */
    CI_TYPE   = 4, /* Struct / union / enum / typedef / primitive type */
    CI_MACRO  = 5, /* Macro / constant / enum value */
    CI_KW     = 6, /* C keyword */
    CI_HEADER = 7  /* C standard header */
};

/* ---------- 32-bit RGBA Completion Icons (14x14 crisp colored badges for Scintilla) ---------- */
static void ci_make_badge_rgba(unsigned char out[14 * 14 * 4],
                               unsigned char r, unsigned char g, unsigned char b,
                               const char *rows[8]) {
    for (int y = 0; y < 14; y++) {
        for (int x = 0; x < 14; x++) {
            int off = (y * 14 + x) * 4;
            /* Rounded 14x14 square mask (cut 2px corners) */
            int corner = ((x == 0 || x == 13) && (y <= 1 || y >= 12)) ||
                         ((y == 0 || y == 13) && (x <= 1 || x >= 12));
            if (corner) {
                out[off] = out[off + 1] = out[off + 2] = out[off + 3] = 0;
                continue;
            }
            int is_glyph = 0;
            if (y >= 3 && y < 11 && x >= 3 && x < 11) {
                if (rows[y - 3][x - 3] == '#') is_glyph = 1;
            }
            if (is_glyph) {
                out[off] = 255; out[off + 1] = 255; out[off + 2] = 255; out[off + 3] = 255;
            } else {
                out[off] = r; out[off + 1] = g; out[off + 2] = b; out[off + 3] = 255;
            }
        }
    }
}

static void ci_get_icon_rgba(int kind, unsigned char out[14 * 14 * 4]) {
    static const char *G_VAR[8] = {
        "........",
        "#......#",
        "#......#",
        ".#....#.",
        ".#....#.",
        "..#..#..",
        "...##...",
        "........"
    };
    static const char *G_FUNC[8] = {
        "...####.",
        "..##....",
        "..##....",
        ".#####..",
        "..##....",
        "..##....",
        "..##....",
        "........"
    };
    static const char *G_MEM[8] = {
        "........",
        "#..##..#",
        "########",
        "##.##.##",
        "##.##.##",
        "##....##",
        "##....##",
        "........"
    };
    static const char *G_TYPE[8] = {
        "########",
        "########",
        "...##...",
        "...##...",
        "...##...",
        "...##...",
        "...##...",
        "........"
    };
    static const char *G_MACRO[8] = {
        "..#..#..",
        "..#..#..",
        "########",
        "..#..#..",
        "..#..#..",
        "########",
        "..#..#..",
        "........"
    };
    static const char *G_KW[8] = {
        "#....##.",
        "#...##..",
        "#..##...",
        "####....",
        "#..##...",
        "#...##..",
        "#....##.",
        "........"
    };
    static const char *G_HDR[8] = {
        "##....##",
        "##....##",
        "##....##",
        "########",
        "##....##",
        "##....##",
        "##....##",
        "........"
    };

    switch (kind) {
        case CI_VAR:    ci_make_badge_rgba(out, 5, 150, 105, G_VAR); break;
        case CI_FUNC:   ci_make_badge_rgba(out, 37, 99, 235, G_FUNC); break;
        case CI_MEMBER: ci_make_badge_rgba(out, 217, 119, 6, G_MEM); break;
        case CI_TYPE:   ci_make_badge_rgba(out, 8, 145, 178, G_TYPE); break;
        case CI_MACRO:  ci_make_badge_rgba(out, 124, 58, 237, G_MACRO); break;
        case CI_KW:     ci_make_badge_rgba(out, 71, 85, 105, G_KW); break;
        case CI_HEADER: ci_make_badge_rgba(out, 13, 148, 136, G_HDR); break;
        default:        ci_make_badge_rgba(out, 100, 116, 139, G_VAR); break;
    }
}

/* ---------- Complete C89/C99 Standard Library Catalog (Pure C Only) ---------- */
typedef struct {
    const char *name;
    const char *header;    /* NULL = keyword / builtin, or "stdio.h", "stdlib.h", etc. */
    int kind;              /* CI_FUNC, CI_TYPE, CI_MACRO, CI_KW */
    const char *sig;       /* Prototype for CallTips (functions / function-like macros) */
} CStdSymbol;

static const char *const C_STD_HEADERS[] = {
    "assert.h", "ctype.h", "errno.h", "float.h", "inttypes.h", "limits.h",
    "locale.h", "math.h", "stdbool.h", "stddef.h", "stdint.h", "stdio.h",
    "stdlib.h", "string.h", "time.h"
};
#define C_STD_HEADERS_LEN ((int)(sizeof(C_STD_HEADERS) / sizeof(C_STD_HEADERS[0])))

static const CStdSymbol C_STD_SYMBOLS[] = {
    /* C99 Keywords & Core Types */
    {"auto", NULL, CI_KW, NULL},
    {"break", NULL, CI_KW, NULL},
    {"case", NULL, CI_KW, NULL},
    {"char", NULL, CI_TYPE, NULL},
    {"const", NULL, CI_KW, NULL},
    {"continue", NULL, CI_KW, NULL},
    {"default", NULL, CI_KW, NULL},
    {"do", NULL, CI_KW, NULL},
    {"double", NULL, CI_TYPE, NULL},
    {"else", NULL, CI_KW, NULL},
    {"enum", NULL, CI_KW, NULL},
    {"extern", NULL, CI_KW, NULL},
    {"float", NULL, CI_TYPE, NULL},
    {"for", NULL, CI_KW, NULL},
    {"goto", NULL, CI_KW, NULL},
    {"if", NULL, CI_KW, NULL},
    {"inline", NULL, CI_KW, NULL},
    {"int", NULL, CI_TYPE, NULL},
    {"long", NULL, CI_TYPE, NULL},
    {"register", NULL, CI_KW, NULL},
    {"restrict", NULL, CI_KW, NULL},
    {"return", NULL, CI_KW, NULL},
    {"short", NULL, CI_TYPE, NULL},
    {"signed", NULL, CI_TYPE, NULL},
    {"sizeof", NULL, CI_FUNC, "size_t sizeof(type_or_expr)"},
    {"static", NULL, CI_KW, NULL},
    {"struct", NULL, CI_KW, NULL},
    {"switch", NULL, CI_KW, NULL},
    {"typedef", NULL, CI_KW, NULL},
    {"union", NULL, CI_KW, NULL},
    {"unsigned", NULL, CI_TYPE, NULL},
    {"void", NULL, CI_TYPE, NULL},
    {"volatile", NULL, CI_KW, NULL},
    {"while", NULL, CI_KW, NULL},
    {"_Bool", NULL, CI_TYPE, NULL},
    {"size_t", "stddef.h", CI_TYPE, NULL},
    {"ptrdiff_t", "stddef.h", CI_TYPE, NULL},
    {"NULL", "stddef.h", CI_MACRO, NULL},

    /* <stdio.h> */
    {"FILE", "stdio.h", CI_TYPE, NULL},
    {"fpos_t", "stdio.h", CI_TYPE, NULL},
    {"EOF", "stdio.h", CI_MACRO, NULL},
    {"SEEK_SET", "stdio.h", CI_MACRO, NULL},
    {"SEEK_CUR", "stdio.h", CI_MACRO, NULL},
    {"SEEK_END", "stdio.h", CI_MACRO, NULL},
    {"BUFSIZ", "stdio.h", CI_MACRO, NULL},
    {"FILENAME_MAX", "stdio.h", CI_MACRO, NULL},
    {"stdin", "stdio.h", CI_VAR, NULL},
    {"stdout", "stdio.h", CI_VAR, NULL},
    {"stderr", "stdio.h", CI_VAR, NULL},
    {"printf", "stdio.h", CI_FUNC, "int printf(const char *format, ...)"},
    {"scanf", "stdio.h", CI_FUNC, "int scanf(const char *format, ...)"},
    {"sprintf", "stdio.h", CI_FUNC, "int sprintf(char *str, const char *format, ...)"},
    {"snprintf", "stdio.h", CI_FUNC, "int snprintf(char *str, size_t size, const char *format, ...)"},
    {"sscanf", "stdio.h", CI_FUNC, "int sscanf(const char *str, const char *format, ...)"},
    {"fprintf", "stdio.h", CI_FUNC, "int fprintf(FILE *stream, const char *format, ...)"},
    {"fscanf", "stdio.h", CI_FUNC, "int fscanf(FILE *stream, const char *format, ...)"},
    {"fopen", "stdio.h", CI_FUNC, "FILE *fopen(const char *filename, const char *mode)"},
    {"freopen", "stdio.h", CI_FUNC, "FILE *freopen(const char *filename, const char *mode, FILE *stream)"},
    {"fclose", "stdio.h", CI_FUNC, "int fclose(FILE *stream)"},
    {"fflush", "stdio.h", CI_FUNC, "int fflush(FILE *stream)"},
    {"getchar", "stdio.h", CI_FUNC, "int getchar(void)"},
    {"putchar", "stdio.h", CI_FUNC, "int putchar(int ch)"},
    {"fgetc", "stdio.h", CI_FUNC, "int fgetc(FILE *stream)"},
    {"getc", "stdio.h", CI_FUNC, "int getc(FILE *stream)"},
    {"fputc", "stdio.h", CI_FUNC, "int fputc(int ch, FILE *stream)"},
    {"putc", "stdio.h", CI_FUNC, "int putc(int ch, FILE *stream)"},
    {"ungetc", "stdio.h", CI_FUNC, "int ungetc(int ch, FILE *stream)"},
    {"fgets", "stdio.h", CI_FUNC, "char *fgets(char *str, int n, FILE *stream)"},
    {"fputs", "stdio.h", CI_FUNC, "int fputs(const char *str, FILE *stream)"},
    {"puts", "stdio.h", CI_FUNC, "int puts(const char *str)"},
    {"fread", "stdio.h", CI_FUNC, "size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)"},
    {"fwrite", "stdio.h", CI_FUNC, "size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)"},
    {"fseek", "stdio.h", CI_FUNC, "int fseek(FILE *stream, long offset, int whence)"},
    {"ftell", "stdio.h", CI_FUNC, "long ftell(FILE *stream)"},
    {"rewind", "stdio.h", CI_FUNC, "void rewind(FILE *stream)"},
    {"fgetpos", "stdio.h", CI_FUNC, "int fgetpos(FILE *stream, fpos_t *pos)"},
    {"fsetpos", "stdio.h", CI_FUNC, "int fsetpos(FILE *stream, const fpos_t *pos)"},
    {"feof", "stdio.h", CI_FUNC, "int feof(FILE *stream)"},
    {"ferror", "stdio.h", CI_FUNC, "int ferror(FILE *stream)"},
    {"clearerr", "stdio.h", CI_FUNC, "void clearerr(FILE *stream)"},
    {"perror", "stdio.h", CI_FUNC, "void perror(const char *s)"},
    {"remove", "stdio.h", CI_FUNC, "int remove(const char *filename)"},
    {"rename", "stdio.h", CI_FUNC, "int rename(const char *oldname, const char *newname)"},
    {"tmpfile", "stdio.h", CI_FUNC, "FILE *tmpfile(void)"},
    {"setbuf", "stdio.h", CI_FUNC, "void setbuf(FILE *stream, char *buf)"},
    {"setvbuf", "stdio.h", CI_FUNC, "int setvbuf(FILE *stream, char *buf, int mode, size_t size)"},

    /* <stdlib.h> */
    {"EXIT_SUCCESS", "stdlib.h", CI_MACRO, NULL},
    {"EXIT_FAILURE", "stdlib.h", CI_MACRO, NULL},
    {"RAND_MAX", "stdlib.h", CI_MACRO, NULL},
    {"div_t", "stdlib.h", CI_TYPE, NULL},
    {"ldiv_t", "stdlib.h", CI_TYPE, NULL},
    {"malloc", "stdlib.h", CI_FUNC, "void *malloc(size_t size)"},
    {"calloc", "stdlib.h", CI_FUNC, "void *calloc(size_t nmemb, size_t size)"},
    {"realloc", "stdlib.h", CI_FUNC, "void *realloc(void *ptr, size_t size)"},
    {"free", "stdlib.h", CI_FUNC, "void free(void *ptr)"},
    {"atoi", "stdlib.h", CI_FUNC, "int atoi(const char *str)"},
    {"atol", "stdlib.h", CI_FUNC, "long atol(const char *str)"},
    {"atoll", "stdlib.h", CI_FUNC, "long long atoll(const char *str)"},
    {"atof", "stdlib.h", CI_FUNC, "double atof(const char *str)"},
    {"strtol", "stdlib.h", CI_FUNC, "long strtol(const char *str, char **endptr, int base)"},
    {"strtoul", "stdlib.h", CI_FUNC, "unsigned long strtoul(const char *str, char **endptr, int base)"},
    {"strtoll", "stdlib.h", CI_FUNC, "long long strtoll(const char *str, char **endptr, int base)"},
    {"strtod", "stdlib.h", CI_FUNC, "double strtod(const char *str, char **endptr)"},
    {"strtof", "stdlib.h", CI_FUNC, "float strtof(const char *str, char **endptr)"},
    {"rand", "stdlib.h", CI_FUNC, "int rand(void)"},
    {"srand", "stdlib.h", CI_FUNC, "void srand(unsigned int seed)"},
    {"abs", "stdlib.h", CI_FUNC, "int abs(int x)"},
    {"labs", "stdlib.h", CI_FUNC, "long labs(long x)"},
    {"llabs", "stdlib.h", CI_FUNC, "long long llabs(long long x)"},
    {"div", "stdlib.h", CI_FUNC, "div_t div(int numer, int denom)"},
    {"ldiv", "stdlib.h", CI_FUNC, "ldiv_t ldiv(long numer, long denom)"},
    {"qsort", "stdlib.h", CI_FUNC, "void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))"},
    {"bsearch", "stdlib.h", CI_FUNC, "void *bsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))"},
    {"exit", "stdlib.h", CI_FUNC, "void exit(int status)"},
    {"abort", "stdlib.h", CI_FUNC, "void abort(void)"},
    {"atexit", "stdlib.h", CI_FUNC, "int atexit(void (*func)(void))"},
    {"system", "stdlib.h", CI_FUNC, "int system(const char *command)"},
    {"getenv", "stdlib.h", CI_FUNC, "char *getenv(const char *name)"},

    /* <string.h> */
    {"strlen", "string.h", CI_FUNC, "size_t strlen(const char *str)"},
    {"strcpy", "string.h", CI_FUNC, "char *strcpy(char *dest, const char *src)"},
    {"strncpy", "string.h", CI_FUNC, "char *strncpy(char *dest, const char *src, size_t n)"},
    {"strcat", "string.h", CI_FUNC, "char *strcat(char *dest, const char *src)"},
    {"strncat", "string.h", CI_FUNC, "char *strncat(char *dest, const char *src, size_t n)"},
    {"strcmp", "string.h", CI_FUNC, "int strcmp(const char *s1, const char *s2)"},
    {"strncmp", "string.h", CI_FUNC, "int strncmp(const char *s1, const char *s2, size_t n)"},
    {"strchr", "string.h", CI_FUNC, "char *strchr(const char *str, int ch)"},
    {"strrchr", "string.h", CI_FUNC, "char *strrchr(const char *str, int ch)"},
    {"strstr", "string.h", CI_FUNC, "char *strstr(const char *haystack, const char *needle)"},
    {"strspn", "string.h", CI_FUNC, "size_t strspn(const char *str, const char *accept)"},
    {"strcspn", "string.h", CI_FUNC, "size_t strcspn(const char *str, const char *reject)"},
    {"strpbrk", "string.h", CI_FUNC, "char *strpbrk(const char *str, const char *accept)"},
    {"strtok", "string.h", CI_FUNC, "char *strtok(char *str, const char *delim)"},
    {"memcpy", "string.h", CI_FUNC, "void *memcpy(void *dest, const void *src, size_t n)"},
    {"memmove", "string.h", CI_FUNC, "void *memmove(void *dest, const void *src, size_t n)"},
    {"memset", "string.h", CI_FUNC, "void *memset(void *ptr, int value, size_t n)"},
    {"memcmp", "string.h", CI_FUNC, "int memcmp(const void *s1, const void *s2, size_t n)"},
    {"memchr", "string.h", CI_FUNC, "void *memchr(const void *ptr, int value, size_t n)"},
    {"strerror", "string.h", CI_FUNC, "char *strerror(int errnum)"},

    /* <math.h> */
    {"M_PI", "math.h", CI_MACRO, NULL},
    {"INFINITY", "math.h", CI_MACRO, NULL},
    {"NAN", "math.h", CI_MACRO, NULL},
    {"HUGE_VAL", "math.h", CI_MACRO, NULL},
    {"sqrt", "math.h", CI_FUNC, "double sqrt(double x)"},
    {"cbrt", "math.h", CI_FUNC, "double cbrt(double x)"},
    {"pow", "math.h", CI_FUNC, "double pow(double base, double exp)"},
    {"exp", "math.h", CI_FUNC, "double exp(double x)"},
    {"exp2", "math.h", CI_FUNC, "double exp2(double x)"},
    {"log", "math.h", CI_FUNC, "double log(double x)"},
    {"log10", "math.h", CI_FUNC, "double log10(double x)"},
    {"log2", "math.h", CI_FUNC, "double log2(double x)"},
    {"fabs", "math.h", CI_FUNC, "double fabs(double x)"},
    {"fmod", "math.h", CI_FUNC, "double fmod(double x, double y)"},
    {"ceil", "math.h", CI_FUNC, "double ceil(double x)"},
    {"floor", "math.h", CI_FUNC, "double floor(double x)"},
    {"round", "math.h", CI_FUNC, "double round(double x)"},
    {"trunc", "math.h", CI_FUNC, "double trunc(double x)"},
    {"fmax", "math.h", CI_FUNC, "double fmax(double x, double y)"},
    {"fmin", "math.h", CI_FUNC, "double fmin(double x, double y)"},
    {"hypot", "math.h", CI_FUNC, "double hypot(double x, double y)"},
    {"sin", "math.h", CI_FUNC, "double sin(double x)"},
    {"cos", "math.h", CI_FUNC, "double cos(double x)"},
    {"tan", "math.h", CI_FUNC, "double tan(double x)"},
    {"asin", "math.h", CI_FUNC, "double asin(double x)"},
    {"acos", "math.h", CI_FUNC, "double acos(double x)"},
    {"atan", "math.h", CI_FUNC, "double atan(double x)"},
    {"atan2", "math.h", CI_FUNC, "double atan2(double y, double x)"},
    {"sinh", "math.h", CI_FUNC, "double sinh(double x)"},
    {"cosh", "math.h", CI_FUNC, "double cosh(double x)"},
    {"tanh", "math.h", CI_FUNC, "double tanh(double x)"},
    {"isnan", "math.h", CI_FUNC, "int isnan(double x)"},
    {"isinf", "math.h", CI_FUNC, "int isinf(double x)"},

    /* <ctype.h> */
    {"isalpha", "ctype.h", CI_FUNC, "int isalpha(int ch)"},
    {"isdigit", "ctype.h", CI_FUNC, "int isdigit(int ch)"},
    {"isalnum", "ctype.h", CI_FUNC, "int isalnum(int ch)"},
    {"isspace", "ctype.h", CI_FUNC, "int isspace(int ch)"},
    {"islower", "ctype.h", CI_FUNC, "int islower(int ch)"},
    {"isupper", "ctype.h", CI_FUNC, "int isupper(int ch)"},
    {"isxdigit", "ctype.h", CI_FUNC, "int isxdigit(int ch)"},
    {"ispunct", "ctype.h", CI_FUNC, "int ispunct(int ch)"},
    {"isprint", "ctype.h", CI_FUNC, "int isprint(int ch)"},
    {"isgraph", "ctype.h", CI_FUNC, "int isgraph(int ch)"},
    {"iscntrl", "ctype.h", CI_FUNC, "int iscntrl(int ch)"},
    {"isblank", "ctype.h", CI_FUNC, "int isblank(int ch)"},
    {"tolower", "ctype.h", CI_FUNC, "int tolower(int ch)"},
    {"toupper", "ctype.h", CI_FUNC, "int toupper(int ch)"},

    /* <stdbool.h> */
    {"bool", "stdbool.h", CI_TYPE, NULL},
    {"true", "stdbool.h", CI_MACRO, NULL},
    {"false", "stdbool.h", CI_MACRO, NULL},

    /* <stdint.h> */
    {"int8_t", "stdint.h", CI_TYPE, NULL},
    {"uint8_t", "stdint.h", CI_TYPE, NULL},
    {"int16_t", "stdint.h", CI_TYPE, NULL},
    {"uint16_t", "stdint.h", CI_TYPE, NULL},
    {"int32_t", "stdint.h", CI_TYPE, NULL},
    {"uint32_t", "stdint.h", CI_TYPE, NULL},
    {"int64_t", "stdint.h", CI_TYPE, NULL},
    {"uint64_t", "stdint.h", CI_TYPE, NULL},
    {"intptr_t", "stdint.h", CI_TYPE, NULL},
    {"uintptr_t", "stdint.h", CI_TYPE, NULL},
    {"INT32_MAX", "stdint.h", CI_MACRO, NULL},
    {"INT32_MIN", "stdint.h", CI_MACRO, NULL},
    {"INT64_MAX", "stdint.h", CI_MACRO, NULL},
    {"INT64_MIN", "stdint.h", CI_MACRO, NULL},
    {"UINT32_MAX", "stdint.h", CI_MACRO, NULL},
    {"UINT64_MAX", "stdint.h", CI_MACRO, NULL},

    /* <limits.h> */
    {"INT_MAX", "limits.h", CI_MACRO, NULL},
    {"INT_MIN", "limits.h", CI_MACRO, NULL},
    {"UINT_MAX", "limits.h", CI_MACRO, NULL},
    {"LONG_MAX", "limits.h", CI_MACRO, NULL},
    {"LONG_MIN", "limits.h", CI_MACRO, NULL},
    {"ULONG_MAX", "limits.h", CI_MACRO, NULL},
    {"LLONG_MAX", "limits.h", CI_MACRO, NULL},
    {"LLONG_MIN", "limits.h", CI_MACRO, NULL},
    {"CHAR_BIT", "limits.h", CI_MACRO, NULL},
    {"CHAR_MAX", "limits.h", CI_MACRO, NULL},
    {"CHAR_MIN", "limits.h", CI_MACRO, NULL},
    {"UCHAR_MAX", "limits.h", CI_MACRO, NULL},
    {"SHRT_MAX", "limits.h", CI_MACRO, NULL},
    {"SHRT_MIN", "limits.h", CI_MACRO, NULL},
    {"USHRT_MAX", "limits.h", CI_MACRO, NULL},

    /* <float.h> */
    {"FLT_MAX", "float.h", CI_MACRO, NULL},
    {"FLT_MIN", "float.h", CI_MACRO, NULL},
    {"FLT_EPSILON", "float.h", CI_MACRO, NULL},
    {"DBL_MAX", "float.h", CI_MACRO, NULL},
    {"DBL_MIN", "float.h", CI_MACRO, NULL},
    {"DBL_EPSILON", "float.h", CI_MACRO, NULL},

    /* <time.h> */
    {"time_t", "time.h", CI_TYPE, NULL},
    {"clock_t", "time.h", CI_TYPE, NULL},
    {"CLOCKS_PER_SEC", "time.h", CI_MACRO, NULL},
    {"time", "time.h", CI_FUNC, "time_t time(time_t *timer)"},
    {"clock", "time.h", CI_FUNC, "clock_t clock(void)"},
    {"difftime", "time.h", CI_FUNC, "double difftime(time_t end, time_t start)"},
    {"localtime", "time.h", CI_FUNC, "struct tm *localtime(const time_t *timer)"},
    {"gmtime", "time.h", CI_FUNC, "struct tm *gmtime(const time_t *timer)"},
    {"mktime", "time.h", CI_FUNC, "time_t mktime(struct tm *timeptr)"},
    {"strftime", "time.h", CI_FUNC, "size_t strftime(char *str, size_t maxsize, const char *format, const struct tm *timeptr)"},

    /* <assert.h> & <errno.h> */
    {"assert", "assert.h", CI_FUNC, "void assert(int expression)"},
    {"errno", "errno.h", CI_VAR, NULL},
    {"EDOM", "errno.h", CI_MACRO, NULL},
    {"ERANGE", "errno.h", CI_MACRO, NULL}
};
#define C_STD_SYMBOLS_LEN ((int)(sizeof(C_STD_SYMBOLS) / sizeof(C_STD_SYMBOLS[0])))

/* ---------- Buffer Symbol Table (Parsed Live From Current C File) ---------- */
#define CI_MAX_SYMS     512
#define CI_MAX_STRUCTS   48
#define CI_MAX_FIELDS    24
#define CI_MAX_HEADERS   24
#define CI_MAX_NAME      64
#define CI_MAX_SIG      192

typedef struct {
    char name[CI_MAX_NAME];
    char type_name[CI_MAX_NAME]; /* base type e.g. "Student", "Node", "int", "Box" */
    int ptr_depth;               /* number of '*' or array decays */
} CIField;

typedef struct {
    char name[CI_MAX_NAME];      /* struct/union tag or typedef alias */
    char alias[CI_MAX_NAME];     /* optional second name (e.g. tag="Node", alias="Node") */
    CIField fields[CI_MAX_FIELDS];
    int field_count;
} CIStruct;

typedef struct {
    char name[CI_MAX_NAME];
    char type_name[CI_MAX_NAME];
    char sig[CI_MAX_SIG];
    int kind;        /* CI_VAR, CI_FUNC, CI_TYPE, CI_MACRO */
    int ptr_depth;
    int scope_depth; /* 0 = global, >0 = local block depth */
    int pos;         /* byte offset in buffer */
} CISymbol;

typedef struct {
    char headers[CI_MAX_HEADERS][32];
    int header_count;

    CIStruct structs[CI_MAX_STRUCTS];
    int struct_count;

    CISymbol syms[CI_MAX_SYMS];
    int sym_count;
} CIContext;

static int ci_is_id_start(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
static int ci_is_id_char(int c) {
    return ci_is_id_start(c) || (c >= '0' && c <= '9');
}

static int ci_is_c_keyword(const char *w) {
    static const char *const KWS[] = {
        "auto","break","case","char","const","continue","default","do","double","else",
        "enum","extern","float","for","goto","if","inline","int","long","register",
        "restrict","return","short","signed","sizeof","static","struct","switch",
        "typedef","union","unsigned","void","volatile","while","_Bool"
    };
    for (int i = 0; i < (int)(sizeof(KWS) / sizeof(KWS[0])); i++) {
        if (strcmp(w, KWS[i]) == 0) return 1;
    }
    return 0;
}

static int ci_is_type_qualifier(const char *w) {
    return strcmp(w, "const") == 0 || strcmp(w, "volatile") == 0 ||
           strcmp(w, "static") == 0 || strcmp(w, "extern") == 0 ||
           strcmp(w, "register") == 0 || strcmp(w, "auto") == 0 ||
           strcmp(w, "inline") == 0 || strcmp(w, "restrict") == 0 ||
           strcmp(w, "signed") == 0 || strcmp(w, "unsigned") == 0 ||
           strcmp(w, "short") == 0 || strcmp(w, "long") == 0;
}

static int ci_is_builtin_type(const char *w) {
    return strcmp(w, "int") == 0 || strcmp(w, "char") == 0 || strcmp(w, "float") == 0 ||
           strcmp(w, "double") == 0 || strcmp(w, "void") == 0 || strcmp(w, "long") == 0 ||
           strcmp(w, "short") == 0 || strcmp(w, "signed") == 0 || strcmp(w, "unsigned") == 0 ||
           strcmp(w, "_Bool") == 0 || strcmp(w, "bool") == 0 || strcmp(w, "size_t") == 0 ||
           strcmp(w, "FILE") == 0 || strcmp(w, "time_t") == 0 || strcmp(w, "clock_t") == 0 ||
           strcmp(w, "ptrdiff_t") == 0 || strcmp(w, "int8_t") == 0 || strcmp(w, "uint8_t") == 0 ||
           strcmp(w, "int16_t") == 0 || strcmp(w, "uint16_t") == 0 || strcmp(w, "int32_t") == 0 ||
           strcmp(w, "uint32_t") == 0 || strcmp(w, "int64_t") == 0 || strcmp(w, "uint64_t") == 0;
}

static int ci_has_header(const CIContext *ctx, const char *hdr) {
    if (!hdr) return 1;
    for (int i = 0; i < ctx->header_count; i++) {
        if (strcmp(ctx->headers[i], hdr) == 0) return 1;
    }
    /* Implicitly treat stddef.h as present if stdio/stdlib/string included */
    if (strcmp(hdr, "stddef.h") == 0) {
        return ci_has_header(ctx, "stdio.h") || ci_has_header(ctx, "stdlib.h") || ci_has_header(ctx, "string.h");
    }
    return 0;
}

static int ci_is_known_type(const CIContext *ctx, const char *w) {
    if (ci_is_builtin_type(w)) return 1;
    for (int i = 0; i < ctx->struct_count; i++) {
        if (strcmp(ctx->structs[i].name, w) == 0 ||
            (ctx->structs[i].alias[0] && strcmp(ctx->structs[i].alias, w) == 0)) return 1;
    }
    for (int i = 0; i < ctx->sym_count; i++) {
        if (ctx->syms[i].kind == CI_TYPE && strcmp(ctx->syms[i].name, w) == 0) return 1;
    }
    return 0;
}

static CIStruct *ci_find_struct(CIContext *ctx, const char *type_name) {
    if (!type_name || !type_name[0]) return NULL;
    for (int i = 0; i < ctx->struct_count; i++) {
        if (strcmp(ctx->structs[i].name, type_name) == 0 ||
            (ctx->structs[i].alias[0] && strcmp(ctx->structs[i].alias, type_name) == 0)) {
            return &ctx->structs[i];
        }
    }
    return NULL;
}

static void ci_add_symbol(CIContext *ctx, const char *name, int kind, const char *type_name,
                          int ptr_depth, const char *sig, int scope_depth, int pos) {
    if (!name || !name[0] || ci_is_c_keyword(name)) return;
    /* Update existing symbol in same scope & kind, or append */
    for (int i = ctx->sym_count - 1; i >= 0; i--) {
        if (strcmp(ctx->syms[i].name, name) == 0 && ctx->syms[i].scope_depth == scope_depth) {
            if (kind == CI_FUNC && sig && sig[0] && !ctx->syms[i].sig[0]) {
                strncpy(ctx->syms[i].sig, sig, CI_MAX_SIG - 1);
            }
            if (type_name && type_name[0] && !ctx->syms[i].type_name[0]) {
                strncpy(ctx->syms[i].type_name, type_name, CI_MAX_NAME - 1);
                ctx->syms[i].ptr_depth = ptr_depth;
            }
            return;
        }
    }
    if (ctx->sym_count >= CI_MAX_SYMS) return;
    CISymbol *s = &ctx->syms[ctx->sym_count++];
    memset(s, 0, sizeof(*s));
    strncpy(s->name, name, CI_MAX_NAME - 1);
    s->kind = kind;
    if (type_name) strncpy(s->type_name, type_name, CI_MAX_NAME - 1);
    s->ptr_depth = ptr_depth;
    if (sig) strncpy(s->sig, sig, CI_MAX_SIG - 1);
    s->scope_depth = scope_depth;
    s->pos = pos;
}

/* Strip comments & string/char literal bodies (keeping length & newlines intact for positions!) */
static char *ci_mask_code(const char *src, int len) {
    char *out = (char *)malloc((size_t)len + 1);
    if (!out) return NULL;
    int i = 0;
    while (i < len) {
        if (src[i] == '/' && i + 1 < len && src[i + 1] == '/') {
            while (i < len && src[i] != '\n') out[i++] = ' ';
        } else if (src[i] == '/' && i + 1 < len && src[i + 1] == '*') {
            out[i++] = ' '; out[i++] = ' ';
            while (i < len) {
                if (src[i] == '*' && i + 1 < len && src[i + 1] == '/') {
                    out[i++] = ' '; out[i++] = ' ';
                    break;
                }
                out[i] = (src[i] == '\n') ? '\n' : ' ';
                i++;
            }
        } else if (src[i] == '"' || src[i] == '\'') {
            char q = src[i];
            out[i++] = q;
            while (i < len) {
                if (src[i] == '\\' && i + 1 < len) {
                    out[i++] = ' ';
                    out[i] = (src[i] == '\n') ? '\n' : ' ';
                    i++;
                } else if (src[i] == q) {
                    out[i++] = q;
                    break;
                } else {
                    out[i] = (src[i] == '\n') ? '\n' : ' ';
                    i++;
                }
            }
        } else {
            out[i] = src[i];
            i++;
        }
    }
    out[len] = '\0';
    return out;
}

static void ci_skip_ws(const char *s, int len, int *p) {
    while (*p < len && (s[*p] == ' ' || s[*p] == '\t' || s[*p] == '\r' || s[*p] == '\n')) (*p)++;
}

static int ci_read_ident(const char *s, int len, int *p, char *out, int cap) {
    ci_skip_ws(s, len, p);
    if (*p >= len || !ci_is_id_start((unsigned char)s[*p])) return 0;
    int st = *p;
    while (*p < len && ci_is_id_char((unsigned char)s[*p])) (*p)++;
    int n = *p - st;
    if (n >= cap) n = cap - 1;
    memcpy(out, s + st, (size_t)n);
    out[n] = '\0';
    return 1;
}

/* Parse struct/union body inside '{ ... }' (supports nested anonymous/named structs/unions like Box.data!) */
static void ci_parse_struct_body(CIContext *ctx, CIStruct *st, const char *m, int len, int *p) {
    int depth = 1;
    while (*p < len && depth > 0) {
        ci_skip_ws(m, len, p);
        if (*p >= len) break;
        if (m[*p] == '}') { (*p)++; depth--; break; }
        if (m[*p] == ';') { (*p)++; continue; }

        char tok[CI_MAX_NAME] = {0};
        char base_type[CI_MAX_NAME] = {0};
        int sub_struct_idx = -1;

        while (ci_read_ident(m, len, p, tok, CI_MAX_NAME)) {
            if (strcmp(tok, "struct") == 0 || strcmp(tok, "union") == 0) {
                char tag[CI_MAX_NAME] = {0};
                int saved = *p;
                if (ci_read_ident(m, len, p, tag, CI_MAX_NAME)) {
                    ci_skip_ws(m, len, p);
                    if (*p < len && m[*p] == '{') {
                        /* Nested struct/union definition */
                        (*p)++;
                        if (ctx->struct_count < CI_MAX_STRUCTS) {
                            sub_struct_idx = ctx->struct_count++;
                            memset(&ctx->structs[sub_struct_idx], 0, sizeof(CIStruct));
                            strncpy(ctx->structs[sub_struct_idx].name, tag, CI_MAX_NAME - 1);
                            ci_parse_struct_body(ctx, &ctx->structs[sub_struct_idx], m, len, p);
                            strncpy(base_type, tag, CI_MAX_NAME - 1);
                        }
                        break;
                    } else {
                        strncpy(base_type, tag, CI_MAX_NAME - 1);
                        break;
                    }
                } else {
                    *p = saved;
                    ci_skip_ws(m, len, p);
                    if (*p < len && m[*p] == '{') {
                        /* Anonymous nested struct/union */
                        (*p)++;
                        if (ctx->struct_count < CI_MAX_STRUCTS) {
                            sub_struct_idx = ctx->struct_count++;
                            memset(&ctx->structs[sub_struct_idx], 0, sizeof(CIStruct));
                            _snprintf(ctx->structs[sub_struct_idx].name, CI_MAX_NAME, "__anon_%d", sub_struct_idx);
                            ci_parse_struct_body(ctx, &ctx->structs[sub_struct_idx], m, len, p);
                            strncpy(base_type, ctx->structs[sub_struct_idx].name, CI_MAX_NAME - 1);
                        }
                        break;
                    }
                }
            } else if (strcmp(tok, "enum") == 0) {
                char tag[CI_MAX_NAME] = {0};
                ci_read_ident(m, len, p, tag, CI_MAX_NAME);
                strncpy(base_type, tag[0] ? tag : "int", CI_MAX_NAME - 1);
                break;
            } else if (ci_is_type_qualifier(tok)) {
                if (!base_type[0]) strncpy(base_type, tok, CI_MAX_NAME - 1);
                continue;
            } else {
                strncpy(base_type, tok, CI_MAX_NAME - 1);
                break;
            }
        }

        if (!base_type[0]) {
            while (*p < len && m[*p] != ';' && m[*p] != '}') (*p)++;
            if (*p < len && m[*p] == ';') (*p)++;
            continue;
        }

        /* Read one or more field declarators separated by ',' until ';' */
        while (*p < len) {
            ci_skip_ws(m, len, p);
            if (*p >= len || m[*p] == ';' || m[*p] == '}') {
                if (*p < len && m[*p] == ';') (*p)++;
                break;
            }
            int ptr = 0;
            while (*p < len && (m[*p] == '*' || m[*p] == ' ' || m[*p] == '\t')) {
                if (m[*p] == '*') ptr++;
                (*p)++;
            }
            char fname[CI_MAX_NAME] = {0};
            if (ci_read_ident(m, len, p, fname, CI_MAX_NAME)) {
                if (st && st->field_count < CI_MAX_FIELDS) {
                    CIField *f = &st->fields[st->field_count++];
                    memset(f, 0, sizeof(*f));
                    strncpy(f->name, fname, CI_MAX_NAME - 1);
                    strncpy(f->type_name, base_type, CI_MAX_NAME - 1);
                    f->ptr_depth = ptr;
                }
            }
            /* Skip array dims [N], bitfields :N, etc. until ',' or ';' or '}' */
            while (*p < len && m[*p] != ',' && m[*p] != ';' && m[*p] != '}') (*p)++;
            if (*p < len && m[*p] == ',') (*p)++;
        }
    }
}

/* Parse parameter list inside '( ... )' into local scope symbols (when scope_depth > 0) */
static void ci_parse_param_list(CIContext *ctx, const char *m, int p_start, int p_end, int scope_depth) {
    int p = p_start;
    while (p < p_end) {
        ci_skip_ws(m, p_end, &p);
        if (p >= p_end) break;
        if (m[p] == ',') { p++; continue; }

        /* Find end of this parameter (top-level ',' or p_end) */
        int par = 0, seg_end = p;
        while (seg_end < p_end) {
            if (m[seg_end] == '(') par++;
            else if (m[seg_end] == ')') par--;
            else if (m[seg_end] == ',' && par == 0) break;
            seg_end++;
        }

        /* Extract base type and parameter name (handling function pointers like int (*op)(int, int)!) */
        char base_type[CI_MAX_NAME] = {0};
        char pname[CI_MAX_NAME] = {0};
        int ptr_depth = 0;
        int q = p;
        int got_base = 0;

        while (q < seg_end) {
            ci_skip_ws(m, seg_end, &q);
            if (q >= seg_end) break;
            if (m[q] == '*') { ptr_depth++; q++; continue; }
            if (m[q] == '(') {
                /* Function pointer parameter e.g. int (*op)(int, int) */
                int k = q + 1;
                ci_skip_ws(m, seg_end, &k);
                if (k < seg_end && m[k] == '*') {
                    k++;
                    ci_read_ident(m, seg_end, &k, pname, CI_MAX_NAME);
                    ptr_depth++;
                }
                break;
            }
            if (m[q] == '[') { ptr_depth++; break; }
            if (ci_is_id_start((unsigned char)m[q])) {
                char tok[CI_MAX_NAME] = {0};
                ci_read_ident(m, seg_end, &q, tok, CI_MAX_NAME);
                if (strcmp(tok, "struct") == 0 || strcmp(tok, "union") == 0 || strcmp(tok, "enum") == 0) {
                    char tag[CI_MAX_NAME] = {0};
                    if (ci_read_ident(m, seg_end, &q, tag, CI_MAX_NAME)) {
                        strncpy(base_type, tag, CI_MAX_NAME - 1);
                        got_base = 1;
                    }
                } else if (!got_base && (ci_is_type_qualifier(tok) || ci_is_known_type(ctx, tok))) {
                    if (!ci_is_type_qualifier(tok) || !base_type[0]) {
                        strncpy(base_type, tok, CI_MAX_NAME - 1);
                    }
                    if (!ci_is_type_qualifier(tok)) got_base = 1;
                } else if (!got_base) {
                    strncpy(base_type, tok, CI_MAX_NAME - 1);
                    got_base = 1;
                } else {
                    strncpy(pname, tok, CI_MAX_NAME - 1);
                }
            } else {
                q++;
            }
        }

        if (pname[0] && strcmp(pname, "void") != 0) {
            ci_add_symbol(ctx, pname, CI_VAR, base_type, ptr_depth, NULL, scope_depth, p);
        }
        p = seg_end + 1;
    }
}

/* Parse variable declaration list starting after base type until ';' or ')' */
static void ci_parse_var_declarators(CIContext *ctx, const char *m, int len, int *p,
                                     const char *base_type, int scope_depth, int stop_at_rparen) {
    while (*p < len) {
        ci_skip_ws(m, len, p);
        if (*p >= len) break;
        if (m[*p] == ';' || m[*p] == '}') { if (m[*p] == ';') (*p)++; break; }
        if (stop_at_rparen && m[*p] == ')') break;

        int ptr = 0;
        while (*p < len && (m[*p] == '*' || m[*p] == ' ' || m[*p] == '\t')) {
            if (m[*p] == '*') ptr++;
            (*p)++;
        }
        /* Handle const after * like char *const p */
        int saved = *p;
        char vname[CI_MAX_NAME] = {0};
        if (ci_read_ident(m, len, p, vname, CI_MAX_NAME)) {
            if (strcmp(vname, "const") == 0 || strcmp(vname, "restrict") == 0) {
                vname[0] = '\0';
                ci_read_ident(m, len, p, vname, CI_MAX_NAME);
            }
        } else {
            *p = saved;
        }

        int vpos = *p;
        ci_skip_ws(m, len, p);
        if (vname[0] && !ci_is_c_keyword(vname)) {
            /* Check if array declarator a[N] */
            int is_arr = (*p < len && m[*p] == '[');
            /* Note: for Student a[50], a[i] dereferences to Student (ptr_depth=0 for element, 1 for decay) */
            ci_add_symbol(ctx, vname, CI_VAR, base_type, ptr + (is_arr ? 1 : 0), NULL, scope_depth, vpos);
        }

        /* Skip initializer (= ...), array brackets, or nested parens until ',' or ';' */
        int par = 0, brk = 0, brc = 0;
        while (*p < len) {
            char c = m[*p];
            if (c == '(') par++;
            else if (c == ')') {
                if (par == 0 && stop_at_rparen) break;
                if (par > 0) par--;
            } else if (c == '[') brk++;
            else if (c == ']' && brk > 0) brk--;
            else if (c == '{') brc++;
            else if (c == '}') {
                if (brc == 0) break;
                brc--;
            } else if (par == 0 && brk == 0 && brc == 0) {
                if (c == ',') { (*p)++; break; }
                if (c == ';') { (*p)++; return; }
            }
            (*p)++;
        }
    }
}

static void ci_parse_preprocessor(CIContext *ctx, const char *src, int len) {
    int i = 0;
    while (i < len) {
        int line_start = i;
        while (i < len && (src[i] == ' ' || src[i] == '\t')) i++;
        if (i < len && src[i] == '#') {
            i++;
            while (i < len && (src[i] == ' ' || src[i] == '\t')) i++;
            if (i + 7 <= len && strncmp(src + i, "include", 7) == 0 && !ci_is_id_char((unsigned char)src[i + 7])) {
                i += 7;
                while (i < len && (src[i] == ' ' || src[i] == '\t')) i++;
                if (i < len && (src[i] == '<' || src[i] == '"')) {
                    char endc = (src[i] == '<') ? '>' : '"';
                    i++;
                    int hs = i;
                    while (i < len && src[i] != endc && src[i] != '\n') i++;
                    int hl = i - hs;
                    if (hl > 0 && hl < 31 && ctx->header_count < CI_MAX_HEADERS) {
                        memcpy(ctx->headers[ctx->header_count], src + hs, (size_t)hl);
                        ctx->headers[ctx->header_count][hl] = '\0';
                        ctx->header_count++;
                    }
                }
            } else if (i + 6 <= len && strncmp(src + i, "define", 6) == 0 && !ci_is_id_char((unsigned char)src[i + 6])) {
                i += 6;
                while (i < len && (src[i] == ' ' || src[i] == '\t')) i++;
                int ms = i;
                while (i < len && ci_is_id_char((unsigned char)src[i])) i++;
                int ml = i - ms;
                if (ml > 0 && ml < CI_MAX_NAME) {
                    char mname[CI_MAX_NAME];
                    memcpy(mname, src + ms, (size_t)ml);
                    mname[ml] = '\0';
                    if (i < len && src[i] == '(') {
                        /* Function-like macro: #define SQR(x) ... */
                        int ps = i;
                        while (i < len && src[i] != ')' && src[i] != '\n') i++;
                        if (i < len && src[i] == ')') i++;
                        char sig[CI_MAX_SIG];
                        int plen = i - ps;
                        if (plen > 96) plen = 96;
                        _snprintf(sig, CI_MAX_SIG, "#define %s%.*s", mname, plen, src + ps);
                        ci_add_symbol(ctx, mname, CI_FUNC, "", 0, sig, 0, line_start);
                    } else {
                        ci_add_symbol(ctx, mname, CI_MACRO, "", 0, NULL, 0, line_start);
                    }
                }
            }
        }
        while (i < len && src[i] != '\n') i++;
        if (i < len && src[i] == '\n') i++;
    }
}

/* Parse the entire C source buffer up to caret_pos for local scopes, plus top-level decls everywhere */
static void ci_parse_document(CIContext *ctx, const char *src, int src_len, int caret_pos) {
    memset(ctx, 0, sizeof(*ctx));
    if (!src || src_len <= 0) return;
    if (caret_pos < 0) caret_pos = 0;
    if (caret_pos > src_len) caret_pos = src_len;

    ci_parse_preprocessor(ctx, src, src_len);

    char *m = ci_mask_code(src, src_len);
    if (!m) return;

    /* Scope stack tracks brace depths and active symbol watermarks so closed sibling scopes are popped! */
    int sym_marks[64];
    int depth = 0;
    int in_active_path = 1;
    int p = 0;

    while (p < src_len) {
        ci_skip_ws(m, src_len, &p);
        if (p >= src_len) break;

        /* Skip preprocessor lines (already parsed) */
        if (m[p] == '#') {
            while (p < src_len && m[p] != '\n') {
                if (m[p] == '\\' && p + 1 < src_len && m[p + 1] == '\n') p += 2;
                else p++;
            }
            continue;
        }

        if (m[p] == '{') {
            if (depth < 63) sym_marks[depth] = ctx->sym_count;
            depth++;
            p++;
            continue;
        }
        if (m[p] == '}') {
            if (depth > 0) {
                depth--;
                /* If this block closed BEFORE caret_pos, pop its local variables (they're out of scope!) */
                if (p < caret_pos && depth < 63) {
                    int mark = sym_marks[depth];
                    int keep = mark;
                    for (int k = mark; k < ctx->sym_count; k++) {
                        if (ctx->syms[k].scope_depth == 0) {
                            if (keep != k) ctx->syms[keep] = ctx->syms[k];
                            keep++;
                        }
                    }
                    ctx->sym_count = keep;
                }
            }
            p++;
            continue;
        }

        /* Past caret_pos, only collect global/top-level declarations (depth == 0) */
        in_active_path = (p <= caret_pos);
        if (!in_active_path && depth > 0) {
            p++;
            continue;
        }

        /* Parse for-loop initializer declarations: for (int i = 0; ...) */
        if (in_active_path && p + 3 < src_len && strncmp(m + p, "for", 3) == 0 && !ci_is_id_char((unsigned char)m[p + 3])) {
            int q = p + 3;
            ci_skip_ws(m, src_len, &q);
            if (q < src_len && m[q] == '(') {
                q++;
                ci_skip_ws(m, src_len, &q);
                char ft[CI_MAX_NAME] = {0};
                int qs = q;
                if (ci_read_ident(m, src_len, &q, ft, CI_MAX_NAME)) {
                    while (ci_is_type_qualifier(ft)) {
                        int q2 = q;
                        char next_t[CI_MAX_NAME] = {0};
                        if (ci_read_ident(m, src_len, &q2, next_t, CI_MAX_NAME) && ci_is_known_type(ctx, next_t)) {
                            strncpy(ft, next_t, CI_MAX_NAME - 1);
                            q = q2;
                        } else break;
                    }
                    if (ci_is_known_type(ctx, ft)) {
                        /* Only keep if caret is inside this for-loop or its body */
                        ci_parse_var_declarators(ctx, m, src_len, &q, ft, depth + 1, 1);
                        p = q;
                        continue;
                    }
                }
                p = qs;
                continue;
            }
        }

        if (!ci_is_id_start((unsigned char)m[p])) {
            p++;
            continue;
        }

        int stmt_start = p;
        char first[CI_MAX_NAME] = {0};
        ci_read_ident(m, src_len, &p, first, CI_MAX_NAME);

        /* typedef ... */
        if (strcmp(first, "typedef") == 0) {
            char kw[CI_MAX_NAME] = {0};
            if (ci_read_ident(m, src_len, &p, kw, CI_MAX_NAME)) {
                if (strcmp(kw, "struct") == 0 || strcmp(kw, "union") == 0) {
                    char tag[CI_MAX_NAME] = {0};
                    int saved = p;
                    if (!ci_read_ident(m, src_len, &p, tag, CI_MAX_NAME)) p = saved;
                    ci_skip_ws(m, src_len, &p);
                    int st_idx = -1;
                    if (p < src_len && m[p] == '{') {
                        p++;
                        if (ctx->struct_count < CI_MAX_STRUCTS) {
                            st_idx = ctx->struct_count++;
                            memset(&ctx->structs[st_idx], 0, sizeof(CIStruct));
                            if (tag[0]) strncpy(ctx->structs[st_idx].name, tag, CI_MAX_NAME - 1);
                            ci_parse_struct_body(ctx, &ctx->structs[st_idx], m, src_len, &p);
                        }
                    }
                    char alias[CI_MAX_NAME] = {0};
                    if (ci_read_ident(m, src_len, &p, alias, CI_MAX_NAME)) {
                        ci_add_symbol(ctx, alias, CI_TYPE, alias, 0, NULL, 0, stmt_start);
                        if (st_idx >= 0) {
                            if (!ctx->structs[st_idx].name[0]) strncpy(ctx->structs[st_idx].name, alias, CI_MAX_NAME - 1);
                            else strncpy(ctx->structs[st_idx].alias, alias, CI_MAX_NAME - 1);
                        }
                    }
                    if (tag[0]) ci_add_symbol(ctx, tag, CI_TYPE, tag, 0, NULL, 0, stmt_start);
                } else if (strcmp(kw, "enum") == 0) {
                    char tag[CI_MAX_NAME] = {0};
                    int saved = p;
                    if (!ci_read_ident(m, src_len, &p, tag, CI_MAX_NAME)) p = saved;
                    ci_skip_ws(m, src_len, &p);
                    if (p < src_len && m[p] == '{') {
                        p++;
                        while (p < src_len && m[p] != '}') {
                            char ev[CI_MAX_NAME] = {0};
                            if (ci_read_ident(m, src_len, &p, ev, CI_MAX_NAME)) {
                                ci_add_symbol(ctx, ev, CI_MACRO, "int", 0, NULL, 0, p);
                            }
                            while (p < src_len && m[p] != ',' && m[p] != '}') p++;
                            if (p < src_len && m[p] == ',') p++;
                        }
                        if (p < src_len && m[p] == '}') p++;
                    }
                    char alias[CI_MAX_NAME] = {0};
                    if (ci_read_ident(m, src_len, &p, alias, CI_MAX_NAME)) {
                        ci_add_symbol(ctx, alias, CI_TYPE, alias, 0, NULL, 0, stmt_start);
                    }
                    if (tag[0]) ci_add_symbol(ctx, tag, CI_TYPE, tag, 0, NULL, 0, stmt_start);
                } else {
                    /* Simple typedef e.g. typedef long long ll; */
                    char last_id[CI_MAX_NAME] = {0};
                    char tmp[CI_MAX_NAME] = {0};
                    while (p < src_len && m[p] != ';') {
                        if (ci_is_id_start((unsigned char)m[p])) {
                            ci_read_ident(m, src_len, &p, tmp, CI_MAX_NAME);
                            strncpy(last_id, tmp, CI_MAX_NAME - 1);
                        } else p++;
                    }
                    if (last_id[0]) ci_add_symbol(ctx, last_id, CI_TYPE, last_id, 0, NULL, 0, stmt_start);
                }
            }
            while (p < src_len && m[p] != ';') p++;
            if (p < src_len && m[p] == ';') p++;
            continue;
        }

        /* Standalone struct / union / enum definition OR variable declaration with struct/union/enum prefix */
        if (strcmp(first, "struct") == 0 || strcmp(first, "union") == 0 || strcmp(first, "enum") == 0) {
            int is_enum = (strcmp(first, "enum") == 0);
            char tag[CI_MAX_NAME] = {0};
            int saved = p;
            if (!ci_read_ident(m, src_len, &p, tag, CI_MAX_NAME)) p = saved;
            ci_skip_ws(m, src_len, &p);
            if (p < src_len && m[p] == '{') {
                p++;
                if (is_enum) {
                    while (p < src_len && m[p] != '}') {
                        char ev[CI_MAX_NAME] = {0};
                        if (ci_read_ident(m, src_len, &p, ev, CI_MAX_NAME)) {
                            ci_add_symbol(ctx, ev, CI_MACRO, "int", 0, NULL, 0, p);
                        }
                        while (p < src_len && m[p] != ',' && m[p] != '}') p++;
                        if (p < src_len && m[p] == ',') p++;
                    }
                    if (p < src_len && m[p] == '}') p++;
                } else if (ctx->struct_count < CI_MAX_STRUCTS) {
                    int st_idx = ctx->struct_count++;
                    memset(&ctx->structs[st_idx], 0, sizeof(CIStruct));
                    if (tag[0]) strncpy(ctx->structs[st_idx].name, tag, CI_MAX_NAME - 1);
                    ci_parse_struct_body(ctx, &ctx->structs[st_idx], m, src_len, &p);
                }
                if (tag[0]) ci_add_symbol(ctx, tag, CI_TYPE, tag, 0, NULL, 0, stmt_start);
                if (tag[0]) ci_parse_var_declarators(ctx, m, src_len, &p, tag, depth, 0);
                continue;
            } else if (tag[0]) {
                /* Variable or function returning struct Tag */
                strncpy(first, tag, CI_MAX_NAME - 1);
                /* fall through to declaration handler below */
            } else {
                continue;
            }
        }

        /* Collect full type specifier prefix (qualifiers + base type) */
        char base_type[CI_MAX_NAME] = {0};
        int is_decl = 0;
        if (ci_is_type_qualifier(first) || ci_is_known_type(ctx, first)) {
            is_decl = 1;
            strncpy(base_type, first, CI_MAX_NAME - 1);
            for (;;) {
                int saved = p;
                char nxt[CI_MAX_NAME] = {0};
                if (!ci_read_ident(m, src_len, &p, nxt, CI_MAX_NAME)) { p = saved; break; }
                if (strcmp(nxt, "struct") == 0 || strcmp(nxt, "union") == 0 || strcmp(nxt, "enum") == 0) {
                    char tag[CI_MAX_NAME] = {0};
                    if (ci_read_ident(m, src_len, &p, tag, CI_MAX_NAME)) {
                        strncpy(base_type, tag, CI_MAX_NAME - 1);
                    }
                    continue;
                }
                if (ci_is_type_qualifier(nxt) || ci_is_known_type(ctx, nxt)) {
                    if (!ci_is_type_qualifier(nxt) || strcmp(base_type, "const") == 0 ||
                        strcmp(base_type, "static") == 0 || strcmp(base_type, "extern") == 0 ||
                        strcmp(base_type, "unsigned") == 0 || strcmp(base_type, "signed") == 0) {
                        strncpy(base_type, nxt, CI_MAX_NAME - 1);
                    }
                    continue;
                }
                p = saved;
                break;
            }
        }

        if (!is_decl) continue;

        /* Look ahead to distinguish function declaration/definition vs variable declaration */
        int after_type = p;
        int ptr = 0;
        while (p < src_len && (m[p] == '*' || m[p] == ' ' || m[p] == '\t' || m[p] == '\n' || m[p] == '\r')) {
            if (m[p] == '*') ptr++;
            p++;
        }
        char dname[CI_MAX_NAME] = {0};
        if (!ci_read_ident(m, src_len, &p, dname, CI_MAX_NAME) || ci_is_c_keyword(dname)) {
            p = after_type;
            continue;
        }
        ci_skip_ws(m, src_len, &p);

        if (depth == 0 && p < src_len && m[p] == '(') {
            /* Function prototype or definition! */
            int par_start = p + 1;
            int par = 1, q = p + 1;
            while (q < src_len && par > 0) {
                if (m[q] == '(') par++;
                else if (m[q] == ')') par--;
                q++;
            }
            int par_end = (q > par_start) ? (q - 1) : par_start;

            /* Build clean normalized signature from raw src[stmt_start..q] */
            char sig[CI_MAX_SIG];
            int s_out = 0, prev_sp = 0;
            for (int k = stmt_start; k < q && s_out < CI_MAX_SIG - 2; k++) {
                char c = src[k];
                if (c == '\r' || c == '\n' || c == '\t' || c == ' ') {
                    if (!prev_sp && s_out > 0) { sig[s_out++] = ' '; prev_sp = 1; }
                } else {
                    sig[s_out++] = c;
                    prev_sp = 0;
                }
            }
            sig[s_out] = '\0';
            ci_add_symbol(ctx, dname, CI_FUNC, base_type, ptr, sig, 0, stmt_start);

            int after_par = q;
            ci_skip_ws(m, src_len, &after_par);
            if (after_par < src_len && m[after_par] == '{') {
                /* Function body starts here — find its matching '}' to see if caret is inside! */
                int b = after_par + 1, bd = 1;
                while (b < src_len && bd > 0) {
                    if (m[b] == '{') bd++;
                    else if (m[b] == '}') bd--;
                    b++;
                }
                if (caret_pos >= after_par && caret_pos <= b) {
                    /* Caret is inside this function: enter body & inject parameters at depth 1! */
                    if (depth < 63) sym_marks[depth] = ctx->sym_count;
                    depth++;
                    ci_parse_param_list(ctx, m, par_start, par_end, depth);
                    p = after_par + 1;
                } else {
                    /* Caret is outside this function: skip entire body quickly */
                    p = b;
                }
            } else {
                p = after_par;
                if (p < src_len && m[p] == ';') p++;
            }
        } else {
            /* Variable declaration list! */
            p = after_type;
            ci_parse_var_declarators(ctx, m, src_len, &p, base_type, depth, 0);
        }
    }

    free(m);
}

/* ---------- Member Chain Resolver (supports var., ptr->, arr[i]., b->data.) ---------- */
static CIStruct *ci_resolve_member_chain(CIContext *ctx, const char *src, int dot_pos) {
    /* Walk backward from dot_pos (which points to '.' or the '-' of '->') to collect chain segments */
    char segs[8][CI_MAX_NAME];
    int nsegs = 0;
    int pos = dot_pos;

    while (pos > 0 && nsegs < 8) {
        while (pos > 0 && (src[pos - 1] == ' ' || src[pos - 1] == '\t')) pos--;
        /* Skip balanced [...] array subscripts */
        while (pos > 0 && src[pos - 1] == ']') {
            int brk = 1;
            pos--;
            while (pos > 0 && brk > 0) {
                pos--;
                if (src[pos] == ']') brk++;
                else if (src[pos] == '[') brk--;
            }
            while (pos > 0 && (src[pos - 1] == ' ' || src[pos - 1] == '\t')) pos--;
        }
        /* Skip balanced (...) e.g. (*p) or func() */
        if (pos > 0 && src[pos - 1] == ')') {
            int par = 1, endp = pos - 1;
            pos--;
            while (pos > 0 && par > 0) {
                pos--;
                if (src[pos] == ')') par++;
                else if (src[pos] == '(') par--;
            }
            /* Check if (*ident) */
            int k = pos + 1;
            while (k < endp && (src[k] == ' ' || src[k] == '*')) k++;
            int ks = k;
            while (k < endp && ci_is_id_char((unsigned char)src[k])) k++;
            if (k > ks && k - ks < CI_MAX_NAME) {
                memcpy(segs[nsegs], src + ks, (size_t)(k - ks));
                segs[nsegs][k - ks] = '\0';
                nsegs++;
            }
            break;
        }
        int end_id = pos;
        while (pos > 0 && ci_is_id_char((unsigned char)src[pos - 1])) pos--;
        int id_len = end_id - pos;
        if (id_len <= 0 || id_len >= CI_MAX_NAME) break;
        memcpy(segs[nsegs], src + pos, (size_t)id_len);
        segs[nsegs][id_len] = '\0';
        nsegs++;

        while (pos > 0 && (src[pos - 1] == ' ' || src[pos - 1] == '\t')) pos--;
        if (pos >= 2 && src[pos - 2] == '-' && src[pos - 1] == '>') {
            pos -= 2;
        } else if (pos >= 1 && src[pos - 1] == '.') {
            pos -= 1;
        } else {
            break;
        }
    }

    if (nsegs == 0) return NULL;
    /* Resolve root identifier (segs[nsegs - 1]) from deepest active scope */
    const char *root_name = segs[nsegs - 1];
    const char *cur_type = NULL;
    int best_scope = -1;
    for (int i = ctx->sym_count - 1; i >= 0; i--) {
        if ((ctx->syms[i].kind == CI_VAR || ctx->syms[i].kind == CI_FUNC) &&
            strcmp(ctx->syms[i].name, root_name) == 0 && ctx->syms[i].scope_depth >= best_scope) {
            best_scope = ctx->syms[i].scope_depth;
            cur_type = ctx->syms[i].type_name;
        }
    }
    if (!cur_type || !cur_type[0]) return NULL;

    CIStruct *st = ci_find_struct(ctx, cur_type);
    for (int s = nsegs - 2; s >= 0 && st != NULL; s--) {
        const char *mname = segs[s];
        const char *next_type = NULL;
        for (int f = 0; f < st->field_count; f++) {
            if (strcmp(st->fields[f].name, mname) == 0) {
                next_type = st->fields[f].type_name;
                break;
            }
        }
        if (!next_type) return NULL;
        st = ci_find_struct(ctx, next_type);
    }
    return st;
}

/* ---------- Exact Lexical Guard (Comment / String / Char Literal Detection) ---------- */
static int ci_is_in_comment_or_string(const char *src, int caret_pos) {
    if (!src || caret_pos <= 0) return 0;
    int state = 0; /* 0=code, 1=line_comment, 2=block_comment, 3=dquote, 4=squote */
    for (int i = 0; i < caret_pos; i++) {
        char c = src[i];
        char nxt = (i + 1 < caret_pos) ? src[i + 1] : '\0';
        if (state == 0) {
            if (c == '/' && nxt == '/') { state = 1; i++; }
            else if (c == '/' && nxt == '*') { state = 2; i++; }
            else if (c == '"') { state = 3; }
            else if (c == '\'') { state = 4; }
        } else if (state == 1) {
            if (c == '\n') state = 0;
        } else if (state == 2) {
            if (c == '*' && nxt == '/') { state = 0; i++; }
        } else if (state == 3) {
            if (c == '\\' && i + 1 < caret_pos) i++;
            else if (c == '"' || c == '\n') state = 0;
        } else if (state == 4) {
            if (c == '\\' && i + 1 < caret_pos) i++;
            else if (c == '\'' || c == '\n') state = 0;
        }
    }
    return state != 0;
}

/* ---------- Completion Candidate Ranking & Structured Result (RedPanda-C++ Style) ---------- */
#define CI_MAX_RESULTS 48

typedef struct {
    char name[CI_MAX_NAME];
    char detail[CI_MAX_SIG];
    int kind;
    int priority; /* lower = higher rank */
    int is_prefix;
} CICand;

typedef struct {
    int mode;       /* 0=none, 1=normal, 2=member (. or ->), 3=#include <header> */
    int prefix_len;
    char prefix[CI_MAX_NAME];
    CICand items[CI_MAX_RESULTS];
    int count;
} CIResult;

static void ci_format_type_detail(const char *type_name, int ptr_depth, char *out, int cap) {
    if (!type_name || !type_name[0]) { out[0] = '\0'; return; }
    int n = _snprintf(out, (size_t)cap, "%s", type_name);
    if (n < 0) n = 0;
    if (ptr_depth > 0 && n < cap - 2) {
        out[n++] = ' ';
        for (int i = 0; i < ptr_depth && n < cap - 1; i++) out[n++] = '*';
        out[n] = '\0';
    }
}

/* Returns match penalty (0=exact-case prefix, 20=ci-prefix, 55=acronym/boundary, 90=subseq) or -1 if no match */
static int ci_match_score(const char *cand, const char *prefix, int plen, int *out_is_prefix) {
    *out_is_prefix = 1;
    if (plen == 0) return 0;
    if (strncmp(cand, prefix, (size_t)plen) == 0) return 0;
    if (_strnicmp(cand, prefix, (size_t)plen) == 0) return 20;

    /* Fuzzy / subsequence matching (RedPanda-C++ style: pf -> printf, mc -> my_count_char) */
    if (plen < 2) return -1;
    if (tolower((unsigned char)cand[0]) != tolower((unsigned char)prefix[0])) return -1;

    *out_is_prefix = 0;
    int ci = 0, pi = 0, boundary_hits = 0;
    int clen = (int)strlen(cand);
    while (ci < clen && pi < plen) {
        if (tolower((unsigned char)cand[ci]) == tolower((unsigned char)prefix[pi])) {
            if (ci == 0 || cand[ci - 1] == '_' || (isupper((unsigned char)cand[ci]) && islower((unsigned char)cand[ci - 1]))) {
                boundary_hits++;
            }
            pi++;
        }
        ci++;
    }
    if (pi < plen) return -1;
    return (boundary_hits == plen) ? 55 : 90;
}

static int ci_cmp_cand(const void *a, const void *b) {
    const CICand *ca = (const CICand *)a;
    const CICand *cb = (const CICand *)b;
    if (ca->priority != cb->priority) return ca->priority - cb->priority;
    return _stricmp(ca->name, cb->name);
}

static void ci_push_cand(CICand *arr, int *cnt, int max_cnt, const char *name, const char *detail,
                         int kind, int prio, int is_prefix) {
    if (!name || !name[0]) return;
    for (int i = 0; i < *cnt; i++) {
        if (strcmp(arr[i].name, name) == 0) {
            if (prio < arr[i].priority) {
                arr[i].priority = prio;
                arr[i].kind = kind;
                arr[i].is_prefix = is_prefix;
                if (detail && detail[0]) {
                    strncpy(arr[i].detail, detail, CI_MAX_SIG - 1);
                    arr[i].detail[CI_MAX_SIG - 1] = '\0';
                }
            }
            return;
        }
    }
    if (*cnt >= max_cnt) return;
    strncpy(arr[*cnt].name, name, CI_MAX_NAME - 1);
    arr[*cnt].name[CI_MAX_NAME - 1] = '\0';
    if (detail && detail[0]) {
        strncpy(arr[*cnt].detail, detail, CI_MAX_SIG - 1);
        arr[*cnt].detail[CI_MAX_SIG - 1] = '\0';
    } else {
        arr[*cnt].detail[0] = '\0';
    }
    arr[*cnt].kind = kind;
    arr[*cnt].priority = prio;
    arr[*cnt].is_prefix = is_prefix;
    (*cnt)++;
}

static int ci_query_completions(const char *src, int src_len, int caret_pos, int force, CIResult *out) {
    memset(out, 0, sizeof(*out));
    if (!src || caret_pos < 0 || caret_pos > src_len) return 0;

    /* 1. Check #include <prefix completion (allowed before comment guard) */
    int line_start = caret_pos;
    while (line_start > 0 && src[line_start - 1] != '\n') line_start--;
    int k = line_start;
    while (k < caret_pos && (src[k] == ' ' || src[k] == '\t')) k++;
    if (k < caret_pos && src[k] == '#') {
        k++;
        while (k < caret_pos && (src[k] == ' ' || src[k] == '\t')) k++;
        if (k + 7 <= caret_pos && strncmp(src + k, "include", 7) == 0) {
            k += 7;
            while (k < caret_pos && (src[k] == ' ' || src[k] == '\t')) k++;
            if (k < caret_pos && src[k] == '<') {
                int hs = k + 1;
                int ok_hdr = 1;
                for (int q = hs; q < caret_pos; q++) {
                    if (src[q] == '>' || src[q] == ' ') { ok_hdr = 0; break; }
                }
                if (ok_hdr) {
                    int plen = caret_pos - hs;
                    char pref[64] = {0};
                    if (plen < 63) memcpy(pref, src + hs, (size_t)plen);
                    out->prefix_len = plen;
                    strncpy(out->prefix, pref, CI_MAX_NAME - 1);
                    for (int i = 0; i < C_STD_HEADERS_LEN && out->count < CI_MAX_RESULTS; i++) {
                        int is_p = 0;
                        int sc = ci_match_score(C_STD_HEADERS[i], pref, plen, &is_p);
                        if (sc >= 0) {
                            ci_push_cand(out->items, &out->count, CI_MAX_RESULTS, C_STD_HEADERS[i], "C99 标准头文件", CI_HEADER, sc, is_p);
                        }
                    }
                    if (out->count > 0) {
                        qsort(out->items, (size_t)out->count, sizeof(CICand), ci_cmp_cand);
                        out->mode = 3;
                        return 3;
                    }
                    return 0;
                }
            }
        }
    }

    /* Suppress completion inside comments, strings, and char literals */
    if (!force && ci_is_in_comment_or_string(src, caret_pos)) return 0;

    /* Extract identifier prefix before caret */
    int id_start = caret_pos;
    while (id_start > 0 && ci_is_id_char((unsigned char)src[id_start - 1])) id_start--;
    int plen = caret_pos - id_start;
    if (plen >= CI_MAX_NAME) return 0;
    if (plen > 0 && isdigit((unsigned char)src[id_start])) return 0;

    char prefix[CI_MAX_NAME];
    memcpy(prefix, src + id_start, (size_t)plen);
    prefix[plen] = '\0';

    /* Check if preceded by '.' or '->' member access */
    int op_pos = id_start;
    while (op_pos > 0 && (src[op_pos - 1] == ' ' || src[op_pos - 1] == '\t')) op_pos--;
    int is_member = 0;
    int dot_pos = -1;
    if (op_pos >= 2 && src[op_pos - 2] == '-' && src[op_pos - 1] == '>') {
        is_member = 1; dot_pos = op_pos - 2;
    } else if (op_pos >= 1 && src[op_pos - 1] == '.' && !(op_pos >= 2 && isdigit((unsigned char)src[op_pos - 2]))) {
        is_member = 1; dot_pos = op_pos - 1;
    }

    if (!is_member && !force && plen < 1) return 0;

    CIContext *ctx = (CIContext *)malloc(sizeof(CIContext));
    if (!ctx) return 0;
    ci_parse_document(ctx, src, src_len, caret_pos);

    CICand cands[256];
    int ncand = 0;

    if (is_member) {
        CIStruct *st = ci_resolve_member_chain(ctx, src, dot_pos);
        if (st) {
            for (int f = 0; f < st->field_count; f++) {
                int is_p = 0;
                int sc = ci_match_score(st->fields[f].name, prefix, plen, &is_p);
                if (sc >= 0) {
                    char det[CI_MAX_SIG];
                    ci_format_type_detail(st->fields[f].type_name, st->fields[f].ptr_depth, det, sizeof(det));
                    ci_push_cand(cands, &ncand, 256, st->fields[f].name, det, CI_MEMBER, sc, is_p);
                }
            }
        }
        free(ctx);
        if (ncand == 0) return 0;
        qsort(cands, (size_t)ncand, sizeof(CICand), ci_cmp_cand);
        out->mode = 2;
        out->prefix_len = plen;
        strncpy(out->prefix, prefix, CI_MAX_NAME - 1);
        out->count = (ncand > CI_MAX_RESULTS) ? CI_MAX_RESULTS : ncand;
        for (int i = 0; i < out->count; i++) out->items[i] = cands[i];
        return 2;
    }

    /* Normal identifier completion: local vars/params > global vars > user funcs/types/macros > included stdlib > keywords */
    for (int i = 0; i < ctx->sym_count; i++) {
        const CISymbol *s = &ctx->syms[i];
        int is_p = 0;
        if (strcmp(s->name, prefix) == 0 && !force) continue;
        int sc = ci_match_score(s->name, prefix, plen, &is_p);
        if (sc < 0) continue;
        int base_p = 10;
        char det[CI_MAX_SIG] = {0};
        if (s->kind == CI_VAR && s->scope_depth > 0) {
            base_p = 10;
            ci_format_type_detail(s->type_name, s->ptr_depth, det, sizeof(det));
        } else if (s->kind == CI_VAR) {
            base_p = 20;
            ci_format_type_detail(s->type_name, s->ptr_depth, det, sizeof(det));
        } else if (s->kind == CI_FUNC) {
            base_p = 25;
            if (s->sig[0]) strncpy(det, s->sig, CI_MAX_SIG - 1);
            else ci_format_type_detail(s->type_name, s->ptr_depth, det, sizeof(det));
        } else if (s->kind == CI_TYPE) {
            base_p = 30;
            strncpy(det, "自定义类型", CI_MAX_SIG - 1);
        } else {
            base_p = 35;
            strncpy(det, "宏 / 枚举常量", CI_MAX_SIG - 1);
        }
        ci_push_cand(cands, &ncand, 256, s->name, det, s->kind, base_p + sc * 2, is_p);
    }

    /* C99 stdlib symbols & keywords */
    for (int i = 0; i < C_STD_SYMBOLS_LEN; i++) {
        const CStdSymbol *st = &C_STD_SYMBOLS[i];
        int is_p = 0;
        if (strcmp(st->name, prefix) == 0 && !force) continue;
        int sc = ci_match_score(st->name, prefix, plen, &is_p);
        if (sc < 0) continue;
        int hdr_inc = ci_has_header(ctx, st->header);
        if (!force && plen == 1 && st->header != NULL && !hdr_inc) continue;
        int base_p = 50;
        if (st->header == NULL) base_p = (st->kind == CI_TYPE) ? 45 : 65;
        else if (hdr_inc) base_p = (st->kind == CI_FUNC) ? 40 : 50;
        else base_p = 80;

        char det[CI_MAX_SIG] = {0};
        if (st->sig && st->sig[0]) {
            strncpy(det, st->sig, CI_MAX_SIG - 1);
        } else if (st->header) {
            _snprintf(det, CI_MAX_SIG, "<%s>", st->header);
        } else if (st->kind == CI_TYPE) {
            strncpy(det, "C 基本类型", CI_MAX_SIG - 1);
        } else {
            strncpy(det, "C 关键字", CI_MAX_SIG - 1);
        }
        ci_push_cand(cands, &ncand, 256, st->name, det, st->kind, base_p + sc * 2, is_p);
    }

    free(ctx);
    if (ncand == 0) return 0;

    qsort(cands, (size_t)ncand, sizeof(CICand), ci_cmp_cand);
    out->mode = 1;
    out->prefix_len = plen;
    strncpy(out->prefix, prefix, CI_MAX_NAME - 1);
    out->count = (ncand > CI_MAX_RESULTS) ? CI_MAX_RESULTS : ncand;
    for (int i = 0; i < out->count; i++) out->items[i] = cands[i];
    return 1;
}

/* Check if a symbol name is a callable function (stdlib or user-defined in current buffer) */
static int ci_is_callable_symbol(const char *src, int src_len, int caret_pos, const char *name) {
    if (!name || !name[0]) return 0;
    for (int i = 0; i < C_STD_SYMBOLS_LEN; i++) {
        if (C_STD_SYMBOLS[i].kind == CI_FUNC && strcmp(C_STD_SYMBOLS[i].name, name) == 0) return 1;
    }
    if (!src || src_len <= 0) return 0;
    CIContext *ctx = (CIContext *)malloc(sizeof(CIContext));
    if (!ctx) return 0;
    ci_parse_document(ctx, src, src_len, caret_pos);
    int is_fn = 0;
    for (int i = 0; i < ctx->sym_count; i++) {
        if (ctx->syms[i].kind == CI_FUNC && strcmp(ctx->syms[i].name, name) == 0) { is_fn = 1; break; }
    }
    free(ctx);
    return is_fn;
}

/* ---------- Live Function CallTip Lookup & Active Parameter Highlighting ---------- */
static int ci_get_calltip(const char *src, int src_len, int caret_pos,
                          char *out_sig, int out_cap, int *out_hlt_start, int *out_hlt_end, int *out_call_pos) {
    out_sig[0] = '\0';
    *out_hlt_start = 0;
    *out_hlt_end = 0;
    *out_call_pos = caret_pos;
    if (!src || caret_pos <= 0 || caret_pos > src_len) return 0;

    char *m = ci_mask_code(src, caret_pos);
    if (!m) return 0;

    /* Walk backward from caret_pos to find enclosing unclosed '(' and count top-level commas */
    int depth = 0, arg_idx = 0, open_par = -1;
    for (int i = caret_pos - 1; i >= 0; i--) {
        char c = m[i];
        if (c == ')') depth++;
        else if (c == '(') {
            if (depth == 0) { open_par = i; break; }
            depth--;
        } else if (c == ',' && depth == 0) {
            arg_idx++;
        } else if ((c == ';' || c == '{' || c == '}') && depth == 0) {
            break;
        }
    }
    free(m);
    if (open_par <= 0) return 0;

    int fe = open_par;
    while (fe > 0 && (src[fe - 1] == ' ' || src[fe - 1] == '\t')) fe--;
    int fs = fe;
    while (fs > 0 && ci_is_id_char((unsigned char)src[fs - 1])) fs--;
    int flen = fe - fs;
    if (flen <= 0 || flen >= CI_MAX_NAME) return 0;

    char fname[CI_MAX_NAME];
    memcpy(fname, src + fs, (size_t)flen);
    fname[flen] = '\0';
    if (ci_is_c_keyword(fname) && strcmp(fname, "sizeof") != 0) return 0;

    const char *found_sig = NULL;
    char user_sig[CI_MAX_SIG] = {0};

    /* 1. Check user-defined functions / macros first */
    CIContext *ctx = (CIContext *)malloc(sizeof(CIContext));
    if (ctx) {
        ci_parse_document(ctx, src, src_len, caret_pos);
        for (int i = 0; i < ctx->sym_count; i++) {
            if (ctx->syms[i].kind == CI_FUNC && strcmp(ctx->syms[i].name, fname) == 0 && ctx->syms[i].sig[0]) {
                strncpy(user_sig, ctx->syms[i].sig, CI_MAX_SIG - 1);
                found_sig = user_sig;
                break;
            }
        }
        free(ctx);
    }

    /* 2. Check C99 stdlib catalog */
    if (!found_sig) {
        for (int i = 0; i < C_STD_SYMBOLS_LEN; i++) {
            if (C_STD_SYMBOLS[i].kind == CI_FUNC && C_STD_SYMBOLS[i].sig && strcmp(C_STD_SYMBOLS[i].name, fname) == 0) {
                found_sig = C_STD_SYMBOLS[i].sig;
                break;
            }
        }
    }
    if (!found_sig) return 0;

    strncpy(out_sig, found_sig, (size_t)out_cap - 1);
    out_sig[out_cap - 1] = '\0';
    *out_call_pos = open_par + 1;

    /* Compute active argument highlight range inside signature's '( ... )' */
    const char *lp = strchr(out_sig, '(');
    if (lp) {
        int cur = 0;
        int p = (int)(lp - out_sig) + 1;
        int slen = (int)strlen(out_sig);
        int par = 0;
        int seg_s = p;
        for (; p < slen; p++) {
            char c = out_sig[p];
            if (c == '(') par++;
            else if (c == ')') {
                if (par == 0) {
                    if (cur == arg_idx || strstr(out_sig + seg_s, "...") != NULL) {
                        while (seg_s < p && out_sig[seg_s] == ' ') seg_s++;
                        *out_hlt_start = seg_s;
                        *out_hlt_end = p;
                    }
                    break;
                }
                par--;
            } else if (c == ',' && par == 0) {
                if (cur == arg_idx) {
                    while (seg_s < p && out_sig[seg_s] == ' ') seg_s++;
                    *out_hlt_start = seg_s;
                    *out_hlt_end = p;
                    break;
                }
                cur++;
                seg_s = p + 1;
            }
        }
    }
    return 1;
}

#endif
