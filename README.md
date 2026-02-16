# Prometheus Compiler

A minimal C++ compiler for an experimental programming language, focused on clean parsing, well-structured AST construction, and extensible code generation.

## Motivation

This project explores **how a simple language can be translated into low-level instructions** by building a full pipeline: 

> lexer → parser → AST → code generator.

## Features

- Recursive-descent parser
- Explicit AST node hierarchy
- Deterministic code generation
- Clear error reporting with line numbers

## Build & Run

Requirements:
- C++23 compiler

Build:
```bash
cmake -S . -B build
cmake --build build
```

Run:
```bash
./build/Prometheus examples/hello.pr
```
## Architecture

The compiler is organised into four main stages:

- Lexer: converts input text into tokens
- Parser: builds the AST from tokens
- Semantic phase: validates symbol usage
- Generator: emits target instructions

<img src="assets/architecture.png" alt="Alt text" />   
