//cpp
#include "types.h"
// @symbol func_ov006_020e0308
// recovered name: dScMgCup_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgCup_c::InitResources - recovered from vtable slot identity */
/* func_ov006_020e0308 @ 0x020e0308 (ov006, size 0x26c)
 * Minigame sub-screen setup: configures sub BG1/BG2 control, loads the
 * board tiles/map/palette files, sets the touch UI margins, initializes
 * the three sliders from the table at data_ov006_0213c0a8, then calls
 * virtual +0x48 with mode 3.
 */
struct VtObj {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual void d4();
    virtual void d5();
    virtual void d6();
    virtual void d7();
    virtual void d8();
    virtual void d9();
    virtual void d10();
    virtual void d11();
    virtual void d12();
    virtual void d13();
    virtual void d14();
    virtual void d15();
    virtual void d16();
    virtual void d17();
    virtual void m18(int);
};

typedef struct Pair6 {
    int a, b;
} Pair6;

typedef struct Obj6e {
    char _p0[0x53e8];
    Pair6 pairs[3];  /* 0x53e8 */
    char _p1[0x540c - 0x5400];
    int ones[3];     /* 0x540c */
    char _p2[0x5420 - 0x5418];
    int ids[3];      /* 0x5420 */
    char _p3[0x5465 - 0x542c];
    u8 flags[3];     /* 0x5465 */
} Obj6e;

extern "C" {
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern int data_0208ee44;
extern Pair6 data_ov006_0213c0a8[];

extern void func_ov006_020c225c(char *);
extern int func_ov006_020c3050(char *);
extern int _ZN3G2S13GetBG2CharPtrEv(void);
extern void LoadCompressedFileAt(int, int);
extern void *LoadFile(int);
extern void _ZN3GXS10LoadBGPlttEPKvjj(void *, unsigned int, unsigned int);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern int _ZN3G2S12GetBG2ScrPtrEv(void);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(void *, unsigned int, unsigned int);

int func_ov006_020e0308(char *c)
{
    void *f;

    data_0209d45c = 0x11;
    func_ov006_020c225c(c + 0x4660);
    if (func_ov006_020c3050(c + 0x4f38) == 0)
        return 0;

    {
        volatile u16 *bg = (volatile u16 *)0x04001008;
        *bg = (*bg & 0x43) | 0x2214;
        *bg = *bg & ~0x40;
        *bg = *bg & ~3;
        SetSubBg0Offset(0, 0);
    }

    data_0209d454 |= 1;
    {
        volatile u16 *bg = (volatile u16 *)0x0400100c;
        *bg = (*bg & 0x43) | 0x414;
        *bg = *bg & ~0x40;
        *bg = (*bg & ~3) | 3;
        SetSubBg2Offset(0, 0);
    }

    data_0209d454 |= 4;

    LoadCompressedFileAt(0x2b, _ZN3G2S13GetBG2CharPtrEv() + 0x4000);
    f = LoadFile(0x2c);
    _ZN3GXS10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
    Deallocate(f);
    LoadCompressedFileAt(0x2a, _ZN3G2S12GetBG0ScrPtrEv());
    LoadCompressedFileAt(0x29, _ZN3G2S12GetBG2ScrPtrEv());
    LoadCompressedFileAt(0xc5, 0x6600000);
    f = LoadFile(0xc6);
    _ZN3GXS11LoadOBJPlttEPKvjj(f, 0, 0xa0);
    Deallocate(f);

    data_0208ee44 = 1;
    func_ov004_020b6808();
    data_ov004_020bc880 = 0x80;
    data_ov004_020bc884 = 0x88;
    data_ov004_020bc8a8 = 0x40;
    data_ov004_020bc898 = 0xa0;
    data_ov004_020bc86c = 0xc0;
    data_ov004_020bc8a4 = 0xa0;

    {
        int i;
        for (i = 0; i < 3; i++) {
            ((Obj6e *)c)->pairs[i].a = data_ov006_0213c0a8[i].a;
            ((Obj6e *)c)->pairs[i].b = data_ov006_0213c0a8[i].b;
            ((Obj6e *)c)->ones[i] = 0x1000;
            ((Obj6e *)c)->ids[i] = i;
            *(u8 *)(c + i + 0x5465) = 1;
        }
    }

    ((VtObj *)c)->m18(3);
    func_ov006_020dec3c(c + 0x50e8);
    return 1;
}
}
