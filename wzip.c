//
// Created by benjamin haase on 3/26/24.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int process_file(char* fname);

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
