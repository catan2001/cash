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

#define TRUE 1
#define FALSE 0
#define ERROR -1

// Maximum size of the input line
#define MAX_SIZE 1000

enum TokenType // TODO: add the rest...
{
    // One-word token
    LEFT_PARENTHESIS,  // done
    RIGHT_PARENTHESIS, // done
    LEFT_BRACE,        // done
    RIGHT_BRACE,       // done
    DOT,               // done
    COMMA,             // done
    SEMICOLON,         // done
    COMMENT,           // done but unnecessary

    // Arithmetic operators:
    ADD,      // done
    SUBTRACT, // done
    MULTIPLY, // done
    DIVIDE,   // done
    MODULUS,  // done

    // Relational and assignment operators:
    EXCLAMATION,                          // done
    EXCLAMATION_EQUEAL,                   // done
    EQUAL,                                // done
    DOUBLE_EQUAL,                         // done
    REDIRECTION_RIGHT_GREATER_RELATIONAL, // done
    GREATER_EQUAL,                        // done
    REDIRECTION_LEFT_LESS_RELATIONAL,     // done
    LESS_EQUAL,                           // done

    // Bitwise operators:
    AND,             // done
    PIPE_OR_BITWISE, // done
    XOR,             // done
                     // NOT is EXCLAMATION; see previous
    SHIFT_LEFT,      // done
    SHIFT_RIGHT,     // done

    // Literals:
    IDENTIFIER,
    STRING,
    NUMBER,

    // Commands:
    EXEC,  // done
    PWD,   // done
    CLEAR, // done
    TIME,

    // Keywords:
    IF,          // done
    ELSE,        // done
    FALSE_TOKEN, // done
    TRUE_TOKEN,  // done
    FOR,         // done
    WHILE,       // done
    NULL_TOKEN,  // done
    ENUM_TOKEN,  // done
    VAR,         // done
    PRINTF,      // done
    FUNCT,       // done
    EOF_TOKEN    // done
};

typedef enum CharacterType {SPECIAL, SPACE, NEW_LINE, OTHER} character_t;

/*@Type Union: Abstract the type for interpreter*/
typedef union value_u
{
    size_t integer_value; // 32
    float float_value;    // 32
    double double_value;  // 64
    char *char_value;     // 64
} Value;

/*@Type Structure: Used for tokenization*/
typedef struct token_s
{
    enum TokenType type;
    char *lexeme;
    Value literal;
    // int line_number; // Add this after implementing error correction
} Token;

static char pcmd[MAX_SIZE];

/*@sigint_handler
**Function: Signal handler function for SIGINT*/
static void sigint_handler(int sig)
{
    // Perform cleanup
    fclose(stdout);
    fclose(stdin);
    fprintf(stderr, "\nCaught SIGINT, cleaning up and exiting...\n");
    exit(EXIT_SUCCESS);
}

/*@read_cmd
**Function: Prints prompt and reads input from stdin*/
static void read_cmd(char *pcmd, int lcmd)
{
    if (isatty(fileno(stdin)))
        fprintf(stdout, "cash> ");
    else
        return;
    memset(pcmd, 0, lcmd);
    fgets(pcmd, lcmd, stdin);
    return;
}

/*@print_term
**Function: Prints output to prompt*/
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

// TODO: implement ErrorReporter
/*@error
**Function: Prints error to prompt*/
static int error(char *msg)
{
    if (isatty(fileno(stdin)))
    {
        fprintf(stderr, msg);
        return EXIT_SUCCESS;
    }

    fprintf(stderr, "Could not output to terminal, file descriptor %d", fileno(stdin));
    return EXIT_FAILURE;
}

/*@clear_terminal
**Function: Clears the terminal*/
static void clear_terminal(void)
{
    system("clear");
    return;
}

