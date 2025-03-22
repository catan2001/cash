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

#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include "coretypes.h"
#include "error.h"
#include "parser.h"

static jmp_buf sync_env;

static int next_position(size_t *current_position, Token *token_list) 
{
    if(token_list[*current_position+1].type == EOF_TOKEN || token_list[*current_position].type == EOF_TOKEN)
        return 1;
        
    (*current_position)++;
    printf("New position %d, token %s\n", (*current_position), token_list[*current_position].lexeme);
    return 0;
}

static AST *ast_new(AST ast) 
{
    AST *ast_ptr = malloc(sizeof(AST));
    if(ast_ptr) *ast_ptr = ast;
    else
        error("ast_new failed to allocate memory!", __FILE__, __LINE__);
    return ast_ptr;
}

extern void ast_print(AST *ast) 
{
    if(!ast) {
        fprintf(stderr, "Error! AST IS NULL!\n");
        return;
    }

    switch(ast->tag) {
        case AST_VAR_DECL_STMT:
        {
            fprintf(stdout, "Variable Declaration Statement Node: %s\n", ast->data.AST_VAR_DECL_STMT.name->lexeme);
            ast_print(ast->data.AST_VAR_DECL_STMT.init);
            break;
        }
        case AST_FUNCT_DECL_STMT:
        {
            fprintf(stdout, "Function Declaration Statement Node: %s\n", ast->data.AST_FUNCT_DECL_STMT.name->lexeme);
            fprintf(stdout, "Function Declaration Statement parameters: \n");
            for(size_t i = 0; i < ast->data.AST_FUNCT_DECL_STMT.param_num; ++i)
                ast_print(ast->data.AST_FUNCT_DECL_STMT.parameters[i]);
            fprintf(stdout, "Function Declaration Statement block: \n");
            for(size_t i = 0; i < ast->data.AST_FUNCT_DECL_STMT.stmt_num; ++i)
                ast_print(ast->data.AST_FUNCT_DECL_STMT.stmt_list[i]);
            break;
        }
        case AST_EXPR_STMT: 
        {
            fprintf(stdout, "Expression Statement Node.\n");
            ast_print(ast->data.AST_EXPR_STMT.expr);
            break;
        }
        case AST_BLOCK_STMT:
        {
            fprintf(stdout, "Block Statement Node.\n");
            for(size_t i = 0; i < ast->data.AST_BLOCK_STMT.stmt_num; ++i)
                ast_print(ast->data.AST_BLOCK_STMT.stmt_list[i]);
            break;
        }
        case AST_IF_STMT:
        {
            fprintf(stdout, "If statement Node.\n");
            fprintf(stdout, "Condition AST:\n");
            ast_print(ast->data.AST_IF_STMT.condition);
            fprintf(stdout, "True Branch AST:\n");
            ast_print(ast->data.AST_IF_STMT.true_branch);
            fprintf(stdout, "Else Branch AST:\n");
            if(ast->data.AST_IF_STMT.else_branch != NULL)
                ast_print(ast->data.AST_IF_STMT.else_branch);
            break;
        }
        case AST_WHILE_STMT:
        {
            fprintf(stdout, "While statement Node.\n");
            fprintf(stdout, "Condition AST:\n");
            ast_print(ast->data.AST_WHILE_STMT.condition);
            fprintf(stdout, "True Branch AST:\n");
            if(ast->data.AST_WHILE_STMT.body != NULL)
                ast_print(ast->data.AST_WHILE_STMT.body);
            break;
        }        
        case AST_FOR_STMT:
        {
            fprintf(stdout, "For Statement Node.\n");
            fprintf(stdout, "Init node ast:\n");
            if(ast->data.AST_FOR_STMT.initializer != NULL)
                ast_print(ast->data.AST_FOR_STMT.initializer);
            else
                fprintf(stdout, "Init Node is NULL\n");
 
            fprintf(stdout, "Condition node ast:\n");
            if(ast->data.AST_FOR_STMT.condition != NULL)
                ast_print(ast->data.AST_FOR_STMT.condition);
            else
                fprintf(stdout, "condition Node is NULL\n");

            fprintf(stdout, "Increment node ast:\n");
            if(ast->data.AST_FOR_STMT.increment != NULL)
                ast_print(ast->data.AST_FOR_STMT.increment);
            else
                fprintf(stdout, "Increment Node is NULL\n");

            fprintf(stdout, "Body node ast:\n");
            ast_print(ast->data.AST_FOR_STMT.body);
        }
        case AST_ECHO_STMT: 
        {
            fprintf(stdout, "echo statement Node.\n");
            ast_print(ast->data.AST_ECHO_STMT.expr);
            break;
        }
        case AST_RETURN_STMT:
        {
            fprintf(stdout, "Return statement Node.\n");
            ast_print(ast->data.AST_RETURN_STMT.expr);
            break;
        }
        case AST_TIME_STMT:
        {
            fprintf(stdout, "Time statement Node.\n");
            break;
        }
        case AST_CLEAR_STMT:
        {
            fprintf(stdout, "Clear statement Node.\n");
            break;
        }
        case AST_CD_STMT: 
        {
            fprintf(stdout, "cd statement Node.\n");
            ast_print(ast->data.AST_CD_STMT.expr);
            break;
        }
        case AST_RUN_STMT:
        {     
            fprintf(stdout, "run statement node\n");
            if(ast->data.AST_RUN_STMT.program_name != NULL)
                fprintf(stdout, "Program name: %s\n", ast->data.AST_RUN_STMT.program_name->lexeme);
            for(size_t i = 0; i < ast->data.AST_RUN_STMT.arg_num; ++i)
                ast_print(ast->data.AST_RUN_STMT.args_list[i]);
            break;
        }
        case AST_ASSIGN_EXPR:
        {
            fprintf(stdout, "Assignment Expression Node: %s\n", ast->data.AST_ASSIGN_EXPR.token->lexeme);
            ast_print(ast->data.AST_ASSIGN_EXPR.expr);
            break;               
        }
        case AST_BINARY_EXPR:
        {
            fprintf(stdout, "Binary Node: %s\n", ast->data.AST_BINARY_EXPR.token->lexeme);
            ast_print(ast->data.AST_BINARY_EXPR.left);
            ast_print(ast->data.AST_BINARY_EXPR.right);
            break;
        }
        case AST_CALL_EXPR:
        {
            fprintf(stdout, "Call Expr node: \n");
            ast_print(ast->data.AST_CALL_EXPR.callee);
            for(size_t i = 0; i < ast->data.AST_CALL_EXPR.stmt_num; ++i) {
                fprintf(stdout, "Argument %d:\n", i);
                ast_print(ast->data.AST_CALL_EXPR.stmt_list[i]);
            }
            break;
        } 
        case AST_UNARY_EXPR:
        {
            fprintf(stdout, "Unary Node: %s\n", ast->data.AST_UNARY_EXPR.token->lexeme);
            ast_print(ast->data.AST_UNARY_EXPR.right);
            break;
        }
        case AST_IDENTIFIER:
        {
            fprintf(stdout, "Identifier node: %s\n", ast->data.token->lexeme);
            break;
        }
        case AST_LITERAL:
        {
            fprintf(stdout, "Literal Node: %s\n", ast->data.token->lexeme);
            break;
        }
        default:
            fprintf(stderr, "Error! Cannot print AST node: %s\n", ast->data.token->lexeme);
            break;
    }
}

