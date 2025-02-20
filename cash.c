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

#define TRUE 1
#define FALSE 0
#define ERROR -1

/* Maximum size of the input line */
#define MAX_LINE_SIZE 1000

// TsodingDaily <3
#define TODO(msg) do {                                              \
        fprintf(stderr, "%s:%d TODO: %s\n", __FILE__, __LINE__, msg); \
        abort();                                                    \
    } while(0)

#define BINARY_OPERATION(op, left, left_t, right, right_t, result) do {                 \
        if(left_t == NUMBER_INT && right_t == NUMBER_INT) {                             \
            (result).integer_value = (left).integer_value op (right).integer_value;     \
        }                                                                               \
        else if(left_t == NUMBER_INT) {                                                 \
            (result).float_value = (left).integer_value op (right).float_value;         \
        }                                                                               \
        else if(right_t == NUMBER_INT){                                                 \
            (result).float_value = (left).float_value op (right).integer_value;         \
        }                                                                               \
        else {                                                                          \
            (result).float_value = (left).float_value op (right).float_value;           \
        }                                                                               \
    } while(0)  

#define BINARY_COMPARISON(op, left, left_t, right, right_t, result) do {                    \
        if(left_t == NUMBER_INT && right_t == NUMBER_INT) {                             \
            (result).integer_value = (left).integer_value op (right).integer_value;     \
        }                                                                               \
        else if(left_t == NUMBER_INT) {                                                 \
            (result).integer_value = (left).integer_value op (right).float_value;         \
        }                                                                               \
        else if(right_t == NUMBER_INT){                                                 \
            (result).integer_value = (left).float_value op (right).integer_value;         \
        }                                                                               \
        else {                                                                          \
            (result).integer_value = (left).float_value op (right).float_value;           \
        }                                                                               \
    } while(0)  

static jmp_buf sync_env;

/* Enumerating type for defining token type */
typedef enum value_t
{
    /* Used to indicate failure: */
    FAILED_TO_CLASSIFY = 1, 
   
    /* Single character special token: */
    EXCLAMATION                             = '!',
    COMMENT                                 = '#',
    MODULUS                                 = '%',
    AND                                     = '&',
    LEFT_PARENTHESIS                        = '(',
    RIGHT_PARENTHESIS                       = ')',
    MULTIPLY                                = '*',
    ADD                                     = '+',
    COMMA                                   = ',',
    SUBTRACT                                = '-',
    DOT                                     = '.',
    DIVIDE                                  = '/',
    SEMICOLON                               = ';',
    REDIRECTION_LEFT_LESS_RELATIONAL        = '<',
    EQUAL                                   = '=',
    REDIRECTION_RIGHT_GREATER_RELATIONAL    = '>',
    LEFT_BRACE                              = '{',
    PIPE                                    = '|',
    RIGHT_BRACE                             = '}',
    XOR                                     = '~',

    /* Two character special token: */
    EXCLAMATION_EQUEAL, 
    DOUBLE_EQUAL,       
    GREATER_EQUAL,       
    LESS_EQUAL,          
    SHIFT_LEFT,          
    SHIFT_RIGHT,         

    /* Literals: */
    IDENTIFIER,          
    STRING,
    NUMBER_INT,
    NUMBER_FLOAT,

    /* Commands: */
    EXEC,   
    PWD,    
    CLEAR,  
    TIME,

    /* Reserved Words: */
    IF,           
    ELSE,         
    FALSE_TOKEN,  
    TRUE_TOKEN,   
    FOR,          
    WHILE,        
    NULL_TOKEN,   
    ENUM_TOKEN,   
    VAR,          
    PRINTF,       
    FUNCT,
    STRUCT,
    CLASS,        
    EOF_TOKEN     
} TokenType;

/* Enumerating type used for defining character type */
typedef enum character_t
{
    SPECIAL,
    QUOTES,
    SPACE,
    NEW_LINE,
    OTHER,
    UNUSED_CHARACTERS
} CharacterType;

/*@Type Union: Abstract the type for tokenizer, classifier and parser*/
typedef union value_u
{
    long long int integer_value;    // 64
    double float_value;             // 64
    char *char_value;               // 64 
    int boolean_value;              // 8
} Value;

/*@Type Union: Abstract the type for interpreter*/
typedef struct value_tag_s
{
    TokenType type;
    Value literal;
} ValueTagged;

/*@Type Structure: Used for tokenization*/
typedef struct token_s
{
    TokenType type;
    char *lexeme;
    Value literal;
    int line_number; // Add this after implementing error correction
} Token;

