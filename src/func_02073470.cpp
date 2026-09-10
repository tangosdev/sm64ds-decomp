//cpp
// Load-bearing: the ROM build passes -Cpp_exceptions off and has no per-file CFLAGS mechanism; the .exceptix record at 0x020739ac (length 0xc4) exists only because this pragma turns exceptions back on for this file.
#pragma exceptions on

// The family is complete: the last member, __cxa_vec_ctor at 0x020733a8, is the
// plain array construct loop this one wraps with an allocation and a cookie.
//
// MSL C++ runtime array construct-with-cleanup, the fourth of the family that
// starts at __cxa_vec_cleanup. Allocates n * elem_size + cookie_size bytes, writes
// the array cookie (element count just below the array, element size a word
// below that when the cookie is big enough), then constructs the elements
// forward. If an element constructor throws, the handler destroys the elements
// already built by calling the partial destroyer at 0x02073534, frees the
// block, and rethrows.
//
// The handler is out of line at 0x020734f4 and normal flow jumps over it, which
// is why it reads as unreachable; it is the try's landing pad and the cartridge
// counts it inside this function. The .exceptix record at 0x020739ac declares
// the length as 0xc4, which is the whole range including the pad, so the symbol
// table already had this one boundary right.
//
// The fifth argument arrives on the stack and the handler reads it back from
// [fp,#0x40]: 9 pushed registers plus the 0x1c frame put the first stacked
// argument exactly there.

namespace Memory {
void *operator_new2(unsigned int size);
}

extern "C" void func_0203cbc0(void *block);

typedef void (*ctor_t)(void *);
typedef void (*dtor_t)(void *);

extern "C" void func_02073534(void *base, void *current, unsigned int size, dtor_t dtor);

extern "C" void *func_02073470(unsigned int n, unsigned int elem_size,
                               unsigned int cookie_size, ctor_t ctor, dtor_t dtor)
{
    char *block = (char *)Memory::operator_new2(n * elem_size + cookie_size);
    if (!block) {
        return 0;
    }
    if (cookie_size) {
        char *base = block + cookie_size;
        ((unsigned int *)base)[-1] = n;
        if (cookie_size >= 8) {
            ((unsigned int *)base)[-2] = elem_size;
        }
    }
    if (ctor) {
        char *base = block + cookie_size;
        char *p = base;
        try {
            if (n) {
                do {
                    ctor(p);
                    p += elem_size;
                } while (--n);
            }
        } catch (...) {
            if (dtor) {
                func_02073534(base, p, elem_size, dtor);
            }
            func_0203cbc0(block);
            throw;
        }
    }
    return block + cookie_size;
}