extern void ast_free(AST *ast) 
{
    if(!ast) {
        fprintf(stderr, "%s: %d Error: Cannot free AST: %s\n", __FILE__, __LINE__, ast->data.token->lexeme);
        return;
    }
    switch(ast->tag) {
        case AST_VAR_DECL_STMT:
        {
            if(ast->data.AST_VAR_DECL_STMT.init != NULL)
                ast_free(ast->data.AST_VAR_DECL_STMT.init);
            break;
        }
        case AST_FUNCT_DECL_STMT:
        {
            for(size_t i = 0; i < ast->data.AST_FUNCT_DECL_STMT.param_num; ++i)
                ast_free(ast->data.AST_FUNCT_DECL_STMT.parameters[i]);
            if(ast->data.AST_FUNCT_DECL_STMT.parameters != NULL)
                free(ast->data.AST_FUNCT_DECL_STMT.parameters);
            for(size_t i = 0; i < ast->data.AST_FUNCT_DECL_STMT.stmt_num; ++i)
                ast_free(ast->data.AST_FUNCT_DECL_STMT.stmt_list[i]);
            if(ast->data.AST_FUNCT_DECL_STMT.stmt_list != NULL)
                free(ast->data.AST_FUNCT_DECL_STMT.stmt_list);
            break;
        }
        case AST_EXPR_STMT: 
        {
            ast_free(ast->data.AST_EXPR_STMT.expr);
            break;
        }
        case AST_BLOCK_STMT:
        {
            for(size_t i = 0; i < ast->data.AST_BLOCK_STMT.stmt_num; ++i)
                ast_free(ast->data.AST_BLOCK_STMT.stmt_list[i]);
            free(ast->data.AST_BLOCK_STMT.stmt_list);
            break;
        }
        case AST_IF_STMT:
        {
            ast_free(ast->data.AST_IF_STMT.condition);
            ast_free(ast->data.AST_IF_STMT.true_branch);
            if(ast->data.AST_IF_STMT.else_branch != NULL) 
                ast_free(ast->data.AST_IF_STMT.else_branch);  
            break; 
        }
        case AST_WHILE_STMT:
        {
            if(ast->data.AST_WHILE_STMT.condition != NULL)
                ast_free(ast->data.AST_WHILE_STMT.condition);
            if(ast->data.AST_WHILE_STMT.body != NULL)
                ast_free(ast->data.AST_WHILE_STMT.body);
            break;
        }
        case AST_FOR_STMT:
        {
            if(ast->data.AST_FOR_STMT.initializer != NULL)
                ast_free(ast->data.AST_FOR_STMT.initializer);
            if(ast->data.AST_FOR_STMT.condition != NULL)
                ast_free(ast->data.AST_FOR_STMT.condition);
            if(ast->data.AST_FOR_STMT.increment != NULL)
                ast_free(ast->data.AST_FOR_STMT.increment);
            ast_free(ast->data.AST_FOR_STMT.body);
            break;
        } 
        case AST_ECHO_STMT: 
        {
            ast_free(ast->data.AST_ECHO_STMT.expr);
            break;
        }
        case AST_RETURN_STMT:
        {
            ast_free(ast->data.AST_RETURN_STMT.expr);
            break;
        }
        case AST_CD_STMT: 
        {
            if(ast->data.AST_CD_STMT.expr != NULL)
                ast_free(ast->data.AST_CD_STMT.expr);
            break;
        }
        case AST_RUN_STMT:
        {     
            for(size_t i = 0; i < ast->data.AST_RUN_STMT.arg_num; ++i)
                ast_free(ast->data.AST_RUN_STMT.args_list[i]);
            if(ast->data.AST_RUN_STMT.args_list != NULL)
                free(ast->data.AST_RUN_STMT.args_list);
            break;
        }
        /* AST_TIME_STMT is default deallocation */
        /* AST_CLEAR_STMT is default deallocation */
        case AST_ASSIGN_EXPR:
        {
            ast_free(ast->data.AST_ASSIGN_EXPR.expr);
            break;               
        }
        case AST_BINARY_EXPR: 
        {
            ast_free(ast->data.AST_BINARY_EXPR.left);
            ast_free(ast->data.AST_BINARY_EXPR.right);
            break;
        }
        case AST_CALL_EXPR:
        {
            for(size_t i = 0; i < ast->data.AST_CALL_EXPR.stmt_num; ++i)
                ast_free(ast->data.AST_CALL_EXPR.stmt_list[i]);
            if(ast->data.AST_CALL_EXPR.stmt_list != NULL) {
                free(ast->data.AST_CALL_EXPR.stmt_list);
            }
            ast_free(ast->data.AST_CALL_EXPR.callee);
            break;
        }
        case AST_UNARY_EXPR: 
        {
            ast_free(ast->data.AST_UNARY_EXPR.right);
            break;
        }
        case AST_GROUPING_EXPR:
        {
            ast_free(ast->data.AST_GROUPING_EXPR.left);
            break;
        }
        default:
            break;
    }
    free(ast);
}

