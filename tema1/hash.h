#pragma once

#include "utils.h"

/* ========================================================================= */

typedef struct _hash_node
{
    int hash;
    string key;
    string value;
    struct _hash_node* next;
} hash_node;

/* ------------------------------------------------------------------------- */

typedef struct
{
    hash_node* first;
    hash_node* last;
} hash_bucket;

/* ------------------------------------------------------------------------- */

typedef struct
{
    hash_bucket** bucket;
} hash_map;

/* ========================================================================= */

int hash_function(string str)
{
    int hash = 1, i;
    for (i = strlen(str) - 1; i >= 0; --i)
        hash ^= (int)str[i];
    return hash;
}

/* ------------------------------------------------------------------------- */

void add_node(hash_bucket* list, int hash, string key, string value)
{
    hash_node* i;

    if (list->first == NULL)
    {
        list->first = alloc(sizeof(hash_node));
        list->first->hash = hash;
        list->first->key = strdupl(key);
        list->first->value = strdupl(value);

        list->last = alloc(sizeof(hash_node));
        list->first->next = list->last;
    }
    else
    {
        for (i = list->first; i != list->last; i = i->next)
            if (i->hash == hash)
                if (strcmp(i->key, key) == 0)
                {
                    free(i->value);
                    i->value = strdupl(value);
                    return;
                }

        list->last->hash = hash;
        list->last->key = strdupl(key);
        list->last->value = strdupl(value);

        list->last->next = alloc(sizeof(hash_node));
        list->last = list->last->next;
    }
}

/* ------------------------------------------------------------------------- */

void insert(hash_map* map, string key, string value)
{
    int hash = hash_function(key);
    add_node(map->bucket[hash % HASH_MAP_SIZE], hash, key, value);
}

/* ------------------------------------------------------------------------- */

hash_map* create_hash_map()
{
    int i;
    hash_map* map = alloc(sizeof(hash_map));
    map->bucket = alloc(HASH_MAP_SIZE * sizeof(hash_bucket*));

    for (i = 0; i < HASH_MAP_SIZE; ++i)
    {
        map->bucket[i] = alloc(sizeof(hash_bucket));
        map->bucket[i]->first = NULL;
        map->bucket[i]->last = NULL;
    }

    return map;
}

/* ------------------------------------------------------------------------- */

string find_key(hash_map* map, string key)
{
    int hash = hash_function(key);
    int index = hash % HASH_MAP_SIZE;
    hash_node* i = map->bucket[index]->first;

    for (; i != map->bucket[index]->last; i = i->next)
        if (i->hash == hash)
            if (compare(i->key, key))
                return i->value;

    return NULL;
}

/* ------------------------------------------------------------------------- */

void remove_key(hash_map* map, string key)
{
    int hash = hash_function(key) % HASH_MAP_SIZE;
    hash_node* aux, *i = map->bucket[hash]->first;

    if (find_key(map, key) == NULL) return;

    if (compare(i->key, key))
    {
        aux = i->next;
        free(i->key);
        free(i->value);
        free(i);
        map->bucket[hash]->first = aux;
        return;
    }

    for (; i->next != map->bucket[hash]->last; i = i->next)
        if (compare(i->next->key, key))
        {
            aux = i->next->next;
            free(i->next->key);
            free(i->next->value);
            free(i->next);
            i->next = aux;
        }
}

/* ------------------------------------------------------------------------- */

void delete_hash_map(hash_map* map)
{
    int i;
    hash_node* j;
    hash_node* aux;

    for (i = 0; i < HASH_MAP_SIZE; ++i)
    {
        for (j = map->bucket[i]->first; j != map->bucket[i]->last;)
        {
            aux = j;
            j = j->next;
            free(aux->key);
            free(aux->value);
            free(aux);
        }

        free(map->bucket[i]->last);
        free(map->bucket[i]);
    }

    free(map->bucket);
    free(map);
}

/* ------------------------------------------------------------------------- */

int get_size(hash_map* map)
{
    int i, count = 0;
    hash_node* j;

    for (i = 0; i < HASH_MAP_SIZE; ++i)
        for (j = map->bucket[i]->first; j != map->bucket[i]->last; j = j->next)
            ++count;

    return count;
}

/* ------------------------------------------------------------------------- */

void insert_at(string vector[], string value, int position, int last)
{
    int i;
    for (i = last - 1; i > position; --i)
        vector[i] = vector[i - 1];
    vector[position] = value;
}

/* ------------------------------------------------------------------------- */

string* get_keys(hash_map* map)
{
    int i, k, size, key_size, count = 0;
    hash_node* j;
    string* keys;

    size = get_size(map);
    keys = alloc(size * sizeof(string));

    for (i = 0; i < size; ++i)
        keys[i] = NULL;

    for (i = 0; i < HASH_MAP_SIZE; ++i)
        for (j = map->bucket[i]->first; j != map->bucket[i]->last; j = j->next)
        {
            key_size = strlen(j->key);
            ++count;

            for (k = 0; k < count; ++k)
                if (keys[k] == NULL || strlen(keys[k]) <= key_size)
                {
                    insert_at(keys, j->key, k, count);
                    break;
                }
        }

    return keys;
}

/* ========================================================================= */