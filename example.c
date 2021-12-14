/**
 * MIT License
 * Copyright (c) 2022 FilipZajdel
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <cmap.h>

typedef struct str_s
{
    char bytes[8];
} str_t;

typedef struct
{
    const char *s;
} str2_t;

bool compare_chars(void *c1, void *c2)
{
    return *((char *)c1) == *((char *)c2);
}

bool compare_strings(void *str1, void *str2)
{
    return (0 == strcmp(((str_t*)str1)->bytes, ((str_t*)str2)->bytes));
}

bool compare_str2(void *s1, void *s2)
{
    str2_t *_s1 = (str2_t*)s1;
    str2_t *_s2 = (str2_t*)s2;

    return (_s1->s == _s2->s);
}

DECLARE_STATIC_MAP(char, unsigned, test_map, 20, compare_chars);

DECLARE_STATIC_MAP(str_t, unsigned, str_map, 20, compare_strings);

DECLARE_STATIC_MAP(str2_t, str_t, str2_map, 10, compare_str2);

void test_str2_t_str_t()
{
    str2_t keys[4] = {
        {"one"},
        {"two"},
        {"three"},
        {"four"}
    };

    str_t values[4] = {
        "ein", "zwei", "drei", "vier"
    };

    char str_buf[255];

    for (unsigned i=0; i<4; i++)
    {
        map_add((map_t*)&str2_map, (void*)&keys[i], (void*)&values[i]);
    }

    map_to_string((map_t*)&str2_map, str_buf);

    printf("%s\n", str_buf);

    str_t value;

    if (map_get(&str2_map.map, (void*)&keys[2], (void*)&value))
    {
        printf("Got %s\n", value.bytes);
    } else {
        printf("Couldn't get value for %s\n", keys[2].s);
    }

    strcpy(value.bytes, "update");

    map_update(&str2_map.map, (void*)&keys[2], (void*)&value, false);

    if (map_get(&str2_map.map, (void*)&keys[2], (void*)&value))
    {
        printf("Got %s\n", value.bytes);
    } else {
        printf("Couldn't get value for %s\n", keys[2].s);
    }
}

int main(int narg, char *agrs[])
{
    unsigned u = 20;
    str_t s1 = {"str4"};
    char map_as_string[255] = "";

    str_t keys[5] = {
        {"str1"},
        {"str2"},
        {"str3"},
        {"str4"},
        {"str5"}
    };

    for (unsigned i=0; i<5; i++)
    {
        map_add((map_t*)&str_map, (void*)&keys[i], &i);
    }

    if (map_get((map_t*)&str_map, (void*)&s1, (void*)&u))
    {
        printf("Got %s %u\n", s1.bytes, u);
    } else {
        printf("Could not get %s\n", s1.bytes);
    }

    map_to_string((map_t*)&str_map, map_as_string);

    printf("%s\n", map_as_string);

    map_erase((map_t*)&str_map);

    map_to_string((map_t*)&str_map, map_as_string);

    printf("%s\n", map_as_string);

    for (unsigned i=0; i<5; i++)
    {
        map_add((map_t*)&str_map, (void*)&keys[i], &i);
    }

    map_delete((map_t*)&str_map, (void*)&keys[3]);

    map_to_string((map_t*)&str_map, map_as_string);

    printf("%s\n", map_as_string);

    u = 20;
    map_add((map_t*)&str_map, (void*)&keys[3], (void*)&u);

    map_to_string((map_t*)&str_map, map_as_string);

    printf("%s\n", map_as_string);


    u = 20;
    bool result_add_dup = map_add((map_t*)&str_map, (void*)&keys[3], (void*)&u);

    printf("Result adding duplicate: %u\n", result_add_dup);

    map_to_string((map_t*)&str_map, map_as_string);

    printf("%s\n", map_as_string);

    test_str2_t_str_t();
}

