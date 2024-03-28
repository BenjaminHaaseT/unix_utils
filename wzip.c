//
// Created by benjamin haase on 3/26/24.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int process_file(char* fname);
char* read_line(FILE* fp);
char* rle_encode(char* line);

int main(int argc, char** argv)
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

    return EXIT_SUCCESS;
}

int process_file(char* fname)
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
        //TODO: remove, for debugging only
//        printf("%s", line);

        char* rle_enc_line = NULL;
        if ((rle_enc_line = rle_encode(line)))
        {
            printf("%s", rle_enc_line);
            free(rle_enc_line);
        }
        else
        {
            free(line);
            fprintf(stderr, "%s:%s:%d - error encoding line\n", __FILE__, __FUNCTION__, __LINE__);
            return 1;
        }

        free(line);
    }

    // ensure we have successfully finished reading the file i.e. we have not broken the loop due to errors

    return 0;
}

char* read_line(FILE* fp)
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

char* rle_encode(char* line)
{
    size_t len = strlen(line);
    size_t buf_size = 32;
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
        size_t count_buf_size = 10;
        size_t count_buf_idx = 0;
//        printf("count - %d\n", count);

        // for reading the digits of count
        char* count_buf = malloc(count_buf_size);

        // Transform the count into a sequence of 1 byte digits, stored in count_buf
        while (count)
        {
            char digit = ((char) (count % 10)) + '0';
//            printf("%c\n", digit);
            if (count_buf_idx == count_buf_size)
            {
                count_buf_size *= 2;
                char* new_count_buf = realloc(count_buf, count_buf_size);
                if (!new_count_buf)
                {
                    fprintf(stderr, "%s:%s:%d - error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
                    return NULL;
                }

                count_buf = new_count_buf;
            }

            count_buf[count_buf_idx++] = digit;
            count /= 10;
        }

        count_buf_idx--;

        while (count_buf_idx)
        {
            // write digits in reverse order to the buffer
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

            buf[buf_idx++] = count_buf[count_buf_idx--];
        }

        // write the last digit in count_buf
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

        buf[buf_idx++] = count_buf[count_buf_idx];

        // now attempt to write the current character at index i
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

        buf[buf_idx++] = line[i];
        i = j;
    }

    // if we make it here we have successfully processed the line
    char* temp_buf = realloc(buf, buf_idx + 1);
    if (!temp_buf)
    {
        fprintf(stderr, "%s:%s:%d - error reallocating buffer\n", __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    buf = temp_buf;
    buf[buf_idx] = '\0';

    return buf;
}



