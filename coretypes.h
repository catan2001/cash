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
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef CORETYPES_H
#define CORETYPES_H

#define TRUE 1
#define FALSE 0
#define MAX_LINE_SIZE 1000
#define FILE_PATH_SIZE 100
#define MAX_ARG_CNT 127


// TsodingDaily <3
#define TODO(msg) do {                                                  \
        fprintf(stderr, "%s:%d TODO: %s\n", __FILE__, __LINE__, msg);   \
        abort();                                                        \
    } while(0)

/* Definition used for interpreter binary operations */    
#define BINARY_ADD_SUB_MULTIPLY_OPERATION(op, left, right, result)                                              \
    do                                                                                                          \
    {                                                                                                           \
        if ((left)->type == NUMBER_FLOAT && (right)->type == NUMBER_FLOAT)                                      \
        {                                                                                                       \
            (result)->literal.float_value = (left)->literal.float_value op (right)->literal.float_value;        \
            (result)->type = NUMBER_FLOAT;                                                                      \
        }                                                                                                       \
        else if ((left)->type == NUMBER_FLOAT && (right)->type == NUMBER_INT)                                   \
        {                                                                                                       \
            (result)->literal.float_value = (left)->literal.float_value op (right)->literal.integer_value;      \
            (result)->type = NUMBER_FLOAT;                                                                      \
        }                                                                                                       \
        else if ((left)->type == NUMBER_FLOAT && ((right)->type == TRUE_TOKEN || (right)->type == FALSE_TOKEN)) \
        {                                                                                                       \
            (result)->literal.float_value = (left)->literal.float_value op (right)->literal.boolean_value;      \
            (result)->type = NUMBER_FLOAT;                                                                      \
        }                                                                                                       \
        else if ((left)->type == NUMBER_INT && (right)->type == NUMBER_FLOAT)                                   \
        {                                                                                                       \
            (result)->literal.float_value = (left)->literal.integer_value op (right)->literal.float_value;      \
            (result)->type = NUMBER_FLOAT;                                                                      \
        }                                                                                                       \
        else if ((left)->type == NUMBER_INT && (right)->type == NUMBER_INT)                                     \
        {                                                                                                       \
            (result)->literal.integer_value = (left)->literal.integer_value op (right)->literal.integer_value;  \
            (result)->type = NUMBER_INT;                                                                        \
        }                                                                                                       \
        else if ((left)->type == NUMBER_INT && ((right)->type == TRUE_TOKEN || (right)->type == FALSE_TOKEN))   \
        {                                                                                                       \
            (result)->literal.integer_value = (left)->literal.integer_value op (right)->literal.boolean_value;  \
            (result)->type = NUMBER_INT;                                                                        \
        }                                                                                                       \
        else if (((left)->type == TRUE_TOKEN || (left)->type == FALSE_TOKEN) && (right)->type == NUMBER_INT)    \
        {                                                                                                       \
            (result)->literal.integer_value = (left)->literal.boolean_value op (right)->literal.integer_value;  \
            (result)->type = NUMBER_INT;                                                                        \
        }                                                                                                       \
        else if (((left)->type == TRUE_TOKEN || (left)->type == FALSE_TOKEN) && (right)->type == NUMBER_FLOAT)  \
        {                                                                                                       \
            (result)->literal.float_value = (left)->literal.boolean_value op (right)->literal.float_value;      \
            (result)->type = NUMBER_FLOAT;                                                                      \
        }                                                                                                       \
        else                                                                                                    \
        {                                                                                                       \
            (result)->literal.integer_value = (left)->literal.boolean_value op (right)->literal.boolean_value;  \
            (result)->type = NUMBER_INT;                                                                        \
        }                                                                                                       \
    } while (0)

