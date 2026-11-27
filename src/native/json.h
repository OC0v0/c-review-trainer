/* Minimal self-contained JSON parser & string builder for C99 (TCC-compatible). */
#ifndef CREV_JSON_H
#define CREV_JSON_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *data;
    size_t len;
    size_t cap;
} StrBuf;

static void sb_init(StrBuf *sb) {
    sb->cap = 256;
    sb->len = 0;
    sb->data = (char *)malloc(sb->cap);
    if (sb->data) sb->data[0] = '\0';
}

static void sb_free(StrBuf *sb) {
    free(sb->data);
    sb->data = NULL;
    sb->len = sb->cap = 0;
}

static void sb_reserve(StrBuf *sb, size_t extra) {
    if (sb->len + extra + 1 <= sb->cap) return;
    size_t ncap = sb->cap ? sb->cap * 2 : 256;
    while (ncap < sb->len + extra + 1) ncap *= 2;
    char *nd = (char *)realloc(sb->data, ncap);
    if (nd) {
        sb->data = nd;
        sb->cap = ncap;
    }
}

static void sb_append_mem(StrBuf *sb, const char *s, size_t n) {
    sb_reserve(sb, n);
    memcpy(sb->data + sb->len, s, n);
    sb->len += n;
    sb->data[sb->len] = '\0';
}

static void sb_append(StrBuf *sb, const char *s) {
    sb_append_mem(sb, s, strlen(s));
}

static void sb_append_char(StrBuf *sb, char c) {
    sb_reserve(sb, 1);
    sb->data[sb->len++] = c;
    sb->data[sb->len] = '\0';
}

static void sb_append_int(StrBuf *sb, long long v) {
    char tmp[64];
    sprintf(tmp, "%lld", v);
    sb_append(sb, tmp);
}

static void sb_append_json_str(StrBuf *sb, const char *s) {
    sb_append_char(sb, '"');
    if (s) {
        for (const unsigned char *p = (const unsigned char *)s; *p; p++) {
            unsigned char c = *p;
            if (c == '"') sb_append(sb, "\\\"");
            else if (c == '\\') sb_append(sb, "\\\\");
            else if (c == '\b') sb_append(sb, "\\b");
            else if (c == '\f') sb_append(sb, "\\f");
            else if (c == '\n') sb_append(sb, "\\n");
            else if (c == '\r') sb_append(sb, "\\r");
            else if (c == '\t') sb_append(sb, "\\t");
            else if (c < 0x20) {
                char esc[8];
                sprintf(esc, "\\u%04x", (unsigned)c);
                sb_append(sb, esc);
            } else {
                sb_append_char(sb, (char)c);
            }
        }
    }
    sb_append_char(sb, '"');
}

/* JSON DOM */
typedef enum {
    J_NULL = 0,
    J_BOOL,
    J_NUM,
    J_STR,
    J_ARR,
    J_OBJ
} JType;

typedef struct JVal JVal;

typedef struct {
    char *key;
    JVal *val;
} JPair;

struct JVal {
    JType type;
    int bval;
    double nval;
    char *sval;
    JVal **items;
    size_t arr_len;
    size_t arr_cap;
    JPair *pairs;
    size_t obj_len;
    size_t obj_cap;
};

static JVal *j_new(JType t) {
    JVal *v = (JVal *)calloc(1, sizeof(JVal));
    if (v) v->type = t;
    return v;
}

static char *xstrdup(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s);
    char *r = (char *)malloc(n + 1);
    if (r) memcpy(r, s, n + 1);
    return r;
}

static JVal *j_str(const char *s) {
    JVal *v = j_new(J_STR);
    v->sval = xstrdup(s ? s : "");
    return v;
}

static JVal *j_num(double n) {
    JVal *v = j_new(J_NUM);
    v->nval = n;
    return v;
}

static JVal *j_bool(int b) {
    JVal *v = j_new(J_BOOL);
    v->bval = b ? 1 : 0;
    return v;
}

static void j_free(JVal *v) {
    if (!v) return;
    if (v->type == J_STR) free(v->sval);
    else if (v->type == J_ARR) {
        for (size_t i = 0; i < v->arr_len; i++) j_free(v->items[i]);
        free(v->items);
    } else if (v->type == J_OBJ) {
        for (size_t i = 0; i < v->obj_len; i++) {
            free(v->pairs[i].key);
            j_free(v->pairs[i].val);
        }
        free(v->pairs);
    }
    free(v);
}

static void j_arr_push(JVal *arr, JVal *item) {
    if (arr->arr_len == arr->arr_cap) {
        arr->arr_cap = arr->arr_cap ? arr->arr_cap * 2 : 8;
        arr->items = (JVal **)realloc(arr->items, arr->arr_cap * sizeof(JVal *));
    }
    arr->items[arr->arr_len++] = item;
}

