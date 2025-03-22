# TODO List for Cash

This is a TODO list for Cash, used to track tasks and maintain a history of code updates with corresponding dates.

### 20.1.2025.

- [x] clean heap allocation
  - [x] added SIGINT to clean heap
- [x] cleaned code
- [x] updated Makefile  

### 21.1.2025.
- [x] Fixing tokenization of special characters [WIP]
- [x] Styled code
- [x] implement comment #

### 23.1.2025.
- [x] Fixed tokenizer to properly tokenize special characters
- [x] Added helper functions type_of_character and add_token
- [x] Cleaned up the code

### 27.1.2025.
- [x] Refactored code

### 28.1.2025.
- [x] tokenized strings
- [x] classify strings
- [x] classify special token
- [x] classify reserved words
- [x] classify numbers

### 29.1.2025.
- [x] manage unused characters

### 9.2.2025.
- [x] Helper functions for AST
- [x] ASTs builder and representing code
- [x] ASTs print function for debug
- [x] Define Rules for grammar of cash

### 11.2.2025.
- [x] Fixed Seg Faulting
- [x] Error AST print
- [x] Wired-up Parser

### 14.2.2025.
- [x] Error handling function
- [x] Synchronization 
- [x] Add class and struct keywords to parser

### 16.2.2025.
- [x] Fix error function

### 19.2.2025.
- [x] Add error productions to handle each binary operator appearing without a left-hand operand
- [x] Started working on evaluation

### 20.2.2025.
- [x] Added Boolean type and classified TRUE and FALSE
- [x] Fixed evaluation of unary expression.
- [x] Tested evaluate_unary_expression function

### 21.2.2025.
- [x] Refactored functions.
- [x] Added evalution for binary expression.
- [x] Added evaluation caller function.

### 22.2.2025.
- [x] Added evaluation for grouping expression.
- [x] Added runtime error handling function.
- [x] Added result printing function.
 
### 23.2.2025.
- [x] Fixed error handling  
- [x] Added global error variable
- [x] Added expression statements
- [x] Added printf statement

### 25.2.2025.
- [x] Separated source code into designated files
- [x] Refactored code
- [x] Parsed global variables

### 26.2.2025.
- [x] Refactored code
- [x] Fixed Identifier functions

### 27.2.2025.
- [x] Deal with undefined variables (thrash null)
- [x] Finished Global variables.

### 28.2.2025.
- [x] Added block statement.
- [x] Interpreted Local variables
- [x] Added If statements

### 3.3.2025.
- [x] Add loading files!
- [x] Added While loop stmt

### 4.3.2025.
- [x] Added For loops statements
- [x] Fixed For loops and while loop ast_free and ast_print

### 5.3.2025.
- [x] Fixed deallocation!
- [x] Refactored code
- [x] Fixed New Line in print stmt

### 6.3.2025.
- [x] Added call expression
- [x] Interpreting call WIP

### 9.3.2025.
- [x] Parsed function definition
- [x] Interpreted function definition
- [x] Fixed interpreting a call
- [x] Updated Environment

### 11.3.2025.
- [x] Fixed error in function call when wrong arity
- [x] Fixed error in if else statement
- [x] Changed Lexer and parsed return statement
- [x] Interpreting return stmt is wip

### 12.3.2025.
- [x] Interpreting return statement WIP

### 13.3.2025.
- [x] Fixed Interpreting return statement.

### 18.3.2025.
- [x] Added time command
- [x] Added clear command

### 20.3.2025.
- [x] Added cd command
- [x] run command work in progress

### 22.3.2025.
- [x] Fixed bugs
- [x] run command works

### NEXT TIME:
- [ ] Major bug when adding new line to .cash file
- [ ] Fix errors, and function line numbering!
- [ ] Fix Panic mode!
- [ ] Implement break and continue
- [ ] Bug when freeing memory for unused characters
- [ ] Add later an option for searching and running an identifier from $PATH variable
- [ ] When you implement the option for running an executable you can add & for running in the background
- [ ] If we are running a cash script the kernel looks for a shebang #! /bin/sh to execute the script 
- [ ] Add support for ternary ?: operators
- [ ] Add support for comma Expressions
- [ ] Create Coroutine for synchronization
- [ ] Encapsulate number abstract type for print
- [ ] In future Add special NULL types.
- [ ] Add native functions from stdlib, math, time, stdio

## Ideas
- [ ] Instead of having lexer -> parser -> execution. 
Create syntax tree AST as output file and then execute the AST separately. 
- [ ] Graphically represent parsed ASTs.
- [ ] Classes/Structures or Prototypes
- [ ] Shift ASCII characters, example: a >> 1 -> b... 
- [ ] Add an option for user to write a function in C and later incorporate them in cash. Basically implement foreign function interface
## Steps

- [x] scanning the code from stdin
- [x] tokenization
- [x] classification of tokens
  - [x] control flow constructs
  - [x] subshells and command substitution
  - [x] redirection
  - [x] if statements
  - [x] pipeline
  - [x] logical operators
  - [x] dynamic types
  - [x] comments
- [ ] parsing into ASTs
- [ ] code analysis
  - [ ] type error
- [ ] [garbage collector](https://courses.cs.washington.edu/courses/cse590p/05au/p50-bacon.pdf) | 
     [reference counting](https://ps.uci.edu/~cyu/p231C/LectureNotes/lecture13:referenceCounting/lecture13.pdf)
- [ ] execute the code

## Implementation Details
- [x] [Data types](https://en.wikipedia.org/wiki/Data_type):
  - [ ] numbers: integer, decimal, hex, octal, binary
  - [x] strings
  - [x] enum
  - [x] null
  - [ ] structures?
  - [ ] classes?
  - [ ] arrays
  - [ ] trees?

- [x] Expressions:
  - [x] add, subtract, multiply, divide, modulus
  - [x] logical operators
  - [x] comparison operators (implicit conversion?)
  - [x] bitwise operators
  - [x] conditional operators
  - [x] precedence and associativity 
  - [x] shift operators

- [ ] Comments 
```python
       # I think that using this sign will help later when constructing path

       #* This should be used if a block of comment is necessary *#
```

- [ ] Statements
  - [ ] similar to the Python or Bash?
- [ ] Variables
- [ ] Control Flow
  - [ ] if, else
  - [ ] while
  - [ ] for
  - [ ] foreach
  - [ ] switch case?
  - [ ] break and continue

- Function
```python
       # use funct just to appreciate Robert Nystorm 
       funct SumOfTwo(a, b){ 
              return a+b;
       }
```
- [ ] Closures
```
funct returnFunction() {
  var outside = "outside";

  funct inner() {
    print outside;
  }

  return inner;
}
```