static void synchronize(Token *token_list, size_t *token_position) 
{
    while(!next_position(token_position, token_list)) {
       switch(token_list[*token_position].type) {
            case SEMICOLON:
            case FUNCT:
            case CLASS:
            case STRUCT:
            case VAR:
            case FOR:
            case IF:
            case WHILE:
            case ECHO:
            case EXEC:
            case CD:
            case TIME:
            case CLEAR:
                return;
        } 
    }
    // set token_position to EOF_TOKEN
    (*token_position)++;
}

static void panic_mode(Token *token_list, size_t *token_position) 
{
    synchronize(token_list, token_position);
    // Jump back to sync_env
    longjmp(sync_env, 1);
}

static AST *expression(Token *, size_t *, AST *);

static AST *primary(Token *token_list, size_t *token_position, AST *ast) 
{
    switch (token_list[*token_position].type) {
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
            if (token_list[*token_position].type == RIGHT_PARENTHESIS) {
                next_position(token_position, token_list);
                return ast;
            }
        }
        case IDENTIFIER:
        {
            ast = ast_new((AST){
                .tag = AST_IDENTIFIER,
                .data.token = &token_list[*token_position]}); 
            next_position(token_position, token_list);
            return ast;           
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

static AST *call(Token *token_list, size_t *token_position, AST *ast)
{   
    ast = primary(token_list, token_position, ast);

    if(token_list[*token_position].type == LEFT_PARENTHESIS) {
        if(next_position(token_position, token_list)) {TODO("Add error later");}
        AST **args = NULL;
        AST *expr = NULL;
        size_t stmt_num = 0;

        if(token_list[*token_position].type != RIGHT_PARENTHESIS) {
            do {
                args = realloc(args, sizeof(AST *) * (stmt_num + 1));
                args[stmt_num++] = expression(token_list, token_position, expr);
                if(stmt_num >= MAX_ARG_CNT) {TODO("Add error later!");}
            } while(token_list[*token_position].type == COMMA && !next_position(token_position, token_list));
            
            if(token_list[*token_position].type != RIGHT_PARENTHESIS) {TODO("Add error");}
        }
        
        if(next_position(token_position, token_list)) {TODO("Add error later");}

        ast = ast_new((AST)
            {
                .tag = AST_CALL_EXPR,
                .data.AST_CALL_EXPR = {
                    ast,
                    args,
                    stmt_num
                }
            }
        );
    }
    return ast;
}

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
                .tag = AST_UNARY_EXPR,
                .data.AST_UNARY_EXPR = {
                    right,
                    operator,
                }
            }
        );
        return ast;
    }
    return call(token_list, token_position, ast);
}

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
                .tag = AST_BINARY_EXPR,
                .data.AST_BINARY_EXPR = {
                    ast,
                    operator,
                    right
                }
            }
        );
    }
    return ast;
}

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
                .tag = AST_BINARY_EXPR,
                .data.AST_BINARY_EXPR = {
                    ast,
                    operator,
                    right
                }
            }
        );
    }
    return ast;
}

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
                .tag = AST_BINARY_EXPR,
                .data.AST_BINARY_EXPR = {
                    ast,
                    operator,
                    right
                }
            }
        );
    }
    return ast;
}

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
                .tag = AST_BINARY_EXPR,
                .data.AST_BINARY_EXPR = {
                    ast,
                    operator,
                    right
                }
            }
        );
    }
    return ast;
}

