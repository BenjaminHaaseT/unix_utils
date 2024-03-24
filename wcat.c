//
// Created by benjamin haase on 3/24/24.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Reads the file with the file name specified by `fname` and displays each line
 * on stdout. Returns an integer denoting the success/failure of the function
 * 0 for successful execution and non-zero otherwise.
 */
int print_file(char *fname);

/*
 * Reads a single line of variable length from the stream `fp` into `buf`, and returns an integer
 * representing the number of characters read. If the function returns 0, it may be that all lines have been read
 * or that the end of the file has been reached
 */
int read_line(FILE *fp, char *buf, size_t *buf_size);

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        int i = 1;
        while (i < argc)
        {
            if (print_file(argv[i++]))
            {
                exit(1);
            }
        }
    }

    return EXIT_SUCCESS;
}

int print_file(char *fname)
{
    FILE *fp = fopen(fname, "r");
    if (!fp)
    {
        printf("error reading file\n");
        return 1;
    }

    size_t buf_size = 32;
    char *buf = malloc(buf_size);

    while (read_line(fp, buf, &buf_size) > 0)
    {
        printf("%s", buf);

        buf_size = 32;
        char *next_buf = realloc(buf, buf_size);
        if (next_buf)
        {
            memset(next_buf, 0, 32);
            buf = next_buf;
        }
        else
        {
            printf("error reallocating buffer\n");
            return 1;
        }
    }

    free(buf);

    if (ferror(fp))
        return 1;

    return 0;
}

int read_line(FILE *fp, char *buf, size_t *buf_size)
{
    size_t i = 0;
    int cur_char;

    while ((cur_char = fgetc(fp)) != EOF)
    {
        // Check that we have enough space in buffer
        if (i == *buf_size)
        {
            *buf_size *= 2;
            char *new_buf = realloc(buf, *buf_size);
            if (new_buf)
            {
                buf = new_buf;
            }
            else
            {
                printf("error reallocating buffer\n");
                return 0;
            }
        }
        buf[i++] = (char) cur_char;
        if (cur_char == '\n')
            break;
    }

    if (i == *buf_size)
    {
        *buf_size *= 2;
        char *new_buf = realloc(buf, i + 1);
        if (new_buf)
            buf = new_buf;
        else
        {
            printf("error reallocating buffer\n");
            return 0;
        }
    }

    buf[i] = '\0';
    return (int) i;
}
