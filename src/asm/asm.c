enum { none, x86, x64, arm, risc };
const char targets[][8] = { [x86] = "x86", [x64] = "x64", [arm] = "arm", [risc] = "risc" };

enum {
        // Noop
    noop, halt,
        // Assembly Instructions
    move, load, store, set,
    fmov, imov, fcast, icast,
    add, sub, mul, div, mod, fadd, fsub, fmul, fdiv, fmod,
    left, right, and, or, xor, flip, dec, inc,
    push, pop, compare, fcompare,
    iff, not, lt, gt, le, ge,
    lts, gts, les, ges,
    jump, jumpr, call, ret,
    syscall, interrupt,
        // Assembler Directives
    readable, writable, executable,
    define, fill, origin,
    data8, data16, data32, data64
};

enum {
    noor, ar, br, cr, dr, xr, yr,
    ip, sp, bp
};

token_t * assembler ( char * stream ) {

}