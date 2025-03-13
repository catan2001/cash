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
#include <setjmp.h>
#include "coretypes.h"
#include "error.h"
#include "environment.h"

EnvironmentMap env_global = {.env = NULL, .env_enclosing = NULL, .env_size = 0};

static void env_copy_value(ValueTagged *value, Environment *env_value) 
{
    if(value == NULL) {
        env_value->data.value.type = NUMBER_INT;
        env_value->data.value.literal.integer_value = 0;
        return;
    };

    env_value->data.value.type = value->type;
    switch (value->type) {
        case NUMBER_INT:
            env_value->data.value.literal.integer_value = value->literal.integer_value;
            return;
        case NUMBER_FLOAT:
            env_value->data.value.literal.float_value = value->literal.float_value;
            return; 
        case STRING:
            env_value->data.value.literal.char_value = strdup(value->literal.char_value);
            return;
        case TRUE_TOKEN:
        case FALSE_TOKEN:
            env_value->data.value.literal.boolean_value = value->literal.boolean_value;
            return;
        default:
            break;
    }
}

extern int env_delete_var(char *name, EnvironmentMap *env_map) 
{
    for(size_t i = 0; i < env_map->env_size; ++i) {
        if(!strcmp(name, env_map->env[i].name) && env_map->env[i].type == ENV_VARIABLE) {
            free(env_map->env[i].name);
            if(env_map->env[i].data.value.type == STRING) free(env_map->env[i].data.value.literal.char_value);
            env_map->env_size--;
            env_map->env = realloc(env_map->env, env_map->env_size);
            return 0;
        }
    }
    return 1;
}

extern void env_reset(EnvironmentMap *env_map) 
{
    for(size_t i = 0; i < env_map->env_size; ++i) {
      free(env_map->env[i].name);
      switch (env_map->env[i].type) 
      {
          case ENV_VARIABLE:
              if(env_map->env[i].data.value.type == STRING) free(env_map->env[i].data.value.literal.char_value);
              break;
          case ENV_FUNCTION:
              // AST is later freed
              break;
          default:
              break;
      }
    }
    if(env_map->env != NULL) free(env_map->env);
    if(env_map->env_return != NULL) {
        if(env_map->env_return->type == STRING) free(env_map->env_return->literal.char_value);
        free(env_map->env_return);
    } 
    env_map->env_size = 0;
}

extern void env_assign_var(Token *name, ValueTagged *value, EnvironmentMap *env_map) 
{
    if(name == NULL) INTERNAL_ERROR("Passed null name argument.");

    /* Search Environment for the same variable */
    for(size_t i = 0; i < env_map->env_size; ++i) {
      if(!strcmp(name->lexeme, env_map->env[i].name) && env_map->env[i].type == ENV_VARIABLE) {
         if(env_map->env[i].data.value.type == STRING)
            free(env_map->env[i].data.value.literal.char_value);
         env_copy_value(value, &env_map->env[i]); 
         return;
      }
    }

    if(env_map->env_enclosing != NULL) return env_assign_var(name, value, env_map->env_enclosing);

    set_error_flag();
    environment_error(name, "Undefined variable");
}

extern void env_define_var(Token *name, ValueTagged *value, EnvironmentMap *env_map) 
{
    if(name == NULL) INTERNAL_ERROR("Passed null name argument.");
    /* Search Environment for the same variable */
    for(size_t i = 0; i < env_map->env_size; ++i) {
      if(!strcmp(name->lexeme, env_map->env[i].name) && env_map->env[i].type == ENV_VARIABLE) {
         if(env_map->env[i].data.value.type == STRING)
            free(env_map->env[i].data.value.literal.char_value);
         env_copy_value(value, &env_map->env[i]); 
         return;
      }
    }
    /* If it was not found in Environment, add new variable to the Environment */
    env_map->env = realloc(env_map->env, sizeof(Environment) * (env_map->env_size + 1));
    if(env_map->env == NULL) INTERNAL_ERROR("Could not reallocate environment size!");

    env_map->env[env_map->env_size].name = strdup(name->lexeme);
    env_map->env[env_map->env_size].type = ENV_VARIABLE;
    env_copy_value(value, &env_map->env[env_map->env_size]); 
    env_map->env_size++;
}

extern ValueTagged *env_get_var(Token *name, EnvironmentMap *env_map) 
{
    for(size_t i = 0; i < env_map->env_size; ++i) { 
      if(!strcmp(env_map->env[i].name, name->lexeme) && env_map->env[i].type == ENV_VARIABLE) return &env_map->env[i].data.value;
    }

    if(env_map->env_enclosing != NULL) return env_get_var(name, env_map->env_enclosing);

    set_error_flag();
    environment_error(name, "Undefined variable");
    return NULL;
}

extern void env_define_function(Token *name, EnvironmentMap *env_map, AST *ast_definition)
{
    if(name == NULL) INTERNAL_ERROR("Passed null name argument");
    /* Search Environment for the same variable */
    for(size_t i = 0; i < env_map->env_size; ++i) {
        if(!strcmp(name->lexeme, env_map->env[i].name) && env_map->env[i].type == ENV_FUNCTION) {
            env_map->env[i].data.ENV_FUNCTION.definition = ast_definition;
            return;
        }
    }
    /* If it was not found in Environment, add new variable to the Environment */
    env_map->env = realloc(env_map->env, sizeof(Environment) * (env_map->env_size + 1));
    if(env_map->env == NULL) INTERNAL_ERROR("Could not reallocate environment size!");

    env_map->env[env_map->env_size].name = strdup(name->lexeme);
    env_map->env[env_map->env_size].data.ENV_FUNCTION.definition = ast_definition;
    env_map->env[env_map->env_size].type = ENV_FUNCTION;
    env_map->env_size++;
}

extern Environment *env_get_function(Token *name, EnvironmentMap *env_map)
{
    if(name == NULL) INTERNAL_ERROR("Passed null name argument");
    
    for(size_t i = 0; i < env_map->env_size; ++i) { 
      if(!strcmp(env_map->env[i].name, name->lexeme) && env_map->env[i].type == ENV_FUNCTION) return &env_map->env[i];
    }

    if(env_map->env_enclosing != NULL) return env_get_function(name, env_map->env_enclosing);

    set_error_flag();
    environment_error(name, "Undefined function");
    return NULL;
}
