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

### 31.1.2025.
- [ ] Combine tokenization and classification into one function Scanner/Lexer
- [ ] Define Rules for grammar of cashl
- [ ] ASTs and representing code
- [ ] encapsulate number abstract type for print
- [ ] Error handling function

## Steps

- [x] scanning the code from stdin
- [x] tokenization
- [ ] classification of tokens
  - [ ] control flow constructs
  - [ ] subshells and command substitution
  - [ ] redirection
  - [ ] if statements
  - [ ] pipeline
  - [ ] logical operators
  - [ ] dynamic types
  - [ ] comments
- [ ] parsing into ASTs
- [ ] code analysis
  - [ ] type error
- [ ] [garbage collector](https://courses.cs.washington.edu/courses/cse590p/05au/p50-bacon.pdf) | 
     [reference counting](https://ps.uci.edu/~cyu/p231C/LectureNotes/lecture13:referenceCounting/lecture13.pdf)
- [ ] execute the code

## Implementation Details
- [ ] [Data types](https://en.wikipedia.org/wiki/Data_type):
  - [ ] numbers: integer, decimal, hex, octal, binary
  - [ ] strings
  - [ ] enum
  - [ ] null
  - [ ] structures?
  - [ ] classes?
  - [ ] arrays
  - [ ] trees?

- [ ] Expressions:
  - [ ] add, subtract, multiply, divide, modulus
  - [ ] logical operators
  - [ ] comparison operators (implicit conversion?)
  - [ ] bitwise operators
  - [ ] conditional operators
  - [ ] precedence and associativity 
  - [ ] shift operators

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

- [] Classes/Structures or Prototypes