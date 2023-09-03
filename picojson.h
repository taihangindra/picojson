//
// Created by sagasiegfried on 2023/8/22.
//

#ifndef PICOJSON_PICOJSON_H
#define PICOJSON_PICOJSON_H

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
    double number;
}pjson_value;

enum {
    PJSON_PARSE_OK = 0,
    PJSON_PARSE_EXPECT_VALUE,
    PJSON_PARSE_INVALID_VALUE,
    PJSON_PARSE_ROOT_NOT_SINGULAR,
    PJSON_PARSE_NUMBER_TOO_BIG
};

int pjson_parse(pjson_value* value, const char* json);

pjson_type pjson_get_type(const pjson_value* json);

double pjson_get_number(const pjson_value* json);

#endif //PICOJSON_PICOJSON_H
