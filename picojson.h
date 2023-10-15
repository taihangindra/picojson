//
// Created by sagasiegfried on 2023/8/22.
//

#ifndef PICOJSON_PICOJSON_H
#define PICOJSON_PICOJSON_H

#include <stddef.h> /* using size_t */

typedef enum{
    PJSON_NULL = 0,
    PJSON_FALSE,
    PJSON_TRUE,
    PJSON_NUMBER,
    PJSON_STRING,
    PJSON_ARRAY,
    PJSON_OBJECT,
}pjson_type;

typedef struct {
    pjson_type type;
    union {
       double number;
       struct {char* s; size_t len; } string;
    } u;
}pjson_value;

enum {
    PJSON_PARSE_OK = 0,
    PJSON_PARSE_EXPECT_VALUE,
    PJSON_PARSE_INVALID_VALUE,
    PJSON_PARSE_ROOT_NOT_SINGULAR,
    PJSON_PARSE_NUMBER_TOO_BIG,
    PJSON_PARSE_MISS_QUOTATION_MARK
};

#define pjson_init(v) do { (v) -> type = PJSON_NULL; } while(0)

int pjson_parse(pjson_value* value, const char* json);

void pjson_free(pjson_value *value);

pjson_type pjson_get_type(const pjson_value* json);

#define pjson_set_null(v) do { pjson_free(v); } while(0)

int pjson_get_boolean(const pjson_value *value);
void pjson_set_boolean(pjson_value *value, int boolean);

double pjson_get_number(const pjson_value* json);
void pjson_set_number(pjson_value *value, double number);

const char *pjson_get_string(const pjson_value *value);
size_t pjson_get_string_length(const pjson_value *value);
void pjson_set_string(pjson_value *value, const char* string, size_t length);

#endif //PICOJSON_PICOJSON_H
