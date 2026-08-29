//cpp
#include "dScEntry_c.h"

extern "C" {
extern void* _ZN7fBase_cnwEj(unsigned int);
extern void _ZN7fBase_cC2Ev(void*);
extern void func_020733a8(void*, int, int, void*, void*);

struct P2 { int a, b; };
struct P2Copy { int words[2]; };
extern char data_0208e4b8[];
extern char _ZTV8dScene_c[];
extern char _ZTV10dScEntry_c[];
extern struct P2 data_02086b58;
extern void _ZN10dScEntry_c6icon_cD1Ev(void);
extern void _ZN10dScEntry_c6icon_cC1Ev(void);
extern void func_ov075_02115bc8(void);
extern void func_ov075_0211a834(void);

void* func_ov075_0211a740(void)
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
        func_020733a8(p + 0x70, 9, sizeof(dScEntry_c::icon_c),
                      (void *)_ZN10dScEntry_c6icon_cC1Ev,
                      (void *)_ZN10dScEntry_c6icon_cD1Ev);
        func_020733a8(p + 0x1b4, 4, sizeof(((dScEntry_c *)p)->mUnkArray[0]),
                      (void *)func_ov075_0211a834,
                      (void *)func_ov075_02115bc8);
    }
    return p;
}
}
