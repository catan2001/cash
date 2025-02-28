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
<PROG>        ::=   <DECL-STMT>* <EOF>
<DECL-STMT>   ::=   <VAR-DECL> | <STMT>
<VAR-DECL>    ::=   "var" <ID> ("=" <EXPR>)? ";"
<STMT>        ::=   <EXPR-STMT> | <PRINT-STMT> | <BLOCK-STMT> 
                    | <IF-STMT>
<EXPR-STMT>   ::=   <EXPR> ";"
<PRINT-STMT>  ::=   "printf" <EXPR> ";"
<BLOCK-STMT>  ::=   "{" <DECL-STMT>* "}"
<IF-STMT>     ::=   "if" "(" <EXPR> ")" <STMT> ("else" <STMT>)?
<EXPR>        ::=   <ASSIGN>
<ASSIGN>      ::=   <ID> "=" <ASSIGN>
                    | <EQL>
<EQL>         ::=   <CMPR> (( "!=" | "==" ) <CMPR> )*
<CMPR>        ::=   <TERM> (( ">" | ">=" | "<" | "<=" ) <TERM> )*
<TERM>        ::=   <FACT> (( "-" | "+" ) <FACT> )*
<FACT>        ::=   <UNRY> (( "/" | "*") <UNRY> )*
<UNRY>        ::=   ( "!" | "-" ) <UNRY>
                    | <PRIM>
<PRIM>        ::=   <NUM> | <STR> | "TRUE" | "FALSE" | "NULL"
                    | "(" <EXPR> ")" 
                    | <ID>
<EOF>         ::= "end of file"


Syntax:
**********************************************
<NAME>        -> nonterminal
"NAME"        -> terminal

Scheme:
**********************************************
<PROG>        -> program;
<DECL-STMT>   -> declaration statement;
<VAR-DECL>    -> variable declaration statement;
<STMT>        -> statement;
<EXPR-STMT>   -> expression statement;
<PRINT-STMT>  -> print statement;
<BLOCK-STMT>  -> block statement;
<IF-STMT>     -> if statement;
<EXPR>        -> expression;
<ASSIGN>      -> assignment-expression;
<EQL>         -> equality;
<CMPR>        -> comparison;
<TERM>        -> term;
<FACT>        -> factor;
<UNRY>        -> unary;
<PRIM>        -> primary;
<ID>          -> identifier

``` 
