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

### NEXT TIME
- [ ] Add support for comma Expressions
- [ ] Add support for ternary operators
- [ ] Add error productions to handle each binary operator appearing without a left-hand operand
- [ ] Create Coroutine for synchronization
- [ ] Semantic evaluation of AST
- [ ] Combine tokenization and classification into one function Scanner/Lexer
- [ ] Encapsulate number abstract type for print

## Ideas
- [ ] Instead of having lexer -> parser -> execution. 
Create syntax tree AST as output file and then execute the AST separately. 
- [ ] Graphically represent parsed ASTs.
- [ ] Classes/Structures or Prototypes
- [ ] Shift ASCII characters, example: a >> 1 -> b... 
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
