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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <cmap.h>

#define USAGE_SINGLE_ENTRY_SIZE      sizeof(byte_t)

#define KEYS(p_map)     ((byte_t*)p_map + sizeof(*p_map))
#define VALUES(p_map)   ((KEYS(p_map) + ((p_map->key_size * p_map->size))))
#define USAGE(p_map)    (VALUES(p_map) + ((p_map->val_size * p_map->size)))

#define KEYS_ITER(map, keys_ptr, idx) (keys_ptr + (idx * map->key_size))
#define VALS_ITER(map, vals_ptr, idx) (vals_ptr + (idx * map->val_size))
#define USAGE_ITER(map, usage_ptr, idx) (usage_ptr + (idx * USAGE_SINGLE_ENTRY_SIZE))

#define IS_ENTRY_FREE(map, usage_ptr, idx) (*USAGE_ITER(map, usage_ptr, idx) == MAP_ENTRY_FREE)

#define MAP_ENTRY_FREE (0)
#define MAP_ENTRY_USED (1)


/** @brief Find an index of a {key, val} in the map
 *
 * @param[in]  map   The pointer to the map object.
 * @param[in]  key   The pointer to the searched key.
 * @param[out] idx   The pointer to variable that will be used to pass the index
 *                   of a found key.
 *
 * @return true if key was found, false otherwise
 */
static inline bool map_find(map_t *map, void *key, unsigned *idx)
{
    byte_t *keys = KEYS(map);
    byte_t *usage = USAGE(map);
    bool found = false;

    for (unsigned itr = 0; itr < map->count; itr++)
    {
        if (IS_ENTRY_FREE(map, usage, itr)) { continue; }

        if (map->compare(key, KEYS_ITER(map, keys, itr)))
        {
            found = true;
            *idx = itr;
            break;
        }
    }

    return found;
}

/**
 * @brief Write bytes in a hex format to the c-string.
 *
 * @param[in]  bytes  The data to parse.
 * @param[in]  nbytes The number of bytes to parse.
 * @param[out] out    The out buffer to save c-string.
 *
 * @return number of characters written to out
 */
static inline unsigned hexdump_str(byte_t *bytes, unsigned nbytes, char *out)
{
    unsigned count = 0;

    for (unsigned i = 0; i < nbytes; i++) {
        count += sprintf(out + count, "%.2x", bytes[i]);
    }

    return count;
}

/**
 * @brief Attempt to alloc a new entry
 *
 * @param[in]  map The pointer to the map object.
 * @param[out] free_idx The pointer to variable which will be used to pass a free entry.
 *
 * @return true if allocation succeeded, false otherwise
 */
static bool map_alloc_new_entry(map_t *map, unsigned *free_idx)
{
    bool found = false;
    byte_t *usage = USAGE(map);

    for (unsigned idx = 0; idx < map->size; idx++)
    {
        if (IS_ENTRY_FREE(map, usage, idx))
        {
            found = true;
            *free_idx = idx;
            break;
        }
    }

    return found;
}

bool map_add(map_t *map, void *key, void *val)
{
    unsigned idx;

    if (map_find(map, key, &idx))
    {
	// reject duplicates	
        return false;
    }

    if (!map_alloc_new_entry(map, &idx))
    {
        return false;
    }

    memcpy(KEYS_ITER(map, KEYS(map), idx) , key, map->key_size);
    memcpy(VALS_ITER(map, VALUES(map), idx), val, map->val_size);
    *USAGE_ITER(map, USAGE(map), idx) = MAP_ENTRY_USED;

    map->count++;

    return true;
}

bool map_get(map_t *map, void *key, void *val)
{
    unsigned idx;
    bool found = map_find(map, key, &idx);

    if (found)
    {
        memcpy(val, VALS_ITER(map, VALUES(map), idx), map->val_size);
    }

    return found;
}

void map_to_string(map_t *map, char *str)
{
    byte_t *keys = KEYS(map);
    byte_t *values = VALUES(map);
    byte_t *usage = USAGE(map);

    unsigned count = sprintf(str, "<map size(%u) len(%u) items({",
                             map_size(map), map_len(map));

    for (unsigned i = 0, added_entries = 0; i < map->size; i++)
    {
        if (IS_ENTRY_FREE(map, usage, i)) { continue; }

        count += sprintf(str + count, "\"");
        count += hexdump_str(keys+(i*map->key_size), map->key_size, str+count);
        count += sprintf(str+count, "\" : \"");
        count += hexdump_str(values+(i*map->val_size), map->val_size, str+count);
        count += sprintf(str+count, (added_entries == (map->count-1)) ? "\"" : "\", ");

        added_entries++;
    }

    count += sprintf(str+count, "})>");
}

unsigned map_size(map_t *map)
{
    return map->size;
}

unsigned map_len(map_t *map)
{
    return map->count;
}

bool map_is_empty(map_t *map)
{
    return map_len(map) == 0;
}

void map_erase(map_t *map)
{
    memset(USAGE(map), MAP_ENTRY_FREE, map->size + USAGE_SINGLE_ENTRY_SIZE);
    map->count = 0;
}

void map_delete(map_t *map,  void *key)
{
    unsigned idx;

    if (map_find(map, key, &idx))
    {
        *USAGE_ITER(map, USAGE(map), idx) = MAP_ENTRY_FREE;
        map->count--;
    }
}

bool map_update(map_t *map, void *key, void *new_value, bool create_if_absent)
{
    bool updated = false;
    unsigned idx;

    if (map_find(map, key, &idx))
    {
        memcpy(VALS_ITER(map, VALUES(map), idx), new_value, map->val_size);
        updated = true;
    } else if (create_if_absent)
    {
        updated = map_add(map, key, new_value);
    }

    return updated;
}

