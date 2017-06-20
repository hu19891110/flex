// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See the LICENSE.txt file in the project root
// for the license information.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <tgmath.h>

#include "flex.h"

static const char *current_unit = NULL;
#define MAX_FAILURES 100
static const char *failures[MAX_FAILURES];
static unsigned int failures_n = 0;

#define _PASS() printf(".")
#define _FAIL(fmt, ...) \
    do { \
        if (failures_n < MAX_FAILURES) { \
            printf("E"); \
            char _buf[100] = { 0 }; \
            int _p = snprintf(_buf, sizeof(_buf), "%s (%s:%d): ", \
                    current_unit, __FILE__, __LINE__); \
            snprintf(_buf + _p, sizeof(_buf) - _p, fmt, __VA_ARGS__); \
            failures[failures_n++] = strdup(_buf); \
        } \
    } \
    while (0)

#define TEST(expr) \
    do { \
        if (expr) { \
            _PASS(); \
        } \
        else { \
            _FAIL("failed test `%s'", #expr); \
        } \
    } \
    while (0)

#define TEST_EQUAL_T(expr, val, type_fmt) \
    do { \
        __auto_type _res = expr; \
        typeof(_res) _val = val; \
        if (_res == _val) { \
            _PASS(); \
        } \
        else { \
            char _fmt[100] = { 0 }; \
            snprintf(_fmt, sizeof _fmt, "failed test `%%s == %s' (is %s)", \
                    type_fmt, type_fmt); \
            _FAIL(_fmt, #expr, _val, _res); \
        } \
    } \
    while (0)

#define TEST_EQUAL(expr, val) \
    TEST_EQUAL_T(expr, val, \
            (_Generic((expr), \
                      unsigned int: "%u", \
                      int: "%d", \
                      float: "%0.1f")))

#define TEST_FRAME_EQUAL(item, x, y, width, height) \
    do { \
        struct flex_item *_item = item; \
        TEST_EQUAL(flex_item_get_frame_x(_item), x); \
        TEST_EQUAL(flex_item_get_frame_y(_item), y); \
        TEST_EQUAL(flex_item_get_frame_width(_item), width); \
        TEST_EQUAL(flex_item_get_frame_height(_item), height); \
    } \
    while (0)

static struct flex_item *
flex_item_with_size(float width, float height)
{
    struct flex_item *item = flex_item_new();
    flex_item_set_width(item, width);
    flex_item_set_height(item, height);
    return item;
}

#include "units.c"

int
main(void)
{
    void run_all_units();
    run_all_units();

    if (failures_n > 0) {
        printf("\n");
        for (int i = 0; i < failures_n; i++) {
            printf("%s\n", failures[i]);
        }
        return 1;
    }
    printf(" OK\n");
    return 0;
}
