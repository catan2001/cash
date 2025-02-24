# Cash

<p align="center">
  <img src="/misc/Logo.svg" />
</p>

---
Simple shell implementation in C for Linux. **Cash** is a learning project designed to explore the inner workings of Bash (or any other shell). The goal is to implement a few fundamental features of Bash, offering a hands-on understanding of shell functionality.

> [!WARNING]
> The Interpreter is not in usable state yet. Work in progress!

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

## Notes from the book [Crafting Interpreters](#about-interpreters) and [other](#references) sources
**Note:** Sources from the book are marked with a number of the chapter.


### [Lexical Analysis (Lexer)](#lexical-analysis)

Lexical analysis is the first phase of processing source code. It is often called "scanning." In this phase, the interpreter reads the raw text (a sequence of characters) and groups them into meaningful sequences known as tokens. A token might represent a keyword, an identifier, a number, or a symbol like an operator.

- When two lexical grammar rules can both match a chunk of code that the scanner is looking at, whichever one matches the most characters wins.

### [Formal Language and Formal Grammar Theory](#formal-language-and-formal-grammar-1) 

Formal language and formal grammar theory help us understand and define the rules that determine which sequences of symbols (or strings) are considered valid in a language. Imagine you have a set of characters, known as an alphabet—for example, the digits 0 through 9 along with symbols such as "+" and "=". A string is simply a sequence of these characters, like "123+456". A formal language is a collection of such strings that are deemed well-formed according to a specific set of rules.

In order to evaluate an arithmetic node, you need to know the numeric values of its subtrees, so you have to evaluate those first. That means working your way from the leaves up to the root—a post-order traversal. [Check here](#explanation)

The formalism we used for defining the lexical grammar—the rules for how characters get grouped into tokens—was called a regular language. That was fine for our scanner, which emits a flat sequence of tokens. But regular languages aren’t powerful enough to handle expressions which can nest arbitrarily deeply.

| Terminology        | Lexical Grammar (Lexer)  | Syntactic Grammar (Parser) |
|--------------------|-----------------|-------------------|
| The “alphabet” is | Characters       | Tokens            |
| A “string” is     | Lexeme or token  | Expression        |
| It’s implemented by the | Scanner    | Parser            |

The alphabet of a formal language consists of symbols, letters or tokens that concatenate into strings called words. A formal language is often defined by means of a formal grammar such as a regular grammar or context-free grammar. It often makes sense to use an alphabet in the usual sense of the word, or more generally any finite character encoding such as ASCII or Unicode.  

- A Formal language consists of words which letters are taken from an alphabet and a are well-formed according to a specific set of rules defined in **Formal grammar**. 

**Example:**
The following rules describe a formal language **L** over the alphabet **Σ = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, +, =}:** 
- Every nonempty string that does not contain "+" or "=" and does not start with "0" is in L.
- The string "0" is in L.
- A string containing "=" is in L if and only if there is exactly one "=", and it separates two valid strings of L.
- A string containing "+" but not "=" is in L if and only if every "+" in the string separates two valid strings of L.
- No string is in L other than those implied by the previous rules.

A Formal grammar describes how strings from an alphabet are valid according to the Languages Syntax It is a set of rules for reweriting strings along with a start symbol from which rewriting starts. It can be though as a language generator or language recognizer.

A grammar mainly consists of a set of production rules, rewriting rules for transforming strings. Each rule specifies a replacement of a particular string (its left-hand side) with another (its right-hand side).

If you start with the rules, you can use them to generate strings that are in the grammar. Strings created this way are called derivations because each is derived from the rules of the grammar. In each step of the game, you pick a rule and follow what it tells you to do. Rules are called productions because they produce strings in the grammar.

Each production in a context-free grammar has a head—its name—and a body, which describes what it generates. In its pure form, the body is simply a list of symbols. a grammar further distinguishes between two kinds of symbols: nonterminal and terminal symbols;

- A terminal is a letter from the grammar’s alphabet. You can think of it like a literal value. In the syntactic grammar we’re defining, the terminals are individual lexemes—tokens coming from the scanner like if or 1234.

  - These are called “terminals”, in the sense of an “end point” because they don’t lead to any further “moves” in the game. You simply produce that one symbol.

