#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <assert.h>

#define TRUE 1
#define FALSE 0
#define ERROR -1

#define MAX_SIZE 100

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

/*Type Union: Abstract the type for interpreter*/
typedef union value_u
{
    int integer_value;   // 32
    float float_value;   // 32
    double double_value; // 64
    char *char_value;    // 64
} Value;

/*Type Structure: Used for tokenization*/
typedef struct token_s
{
    enum TokenType type;
    char *lexeme;
    Value literal;
    // int line_number; // Add this after implementing error correction
} Token;

char pcmd[MAX_SIZE];

/*@read_cmd
**Function: Prints prompt and reads input from stdin*/
void read_cmd(char *pcmd, int lcmd)
{
    if (isatty(fileno(stdin)))
        fprintf(stdout, "cash> ");
    else
        return;
    memset(pcmd, 0, lcmd); // necessary?
    fgets(pcmd, lcmd, stdin);
}

/*@print_term
**Function: Prints output to prompt*/
int print_term(char *msg)
{
    if (isatty(fileno(stdin)))
    {
        fprintf(stdout, msg);
        return EXIT_SUCCESS;
    }

    fprintf(stderr, "Could not output to terminal, file descriptor %d", fileno(stdin));
    return EXIT_FAILURE;
}

int error(char *msg)
{
    if (isatty(fileno(stdin)))
    {
        fprintf(stderr, msg);
        return EXIT_SUCCESS;
    }

    fprintf(stderr, "Could not output to terminal, file descriptor %d", fileno(stdin));
    return EXIT_FAILURE;
}

// TODO: implement ErrorReporter
void clrs(void)
{
    system("clear");
}

int compare_token(char *token, char *value)
{
    if (strlen(token) != strlen(value))
        return EXIT_FAILURE;
    for (int i = 0; token[i] != 0; ++i)
    {
        if (token[i] != value[i])
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// TODO: 19.1.2025. Implement "" and #
char **tokenizer(char *cmd) // lexical analysis
{
    if (cmd[0] == 0 || cmd[0] == '\n')
        return NULL;

    int word_cnt = 0;
    int capacity = 2;
    char **tokens = (char **)malloc(capacity * sizeof(char *));
    if (tokens == NULL)
    {
        fprintf(stderr, "ERROR: tokenizer [could not allocate memory]\n");
        exit(EXIT_FAILURE);
    }

    char *token = cmd;
    for (int i = 0; cmd[i] != '\0'; ++i)
    {
        //if(cmd[i] == '#') break; // Implement when Comment...
        if (cmd[i] != ' ' && cmd[i] != '\t' && cmd[i] != '\r' && cmd[i] != '\n') 
            continue;
        cmd[i] = '\0'; // replace white space with null terminator

        if (word_cnt >= capacity) // reallocate larger memory...
        {
            capacity *= 2;
            char **new_tokens = realloc(tokens, capacity * sizeof(char *));
            if (!new_tokens) // deallocate if fail
            {
                fprintf(stderr, "ERROR: tokenizer [realloc failed]\n");
                for (int j = 0; j < word_cnt; ++j)
                    free(tokens[j]);
                free(tokens);
                exit(EXIT_FAILURE);
            }
            tokens = new_tokens;
        }
        tokens[word_cnt++] = strdup(token); // Copy token into the array

        while (cmd[++i] == ' ')
            ;            // skip spaces
        token = &cmd[i]; // fetch next token
    }
    return tokens;
}

Token *classify_tokens(char **token) // part of parser
{
    if (!token)
        return NULL;
    Token *ctoken = (Token *)malloc(sizeof(Token));
    for (int i = 0; token[i] != NULL; i++)
    {
        ctoken = (Token *)realloc(ctoken, sizeof(Token) * (i + 1) * 2);
        ctoken[i].lexeme = strdup(token[i]);
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
        else if (!compare_token(token[i], "clear") || !compare_token(token[i], "clrs"))
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
    return ctoken;
}

// TODO: ./execution does not work
void exec(char *cmd)
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
    clrs();

    while (TRUE)
    {
        read_cmd(pcmd, sizeof(pcmd));
        char **tokens = tokenizer(pcmd);
        Token *ctox = classify_tokens(tokens);

        free(tokens); // add for loop to properly free
        exec(pcmd);
        wait(NULL);
    }
    return 0;
}
