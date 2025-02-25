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

EnvironmentMap env_map;

extern void reset_environment(void)
{
   if(env_map.env != NULL) free(env_map.env);
   env_map.env_size = 0;
}

extern void define_env_var(char *name, ValueTagged *value) {
   if(name == NULL) INTERNAL_ERROR("Passed null argument into define_env_var");
   env_map.env = realloc(env_map.env, sizeof(Environment) * (env_map.env_size + 1));
   if(env_map.env == NULL) INTERNAL_ERROR("Could not reallocate environment size!");

   env_map.env[env_map.env_size].name = name;
   env_map.env[env_map.env_size].value = value;

   env_map.env_size++;
}

extern Environment *get_env_var(Token *name)
{
   for(size_t i = 0; i < env_map.env_size; ++i)
      if(strcmp(env_map.env[i].name, name->lexeme)) return &env_map.env[i];
   
   set_error_flag();
   environment_error(name, "Undefined variable");
   return NULL;
}

