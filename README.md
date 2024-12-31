# Cash
simple shell implementation in C for Linux

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