static AST *logic_and(Token *token_list, size_t *token_postition, AST *ast) 
{
    ast = equality(token_list, token_postition, ast);

    while(token_list[*token_postition].type == DOUBLE_AND) {
        Token *operator = &token_list[*token_postition];
        if(next_position(token_postition, token_list)) {
            parser_error(*operator, "Missing right operator!\n");
            panic_mode(token_list, token_postition);
            return ast; 
        }
        AST *right = equality(token_list, token_postition, ast);
        ast = ast_new((AST)
            {
                .tag = AST_LOGICAL_EXPR,
                .data.AST_LOGICAL_EXPR = {
                    ast,
                    operator,
                    right
                }
            }
        );               
    }
    return ast;
}

static AST *logic_or(Token *token_list, size_t *token_postition, AST *ast) 
{
    ast = logic_and(token_list, token_postition, ast);

    while(token_list[*token_postition].type == DOUBLE_OR) {
        Token *operator = &token_list[*token_postition];
        if(next_position(token_postition, token_list)) {
            parser_error(*operator, "Missing right operator!\n");
            panic_mode(token_list, token_postition);
            return ast; 
        }
        AST *right = logic_and(token_list, token_postition, ast);
        ast = ast_new((AST)
            {
                .tag = AST_LOGICAL_EXPR,
                .data.AST_LOGICAL_EXPR = {
                    ast,
                    operator,
                    right
                }
            }
        );       
    }
    return ast;
}

static AST *assignment(Token *token_list, size_t *token_position, AST *ast) 
{
    ast = logic_or(token_list, token_position, ast);

    if(token_list[*token_position].type == EQUAL) {
        Token *equals = &token_list[*token_position];

        if(next_position(token_position, token_list)) {
            parser_error(token_list[*token_position], "Expected expression after equals sign");
            panic_mode(token_list, token_position);
        }
        AST *value = assignment(token_list, token_position, ast);
        
        if(ast->tag == AST_IDENTIFIER) {
            Token *name = ast->data.token;
            ast_free(ast); 
            ast = ast_new((AST)
                {  
                    .tag = AST_ASSIGN_EXPR,
                    .data.AST_ASSIGN_EXPR = {
                        name,
                        value
                    }
                }            
            );

            return ast;
        }
        
        parser_error(token_list[*token_position], "Invalid assignment target");
        panic_mode(token_list, token_position);

    }
    return ast;    
}

