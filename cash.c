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
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <assert.h>
#include <signal.h>
#include <setjmp.h>
#include "coretypes.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

static char pcmd[MAX_LINE_SIZE];
static jmp_buf sync_env;

static void clear_terminal(void)
{
    printf("\033[H\033[J");
    fflush(stdout);
    return;
}

static int print_term(char *msg)
{
    if (isatty(fileno(stdin)))
    {
        fprintf(stdout, msg);
        return EXIT_SUCCESS;
    }   

    fprintf(stderr, "Could not output to terminal, file descriptor %d", fileno(stdin));
    return EXIT_FAILURE;
}

static void read_cmd(char *pcmd, const int lcmd)
{
    if (isatty(fileno(stdin)))
        fprintf(stdout, "cash> ");
    else
        return;
    memset(pcmd, 0, lcmd);
    fgets(pcmd, lcmd, stdin);
    return;
}

// TODO: ./execution does not work
static void exec(char *cmd)
{
    size_t len;
    if (cmd[0] == '.' && cmd[1] == '.')
    {
        len = strlen(cmd + 2); // for some reason it's not NULL terminated...
        cmd[len + 1] = 0;
        if (execl(cmd + 2, cmd + 2, NULL) == -1)
        {
            fprintf(stderr, "Could not execute file\n");
        }
    }
}

int main(void)
{
    // Set up the signal handler for SIGINT to close
    // stdin and stdout streams
    signal(SIGINT, sigint_handler);
    /*Clear the terminal at start*/
    clear_terminal();

    while (TRUE)
    {
        char **tokens;
        size_t number_of_tokens = 0;
        size_t number_of_ctokens = 0;
        size_t number_of_statements = 0;
        AST **ast = NULL;
        reset_error_flag();
        
        read_cmd(pcmd, MAX_LINE_SIZE);
        tokens = tokenizer(pcmd, &number_of_tokens);
        if (tokens == NULL || error_flag) goto DEALLOCATE_TOKENS_LABEL;

        Token *ctox = token_classifier(tokens, number_of_tokens, &number_of_ctokens);
        if (tokens == NULL || error_flag) goto DEALLOCATE_CTOKENS_LABEL;

        ast = parser(ctox, &number_of_statements);
        if(ast == NULL || error_flag) goto DEALLOCATE_AST_LABEL;

        for(size_t i = 0; i < number_of_statements; ++i) 
            if(ast[i] != NULL) 
            {
                ast_print(ast[i]);
                //interpret(ast[i]);
            }
        /* Deallocate Heap memory */ 
        DEALLOCATE_AST_LABEL:
        for(size_t i = 0; i < number_of_statements; ++i)
            if(ast[i] != NULL) 
                ast_free(ast[i]);      
        free(ast); 
        
        DEALLOCATE_CTOKENS_LABEL:
        for (size_t i = 0; ctox[i].type != EOF_TOKEN; ++i) {
            free(ctox[i].lexeme);
        }
        free(ctox);

        DEALLOCATE_TOKENS_LABEL:
        for (size_t i = 0; i < number_of_tokens; ++i) {
            free(tokens[i]);
        }
        free(tokens);
        wait(NULL);
    } 
    return 0;
}
