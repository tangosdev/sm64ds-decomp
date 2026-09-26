//cpp
#include "dScEntry_c.h"

/* The array runtime passes each element address and ignores lifecycle results.
 * Explicit function-pointer casts mark that runtime ABI boundary. */
extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int);
extern void *_ZN7fBase_cC2Ev(void*);
extern void __cxa_vec_ctor(void *base, unsigned int count, unsigned int stride,
                           void (*ctor)(void *), void (*dtor)(void *));

struct P2 { int a, b; };
struct P2Copy { int words[2]; };
extern char data_0208e4b8[];
extern char _ZTV8dScene_c[];
extern char _ZTV10dScEntry_c[];
extern struct P2 data_02086b58;
extern dScEntry_c::icon_c *_ZN10dScEntry_c6icon_cD1Ev(dScEntry_c::icon_c *object);
extern dScEntry_c::icon_c *_ZN10dScEntry_c6icon_cC1Ev(dScEntry_c::icon_c *object);
extern OamAnimation *_ZN12OamAnimationD1Ev(OamAnimation *object);
extern OamAnimation *_ZN12OamAnimationC1Ev(OamAnimation *object);

/* Reconstructed source-style name: SM64DS proves dScEntry_c through RTTI,
 * allocation size, vtable identity, and the ENTRY registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov075_0211a854. */
void* dScEntry_c_classInit_ENTRY(void)
{
    /* The raw byte pointer preserves the ROM factory's address-expression
       tree; typed member expressions fold five instructions in mwccarm. */
    char *p = (char *)_ZN7fBase_cnwEj(sizeof(dScEntry_c));
    if (p) {
        struct P2 v;
        _ZN7fBase_cC2Ev(p);
        *(char**)p = data_0208e4b8;
        *(char**)p = _ZTV8dScene_c;
        {
            unsigned char *flags = (unsigned char *)((int)p + 0x13);
            *flags |= 1;
            *flags |= 4;
        }
        *(char**)p = _ZTV10dScEntry_c;
        /* Array-member copies preserve the original C aggregate-copy schedule
           in C++ without changing the two-word value's type or layout. */
        *(struct P2Copy *)&v = *(struct P2Copy *)&data_02086b58;
        *(struct P2Copy *)(p + 0x54) = *(struct P2Copy *)&v;
        *(struct P2Copy *)(p + 0x5c) = *(struct P2Copy *)&v;
        *(struct P2Copy *)(p + 0x64) = *(struct P2Copy *)&v;
        __cxa_vec_ctor(p + 0x70, 9, sizeof(dScEntry_c::icon_c),
                      (void (*)(void *))_ZN10dScEntry_c6icon_cC1Ev,
                      (void (*)(void *))_ZN10dScEntry_c6icon_cD1Ev);
        __cxa_vec_ctor(p + 0x1b4, 4,
                      sizeof(((dScEntry_c *)p)->mOamAnimations[0]),
                      (void (*)(void *))_ZN12OamAnimationC1Ev,
                      (void (*)(void *))_ZN12OamAnimationD1Ev);
    }
    return p;
}
}