/* Definition used for interpreter binary divide operations */
#define BINARY_DIVIDE_OPERATION(op, left, right, result)                                                                        \
    do                                                                                                                          \
    {                                                                                                                           \
        if ((left)->type == NUMBER_FLOAT && (right)->type == NUMBER_FLOAT)                                                      \
        {                                                                                                                       \
            (result)->literal.float_value = (left)->literal.float_value op (right)->literal.float_value;                        \
            (result)->type = NUMBER_FLOAT;                                                                                      \
        }                                                                                                                       \
        else if ((left)->type == NUMBER_FLOAT && (right)->type == NUMBER_INT)                                                   \
        {                                                                                                                       \
            (result)->literal.float_value = (left)->literal.float_value op (double)(right)->literal.integer_value;              \
            (result)->type = NUMBER_FLOAT;                                                                                      \
        }                                                                                                                       \
        else if ((left)->type == NUMBER_FLOAT && ((right)->type == TRUE_TOKEN || (right)->type == FALSE_TOKEN))                 \
        {                                                                                                                       \
            (result)->literal.float_value = (left)->literal.float_value op (double)(right)->literal.boolean_value;              \
            (result)->type = NUMBER_FLOAT;                                                                                      \
        }                                                                                                                       \
        else if ((left)->type == NUMBER_INT && (right)->type == NUMBER_FLOAT)                                                   \
        {                                                                                                                       \
            (result)->literal.float_value = (double)(left)->literal.integer_value op (right)->literal.float_value;              \
            (result)->type = NUMBER_FLOAT;                                                                                      \
        }                                                                                                                       \
        else if ((left)->type == NUMBER_INT && (right)->type == NUMBER_INT)                                                     \
        {                                                                                                                       \
            (result)->literal.float_value = (left)->literal.integer_value op (double)(right)->literal.integer_value;            \
            (result)->type = NUMBER_FLOAT;                                                                                      \
        }                                                                                                                       \
        else if ((left)->type == NUMBER_INT && ((right)->type == TRUE_TOKEN || (right)->type == FALSE_TOKEN))                   \
        {                                                                                                                       \
            (result)->literal.float_value = (double)(left)->literal.integer_value op (double)(right)->literal.boolean_value;    \
            (result)->type = NUMBER_FLOAT;                                                                                      \
        }                                                                                                                       \
        else if (((left)->type == TRUE_TOKEN || (left)->type == FALSE_TOKEN) && (right)->type == NUMBER_INT)                    \
        {                                                                                                                       \
            (result)->literal.float_value = (double)(left)->literal.boolean_value op (double)(right)->literal.integer_value;    \
            (result)->type = NUMBER_FLOAT;                                                                                      \
        }                                                                                                                       \
        else if (((left)->type == TRUE_TOKEN || (left)->type == FALSE_TOKEN) && (right)->type == NUMBER_FLOAT)                  \
        {                                                                                                                       \
            (result)->literal.float_value = (double)(left)->literal.boolean_value op (double)(right)->literal.float_value;      \
            (result)->type = NUMBER_FLOAT;                                                                                      \
        }                                                                                                                       \
        else                                                                                                                    \
        {                                                                                                                       \
            (result)->literal.float_value = (double)(left)->literal.boolean_value op (double)(right)->literal.boolean_value;    \
            (result)->type = NUMBER_FLOAT;                                                                                      \
        }                                                                                                                       \
    } while (0)

 /* Definition used for interpreter binary comparison operations */  
#define BINARY_COMPARISON_OPERATION(op, left, right, result)                                                    \
    do                                                                                                          \
    {                                                                                                           \
        if ((left)->type == NUMBER_FLOAT && (right)->type == NUMBER_FLOAT)                                      \
            (result)->literal.boolean_value = (left)->literal.float_value op (right)->literal.float_value;      \
        else if ((left)->type == NUMBER_FLOAT && (right)->type == NUMBER_INT)                                   \
            (result)->literal.boolean_value = (left)->literal.float_value op (right)->literal.integer_value;    \
        else if ((left)->type == NUMBER_FLOAT && ((right)->type == TRUE_TOKEN || (right)->type == FALSE_TOKEN)) \
            (result)->literal.boolean_value = (left)->literal.float_value op (right)->literal.boolean_value;    \
        else if ((left)->type == NUMBER_INT && (right)->type == NUMBER_FLOAT)                                   \
            (result)->literal.boolean_value = (left)->literal.integer_value op (right)->literal.float_value;    \
        else if ((left)->type == NUMBER_INT && (right)->type == NUMBER_INT)                                     \
            (result)->literal.boolean_value = (left)->literal.integer_value op (right)->literal.integer_value;  \
        else if ((left)->type == NUMBER_INT && ((right)->type == TRUE_TOKEN || (right)->type == FALSE_TOKEN))   \
            (result)->literal.boolean_value = (left)->literal.integer_value op (right)->literal.boolean_value;  \
        else if (((left)->type == TRUE_TOKEN || (left)->type == FALSE_TOKEN) && (right)->type == NUMBER_INT)    \
            (result)->literal.boolean_value = (left)->literal.boolean_value op (right)->literal.integer_value;  \
        else if (((left)->type == TRUE_TOKEN || (left)->type == FALSE_TOKEN) && (right)->type == NUMBER_FLOAT)  \
            (result)->literal.boolean_value  = (left)->literal.boolean_value op (right)->literal.float_value;   \
        else                                                                                                    \
            (result)->literal.boolean_value = (left)->literal.boolean_value op (right)->literal.boolean_value;  \
        (result)->type = (result->literal.boolean_value) ? TRUE_TOKEN : FALSE_TOKEN;                            \
    } while (0)