/*@compare_token
**Function: Compares token with value.*/
static int compare_token(char *token, char *value)
{
    if (strlen(token) != strlen(value))
        return EXIT_FAILURE;
    for (size_t i = 0; token[i] != 0; ++i)
    {
        if (token[i] != value[i])
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/*Lexical analysis section of code*/

/*@add_token
**Helper Function: Adds a token to the array*/
static char **add_token(char **tokens, size_t *token_cnt, char *token)
{
    char **new_tokens = realloc(tokens, (*token_cnt + 1) * sizeof(char *));
    // Deallocate memory if realloc fails
    if (!new_tokens)
    {
        fprintf(stderr, "ERROR: tokenizer [realloc failed]\n");
        for (size_t j = 0; j < *token_cnt; ++j)
            free(tokens[j]);
        free(tokens);
        exit(EXIT_FAILURE);
    }
    tokens = new_tokens;

    // Copy token into an array of tokens
    tokens[*token_cnt] = strdup(token);
    if (!tokens[*token_cnt])
    {
        fprintf(stderr, "ERROR: tokenizer [strdup failed]\n");
        exit(EXIT_FAILURE);
    }
    // Increment token count
    (*token_cnt)++;
    return tokens;
}

/*@type_of_character
**Helper Function: Determines type of character*/
static character_t type_of_character(char c)
{
    switch (c)
    {
            case '#': 
            case '(':
            case ')':
            case '{':
            case '}':
            case '*':
            case '+':
            case '-':
            case '%':
            case '/':
            case '!': 
            case '=':
            case '<':   
            case '>':   return SPECIAL;
            case ' ':
            case '\t':
            case '\r':  return SPACE;
            case '\n':  return NEW_LINE;
            default:    return OTHER;
    }
}

/*@tokenizer
**Function: Separates line into individual null terminated tokens.*/
static char **tokenizer(char *cmd, size_t *token_cnt)
{
    if (cmd == NULL || !cmd[0] || cmd[0] == '\n')
        return NULL;

    size_t head_position = 0; 
    char **tokens = NULL; 

    // Reinitialize token count back to zero
    *token_cnt = 0;

    for (size_t i = 0; cmd[i]; ++i)
    {
        switch (type_of_character(cmd[i]))
        {
            case SPECIAL:
                char special_token[3] = {cmd[i], '\0', '\0'};

                /*Copy previous token into the array*/
                if (head_position < i)
                {
                    cmd[i] = '\0'; // null terminate token
                    tokens = add_token(tokens, token_cnt, &cmd[head_position]);
                    // Position head onto new token
                    head_position = i + 1;
                }
        
                if(special_token[0] == '#') 
                    return tokens; // TODO: Add comment block..

                /*Copy special token into the array*/
                if (cmd[i + 1] == '=' || cmd[i + 1] == '>' || cmd[i + 1] == '<')
                    special_token[1] = cmd[i + 1];
                
                tokens = add_token(tokens, token_cnt, special_token);
                break;
            case SPACE:
            case NEW_LINE:
                // Copy token into the array
                if(head_position < i)
                {
                    cmd[i] = '\0'; // null terminate token
                    tokens = add_token(tokens, token_cnt, &cmd[head_position]);
                }
                break;
            case OTHER:
            default:    
                continue;
        }
        // Position head onto new token
        head_position = i + 1;
    }
    return tokens;
} 

static Token *token_classifier(char **token, size_t number_of_tokens, size_t *number_of_ctokens) // part of parser
{
    size_t number_of_ctox = 0;

    if (!token)
        return NULL;

    Token *ctoken = (Token *)malloc(sizeof(Token));
    if (ctoken == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for ctoken!");
        free(ctoken);
    }

    for (size_t i = 0; i < number_of_tokens; ++i)
    {
        number_of_ctox++;
        ctoken = (Token *)realloc(ctoken, sizeof(Token) * number_of_ctox);
        // Free all allocated memory before exiting
        if (!ctoken)
        {
            fprintf(stderr, "ERROR: tokenizer [realloc failed]\n");
            for (size_t j = 0; j < number_of_ctox - 1; ++j)
                free(ctoken[j].lexeme);
            free(ctoken);
            exit(EXIT_FAILURE);
        }

        ctoken[i].lexeme = strdup(token[i]);

        if (!ctoken[i].lexeme)
        {
            perror("Failed to duplicate string");
            // Free all allocated memory before exiting
            for (size_t j = 0; j < (i + 1); j++)
            {
                free(ctoken[j].lexeme);
            }
            free(ctoken);
            return NULL;
        }
        ctoken[i].literal.char_value = ctoken[i].lexeme;
        if (strlen(token[i]) == 1)
        {
            switch (token[i][0])
            {
            case '(':
                ctoken[i].type = LEFT_PARENTHESIS;
                break;
            case ')':
                ctoken[i].type = RIGHT_PARENTHESIS;
                break;
            case '{':
                ctoken[i].type = LEFT_BRACE;
                break;
            case '}':
                ctoken[i].type = RIGHT_BRACE;
                break;
            case '.':
                ctoken[i].type = DOT;
                break;
            case ';':
                ctoken[i].type = SEMICOLON;
                break;
            case '#':
                ctoken[i].type = COMMENT;
                break;
            case '|':
                ctoken[i].type = PIPE_OR_BITWISE;
                break;
            case '&':
                ctoken[i].type = AND;
                break;
            case '~':
                ctoken[i].type = XOR;
                break;
            case '<':
                ctoken[i].type = REDIRECTION_LEFT_LESS_RELATIONAL;
                break;
            case '>':
                ctoken[i].type = REDIRECTION_RIGHT_GREATER_RELATIONAL;
                break;
            case '+':
                ctoken[i].type = ADD;
                break;
            case '-':
                ctoken[i].type = SUBTRACT;
                break;
            case '*':
                ctoken[i].type = MULTIPLY;
                break;
            case '/':
                ctoken[i].type = DIVIDE;
                break;
            case '%':
                ctoken[i].type = MODULUS;
                break;
            case '!':
                ctoken[i].type = EXCLAMATION;
                break;
            case '=':
                ctoken[i].type = EQUAL;
                break;
            default:
                ctoken[i].type = IDENTIFIER;
                break;
            }
        }
        else if (!compare_token(token[i], "!="))
            ctoken[i].type = EXCLAMATION_EQUEAL;
        else if (!compare_token(token[i], "=="))
            ctoken[i].type = DOUBLE_EQUAL;
        else if (!compare_token(token[i], ">="))
            ctoken[i].type = GREATER_EQUAL;
        else if (!compare_token(token[i], "<="))
            ctoken[i].type = LESS_EQUAL;
        else if (!compare_token(token[i], "<<"))
            ctoken[i].type = SHIFT_LEFT;
        else if (!compare_token(token[i], ">>"))
            ctoken[i].type = SHIFT_RIGHT;
        else if (!compare_token(token[i], "pwd"))
            ctoken[i].type = PWD;
        else if (!compare_token(token[i], "exec"))
            ctoken[i].type = EXEC;
        else if (!compare_token(token[i], "clear") || !compare_token(token[i], "clear_terminal"))
            ctoken[i].type = CLEAR;
        else if (!compare_token(token[i], "if"))
            ctoken[i].type = IF;
        else if (!compare_token(token[i], "else"))
            ctoken[i].type = ELSE;
        else if (!compare_token(token[i], "false"))
            ctoken[i].type = FALSE;
        else if (!compare_token(token[i], "true"))
            ctoken[i].type = TRUE;
        else if (!compare_token(token[i], "for"))
            ctoken[i].type = FOR;
        else if (!compare_token(token[i], "while"))
            ctoken[i].type = WHILE;
        else if (!compare_token(token[i], "null"))
            ctoken[i].type = NULL_TOKEN;
        else if (!compare_token(token[i], "enum"))
            ctoken[i].type = ENUM_TOKEN;
        else if (!compare_token(token[i], "var"))
            ctoken[i].type = VAR;
        else if (!compare_token(token[i], "printf"))
            ctoken[i].type = PRINTF;
        else if (!compare_token(token[i], "funct"))
            ctoken[i].type = FUNCT;
        else if (!compare_token(token[i], "eof"))
            ctoken[i].type = EOF_TOKEN;
        else
            ctoken[i].type = IDENTIFIER;

        printf("TOKEN: ");
        print_term(ctoken[i].lexeme);
        print_term("\n");
    }
    *number_of_ctokens = number_of_ctox;
    printf("Finished\n");
    return ctoken;
}

// TODO: ./execution does not work
static void exec(char *cmd)
{
    int len;
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

    // Clear the terminal at start
    clear_terminal();

    while (TRUE)
    {
        char **tokens;
        size_t number_of_tokens;
        size_t number_of_ctokens;

        read_cmd(pcmd, MAX_SIZE);
        tokens = tokenizer(pcmd, &number_of_tokens);
        if (tokens != NULL)
        {
            Token *ctox = token_classifier(tokens, number_of_tokens, &number_of_ctokens);
            // exec(pcmd);
            // Deallocate Heap memory
            for (size_t i = 0; i < number_of_tokens; ++i) {
                free(tokens[i]);
            }
            free(tokens);
            for (size_t i = 0; i < number_of_ctokens; ++i)
                free(ctox[i].lexeme);
            free(ctox);
        }

        wait(NULL);
    }
    return 0;
}
