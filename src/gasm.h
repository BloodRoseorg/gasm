#define uint u32

const char * msg_version = "Garter assembler, v 0.0.0\n";
const char * msg_license = "(C) 2025 Blood Rose <https://bloodrose.org/garter>\nThis software is made available under the BSD-3 Clause License\n";
const char * msg_help = "Usage: gasm [target] (input file) (output file)\nTargets: x86 x64 arm risc\n";

#define RED "\e[0;31m"
#define WHITE "\e[0m"

// FUNCTION SIGNATURES

typedef struct {
    u8 opcode; // if "set", also load the 64-bit data
    u8 sources;
} token_t;

token_t * assembler ( char * stream );