static AST *expression(Token *token_list, size_t *token_position, AST *ast) 
{
    return assignment(token_list, token_position, ast);   
}

static AST *expression_statement(Token *token_list, size_t *token_position, AST *ast) 
{
    ast = expression(token_list, token_position, ast);
    ast = ast_new((AST)
        {
            .tag = AST_EXPR_STMT,
            .data.AST_EXPR_STMT = {
                ast,
            }
        }            
    );

    if(token_list[*token_position].type != SEMICOLON) {
        fprintf(stderr, "Expected ';' at the end of the expression.\n");
        set_error_flag();
        panic_mode(token_list, token_position);
    }
    return ast;
}

static AST *echo_statement(Token *token_list, size_t *token_position, AST *ast) 
{
    ast = expression(token_list, token_position, ast);
    ast = ast_new((AST)
        {
            .tag = AST_ECHO_STMT,
            .data.AST_ECHO_STMT = {
                ast,
            }
        }            
    );

    if(token_list[*token_position].type != SEMICOLON) {
        fprintf(stderr, "Expected ';' at the end of the echo expression.\n");
        set_error_flag();
        panic_mode(token_list, token_position);
    }
    return ast;
}

static AST *block_statement(Token *token_list, size_t *token_position, AST *ast) 
{
    ast = ast_new((AST)
        {
            .tag = AST_BLOCK_STMT,
            .data.AST_BLOCK_STMT = {
                NULL,   // initialize to no arrays (block could be just {} )
                0       // initialize to 0 since there are no arrays 
            }
        }
    );
    AST **_stmt_list = ast->data.AST_BLOCK_STMT.stmt_list;
    while(token_list[*token_position].type != RIGHT_BRACE && token_list[(*token_position) + 1].type != EOF_TOKEN) {
        _stmt_list = realloc(_stmt_list, sizeof(AST *) * (ast->data.AST_BLOCK_STMT.stmt_num + 1));
        _stmt_list[ast->data.AST_BLOCK_STMT.stmt_num] = declaration(token_list, token_position, ast);

        if(next_position(token_position, token_list)) {
            TODO("Fix panic mode!");
            parser_error(token_list[*token_position], "Expected '}' after block statement.");
            panic_mode(token_list, token_position);
        }
        ast->data.AST_BLOCK_STMT.stmt_num++;
    } 
    ast->data.AST_BLOCK_STMT.stmt_list = _stmt_list;
    
    if(token_list[*token_position].type != RIGHT_BRACE) {
        parser_error(token_list[*token_position], "Expected '}' after block statement.");
        panic_mode(token_list, token_position);
    }

    return ast;
}

static AST *if_statement(Token *token_list, size_t *token_position, AST *ast) 
{
    if(token_list[*token_position].type != LEFT_PARENTHESIS) {
        TODO("Fix errors");
        parser_error(token_list[*token_position], "Expected ( after if statement!");
        return ast;
    }

    next_position(token_position, token_list);
    AST *condition = expression(token_list, token_position, ast);
    if(token_list[*token_position].type != RIGHT_PARENTHESIS) {
        TODO("Fix errors");
        parser_error(token_list[*token_position], "Expected ) after if statement!");
        return ast;
    }
    next_position(token_position, token_list);
    AST *true_branch = statement(token_list, token_position, ast);
    AST *else_branch = NULL;
    if(token_list[*token_position + 1].type == ELSE && !next_position(token_position, token_list)) {
        next_position(token_position, token_list);
        else_branch = statement(token_list, token_position, ast);
    }
    ast = ast_new((AST)
        {
            .tag = AST_IF_STMT,
            .data.AST_IF_STMT = {
                condition,
                true_branch,
                else_branch
            }
        }            
    );  
    return ast; 
}

static AST *while_statement(Token *token_list, size_t *token_position, AST *ast) 
{
    if(token_list[*token_position].type != LEFT_PARENTHESIS) {
        TODO("Fix errors");
        parser_error(token_list[*token_position], "Expected ( after while statement!");
        return ast;
    }

    next_position(token_position, token_list);
    AST *condition = expression(token_list, token_position, ast);
    if(token_list[*token_position].type != RIGHT_PARENTHESIS) {
        TODO("Fix errors");
        parser_error(token_list[*token_position], "Expected ) after while statement!");
        return ast;
    }
    next_position(token_position, token_list);
    AST *body = statement(token_list, token_position, ast);

    ast = ast_new((AST)
        {
            .tag = AST_WHILE_STMT,
            .data.AST_WHILE_STMT = {
                condition,
                body
            }
        }            
    );
    return ast; 
}

