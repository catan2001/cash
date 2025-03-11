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
#include "environment.h"
#include "interpreter.h"
#include "cash.h"

char pcmd[MAX_LINE_SIZE];

static char * file_read_line(FILE *file) 
{
    if(file == NULL) {perror("Error opening a file"); exit(EXIT_FAILURE);}
    if(fgets(pcmd, MAX_LINE_SIZE, file) == NULL) return NULL;
    return strdup(pcmd);
}

extern void clear_terminal(void) 
{
    printf("\033[H\033[J");
    fflush(stdout);
    return;
}

extern int print_term(char *msg) 
{
    if (isatty(fileno(stdin))) {
        fprintf(stdout, msg);
        return EXIT_SUCCESS;
    }   

    fprintf(stderr, "Could not output to terminal, file descriptor %d", fileno(stdin));
    return EXIT_FAILURE;
}

extern void read_cmd(char *pcmd, const int lcmd) 
{
    if (isatty(fileno(stdin)))
        fprintf(stdout, "cash> ");
    else
        return;
    memset(pcmd, 0, lcmd);
    fgets(pcmd, lcmd, stdin);
    return;
}

extern void run_file(char *file_name) 
{
    char cwd[FILE_PATH_SIZE];
    getcwd(cwd, FILE_PATH_SIZE);
    strcat(cwd, "/");
    strcat(cwd, file_name);

    FILE *file = fopen(file_name, "r");
    if(file == NULL) {perror("Error opening a file"); exit(EXIT_FAILURE);}

    char **tokens = NULL;
    Token *ctokens = NULL;
    AST **ast = NULL;
    size_t line_number = 1;
    size_t number_of_tokens = 0;
    size_t number_of_ctokens = 0;
    size_t number_of_statements = 0;

    for(char *line = file_read_line(file); line != NULL; line = (line_number++, file_read_line(file))) {
        if((tokens = tokenizer(pcmd, &number_of_tokens)) == NULL) continue;
        if(error_flag) goto DEALLOCATE_TOKENS_LABEL;
        if((ctokens = token_classifier(tokens, ctokens, number_of_ctokens + number_of_tokens, &number_of_ctokens)) == NULL) TODO("Add error");
        if(error_flag) goto DEALLOCATE_CTOKENS_LABEL;
        free(line);
    }

    /* Add EOF token at the end */
    number_of_ctokens = eof_token(&ctokens, number_of_ctokens);
    ast = parser(ctokens, &number_of_statements);
    if(ast == NULL || error_flag) goto DEALLOCATE_AST_LABEL;

    for(size_t i = 0; i < number_of_statements; ++i) {
        if(ast[i] != NULL) {
            ast_print(ast[i]);
            interpret(ast[i]);
        }
        if(error_flag) break;
    }
    //Deallocate Heap memory 
    env_reset(&env_global);

    DEALLOCATE_AST_LABEL:
    for(size_t i = 0; i < number_of_statements; ++i) {
        if(ast[i] != NULL) 
            ast_free(ast[i]);      
    }
    free(ast);

    DEALLOCATE_CTOKENS_LABEL:
    for (size_t i = 0; ctokens[i].type != EOF_TOKEN; ++i) {
        free(ctokens[i].lexeme);
    }
    free(ctokens);

    DEALLOCATE_TOKENS_LABEL:
    for (size_t i = 0; i < number_of_tokens; ++i) {
        free(tokens[i]);
    }
    free(tokens);

    exit(EXIT_SUCCESS); 
}

extern void run_term(void) 
{
    while (TRUE) {
        char **tokens;
        size_t number_of_tokens = 0;
        size_t number_of_ctokens = 0;
        size_t number_of_statements = 0;
        AST **ast = NULL;
        Token *ctokens = NULL;
        reset_error_flag();
        
        read_cmd(pcmd, MAX_LINE_SIZE);
        tokens = tokenizer(pcmd, &number_of_tokens);
        if (tokens == NULL || error_flag) goto DEALLOCATE_TOKENS_LABEL;

        ctokens = token_classifier(tokens, ctokens, number_of_tokens, &number_of_ctokens);
        if (tokens == NULL || error_flag) goto DEALLOCATE_CTOKENS_LABEL;
        /* Add EOF token at the end */
        number_of_ctokens = eof_token(&ctokens, number_of_ctokens);

        ast = parser(ctokens, &number_of_statements);
        if(ast == NULL || error_flag) goto DEALLOCATE_AST_LABEL;

        for(size_t i = 0; i < number_of_statements; ++i) 
            if(ast[i] != NULL) {
                ast_print(ast[i]);
                interpret(ast[i]);
            }

        //Deallocate Heap memory 
        env_reset(&env_global);

        DEALLOCATE_AST_LABEL:
        for(size_t i = 0; i < number_of_statements; ++i)
            if(ast[i] != NULL) 
                ast_free(ast[i]);      
        free(ast);

        DEALLOCATE_CTOKENS_LABEL:
        for (size_t i = 0; i < number_of_ctokens; ++i) {
            free(ctokens[i].lexeme);
        }
        free(ctokens);

        DEALLOCATE_TOKENS_LABEL:
        for (size_t i = 0; i < number_of_tokens; ++i) {
            free(tokens[i]);
        }
        free(tokens);
        wait(NULL);
    } 
}

extern void cash(int argc,char **argv) 
{
    reset_error_flag();
    if(argc > 2) {fprintf(stderr, "Can't interpret multiple files at once!"); exit(EXIT_FAILURE);}
    if(argc > 1) {
        /* Run cash from file */
        run_file(argv[1]);
    }
    /* run cash from terminal */
    run_term();
}
