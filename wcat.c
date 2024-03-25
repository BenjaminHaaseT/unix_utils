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
 * Reads a single line of variable length from the stream `fp` and returns a string.
 * If execution is unsuccessful for any reason, the function will return NULL, otherwise it will return the
 * current line of the file as a char*.
 */
char *read_line(FILE *fp);

int main(int argc, char **argv)
{
    printf("%d\n", argc);
    if (argc > 1)
    {
        int i = 1;
        while (i < argc)
        {
            printf("FILE: %s\n", argv[i]);
            if (print_file(argv[i++]))
            {
                exit(1);
            }
            printf("\n\n");
        }
    }

    return EXIT_SUCCESS;
}

int print_file(char *fname)
{
    FILE *fp = fopen(fname, "r");
    if (!fp)
    {
        fprintf(stderr, "error reading file: %s\n", fname);
        return 1;
    }

    char *line = NULL;
    while ((line = read_line(fp)))
    {
        printf("%s", line);
        free(line);
    }

    if (ferror(fp))
    {
        clearerr(fp);
        fclose(fp);
        return 1;
    }

    fclose(fp);
    return 0;
}

char *read_line(FILE *fp)
{
    size_t buf_size = 32;
    char *buf = malloc(buf_size);
    size_t i = 0;
    int cur_char;

    while ((cur_char = fgetc(fp)) != EOF)
    {
        if (i == buf_size)
        {
            buf_size *= 2;
            char *new_buf = realloc(buf, buf_size);
            if (new_buf)
                buf = new_buf;
            else
            {
                fprintf(stderr, "[%s:%s:%d] error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }
        }

        buf[i++] = (char) cur_char;
        if (cur_char == '\n')
            break;
    }

    if (ferror(fp) || (cur_char != '\n' && !feof(fp)))
    {
        fprintf(stderr, "[%s:%s:%d] error reading from file\n", __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }

    if (!i)
    {
        free(buf);
        return NULL;
    }

    if (i == buf_size)
    {
        char *new_buf = realloc(buf, i + 1);
        if (new_buf)
            buf = new_buf;
        else
        {
            fprintf(stderr, "[%s:%s:%d] error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
            return NULL;
        }
    }

    buf[i] = '\0';
    return buf;
}
