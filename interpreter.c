#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include "coretypes.h"
#include "error.h"
#include "interpreter.h"

static jmp_buf sync_env;

static ValueTagged *evaluate(AST *);

static void runtime_error_mode(void)
{
    longjmp(sync_env, TRUE);
}

static void eval_print(ValueTagged *result)
{ 
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
        error("Tried to print undefined undefined ValueTagged value in eval_print", __FILE__, __LINE__);
        break;
    }
}

static ValueTagged *literal_value(AST *node)
{
    if(node->tag != AST_LITERAL) 
    {
        error("Tried to return non-literal node.", __FILE__, __LINE__);
        abort();
    }
    
    ValueTagged *result = (ValueTagged *)malloc(sizeof(ValueTagged));
    return (result->literal = node->data.token->literal, result->type = node->data.token->type, result);
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

static ValueTagged *evaluate_unary_expression(AST *node)
{
    ValueTagged *right = evaluate(node->data.AST_UNARY.right);
    ValueTagged *result = (ValueTagged *)malloc(sizeof(ValueTagged));
    TokenType operator_type = node->data.AST_UNARY.token->type;
    result->type = right->type;

    switch(operator_type)
    {
        case SUBTRACT:
        {
            if(right->type == STRING) 
            {
                runtime_error(node->data.AST_UNARY.right, "Can't do unary subtract operation on strings!");
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
                runtime_error(node->data.AST_UNARY.right, "Can't do unary XOR operation on strings!");
                break;
            }
            if(right->type == NUMBER_FLOAT) 
            {
                runtime_error(node->data.AST_UNARY.right, "Can't do unary subtract operation on float!");
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

static ValueTagged *evaluate_binary_expression(AST *node)
{

    ValueTagged *left = evaluate(node->data.AST_BINARY.left);
    ValueTagged *right = evaluate(node->data.AST_BINARY.right);
    ValueTagged *result = (ValueTagged *)malloc(sizeof(ValueTagged));
    TokenType operator_type = node->data.AST_BINARY.token->type;
    
    if((left->type == STRING || right->type == STRING) && operator_type != ADD){
        (left->type == STRING) ? runtime_error(node->data.AST_BINARY.left, "Binary operator is not allowed on strings!")
                               : runtime_error(node->data.AST_BINARY.right, "Binary operator is not allowed on strings!");
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
                else if(left->type == STRING && right->type == NUMBER_INT)
                    TODO("Add support for STRING + INT = STRING");
                else if(left->type == STRING && right->type == NUMBER_FLOAT)
                    TODO("Add support for STRING + FLOAT = STRING");
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

static ValueTagged *evaulate_grouping_expression(AST *node)
{
    return evaluate(node->data.AST_GROUPING.left);
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

static ValueTagged *evaluate(AST *node)
{   
    switch (node->tag)
    {
    case AST_LITERAL:
        return literal_value(node);
    case AST_UNARY:
        return evaluate_unary_expression(node);
    case AST_BINARY:
        return evaluate_binary_expression(node);
    case AST_GROUPING:
        return evaulate_grouping_expression(node);
    case AST_EXPR_STMT:
        return evaluate(node->data.AST_EXPR_STMT.expr);
    case AST_PRINT_STMT:
        ValueTagged *result = evaluate(node->data.AST_PRINT_STMT.expr);
        return ((ValueTagged *)_printf(result));
    case AST_ASSIGN_EXPR:
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

    ValueTagged *value = evaluate(expr);
    eval_print(value);
}

