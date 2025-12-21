/* ------------ ASM.H -------------------------------------- */

//      License:    Blood Rose 4 Clause License v1.0
//      Licensor:   Blood Rose <https://bloodrose.org>

//      #define ASM_H_IMPLEMENTATION once before #include
//      Full License Text available at the bottom of this header

/* ------------ DEPENDENCIES ------------------------------- */

//                  #include "core.h"
//                  #include "lib.h"
extern void         print           (char *);
extern void         memcpy          (char *dest, char *src, i32 max);
extern void *       brk             (void *address);
extern i32          read            (i32 fd, u8 *buf, i32 count);
extern i32          write           (i32 fd, u8 *buf, i32 count);
extern i32          open            (u8 *filename, i32 flags, i32 mode);
extern i32          close           (i32 fd);

/* ------------ PREPROCESSOR DIRECTIVES -------------------- */

#ifndef ASM_H
#define ASM_H

#define RED                     "\x1b[0;31m"
#define GREEN                   "\x1b[0;32m"
#define PURPLE                  "\x1b[0;35m"
#define WHITE                   "\x1b[0m"

#define ELF32_MAGIC             "\x7F" "ELF" "\x01" "\x01" "\x01" "\x00" // Class32, Little Endian, EVCurrent, SYS-V ABI
#define ELF32_EXECUTABLE        0x02 // Executable ELF
#define ELF32_MACHINE           0x03 // EM_386
#define ELF32_ENTRY             0x08048000 // default for common linkers like `ld`
#define ELF32_PT_LOAD           1
#define ELF32_PF_X              1
#define ELF32_PF_W              2
#define ELF32_PF_R              4
#define UNDEFINED               0
#define UNRESOLVED              -2
#define ERRNO                   -1
#define LITERAL                 -1
#define symbol_table_size       ( 1024 * 1024 * 4 )
#define symbol_overflow_size    0xFF
#define section_max             4 // LRWE
#define section_size            1024 * 1024 * 4
#define ELF32_Headspace         (sizeof(elf32h_t) + 3*sizeof(elf32h_t))
#define opcode_branch_start     opcode_goto
#define opcode_branch_end       opcode_ges
#define nop "\x90"
#define modrm "\x01"

