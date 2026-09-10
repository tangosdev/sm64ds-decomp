//cpp
// Load-bearing: the ROM build passes -Cpp_exceptions off and has no per-file CFLAGS mechanism; the .exceptix record at 0x020739b8 (length 0x68) exists only because this pragma turns exceptions back on for this file.
#pragma exceptions on

// Both of the family's construct helpers call this one from their landing pads:
// func_02073470 at its +0x9c and __cxa_vec_ctor at 0x020733a8 + 0x7c.
//
// MSL C++ runtime partial array destroy, the third of the family that starts
// at __cxa_vec_cleanup. Walks `current` back down to `base` in `size` steps,
// running `dtor` on each element; if one throws, the catch hands off to the
// terminate path. The compiler places that handler out of line at 0x02073584,
// which the symbol table had split off as its own function, and emits the
// matching .exception / .exceptix unwind records. The cartridge's .exceptix
// entry at 0x020739b8 declares the length as 0x68, so the whole range plus
// both unwind entries come from this one function.
//
// This is the cleanup func_02073470 calls when an element constructor throws
// partway through building an array.

extern "C" void func_020731fc(void);

typedef void (*dtor_t)(void *);

extern "C" void func_02073534(void *base, void *current, unsigned int size, dtor_t dtor)
{
    char *p = (char *)current;
    try {
        while (p > (char *)base) {
            p -= size;
            dtor(p);
        }
    } catch (...) {
        func_020731fc();
    }
}
