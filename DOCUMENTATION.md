# Documentation for cash interpreter
Welcome to the official documentation for the **cash interpreter**. This document serves as a comprehensive guide to understanding the features, capabilities, and design principles of the interpreter. 

> [!WARNING]
> The Interpreter is not in usable state yet.

Implementation is in GNU C.

Within this documentation, you will find detailed explanations of:

- The syntax and semantics of the cash interpreter
- Supported data types, operators, and control structures
- How the interpreter processes expressions, statements, and functions
- Implementation details, including parsing, evaluation, and execution
- Examples and usage guidelines for writing programs in cash

---
<p align="center">
  <img src="/misc/Logo.svg" />
</p>

## 1. Syntax

### Operators
The cash interpreting language supports a rich set of operators:
&rarr;


### Reserved words

### [Backus-Naur Form (BNF)](https://en.wikipedia.org/wiki/Backus%E2%80%93Naur_form)

```BNF
Rules:
**********************************************
<PROG>    ::=  <STMT>
<STMT>    ::=  <EXPR>
<EXPR>    ::=  <EQL>
<EQL>     ::=  <CMPR> (( "!=" | "==" ) <CMPR> )*
<CMPR>    ::=  <TERM> (( ">" | ">=" | "<" | "<=" ) <TERM> )*
<TERM>    ::=  <FACT> (( "-" | "+" ) <FACT> )*
<FACT>    ::=  <UNRY> (( "/" | "*") <UNRY> )*
<UNRY>    ::=  ( "!" | "-" ) <UNRY>
              | <PRIM>
<PRIM>    ::=  <NUM> | <STR> | "TRUE" | "FALSE" | "NULL"
              | "(" <EXPR> ")"


Syntax:
**********************************************
<NAME>    -> nonterminal
"NAME"    -> terminal

Scheme:
**********************************************
<PROG>    -> programm;
<STMT>    -> statement;
<EXPR>    -> expression;
<EQL>     -> equality;
<CMPR>    -> comparison;
<TERM>    -> term;
<FACT>    -> factor;
<UNRY>    -> unary;
<PRIM>    -> primary;

``` 
