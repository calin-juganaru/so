#include "parser.h"

/* ========================================================================= */

int main(int argc, string argv[])
{
    int i, j, size;
    string input_file, output_file;
    string symbol, value, aux;
    file paths;
    hash_map* defines;
    stream output;
    file_t* file;
    bool found = false;

    aux = NULL;
    value = NULL;
    symbol = NULL;
    file = NULL;
    input_file = NULL;
    output_file = NULL;

    paths = create_file();
    if (paths == NULL)
    {
        exit(12);
    }
    pushback_line(paths, "./");

    defines = create_hash_map();
    if (defines == NULL)
    {
        delete_file(paths);
        exit(12);
    }
    insert(defines, "0", "0");

    /* --------------------------------------------------------------------- */

    for (i = 1; i < argc; ++i)
    {
        switch (argv[i][1])
        {
            /* ------------------------------------------------------------- */

            case 'D':
            {
                if (argv[i][2] != '\0' && argv[i][2] != ' ')
                {
                    insert(defines, argv[1] + 2, " ");
                    break;
                }

                for (j = 0, ++i; j < strlen(argv[i]); ++j)
                    if (argv[i][j] == '=')
                    {
                        argv[i][j] = '\0';
                        symbol = argv[i];
                        value = argv[i] + j + 1;
                    }

                if (symbol)
                    insert(defines, symbol, value);

                break;
            }

            /* ------------------------------------------------------------- */

            case 'I':
            {
                pushback_line(paths, argv[++i]);
                break;
            }

            /* ------------------------------------------------------------- */

            case 'o':
            {
                output_file = argv[++i];
                break;
            }

            /* ------------------------------------------------------------- */

            default:
            {
                if (argv[i][0] != '-')
                {
                    if (input_file == NULL)
                    {
                        input_file = argv[i];
                        break;
                    }

                    if (output_file == NULL)
                    {
                        output_file = argv[i];
                        break;
                    }

                    if (input_file != NULL && output_file != NULL)
                        exit(1);
                }
                break;
            }
        }
    }

    /* --------------------------------------------------------------------- */

    if (!is_null(input_file) && !file_exists(input_file))
        exit(EXIT_FAILURE);

    if (!is_null(input_file))
    {
        size = strlen(input_file);
        for (i = size - 1; i >= 0; --i)
            if (input_file[i] == '/')
            {
                found = true;
                break;
            }

        if (found)
        {
            aux = strdupl(input_file);
            aux[i + 1] = '\0';
            pushfront_line(paths, aux);
            delete(aux);
        }
    }

    /* --------------------------------------------------------------------- */

    file = parse_file(input_file, defines, paths);

    if (!is_null(output_file))
    {
        output = fopen(output_file, "w");
        print_file(file, output);
        fclose(output);
    }
    else print_file(file, stdout);

    /* --------------------------------------------------------------------- */

    delete_file(paths);
    delete_file(file);
    delete_hash_map(defines);

    return 0;
}

/* ========================================================================= */