/* Structure used for classifying reserved words */
typedef struct reserved_word_map_t
{
    const char *reserved_word;
    TokenType type;
} ReservedWordMapType;

/* Forward reference AST structure */
typedef struct AST AST; 

/* AST structure, previously forward referenced */
struct AST 
{
    enum tag
    {
        AST_LITERAL,
        AST_EXPR,
        AST_ASSIGN_EXPR,
        AST_BINARY,
        AST_UNARY,
        AST_GROUPING
    } tag;
    union 
    {
        Token *token;
        struct AST_EXPR {AST *left; Token *token; AST *right;} AST_EXPR;
        struct AST_ASSIGN_EXPR {AST *left; Token *token; AST *right;} AST_ASSIGN_EXPR;
        struct AST_GROUPING {AST *left; Token *token;} AST_GROUPING;
        struct AST_BINARY {AST *left; Token *token; AST *right;} AST_BINARY;
        struct AST_UNARY {AST *right; Token *token;} AST_UNARY;
    } data;
};  

/* Hash Map used for reserved words */
static const ReservedWordMapType reserved_word_map[] = {
    {"pwd",     PWD},
    {"exec",    EXEC},
    {"clear",   CLEAR},
    {"if",      IF},
    {"else",    ELSE},
    {"false",   FALSE_TOKEN},
    {"true",    TRUE_TOKEN},
    {"for",     FOR},
    {"while",   WHILE},
    {"null",    NULL_TOKEN},
    {"enum",    ENUM_TOKEN},
    {"var",     VAR},
    {"printf",  PRINTF},
    {"funct",   FUNCT},
    {"class",   CLASS},
    {"struct",  STRUCT},
    {"eof",     EOF_TOKEN}
};

static char pcmd[MAX_LINE_SIZE];

/*@sigint_handler
**Function: Signal handler function for SIGINT*/
static void sigint_handler(const int sig)
{
    // Perform cleanup
    fclose(stdout);
    fclose(stdin);
    fprintf(stderr, "\nExiting shell...\n");    
    exit(EXIT_SUCCESS);
}

// TODO: implement ErrorReporter
/*@error
**Function: Prints error to prompt*/
static int error(char *msg) 
{
    if (isatty(fileno(stdin)))
    {
        fprintf(stderr,"%s: %d ERROR: %s\n", __FILE__, __LINE__, msg);
        return EXIT_SUCCESS;
    }
    fprintf(stderr, "Could not output to terminal, file descriptor %d", fileno(stdin));
    return EXIT_FAILURE;
}

/* Terminal utilities */

