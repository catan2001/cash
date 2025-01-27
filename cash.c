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
    FAILED_TO_CLASSIFY = 1,
    
    // Single character special token
    EXCLAMATION = 33,                          // done 33 0
    COMMENT = 35,                              // done 35 2
    MODULUS = 37,                              // done 37 4
    AND = 38,                                  // done 38 5
    LEFT_PARENTHESIS = 40,                     // done 40 7
    RIGHT_PARENTHESIS = 41,                    // done 41 8
    MULTIPLY = 42,                             // done 42 9
    ADD = 43,                                  // done 43 10
    COMMA = 44,                                // done 44 11
    SUBTRACT = 45,                             // done 45 12
    DOT = 46,                                  // done 46 13
    DIVIDE = 47,                               // done 47 14
    SEMICOLON = 59,                            // done 59 26
    REDIRECTION_LEFT_LESS_RELATIONAL = 60,     // done 60 27
    EQUAL = 61,                                // done 61 28
    REDIRECTION_RIGHT_GREATER_RELATIONAL = 62, // done 62 29
    LEFT_BRACE = 123,                          // done 123 24
    PIPE_OR_BITWISE = 124,                     // done 124 25
    RIGHT_BRACE = 125,                         // done 125 26
    XOR = 126,                                 // done 126 27

    // Two character special token
    EXCLAMATION_EQUEAL, // done
    DOUBLE_EQUAL,       // done
    GREATER_EQUAL,      // done
    LESS_EQUAL,         // done
    SHIFT_LEFT,         // done
    SHIFT_RIGHT,        // done

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
static void sigint_handler(const int sig)
{
    // Perform cleanup
    fclose(stdout);
    fclose(stdin);
    fprintf(stderr, "\nCaught SIGINT, cleaning up and exiting...\n");
    exit(EXIT_SUCCESS);
}

/*@read_cmd
**Function: Prints prompt and reads input from stdin*/
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
            case '~':
            case '|':
            case ';':
            case '&':
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

static int classify_special_token(const char *token)
{
    if (type_of_character(token[1]) == SPECIAL)
    {
        switch (token[0])
        {
        case '<':
            return (token[1] == '=') ? LESS_EQUAL : REDIRECTION_LEFT_LESS_RELATIONAL;
        case '>':
            return (token[1] == '=') ? GREATER_EQUAL : REDIRECTION_RIGHT_GREATER_RELATIONAL;
        case '!':
            return EXCLAMATION_EQUEAL;
        case '=':
            return DOUBLE_EQUAL;
        default:
            fprintf(stderr, "error: syntax mistake at %s\n", token);
            return FAILED_TO_CLASSIFY;
        }
    }
    return (int)token[0];
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

            if (special_token[0] == '#')
                return tokens; // TODO: Add comment block..

            /*Copy special token into the array*/
            if (cmd[i + 1] == '=' || cmd[i + 1] == '>' || cmd[i + 1] == '<')
                special_token[1] = cmd[++i];

            tokens = add_token(tokens, token_cnt, special_token);
            break;

        case SPACE:
        case NEW_LINE:
            // Copy token into the array
            if (head_position < i)
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

/*@token_classifier
**Function: Classifies tokens.*/
static Token *token_classifier(char **token, const size_t number_of_tokens, size_t *number_of_ctokens) // part of parser
{

    if (!token)
        return NULL;

    // Allocate number of tokens as ctoken
    Token *ctoken = (Token *)malloc(sizeof(Token) * number_of_tokens);
    if (ctoken == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for ctoken!");
        free(ctoken);
    }

    for (size_t i = 0; i < number_of_tokens; ++i)
    {
        // Check for special characters
        if (type_of_character(token[i][0]) == SPECIAL)
            ctoken[i].type = classify_special_token(token[i]);
        else if (!compare_token(token[i], "pwd"))
            ctoken[i].type = PWD;
        else if (!compare_token(token[i], "exec"))
            ctoken[i].type = EXEC;
        else if (!compare_token(token[i], "clear"))
            ctoken[i].type = CLEAR;
        else if (!compare_token(token[i], "if"))
            ctoken[i].type = IF;
        else if (!compare_token(token[i], "else"))
            ctoken[i].type = ELSE;
        else if (!compare_token(token[i], "false"))
            ctoken[i].type = FALSE_TOKEN;
        else if (!compare_token(token[i], "true"))
            ctoken[i].type = TRUE_TOKEN;
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

        ctoken[i].lexeme = strdup(token[i]);
        if (!ctoken[i].lexeme || ctoken[i].type == EXIT_FAILURE)
        {
            fprintf(stderr, "error: Failed to classify token %s\n", token[i]);
            // Free all allocated memory before exiting
            for (size_t j = 0; j < (i + 1); j++)
            {
                free(ctoken[j].lexeme);
            }
            free(ctoken);
            return NULL;
        }

        ctoken[i].literal.char_value = ctoken[i].lexeme;
    }
    *number_of_ctokens = number_of_tokens;
    for (int i = 0; i < *number_of_ctokens; ++i)
    {
        printf("TOKEN: ");
        print_term(ctoken[i].lexeme);
        print_term("\n");
    }
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
