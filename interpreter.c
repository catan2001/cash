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
#include <string.h>
#include "coretypes.h"
#include "error.h"
#include "environment.h"
#include "interpreter.h"

static jmp_buf sync_env;

static ValueTagged *evaluate(AST *, EnvironmentMap *);

static void runtime_error_mode(void)
{
    longjmp(sync_env, TRUE);
}

static void eval_print(ValueTagged *result)
{ 
    if(result == NULL) return;
    switch (result->type)
    {
    case NUMBER_INT:
        fprintf(stdout, "Value INT = %d\n", result->literal.integer_value);
        break;
    case NUMBER_FLOAT:
        fprintf(stdout, "Value FLOAT = %lf\n", result->literal.float_value);
        break;
    case STRING:
        fprintf(stdout, "Value STRING = %s\n", result->literal.char_value);
        break;
    case TRUE_TOKEN:
    case FALSE_TOKEN:
        fprintf(stdout, "Value BOOLEAN = %d\n", result->literal.boolean_value);
        break;
   default:
        error("Tried to print undefined ValueTagged value in eval_print", __FILE__, __LINE__);
        break;
    }
}

static ValueTagged *literal_value(AST *node)
{
    if(node->tag != AST_LITERAL) 
    {
        INTERNAL_ERROR("Tried to return non-literal node.");
        abort();
    }
    ValueTagged *result = (ValueTagged *)malloc(sizeof(ValueTagged));
    return (result->literal = node->data.token->literal, result->type = node->data.token->type, result);
}

static ValueTagged *identifier_value(AST *node, EnvironmentMap *env_host)
{
    if(node->tag != AST_IDENTIFIER)
    {
        INTERNAL_ERROR("Tried to return non-identifier node.");
        abort();
    }
    ValueTagged *found = env_get_var(node->data.token, env_host);
    if(found == NULL) return NULL;
    ValueTagged *result = (ValueTagged *)malloc(sizeof(ValueTagged));
    return (result->literal = found->literal, result->type = found->type, result);
}

static Value is_truth(ValueTagged *value, TokenType type)
{
    Value ret;
    if(type == NUMBER_INT) 
        return (ret.boolean_value = (value->literal.integer_value) ? TRUE : FALSE, ret);
    if(type == NUMBER_FLOAT) 
        return (ret.boolean_value = (value->literal.float_value) ? TRUE : FALSE, ret);
    if(value->literal.char_value == NULL) 
        return (ret.boolean_value = FALSE, ret);

    return (ret.boolean_value = TRUE, ret);
}

static ValueTagged *evaluate_unary_expression(AST *node, EnvironmentMap *env_host)
{
    ValueTagged *right = evaluate(node->data.AST_UNARY_EXPR.right, env_host);
    ValueTagged *result = (ValueTagged *)malloc(sizeof(ValueTagged));
    TokenType operator_type = node->data.AST_UNARY_EXPR.token->type;
    result->type = right->type;

    switch(operator_type)
    {
        case SUBTRACT:
        {
            if(right->type == STRING) 
            {
                runtime_error(node->data.AST_UNARY_EXPR.right, "Can't do unary subtract operation on strings!");
                break;
            }

            if(right->type == NUMBER_FLOAT)
                result->literal.float_value = (-1)*right->literal.float_value;
            if(right->type == NUMBER_INT)
                result->literal.integer_value = (-1)*(right->literal.integer_value); 
            if(right->type == TRUE_TOKEN || right->type == FALSE_TOKEN)
            {
                result->literal.integer_value = (-1)*right->literal.boolean_value;
                result->type = NUMBER_INT;
            }
            free(right);
            return result;                
        }
        case XOR:
        {
            if(right->type == STRING) 
            {
                runtime_error(node->data.AST_UNARY_EXPR.right, "Can't do unary XOR operation on strings!");
                break;
            }
            if(right->type == NUMBER_FLOAT) 
            {
                runtime_error(node->data.AST_UNARY_EXPR.right, "Can't do unary subtract operation on float!");
                break;
            }
            if(right->type == NUMBER_INT)
                result->literal.integer_value = ~right->literal.integer_value; 
            if(right->type == TRUE_TOKEN || right->type == FALSE_TOKEN)
            {
                result->literal.integer_value = ~right->literal.boolean_value;
                result->type = NUMBER_INT;
            }
            free(right);
            return result;
        }
        case EXCLAMATION:
        {
            result->literal.boolean_value = !is_truth(right, right->type).boolean_value;
            free(right);
            return (result->type = (result->literal.boolean_value) ? TRUE_TOKEN : FALSE_TOKEN, result);
        }
        default:
            error("Unallowed operator on unary expression!", __FILE__, __LINE__); 
            break;
    }

    free(result);
    free(right);
    runtime_error_mode();
    abort();
}

