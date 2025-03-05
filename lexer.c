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
#include <string.h>
#include "coretypes.h"
#include "lexer.h"
#include "error.h"

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

static int compare_token(const char *token, const char *value) 
{
    if (strlen(token) != strlen(value))
        return EXIT_FAILURE;
    for (size_t i = 0; token[i] != 0; ++i) {
        if (token[i] != value[i])
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}       

static char **add_token(char **tokens, size_t *token_cnt, char *token)
{
    char **new_tokens = realloc(tokens, (*token_cnt + 1) * sizeof(char *));
    /* Deallocate memory if realloc fails */
    if (!new_tokens) {
        INTERNAL_ERROR("Failed to reallocate memory during add_token!");
        for (size_t j = 0; j < *token_cnt; ++j)
            free(tokens[j]);
        free(tokens);
        exit(EXIT_FAILURE);
    }
    tokens = new_tokens;

    /* Copy token into an array of tokens */
    tokens[*token_cnt] = strdup(token);
    if (!tokens[*token_cnt]) {
        INTERNAL_ERROR("Tokenizer strdup failed!");
        exit(EXIT_FAILURE);
    }
    /* Increment token count */
    (*token_cnt)++;
    return tokens;
}

static int is_digit(const char c) 
{
    return c >= '0' && c <= '9';
}

static int is_letter(char c) 
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static CharacterType type_of_character(char c) 
{
    /* Determine if letter, number (float or int) or underscore */
    if(is_digit(c) || is_letter(c) || c == '_' || c =='.')
        return OTHER;

    /* Determine if Special characters */
    switch (c) {
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

static TokenType classify_special_token(const char *token, Token *ctoken) 
{   
    ctoken->lexeme = strdup(token);
    ctoken->literal.char_value = ctoken->lexeme;
    if(!ctoken->lexeme)
        return FAILED_TO_CLASSIFY;

    if (type_of_character(token[1]) == SPECIAL) {
        switch (token[0]) {
        case '<':
            return (token[1] == '=') ? LESS_EQUAL : REDIRECTION_LEFT_LESS_RELATIONAL;
        case '>':
            return (token[1] == '=') ? GREATER_EQUAL : REDIRECTION_RIGHT_GREATER_RELATIONAL;
        case '&':
            return (token[1] == '&') ? DOUBLE_AND : AND;
        case '|':
            return (token[1] == '|') ? DOUBLE_OR : PIPE;
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

static TokenType classify_string(char *token, Token *ctoken) 
{
    char tmp_char = token[strlen(token)-1];
    
    /* Temporary change token and add string that is inside quotes */
    token[strlen(token) - 1] = '\0';
    ctoken->literal.char_value = strdup(&token[1]);
    ctoken->lexeme = ctoken->literal.char_value;
    if(!ctoken->literal.char_value)
        return FAILED_TO_CLASSIFY;
    
    token[strlen(token) - 1] = tmp_char;
    return STRING;
}

static TokenType classify_number(const char *token, Token *ctoken) 
{
    ctoken->lexeme = strdup(token);
    if(!ctoken->lexeme)
        return FAILED_TO_CLASSIFY;

    for(int i = 0; token[i]; ++i) {
        if(token[i] == '.') {
            ctoken->literal.float_value = atof(token);
            return NUMBER_FLOAT;
        }
    }
    ctoken->literal.integer_value = atoi(token);
    return NUMBER_INT;
}

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

extern char **tokenizer(char *cmd, size_t *token_cnt) 
{
    if (cmd == NULL || !cmd[0] || cmd[0] == '\n')
        return NULL;

    size_t head_position = 0;
    char **tokens = NULL;

    /* Reinitialize token count back to zero */
    *token_cnt = 0;
    
    for (size_t i = 0; cmd[i]; ++i) {
        switch (type_of_character(cmd[i])) {
        case UNUSED_CHARACTERS:
            fprintf(stderr, "error: Character is not allowed in Cash!\n\t Error at: %d! \n", i);
            set_error_flag();
            break;
        case SPECIAL:
            char special_token[3] = {cmd[i], '\0', '\0'};

            /* Copy previous token into the array */
            if (head_position < i) {
                cmd[i] = '\0'; 
                tokens = add_token(tokens, token_cnt, &cmd[head_position]);
                /* Position head onto new token */
                head_position = i + 1;
            }

            if (special_token[0] == '#')
                return tokens; // TODO: Add comment block..

            /*Copy special token into the array*/
            if (cmd[i+1] == '=' || cmd[i+1] == '>' || cmd[i+1] == '<' || cmd[i+1] == '&' || cmd[i+1] == '|')
                special_token[1] = cmd[++i];

            tokens = add_token(tokens, token_cnt, special_token);
            break;

        case QUOTES:
            /*Report an error if an identifier is followed by quotes*/
            if (head_position < i) {
                fprintf(stderr,"error: Syntax Error at %d! Expected valid separator after identifier.\n", i);
                set_error_flag();
                return tokens;
            }

            while(cmd[++i] != '"') {
                /*Report a warning if quotes are unterminated*/
                if(cmd[i] == '\n' || cmd[i] == '\0') { 
                    fprintf(stderr, "error: Unterminated string at %d! \n", i);
                    set_error_flag();
                    break;
                } 
            }
            /*Report an error if an quotes are followed by an identifier*/
            if(type_of_character(cmd[i+1]) == OTHER) {
                fprintf(stderr, "error: Syntax Error at %d! Unexpected token.\n", i);
                set_error_flag();
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
            if (head_position < i) {
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

extern size_t eof_token(Token **ctoken, size_t number_of_ctokens) 
{
    number_of_ctokens++;
    *ctoken = realloc(*ctoken, sizeof(Token) * number_of_ctokens);
    (*ctoken)[number_of_ctokens - 1].lexeme = NULL;
    (*ctoken)[number_of_ctokens - 1].literal.char_value = NULL;
    (*ctoken)[number_of_ctokens - 1].type = EOF_TOKEN;
    return number_of_ctokens;
}

extern Token *token_classifier(char **token, Token *ctoken, const size_t number_of_tokens, size_t *number_of_ctokens) 
{
    if (!token)
        return NULL;

    ctoken = realloc(ctoken, sizeof(Token) * (number_of_tokens));
    if (ctoken == NULL) {
        fprintf(stderr, "Failed to allocate memory for ctoken!");
        free(ctoken);
    }

    for (size_t i = 0; i < number_of_tokens - *number_of_ctokens; ++i) {
        if (type_of_character(token[i][0]) == SPECIAL)
            ctoken[*number_of_ctokens + i].type = classify_special_token(token[i], &ctoken[*number_of_ctokens + i]);
        else if (type_of_character(token[i][0]) == QUOTES)
            ctoken[*number_of_ctokens + i].type = classify_string(token[i], &ctoken[*number_of_ctokens + i]);
        else if (is_digit(token[i][0]))
            ctoken[*number_of_ctokens + i].type = classify_number(token[i], &ctoken[*number_of_ctokens + i]);
        else 
            ctoken[*number_of_ctokens + i].type = classify_reserved_words(token[i], &ctoken[*number_of_ctokens + i]);

        if (ctoken[*number_of_ctokens + i].type == FAILED_TO_CLASSIFY) {
            fprintf(stderr, "error: Failed to classify token %s\n", token[i]);
            set_error_flag();
            for (size_t j = 0; j < (i + 1); j++)
                free(ctoken[*number_of_ctokens + j].lexeme);
            free(ctoken);

            *number_of_ctokens = 0;
            return NULL;
        }
    }

    *number_of_ctokens = number_of_tokens;
    return ctoken;
}


