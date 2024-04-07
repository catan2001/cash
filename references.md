#References

## exec system call
https://man7.org/linux/man-pages/man3/exec.3.html

## getcwd
returns current working directory
https://man7.org/linux/man-pages/man3/getcwd.3.html

## isatty()
checks whether a file descriptor refers to a terminal
usefull  when printing on terminal!

## fileno
fileno - obtain file descriptor of a stdio stream
used in isatty, 0 (stdin), 1 (stdout), and 2 (stderr)

## Tokenizer
tokenizer breaks a stream of text into tokens, usually 
by looking for whitespaces etc.

## Lexer
it's basically a tokenizer but it usually attaches extra context to the token (number, string, program etc)

## Parser
a parser takes stream of tokens from the lexer and turns it into an abstract syntax tree representing the (usually) program represented by original text

### might need:
**levenshtein distance** but you have to implement cache system to make it faster