/*@clear_terminal
**Function: Clears the terminal*/
static void clear_terminal(void)
{
    printf("\033[H\033[J");
    fflush(stdout);
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


/*@compare_token
**Function: Compares token with value.*/
static int compare_token(const char *token, const char *value)
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

/*@add_token
**Helper Function: Adds a token to the array*/
static char **add_token(char **tokens, size_t *token_cnt, char *token)
{
    char **new_tokens = realloc(tokens, (*token_cnt + 1) * sizeof(char *));
    // Deallocate memory if realloc fails
    if (!new_tokens)
    {
        error("Tokenizer reallocation failed!");
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
        error("Tokenizer strdup failed!");
        exit(EXIT_FAILURE);
    }
    // Increment token count
    (*token_cnt)++;
    return tokens;
}

/*@is_digit
**Helper Function: Determines if character is digit*/
static int is_digit(const char c)
{
    return c >= '0' && c <= '9';
}

/*@is_letter
**Helper Function: Determines if character is letter*/
static int is_letter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/*@type_of_character
**Helper Function: Determines type of character*/
static CharacterType type_of_character(char c)
{
    // Determine if letter, number (float or int) or underscore
    if(is_digit(c) || is_letter(c) || c == '_' || c =='.')
        return OTHER;

    // Determine if Special characters
    switch (c)
    {
            case '~':  case '|': case ';': case '&': case '#': 
            case '\'': case '(': case ')': case '{': case '}':
            case '*':  case '+': case '-': case '%': case '/':
            case '!':  case '=': case '<': case '>': case '?':
            case ',':
                return SPECIAL;
            case '"':  
                return QUOTES;
            case ' ':  case '\t': case '\r':  
                return SPACE;
            case '\n':  
                return NEW_LINE;
            default:    
                return UNUSED_CHARACTERS;
    }
}

/*@classify_special_token
**Helper Function: Classifies special character token*/
static TokenType classify_special_token(const char *token, Token *ctoken)
{   
    ctoken->lexeme = strdup(token);
    ctoken->literal.char_value = ctoken->lexeme;
    if(!ctoken->lexeme)
        return FAILED_TO_CLASSIFY;

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
    return (TokenType)token[0];
}

/*@classify_string
**Helper Function: Classifies strings*/
static TokenType classify_string(char *token, Token *ctoken) 
{
    char tmp_char = token[strlen(token)-1];
    
    // Temporary change token and add string that is inside quotes
    token[strlen(token) - 1] = '\0';
    ctoken->literal.char_value = strdup(&token[1]);
    ctoken->lexeme = ctoken->literal.char_value;
    if(!ctoken->literal.char_value)
        return FAILED_TO_CLASSIFY;
    
    token[strlen(token) - 1] = tmp_char;
    return STRING;
}

/*@classify_number
**Helper Function: Classifies numbers*/
static TokenType classify_number(const char *token, Token *ctoken) 
{
    ctoken->lexeme = strdup(token);
    if(!ctoken->lexeme)
        return FAILED_TO_CLASSIFY;

    for(int i = 0; token[i]; ++i) {
        if(token[i] == '.') 
        {
            ctoken->literal.float_value = atof(token);
            return NUMBER_FLOAT;
        }
    }
    ctoken->literal.integer_value = atoi(token);
    return NUMBER_INT;
}

/*@classify_reserved_words
**Helper Function: Classifies reserved words*/
static TokenType classify_reserved_words(const char *token, Token *ctoken)
{
    /* Initialize lexeme and literal value to NULL*/
    ctoken->lexeme = strdup(token);

    for (size_t i = 0; i < sizeof(reserved_word_map) / sizeof(reserved_word_map[0]); i++) {
        if (!compare_token(token, reserved_word_map[i].reserved_word)) {
            if(reserved_word_map[i].type == TRUE_TOKEN)  ctoken->literal.boolean_value = TRUE;
            else
            if(reserved_word_map[i].type == FALSE_TOKEN) ctoken->literal.boolean_value = FALSE;
            else
                ctoken->literal.char_value = NULL;
            return reserved_word_map[i].type;
        }
    }

    if (!ctoken->lexeme)
        return FAILED_TO_CLASSIFY;

    return IDENTIFIER;
}

/*@tokenizer
**Function: Separates line into individual null terminated tokens.*/
static char **tokenizer(char *cmd, size_t *token_cnt)
{
    if (cmd == NULL || !cmd[0] || cmd[0] == '\n')
        return NULL;

    size_t head_position = 0;
    char **tokens = NULL;

    /*Reinitialize token count back to zero*/
    *token_cnt = 0;
    
    for (size_t i = 0; cmd[i]; ++i)
    {
        switch (type_of_character(cmd[i]))
        {
        case UNUSED_CHARACTERS:
            fprintf(stderr, "error: Character is not allowed in Cash!\n\t Error at: %d! \n", i);
            free(cmd);
            exit(EXIT_FAILURE);

        case SPECIAL:
            char special_token[3] = {cmd[i], '\0', '\0'};

            /*Copy previous token into the array*/
            if (head_position < i)
            {
                cmd[i] = '\0'; // null terminate token
                tokens = add_token(tokens, token_cnt, &cmd[head_position]);
                /*Position head onto new token*/
                head_position = i + 1;
            }

            if (special_token[0] == '#')
                return tokens; // TODO: Add comment block..

            /*Copy special token into the array*/
            if (cmd[i + 1] == '=' || cmd[i + 1] == '>' || cmd[i + 1] == '<')
                special_token[1] = cmd[++i];

            tokens = add_token(tokens, token_cnt, special_token);
            break;

        case QUOTES:
            /*Report an error if an identifier is followed by quotes*/
            if (head_position < i)
            {
                fprintf(stderr,"error: Syntax Error at %d! Expected valid separator after identifier.\n", i);
                return tokens;
            }

            while(cmd[++i] != '"') {
                /*Report a warning if quotes are unterminated*/
                if(cmd[i] == '\n' || cmd[i] == '\0') { // In future change to when it reaches EOF
                    fprintf(stderr, "warning: Unterminated string at %d! \n", i);
                    break;
                    //return tokens;
                } 
            }
            /*Report an error if an quotes are followed by an identifier*/
            if(type_of_character(cmd[i+1]) == OTHER) {
                fprintf(stderr, "error: Syntax Error at %d! Unexpected token.\n", i);
                return tokens;   
            }
            /*Temporarily save character and null terminate*/
            char tmp_char = cmd[i+1];
            cmd[i+1] = '\0';
            
            tokens = add_token(tokens, token_cnt, &cmd[head_position]);
            
            /*Change back cmd[i+1] character to temporarily saved one*/
            cmd[i+1] = tmp_char;
            break;
        case SPACE:
        case NEW_LINE:
            /*Copy token into the array*/
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
        /*Position head onto new token*/
        head_position = i + 1;
    }
    return tokens;
}

/*@token_classifier
**Function: Classifies tokens.*/
static Token *token_classifier(char **token, const size_t number_of_tokens, size_t *number_of_ctokens) // part of parser
                                                                                                       //
{
    if (!token)
        return NULL;

    /*Allocate number of tokens as ctoken*/
    Token *ctoken = (Token *)malloc(sizeof(Token) * (number_of_tokens+1));
    if (ctoken == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for ctoken!");
        free(ctoken);
    }

    for (size_t i = 0; i < number_of_tokens; ++i)
    {
        /*Classify special characters*/
        if (type_of_character(token[i][0]) == SPECIAL)
            ctoken[i].type = classify_special_token(token[i], &ctoken[i]);
        /*Classify strings*/ 
        else if (type_of_character(token[i][0]) == QUOTES)
            ctoken[i].type = classify_string(token[i], &ctoken[i]);
        /*Classify numbers*/ 
        else if (is_digit(token[i][0]))
            ctoken[i].type = classify_number(token[i], &ctoken[i]);
        /*Classify reserved words*/
        else 
            ctoken[i].type = classify_reserved_words(token[i], &ctoken[i]);

        /*Handle error when classifying*/ 
        if (ctoken[i].type == FAILED_TO_CLASSIFY)
        {
            fprintf(stderr, "error: Failed to classify token %s\n", token[i]);
            /*Free all allocated memory before exiting*/ 
            for (size_t j = 0; j < (i + 1); j++)
                free(ctoken[j].lexeme);
            free(ctoken);

            *number_of_ctokens = 0;
            return NULL;
        }
    }

    /* Add EOF token at the end */
    *number_of_ctokens = number_of_tokens+1;
    ctoken[number_of_tokens].lexeme = NULL;
    ctoken[number_of_tokens].type = EOF_TOKEN;

    for (int i = 0; i < number_of_tokens+1; ++i)
    {
        printf("TOKEN: ");
        print_term(ctoken[i].lexeme);
        print_term("\n");
    }
    return ctoken;
}

/* Parser part of code */

/* Helper function for returning next iteration of token_list */
static int next_position(size_t *current_position, Token *token_list)
{
    if(token_list[*current_position+1].type == EOF_TOKEN)
        return 1;
        
    (*current_position)++;
    printf("Current position %d\n", (*current_position));
    return 0;
}

/* Function that creates ASTs */
static AST *ast_new(AST ast)
{
    AST *ast_ptr = malloc(sizeof(AST));
    if(ast_ptr) *ast_ptr = ast;
    else
        error("ast_new failed to allocate memory!");
    return ast_ptr;
}

/* Function that prints ASTs */
static void ast_print(AST *ast)
{
    if(!ast) 
    {
        fprintf(stderr, "Error! AST IS NULL!\n");
        return;
    }

    switch(ast->tag)
    {
        case AST_EXPR: 
        {
            fprintf(stdout, "Expression Node: %s\n", ast->data.AST_BINARY.token->lexeme);
            ast_print(ast->data.AST_EXPR.left);
            ast_print(ast->data.AST_EXPR.right);
            break;
        }
        case AST_ASSIGN_EXPR:
        {
            fprintf(stdout, "Assignment Expression Node: %s\n", ast->data.AST_BINARY.token->lexeme);
            ast_print(ast->data.AST_ASSIGN_EXPR.left);
            ast_print(ast->data.AST_ASSIGN_EXPR.right);
            break;               
        }
        case AST_BINARY:
            fprintf(stdout, "Binary Node: %s\n", ast->data.AST_BINARY.token->lexeme);
            ast_print(ast->data.AST_BINARY.left);
            ast_print(ast->data.AST_BINARY.right);
            break;
        case AST_UNARY:
            fprintf(stdout, "Unary Node: %s\n", ast->data.AST_UNARY.token->lexeme);
            ast_print(ast->data.AST_UNARY.right);
            break;
        case AST_LITERAL:
            fprintf(stdout, "Literal Node: %s\n", ast->data.token->lexeme);
            break;
        default:
            fprintf(stderr, "Error! Cannot print AST node: %s\n", ast->data.token->lexeme);
            break;
    }
}

/* Function that deletes AST from current node down */
static void ast_free(AST *ast)
{
    if(!ast) 
    {
        fprintf(stderr, "%s: %d Error: Cannot free AST: %s\n", __FILE__, __LINE__, ast->data.token->lexeme);
        return;
    }
    switch(ast->tag)
    {
        case AST_EXPR: 
        {
            ast_free(ast->data.AST_EXPR.left);
            ast_free(ast->data.AST_EXPR.right);
            break;
        }
        case AST_ASSIGN_EXPR:
        {
            ast_free(ast->data.AST_ASSIGN_EXPR.left);
            ast_free(ast->data.AST_ASSIGN_EXPR.right);
            break;               
        }
        case AST_BINARY: 
        {
            ast_free(ast->data.AST_BINARY.left);
            ast_free(ast->data.AST_BINARY.right);
            break;
        }
        case AST_UNARY: 
        {
            ast_free(ast->data.AST_UNARY.right);
            break;
        }
        case AST_GROUPING:
        {
            ast_free(ast->data.AST_GROUPING.left);
            break;
        }
        default:
            break;
    }
    free(ast);
}

/* Helper function for synchronization when parser enters Panic Mode */
static void synchronize(Token *token_list, size_t *token_position)
{
    while(!next_position(token_position, token_list))
    {
       switch(token_list[*token_position].type)
        {
            case SEMICOLON:
            case FUNCT:
            case CLASS:
            case STRUCT:
            case VAR:
            case FOR:
            case IF:
            case WHILE:
            case PRINTF:
            case EXEC:
            case PWD:
            case TIME:
                return;
        } 
    }
    // set token_position to EOF_TOKEN
    (*token_position)++;
}

/* Function that is called when error happens */
static void panic_mode(Token *token_list, size_t *token_position)
{
    synchronize(token_list, token_position);
    // Jump back to sync_env
    longjmp(sync_env, 1);
}

static void parser_error(Token token, char *msg)
{
    if(token.type == EOF_TOKEN)
        fprintf(stderr, "Error line: %d at end %s\n", token.line_number, msg);
    else
        fprintf(stderr, "Error line: %d at '%s', %s\n", token.line_number, token.lexeme, msg);
}

/* Function declaration of EXPR rule */
static AST *expression(Token *, size_t *, AST *);

/* Function that implements PRIM rule of grammar */
static AST *primary(Token *token_list, size_t *token_position, AST *ast)
{
    switch (token_list[*token_position].type)
    {
        case NUMBER_FLOAT:
        case NUMBER_INT:
        case STRING:
        {
            ast = ast_new((AST){
                .tag = AST_LITERAL,
                .data.token = &token_list[*token_position]});
            next_position(token_position, token_list);
            return ast;
        }
        case TRUE_TOKEN:
        case FALSE_TOKEN:
        case NULL_TOKEN:
        {
             ast = ast_new((AST){
                .tag = AST_LITERAL,
                .data.token = &token_list[*token_position]}); 
            next_position(token_position, token_list);
            return ast;
        }
        case LEFT_PARENTHESIS:
        {
            if (next_position(token_position, token_list)) {
                parser_error(token_list[*token_position], "Unclosed paranthesis");
                longjmp(sync_env, 1);
                break;  
            }
            ast = expression(token_list, token_position, ast);
            if (token_list[*token_position].type == RIGHT_PARENTHESIS)
            {
                next_position(token_position, token_list);
                return ast;
            }
        }
        case ADD: case SUBTRACT: case MULTIPLY: case DIVIDE:
        {
            parser_error(token_list[*token_position], "could not parse such token. Expected right operator.");
            panic_mode(token_list, token_position);
        }
        default:
            break;
    }
    parser_error(token_list[*token_position], "could not parse such token. Expect expression.");
    panic_mode(token_list, token_position);
    return NULL;
}

/* Function that implements UNRY rule of grammar */
static AST *unary(Token *token_list, size_t *token_position, AST *ast)
{
    if(token_list[*token_position].type == SUBTRACT    ||
       token_list[*token_position].type == EXCLAMATION ||
       token_list[*token_position].type == XOR)
    {
        Token *operator = &token_list[*token_position];
        if(next_position(token_position, token_list)) {
            parser_error(*operator, "Missing right operator!\n");
            panic_mode(token_list, token_position);
            return ast; 
        }
        AST *right = unary(token_list, token_position, ast);
        ast = ast_new((AST)
            {
                .tag = AST_UNARY,
                .data.AST_UNARY = {
                    right,
                    operator,
                }
            }
        );
        return ast;
    }
    
    return primary(token_list, token_position, ast);
}

/* Function that implements FACT rule of grammar */
static AST *factor(Token *token_list, size_t *token_position, AST *ast)
{
    ast = unary(token_list, token_position, ast);
    
    while(token_list[*token_position].type == DIVIDE  ||
          token_list[*token_position].type == MULTIPLY)
    {
        Token *operator = &token_list[*token_position];
        if(next_position(token_position, token_list)) {
            parser_error(*operator, "Missing right operator!\n");
            panic_mode(token_list, token_position);
            return ast; 
        }
        AST *right = unary(token_list, token_position, ast);
        ast = ast_new((AST)
            {
                .tag = AST_BINARY,
                .data.AST_BINARY = {
                    ast,
                    operator,
                    right
                }
            }
        );
    }
    return ast;
}

/* Function that implements TERM rule of grammar */
static AST *term(Token *token_list, size_t *token_position, AST *ast)
{
    ast = factor(token_list, token_position, ast);

    while(token_list[*token_position].type == ADD ||
          token_list[*token_position].type == SUBTRACT)
    {   
        Token *operator = &token_list[*token_position];
        if(next_position(token_position, token_list)) {
            parser_error(*operator, "Missing right operator!\n");
            panic_mode(token_list, token_position);
            return ast; 
        }
        AST *right = factor(token_list, token_position, ast);
        ast = ast_new((AST)
            {
                .tag = AST_BINARY,
                .data.AST_BINARY = {
                    ast,
                    operator,
                    right
                }
            }
        );
    }
    return ast;
}

/* Function that implements CMPR rule of grammar */
static AST *comparison(Token *token_list, size_t *token_position, AST *ast)
{
    ast = term(token_list, token_position, ast);

    while(token_list[*token_position].type == REDIRECTION_LEFT_LESS_RELATIONAL ||
          token_list[*token_position].type == LESS_EQUAL ||
          token_list[*token_position].type == REDIRECTION_RIGHT_GREATER_RELATIONAL ||
          token_list[*token_position].type == GREATER_EQUAL)
    {
        Token *operator = &token_list[*token_position];
        if(next_position(token_position, token_list)) {
            parser_error(*operator, "Missing right operator!\n");
            panic_mode(token_list, token_position);
            return ast; 
        }        
        AST *right = term(token_list, token_position, ast);
        ast = ast_new((AST)
            {
                .tag = AST_BINARY,
                .data.AST_BINARY = {
                    ast,
                    operator,
                    right
                }
            }
        );
    }

    return ast;
}

/* Function that implements EQL rule of grammar */
static AST *equality(Token *token_list, size_t *token_position, AST *ast)
{
    ast = comparison(token_list, token_position, ast);
    
    while(token_list[*token_position].type == EXCLAMATION_EQUEAL ||
          token_list[*token_position].type == DOUBLE_EQUAL)
    {
        Token *operator = &token_list[*token_position];
        if(next_position(token_position, token_list)) {
            parser_error(*operator, "Missing right operator!\n");
            panic_mode(token_list, token_position);
            return ast; 
        }         
        AST *right = comparison(token_list, token_position, ast);
        ast = ast_new((AST)
            {
                .tag = AST_BINARY,
                .data.AST_BINARY = {
                    ast,
                    operator,
                    right
                }
            }
        );
    }
    
    return ast;
}

/*
static AST *comma_expression(Token *token_list, size_t *token_position, AST* ast)
{
    TODO("finish comma_expression");    
    Token *operator = &token_list[*token_position]; 
    while(token_list[*token_position].type == COMMA)
    {
        operator = &token_list[*token_position];
         if(next_position(token_position, token_list)) {
            parser_error(*operator, "Missing right expression!\n");
            panic_mode(token_list, token_position);
            return ast;
        }
        AST *right = expression(token_list, token_position, ast);
        if(ast->tag == AST_ASSIGN_EXPR) ast->tag = AST_EXPR;
        ast = ast_new((AST)
            {
                .tag = AST_ASSIGN_EXPR,
                .data.AST_ASSIGN_EXPR = {
                    ast,
                    operator,
                    right
                }
            }            
        );
    }
    return equality();
}
*/

/* Function that implements EXPR rule of grammar */
static AST *expression(Token *token_list, size_t *token_position, AST *ast)
{
   return equality(token_list, token_position, ast);
}

/* Function that implements STAT rule of grammar */
static AST *statement(Token *token_list, size_t *token_position, AST *ast)
{
    /* Add the option for block of statements */
    if(!setjmp(sync_env))
        fprintf(stdout, "Setjmp!\n");

    if(token_list[*token_position].type == EOF_TOKEN)
        return ast;

    ast = expression(token_list, token_position, ast);
    /* Define what type statement is */
    return ast;
}

/* Function that parses tokens into AST using grammar rules*/
static AST *parser(Token *token_list) 
{
    size_t token_position = 0;
    AST *ast = NULL;
    return statement(token_list, &token_position, ast);
}

/* Function that returns value of AST node */
static Value *literal_value(AST *node)
{
    if(node->tag == AST_LITERAL)
        return &node->data.token->literal;
    error("Tried to return non-literal node.");
    abort();
} 

/* Helper function that returns TRUE, FALSE */
static Value is_truth(Value *value, TokenType type)
{
    Value ret;
    if(type == NUMBER_INT) 
        return (ret.boolean_value = (value->integer_value) ? TRUE : FALSE, ret);
    if(type == NUMBER_FLOAT) 
        return (ret.boolean_value = (value->float_value) ? TRUE : FALSE, ret);
    if(value->char_value == NULL) 
        return (ret.boolean_value = FALSE, ret);

    return (ret.boolean_value = TRUE, ret);
}

/* Evaluate part of the Interpreter */

/* Function that evaluates unary expression */
static ValueTagged *evaulate_unary_expression(AST *node)
{
    if(node->tag != AST_UNARY) 
    {
        error("Tried to evaluate non-unary node!");
        abort();
    }

    /* TODO(Add support multiple unary expressions); */

    Value *right = literal_value(node->data.AST_UNARY.right); // change this to evaluate whole
    //ValueTagged *right = (node->data.AST_UNARY.right->tag == AST_LITERAL) ? literal_value(node->data.AST_UNARY.right) : evaluate()
    ValueTagged *result = (ValueTagged *)malloc(sizeof(ValueTagged));
    TokenType right->type = node->data.AST_UNARY.right->data.token->type;
    TokenType operator_type = node->data.AST_UNARY.token->type;
    result->type = right->type;

    switch(operator_type)
    {
        case SUBTRACT:
        {
            if(right->type == STRING) {
                TODO("Throw an error for string value -");
                break;
            }

            if(right->type == NUMBER_FLOAT)
                result->literal.float_value = (-1)*right->float_value;
            if(right->type == NUMBER_INT)
                result->literal.integer_value = (-1)*right->integer_value; 
            if(right->type == TRUE_TOKEN || right_value_type == FALSE_TOKEN)
            {
                result->literal.integer_value = (-1)*right->boolean_value;
                result->type = NUMBER_INT;
            }
            return result;                
        }
        case XOR:
        {
            if(right->type == STRING || right_value_type == NUMBER_FLOAT) 
            {
                TODO("Throw an error for float value ~");
                break;
            }
            if(right->type == NUMBER_INT)
                result->literal.integer_value = ~right->integer_value; 
            if(right->type == TRUE_TOKEN || right_value_type == FALSE_TOKEN)
            {
                result->literal.integer_value = ~right->boolean_value;
                result->type = NUMBER_INT;
            }
            return result;
        }
        case EXCLAMATION:
        {
            result->literal.boolean_value = !is_truth(right, right->type).boolean_value;
            return (result->type = (result->literal.boolean_value) ? TRUE_TOKEN : FALSE_TOKEN, result);
        }
        default:
            error("Unallowed operator on unary expression!"); 
            break;
    }

    free(result);
    abort();
}

static ValueTagged *evaluate_binary_expression(AST *node)
{
    if(node->tag != AST_BINARY)
    {
        error("Tried to evaluate non-binary expression");
        abort();
    }

    //Value *left = literal_value(node->data.AST_BINARY.left); // change this to evaluate whole
    //Value *right = literal_value(node->data.AST_BINARY.right); // change this to evaluate whole

    ValueTagged *left = (node->data.AST_BINARY.left->tag == AST_LITERAL) ? 
                                literal_value(node->data.AST_BINARY.left): 
                                evaluate(node->data.AST_BINARY.left);
    ValueTagged *right = (node->data.AST_BINARY.right->tag == AST_LITERAL) ? 
                                literal_value(node->data.AST_BINARY.right): 
                                evaluate(node->data.AST_BINARY.right);

    TokenType operator_type = node->data.AST_BINARY.token->type;
    ValueTagged *result = (ValueTagged *)malloc(sizeof(ValueTagged));
    
    switch(operator_type)
    {
        case EXCLAMATION_EQUEAL:
            if(left->type == STRING || right->type == STRING)
                TODO("Implement difference for string values.");
            BINARY_COMPARISON(!=, left, left->type, right, right->type, result);
            return ret;
        case DOUBLE_EQUAL:
            if(left->type == STRING || right->type == STRING)
                TODO("Implement equality for string values.");
            BINARY_COMPARISON(==, left, left->type, right, right->type, result);
            return ret;
        case REDIRECTION_RIGHT_GREATER_RELATIONAL:
            if(left->type == STRING || right->type == STRING)
                TODO("Implement greater for string values.");
            BINARY_COMPARISON(>, left, left->type, right, right->type, result);
            return ret;
        case REDIRECTION_LEFT_LESS_RELATIONAL:
            if(left->type == STRING || right->type == STRING)
                TODO("Implement less for string values.");
            BINARY_COMPARISON(<, left, left->type, right, right->type, result);
            return ret;
        case GREATER_EQUAL:
            if(left->type == STRING || right->type == STRING)
                TODO("Implement greater or equal for string values.");
            BINARY_COMPARISON(>=, left, left->type, right, right->type, result);
            return ret;
        case LESS_EQUAL:
            if(left->type == STRING || right->type == STRING)
                TODO("Implement less or equal for string values.");
            BINARY_COMPARISON(<=, left, left->type, right, right->type, result);
            return ret;
        case SUBTRACT:
            if(left->type == STRING || right->type == STRING)
                TODO("Implement subtract for string values.");
            BINARY_OPERATION(-, left, left->type, right, right->type, result);
            return ret;          
        case MULTIPLY:
            if(left->type == STRING || right->type == STRING)
                TODO("Implement multiply for string values.");
            BINARY_OPERATION(*, left, left->type, right, right->type, result);
            return ret;
        case DIVIDE:
            if(left->type == STRING && right->type == STRING)
                TODO("Implement divide for string values.");
            BINARY_OPERATION(/, left, left->type, right, right->type, result);              
            return ret;
        case ADD:
            if(right->type == STRING || left->type == STRING) 
                TODO("Implement addition for string");
            BINARY_OPERATION(+, left, left->type, right, right->type, result);    
            return ret;
        default:
            break;
    }
    free(result);
    abort();
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
        AST *ast = NULL;
        
        read_cmd(pcmd, MAX_LINE_SIZE);
        tokens = tokenizer(pcmd, &number_of_tokens);
        if (tokens != NULL)
        {
            Token *ctox = token_classifier(tokens, number_of_tokens, &number_of_ctokens);
            ast = parser(ctox);
            if(ast != NULL){
                ast_print(ast);
                printf("TEST\n");
                ValueTagged *value = evaulate_unary_expression(ast);
                switch (value->type)
                {
                case NUMBER_INT:
                    fprintf(stdout, "Value INT = %d\n", value->literal.integer_value);
                    break;
                case NUMBER_FLOAT:
                    fprintf(stdout, "Value FLOAT = %lf\n", value->literal.float_value);
                    break;
                case STRING:
                     fprintf(stdout, "Value STRING = %s\n", value->literal.char_value);
                    break;
                case TRUE_TOKEN:
                case FALSE_TOKEN:
                      fprintf(stdout, "Value BOOLEAN = %d\n", value->literal.boolean_value);
                    break;
               default:
                    error("errrrrror!");
                    break;
                }
                ast_free(ast); 
            }
            // exec(pcmd);
            // Deallocate Heap memory

            for (size_t i = 0; i < number_of_tokens; ++i) {
                free(tokens[i]);
            }
            free(tokens);
            for (size_t i = 0; ctox[i].type != EOF_TOKEN; ++i) {
                free(ctox[i].lexeme);
            }
            free(ctox);
        }
        wait(NULL);
    } 
    return 0;
}
