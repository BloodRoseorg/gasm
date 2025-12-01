typedef uint symbol_t;
typedef struct { u8 opcode; u8 source; uint adder; } instruction_t;
typedef struct { symbol_t id; char section; uint value; } data_t;
typedef struct { uint section: 8; uint data: 24; } address_t;
typedef char lookup_t[8][8][8];

#define UNDEFINED               0
#define UNRESOLVED              -2
#define LITERAL                 -1
#define ERRNO                   -1
#define symbol_table_size       ( 1024 * 1024 * 4 )
#define symbol_overflow_size    0xFF
#define sections_max            0xFF
#define stack_size              1024 * 4

typedef struct { uint index; uint data[stack_size]; } stack_t;

data_t symbol_table[ symbol_table_size ] = { UNDEFINED };
data_t symbol_overflow[symbol_overflow_size] = { UNDEFINED };
stack_t emit_stack[sections_max] = { UNDEFINED };

#define emit_type ( type ) { emitter_type = type; }
#define emit_section ( section ) { emitter_section = section; }
#define emit_nop "\x90"

char emitter_section =  UNDEFINED;
char emitter_type =     UNDEFINED;
uint emitter_offset =   UNDEFINED;

enum {
    emitter_undefined = UNDEFINED, emitter_run, 
    emitter_byte, emitter_word, emitter_short, emitter_long
};

enum {
    opcode_undefined = UNDEFINED, 
    opcode_compare, opcode_fcompare,
    opcode_add, opcode_sub, opcode_mul, opcode_div, // opcode_modulo,
    opcode_flip, opcode_and, opcode_or, opcode_xor, 
    opcode_left, opcode_right, opcode_push, opcode_pop,
    opcode_move, opcode_load, opcode_store, opcode_set,
    opcode_imove, opcode_icast, opcode_fmove, opcode_fcast,
    opcode_fadd, opcode_fsub, opcode_fmul, opcode_fdiv,
    opcode_gotor, opcode_goto, opcode_if, opcode_not, 
    opcode_lt, opcode_le, opcode_gt, opcode_ge,
    opcode_lts, opcode_les, opcode_gts, opcode_ges,
    opcode_interrupt, opcode_syscall,
};

// exclude gotor because it does not include a literal
#define opcode_branch_start         opcode_goto
#define opcode_branch_end           opcode_ges

enum {
    ar, br, cr, dr, sp, bp, ip, sr
};

/*
    https://shell-storm.org/online/Online-Assembler-and-Disassembler
    https://defuse.ca/online-x86-assembler.htm
*/

char emitter_size[] = {
    [opcode_compare] =  2,
    [opcode_fcompare] = UNRESOLVED,
    [opcode_add] =      2,
    [opcode_sub] =      2,
    [opcode_mul] =      3,
    [opcode_div] =      2,
    [opcode_flip] =     2,
    [opcode_and] =      2,
    [opcode_or] =       2,
    [opcode_xor] =      2,
    [opcode_left] =     6,
    [opcode_right] =    6,
    [opcode_push] =     1,
    [opcode_pop] =      1,
    [opcode_move] =     2,
    [opcode_load] =     3,
    [opcode_store] =    3,
    [opcode_set] =      1, // ++LITERAL u32
    [opcode_imove] =    4,
    [opcode_icast] =    UNRESOLVED,
    [opcode_fmove] =    4,
    [opcode_fcast] =    UNRESOLVED,
    [opcode_fadd] =     4,
    [opcode_fsub] =     4,
    [opcode_fmul] =     4,
    [opcode_fdiv] =     4,
    [opcode_gotor] =    2,
    [opcode_goto] =     1, // ++LITERAL u32 -4
    [opcode_if] =       2, // ++LITERAL u32 -4
    [opcode_not] =      2, // ++LITERAL u32 -4
    [opcode_lt] =       2, // ++LITERAL u32 -4
    [opcode_le] =       2, // ++LITERAL u32 -4
    [opcode_gt] =       2, // ++LITERAL u32 -4
    [opcode_ge] =       2, // ++LITERAL u32 -4
    [opcode_lts] =      2, // ++LITERAL u32 -4
    [opcode_les] =      2, // ++LITERAL u32 -4
    [opcode_gts] =      2, // ++LITERAL u32 -4
    [opcode_ges] =      2, // ++LITERAL u32 -4
    [opcode_interrupt]= 1, // ++LITERAL u8
    [opcode_syscall] =  2
};

