# Cash

<p align="center">
  <img src="/misc/Logo.svg" />
</p>

---
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

## Notes from the book [Crafting Interpreters](#about-interpreters)
### 4. Scanning

- Ideally, we would have an actual abstraction, some kind of “ErrorReporter” interface that gets passed to the scanner and parser so that we can swap out different reporting strategies. For our simple interpreter here, I didn’t do that, but I did at least move the code for error reporting into a different class.

- When two lexical grammar rules can both match a chunk of code that the scanner is looking at, whichever one matches the most characters wins.

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

- [ ] Shift ASCII characters, example: a >> 1 == b... 

