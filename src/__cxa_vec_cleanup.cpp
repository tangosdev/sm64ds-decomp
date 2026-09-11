//cpp
// @symbol __cxa_vec_cleanup
// Load-bearing: the ROM build passes -Cpp_exceptions off and has no per-file CFLAGS mechanism; the .exceptix record at 0x02073988 (length 0x74) exists only because this pragma turns exceptions back on for this file.
#pragma exceptions on

// MSL C++ runtime array-destroy helper. Runs `dtor` over the `n` elements of
// `size` bytes in reverse order; if an element destructor throws, the catch
// hands off to the terminate path. The compiler places that handler out of
// line at 0x020732e8 and emits the matching .exception / .exceptix unwind
// records, so the whole 0x74-byte range plus both unwind entries come from
// this one function.
//
// The name is the compiler's, not ours. mwccarm 2004/b56 emits `bl
// __cxa_vec_cleanup` for every array-member destruction it generates, and no
// source in this tree writes that spelling, so it is the only name the link can
// resolve once this range is compiled from source instead of carved out of a dsd
// gap object. `__destroy_arr` was the tree's own name for the same address, given
// by the bulk rename in fb96f41f2 with no recorded evidence; it described the role
// and is kept here only as this comment. Its counterpart at 0x020733a8 took the
// same move for the same reason and on stronger evidence: mwccarm's own probe output
// names `__cxa_vec_ctor`, and two objects in this tree import it, so 0x020733a8 is
// now `__cxa_vec_ctor` with a source file of its own.

extern "C" void func_020731fc(void);

typedef void (*dtor_t)(void *);

extern "C" void __cxa_vec_cleanup(void *block, unsigned int n, unsigned int size, dtor_t dtor)
{
    if (dtor) {
        char *p = (char *)block + n * size;
        try {
            if (n) {
                do {
                    p -= size;
                    dtor(p);
                } while (--n);
            }
        } catch (...) {
            func_020731fc();
        }
    }
}
