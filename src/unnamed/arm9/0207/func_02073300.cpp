//cpp
// Load-bearing: the ROM build passes -Cpp_exceptions off and has no per-file CFLAGS mechanism; the .exceptix record at 0x02073994 (length 0xa8) exists only because this pragma turns exceptions back on for this file.
#pragma exceptions on

// The whole family is source-built now: __cxa_vec_cleanup at 0x0207328c, this one,
// __cxa_vec_ctor at 0x020733a8, __cxa_vec_new and func_02073534.
//
// MSL C++ runtime array-destroy-with-rethrow helper, the sibling of
// __cxa_vec_cleanup at 0x0207328c. Runs `dtor` over the `n` elements of `size`
// bytes in reverse order; if an element destructor throws, the handler
// destroys whatever is left below it and rethrows, and a second throw from
// that cleanup goes to the terminate path. The compiler places the handler
// out of line at 0x0207335c, which the symbol table had split off as its own
// function, and emits the matching .exception / .exceptix unwind records; the
// cartridge's .exceptix entry at 0x02073994 declares the length as 0xa8, so
// the whole range plus both unwind entries come from this one function.
//
// The two nested catch contexts are why the frame is 0x30 rather than
// __cxa_vec_cleanup's 0x18: 0x18 bytes of catch state each, sp spilled at +0x14
// inside each one (fp+0x14 for the outer try, fp+0x2c for the inner).
//
// It does the job of the Itanium C++ ABI's __cxa_vec_dtor, and
// __cxa_vec_delete calls it for that step, but it keeps its address name:
// mwccarm never emits a call to __cxa_vec_dtor, so no compiler output
// confirms the name, and its body does not follow GCC's __cxa_vec_dtor
// statement for statement.

#include "MWCPlusLib.h"

namespace std {
void terminate();
}

void func_02073300(void *block, unsigned int n, unsigned int size, dtor_t dtor)
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
            try {
                if (--n) {
                    do {
                        p -= size;
                        dtor(p);
                    } while (--n);
                }
            } catch (...) {
                std::terminate();
            }
            throw;
        }
    }
}