static ValueTagged *evaluate_binary_expression(AST *node, EnvironmentMap *env_host)
{
    ValueTagged *left = evaluate(node->data.AST_BINARY_EXPR.left, env_host);
    ValueTagged *right = evaluate(node->data.AST_BINARY_EXPR.right, env_host);
    ValueTagged *result = (ValueTagged *)malloc(sizeof(ValueTagged));
    TokenType operator_type = node->data.AST_BINARY_EXPR.token->type;

    if((left->type == STRING || right->type == STRING) && operator_type != ADD){
        (left->type == STRING) ? runtime_error(node->data.AST_BINARY_EXPR.left, "Binary operator is not allowed on strings!")
                               : runtime_error(node->data.AST_BINARY_EXPR.right, "Binary operator is not allowed on strings!");
    }
    else
        switch(operator_type)
        {
            case EXCLAMATION_EQUEAL:
                BINARY_COMPARISON_OPERATION(!=, left, right, result);
                return (free(left), free(right), result);
            case DOUBLE_EQUAL:
                BINARY_COMPARISON_OPERATION(==, left, right, result); 
                return (free(left), free(right), result);
            case REDIRECTION_RIGHT_GREATER_RELATIONAL:
                BINARY_COMPARISON_OPERATION(>, left, right, result); 
                return (free(left), free(right), result);
            case REDIRECTION_LEFT_LESS_RELATIONAL:
                BINARY_COMPARISON_OPERATION(<, left, right, result);
                return (free(left), free(right), result);
            case GREATER_EQUAL:
                BINARY_COMPARISON_OPERATION(>=, left, right, result);
                return (free(left), free(right), result);
            case LESS_EQUAL:
                BINARY_COMPARISON_OPERATION(<=, left, right, result);
                return (free(left), free(right), result);
            case SUBTRACT:
                BINARY_ADD_SUB_MULTIPLY_OPERATION(-, left, right, result);
                return (free(left), free(right), result);          
            case MULTIPLY:
                BINARY_ADD_SUB_MULTIPLY_OPERATION(*, left, right, result);
                return (free(left), free(right), result);
            case DIVIDE:
                BINARY_DIVIDE_OPERATION(/, left, right, result);
                return (free(left), free(right), result);
            case ADD:
            {
                if(left->type == STRING && right->type == STRING) {
                    result->literal.char_value = strcat(left->literal.char_value, right->literal.char_value);
                    result->type = STRING;
                }
                else if((left->type == STRING || right->type == STRING) && (left->type == NUMBER_INT || right->type == NUMBER_INT))
                    TODO("Add support for STRING + INT = STRING");
                else if((left->type == STRING || right->type == STRING) && (left->type == NUMBER_FLOAT || right->type == NUMBER_FLOAT))
                    TODO("Add support for STRING + FLOAT = STRING");
                else if((left->type == STRING || right->type == STRING) && (left->type == TRUE || left->type == FALSE || right->type == FALSE || right->type == TRUE))
                    TODO("Add support for STRING + BOOLEAN = STRING");
                else if(left->type != STRING && right->type != STRING)
                    BINARY_ADD_SUB_MULTIPLY_OPERATION(+, left, right, result);
                else
                    break;

                return (free(left), free(right), result);
            }
            default:
                break;
        }
    free(left);
    free(right);
    free(result);
    runtime_error_mode();
    abort();
} 

static ValueTagged *evaulate_grouping_expression(AST *node, EnvironmentMap *env_host)
{
    return evaluate(node->data.AST_GROUPING_EXPR.left, env_host);
}

static ValueTagged *evaluate_logical_expression(AST *node, EnvironmentMap *env_host)
{
    ValueTagged *left = evaluate(node->data.AST_LOGICAL_EXPR.left, env_host);

    if(node->data.AST_LOGICAL_EXPR.token->type == DOUBLE_OR) {
        if(is_truth(left, left->type).boolean_value) return left;
    }
    else {
        if(!is_truth(left, left->type).boolean_value) return left;
    }

    return evaluate(node->data.AST_LOGICAL_EXPR.right, env_host);
}

static ValueTagged *evaluate_assign_expression(AST *node, EnvironmentMap *env_host)
{
    ValueTagged *value = evaluate(node->data.AST_ASSIGN_EXPR.expr, env_host);
    Token *name = node->data.AST_ASSIGN_EXPR.token;
    
    env_assign_var(name, value, env_host);
    return value;
}