static AST *for_statement(Token *token_list, size_t *token_position, AST *ast) 
{
    if(token_list[*token_position].type != LEFT_PARENTHESIS) {
        TODO("Fix errors");
        parser_error(token_list[*token_position], "Expected ( after while statement!");
        return ast;
    }

    if(next_position(token_position, token_list)) {
        TODO("Fix errors");
        parser_error(token_list[*token_position], "Expected expression in initializer part of for statement!");
        return ast;
    }

    AST *initializer;
    
    if(token_list[*token_position].type == SEMICOLON)  {
        initializer = NULL;
    } else if(token_list[*token_position].type == VAR) {
        if(next_position(token_position, token_list)) {TODO("FIX ERROR!");}
	    initializer = variable_declaration(token_list, token_position, initializer);
    } else {
	    initializer = expression_statement(token_list, token_position, initializer);
    } 

    if(token_list[*token_position].type != SEMICOLON) {
        TODO("Fix errors");
        parser_error(token_list[*token_position], "Expected ; after initializerializer in for statement!");
        return ast;
    }

    if(next_position(token_position, token_list)) {
        TODO("Fix errors");
        parser_error(token_list[*token_position], "Expected expression in initializerializer part of for statement!");
        return ast;
    }

    AST *condition = NULL;
    if(token_list[*token_position].type != SEMICOLON) {
	    condition = expression(token_list, token_position, condition);
    }

    if(token_list[*token_position].type != SEMICOLON) {
        TODO("Fix errors");
        parser_error(token_list[*token_position], "Expected ( after while statement!");
        return ast;
    }

    if(next_position(token_position, token_list)) {
        TODO("Fix errors");
        parser_error(token_list[*token_position], "Expected expression in initializer part of for statement!");
        return ast;
    }

    AST *increment = NULL;
    if(token_list[*token_position].type != RIGHT_PARENTHESIS) {
	    increment = expression(token_list, token_position, ast);
    }

    if(token_list[*token_position].type != RIGHT_PARENTHESIS) {
        TODO("Fix errors");
        parser_error(token_list[*token_position], "Expected closing ) in for statement!");
        return ast;
    }
   
    next_position(token_position, token_list);
    AST *body = statement(token_list, token_position, ast);

    ast = ast_new((AST) 
        {
            .tag = AST_FOR_STMT,
            .data.AST_FOR_STMT = {
                initializer,
                condition,
                increment,
        		body
            }
        }            
    );
    return ast; 
}

static AST *return_statement(Token *token_list, size_t *token_position, AST *ast) 
{
    AST *expr = NULL;
    if(token_list[*token_position].type != SEMICOLON) {
        expr = expression(token_list, token_position, ast);
    }

    if(token_list[*token_position].type != SEMICOLON) {
        parser_error(token_list[*token_position], "Expected ; after return expression.");
        return ast;
    }

     ast = ast_new((AST)
            {
                .tag = AST_RETURN_STMT,
                .data.AST_RETURN_STMT = {
                    expr
                }
            }            
        );
     
     return ast;
}
 
static AST *time_statement(Token *token_list, size_t *token_position, AST *ast)
{
    ast = ast_new((AST) 
        {
            .tag = AST_TIME_STMT,
            .data.token = NULL
        }
    );

    if(token_list[*token_position].type != SEMICOLON) {
        fprintf(stderr, "Expected ';' at the end of time statment.\n");
        set_error_flag();
        panic_mode(token_list, token_position);
    }       
    return ast;
}

static AST *clear_statement(Token *token_list, size_t *token_position, AST *ast)
{
    ast = ast_new((AST) 
        {
            .tag = AST_CLEAR_STMT,
            .data.token = NULL
        }
    );

    if(token_list[*token_position].type != SEMICOLON) {
        fprintf(stderr, "Expected ';' at the end of clear statment.\n");
        set_error_flag();
        panic_mode(token_list, token_position);
    }       
    return ast;
}

static AST *cd_statement(Token *token_list, size_t *token_position, AST *ast)
{
    if(token_list[*token_position].type != SEMICOLON) 
        ast = expression(token_list, token_position, ast);
    if(ast != NULL && ast->tag != AST_LITERAL) {
        fprintf(stderr, "Expected \"path\" after cd statement.\n");
        set_error_flag();
        panic_mode(token_list, token_position);
    }       
 
    ast = ast_new((AST)
        {
            .tag = AST_CD_STMT,
            .data.AST_CD_STMT = {
                ast,
            }
        }            
    );

    if(token_list[*token_position].type != SEMICOLON) {
        fprintf(stderr, "Expected ';' at the end of the echo expression.\n");
        set_error_flag();
        panic_mode(token_list, token_position);
    }
    return ast;
}

