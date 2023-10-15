#include "picojson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL */
#include <math.h>
#include <string.h>

#ifndef PJSON_STACK_INIT_SIZE
#define PJSON_STACK_INIT_SIZE 256
#endif

#define EXPECT(c, ch)       do { assert(*c -> json == (ch)); c -> json++; } while(0)
#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')
#define PUTC(c, ch)         do { *(char *)pjson_context_push(c, sizeof(char)) = (ch); } while(0)

typedef struct {
    const char* json;
    char* stack;
    size_t size, top;
}pjson_context;

static void* pjson_context_push(pjson_context* context, size_t size) {
    void* ret;
    assert(size > 0);
    if(context -> top + size >= context -> size) {
        if(context -> size == 0) context -> size = PJSON_STACK_INIT_SIZE;
        while (context -> top + size >= context -> size) {
            context -> size += context -> size >> 1;
        }
        context -> stack = (char *)realloc(context -> stack, context -> size);
    }
    ret = context -> stack + context -> top;
    context -> top += size;
    return ret;
}

static void* pjson_context_pop(pjson_context* context, size_t size) {
    assert(context -> top >= size);
    return context -> stack + (context -> top -= size);
}

static void pjson_parse_whitespace(pjson_context* c) {
    const char *p = c -> json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c -> json = p;
}

static int pjson_parse_literal(pjson_context *context, pjson_value* value, const char* literal, pjson_type type) {
    size_t i;
    EXPECT(context, literal[0]);
    for(i = 0; literal[i + 1]; i++) {
        if(context -> json[i] != literal[i + 1]) return PJSON_PARSE_INVALID_VALUE;
    }
    context -> json += i;
    value -> type = type;
    return PJSON_PARSE_OK;
}

static int pjson_parse_number(pjson_context* c, pjson_value* v) {
    const char* p = c->json;
    if (*p == '-') p++;
    if (*p == '0') p++;
    else {
        if (!ISDIGIT1TO9(*p)) return PJSON_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    if (*p == '.') {
        p++;
        if (!ISDIGIT(*p)) return PJSON_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    if (*p == 'e' || *p == 'E') {
        p++;
        if (*p == '+' || *p == '-') p++;
        if (!ISDIGIT(*p)) return PJSON_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    errno = 0;
    v->u.number = strtod(c->json, NULL);
    if (errno == ERANGE && (v->u.number == HUGE_VAL || v->u.number == -HUGE_VAL))
        return PJSON_PARSE_NUMBER_TOO_BIG;
    v->type = PJSON_NUMBER;
    c->json = p;
    return PJSON_PARSE_OK;
}

static int pjson_parse_string(pjson_context* context, pjson_value* value) {
    size_t head = context -> top, len;
    const char* p;
    EXPECT(context, '\"');
    p = context -> json;
    for(;;) {
        char ch = *p++;
        switch (ch) {
            case '\"':
                len = context -> top - head;
                pjson_set_string(value, (const char*)pjson_context_pop(context, len), len);
                context -> json = p;
                return PJSON_PARSE_OK;
            case '\0':
                context -> top = head;
                return PJSON_PARSE_MISS_QUOTATION_MARK;
            default:
                PUTC(context, ch);
        }
    }

}

static int pjson_parse_value(pjson_context* context, pjson_value* value) {
    switch (*context -> json) {
        case 't':  return pjson_parse_literal(context, value, "true", PJSON_TRUE);
        case 'f':  return pjson_parse_literal(context, value, "false", PJSON_FALSE);
        case 'n':  return pjson_parse_literal(context, value, "null", PJSON_NULL);
        default:   return pjson_parse_number(context, value);
        case '\"': return pjson_parse_string(context, value);
        case '\0': return PJSON_PARSE_EXPECT_VALUE;
    }
}

int pjson_parse(pjson_value* value, const char* json) {
    pjson_context context;
    int ret;
    assert(value != NULL);
    context.json = json;
    context.stack = NULL;
    context.size = 0;
    pjson_init(value);
    pjson_parse_whitespace(&context);
    if ((ret = pjson_parse_value(&context, value)) == PJSON_PARSE_OK) {
        pjson_parse_whitespace(&context);
        if (*context.json != '\0')
            ret = PJSON_PARSE_ROOT_NOT_SINGULAR;
    }
    assert(context.top = 0);
    free(context.stack);
    return ret;
}

void pjson_free(pjson_value * value) {
    assert(value != NULL);
    if (value->type == PJSON_STRING)
        free(value->u.string.s);
    value -> type = PJSON_NULL;
}

pjson_type pjson_get_type(const pjson_value* json) {
    assert(json != NULL);
    return json -> type;
}

int pjson_get_boolean(const pjson_value* v) {
    /* \TODO */
    return 0;
}

void pjson_set_boolean(pjson_value* v, int b) {
    /* \TODO */
}

double pjson_get_number(const pjson_value* v) {
    assert(v != NULL && v->type == PJSON_NUMBER);
    return v->u.number;
}

void pjson_set_number(pjson_value* v, double n) {
    /* \TODO */
}

const char* pjson_get_string(const pjson_value* v) {
    assert(v != NULL && v->type == PJSON_STRING);
    return v->u.string.s;
}

size_t pjson_get_string_length(const pjson_value* v) {
    assert(v != NULL && v->type == PJSON_STRING);
    return v->u.string.len;
}

void pjson_set_string(pjson_value* v, const char* s, size_t len) {
    assert(v != NULL && (s != NULL || len == 0));
    pjson_free(v);
    v->u.string.s = (char*)malloc(len + 1);
    memcpy(v->u.string.s, s, len);
    v->u.string.s[len] = '\0';
    v->u.string.len = len;
    v->type = PJSON_STRING;
}