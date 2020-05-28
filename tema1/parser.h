#pragma once

#include "hash.h"
#include "io.h"

/* ========================================================================= */

file_t* parse_line(string line, hash_map* defines, file paths);

file_t* parse_file(string input_file, hash_map* defines, file paths);

/* ========================================================================= */

/* replace in s the occurence of x at position pos with y */
bool replace(string s, string x, string y, int pos)
{
    int size;
    string aux;

    size = strlen(s) + strlen(y) + MAX_LINE_SIZE / 2;
    aux = alloc(size);

    if (aux == NULL) return false;

    memset(aux, 0, size);
    strncpy(aux, s, pos);
    strncpy(aux + pos, y, strlen(y));

    strncpy(aux + pos + strlen(y), s + pos + strlen(x),
            strlen(s) - pos + strlen(y) - 2);
    aux[strlen(s) + strlen(y) - strlen(x)] = '\0';

    strcpy(s, aux);
    delete(aux);

    return true;
}

/* ========================================================================= */

file_t* parse_line(string line, hash_map* defines, file paths)
{
    static string key = NULL;
    static string temp = NULL;
    static bool define = false;
    static bool if_def = false;
    static bool condition = false;

    int x = 0, i, size, pos;
    bool success, valid_header = false;
    string aux, word, copy;
    string* keys;
    string result;
    list it;
    file_t* file = NULL;

    copy = strdupl(line);

    /* --------------------------------------------------------------------- */

    if (define == true)
    {
        if (copy[strlen(copy) - 2] == '\\')
        {
            copy[strlen(copy) - 2] = '\0';
            size = strlen(copy) - 1;

            for (i = 0; i < size; ++i)
                if (copy[i + 1] != ' ') break;

            strcat(temp, copy + i);
        }
        else
        {
            define = false;

            copy[strlen(copy) - 1] = '\0';
            size = strlen(copy) - 1;

            for (i = 0; i < size; ++i)
                if (copy[i + 1] != ' ') break;

            strcat(temp, copy + i);
            insert(defines, key, temp);

            delete(key);
            delete(temp);
        }

        delete(copy);
        null(line);
        return NULL;
    }

    /* --------------------------------------------------------------------- */

    for (word = strtok(copy, tokens);
         word != NULL; word = strtok(NULL, tokens))
    {
        /* ----------------------------------------------------------------- */

        _base_case(word, "define")
        {
            aux = strtok(NULL, " \t");
            word = strtok(NULL, "\n");

            if (is_null(word))
            {
                insert(defines, aux, " ");
                null(line);
                break;
            }

            if (if_def && !condition)
            {
                null(line);
                break;
            }

            /* ------------------------------------------------------------- */

            keys = get_keys(defines);
            size = get_size(defines);

            for (i = 0; i < size; ++i)
            {
                result = strstr(word, keys[i]);

                if (result != NULL && result != keys[i])
                {
                    pos = (int)(result - word);
                    success = replace(word, keys[i], find_key(defines, keys[i]), pos);

                    if (!success)
                    {
                        delete_file(paths);
                        delete_hash_map(defines);
                        delete(copy);
                        delete(line);
                        delete(keys);
                        exit(12);
                    }
                }
            }

            delete(keys);

            /* ------------------------------------------------------------- */

            if (word[strlen(word) - 1] == '\\')
            {
                define = true;

                if (word[strlen(word) - 2] == ' ')
                     word[strlen(word) - 2] = '\0';
                else word[strlen(word) - 1] = '\0';

                temp = alloc(MAX_LINE_SIZE);
                strncpy(temp, word, strlen(word) + 1);
                key = strdupl(aux);
                null(line);

                break;
            }

            /* ------------------------------------------------------------- */

            if (!define)
                insert(defines, aux, word);

            null(line);
        }

        /* ----------------------------------------------------------------- */

        _case(word, "undef")
        {
            word = strtok(NULL, tokens);
            remove_key(defines, word);
            null(line);
        }

        /* ----------------------------------------------------------------- */

        _case(word, "if")
        {
            word = strtok(NULL, tokens);
            aux = find_key(defines, word);

            if (aux != NULL)
                 condition = str_to_bool(aux);
            else condition = str_to_bool(word);

            if_def = true;
            null(line);
        }

        /* ----------------------------------------------------------------- */

        _case(word, "endif")
        {
            if_def = false;
            condition = false;
            null(line);
        }

        /* ----------------------------------------------------------------- */

        _case(word, "else")
        {
            if_def = true;
            condition = !condition;
            null(line);
        }

        /* ----------------------------------------------------------------- */

        _case(word, "elif")
        {
            word = strtok(NULL, tokens);
            aux = find_key(defines, word);

            if_def = true;
            condition = str_to_bool(aux);
            null(line);
        }

        /* ----------------------------------------------------------------- */

        _case(word, "ifdef")
        {
            word = strtok(NULL, tokens);
            condition = !is_null(find_key(defines, word));
            if_def = true;
            null(line);
        }

        /* ----------------------------------------------------------------- */

        _case(word, "ifndef")
        {
            word = strtok(NULL, tokens);
            condition = is_null(find_key(defines, word));
            if_def = true;
            null(line);
        }

        /* ----------------------------------------------------------------- */

        else if (compare(word, "include"))
        {
            word = strtok(NULL, " \t<>\"");
            aux = alloc(MAX_LINE_SIZE / 2);

            for (it = paths->first; it != paths->last; it = it->next)
            {
                strncpy(aux, it->value, strlen(it->value) + 1);
                strcat(aux, "/");
                strncat(aux, word, strlen(word) + 1);

                if (file_exists(aux))
                {
                    file = parse_file(aux, defines, paths);
                    valid_header = true;
                    break;
                }
            }

            if (!valid_header)
            {
                delete(aux);
                null(line);
                delete(copy);
                delete_file(paths);
                delete_hash_map(defines);
                exit(1);
            }

            delete(aux);
            null(line);
            break;
        }

        /* ----------------------------------------------------------------- */

        _default
        {
            if (if_def && !condition) null(line);
            else
            {
                aux = NULL;
                if (compare(word, "0")) break;
                aux = find_key(defines, word);

                if (aux != NULL && !is_null(line) && !is_null(word))
                {
                    success = replace(line, word, aux, word - copy - x);
                    if (!success)
                    {
                        delete_file(paths);
                        delete_hash_map(defines);
                        delete(copy);
                        delete(line);
                        exit(12);
                    }
                    x += strlen(word) - strlen(aux);
                }
            }
        }
    }

    delete(copy);
    return file;
}

