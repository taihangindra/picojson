//
// Created by sagasiegfried on 2023/8/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "picojson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)
#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")

static void test_parse_null() {
    pjson_value v;
    v.type = PJSON_FALSE;
    EXPECT_EQ_INT(PJSON_PARSE_OK, pjson_parse(&v, "null"));
    EXPECT_EQ_INT(PJSON_NULL, pjson_get_type(&v));
}

static void test_parse_true() {
    pjson_value v;
    v.type = PJSON_FALSE;
    EXPECT_EQ_INT(PJSON_PARSE_OK, pjson_parse(&v, "true"));
    EXPECT_EQ_INT(PJSON_TRUE, pjson_get_type(&v));
}

static void test_parse_false() {
    pjson_value v;
    v.type = PJSON_TRUE;
    EXPECT_EQ_INT(PJSON_PARSE_OK, pjson_parse(&v, "false"));
    EXPECT_EQ_INT(PJSON_FALSE, pjson_get_type(&v));
}

#define TEST_NUMBER(expect, json)\
    do {\
        pjson_value v;\
        EXPECT_EQ_INT(PJSON_PARSE_OK, pjson_parse(&v, json));\
        EXPECT_EQ_INT(PJSON_NUMBER, pjson_get_type(&v));\
        EXPECT_EQ_DOUBLE(expect, pjson_get_number(&v));\
    } while(0)

static void test_parse_number() {
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000"); /* must underflow */
}

static void test_parse() {
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_number();
}

int main() {
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}