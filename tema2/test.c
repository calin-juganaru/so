#include "so_stdio.h"

const int N = 9;

int main()
{
    int i, count;
    char buf[SO_BUFFER_SIZE];
    SO_FILE* in, *out;

    in = so_fopen("test.in", "r");
    out = so_fopen("test.out", "w");

    so_fread(buf, 1, SO_BUFFER_SIZE, in);
    so_fwrite(buf, 1, SO_BUFFER_SIZE, out);

    so_fread(buf, 1, SO_BUFFER_SIZE, in);
    so_fwrite(buf, 1, SO_BUFFER_SIZE, out);

    so_fread(buf, 1, SO_BUFFER_SIZE, in);


    so_fclose(in);
    so_fclose(out);

    return 0;
}