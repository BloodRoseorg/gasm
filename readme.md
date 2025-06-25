# Garter Assembler

`gasm` is the intermediate language and assembly language
of the [garter](https://github.com/topics/garter) compiler toolchain.
`gasm` syntax is intended to be platform-agnostic wherever possible.
To this end, it sometimes will have "redundancies" like `mod` to x86, 
and will sometimes be "missing" instructions or registers that are platform-specific.

For systems that may not have support for an instruction, such as systems which do not have floating-point registers and arithmetic, the assembler should inject an "inline library" of sorts that emulates the behavior on the target.

For systems which have a simpler or more direct way of handling some series of instructions, it is okay for the assembler to optimize them _as long as optimizations are turned on_ <!-- TODO -->


## Usage

the Garter Assembler takes in `*.gasm` files and generates
`*.gyb` [object files](https://github.com/BloodRoseorg/gyb).

## Implementation

Lexical analysis breaks apart the source files
into tokens: `keywords`, `labels`/`addresses`, `constants`,
et cetera. Then the parser iterates over the list of tokens
and essentially "interprets" them --
instructions will either change the assembler's internal state,
or be translated and treated as emitter commands.

Offsets into the executable section are interpreted as
_number of instructions past section start_,
whereas other offsets are interpreted as 
_number of bytes past section start_.
Making this distinction simplifies the
emission stage, but also means that edge cases,
such as dealing with _data in an executable section_,
could be problematic.

## How does Object Emission Work?

When the assembler first starts,
it initializes an empty object with
a clean-slate symbol table.
Any time we encounter symbol definitions,
we push those to our symbol table.
Any time we encounter instructions,
we push those to the data sector.
The symbol table will contain offsets within each sector,
and when converting the object to a raw binary,
the offsets will be recalculated during emission.

# GASM Assembly

`gasm` is heavily influenced by x86 Intel Assembly,
FlatAssembler, BASIC, and Python, and looks something like:
```asm
writable origin 0x84006000
    hello
    u8 "Hello, World" 0
executable origin 0x84000000
    set ar hello
    set br 12
    syscall write stdout ar br
    set ar 0
    syscall exit ar
```

For tutorials, see the `garter` website.
For a guide on writing Garter Assembly,
see the `garter` specification doc.