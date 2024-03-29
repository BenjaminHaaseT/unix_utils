//
// Created by benjamin haase on 3/29/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int decode_file(char *fname);
char *decode_line(FILE *fp);

int main(int argc, char **argv)
{
    // validate cli arguments
    if (argc == 1)
    {
        fprintf(stderr, "wunzip: file1 [file2,...] \n");
        exit(1);
    }

    // attempt to decode each file passed
    for (int fcount = 1; fcount < argc; fcount++)
    {
        if (decode_file(argv[fcount]))
        {
            exit(1);
        }
    }

    return EXIT_SUCCESS;
}

int decode_file(char *fname)
{
    FILE *fp = fopen(fname, "r");

    // validate that we can open the file
    if (!fp)
    {
        fprintf(stderr, "%s:%s:%d wunzip unable to open file %s\n", __FILE__, __FUNCTION__, __LINE__, fname);
        return 1;
    }

    // for keeping track of the current character and run length
    int run_len = 0;
    int cur_char;

    while ((cur_char = fgetc(fp)) != EOF)
    {
        int bit = sizeof(int) * CHAR_BIT - 1;
        while (bit > -1 && cur_char != EOF)
        {
            // ensure that cur char is either 0 or 1 otherwise we have invalid encoding
            if (cur_char % '0' != 0 && cur_char % '0' != 1)
            {
                fprintf(
                        stderr,
                        "%s:%s:%d file %s was not encoded properly, unable to encode run length\n",
                        __FILE__,
                        __FUNCTION__,
                        __LINE__,
                        fname);
                return 1;
            }


            run_len ^= (cur_char % '0') << bit--;
            cur_char = fgetc(fp);
        }

        // ensure that we have read all the bits for the run length and that cur_char is a valid ascii symbol
        if (bit > -1 || cur_char == EOF)
        {
            fprintf(
                    stderr,
                    "%s:%s:%d file %s was not encoded properly\n",
                    __FILE__,
                    __FUNCTION__,
                    __LINE__,
                    fname);
            return 1;
        }

        for (int i = 0; i < run_len; i++)
        {
            printf("%c", (char) cur_char);
        }

        run_len = 0;
    }

    if (ferror(fp) || !feof(fp))
    {
        fprintf(stderr, "%s:%s:%d unable to decode file successfully\n", __FILE__, __FUNCTION__, __LINE__);
        return 1;
    }

    fclose(fp);
    return 0;
}