static AST *run_statement(Token *token_list, size_t *token_position, AST *ast)
{
    Token *program_name = NULL;
    AST **args_list = NULL;
    AST *expr = NULL;
    size_t arg_num = 0;
    
    if(token_list[*token_position].type != SEMICOLON) {
        program_name = &token_list[*token_position];
        if(next_position(token_position, token_list)) {TODO("Add error later");}
        while(token_list[*token_position].type != SEMICOLON) {
            args_list = realloc(args_list, sizeof(AST *) * (arg_num + 1));
            args_list[arg_num++] = expression(token_list, token_position, expr);
            if(arg_num >= MAX_ARG_CNT) {TODO("Add error later!");}
        }
    }
    if(token_list[*token_position].type != SEMICOLON) {
        fprintf(stderr, "Expected ';' at the end of the echo expression.\n");
        set_error_flag();
        panic_mode(token_list, token_position);
    }

    ast = ast_new((AST)
        {
            .tag = AST_RUN_STMT,
            .data.AST_RUN_STMT = {
                program_name,
                args_list,
                arg_num
            }
        }
    );
    return ast;
}

static AST *variable_declaration(Token *token_list, size_t *token_position, AST *ast)
{
    if(token_list[*token_position].type != IDENTIFIER) {
        parser_error(token_list[*token_position], "Expected Identifier after var.");
        return ast;
    }

    Token *name = &token_list[*token_position];
    AST *initializer = NULL;
    if(next_position(token_position,token_list))
        parser_error(token_list[*token_position], "Unexpected EOF after var.");

    if(token_list[*token_position].type == EQUAL) {
        if(next_position(token_position,token_list))
            parser_error(token_list[*token_position], "Unexpected EOF after initialization of variable.");
        initializer = expression(token_list, token_position, ast);
    }
    ast = ast_new((AST)
        {
            .tag = AST_VAR_DECL_STMT,
            .data.AST_VAR_DECL_STMT = {
                name,
                initializer
            }
        }            
    );

    if(token_list[*token_position].type != SEMICOLON) {
        fprintf(stderr, "Expected ';' at the end of the expression.\n");
        set_error_flag();
        panic_mode(token_list, token_position);
    }       
    return ast;
}

static AST *funct_declaration(Token *token_list, size_t *token_position, AST *ast)
{
    if(token_list[*token_position].type != IDENTIFIER) {
        TODO("Handle the error!");
    } 

    size_t param_num = 0;
    size_t stmt_num = 0;
    AST **params = NULL;
    AST **stmt_list = NULL;
    Token *name = &token_list[*token_position];
    
    if(next_position(token_position, token_list) || token_list[*token_position].type != LEFT_PARENTHESIS) {
        TODO("Handle the error!");
    } 

    if(next_position(token_position, token_list)) {
        TODO("Handle the error!");
    } 

    if(token_list[*token_position].type != RIGHT_PARENTHESIS) {
        do {
            params = realloc(params, sizeof(AST *) * (param_num + 1));
            params[param_num++] = expression(token_list, token_position, ast);
            if(param_num >= MAX_ARG_CNT) {TODO("Add error later!");}
        } while(token_list[*token_position].type == COMMA && !next_position(token_position, token_list));
        
        if(token_list[*token_position].type != RIGHT_PARENTHESIS) {TODO("Add error");}
    }

    if(next_position(token_position, token_list) || token_list[*token_position].type != LEFT_BRACE) {
        TODO("Handle the error!");
    }

    if(next_position(token_position, token_list)) {
        TODO("Handle the error!");
    } 

    while(token_list[*token_position].type != RIGHT_BRACE && token_list[(*token_position) + 1].type != EOF_TOKEN) {
        stmt_list = realloc(stmt_list, sizeof(AST *) * (stmt_num + 1));
        stmt_list[stmt_num++] = declaration(token_list, token_position, ast);
        if(next_position(token_position, token_list)) {
            TODO("Fix panic mode!");
            parser_error(token_list[*token_position], "Expected '}' after block statement.");
            panic_mode(token_list, token_position);
        }
    } 
    
    if(token_list[*token_position].type != RIGHT_BRACE) {
        parser_error(token_list[*token_position], "Expected '}' after block statement.");
        panic_mode(token_list, token_position);
    }
   
    ast = ast_new((AST)
        {
            .tag = AST_FUNCT_DECL_STMT,
            .data.AST_FUNCT_DECL_STMT = {
                name,
                params,
                stmt_list,
                param_num,
                stmt_num
            }
        }            
    );

    return ast;
}

