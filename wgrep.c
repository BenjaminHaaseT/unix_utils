//
// Created by benjamin haase on 3/26/24.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int search_file(FILE *fp, char *search_term);
char *read_line(FILE *fp);

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        puts("wgrep: searchterm [file]\n");
        exit(1);
    }

    char* search_term = argv[1];

    // check if search term is the empty string
    if (strlen(search_term) == 0)
        goto successful_termination;

    if (argc == 2)
    {
        if (search_file(stdin, search_term))
            exit(1);
    }
    else
    {
        // now parse the files passed in
        for (int i = 2; i < argc; i++)
        {
            FILE *fp = fopen(argv[i], "r");
            if (!fp)
            {
                printf("wgrep cannot open file: %s\n", argv[i]);
                exit(1);
            }
            else
            {
                // check for errors bubbling up
                if (search_file(fp, search_term))
                    exit(1);
            }
        }
    }

    successful_termination:
    return EXIT_SUCCESS;
}

int search_file(FILE *fp, char *search_term)
{
    // read the file line by line
    char *line = NULL;
    while ((line = read_line(fp)))
    {
        if (strstr(line, search_term))
        {
            printf("%s\n", line);
        }

        free(line);
    }

    // check for errors in the stream
    if (ferror(fp))
    {
        clearerr(fp);
        return 1;
    }

    return 0;
}

char *read_line(FILE *fp)
{
    size_t buf_size = 64;
    char *buf = malloc(buf_size);
    size_t i = 0;
    int cur_char;

    while ((cur_char = fgetc(fp)) != EOF)
    {
        if (i == buf_size)
        {
            buf_size *= 2;
            char *new_buf = realloc(buf, buf_size);
            if (!new_buf)
            {
                fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }

            buf = new_buf;
        }

        buf[i++] = (char) cur_char;
        if (cur_char == '\n')
            break;
    }

    if (ferror(fp) || (cur_char != '\n' && !feof(fp)))
    {
        fprintf(stderr, "%s:%s:%d error reading from file stream\n", __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    } else if (!i)
    {
        return NULL;
    }

    char *new_buf = realloc(buf, i + 1);
    if (!new_buf)
    {
        fprintf(stderr, "%s:%s:%d error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }

    buf = new_buf;
    buf[i] = '\0';
    return buf;
}