static ValueTagged *evaluate_if_statement(AST *node, EnvironmentMap *env_host)
{
    ValueTagged *condition = evaluate(node->data.AST_IF_STMT.condition, env_host);
    if(is_truth(condition, condition->type).boolean_value)
        evaluate(node->data.AST_IF_STMT.true_branch, env_host);
    else
        evaluate(node->data.AST_IF_STMT.else_branch, env_host);
    
    free(condition);
    return NULL;
}

static ValueTagged *evaluate_while_statement(AST *node, EnvironmentMap *env_host)
{
    ValueTagged *condition = evaluate(node->data.AST_WHILE_STMT.condition, env_host);
    while(is_truth(condition, condition->type).boolean_value) {
        evaluate(node->data.AST_WHILE_STMT.body, env_host);
	condition = evaluate(node->data.AST_WHILE_STMT.condition, env_host); 
    }
    free(condition);
    return NULL;
}

static ValueTagged *evaluate_block_statement(AST *node, EnvironmentMap *env_parrent, EnvironmentMap *env_host)
{
    env_host->env_enclosing = env_parrent;

    for(size_t i = 0; i < node->data.AST_BLOCK_STMT.stmt_num; ++i)
    {
        ValueTagged *result = evaluate(node->data.AST_BLOCK_STMT.stmt_list[i], env_host);
    }
    /* Free memory of Local Environment */
    env_reset(env_host);
    return NULL;
}

static ValueTagged *evaluate_variable_statement(AST *node, EnvironmentMap *env_host)
{   
    Token *name = node->data.AST_VAR_DECL_STMT.name;
    ValueTagged *value = NULL;
    if(node->data.AST_VAR_DECL_STMT.init != NULL) 
        value = evaluate(node->data.AST_VAR_DECL_STMT.init, env_host);

    env_define_var(name, value, env_host);
    return value;            
}

static ValueTagged * _printf(ValueTagged *result)
{    
    switch (result->type)
    {
        case NUMBER_INT:
            fprintf(stdout, "%d\n", result->literal.integer_value);
            break;
        case NUMBER_FLOAT:
            fprintf(stdout, "%lf\n", result->literal.float_value);
            break;
        case STRING:
            fprintf(stdout, "%s\n", result->literal.char_value);
            break;
        case TRUE_TOKEN:
        case FALSE_TOKEN:
            fprintf(stdout, "%d\n", result->literal.boolean_value);
            break;
       default:
            error("Tried to print undefined undefined ValueTagged value in eval_print", __FILE__, __LINE__);
            break;
    }
    return result;
}

static ValueTagged *evaluate(AST *node, EnvironmentMap *env_host)
{   
    switch (node->tag)
    {
    case AST_LITERAL:
        return literal_value(node);
    case AST_IDENTIFIER:
        return identifier_value(node, env_host);    
    case AST_UNARY_EXPR:
        return evaluate_unary_expression(node, env_host);
    case AST_BINARY_EXPR:
        return evaluate_binary_expression(node, env_host);
    case AST_GROUPING_EXPR:
        return evaulate_grouping_expression(node, env_host);
    case AST_LOGICAL_EXPR:
        return evaluate_logical_expression(node, env_host);
    case AST_ASSIGN_EXPR:
        return evaluate_assign_expression(node, env_host);
    case AST_EXPR_STMT:
        return evaluate(node->data.AST_EXPR_STMT.expr, env_host);
    case AST_BLOCK_STMT:
        EnvironmentMap env_child = {NULL, NULL, 0};
        return evaluate_block_statement(node, env_host, &env_child);
    case AST_IF_STMT:
        return evaluate_if_statement(node, env_host);
    case AST_WHILE_STMT:
	return evaluate_while_statement(node, env_host);	
    case AST_PRINT_STMT:
        ValueTagged *result = evaluate(node->data.AST_PRINT_STMT.expr, env_host);
        return ((ValueTagged *)_printf(result));
    case AST_VAR_DECL_STMT:
        return evaluate_variable_statement(node, env_host);
    default:
        break;
    }
    error("Tried to evaluate undefined AST node type.", __FILE__, __LINE__);
    return NULL;
}

extern void interpret(AST *expr)
{
    if(setjmp(sync_env)) return;
    else
        fprintf(stdout, "Setjmp for interpreter!\n");

    ValueTagged *value = evaluate(expr, &env_global);
    //eval_print(value);
}


