# Cash
Simple shell implementation in C for Linux. **Cash** is a learning project designed to explore the inner workings of Bash (or any other shell). The goal is to implement a few fundamental features of Bash, offering a hands-on understanding of shell functionality.

## Introduction
**Cash** is a learning project designed to explore the inner workings of Bash (or any other shell). The goal is to implement a few fundamental features of Bash, offering a hands-on understanding of shell functionality. This project is intended solely for educational and tutorial purposes.

## Explanation


```
EXAMPLE: cmd1 | cmd2 > file

        root
         |
       /   \
   cmd1     >
          /   \
       cmd2   file
```

**Basic Rule**: execute always first left branch

**Explanation**: A lexical analysis tool can theoretically utilize various data structures. For instance, Bash employs a combination of parse trees, hash tables, linked lists, stacks, and dynamic arrays. Once classification is complete, it should be clear how to implement a parse tree in Cash. The difficulty of implementation depends on the computational resources available in Cash. Introducing brackets, () or [] makes huge difference in how parsing is done.


## Update and TODO List

#### 20.1.2025.

- [x] clean heap allocation
  - [x] added SIGINT to clean heap
- [x] cleaned code
- [x] updated Makefile  

#### 21.2.2025.
- [x] Fixing tokenization of special characters [WIP]
- [x] Styled code
- [x] implement comment #

#### 23.2.2025.
- [x] Fixed tokenizer to properly tokenize special characters
- [x] Added helper functions type_of_character and add_token
- [x] Cleaned up the code

#### 24.2.2025.
- [] HASH map
- [] implement numbers
- [] implement string "..."

### Steps

- [x] scanning the code from stdin
- [x] tokenization
- [] classification of tokens
  - [] control flow constructs
  - [] subshells and command substitution
  - [] redirection
  - [] if statements
  - [] pipeline
  - [] logical operators
  - [] dynamic types
  - [] comments
- [] parsing into ASTs
- [] code analysis
  - [] type error
- [] [garbage collector](https://courses.cs.washington.edu/courses/cse590p/05au/p50-bacon.pdf) | 
     [reference counting](https://ps.uci.edu/~cyu/p231C/LectureNotes/lecture13:referenceCounting/lecture13.pdf)
- [] execute the code

### Implementation Details
- [] [Data types](https://en.wikipedia.org/wiki/Data_type):
  - [] numbers: integer, decimal, hex, octal, binary
  - [] strings
  - [] enum
  - [] null
  - [] structures?
  - [] classes?
  - [] arrays
  - [] trees?

- [] Expressions:
  - [] add, subtract, multiply, divide, modulus
  - [] logical operators
  - [] comparison operators (implicit conversion?)
  - [] bitwise operators
  - [] conditional operators
  - [] precedence and associativity 
  - [] shift operators

- [] Comments 
```python
       # I think that using this sign will help later when constructing path

       #* This should be used if a block of comment is necessary *#
```

- [] Statements
  - [] similar to the Python or Bash?
- [] Variables
- [] Control Flow
  - [] if, else
  - [] while
  - [] for
  - [] foreach
  - [] switch case?
  - [] break and continue

- Function
```python
       # use funct just to appreciate Robert Nystorm 
       funct SumOfTwo(a, b){ 
              return a+b;
       }
```
- [] Closures
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

## References

### **About Shell:**
- [Bash (Unix shell) - Wikipedia](https://en.wikipedia.org/wiki/Bash_(Unix_shell))
- [Bash Reference Manual - gnu.org](https://www.gnu.org/software/bash/manual/html_node/index.html)

### **Lexical Analysis:**
- [Lexical Analysis - Wikipedia](https://en.wikipedia.org/wiki/Lexical_analysis)
- [PackCC: a parser generator for C - Github](https://github.com/arithy/packcc/blob/master/src/packcc.c)
### **Abstract Syntax Tree (ASTs):**
- [Introduction to Binary Tree - GFG](https://www.geeksforgeeks.org/introduction-to-binary-tree/)
- [Abstract Syntax Tree - Wikipedia](https://en.wikipedia.org/wiki/Abstract_syntax_tree)

### **About Interpreters:**
- [Crafting Interpreters - Robert Nystorm](https://craftinginterpreters.com/contents.html)



## Ideas:

- [] Shift ASCII characters, example: a >> 1 == b... 

