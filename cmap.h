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

#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t byte_t;
/**
 * @brief The callback type of a function used to compare keys in a map.
 *
 * @param a The pointer to the first element to compare.
 * @param b The pointer to the second element to compare.
 *
 * @return true is a and b are considered equal, false otherwise
 */
typedef bool (*compare_t)(void* a, void* b);
typedef struct map_s
{
    compare_t compare;
    unsigned key_size;
    unsigned val_size;
    unsigned count;
    const unsigned size;
} __attribute__((packed)) map_t;


/**
 * @brief Get a value of a given key in the map.
 *
 * @param[in]  map The pointer to the map object.
 * @param[in]  key The pointer to the searched key.
 * @param[out] val The pointer to hold the returned value.
 *
 * @return true if getting the value succeeded, false otherwise.
 */
bool map_get(map_t *map, void *key, void *val);

/**
 * @brief Insert new entry {key, val} into a map.
 *
 * @param[in] map The pointer to the map object.
 * @param[in] key The pointer to the new key.
 * @param[in] val The pointer to the new value.
 *
 * @return true if adding new entry succeded, false otherwise
 */
bool map_add(map_t *map, void *key, void *val);

/**
 * @brief Parse a map into a c-string.
 *
 * @param[in]  map The pointer to the map object.
 * @param[out] str The pointer to the string buffer.
 */
void map_to_string(map_t *map, char *str);

/**
 * @brief Get a full size of a map.
 *
 * @param[in] map The pointer to the map object.
 *
 * @return size of the map
 */
unsigned map_size(map_t *map);

/**
 * @brief Get a length of a map.
 *
 * @param[in] map The pointer to the map object.
 *
 * @return length of the map
 */
unsigned map_len(map_t *map);

/**
 * @brief Determine if map have no entries.
 *
 * @param[in] map The pointer to the map object.
 *
 * @return true if the map is empty, false otherwise.
 */
bool map_is_empty(map_t *map);

/**
 * @brief Remove all entries from the map.
 *
 * @param[in] map The pointer to the map object.
 */
void map_erase(map_t *map);

/**
 * @brief Delete entry of a given key in the map.
 *
 * @param[in] map The pointer to the map object.
 * @param[in] key The key of an entry to delete.
 */
void map_delete(map_t *map, void *key);

/**
 * @brief Update a value of the given key in the map.
 *
 * @param[in] map The pointer to the map object.
 * @param[in] key The key to update the value of.
 * @param[in] new_value The new value of a key.
 * @param[in] create_if_absent The flag to determine if new entry is created,
 *                             when map lacks given key.
 *
 * @return true if update (or adding new entry) succeded, false otherwise
 */
bool map_update(map_t *map, void *key, void *new_value, bool create_if_absent);

/**
 * @brief Declare static map object.
 *
 * @param key_type     The type of a keys in the map.
 * @param val_type     The type of a values in the map.
 * @param varname      The name of defined map object.
 * @param static_size  The maximum number of entries in the map.
 * @param compare_func The callback to function needed to compare keys of a given key_type
 */
#define DECLARE_STATIC_MAP(key_type, val_type, varname, static_size, compare_func)   \
struct map##key_type##val_type                                                       \
{                                                                                    \
    map_t map;                                                                       \
    key_type keys   [static_size];                                                   \
    val_type values [static_size];                                                   \
    byte_t usage    [static_size];                                                   \
} __attribute__((packed));                                                           \
struct map##key_type##val_type varname = {                                           \
    .map = {                                                                         \
        .key_size = sizeof(key_type),                                                \
        .val_size = sizeof(val_type),                                                \
        .count    = 0,                                                               \
        .size     = static_size,                                                     \
        .compare  = compare_func                                                     \
    },                                                                               \
}                                                                                    \

