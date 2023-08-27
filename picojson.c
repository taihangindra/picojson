#include "picojson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL */

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
    const char* json;
}pjson_context;

static void pjson_parse_whitespace(pjson_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int pjson_parse_true(pjson_context* c, pjson_value* v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return PJSON_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = PJSON_TRUE;
    return PJSON_PARSE_OK;
}

static int pjson_parse_false(pjson_context* c, pjson_value* v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return PJSON_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = PJSON_FALSE;
    return PJSON_PARSE_OK;
}

static int pjson_parse_null(pjson_context* c, pjson_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return PJSON_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = PJSON_NULL;
    return PJSON_PARSE_OK;
}

static int pjson_parse_value(pjson_context* c, pjson_value* v) {
    switch (*c->json) {
        case 't':  return pjson_parse_true(c, v);
        case 'f':  return pjson_parse_false(c, v);
        case 'n':  return pjson_parse_null(c, v);
        case '\0': return PJSON_PARSE_EXPECT_VALUE;
        default:   return PJSON_PARSE_INVALID_VALUE;
    }
}

int pjson_parse(pjson_value* v, const char* json) {
    pjson_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = PJSON_NULL;
    pjson_parse_whitespace(&c);
    if ((ret = pjson_parse_value(&c, v)) == PJSON_PARSE_OK) {
        pjson_parse_whitespace(&c);
        if (*c.json != '\0')
            ret = PJSON_PARSE_ROOT_NOT_SINGULAR;
    }
    return ret;
}

pjson_type pjson_get_type(const pjson_value* v) {
    assert(v != NULL);
    return v->type;
}