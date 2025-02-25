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

#ifndef ERROR_H
#define ERROR_H

#define ERROR -1
#define USER_ERROR 300
#define INTERNAL_ERROR(msg) do {                                            \
        fprintf(stderr, "%s:%d Internal Error: %s\n", __FILE__, __LINE__);  \
    } while(0);

/* Global flag variable that indicates if error happened with error code */
extern int error_flag;

/*@Function: set_error_flag
*Sets error_flag variable to specific error code */
extern void set_error_flag(void);

/*@Function: reset_error_flag
*Resets error_flag variable to 0 (FALSE) */
extern void reset_error_flag(void);

/*@Function: sigint_handler
*Handler for signal interrupts */
extern void sigint_handler(const int);

/*@Function: parser_error
*Helper Function for printing parser errors */
extern void parser_error(Token, char *);

/*@Function: runtime_error
*Function that prints an error during runtime*/
extern void runtime_error(AST *, char *);

/*@Function: error
*Displays error msg to stdout */
extern int error(char *, char *, int );

#endif // ERROR_H