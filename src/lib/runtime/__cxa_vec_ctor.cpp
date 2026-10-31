//cpp
// Load-bearing: the ROM build passes -Cpp_exceptions off and has no per-file CFLAGS mechanism; the .exceptix record at 0x020739a0 (length 0xc8) exists only because this pragma turns exceptions back on for this file.
#pragma exceptions on

// MSL C++ runtime array construct helper, the last of the family that starts
// at __cxa_vec_cleanup. Runs `ctor` forward over the `n` elements of
// `elem_size` bytes at `base`. If an element constructor throws, the handler
// destroys the prefix that was already constructed by calling the partial
// destroyer at 0x02073534 and rethrows.
//
// The destructor argument decides the shape rather than the behaviour: with no
// destructor there is nothing to unwind, so the compiler emits a second, plain
// copy of the loop with no try around it. That is the two loops in the
// cartridge, at +0x4c (guarded) and +0xa8 (plain), and it is why `base` has to
// stay live in its own register across the guarded one: the landing pad needs
// it while the plain loop is free to walk the same register.
//
// The handler is out of line at 0x02073414 and normal flow jumps over it, which
// is why it reads as unreachable; it is the try's landing pad and the cartridge
// counts it inside this function. The .exceptix record at 0x020739a0 declares
// the length as 0xc8, which is the whole range including the pad, so the symbol
// table already had this boundary right and no symbol merge was needed.
//
// The fifth argument arrives on the stack and both the guard at +0x28 and the
// handler at +0x6c read it back from [fp,#0x38]: 7 pushed registers plus the
// 0x1c frame put the first stacked argument exactly there.
//
// THE NAME IS THE COMPILER'S, NOT AN INVENTION. mwccarm 2004/b56 emits a call to
// __cxa_vec_ctor for an array of a class that has a constructor and a destructor; a
// bare probe compiled with the ROM's own CFLAGS leaves __cxa_vec_ctor, __cxa_vec_new,
// __cxa_vec_delete and __cxa_vec_cleanup undefined, and two objects in this tree,
// _ZN10dMgPsOpt_cC1Ev and _ZN17dMg3DEspAnimSet_cC1Ev, really do import __cxa_vec_ctor.
// Until this file was enrolled, the only thing defining that name was the dsd gap object
// covering 0x020733a8, which defined it because config/arm9/symbols.txt carried a size-0
// alias row beside the sized func_020733a8 row. Enrolling the range as `complete` carves
// it out of the gap, so the alias would have stopped being a definition and the link
// would have failed on __cxa_vec_ctor exactly as it failed on __cxa_vec_cleanup. The fix
// is the one 0x0207328c took: the sized row takes the compiler's spelling, the size-0 row
// is deleted, and the file stem follows the symbol. The old spelling, func_020733a8, was
// only ever an address placeholder; MSL's own name for this entry is not recorded
// anywhere in the cartridge.

typedef void (*ctor_t)(void *);
typedef void (*dtor_t)(void *);

extern "C" void func_02073534(void *base, void *current, unsigned int size, dtor_t dtor);

extern "C" void __cxa_vec_ctor(void *base, unsigned int n, unsigned int elem_size,
                               ctor_t ctor, dtor_t dtor)
{
    if (ctor) {
        if (dtor) {
            char *p = (char *)base;
            try {
                if (n) {
                    do {
                        ctor(p);
                        p += elem_size;
                    } while (--n);
                }
            } catch (...) {
                func_02073534(base, p, elem_size, dtor);
                throw;
            }
        } else {
            char *p = (char *)base;
            if (n) {
                do {
                    ctor(p);
                    p += elem_size;
                } while (--n);
            }
        }
    }
}