static JVal *j_obj_get(const JVal *obj, const char *key) {
    if (!obj || obj->type != J_OBJ) return NULL;
    for (size_t i = 0; i < obj->obj_len; i++) {
        if (strcmp(obj->pairs[i].key, key) == 0) return obj->pairs[i].val;
    }
    return NULL;
}

static const char *j_get_str(const JVal *obj, const char *key, const char *def) {
    JVal *v = j_obj_get(obj, key);
    return (v && v->type == J_STR && v->sval) ? v->sval : def;
}

static int j_get_bool(const JVal *obj, const char *key, int def) {
    JVal *v = j_obj_get(obj, key);
    return (v && v->type == J_BOOL) ? v->bval : def;
}

static int j_get_int(const JVal *obj, const char *key, int def) {
    JVal *v = j_obj_get(obj, key);
    return (v && v->type == J_NUM) ? (int)v->nval : def;
}

static void j_obj_set(JVal *obj, const char *key, JVal *val) {
    if (!obj || obj->type != J_OBJ) return;
    for (size_t i = 0; i < obj->obj_len; i++) {
        if (strcmp(obj->pairs[i].key, key) == 0) {
            j_free(obj->pairs[i].val);
            obj->pairs[i].val = val;
            return;
        }
    }
    if (obj->obj_len == obj->obj_cap) {
        obj->obj_cap = obj->obj_cap ? obj->obj_cap * 2 : 8;
        obj->pairs = (JPair *)realloc(obj->pairs, obj->obj_cap * sizeof(JPair));
    }
    obj->pairs[obj->obj_len].key = xstrdup(key);
    obj->pairs[obj->obj_len].val = val;
    obj->obj_len++;
}

static void j_serialize_into(const JVal *v, StrBuf *sb) {
    if (!v || v->type == J_NULL) {
        sb_append(sb, "null");
    } else if (v->type == J_BOOL) {
        sb_append(sb, v->bval ? "true" : "false");
    } else if (v->type == J_NUM) {
        char tmp[64];
        if ((double)(long long)v->nval == v->nval) sprintf(tmp, "%lld", (long long)v->nval);
        else sprintf(tmp, "%.15g", v->nval);
        sb_append(sb, tmp);
    } else if (v->type == J_STR) {
        sb_append_json_str(sb, v->sval);
    } else if (v->type == J_ARR) {
        sb_append_char(sb, '[');
        for (size_t i = 0; i < v->arr_len; i++) {
            if (i > 0) sb_append_char(sb, ',');
            j_serialize_into(v->items[i], sb);
        }
        sb_append_char(sb, ']');
    } else if (v->type == J_OBJ) {
        sb_append_char(sb, '{');
        for (size_t i = 0; i < v->obj_len; i++) {
            if (i > 0) sb_append_char(sb, ',');
            sb_append_json_str(sb, v->pairs[i].key);
            sb_append_char(sb, ':');
            j_serialize_into(v->pairs[i].val, sb);
        }
        sb_append_char(sb, '}');
    }
}

/* Parser */
typedef struct {
    const char *s;
    size_t pos;
    size_t len;
} JParser;

static void jp_skip(JParser *p) {
    while (p->pos < p->len) {
        char c = p->s[p->pos];
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') p->pos++;
        else break;
    }
}