static AST *statement(Token *token_list, size_t *token_position, AST *ast) 
{
    /* print statement rule */
    if(token_list[*token_position].type == ECHO) {
        if(next_position(token_position, token_list)) {
            parser_error(token_list[*token_position], "Expected expression after echo!");
            return ast;
        }
        return echo_statement(token_list, token_position, ast);
    }

    /* Block statement rule */
    if(token_list[*token_position].type == LEFT_BRACE) {
        if(next_position(token_position, token_list)) {
            parser_error(token_list[*token_position], "Expected expression or '}' after opening block statement!");
            return ast;
        }
        return block_statement(token_list, token_position, ast);       
    }

    /* If statement rule */
    if(token_list[*token_position].type == IF) {
        if(next_position(token_position, token_list)) {
            parser_error(token_list[*token_position], "Expected '(' condition ')' after if statement!");
            return ast;
        }
        return if_statement(token_list, token_position, ast);       
    }
    /* While statement rule */
    if(token_list[*token_position].type == WHILE) {
        if(next_position(token_position, token_list)) {
            parser_error(token_list[*token_position], "Expected '(' condition ')' after if statement!");
            return ast;
        }
        return while_statement(token_list, token_position, ast);       
    }
    /* For statement rule */
    if(token_list[*token_position].type == FOR) {
        if(next_position(token_position, token_list)) {
            parser_error(token_list[*token_position], "Expected '(' after for statement!");
            return ast;
        }
        return for_statement(token_list, token_position, ast);       
    }
    /* return statement rule */
    if(token_list[*token_position].type == RETURN) {
        if(next_position(token_position, token_list)) {
            parser_error(token_list[*token_position], "Expected expression or ';'  after return statement!");
            return ast;
        }
        return return_statement(token_list, token_position, ast);       
    }
    /* time statement rule */
    if(token_list[*token_position].type == TIME) {
        if(next_position(token_position, token_list)) {
            parser_error(token_list[*token_position], "Expected ';'  after time statement!");
            return ast;
        }
        return time_statement(token_list, token_position, ast);       
    }

    /* clear statement rule */
    if(token_list[*token_position].type == CLEAR) {
        if(next_position(token_position, token_list)) {
            parser_error(token_list[*token_position], "Expected ';'  after clear statement!");
            return ast;
        }
        return clear_statement(token_list, token_position, ast);       
    }

    /* cd statement rule */
    if(token_list[*token_position].type == CD) {
        if(next_position(token_position, token_list)) {
            parser_error(token_list[*token_position], "Expected ';'  after cd statement!");
            return ast;
        }
        return cd_statement(token_list, token_position, ast);       
    }

    /* run statement rule */
    if(token_list[*token_position].type == RUN) {
        if(next_position(token_position, token_list)) {
            parser_error(token_list[*token_position], "Expected ';'  after run statement!");
            return ast;
        }
        return run_statement(token_list, token_position, ast);       
    }

    /* Regular expression statement */
    return expression_statement(token_list, token_position, ast);
}

static AST *declaration(Token *token_list, size_t *token_position, AST *ast) 
{
    if(!setjmp(sync_env))
        fprintf(stdout, "Setjmp for parser!\n");

    if(token_list[*token_position].type == EOF_TOKEN)
        return ast;
    
    if(token_list[*token_position].type == VAR) {
        if(next_position(token_position, token_list)) {
            parser_error(token_list[*token_position], "Expected expression after var.");
            return ast;
        }
        return variable_declaration(token_list, token_position, ast);
    }
    
    if(token_list[*token_position].type == FUNCT) {
        if(next_position(token_position, token_list)) {
            parser_error(token_list[*token_position], "Expected function identifier after funct.");
            return ast;
        }
        return funct_declaration(token_list, token_position, ast);
    }

    return statement(token_list, token_position, ast);
}

extern AST **parser(Token *token_list, size_t *statement_number) 
{
    size_t token_position = 0;
    size_t num_of_stmt = 0;
    AST **ast = NULL;
    *statement_number = 0;

    do {
        ast = realloc(ast, sizeof(AST *)*(num_of_stmt+1));
        ast[num_of_stmt] = (ast[num_of_stmt] = NULL, declaration(token_list, &token_position, ast[num_of_stmt]));
        if(ast[num_of_stmt] != NULL) {
            num_of_stmt++;
        }
        printf("Number of statements parsed: %d\n\n", num_of_stmt);
    } while(!next_position(&token_position, token_list));
    
    *statement_number = num_of_stmt;
    return ast;
}