- A nonterminal is a named reference to another rule in the grammar. It means “play that rule and insert whatever it produces here”. In this way, the grammar composes.

**Example**

Suppose we have a production rules:
1.  **S** &rarr; kb
2.  **S** &rarr; k**S**b

With these two rules we can begin with starting Symbol S. We chose one of the production rules for S to apply. If we chose first rule we generate **kb**. 

If we start with second rule production rule produces k**S**b. We can replace then again **S** with one of the rules. If we chose 1. rule we generate **kkbb** else we generate **kkSbb**. We can continue into infinity. 

The language of the grammar is the infinite set:
{${k^n kb  b^n | n \ge 0}$}

**Important:** This grammar is **context-free** (only single nonterminals appear as left-hand sides) and unambiguous.

#### [Chomsky Hierarchy](#formal-language-and-formal-grammar-1)

The Chomsky hierarchy in the fields of the formal language theory is a hierarchy of classes of formal grammars.

There are four thorized types of formal grammar:

1. **Regular (Type-3) Grammar**: Type-3 grammars generate regular languages. Such a grammar restrict its rules to a single nonterminal on left-hand side and a single terminal on the right-hand side (possibly followed by a single nonterminal) - **right-regular** and vice-versa **left-regular**. Regular languages are commonly used to define search patterns and the lexical structure of programming languages. 

2. **Context-free (Type-2) grammar**: Type-2 grammars generate the **context-free languages**. These are defines as a single nonterminal on the leftside. This restriction is non-trivial; not all languages can be generated by context-free grammars.

3. **Context-sensitive (Type-1) grammar**: Type-1 grammars generate the **context-sensitive languages**.  These grammars have rules of the form α A β → α γ β with A a nonterminal and  α γ β strings of nonterminal or terminal type. The languages described with such grammar are exactly all languages that can be recognized by a linear bounded automaton (a nondeterministic Turing machine whose tape is bounded by a constant times the length of the input.)

4. **Recursively enumerable (Type-0)**: Type-0 grammars include all formal grammars. There are no constraints on the productions rules. They generate all lagnuages that can be recognized by a **Turing Machine**. 

Although Type-3 and Type-2 types of formal grammar are less powerful and use more restriction they are often used to describe parsers for programming languages. 

There are many different variations on Chomsky's classification of formal grammars (ex. **Tree-adjoining grammars** TAG increase the expressiveness of conventional generative grammars by allowing rewrite rules to operate on parse trees instead of just strings).

### [AST Implementation](#abstract-syntax-tree-asts)
An abstract syntax tree is a tree-shaped representation of formal grammar, convenient for compiler to operate.

**Example:** An interpreter might represent an expression like **4 + 2 * 10 + 3 * (5 + 1)** using tree structure like this:

<p align="center">
  <img src="/misc/ast.svg" />
</p>

Syntax rules are important in this step! For example:
6 / 3 - 1 can be parsed into a tree like (6/3) - 1 or 6 /(3-1). These two ways give different results. Because of this we have to introduce new rules:
- **associativity**: tells which operator is executed first in the series of same operator:
  - **left-associative**: (7 - 4 - 1);
  - **right-associative**: a = b = c;
- **precedence**: tells which operator has more power!

We introduce same rules for associativity as in C programming language:

| Name | Operators | Associates |
|------|-----------|------------|
| Equality | == !=       | left            |
| Comparison |  	> >= < <=  | left        |
| Term |  	- +    | left            |
| Factor | / *    | left            |
| Unary | ! -    | right            |

### [Error Detection and Recovery](#error-detection-and-recovery-1)

An error is a user-initiated action that results in a program's abnormal behavior or issues. It is critical to recovering these errors as quickly as possible because if they are not retrieved in a timely manner, they will lead to a situation from which it will be extremely difficult to recover.

Error handling process is very important step in implementation of compiler/interpreter. There are three steps each handler has to to:
- 1. Error Detection.
- 2. Error Reporting
- 3. Error Handling

Error recovery is a process of parser responding to an error while minimizing negative effects on the rest of process. To recover from an error a parser first has to detect an error. 

There are four types of errors that can be caught in different stages of parser:
- 1. Lexical Error: When some identifier is misspelled or unused character is found.
- 2. Syntactical Error: When there is a syntax that was badly written.
- 3. Semantical Error: Incompatible type was used for ex.
- 4. Logical Error: Infinite loop was detected.

