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

#ifndef INTERPRETER_H
#define INTERPRETER_H

/*@Function: runtime_error_mode
*Function that deals with runtime error by jumping to next stmt */
static void runtime_error_mode(void);

/*@Function: eval_print
*Function that is used to print the result of evaluation */
static void eval_print(ValueTagged *);

/*@Function: is_truth
*Helper function that returns TRUE, FALSE */
static Value is_truth(ValueTagged *, TokenType);

/*@Function: literal_value
*Function that returns value of AST node */
static ValueTagged *literal_value(AST *);

/*@Function: identifier_value
*Function that returns value of identifier found in Environment*/
static ValueTagged *identifier_value(AST *, EnvironmentMap *);

/*@Function: evaluate_unary_expression
*Function that evaluates unary expression */
static ValueTagged *evaluate_unary_expression(AST *, EnvironmentMap *);

/*@Function: evaluate_binary expression
*Function that evaluates binary expression */
static ValueTagged *evaluate_binary_expression(AST *, EnvironmentMap *);

/*@Function: evaluate_grouping_expression
*Function that evaluates grouping expression */
static ValueTagged *evaulate_grouping_expression(AST *, EnvironmentMap *);

/*@Function: evaluate_assign_expression 
*Function that evaluates assign expression */
static ValueTagged *evaluate_assign_expression(AST *, EnvironmentMap *);

/*@Function: evaluate_block_statement
*Function that evaluates block statement */
static ValueTagged *evaluate_block_statement(AST *, EnvironmentMap *, EnvironmentMap *); 

/*@Function: evaluate_if_statement
*Function that evaluates if statement */
static ValueTagged *evaluate_if_statement(AST *, EnvironmentMap *); 

/*@Function: evaluate_while_statement
*Function that evaluates while statement */
static ValueTagged *evaluate_while_statement(AST *, EnvironmentMap *); 

/*@Function: evaluate_variable_statement
*Function that evaluates variable statement */
static ValueTagged *evaluate_variable_statement(AST *, EnvironmentMap *); 

/*@Function: evaluate_return_statemente
*Function that evaluates return statement */
static ValueTagged *evaluate_return_statement(AST *, EnvironmentMap *);

/*@Function: evaluate_time_statement
*Function that evaluates time statement*/
static ValueTagged *evaluate_time_statement(AST *, EnvironmentMap *); 

/*@Function: evaluate_clear_statement
*Function that evaluates clear statement*/
static ValueTagged *evaluate_clear_statement(AST *, EnvironmentMap *); 

/*@Function: _printf
*Function that evaluates result of AST printf */
static ValueTagged * _printf(ValueTagged *);

/*@Function: evaluate
*Function that calls evaluation of specific node type */
static ValueTagged *evaluate(AST *, EnvironmentMap *);

/*@Function: interpret
*Function that interprets expressions*/
extern void interpret(AST *);

/*@Global Variable: cwd
 * Variable used for specifying current working directory*/
extern char cwd[FILE_PATH_SIZE];

#endif // INTERPRETER_H