enum {
    size_byte = 1, size_word = 2, size_short = 4, size_long = 8
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

#define oprange_float(x)    ( x >= opcode_imove && x <= opcode_fdiv )
#define oprange_literal(x)  ( x >= opcode_goto && x <= opcode_interrupt )

enum {
    section_literal, section_readable, section_writable, section_executable
};

enum {
    ar, br, cr, dr, sp, bp, ip, sr, xr, yr
};

enum {
    reg_t, lit_t, fp_t, instr_t, directive_t, sym_t
};

/* ------------ TYPE DEFINITIONS --------------------------- */

typedef uint symbol_t;
typedef struct { u8 opcode; u8 source; uint adder; int ln; int col; char strlen; } instruction_t;
typedef struct { symbol_t id; char section; uint value; } data_t;
typedef struct { uint section: 8; uint data: 24; } address_t;
typedef struct { uint fill; uint offset; char data[section_size]; } section_t;
typedef struct { char opcode; char argc; char argv[2]; bool literal; char *dat; } operation_t;
typedef struct { char type; char data; char * text; } keyword_t;

typedef struct { 
    unsigned char magic [16]; uint16_t type; uint16_t machine; uint32_t version; uint32_t entry; 
    uint32_t phoff; uint32_t shoff; uint32_t flags; uint16_t ehsize; uint16_t phentsize; uint16_t phnum; 
    uint16_t shentsize; uint16_t shnum; uint16_t shstrndx; } elf32h_t;

typedef struct {
    uint32_t type; uint32_t offset; uint32_t vaddr; uint32_t paddr; 
    uint32_t filesz; uint32_t memsz; uint32_t flags; uint32_t align; } elf32p_t;

/* ------------ PROTOTYPES --------------------------------- */

/*
    ==== TODO:
    Pass 0:
        Run the preprocessor
    Pass 1: 
        From the top of the file,
        Discover symbolic names,
        Process instructions into a table,
        process data_t but do not emit sector data,
        Update working size of each sector,
        Determine the resolved locations of all symbols
    Pass 2:
        Return to the top of the file again,
        Begin emission into the file buffer
*/

instruction_t parse_instruction (char *stream);
int parse_nextbreak             (char *stream);

char emit_size                  (instruction_t this);
void emit_instruction           (instruction_t this);
char emit_modrm                 (instruction_t this);
void emit_raw                   (uint literal, char width);
void emit_flush                 (char *filename);
void emit_data                  (data_t *this);
void emit_error                 (instruction_t * this, char * message);

symbol_t symbol_index           (char * stream);
data_t * symbol_resolve         (symbol_t index);
symbol_t symbol_define          (char *name, char section, uint data);
void symbol_error               (char *name, char * message);

/* ------------ GLOBAL VARIABLES --------------------------- */

data_t symbol_table[ symbol_table_size ] = { UNDEFINED };
data_t symbol_overflow[symbol_overflow_size] = { UNDEFINED };
section_t emit_sectors[section_max] = { UNDEFINED };
char emit_destination = UNDEFINED; char emit_width = UNDEFINED;
extern operation_t instruction_table[];
int parse_int, parse_ln = 0;
const int keyword_count;

/* ------------ IMPLEMENTATION ----------------------------- */
/* enum {
    ar, br, cr, dr, sp, bp, ip, sr, xr, yr
};*/

instruction_t parse_instruction     (char *stream)
{
    char * instruction = (char *)( stream + parse_nextbreak(stream));
}

int parse_nextbreak                 (char *stream)
{
    int i; for(i=0; i<32; i++) { if (stream[i]==0 || stream[i]==' ' || stream[i]==',') { break; } }
    if ( i < 32 ) { return i; } return 0;
}

char emit_modrm                     (instruction_t this) 
{
    const unsigned char map[] = {0, 3, 2, 1, 4, 5, 6, 7, 0, 1};
    switch(this.opcode) {
        default:                return 0xC0 | (map[this.source] << 3) | map[this.adder]; break;
        case opcode_div:        return ("\xf8\xfb\xf9\xfa\xfc\xfd" nop "\xfe" nop nop)[this.adder]; break; // always div eax, <other>
        case opcode_flip:       return ("\xd8\xdb\xd9\xda\xdc\xdd" nop "\xde" nop nop)[this.source]; break;
        case opcode_push:       return ("\x50\x53\x51\x52\x54\x55" nop "\x56" nop nop)[this.source]; break;
        case opcode_pop:        return ("\x58\x5b\x59\x5a\x5c\x5d" nop "\x5e" nop nop)[this.source]; break;
        case opcode_set:        return ("\xb8\xbb\xb9\xba\xbc\xbd" nop "\xbe" nop nop)[this.source]; break;
        case opcode_gotor:      return ("\xe0\xe3\xe1\xe2\xe4\xe5" nop "\xe6" nop nop)[this.source]; break;
        case opcode_interrupt:  return this.adder & 255;
    }
}

char emit_size                      (instruction_t this)
{
    return strlen(instruction_table[this.opcode].dat, 16);
}

void emit_instruction               (instruction_t this)
{
    operation_t *opsource = &instruction_table[this.opcode];
    char * opdat = opsource->dat; int len = strlen(opdat, 16); int i;
        // Handle some edge-case errors
    if ( emit_destination != section_executable )           { emit_error(&this, "Invalid emission destination"); }
    if (this.source >= xr || this.adder >= xr && oprange_float(this.opcode)) { emit_error(&this, "Unexpected floating point register"); }
    if (this.source == ip || this.adder == ip )             { emit_error(&this, "Instruction pointer cannot be used in instructions"); }
    if (this.opcode == opcode_div && this.source != ar )    { emit_error(&this, "Invalid source register for integer division"); }
    if (this.opcode == opcode_left || this.opcode == opcode_right) { 
        if ( this.source != cr || this.adder != ar )        { emit_error(&this, "Bitwise shifts must use ar and cr for source/destination registers"); }}
        // Emit instruction content
    for(i=0;i<len;i++) {
        char next = opdat[i];
        if ( opdat[i] == modrm [0] ) { next = emit_modrm(this); }
        emit_raw(next, sizeof(char));
    }
        // Emit attached literal if applicable
    if ( opsource->literal == true ) {
        symbol_t index = this.adder;
        data_t * ldata = symbol_resolve(index);
        if ( ldata == NULL ) { emit_error(&this, "Unable to resolve symbol data"); }
        emit_data(ldata);
        emit_raw(this.adder, sizeof(u32));
    }
}

void emit_data                      (data_t *this)
{
    char sector = this->section;
    uint offset = emit_sectors[sector].offset;
    uint raw = this->value; if ( sector != section_literal ) { raw += offset; }
    emit_raw(raw, emit_width);
}

void emit_raw                       (uint literal, char width)
{
    // UNDEFINED == section_literal, so no need for multiple checks
    if ( emit_destination == section_literal ) { emit_error(NULL, "Cannot emit data into \"literal\" section"); }
    uint offset =           emit_sectors[emit_destination].fill;
                            emit_sectors[emit_destination].fill += width;
    char * buffer =         emit_sectors[emit_destination].data + offset;
    switch(width) {
        default:            emit_error(NULL, "Internal error, unable to resolve data width");
        case size_byte:     { *(char *)buffer = literal; } break;
        case size_word:     { *(u16  *)buffer = literal; } break;
        case size_short:    { *(u32  *)buffer = literal; } break;
        case size_long:     { *(u64  *)buffer = literal; } break;
    }
}

void emit_flush                     (char *filename)
{
    uint datasize = 0;
    int i; for(i=section_readable;i<section_executable;i++) { datasize += emit_sectors[i].fill; }
    uint filesize = datasize + ELF32_Headspace;
    void * start = brk(NULL); char * stringBuffer = brk((char *) start + (int)filesize); // "malloc" replacement
    if ( !stringBuffer ) { emit_error(NULL, "Unable to allocate memory for file IO"); }
    char * cursor = stringBuffer;

    // Copy a formatted ELF Header and Program Headers into the file
    elf32h_t HEADER = {
        .magic =            ELF32_MAGIC,
        .type =             ELF32_EXECUTABLE,
        .version =          1,
        .entry =            ELF32_ENTRY,
        .phoff =            sizeof(elf32h_t),
        .shoff =            0, // no section headers
        .flags =            0, // none?
        .ehsize =           sizeof(elf32h_t),
        .phentsize =        sizeof(elf32p_t),
        .phnum =            3, // Readable, Writable, Executable
        .shentsize =        0,
        .shnum =            0,
        .shstrndx =         0
    };  memcpy(cursor, (char *)&HEADER, sizeof(elf32h_t)); cursor += sizeof(elf32h_t);

    elf32p_t EXECUTABLE = {
        .type =             ELF32_PT_LOAD,
        .offset =           ELF32_Headspace,
        .vaddr =            ELF32_ENTRY,
        .paddr =            ELF32_ENTRY ,
        .filesz =           emit_sectors[section_executable].fill,
        .memsz =            emit_sectors[section_executable].fill,
        .flags =            ELF32_PF_R | ELF32_PF_X,
        .align =            0x1000
    };  memcpy(cursor, (char *)&EXECUTABLE, sizeof(elf32p_t)); 
    cursor += sizeof(elf32p_t);

    elf32p_t READABLE = {
        .type =             ELF32_PT_LOAD,
        .offset =           ELF32_Headspace + EXECUTABLE.filesz,
        .vaddr =            ELF32_ENTRY + EXECUTABLE.memsz,
        .paddr =            ELF32_ENTRY + EXECUTABLE.memsz,
        .filesz =           emit_sectors[section_readable].fill,
        .memsz =            emit_sectors[section_readable].fill,
        .flags =            ELF32_PF_R,
        .align =            0x1000
    };  memcpy(cursor, (char *)&READABLE, sizeof(elf32p_t)); 
    cursor += sizeof(elf32p_t);

    elf32p_t WRITABLE = {
        .type =             ELF32_PT_LOAD,
        .offset =           ELF32_Headspace + READABLE.filesz + EXECUTABLE.filesz,
        .vaddr =            ELF32_ENTRY + READABLE.memsz,
        .paddr =            ELF32_ENTRY + READABLE.memsz,
        .filesz =           emit_sectors[section_writable].fill,
        .memsz =            emit_sectors[section_writable].fill,
        .flags =            ELF32_PF_R | ELF32_PF_W,
        .align =            0x1000
    };  memcpy(cursor, (char *)&WRITABLE, sizeof(elf32p_t)); 
    cursor += sizeof(elf32p_t);

    // Copy in the literal section data
    memcpy(cursor, (char *)(emit_sectors[section_executable].data), EXECUTABLE.filesz);
        cursor += EXECUTABLE.filesz;
    memcpy(cursor, (char *)(emit_sectors[section_readable].data), READABLE.filesz);
        cursor += READABLE.filesz;
    memcpy(cursor, (char *)(emit_sectors[section_writable].data), WRITABLE.filesz);
        cursor += WRITABLE.filesz;

    // Write to file
    const short rw =        0x02;
    const short create =    0x40;
    const short truncate =  0x200;
    const short flags = rw + create + truncate;
    int fd = open((unsigned char *)filename, flags, 0666); if ( fd <= STDERR ) { emit_error(NULL, "Could not get handle for file IO"); }
    int written = 0; while(written < filesize) 
    {
        int loop = write(fd, (unsigned char *)stringBuffer + written, filesize - written); if ( loop <= 0 ) { emit_error(NULL, "Error while trying to write file data"); }
        written += loop;
    }
    close(fd);
}

void emit_error(instruction_t *this, char * message)
{
    if ( this == NULL ) { printf(RED "[Emitter] " WHITE "%s\n", message); exit(-1); }
    printf(RED GASM "[Emitter] " PURPLE "(%d, %d)" WHITE "%s\n", &this->ln, &this->col, message); exit(-1);
}

symbol_t symbol_index               (char * stream)
{
    uint a = 0; uint b = 0; uint c = 0;
    char * start = stream;
    while(*stream && *stream != ' ') { char ref = *stream - 32; a += ( ref % 3 ); b += ( ref % 5 ); c += ( ref % 7); stream++; }
    symbol_t result = ( stream - start ) + (a << 8) + (b << 16) + (c << 24);
    if ( result >= UNRESOLVED ) { result = ~result; } return result; // do not touch our sentinel values
}

data_t * symbol_resolve             (symbol_t index)
{
    data_t * reference = &symbol_table[ index % symbol_table_size ];
    if ( reference->id == index || reference->section == UNDEFINED ) { return reference; } // slot immediately found or open
    int i; for(i=0;i<symbol_overflow_size;i++) { // Collisions
        reference = &symbol_overflow[i];
        if ( reference->id == index ) { return reference; }
    }   return NULL;    
}

symbol_t symbol_define              (char *name, char section, uint data)
{
    int len = strlen(name, 32); if ( len < 3 ) { symbol_error(name, "Symbolic names may not be shorter than 3"); }
    symbol_t index = symbol_index(name);
    data_t * reference = symbol_resolve(index);
    if ( reference == NULL ) { symbol_error(name, "Unable to allocate index to symbolic name"); }
    reference->id = index;
    reference->section = section;
    reference->value = data;
    return index;
}

void symbol_error                   (char * name, char * message)
{
    printf(RED GASM "[Symbol Definition] " WHITE "%s " PURPLE "(%s)" WHITE "\n", message, name); exit(-1);
}

/* ------------ OPERAND TABLE ------------------------------ */

keyword_t keyword_table[] = {

        // Assembler Directives
    { .text = "section",        .type = directive_t },
    { .text = "r",              .type = directive_t },
    { .text = "w",              .type = directive_t },
    { .text = "x",              .type = directive_t },
    { .text = "byte",           .type = directive_t },
    { .text = "word",           .type = directive_t },
    { .text = "short",          .type = directive_t },
    { .text = "long",           .type = directive_t },
    { .text = "exe",            .type = directive_t },

    { .text = "readable",       .type = directive_t }, // aliases
    { .text = "writable",       .type = directive_t },
    { .text = "executable",     .type = directive_t },

        // Registers
    { .text = "ar", .type = reg_t, .data = ar },
    { .text = "br", .type = reg_t, .data = br },
    { .text = "cr", .type = reg_t, .data = cr },
    { .text = "dr", .type = reg_t, .data = dr },
    { .text = "sp", .type = reg_t, .data = sp },
    { .text = "bp", .type = reg_t, .data = bp },
    { .text = "ip", .type = reg_t, .data = ip },
    { .text = "sr", .type = reg_t, .data = sr },
    { .text = "xr", .type = fp_t,  .data = xr },
    { .text = "yr", .type = fp_t,  .data = yr },
    
        // Instructions
    { .text = "compare",    .type = instr_t, .data = opcode_compare },
    { .text = "fcompare",   .type = instr_t, .data = opcode_fcompare },
    { .text = "add",        .type = instr_t, .data = opcode_add },
    { .text = "sub",        .type = instr_t, .data = opcode_sub },
    { .text = "mul",        .type = instr_t, .data = opcode_mul },
    { .text = "div",        .type = instr_t, .data = opcode_div },
    { .text = "flip",       .type = instr_t, .data = opcode_flip },
    { .text = "and",        .type = instr_t, .data = opcode_and },
    { .text = "or",         .type = instr_t, .data = opcode_or },
    { .text = "xor",        .type = instr_t, .data = opcode_xor },
    { .text = "left",       .type = instr_t, .data = opcode_left },
    { .text = "right",      .type = instr_t, .data = opcode_right },
    { .text = "push",       .type = instr_t, .data = opcode_push },
    { .text = "pop",        .type = instr_t, .data = opcode_pop },
    { .text = "move",       .type = instr_t, .data = opcode_move },
    { .text = "load",       .type = instr_t, .data = opcode_load },
    { .text = "store",      .type = instr_t, .data = opcode_store },
    { .text = "set",        .type = instr_t, .data = opcode_set },
    { .text = "imove",      .type = instr_t, .data = opcode_imove },
    { .text = "icast",      .type = instr_t, .data = opcode_icast },
    { .text = "fmove",      .type = instr_t, .data = opcode_fmove },
    { .text = "fcast",      .type = instr_t, .data = opcode_fcast },
    { .text = "fadd",       .type = instr_t, .data = opcode_fadd },
    { .text = "fsub",       .type = instr_t, .data = opcode_fsub },
    { .text = "fmul",       .type = instr_t, .data = opcode_fmul },
    { .text = "fdiv",       .type = instr_t, .data = opcode_fdiv },
    { .text = "gotor",      .type = instr_t, .data = opcode_gotor },
    { .text = "goto",       .type = instr_t, .data = opcode_goto },
    { .text = "if",         .type = instr_t, .data = opcode_if },
    { .text = "not",        .type = instr_t, .data = opcode_not },
    { .text = "lt",         .type = instr_t, .data = opcode_lt },
    { .text = "le",         .type = instr_t, .data = opcode_le },
    { .text = "gt",         .type = instr_t, .data = opcode_gt },
    { .text = "ge",         .type = instr_t, .data = opcode_ge },
    { .text = "lts",        .type = instr_t, .data = opcode_lts },
    { .text = "les",        .type = instr_t, .data = opcode_les },
    { .text = "gts",        .type = instr_t, .data = opcode_gts },
    { .text = "ges",        .type = instr_t, .data = opcode_ges },
    { .text = "interrupt",  .type = instr_t, .data = opcode_interrupt },
    { .text = "syscall",    .type = instr_t, .data = opcode_syscall },
};  const int keyword_count = sizeof(keyword_table) / sizeof(keyword_table[0]);

operation_t instruction_table[] = {
    [opcode_compare]        = { .opcode = opcode_compare,   .argc = 2, .argv = { reg_t, reg_t },    .dat = "\x39" modrm },
    [opcode_fcompare]       = { .opcode = opcode_fcompare,  .argc = 2, .argv = { fp_t,  fp_t  },    .dat = "\x66\x0f\x2e" modrm },
    [opcode_add]            = { .opcode = opcode_add,       .argc = 2, .argv = { reg_t, reg_t },    .dat = "\x01" modrm },
    [opcode_sub]            = { .opcode = opcode_sub,       .argc = 2, .argv = { reg_t, reg_t },    .dat = "\x29" modrm },
    [opcode_mul]            = { .opcode = opcode_mul,       .argc = 2, .argv = { reg_t, reg_t },    .dat = "\x0f\xaf" modrm },
    [opcode_div]            = { .opcode = opcode_div,       .argc = 2, .argv = { reg_t, reg_t },    .dat = "\xf7" modrm }, // Division acts like unary, special case
    [opcode_flip]           = { .opcode = opcode_flip,      .argc = 1, .argv = { reg_t },           .dat = "\xf7" modrm }, // Unary, special case
    [opcode_and]            = { .opcode = opcode_and,       .argc = 2, .argv = { reg_t, reg_t },    .dat = "\x21" modrm },
    [opcode_or]             = { .opcode = opcode_or,        .argc = 2, .argv = { reg_t, reg_t },    .dat = "\x09" modrm },
    [opcode_xor]            = { .opcode = opcode_xor,       .argc = 2, .argv = { reg_t, reg_t },    .dat = "\x31" modrm },
    [opcode_left]           = { .opcode = opcode_left,      .argc = 2, .argv = { reg_t, reg_t },    .dat = "\xD3\xE0" }, // Left-Right, special cases
    [opcode_right]          = { .opcode = opcode_right,     .argc = 2, .argv = { reg_t, reg_t },    .dat = "\xD3\xE8" }, // Left-Right, special cases
    [opcode_push]           = { .opcode = opcode_push,      .argc = 1, .argv = { reg_t },           .dat = modrm }, // Push-Pop, special cases
    [opcode_pop]            = { .opcode = opcode_pop,       .argc = 1, .argv = { reg_t },           .dat = modrm }, // Push-Pop, special cases
    [opcode_move]           = { .opcode = opcode_move,      .argc = 2, .argv = { reg_t, reg_t },    .dat = "\x89" modrm },
    [opcode_load]           = { .opcode = opcode_load,      .argc = 2, .argv = { reg_t, reg_t },    .dat = "\x8b" modrm },
    [opcode_store]          = { .opcode = opcode_store,     .argc = 2, .argv = { reg_t, reg_t },    .dat = "\x89" modrm },
    [opcode_set]            = { .opcode = opcode_set,       .argc = 2, .argv = { reg_t, lit_t },    .dat = modrm, .literal = true }, // Set, special case
    [opcode_imove]          = { .opcode = opcode_imove,     .argc = 2, .argv = { reg_t, fp_t  },    .dat = "\x66\x0f\x6e" modrm },
    [opcode_icast]          = { .opcode = opcode_icast,     .argc = 2, .argv = { reg_t, fp_t  },    .dat = "\xf2\x0f\x2a" modrm },
    [opcode_fmove]          = { .opcode = opcode_fmove,     .argc = 2, .argv = { reg_t, fp_t  },    .dat = "\x66\x0f\x7e" modrm },
    [opcode_fcast]          = { .opcode = opcode_fcast,     .argc = 2, .argv = { reg_t, fp_t  },    .dat = "\xf2\x0f\x2d" modrm },
    [opcode_fadd]           = { .opcode = opcode_fadd,      .argc = 2, .argv = { fp_t,  fp_t  },    .dat = "\xf3\x0f\x58" modrm },
    [opcode_fsub]           = { .opcode = opcode_fsub,      .argc = 2, .argv = { fp_t,  fp_t  },    .dat = "\xf3\x0f\x5c" modrm },
    [opcode_fmul]           = { .opcode = opcode_fmul,      .argc = 2, .argv = { fp_t,  fp_t  },    .dat = "\xf3\x0f\x59" modrm },
    [opcode_fdiv]           = { .opcode = opcode_fdiv,      .argc = 2, .argv = { fp_t,  fp_t  },    .dat = "\xf3\x0f\x5e" modrm },
    [opcode_gotor]          = { .opcode = opcode_gotor,     .argc = 1, .argv = { reg_t },           .dat = "\xff" modrm }, // Unary, special case
    [opcode_goto]           = { .opcode = opcode_goto,      .argc = 1, .argv = { lit_t },           .dat = "\xe9", .literal = true },
    [opcode_if]             = { .opcode = opcode_if,        .argc = 1, .argv = { lit_t },           .dat = "\x0f\x84", .literal = true },
    [opcode_not]            = { .opcode = opcode_not,       .argc = 1, .argv = { lit_t },           .dat = "\x0f\x85", .literal = true },
    [opcode_lt]             = { .opcode = opcode_lt,        .argc = 1, .argv = { lit_t },           .dat = "\x0f\x8c", .literal = true },
    [opcode_le]             = { .opcode = opcode_le,        .argc = 1, .argv = { lit_t },           .dat = "\x0f\x8e", .literal = true },
    [opcode_gt]             = { .opcode = opcode_gt,        .argc = 1, .argv = { lit_t },           .dat = "\x0f\x8f", .literal = true },
    [opcode_ge]             = { .opcode = opcode_ge,        .argc = 1, .argv = { lit_t },           .dat = "\x0f\x8d", .literal = true },
    [opcode_lts]            = { .opcode = opcode_lts,       .argc = 1, .argv = { lit_t },           .dat = "\x0f\x82", .literal = true },
    [opcode_les]            = { .opcode = opcode_les,       .argc = 1, .argv = { lit_t },           .dat = "\x0f\x86", .literal = true },
    [opcode_gts]            = { .opcode = opcode_gts,       .argc = 1, .argv = { lit_t },           .dat = "\x0f\x87", .literal = true },
    [opcode_ges]            = { .opcode = opcode_ges,       .argc = 1, .argv = { lit_t },           .dat = "\x0f\x83", .literal = true },
    [opcode_interrupt]      = { .opcode = opcode_interrupt, .argc = 1, .argv = { lit_t },           .dat = "\xcd" modrm }, // Special case; really just literal CHAR instead of INT
    [opcode_syscall]        = { .opcode = opcode_syscall,   .argc = 0, .argv = { UNDEFINED },       .dat = "\x0f\x05" }
};

#endif

/* ------------ LICENSE ------------------------------------ //

    ***    Garter Bootstrapping Assembler    *** 
    (C) 2025 Blood Rose / Blood Rose Records 
    <https://bloodrose.org>

Redistribution and use in source and binary forms,
with or without modification,
are permitted provided that the following conditions are met:

1. Redistribution of source code must retain 
the above copyright notice, this list of conditions, and the following disclaimer.

2. Redistribution in binary form must reproduce 
the above copyright notice, this list of conditions, and the following disclaimer
in the documentation and/or other materials provided with the distribution.

3. This license does not grant the right to sell the software.
"Sell" means practicing any or all of the rights granted by this license
to provide a product or service to third parties for a fee or other consideration
(including, without limitation, fees for hosting, consulting, or support services),
where the product or service derives substantially or wholly from the software.

4. Neither the name of the licensor nor the names of
the software's contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE */