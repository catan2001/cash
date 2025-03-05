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
#include "coretypes.h"
#include "error.h"
#include "environment.h"

EnvironmentMap env_global = {.env = NULL, .env_enclosing = NULL, .env_size = 0};

static void env_copy_value(ValueTagged *value, Environment *env_value) 
{
   if(value == NULL) {
      env_value->value.type = NUMBER_INT;
      env_value->value.literal.integer_value = 0;
      return;
   };
   
   env_value->value.type = value->type;
   switch (value->type) {
   case NUMBER_INT:
      env_value->value.literal.integer_value = value->literal.integer_value;
      return;
   case NUMBER_FLOAT:
      env_value->value.literal.float_value = value->literal.float_value;
      return; 
   case STRING:
      env_value->value.literal.char_value = strdup(value->literal.char_value);
      return;
   case TRUE_TOKEN:
   case FALSE_TOKEN:
      env_value->value.literal.boolean_value = value->literal.boolean_value;
      return;
   default:
      break;
   }
}

extern int env_delete_var(char *name, EnvironmentMap *env_map) 
{
   for(size_t i = 0; i < env_map->env_size; ++i) {
      if(!strcmp(name, env_map->env[i].name)) {
         free(env_map->env[i].name);
         if(env_map->env[i].value.type == STRING) free(env_map->env[i].value.literal.char_value);
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
      if(env_map->env[i].value.type == STRING) free(env_map->env[i].value.literal.char_value);
   }
   if(env_map->env != NULL) free(env_map->env);
   env_map->env_size = 0;
}

extern void env_assign_var(Token *name, ValueTagged *value, EnvironmentMap *env_map) 
{
   if(name == NULL) INTERNAL_ERROR("Passed null name argument.");

   /* Search Environment for the same variable */
   for(size_t i = 0; i < env_map->env_size; ++i) {
      if(!strcmp(name->lexeme, env_map->env[i].name)) {
         if(env_map->env[i].value.type == STRING)
            free(env_map->env[i].value.literal.char_value);
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
      if(!strcmp(name->lexeme, env_map->env[i].name)) {
         if(env_map->env[i].value.type == STRING)
            free(env_map->env[i].value.literal.char_value);
         env_copy_value(value, &env_map->env[i]); 
         return;
      }
   }
   /* If it was not found in Environment, add new variable to the Environment */
   env_map->env = realloc(env_map->env, sizeof(Environment) * (env_map->env_size + 1));
   if(env_map->env == NULL) INTERNAL_ERROR("Could not reallocate environment size!");

   env_map->env[env_map->env_size].name = strdup(name->lexeme);
   env_copy_value(value, &env_map->env[env_map->env_size]); 
   env_map->env_size++;
}

extern ValueTagged *env_get_var(Token *name, EnvironmentMap *env_map) 
{
   for(size_t i = 0; i < env_map->env_size; ++i) { 
      if(!strcmp(env_map->env[i].name, name->lexeme)) return &env_map->env[i].value;
    }
  
   if(env_map->env_enclosing != NULL) return env_get_var(name, env_map->env_enclosing);

   set_error_flag();
   environment_error(name, "Undefined variable");
   return NULL;
}

