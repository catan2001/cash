/*
MIT License

Copyright (c) 2024 catan2001

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include "coretypes.h"
#include "error.h"

int error_flag = FALSE;

extern void set_error_flag(void)
{
    error_flag = TRUE;
}

extern void reset_error_flag(void)
{
    error_flag = FALSE;
}

extern void sigint_handler(const int sig)
{
    fclose(stdout);
    fclose(stdin);
    fprintf(stderr, "\nExiting shell...\n");    
    exit(EXIT_SUCCESS);
}

extern void parser_error(Token token, char *msg)
{
    set_error_flag();
    if(token.type == EOF_TOKEN)
        fprintf(stderr, "Error line: %d at end %s\n", token.line_number, msg);
    else
        fprintf(stderr, "Error line: %d at '%s', %s\n", token.line_number, token.lexeme, msg);
}

extern void runtime_error(AST *node, char *msg) 
{
    fprintf(stderr, "Runtime error line: %d at '%s', %s\n", node->data.token->line_number, node->data.token->lexeme, msg);
}

extern void environment_error(Token *token, char *msg)
{
     fprintf(stderr, "Runtime error line: %d at '%s', %s\n", token->line_number, msg);
}

extern int error(char *msg, char *file, int line) 
{
    fprintf(stderr,"%s: %d ERROR: %s\n", file, line, msg);
    return EXIT_SUCCESS;
}

