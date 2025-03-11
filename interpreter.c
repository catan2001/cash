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
#include "function.h"
#include "interpreter.h"

static jmp_buf sync_env;

static ValueTagged *evaluate(AST *, EnvironmentMap *);

static void runtime_error_mode(void) 
{
    error_flag = TRUE;
    longjmp(sync_env, TRUE);
}

static void eval_print(ValueTagged *result) 
{ 
    if(result == NULL) return;
    switch (result->type) {
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

static void free_value(ValueTagged *value)
{   
    if(value == NULL) return;
    if(value->type == STRING) {
        free(value->literal.char_value);
    }
    free(value);
}

extern void function_interpret(Token *callee, ValueTagged **args, const size_t arg_num, EnvironmentMap *env_parrent) 
{
    if(callee == NULL) INTERNAL_ERROR("Passed null callee argument");
   
    EnvironmentMap env_child = {NULL, env_parrent, 0} ;
    Environment *function = env_get_function(callee, env_parrent);
    AST **parameters = function->data.ENV_FUNCTION.definition->data.AST_FUNCT_DECL_STMT.parameters;
    AST **stmt_list = function->data.ENV_FUNCTION.definition->data.AST_FUNCT_DECL_STMT.stmt_list;
    size_t param_num = function->data.ENV_FUNCTION.definition->data.AST_FUNCT_DECL_STMT.param_num;
    size_t stmt_num =  function->data.ENV_FUNCTION.definition->data.AST_FUNCT_DECL_STMT.stmt_num;
    
    if(arg_num != param_num) {
        fprintf(stderr, "Error when calling %s, number of arguments given %d but expected %d\n", callee->lexeme, arg_num, param_num);
        runtime_error_mode();
    }

    for(size_t i = 0; i < param_num; ++i) {
        Token *name = parameters[i]->data.token;
        env_define_var(name, args[i], &env_child);
    }
    
    for(size_t i = 0; i < stmt_num; ++i) {
        free_value(evaluate(stmt_list[i], &env_child));
    }

    env_reset(&env_child);
}

static ValueTagged *literal_value(AST *node) 
{
    if(node->tag != AST_LITERAL) {
        INTERNAL_ERROR("Tried to return non-literal node.");
        abort();
    }
    ValueTagged *result = (ValueTagged *)malloc(sizeof(ValueTagged));
    result->type = node->data.token->type;
    if(node->data.token->type == STRING) 
        return(result->literal.char_value = strdup(node->data.token->literal.char_value), result);
    return (result->literal = node->data.token->literal, result);
}

static ValueTagged *identifier_value(AST *node, EnvironmentMap *env_host) 
{
    if(node->tag != AST_IDENTIFIER) {
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

    switch(operator_type) {
        case SUBTRACT:
        {
            if(right->type == STRING) {
                runtime_error(node->data.AST_UNARY_EXPR.right, "Can't do unary subtract operation on strings!");
                break;
            }

            if(right->type == NUMBER_FLOAT)
                result->literal.float_value = (-1)*right->literal.float_value;
            if(right->type == NUMBER_INT)
                result->literal.integer_value = (-1)*(right->literal.integer_value); 
            if(right->type == TRUE_TOKEN || right->type == FALSE_TOKEN) {
                result->literal.integer_value = (-1)*right->literal.boolean_value;
                result->type = NUMBER_INT;
            }
            free_value(right);
            return result;                
        }
        case XOR:
        {
            if(right->type == STRING) {
                runtime_error(node->data.AST_UNARY_EXPR.right, "Can't do unary XOR operation on strings!");
                break;
            }
            if(right->type == NUMBER_FLOAT) {
                runtime_error(node->data.AST_UNARY_EXPR.right, "Can't do unary subtract operation on float!");
                break;
            }
            if(right->type == NUMBER_INT)
                result->literal.integer_value = ~right->literal.integer_value; 
            if(right->type == TRUE_TOKEN || right->type == FALSE_TOKEN) {
                result->literal.integer_value = ~right->literal.boolean_value;
                result->type = NUMBER_INT;
            }
            free_value(right);
            return result;
        }
        case EXCLAMATION:
        {
            result->literal.boolean_value = !is_truth(right, right->type).boolean_value;
            free_value(right);
            return (result->type = (result->literal.boolean_value) ? TRUE_TOKEN : FALSE_TOKEN, result);
        }
        default:
            error("Unallowed operator on unary expression!", __FILE__, __LINE__); 
            break;
    }

    free_value(result);
    free_value(right);
    runtime_error_mode();
    abort();
}

static ValueTagged *evaluate_call_expression(AST *node, EnvironmentMap *env_host)
{
    Token *callee = node->data.AST_CALL_EXPR.callee->data.token;
    ValueTagged **args = malloc(sizeof(ValueTagged *) * node->data.AST_CALL_EXPR.stmt_num);

    for(size_t i = 0; i < node->data.AST_CALL_EXPR.stmt_num; ++i) {
        args[i] = evaluate(node->data.AST_CALL_EXPR.stmt_list[i], env_host);
    }

    function_interpret(callee, args, node->data.AST_CALL_EXPR.stmt_num, env_host);

    for(size_t i = 0; i < node->data.AST_CALL_EXPR.stmt_num; ++i) {
        free_value(args[i]);
    }
    free(args);
    
    return NULL;
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

                return (free_value(left), free_value(right), result);
            }
            default:
                break;
        }
    free_value(left);
    free_value(right);
    free_value(result);
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
    return (free_value(value), NULL);
}

static ValueTagged *evaluate_if_statement(AST *node, EnvironmentMap *env_host) 
{
    ValueTagged *condition = evaluate(node->data.AST_IF_STMT.condition, env_host);

    if(is_truth(condition, condition->type).boolean_value) 
        free_value(evaluate(node->data.AST_IF_STMT.true_branch, env_host)); 
    else if(node->data.AST_IF_STMT.else_branch != NULL) 
        free_value(evaluate(node->data.AST_IF_STMT.else_branch, env_host));

    return (free_value(condition), NULL);
}

static ValueTagged *evaluate_while_statement(AST *node, EnvironmentMap *env_host) 
{
    ValueTagged *condition = evaluate(node->data.AST_WHILE_STMT.condition, env_host);
    while(is_truth(condition, condition->type).boolean_value) {
        free_value(evaluate(node->data.AST_WHILE_STMT.body, env_host));
        free_value(condition);
    	condition = evaluate(node->data.AST_WHILE_STMT.condition, env_host); 
    }
    return (free_value(condition), NULL);
}

static ValueTagged *evaluate_for_statement(AST *node, EnvironmentMap *env_host) 
{
    AST *init_node = node->data.AST_FOR_STMT.initializer;
    AST *cond_node = node->data.AST_FOR_STMT.condition;

    EnvironmentMap env_child = {NULL, env_host, 0};
    ValueTagged *initializer = (init_node == NULL) ? NULL : evaluate(init_node, &env_child);
    ValueTagged *condition = (cond_node == NULL) ? NULL : evaluate(cond_node, &env_child);
    ValueTagged *increment = NULL;
    
    while(is_truth(condition, condition->type).boolean_value) {
        free_value(condition);
        free_value(increment);
        free_value(evaluate(node->data.AST_FOR_STMT.body, &env_child));
        increment = evaluate(node->data.AST_FOR_STMT.increment, &env_child);
	    condition = evaluate(node->data.AST_FOR_STMT.condition, &env_child); 
    }
    
    env_reset(&env_child);
    free_value(initializer);
    free_value(condition);
    free_value(increment);
    return NULL;
}

static ValueTagged *evaluate_block_statement(AST *node, EnvironmentMap *env_parrent, EnvironmentMap *env_host) 
{
    env_host->env_enclosing = env_parrent;

    for(size_t i = 0; i < node->data.AST_BLOCK_STMT.stmt_num; ++i)
    {
        free_value(evaluate(node->data.AST_BLOCK_STMT.stmt_list[i], env_host));
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
    return (free_value(value), NULL);
}

static ValueTagged *evaluate_function_declaration_statement(AST *node, EnvironmentMap *env_host)
{
    Token *name = node->data.AST_FUNCT_DECL_STMT.name;
    AST *function_definition = node;
    env_define_function(name, env_host, function_definition);
    return NULL;
}

static ValueTagged * _printf(ValueTagged *result) 
{   
    if(error_flag) return NULL;
    switch (result->type) {
        case NUMBER_INT:
            fprintf(stdout, "%d", result->literal.integer_value);
            break;
        case NUMBER_FLOAT:
            fprintf(stdout, "%lf", result->literal.float_value);
            break;
        case STRING:
            for(size_t i = 0; result->literal.char_value[i] != '\0'; ++i) {
                if(result->literal.char_value[i] == '\\' && result->literal.char_value[i+1] == 'n') {
                    fprintf(stdout, "\n");
                    i++;
                }
                else
                    fprintf(stdout, "%c", result->literal.char_value[i]);
            }
            break;
        case TRUE_TOKEN:
        case FALSE_TOKEN:
            fprintf(stdout, "%d", result->literal.boolean_value);
            break;
       default:
            error("Tried to print undefined undefined ValueTagged value in eval_print", __FILE__, __LINE__);
            break;
    }
    return (free_value(result), NULL);
}

static ValueTagged *evaluate(AST *node, EnvironmentMap *env_host) 
{  
    switch (node->tag) {
    case AST_LITERAL:
        return literal_value(node);
    case AST_IDENTIFIER:
        return identifier_value(node, env_host);    
    case AST_UNARY_EXPR:
        return evaluate_unary_expression(node, env_host);
    case AST_CALL_EXPR:
        return evaluate_call_expression(node, env_host);
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
    case AST_FOR_STMT:
        return evaluate_for_statement(node, env_host);
    case AST_PRINT_STMT:
        ValueTagged *result = evaluate(node->data.AST_PRINT_STMT.expr, env_host);
        return ((ValueTagged *)_printf(result));
    case AST_VAR_DECL_STMT:
        return evaluate_variable_statement(node, env_host);
    case AST_FUNCT_DECL_STMT:
        return evaluate_function_declaration_statement(node, env_host);
    case AST_RETURN_STMT:
        TODO("Finish Interpreting the return statement!");
    default:
        break;
    }
    error("Tried to evaluate undefined AST node type.", __FILE__, __LINE__);
    return NULL;
}

extern void interpret(AST *expr) 
{
     ValueTagged *value = NULL;
    if(setjmp(sync_env));
    else {
        fprintf(stdout, "Setjmp for interpreter!\n");
        value = evaluate(expr, &env_global);
    }
    free_value(value);
}

