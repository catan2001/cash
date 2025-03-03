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

#ifndef LEXER_H
#define LEXER_H

/*@compare_token
*Function: Compares token with value.*/
static int compare_token(const char *, const char *);

/*@add_token
*Helper Function: Adds a token to the array*/
static char **add_token(char **, size_t *, char *);

/*@is_digit
*Helper Function: Determines if character is digit*/
static int is_digit(const char c);

/*@is_letter
*Helper Function: Determines if character is letter*/
static int is_letter(char c);

/*@type_of_character
*Helper Function: Determines type of character*/
static CharacterType type_of_character(char c);

/*@classify_special_token
*Helper Function: Classifies special character token*/
static TokenType classify_special_token(const char *, Token *);

/*@classify_string
*Helper Function: Classifies strings*/
static TokenType classify_string(char *, Token *);

/*@classify_number
*Helper Function: Classifies numbers*/
static TokenType classify_number(const char *, Token *);

/*@classify_reserved_words
*Helper Function: Classifies reserved words*/
static TokenType classify_reserved_words(const char *, Token *);

/*@tokenizer
*Function: Separates line into individual null terminated tokens.*/
extern char **tokenizer(char *, size_t *);

/*@eof_token
*Function: Allocates and assigns EOF to the end of Token *, returns new number of ctokens*/
extern size_t eof_token(Token **, size_t );

/*@token_classifier
*Function: Classifies tokens and returns pointer to classified tokens*/
extern Token *token_classifier(char **, Token *, const size_t , size_t *);

#endif // LEXER_H