/*@Type TokenType: Enumerating type for defining token type */
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
    DOUBLE_AND,
    DOUBLE_OR,       

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
    RETURN,
    EOF_TOKEN     
} TokenType;

/*@Type CharacterType:  Enumerating type used for defining character type */
typedef enum character_t
{
    SPECIAL,
    QUOTES,
    SPACE,
    NEW_LINE,
    OTHER,
    UNUSED_CHARACTERS
} CharacterType;

/*@Type Value: Abstract the type for tokenizer, classifier and parser*/
typedef union value_u
{
    long long int integer_value;    
    double float_value;             
    char *char_value;                
    int boolean_value;              
} Value;

/*@Type AST: Structure used for parsing AST tree */
typedef struct AST AST; 

/*@Type ValueTagged: Abstract type used by interpreter */
typedef struct value_tag_s
{
    TokenType type;
    Value literal;
} ValueTagged;

/*@Type Token: Used for tokenization*/
typedef struct token_s  
{
    TokenType type;
    char *lexeme;
    Value literal;
    size_t line_number; // Add this after implementing error correction
} Token;

/*@Type Environment: used for runtime environment */
typedef struct environment_s
{
    enum type
    {
        ENV_VARIABLE,
        ENV_FUNCTION
    } type;
    
    union 
    {
        ValueTagged value;
        struct ENV_FUNCTION {AST *definition;} ENV_FUNCTION;
    } data;

    char *name;
} Environment;

/*@Type EnvironmentMap: Environment map with pointer to allocated Environment and its size */
typedef struct EnvironmentMap EnvironmentMap;

/*@Type EnvironmentMap: Environment map with pointer to allocated Environment and its size */
struct EnvironmentMap
{
    Environment *env;
    EnvironmentMap *env_enclosing;
    size_t env_size;
};

/*@Type ReservedWordMapType: Structure used for classifying reserved words */
typedef struct reserved_word_map_t
{
    const char *reserved_word;
    TokenType type;
} ReservedWordMapType;

/*@Type AST: Structure, previously forward referenced */
struct AST 
{
    enum tag
    {
        AST_LITERAL,
        AST_IDENTIFIER,
    
        AST_FUNCT_DECL_STMT,
        AST_VAR_DECL_STMT,
        AST_EXPR_STMT,
        AST_BLOCK_STMT,
        AST_IF_STMT,
    	AST_WHILE_STMT,
        AST_FOR_STMT,
        AST_PRINT_STMT,
        AST_RETURN_STMT,

        AST_ASSIGN_EXPR,
        AST_LOGICAL_EXPR,
        AST_BINARY_EXPR,
        AST_GROUPING_EXPR,
        AST_CALL_EXPR,
        AST_UNARY_EXPR
    } tag;
    union 
    {
        Token *token;
        struct AST_VAR_DECL_STMT {Token *name; AST *init;} AST_VAR_DECL_STMT;
        struct AST_FUNCT_DECL_STMT {Token *name; AST **parameters; AST **stmt_list; size_t param_num; size_t stmt_num;} AST_FUNCT_DECL_STMT;
        struct AST_EXPR_STMT {AST *expr;} AST_EXPR_STMT;
        struct AST_BLOCK_STMT {AST **stmt_list; size_t stmt_num;} AST_BLOCK_STMT;
        struct AST_IF_STMT {AST *condition; AST *true_branch; AST *else_branch;} AST_IF_STMT;
        struct AST_WHILE_STMT {AST *condition; AST *body;} AST_WHILE_STMT;
        struct AST_FOR_STMT {AST *initializer; AST *condition; AST *increment; AST *body;} AST_FOR_STMT;
        struct AST_PRINT_STMT {AST *expr;} AST_PRINT_STMT;
        struct AST_RETURN_STMT {AST *expr;} AST_RETURN_STMT;
        struct AST_ASSIGN_EXPR {Token *token; AST *expr;} AST_ASSIGN_EXPR;
        struct AST_LOGICAL_EXPR {AST *left; Token *token; AST *right;} AST_LOGICAL_EXPR;
        struct AST_BINARY_EXPR {AST *left; Token *token; AST *right;} AST_BINARY_EXPR;
        struct AST_GROUPING_EXPR {AST *left; Token *token;} AST_GROUPING_EXPR;
        struct AST_CALL_EXPR {AST *callee; AST **stmt_list; size_t stmt_num;} AST_CALL_EXPR;
        struct AST_UNARY_EXPR {AST *right; Token *token;} AST_UNARY_EXPR;
    } data;
};  

#endif // CORETYPES_H
