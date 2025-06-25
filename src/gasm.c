#include "lib/core.h"
#include "lib/lib.h"
#include "gasm.h"
#include "asm/asm.c"

uint main (uint argc, char ** argv) {
    char * target, * input, * output = NULL;
    switch(argc) {
        default: 
            print(RED "Error, bad number of arguments" WHITE "\n");
            printf("%s%s\n%s", (char *)msg_version, (char *)msg_license, (char *)msg_help);
            exit(-1);
        case 3: case 4: 
            input = argv[argc-2]; output = argv[argc-1];
            if ( argc == 3 ) { target = (char *)targets[x64]; } else { target = argv[argc-3]; }
            int bytes = file_size(input); if ( bytes <= 0 ) { print(RED "Error, unable to open input file" WHITE "\n"); exit(-1); }
            char buffer[bytes+1];
            int read = file_read(input, buffer, bytes); if ( read <= 0 ) { print(RED "Error, unable to read input file" WHITE "\n"); exit(-1); }
            assembler(buffer);
    }
}