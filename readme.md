# Garter Assembler

`gasm` is the intermediate language and assembly language
of the [`garter`](https://github.com/topics/garter) compiler toolchain.
The `gasm` assembly syntax is intended to be platform-agnostic wherever possible.

## Usage

the Garter Assembler takes in `*.gasm` files and generates
`*.gyb` [object files](https://github.com/Modula-dev/gyb).

## Implementation

Lexical analysis breaks apart the source files
into tokens: `keywords`, `labels`/`addresses`, `constants`,
et cetera. Then the parser iterates over the list of tokens
and essentially "interprets" them --
instructions will either change the assembler's internal state,
or be translated and treated as emitter commands.

## How does Object Emission Work?

When the assembler first starts,
it initializes an empty object with
a clean-slate symbol table.
Any time we encounter symbol definitions,
we push those to our symbol table.

Later, somewhere in our `*.gasm` file,
we might see a block of code that looks like
```asm
section executable
    move rax, rax
```
`section executable` is a directive that
changes what section the emitter is currently
sending data into,
then the instruction `move rax, rax`
will be converted into data and put
into the selected section.

# GASM Assembly

`gasm` is heavily influenced by x86 Intel Assembly,
FlatAssembler, BASIC, and Python, and looks something like:
```asm
section .w
    hello
    u8 "Hello, World" 0
section .x
    set ar hello
    set br 12
    syscall write stdout ar br
    set ar 0
    syscall exit ar
```

For tutorials, see the `garter` website.
For a guide on writing Garter Assembly,
see the `garter` specification doc.