static unsigned jp_hex4(JParser *p) {
    unsigned val = 0;
    for (int i = 0; i < 4 && p->pos < p->len; i++) {
        char c = p->s[p->pos++];
        val <<= 4;
        if (c >= '0' && c <= '9') val |= (unsigned)(c - '0');
        else if (c >= 'a' && c <= 'f') val |= (unsigned)(c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') val |= (unsigned)(c - 'A' + 10);
    }
    return val;
}

static void sb_append_utf8(StrBuf *sb, unsigned cp) {
    if (cp < 0x80) {
        sb_append_char(sb, (char)cp);
    } else if (cp < 0x800) {
        sb_append_char(sb, (char)(0xC0 | (cp >> 6)));
        sb_append_char(sb, (char)(0x80 | (cp & 0x3F)));
    } else if (cp < 0x10000) {
        sb_append_char(sb, (char)(0xE0 | (cp >> 12)));
        sb_append_char(sb, (char)(0x80 | ((cp >> 6) & 0x3F)));
        sb_append_char(sb, (char)(0x80 | (cp & 0x3F)));
    } else {
        sb_append_char(sb, (char)(0xF0 | (cp >> 18)));
        sb_append_char(sb, (char)(0x80 | ((cp >> 12) & 0x3F)));
        sb_append_char(sb, (char)(0x80 | ((cp >> 6) & 0x3F)));
        sb_append_char(sb, (char)(0x80 | (cp & 0x3F)));
    }
}

static char *jp_parse_str_raw(JParser *p) {
    if (p->pos >= p->len || p->s[p->pos] != '"') return NULL;
    p->pos++;
    StrBuf sb;
    sb_init(&sb);
    while (p->pos < p->len) {
        char c = p->s[p->pos++];
        if (c == '"') {
            return sb.data;
        }
        if (c == '\\' && p->pos < p->len) {
            char e = p->s[p->pos++];
            if (e == '"') sb_append_char(&sb, '"');
            else if (e == '\\') sb_append_char(&sb, '\\');
            else if (e == '/') sb_append_char(&sb, '/');
            else if (e == 'b') sb_append_char(&sb, '\b');
            else if (e == 'f') sb_append_char(&sb, '\f');
            else if (e == 'n') sb_append_char(&sb, '\n');
            else if (e == 'r') sb_append_char(&sb, '\r');
            else if (e == 't') sb_append_char(&sb, '\t');
            else if (e == 'u') {
                unsigned cp = jp_hex4(p);
                if (cp >= 0xD800 && cp <= 0xDBFF && p->pos + 6 <= p->len &&
                    p->s[p->pos] == '\\' && p->s[p->pos + 1] == 'u') {
                    p->pos += 2;
                    unsigned low = jp_hex4(p);
                    cp = 0x10000 + (((cp - 0xD800) << 10) | (low - 0xDC00));
                }
                sb_append_utf8(&sb, cp);
            } else {
                sb_append_char(&sb, e);
            }
        } else {
            sb_append_char(&sb, c);
        }
    }
    sb_free(&sb);
    return NULL;
}

static JVal *jp_parse_val(JParser *p);

static JVal *jp_parse_val(JParser *p) {
    jp_skip(p);
    if (p->pos >= p->len) return NULL;
    char c = p->s[p->pos];
    if (c == '"') {
        char *str = jp_parse_str_raw(p);
        if (!str) return NULL;
        JVal *v = j_new(J_STR);
        v->sval = str;
        return v;
    }
    if (c == '{') {
        p->pos++;
        JVal *obj = j_new(J_OBJ);
        jp_skip(p);
        if (p->pos < p->len && p->s[p->pos] == '}') {
            p->pos++;
            return obj;
        }
        while (p->pos < p->len) {
            jp_skip(p);
            char *k = jp_parse_str_raw(p);
            if (!k) { j_free(obj); return NULL; }
            jp_skip(p);
            if (p->pos >= p->len || p->s[p->pos] != ':') { free(k); j_free(obj); return NULL; }
            p->pos++;
            JVal *val = jp_parse_val(p);
            if (!val) { free(k); j_free(obj); return NULL; }
            j_obj_set(obj, k, val);
            free(k);
            jp_skip(p);
            if (p->pos < p->len && p->s[p->pos] == ',') { p->pos++; continue; }
            if (p->pos < p->len && p->s[p->pos] == '}') { p->pos++; return obj; }
            j_free(obj);
            return NULL;
        }
        j_free(obj);
        return NULL;
    }
    if (c == '[') {
        p->pos++;
        JVal *arr = j_new(J_ARR);
        jp_skip(p);
        if (p->pos < p->len && p->s[p->pos] == ']') {
            p->pos++;
            return arr;
        }
        while (p->pos < p->len) {
            JVal *item = jp_parse_val(p);
            if (!item) { j_free(arr); return NULL; }
            j_arr_push(arr, item);
            jp_skip(p);
            if (p->pos < p->len && p->s[p->pos] == ',') { p->pos++; continue; }
            if (p->pos < p->len && p->s[p->pos] == ']') { p->pos++; return arr; }
            j_free(arr);
            return NULL;
        }
        j_free(arr);
        return NULL;
    }
    if (strncmp(p->s + p->pos, "true", 4) == 0) {
        p->pos += 4;
        return j_bool(1);
    }
    if (strncmp(p->s + p->pos, "false", 5) == 0) {
        p->pos += 5;
        return j_bool(0);
    }
    if (strncmp(p->s + p->pos, "null", 4) == 0) {
        p->pos += 4;
        return j_new(J_NULL);
    }
    if (c == '-' || (c >= '0' && c <= '9')) {
        char *endptr = NULL;
        double d = strtod(p->s + p->pos, &endptr);
        if (endptr && endptr > p->s + p->pos) {
            p->pos = (size_t)(endptr - p->s);
            JVal *v = j_new(J_NUM);
            v->nval = d;
            return v;
        }
    }
    return NULL;
}

static JVal *j_parse(const char *src, size_t len) {
    JParser p;
    p.s = src;
    p.pos = 0;
    p.len = len;
    return jp_parse_val(&p);
}

#endif
