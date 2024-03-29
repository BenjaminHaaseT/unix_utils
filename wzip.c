//
// Created by benjamin haase on 3/26/24.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

int process_file(char* fname);
char *read_line(FILE* fp);
char *rle_encode(char* line);

int main(int argc, char **argv)
{
    // Check that at least one file has been passed to the program
    if (argc == 1)
    {
        fprintf(stderr, "wzip: file1 [file2,...]\n");
        exit(1);
    }

    // While there are still more files to be processed, process the file
    int fcount = 1;
    while (fcount < argc)
    {
        if (process_file(argv[fcount++]))
        {
            exit(1);
        }
    }

    printf("\n");
    return EXIT_SUCCESS;
}

int process_file(char *fname)
{
    FILE* fp = fopen(fname, "r");
    if (!fp)
    {
        fprintf(stderr, "%s:%s:%d - wzip is unable to open file %s\n", __FILE__, __FUNCTION__, __LINE__, fname);
        return 1;
    }

    // for reading a line from the file
    char* line = NULL;

    while ((line = read_line(fp)))
    {
        char* buf = rle_encode(line);
        printf("%s", buf);
        free(buf);
        free(line);
    }

    // ensure we have successfully finished reading the file i.e. we have not broken the loop due to errors
    if (ferror(fp) || !feof(fp))
        return 1;

    return 0;
}

char *read_line(FILE* fp)
{
    size_t buf_size = 64;
    size_t buf_idx = 0;
    char* buf = malloc(buf_size);

    // for the current char of the file
    int cur_char = 0;

    while ((cur_char = fgetc(fp)) != EOF)
    {
        if (buf_idx == buf_size)
        {
            buf_size *= 2;
            char* new_buf = realloc(buf, buf_size);
            if (!new_buf)
            {
                fprintf(stderr, "%s:%s:%d - error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }

            buf = new_buf;
        }

        buf[buf_idx++] = (char) cur_char;
        if (cur_char == '\n')
            break;
    }

    if (ferror(fp))
    {
        fprintf(stderr, "%s:%s:%d - error reading from file\n", __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    else if (!buf_idx)
    {
        return NULL;
    }

    char* new_buf = realloc(buf, buf_idx + 1);
    if (!new_buf)
    {
        fprintf(stderr, "%s:%s:%d - error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }

    buf = new_buf;
    buf[buf_idx] = '\0';
    return buf;
}

char *rle_encode(char *line)
{
    size_t len = strlen(line);
    size_t buf_size = 64;
    size_t buf_idx = 0;
    char* buf = malloc(buf_size);

    size_t i = 0;
    while (i < len)
    {
        // Read as many characters that are equal to current character at index i
        size_t j = i;
        while (j < len && line[i] == line[j])
            j++;

        // store the total count, of the current character at index
        int count = (int) (j - i);
        size_t count_bit_sz = sizeof(int) * CHAR_BIT;

        for (int k = count_bit_sz - 1; k > -1; k--)
        {
            char c = '0' + ((count >> k) & 1);
            if (buf_idx == buf_size)
            {
                buf_size *= 2;
                char *new_buf = realloc(buf, buf_size);
                if (!new_buf)
                {
                    fprintf(stderr, "%s:%s:%d - error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
                    return NULL;
                }

                buf = new_buf;
            }

            buf[buf_idx++] = c;
        }

        if (buf_idx == buf_size)
        {
            buf_size *= 2;
            char *new_buf = realloc(buf, buf_size);
            if (!new_buf)
            {
                fprintf(stderr, "%s:%s:%d - error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
                return NULL;
            }

            buf = new_buf;
        }

        buf[buf_idx++] = line[i];
        i = j;
    }

    // Resize the buffer to new size
    char *new_buf = realloc(buf, buf_idx + 1);
    if (!new_buf)
    {
        fprintf(stderr, "%s:%s:%d - error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }

    buf = new_buf;
    buf[buf_idx] = '\0';
    return buf;
}



