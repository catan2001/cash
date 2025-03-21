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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

/*@Variable: env_global
*Global EnvironmentMap variable */
extern EnvironmentMap env_global;

/*@Function: env_copy_value
*Helper function that copies a variable value into particular Environment node */
static void env_copy_value(ValueTagged *, Environment *);

/*@Function: env_delete_var
*Function that deletes particular local variable in Environment and returns 0 if deleted*/
extern int env_delete_var(char *, EnvironmentMap *);

/*@Function: env_reset
*Function that resets (free) local Environment */
extern void env_reset(EnvironmentMap *);

/*@Function: env_assign_var
*Function that assignes some value to a variable in Environment */
extern void env_assign_var(Token *, ValueTagged *, EnvironmentMap *);

/*@Function: env_define_var
*Function that defines new variable Environment, reallocates size of Environment by one */
extern void env_define_var(Token *, ValueTagged *, EnvironmentMap *);

/*@Function: env_get_var
*Function that tries to find variable name in Environment map*/
extern ValueTagged *env_get_var(Token *, EnvironmentMap *);

/*@Function: env_define_function
*Function that defines new Function definition in Environment map*/
extern void env_define_function(Token *name, EnvironmentMap *env_map, AST *ast_definition);

/*@Function: env_get_function
*Function that tries to find a function name in Environment map*/
extern Environment *env_get_function(Token *name, EnvironmentMap *env_map);

#endif // ENVIRONMENT_H
