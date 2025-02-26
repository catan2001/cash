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

/*@Variable: env_map
*Global EnvironmentMap variable */
extern EnvironmentMap env_map;

/*@Function: copy_val_env
*Helper function that copies a variable value into particular Environment node */
static void copy_val_env(ValueTagged *value, Environment *env_value);

/*@Function: delete_env_var
*Function that deletes particular variable in environment and returns 0 if deleted*/
extern int delete_env_var(char *name);

/*@Function: reset_environment
*Function that resets (free) environment */
extern void reset_environment(void);

/*@Function: define_env_var
*Function that defines new variable environment, reallocates size of environment by one */
extern void define_env_var(char *, ValueTagged *);

/*@Function: get_env_var
*Function that tries to find variable name in environment map*/
extern ValueTagged *get_env_var(Token *);

#endif // ENVIRONMENT_H