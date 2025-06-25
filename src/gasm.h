#define uint u32

const char * msg_version = "Garter assembler, v 0.0.0\n";
const char * msg_license = "(C) 2025 Blood Rose <https://bloodrose.org/garter>\nThis software is made available under the BSD-3 Clause License\n";
const char * msg_help = "Usage: gasm [target] (input file) (output file)\nTargets: x86 x64 arm risc\n";

#define RED "\e[0;31m"
#define WHITE "\e[0m"

// FUNCTION SIGNATURES

typedef struct {
    u8 opcode; // if "set", load the 16-bit symbol index holding the literal value or offset
    u8 sources;
} token_t;

typedef struct {
    u8 type;    // R, W, X, @, #
    u16 size;
    u32 origin;
} section_t;

typedef struct {
    u8 magic[4];
    u16 symbols; // count, found at *end* of file
    u16 sections;
    section_t headers[];
} object_t;

typedef struct {

} symbol_t;

token_t * assembler ( char * stream );