symbol_t symbol_index           (char * stream);
data_t * symbol_data            (symbol_t reference);
symbol_t symbol_define          (char *name, char section, uint data);

symbol_t symbol_index (char * stream) {
    uint a = 0; uint b = 0; uint c = 0;
    char * start = stream;
    while(*stream && *stream != ' ') { char ref = *stream - 32; a += ( ref % 3 ); b += ( ref % 5 ); c += ( ref % 7); stream++; }
    symbol_t result = ( stream - start ) + (a << 8) + (b << 16) + (c << 24);
    if ( result >= UNRESOLVED ) { result = ~result; } return result; // do not touch our sentinel values
}

data_t * symbol_resolve(symbol_t index) {
    data_t * reference = &symbol_table[ index % symbol_table_size ];
    if ( reference->id == index || reference->section == UNDEFINED ) { return reference; } // slot immediately found or open
    int i; for(i=0;i<symbol_overflow_size;i++) { // Collisions
        reference = &symbol_overflow[i];
        if ( reference->id == index ) { return reference; }
    }   return NULL;
}

symbol_t symbol_define (char *name, char section, uint data) {
    symbol_t index = symbol_index(name);
    data_t * reference = symbol_resolve(index);
    if ( reference == NULL ) { return ERRNO; }
    reference->id = index;
    reference->section = section;
    reference->value = data;
    return index;
}