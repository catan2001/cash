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

#ifndef PARSER_H 
#define PARSER_H

/*@Function: next_position
*Helper function for returning next iteration of token_list */
static int next_position(size_t *, Token *);

/*@Function ast_new:
*Function that creates ASTs */
static AST *ast_new(AST );

/*@Function: synchronize
*Helper function for synchronization when parser enters Panic Mode */
static void synchronize(Token *, size_t *);

/*@Function: panic_mode
*Function that is called when error happens */
static void panic_mode(Token *, size_t *);

/*@Function: primary
*Function that implements PRIM rule of grammar */
static AST *primary(Token *, size_t *, AST *);

/*@Function: unary
*Function that implements UNRY rule of grammar */
static AST *unary(Token *, size_t *, AST *);

/*@Function: factor
*Function that implements FACT rule of grammar */
static AST *factor(Token *, size_t *, AST *);

/*@Function: term
*Function that implements TERM rule of grammar */
static AST *term(Token *, size_t *, AST *);

/*@Function: comparison
*Function that implements CMPR rule of grammar */
static AST *comparison(Token *, size_t *, AST *);

/*@Function: equality
*Function that implements EQL rule of grammar */
static AST *equality(Token *, size_t *, AST *);

/*@Function: assignment
*Function that implements ASSIGN expression rule of grammar */
static AST *assignment(Token *, size_t *, AST *);

/*@Function: expression
*Function that implements EXPR rule of grammar */
static AST *expression(Token *, size_t *, AST *);

/*@Function: variable_declaration
*Function that implements VAR-DECL statement rule of grammar*/
static AST *variable_declaration(Token *, size_t *, AST *);

/*@Function: expression_statement
*Function that implements EXPR-STMT statement rule of grammar */
static AST *expression_statement(Token *, size_t *, AST *);

/*@Function: block_statement
*Function that implements BLOCK-STMT statement rule of grammar */
static AST *block_statement(Token *, size_t *, AST *);

/*@Function: block_statement
*Function that implements BLOCK-STMT statement rule of grammar */
static AST *if_statement(Token *, size_t *, AST *);

/*@Function: print_statement
*Function that implements PRINT-STMT statement rule of grammar */
static AST *print_statement(Token *, size_t *, AST *);

/*@Function: statement
*Function that implements STAT statement rule of grammar */
static AST *statement(Token *, size_t *, AST *);

/*@Function: declaration
*Function that implements DECL statement rule of grammar*/
static AST *declaration(Token *, size_t *, AST *);

/*@Function: ast_free
*Function that deletes AST from current node down */
extern void ast_free(AST *);

/*@Function: ast_print
*Function that prints ASTs */
extern void ast_print(AST *);

/*@Function: parser
*Function that parses tokens into AST using grammar rules*/
extern AST **parser(Token *, size_t *);

#endif // PARSER_H 