There are multiple types of error recovery and all of them relly on different trade-off. Following this, there are three kinds of error recovery that can be implemented in the parser to deal with the errors in the code:
- 1. Panic mode: It is the easiest way to error recovery preventing parser to develop infinite loops.
- 2. Statement mode: When the parser encounters an error it tries to add that particle of error virtually keeping in the box that also one wrong correction can result in the infinite loop.
- 3. Global correction mode: The parsers tries to consider whole input file and decides what went wrong and what could replace the error.
- 4. Pharse-level mode: It performs local correction on the remaining input.

Of all therecovery techniques devised in yesteryear, the one that best stood the test of time is called—somewhat alarmingly—panic mode.

**Synchronization**

When the parser encounters an unexpected token (i.e. one that doesn’t fit the grammar rule being processed), it cannot continue normal parsing because its internal state is no longer aligned with the expected structure. To recover, the parser “synchronizes” by discarding tokens until it reaches a token from a predetermined set—the synchronization set. This set is chosen because the tokens within it mark points where the input is likely to be “in sync” with the grammar.

A synchronization token is typically one that indicates the beginning of a new statement or expression. For instance, in languages like C or Java, a semicolon often marks the end of a statement. When an error occurs, the parser might discard tokens until it sees a semicolon, then assume that the next token starts a new statement. In other languages (or in interactive settings), a newline may serve this role.

More formally, many parser generators use the concept of the FOLLOW set of a nonterminal. The FOLLOW set of a nonterminal consists of all terminal symbols that can legally appear immediately after that nonterminal in some derivation. Using the FOLLOW set helps the parser determine safe tokens at which it can resume parsing after skipping over erroneous input.

### Evaluation

To execute parsed AST, it's necessary to evaluate each node in the tree. We can do it yet again using recursion and checking each node. Doing this will enable us to intrepret each node and continue to it's parent node. 

- Read about REPL.

### [Statements and State](#statements-and-state-side-effect)

State and statements go hand in hand. Since statements, by definition, don’t
evaluate to a value, they need to do something else to be useful. That something
is called a side effect. It could mean producing user-visible output or modifying
some state in the interpreter that can be detected later.

## References

### **About Shell:**
- [Bash (Unix shell) - Wikipedia](https://en.wikipedia.org/wiki/Bash_(Unix_shell))
- [Bash Reference Manual - gnu.org](https://www.gnu.org/software/bash/manual/html_node/index.html)

### **Lexical Analysis:**
- [Lexical Analysis - Wikipedia](https://en.wikipedia.org/wiki/Lexical_analysis)

### **Syntax Analysis**
- [PackCC: a parser generator for C - Github](https://github.com/arithy/packcc/blob/master/src/packcc.c)
- [Parsing - Wikipedia](https://en.wikipedia.org/wiki/Parsing#Types_of_parsers)

### **Formal Language and Formal Grammar:**
- [Formal Grammar - Wikipedia](https://en.wikipedia.org/wiki/Formal_grammar)
- [Formal Language - Wikipedia](https://en.wikipedia.org/wiki/Formal_language)
- [Chomsky Hierarchy - Wikipedia](https://en.wikipedia.org/wiki/Chomsky_hierarchy)

### **Abstract Syntax Tree (ASTs):**
- [Introduction to Binary Tree - GFG](https://www.geeksforgeeks.org/introduction-to-binary-tree/)
- [Abstract Syntax Tree - Wikipedia](https://en.wikipedia.org/wiki/Abstract_syntax_tree)
- [AST example in C - Keleshev](https://keleshev.com/abstract-syntax-tree-an-example-in-c/) 

### **Error Detection and Recovery:**
- [What is Error recovery - GFG](https://www.geeksforgeeks.org/what-is-error-recovery/)
- [Error recovery and Handling - naukri](https://www.naukri.com/code360/library/error-recovery-and-handling)

### **Statements and State, Side effect**
- [Side Effect - Wikipedia](https://en.wikipedia.org/wiki/Side_effect_(computer_science))
- [Imperative Programming - Wikipedia](https://en.wikipedia.org/wiki/Imperative_programming)

### **About Interpreters:**
- [Crafting Interpreters - Robert Nystorm](https://craftinginterpreters.com/contents.html)