lookup_t emitter_lookup[] = {
    [opcode_compare] = {
        [ar][ar] = "\x39\xc0",
        [ar][br] = "\x39\xd8",
        [ar][cr] = "\x39\xc8",
        [ar][dr] = "\x39\xd0",
        [ar][sp] = "\x39\xe0",
        [ar][bp] = "\x39\xe8",
        [ar][sr] = "\x39\xf0",

        [br][ar] = "\x39\xc3",
        [br][br] = "\x39\xdb",
        [br][cr] = "\x39\xcb",
        [br][dr] = "\x39\xd3",
        [br][sp] = "\x39\xe3",
        [br][bp] = "\x39\xeb",
        [br][sr] = "\x39\xf3",

        [cr][ar] = "\x39\xc1",
        [cr][br] = "\x39\xd9",
        [cr][cr] = "\x39\xc9",
        [cr][dr] = "\x39\xd1",
        [cr][sp] = "\x39\xe1",
        [cr][bp] = "\x39\xe9",
        [cr][sr] = "\x39\xf1",

        [dr][ar] = "\x39\xc2",
        [dr][br] = "\x39\xda",
        [dr][cr] = "\x39\xca",
        [dr][dr] = "\x39\xd2",
        [dr][sp] = "\x39\xe2",
        [dr][bp] = "\x39\xea",
        [dr][sr] = "\x39\xf2",

        [sp][ar] = "\x39\xc4",
        [sp][br] = "\x39\xdc",
        [sp][cr] = "\x39\xcc",
        [sp][dr] = "\x39\xd4",
        [sp][sp] = "\x39\xe4",
        [sp][bp] = "\x39\xec",
        [sp][sr] = "\x39\xf4",

        [bp][ar] = "\x39\xc5",
        [bp][br] = "\x39\xdd",
        [bp][cr] = "\x39\xcd",
        [bp][dr] = "\x39\xd5",
        [bp][sp] = "\x39\xe5",
        [bp][bp] = "\x39\xed",
        [bp][sr] = "\x39\xf5",

        [sr][ar] = "\x39\xc6",
        [sr][br] = "\x39\xde",
        [sr][cr] = "\x39\xce",
        [sr][dr] = "\x39\xd6",
        [sr][sp] = "\x39\xe6",
        [sr][bp] = "\x39\xee",
        [sr][sr] = "\x39\xf6",
    },

    // TODO: fcompare

    [opcode_add] = {
        [ar][ar] = "\x01\xc0",
        [ar][br] = "\x01\xd8",
        [ar][cr] = "\x01\xc8",
        [ar][dr] = "\x01\xd0",
        [ar][sp] = "\x01\xe0",
        [ar][bp] = "\x01\xe8",
        [ar][sr] = "\x01\xf0",

        [br][ar] = "\x01\xc3",
        [br][br] = "\x01\xdb",
        [br][cr] = "\x01\xcb",
        [br][dr] = "\x01\xd3",
        [br][sp] = "\x01\xe3",
        [br][bp] = "\x01\xeb",
        [br][sr] = "\x01\xf3",

        [cr][ar] = "\x01\xc1",
        [cr][br] = "\x01\xd9",
        [cr][cr] = "\x01\xc9",
        [cr][dr] = "\x01\xd1",
        [cr][sp] = "\x01\xe1",
        [cr][bp] = "\x01\xe9",
        [cr][sr] = "\x01\xf1",

        [dr][ar] = "\x01\xc2",
        [dr][br] = "\x01\xda",
        [dr][cr] = "\x01\xca",
        [dr][dr] = "\x01\xd2",
        [dr][sp] = "\x01\xe2",
        [dr][bp] = "\x01\xea",
        [dr][sr] = "\x01\xf2",

        [sp][ar] = "\x01\xc4",
        [sp][br] = "\x01\xdc",
        [sp][cr] = "\x01\xcc",
        [sp][dr] = "\x01\xd4",
        [sp][sp] = "\x01\xe4",
        [sp][bp] = "\x01\xec",
        [sp][sr] = "\x01\xf4",

        [bp][ar] = "\x01\xc5",
        [bp][br] = "\x01\xdd",
        [bp][cr] = "\x01\xcd",
        [bp][dr] = "\x01\xd5",
        [bp][sp] = "\x01\xe5",
        [bp][bp] = "\x01\xed",
        [bp][sr] = "\x01\xf5",

        [sr][ar] = "\x01\xc6",
        [sr][br] = "\x01\xde",
        [sr][cr] = "\x01\xce",
        [sr][dr] = "\x01\xd6",
        [sr][sp] = "\x01\xe6",
        [sr][bp] = "\x01\xee",
        [sr][sr] = "\x01\xf6",
    },
    [opcode_sub] = {
        [ar][ar] = "\x29\xc0",
        [ar][br] = "\x29\xd8",
        [ar][cr] = "\x29\xc8",
        [ar][dr] = "\x29\xd0",
        [ar][sp] = "\x29\xe0",
        [ar][bp] = "\x29\xe8",
        [ar][sr] = "\x29\xf0",

        [br][ar] = "\x29\xc3",
        [br][br] = "\x29\xdb",
        [br][cr] = "\x29\xcb",
        [br][dr] = "\x29\xd3",
        [br][sp] = "\x29\xe3",
        [br][bp] = "\x29\xeb",
        [br][sr] = "\x29\xf3",

        [cr][ar] = "\x29\xc1",
        [cr][br] = "\x29\xd9",
        [cr][cr] = "\x29\xc9",
        [cr][dr] = "\x29\xd1",
        [cr][sp] = "\x29\xe1",
        [cr][bp] = "\x29\xe9",
        [cr][sr] = "\x29\xf1",

        [dr][ar] = "\x29\xc2",
        [dr][br] = "\x29\xda",
        [dr][cr] = "\x29\xca",
        [dr][dr] = "\x29\xd2",
        [dr][sp] = "\x29\xe2",
        [dr][bp] = "\x29\xea",
        [dr][sr] = "\x29\xf2",

        [sp][ar] = "\x29\xc4",
        [sp][br] = "\x29\xdc",
        [sp][cr] = "\x29\xcc",
        [sp][dr] = "\x29\xd4",
        [sp][sp] = "\x29\xe4",
        [sp][bp] = "\x29\xec",
        [sp][sr] = "\x29\xf4",

        [bp][ar] = "\x29\xc5",
        [bp][br] = "\x29\xdd",
        [bp][cr] = "\x29\xcd",
        [bp][dr] = "\x29\xd5",
        [bp][sp] = "\x29\xe5",
        [bp][bp] = "\x29\xed",
        [bp][sr] = "\x29\xf5",

        [sr][ar] = "\x29\xc6",
        [sr][br] = "\x29\xde",
        [sr][cr] = "\x29\xce",
        [sr][dr] = "\x29\xd6",
        [sr][sp] = "\x29\xe6",
        [sr][bp] = "\x29\xee",
        [sr][sr] = "\x29\xf6",
    },
    [opcode_mul] = {
        [ar][ar] = "\x0f\xaf\xc0",
        [ar][br] = "\x0f\xaf\xc3",
        [ar][cr] = "\x0f\xaf\xc1",
        [ar][dr] = "\x0f\xaf\xc2",
        [ar][sp] = "\x0f\xaf\xc4",
        [ar][bp] = "\x0f\xaf\xc5",
        [ar][sr] = "\x0f\xaf\xc6",

        [br][ar] = "\x0f\xaf\xd8",
        [br][br] = "\x0f\xaf\xdb",
        [br][cr] = "\x0f\xaf\xd9",
        [br][dr] = "\x0f\xaf\xda",
        [br][sp] = "\x0f\xaf\xdc",
        [br][bp] = "\x0f\xaf\xdd",
        [br][sr] = "\x0f\xaf\xde",

        [cr][ar] = "\x0f\xaf\xc8",
        [cr][br] = "\x0f\xaf\xcb",
        [cr][cr] = "\x0f\xaf\xc9",
        [cr][dr] = "\x0f\xaf\xca",
        [cr][sp] = "\x0f\xaf\xcc",
        [cr][bp] = "\x0f\xaf\xcd",
        [cr][sr] = "\x0f\xaf\xce",

        [dr][ar] = "\x0f\xaf\xd0",
        [dr][br] = "\x0f\xaf\xd3",
        [dr][cr] = "\x0f\xaf\xd1",
        [dr][dr] = "\x0f\xaf\xd2",
        [dr][sp] = "\x0f\xaf\xd4",
        [dr][bp] = "\x0f\xaf\xd5",
        [dr][sr] = "\x0f\xaf\xd6",

        [sp][ar] = "\x0f\xaf\xe0",
        [sp][br] = "\x0f\xaf\xe3",
        [sp][cr] = "\x0f\xaf\xe1",
        [sp][dr] = "\x0f\xaf\xe2",
        [sp][sp] = "\x0f\xaf\xe4",
        [sp][bp] = "\x0f\xaf\xe5",
        [sp][sr] = "\x0f\xaf\xe6",

        [bp][ar] = "\x0f\xaf\xe8",
        [bp][br] = "\x0f\xaf\xeb",
        [bp][cr] = "\x0f\xaf\xe9",
        [bp][dr] = "\x0f\xaf\xea",
        [bp][sp] = "\x0f\xaf\xec",
        [bp][bp] = "\x0f\xaf\xed",
        [bp][sr] = "\x0f\xaf\xee",

        [sr][ar] = "\x0f\xaf\xf0",
        [sr][br] = "\x0f\xaf\xf3",
        [sr][cr] = "\x0f\xaf\xf1",
        [sr][dr] = "\x0f\xaf\xf2",
        [sr][sp] = "\x0f\xaf\xf4",
        [sr][bp] = "\x0f\xaf\xf5",
        [sr][sr] = "\x0f\xaf\xf6",
    },
    [opcode_div] = {
        [ar][ar] = "\xf7\xf8",
        [br][br] = "\xf7\xf9",
        [cr][cr] = "\xf7\xfa",
        [dr][dr] = "\xf7\xfb",
        [sp][sp] = "\xf7\xfc",
        [bp][bp] = "\xf7\xfd",
        [sr][sr] = "\xf7\xfe",
    },
    [opcode_flip] = {
        [ar][ar] = "\xf7\xd8",
        [br][br] = "\xf7\xd9",
        [cr][cr] = "\xf7\xda",
        [dr][dr] = "\xf7\xdb",
        [sp][sp] = "\xf7\xdc",
        [bp][bp] = "\xf7\xdd",
        [sr][sr] = "\xf7\xde",
    },
    [opcode_and] = {
        [ar][ar] = "\x21\xc0",
        [ar][br] = "\x21\xd8",
        [ar][cr] = "\x21\xc8",
        [ar][dr] = "\x21\xd0",
        [ar][sp] = "\x21\xe0",
        [ar][bp] = "\x21\xe8",
        [ar][sr] = "\x21\xf0",

        [br][ar] = "\x21\xc3",
        [br][br] = "\x21\xdb",
        [br][cr] = "\x21\xcb",
        [br][dr] = "\x21\xd3",
        [br][sp] = "\x21\xe3",
        [br][bp] = "\x21\xeb",
        [br][sr] = "\x21\xf3",

        [cr][ar] = "\x21\xc1",
        [cr][br] = "\x21\xd9",
        [cr][cr] = "\x21\xc9",
        [cr][dr] = "\x21\xd1",
        [cr][sp] = "\x21\xe1",
        [cr][bp] = "\x21\xe9",
        [cr][sr] = "\x21\xf1",

        [dr][ar] = "\x21\xc2",
        [dr][br] = "\x21\xda",
        [dr][cr] = "\x21\xca",
        [dr][dr] = "\x21\xd2",
        [dr][sp] = "\x21\xe2",
        [dr][bp] = "\x21\xea",
        [dr][sr] = "\x21\xf2",

        [sp][ar] = "\x21\xc4",
        [sp][br] = "\x21\xdc",
        [sp][cr] = "\x21\xcc",
        [sp][dr] = "\x21\xd4",
        [sp][sp] = "\x21\xe4",
        [sp][bp] = "\x21\xec",
        [sp][sr] = "\x21\xf4",

        [bp][ar] = "\x21\xc5",
        [bp][br] = "\x21\xdd",
        [bp][cr] = "\x21\xcd",
        [bp][dr] = "\x21\xd5",
        [bp][sp] = "\x21\xe5",
        [bp][bp] = "\x21\xed",
        [bp][sr] = "\x21\xf5",

        [sr][ar] = "\x21\xc6",
        [sr][br] = "\x21\xde",
        [sr][cr] = "\x21\xce",
        [sr][dr] = "\x21\xd6",
        [sr][sp] = "\x21\xe6",
        [sr][bp] = "\x21\xee",
        [sr][sr] = "\x21\xf6",
    },
    [opcode_or] = {
        [ar][ar] = "\x09\xc0",
        [ar][br] = "\x09\xd8",
        [ar][cr] = "\x09\xc8",
        [ar][dr] = "\x09\xd0",
        [ar][sp] = "\x09\xe0",
        [ar][bp] = "\x09\xe8",
        [ar][sr] = "\x09\xf0",

        [br][ar] = "\x09\xc3",
        [br][br] = "\x09\xdb",
        [br][cr] = "\x09\xcb",
        [br][dr] = "\x09\xd3",
        [br][sp] = "\x09\xe3",
        [br][bp] = "\x09\xeb",
        [br][sr] = "\x09\xf3",

        [cr][ar] = "\x09\xc1",
        [cr][br] = "\x09\xd9",
        [cr][cr] = "\x09\xc9",
        [cr][dr] = "\x09\xd1",
        [cr][sp] = "\x09\xe1",
        [cr][bp] = "\x09\xe9",
        [cr][sr] = "\x09\xf1",

        [dr][ar] = "\x09\xc2",
        [dr][br] = "\x09\xda",
        [dr][cr] = "\x09\xca",
        [dr][dr] = "\x09\xd2",
        [dr][sp] = "\x09\xe2",
        [dr][bp] = "\x09\xea",
        [dr][sr] = "\x09\xf2",

        [sp][ar] = "\x09\xc4",
        [sp][br] = "\x09\xdc",
        [sp][cr] = "\x09\xcc",
        [sp][dr] = "\x09\xd4",
        [sp][sp] = "\x09\xe4",
        [sp][bp] = "\x09\xec",
        [sp][sr] = "\x09\xf4",

        [bp][ar] = "\x09\xc5",
        [bp][br] = "\x09\xdd",
        [bp][cr] = "\x09\xcd",
        [bp][dr] = "\x09\xd5",
        [bp][sp] = "\x09\xe5",
        [bp][bp] = "\x09\xed",
        [bp][sr] = "\x09\xf5",

        [sr][ar] = "\x09\xc6",
        [sr][br] = "\x09\xde",
        [sr][cr] = "\x09\xce",
        [sr][dr] = "\x09\xd6",
        [sr][sp] = "\x09\xe6",
        [sr][bp] = "\x09\xee",
        [sr][sr] = "\x09\xf6",
    },
    [opcode_xor] = {
        [ar][ar] = "\x31\xc0",
        [ar][br] = "\x31\xd8",
        [ar][cr] = "\x31\xc8",
        [ar][dr] = "\x31\xd0",
        [ar][sp] = "\x31\xe0",
        [ar][bp] = "\x31\xe8",
        [ar][sr] = "\x31\xf0",

        [br][ar] = "\x31\xc3",
        [br][br] = "\x31\xdb",
        [br][cr] = "\x31\xcb",
        [br][dr] = "\x31\xd3",
        [br][sp] = "\x31\xe3",
        [br][bp] = "\x31\xeb",
        [br][sr] = "\x31\xf3",

        [cr][ar] = "\x31\xc1",
        [cr][br] = "\x31\xd9",
        [cr][cr] = "\x31\xc9",
        [cr][dr] = "\x31\xd1",
        [cr][sp] = "\x31\xe1",
        [cr][bp] = "\x31\xe9",
        [cr][sr] = "\x31\xf1",

        [dr][ar] = "\x31\xc2",
        [dr][br] = "\x31\xda",
        [dr][cr] = "\x31\xca",
        [dr][dr] = "\x31\xd2",
        [dr][sp] = "\x31\xe2",
        [dr][bp] = "\x31\xea",
        [dr][sr] = "\x31\xf2",

        [sp][ar] = "\x31\xc4",
        [sp][br] = "\x31\xdc",
        [sp][cr] = "\x31\xcc",
        [sp][dr] = "\x31\xd4",
        [sp][sp] = "\x31\xe4",
        [sp][bp] = "\x31\xec",
        [sp][sr] = "\x31\xf4",

        [bp][ar] = "\x31\xc5",
        [bp][br] = "\x31\xdd",
        [bp][cr] = "\x31\xcd",
        [bp][dr] = "\x31\xd5",
        [bp][sp] = "\x31\xe5",
        [bp][bp] = "\x31\xed",
        [bp][sr] = "\x31\xf5",

        [sr][ar] = "\x31\xc6",
        [sr][br] = "\x31\xde",
        [sr][cr] = "\x31\xce",
        [sr][dr] = "\x31\xd6",
        [sr][sp] = "\x31\xe6",
        [sr][bp] = "\x31\xee",
        [sr][sr] = "\x31\xf6",
    },
    /* MACRO: Shift Left
        push ECX
        move %2 into ECX
        shl eax ecx
        pop ECX
    */
    [opcode_left] = {
        [ar][ar] = "\x51\x89\xC1\xD3\xE0\x59",
        [ar][br] = "\x51\x89\xD9\xD3\xE0\x59",
        [ar][cr] = "\x51\x89\xC9\xD3\xE0\x59", // redundant `mov ecx, ecx`
        [ar][dr] = "\x51\x89\xD1\xD3\xE0\x59",
        [ar][sp] = "\x51\x89\xE1\xD3\xE0\x59",
        [ar][bp] = "\x51\x89\xE9\xD3\xE0\x59",
        [ar][sr] = "\x51\x89\xF1\xD3\xE0\x59"
    },
    [opcode_right] = {
        [ar][ar] = "\x51\x89\xC1\xD3\xE8\x59",
        [ar][br] = "\x51\x89\xD9\xD3\xE8\x59",
        [ar][cr] = "\x51\x89\xC9\xD3\xE8\x59", // redundant `mov ecx, ecx`
        [ar][dr] = "\x51\x89\xD1\xD3\xE8\x59",
        [ar][sp] = "\x51\x89\xE1\xD3\xE8\x59",
        [ar][bp] = "\x51\x89\xE9\xD3\xE8\x59",
        [ar][sr] = "\x51\x89\xF1\xD3\xE8\x59"
    },
    [opcode_push] = {
        [ar][UNDEFINED] = "\x50",
        [br][UNDEFINED] = "\x53",
        [cr][UNDEFINED] = "\x51",
        [dr][UNDEFINED] = "\x52",
        [sp][UNDEFINED] = "\x54",
        [bp][UNDEFINED] = "\x55",
        [sr][UNDEFINED] = "\x56",
    },
    [opcode_pop] = {
        [ar][UNDEFINED] = "\x58",
        [br][UNDEFINED] = "\x5b",
        [cr][UNDEFINED] = "\x59",
        [dr][UNDEFINED] = "\x5a",
        [sp][UNDEFINED] = "\x5c",
        [bp][UNDEFINED] = "\x5d",
        [sr][UNDEFINED] = "\x5e",
    },
    [opcode_move] = {
        [ar][ar] = "\x89\xc0", 
        [ar][br] = "\x89\xd8", 
        [ar][cr] = "\x89\xc8", 
        [ar][dr] = "\x89\xd0", 
        [ar][sp] = "\x89\xe0", 
        [ar][bp] = "\x89\xe8", 
        [ar][sr] = "\x89\xf0",

        [br][ar] = "\x89\xc3", 
        [br][br] = "\x89\xdb", 
        [br][cr] = "\x89\xcb", 
        [br][dr] = "\x89\xd3", 
        [br][sp] = "\x89\xe3", 
        [br][bp] = "\x89\xeb", 
        [br][sr] = "\x89\xf3",

        [cr][ar] = "\x89\xc1", 
        [cr][br] = "\x89\xd9", 
        [cr][cr] = "\x89\xc9", 
        [cr][dr] = "\x89\xd1", 
        [cr][sp] = "\x89\xe1", 
        [cr][bp] = "\x89\xe9", 
        [cr][sr] = "\x89\xf1",

        [dr][ar] = "\x89\xc2", 
        [dr][br] = "\x89\xda", 
        [dr][cr] = "\x89\xca", 
        [dr][dr] = "\x89\xd2", 
        [dr][sp] = "\x89\xe2", 
        [dr][bp] = "\x89\xea", 
        [dr][sr] = "\x89\xf2",

        [sp][ar] = "\x89\xc4", 
        [sp][br] = "\x89\xdc", 
        [sp][cr] = "\x89\xcc", 
        [sp][dr] = "\x89\xd4", 
        [sp][sp] = "\x89\xe4", 
        [sp][bp] = "\x89\xec", 
        [sp][sr] = "\x89\xf4",

        [bp][ar] = "\x89\xc5", 
        [bp][br] = "\x89\xdd", 
        [bp][cr] = "\x89\xcd", 
        [bp][dr] = "\x89\xd5", 
        [bp][sp] = "\x89\xe5", 
        [bp][bp] = "\x89\xed", 
        [bp][sr] = "\x89\xf5",

        [sr][ar] = "\x89\xc6", 
        [sr][br] = "\x89\xde", 
        [sr][cr] = "\x89\xce", 
        [sr][dr] = "\x89\xd6", 
        [sr][sp] = "\x89\xe6", 
        [sr][bp] = "\x89\xee", 
        [sr][sr] = "\x89\xf6",
    },
    [opcode_load] = {
        [ar][ar] = "\x8b\x00" emit_nop,
        [ar][br] = "\x8b\x03" emit_nop,
        [ar][cr] = "\x8b\x01" emit_nop,
        [ar][dr] = "\x8b\x02" emit_nop,
        [ar][sp] = "\x8b\x04\x24",
        [ar][bp] = "\x8b\x45\x00",
        [ar][sr] = "\x8b\x06" emit_nop,

        [br][ar] = "\x8b\x18" emit_nop,
        [br][br] = "\x8b\x1b" emit_nop,
        [br][cr] = "\x8b\x19" emit_nop,
        [br][dr] = "\x8b\x1a" emit_nop,
        [br][sp] = "\x8b\x1c\x24",
        [br][bp] = "\x8b\x5d\x00",
        [br][sr] = "\x8b\x1e" emit_nop,

        [cr][ar] = "\x8b\x08" emit_nop,
        [cr][br] = "\x8b\x0b" emit_nop,
        [cr][cr] = "\x8b\x09" emit_nop,
        [cr][dr] = "\x8b\x0a" emit_nop,
        [cr][sp] = "\x8b\x0c\x24",
        [cr][bp] = "\x8b\x4d\x00",
        [cr][sr] = "\x8b\x0e" emit_nop,

        [dr][ar] = "\x8b\x10" emit_nop,
        [dr][br] = "\x8b\x13" emit_nop,
        [dr][cr] = "\x8b\x11" emit_nop,
        [dr][dr] = "\x8b\x12" emit_nop,
        [dr][sp] = "\x8b\x14\x24",
        [dr][bp] = "\x8b\x55\x00",
        [dr][sr] = "\x8b\x16" emit_nop,

        [sp][ar] = "\x8b\x20" emit_nop,
        [sp][br] = "\x8b\x23" emit_nop,
        [sp][cr] = "\x8b\x21" emit_nop,
        [sp][dr] = "\x8b\x22" emit_nop,
        [sp][sp] = "\x8b\x24\x24",
        [sp][bp] = "\x8b\x65\x00",
        [sp][sr] = "\x8b\x26" emit_nop,

        [bp][ar] = "\x8b\x28" emit_nop,
        [bp][br] = "\x8b\x2b" emit_nop,
        [bp][cr] = "\x8b\x29" emit_nop,
        [bp][dr] = "\x8b\x2a" emit_nop,
        [bp][sp] = "\x8b\x2c\x24",
        [bp][bp] = "\x8b\x6d\x00",
        [bp][sr] = "\x8b\x2e" emit_nop,

        [sr][ar] = "\x8b\x30" emit_nop,
        [sr][br] = "\x8b\x33" emit_nop,
        [sr][cr] = "\x8b\x31" emit_nop,
        [sr][dr] = "\x8b\x32" emit_nop,
        [sr][sp] = "\x8b\x34\x24",
        [sr][bp] = "\x8b\x75\x00",
        [sr][sr] = "\x8b\x36" emit_nop,
    },
    [opcode_store] = {
        [ar][ar] = "\x89\x00" emit_nop,
        [ar][br] = "\x89\x18" emit_nop,
        [ar][cr] = "\x89\x08" emit_nop,
        [ar][dr] = "\x89\x10" emit_nop,
        [ar][sp] = "\x89\x20" emit_nop,
        [ar][bp] = "\x89\x28" emit_nop,
        [ar][sr] = "\x89\x30" emit_nop,

        [br][ar] = "\x89\x03" emit_nop,
        [br][br] = "\x89\x1b" emit_nop,
        [br][cr] = "\x89\x0b" emit_nop,
        [br][dr] = "\x89\x13" emit_nop,
        [br][sp] = "\x89\x23" emit_nop,
        [br][bp] = "\x89\x2b" emit_nop,
        [br][sr] = "\x89\x33" emit_nop,

        [cr][ar] = "\x89\x01" emit_nop,
        [cr][br] = "\x89\x19" emit_nop,
        [cr][cr] = "\x89\x09" emit_nop,
        [cr][dr] = "\x89\x11" emit_nop,
        [cr][sp] = "\x89\x21" emit_nop,
        [cr][bp] = "\x89\x29" emit_nop,
        [cr][sr] = "\x89\x31" emit_nop,

        [dr][ar] = "\x89\x02" emit_nop,
        [dr][br] = "\x89\x1a" emit_nop,
        [dr][cr] = "\x89\x0a" emit_nop,
        [dr][dr] = "\x89\x12" emit_nop,
        [dr][sp] = "\x89\x22" emit_nop,
        [dr][bp] = "\x89\x2a" emit_nop,
        [dr][sr] = "\x89\x32" emit_nop,

        [sp][ar] = "\x89\x04\x24",
        [sp][br] = "\x89\x1c\x24",
        [sp][cr] = "\x89\x0c\x24",
        [sp][dr] = "\x89\x14\x24",
        [sp][sp] = "\x89\x24\x24",
        [sp][bp] = "\x89\x2c\x24",
        [sp][sr] = "\x89\x34\x24",

        [bp][ar] = "\x89\x45\x00",
        [bp][br] = "\x89\x5d\x00",
        [bp][cr] = "\x89\x4d\x00",
        [bp][dr] = "\x89\x55\x00",
        [bp][sp] = "\x89\x65\x00",
        [bp][bp] = "\x89\x6d\x00",
        [bp][sr] = "\x89\x75\x00",

        [sr][ar] = "\x89\x06" emit_nop,
        [sr][br] = "\x89\x1e" emit_nop,
        [sr][cr] = "\x89\x0e" emit_nop,
        [sr][dr] = "\x89\x16" emit_nop,
        [sr][sp] = "\x89\x26" emit_nop,
        [sr][bp] = "\x89\x2e" emit_nop,
        [sr][sr] = "\x89\x36" emit_nop,
    },
    [opcode_set] = {
        [ar] = "\xb8\x00\x00\x00\x00",
        [br] = "\xbb\x00\x00\x00\x00",
        [cr] = "\xb9\x00\x00\x00\x00",
        [dr] = "\xba\x00\x00\x00\x00",
        [sp] = "\xbc\x00\x00\x00\x00",
        [bp] = "\xbd\x00\x00\x00\x00",
        [sr] = "\xbe\x00\x00\x00\x00",
    },
    [opcode_imove] = {
        [ar][ar] = "\x66\x0f\x6e\xc0",
        [ar][br] = "\x66\x0f\x6e\xc3",
        [ar][cr] = "\x66\x0f\x6e\xc1",
        [ar][dr] = "\x66\x0f\x6e\xc2",

        [br][ar] = "\x66\x0f\x6e\xc8",
        [br][br] = "\x66\x0f\x6e\xcb",
        [br][cr] = "\x66\x0f\x6e\xc9",
        [br][dr] = "\x66\x0f\x6e\xca",
    },

    // TODO: ICAST

    [opcode_fmove] = {
        [ar][ar] = "\x66\x0f\x7e\xc0",
        [ar][br] = "\x66\x0f\x7e\xc3",
        [ar][cr] = "\x66\x0f\x7e\xc1",
        [ar][dr] = "\x66\x0f\x7e\xc2",

        [br][ar] = "\x66\x0f\x7e\xc8",
        [br][br] = "\x66\x0f\x7e\xcb",
        [br][cr] = "\x66\x0f\x7e\xc9",
        [br][dr] = "\x66\x0f\x7e\xca",
    },

    // TODO: FCAST

    [opcode_fadd] = {
        [ar][ar] = "\xf3\x0f\x58\xc0",
        [ar][br] = "\xf3\x0f\x58\xc1",
        [br][ar] = "\xf3\x0f\x58\xc8",
        [br][br] = "\xf3\x0f\x58\xc9",
    },
    [opcode_fsub] = {
        [ar][ar] = "\xf3\x0f\x5c\xc0",
        [ar][br] = "\xf3\x0f\x5c\xc1",
        [br][ar] = "\xf3\x0f\x5c\xc8",
        [br][br] = "\xf3\x0f\x5c\xc9",
    },
    [opcode_fmul] = {
        [ar][ar] = "\xf3\x0f\x59\xc0",
        [ar][br] = "\xf3\x0f\x59\xc1",
        [br][ar] = "\xf3\x0f\x59\xc8",
        [br][br] = "\xf3\x0f\x59\xc9",
    },
    [opcode_fdiv] = {
        [ar][ar] = "\xf3\x0f\x5e\xc0",
        [ar][br] = "\xf3\x0f\x5e\xc1",
        [br][ar] = "\xf3\x0f\x5e\xc8",
        [br][br] = "\xf3\x0f\x5e\xc9",
    },
    [opcode_goto] = {
        [ar][UNDEFINED] = "\xE9" // -4 from address
    },
    [opcode_gotor] = {
        [ar] = "\xff\xe0",
        [br] = "\xff\xe3",
        [cr] = "\xff\xe1",
        [dr] = "\xff\xe2",
        [sp] = "\xff\xe4",
        [bp] = "\xff\xe5",
        [sr] = "\xff\xe6",
    },
    [opcode_if]  = { [ar][UNDEFINED] = "\x0f\x84" },  // je
    [opcode_not] = { [ar][UNDEFINED] = "\x0f\x85" },  // jne
    [opcode_lts] = { [ar][UNDEFINED] = "\x0f\x82" },  // jb
    [opcode_les] = { [ar][UNDEFINED] = "\x0f\x86" },  // jbe
    [opcode_gts] = { [ar][UNDEFINED] = "\x0f\x87" },  // ja
    [opcode_ges] = { [ar][UNDEFINED] = "\x0f\x83" },  // jae
    [opcode_lt]  = { [ar][UNDEFINED] = "\x0f\x8c" },  // jl
    [opcode_le]  = { [ar][UNDEFINED] = "\x0f\x8e" },  // jle
    [opcode_gt]  = { [ar][UNDEFINED] = "\x0f\x8f" },  // jg
    [opcode_ge]  = { [ar][UNDEFINED] = "\x0f\x8d" },  // jge
    [opcode_interrupt] = { [ar][UNDEFINED] = "\xcd" },
    [opcode_syscall] = { [ar][UNDEFINED] = "\x0f\x05" }
};