/* ========================================================================= */

file_t* parse_file(string input_file, hash_map* defines, file paths)
{
    string buffer;
    stream input;
    file_t* aux_file;
    file_t* result;
    line_t* it;

    /* --------------------------------------------------------------------- */

    if (!is_null(input_file))
        input = fopen(input_file, "r");

    aux_file = create_file();
    if (aux_file == NULL)
    {
        delete_file(paths);
        delete_hash_map(defines);
    }

    buffer = alloc(MAX_LINE_SIZE * sizeof(char));
    if (buffer == NULL)
    {
        delete_file(aux_file);
        delete_file(paths);
        delete_hash_map(defines);
    }

    /* --------------------------------------------------------------------- */

    if (is_null(input_file))
        while (fgets(buffer, MAX_LINE_SIZE, stdin) != NULL)
        {
            if (!is_null(buffer))
            {
                result = parse_line(buffer, defines, paths);

                if (result != NULL)
                {
                    it = result->first;
                    for (; it != result->last; it = it->next)
                        pushback_line(aux_file, it->value);
                    delete_file(result);
                }
            }

            if (!is_null(buffer))
                pushback_line(aux_file, buffer);
        }

    /* --------------------------------------------------------------------- */

    else while (fgets(buffer, MAX_LINE_SIZE - 1, input) != NULL)
        {
            if (!is_null(buffer))
            {
                result = parse_line(buffer, defines, paths);

                if (result != NULL)
                {
                    it = result->first;
                    for (; it != result->last; it = it->next)
                        pushback_line(aux_file, it->value);
                    delete_file(result);
                }
            }

            if (!is_null(buffer))
            {
                pushback_line(aux_file, buffer);
            }
        }

    /* --------------------------------------------------------------------- */

    delete(buffer);

    if (!is_null(input_file))
        fclose(input);

    return aux_file;
}

/* ========================================================================= */