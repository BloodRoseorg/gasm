# Garter Assembly

Garter Assembly is intended to be much more
literal and one-to-one with the emitted machine code,
while also having syntax that makes it easy to read and write,
and having syntax that is _generally_ agnostic of the platform it is running on.
Every instruction keyword maps to one
_and only one_ instruction, unlike in many other assembly dialects,
and besides a simple preprocessor,
this assembler is just that; 
a simple assembler intended to be _trivial_
to port and extend to other systems.

```c
#define SYS_EXIT 60
section text x
run                     ; this is redundant, but can be explicitly stated
entry main
    set ar 0xFFFF
_countdown
    sub ar 1
    cmp ar cr
    gt _countdown
break
    set ar SYS_EXIT
    syscall
```

> [!NOTE] All two-letter name symbol names 
> are reserved by the assembler and may not be reused.

# Syntax

Some rules:
- Preprocessor directives belong on their own lines.
- Instructions also belong on their own line.
- Multiple symbol definitions and pseudoinstructions
can be on the same line
- Except newlines, whitespace is ignored

This is valid Garter Assembly with preprocessor directives enabled
```asm
#define SYS_R0      ar
#define SYS_R1      cr
#define SYS_R2      dr
#define SYS_R3      sr
#define STATUS_OK   0

#include "syscalls.asm"

section text x
entry main exe                          ; exe is optional here
    set SYS_R0 SYS_EXIT                 ; assuming SYS_EXIT is in syscalls.asm
    pop SYS_R1
    syscall
```


## Register Names

| Register | x86 Equiv | ARM Equiv |
|--    |--  |--|
| ar   | ax | r0  |
| br   | bx | r1  |
| cr   | cx | r2  |
| dr   | dx | r3  |
| sp   | sp | sp  | 
| bp   | bp | r7  |
| ip   | ip | pc  |
| xr   | xmm0 | s1 |
| yr   | xmm1 | s2 |
| \*sr | esi | -- |

\* _sr_ is specific to x86 and only included for compatibility

Some registers like the `lr` on ARM systems is not included here
because 
1.  it is extremely platform specific,
2.  its use is very specific to the `call`/`ret` implementation and ABI,
    and exposing it contradicts the goals of this project

## Pseudoinstructions

| Keyword | Description |
|--|--|
| origin      | changes section offset |
| exe         | _variable write size based on opcode_ |
| byte        | 8-bit data  |
| word        | 16-bit data |
| short       | 32-bit data |
| long        | 64-bit data |
| entry       | a unique label saying where execution should begin |
| section     | `section {name} {permissions}` |
| r           | read-only section |
| w           | read+write section |
| x           | executable section |

The following exist as alias to the basic data sizes:
- byte:     `u8` `i8` `char`
- word:     `u16` `i16`
- short:    `u32` `i32` `dword` `single` `f32`
- long:     `u64` `i64` `qword` `float` `double` `f64`

These aliases are also given for section types:
- r:        `readable` `readonly` `ro`
- w:        `writable` `rw`
- x:        `executable` `x` `rx`

## Integer Arithmetic

| Keyword | Notes |
|--|--|
| add   |
| sub   | 
| mul   | signed |
| div   | signed |
| and   |
| or    |
| xor   |
| left  |
| right |
| flip  | nasm: `neg x`, C: `~x` |
| push  |
| pop   |
| move  | register-to-register |
| load  | register to address in register |
| store | register from address in register |
| set   | register to literal value |

## Float Arithmetic

| Keyword | Notes |
|--|--|
| imove | bitwise move from int to float register |
| fmove | bitwise move from float to int register |
| icast | cast integer to float |
| fcast | cast float to integer |
| fadd   |
| fsub   | 
| fmul   |
| fdiv   |

## Branching Instructions

| Keyword | Notes |
|--|--|
| compare / cmp |
| fcompare / fcmp |
| gotor | jump to address in register |
| goto  | jump to literal address |
| if |
| not |
| lt | unsigned |
| le |
| gt |
| ge |
| lts | signed |
| les |
| gts |
| ges |

## System Instructions

| Keyword | Notes |
|--|--|
| interrupt | used for hardware interrupts |
| syscall | used for calling system services |

## Inline Arithmetic

Some limited arithmetic is allowed in Garter Assembly,
but only literals (addresses and immediate values) can be involved.
They are always treated as unsigned integers in this context,
and are simply evaluated left-to-right.
Parenthesis/enveloping are not supported.

Inline Arithmetic supports the following instructions:

- Basic integer maths `+` `-` `*` `/` `%`
- Bitwise maths `<<` `>>` `&` `|` `^` `~`

For example, the following are valid:

```asm
    set ar label + 4
    set br value * constant
```
but this would be invalid
```
    set bp sp << 4 ; sp is not constant at compile-time
```


# Implementation

## _Pass 0\*_

Run the preprocessor on our file before doing any parsing

_\* Not applicable to the bootstrapping assembler_

## Pass 1

Go through our file with a _state machine_-like parsing system,
noting when sections and data emission size/type changes,
and tracking how _far_ into each section our "write-head" would be.
This stage needs to know how large each emission type is,
but does not actually write any data or assemble instructions yet.
As it goes, it defines any symbols.

### Symbol Definitions

Each symbol has a section and then literal data associate with it.
There are also reserved sections for `UNDEFINED`, `UNRESOLVED`, and `LITERAL DATA`.

### Emission Stack

For each section, we have an _emission stack_
which tells the emitter when it needs to change the writing type
and what type it will change to.

### Pass 2

Goes back through each section and, 
using the notes from before about where data are 
and when the "write-head" needs to change size or move to a new section,
it begins the actually emission to file.
Any undefined symbols would get caught during this pass.
