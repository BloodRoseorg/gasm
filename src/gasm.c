/* ------------ GASM.C ------------------------------------- */

//      License:    Blood Rose 4 Clause License v1.0
//      Licensor:   Blood Rose <https://bloodrose.org>

#define GASM                        "Garter Assembler "
#define VERSION                     "v 0.0.0"
#include                            "LICENSE.h"
#include                            "lib/core.h"
#include                            "lib/lib.h"
#include                            "asm.h"

char main                           (int argc, char **argv);
void cli_error                      (char * opt, char * message);
void cli_license                    (void);
void cli_version                    (void);
void cli_help                       (void);

char main                           (int argc, char **argv)
{
    bool license = false; bool help = false; bool version = false;
    char *input = NULL; char *output = NULL; char lflag = 0; char target = 0;
    if ( argc < 2 ) { cli_error(NULL, "expects input filename and optional parameters"); }
    int i; for(i=1;i<argc;i++) 
    {   if ( argv[i][0] != '-' ) {
            if ( lflag == 'o' )     { if ( output == NULL ) { lflag = 0; output = argv[i]; continue; } cli_error(argv[i], "Given multiple output filenames"); }
            if ( input == NULL )    { input = argv[i]; continue; }
            if ( output == NULL )   { output = argv[i]; continue; }
            cli_error(argv[i], "Given more than two filenames, expects one source and one destination filename");
        }
        char * flag = &argv[i][1];
        while (*flag != 0 && *flag == '-') { flag++; }
            if ( *flag <= 90 && *flag >= 65 ) { *flag += 32; } // cast to lowercase
        switch(*flag) {
            default:            cli_error(argv[i], "Invalid commandline argument, try \"--help\" for options");
            case 't':           cli_error(argv[i], "Bootstrapper does not support -t command, outputs only Linux SYS-V x86-32 ELF Binaries"); break;
            case 'l':           license = true; break;
            case 'h':           help = true; break;
            case 'v':           version = true; break;
            case 'o':           lflag = 'o'; break;
        }
    }
    if ( license == true )          { cli_license(); }
    if ( version == true )          { cli_version(); }
    if ( help == true )             { cli_help(); }

    return 0;
}

void cli_error                      (char *opt, char *message)
{
    if ( opt ) {  printf(RED GASM "[Interface] " WHITE "%s " PURPLE "(%s)" WHITE "\n", message, opt); exit(-1); }
    printf(RED GASM "[Interface] " WHITE "%s\n", message); exit(-1);
}

void cli_license() 
{
    cli_version(); print(PURPLE);
    // Walk the text since our "print" fn has a hardcoded 1024 char per write
    int i; for(i=0;i<strlen(LICENSE_TEXT, 4096);i+=1024) {
        print((char *)(LICENSE_TEXT + i)); }
    print   (WHITE "\n");
}

void cli_version() 
{
    static bool pend = true;
    if ( pend == true ) { pend = false;
        char text[] = GREEN GASM WHITE VERSION "\n";
        print(text);
    }
}

void cli_help() 
{
    char text[] = \
    "*** Bootstrapping assembler for the Blood Rose compiler suite ***\nUsage:\n"
    "    " PURPLE "-h" WHITE " or " PURPLE "--help" WHITE "                    Print this help dialog\n"
    "    " PURPLE "-v" WHITE " or " PURPLE "--version" WHITE "                 Print the version string\n"
    "    " PURPLE "-l" WHITE " or " PURPLE "--license" WHITE "                 Print the full user license agreement\n"
    "    " PURPLE "<input> <output>" WHITE "                Reads input filename, writes assembled binary to output filename\n"
    "    " PURPLE "-o <output> <input>" WHITE "             Switch the order of input and output filenames\n"
    "    " PURPLE "-t [platform]" WHITE "                   Unsupported by the bootstrapper: assemble binary for given target platform\n"
    ;
    cli_version(); print(text);
}