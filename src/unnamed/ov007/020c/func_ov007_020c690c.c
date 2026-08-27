#include "types.h"
extern void* func_ov007_020c3df4(int a, int size);
extern void func_ov007_020c684c(void* self);
extern void* func_ov007_020c844c(void* p, int q);
extern void* func_ov007_020c80a4(void);
extern void func_ov007_020c8010(void* self, void* a, void* b);
extern int func_ov007_020c2c68(int a, int b, int c);
extern void func_ov007_020c6724(void* self, int flag);

void* func_ov007_020c690c(int a0, int a1, int a2, int a3, int a4, int a5)
{
    char* o = (char*)func_ov007_020c3df4(0, 0xd0);
    int n4 = a0 << 2;
    int n2;
    int i;

    *(s32*)(o + 0xc) = a0;
    *(s32*)(o + 0x10) = a1;
    *(s32*)(o + 0x18) = a5;
    *(s32*)(o + 0x1c) = a4;
    *(s32*)(o + 0xa8) = a3;

    *(void**)(o + 0x28) = func_ov007_020c3df4(0, n4);
    *(void**)(o + 0x2c) = func_ov007_020c3df4(0, n4);
    *(s32*)(o + 0x30) = 0;
    *(void**)(o + 0x38) = func_ov007_020c3df4(0, n4);
    *(void**)(o + 0x3c) = func_ov007_020c3df4(0, n4);

    n2 = a0 << 1;
    *(void**)(o + 0x48) = func_ov007_020c3df4(0, n2);
    *(void**)(o + 0x4c) = func_ov007_020c3df4(0, n2);
    *(void**)(o + 0x50) = func_ov007_020c3df4(0, n2);
    *(void**)(o + 0x54) = func_ov007_020c3df4(0, n2);

    *(void**)(o + 0x34) = func_ov007_020c3df4(0, n4);
    *(void**)(o + 0x40) = func_ov007_020c3df4(0, n4);
    *(void**)(o + 0x44) = func_ov007_020c3df4(0, n4);

    func_ov007_020c684c(o);

    for (i = 0; i < a0; i++) {
        ((s32*)*(void**)(o + 0x38))[i] = 0;
        ((u16*)*(void**)(o + 0x4c))[i] = 0xffff;
        ((u16*)*(void**)(o + 0x48))[i] = ((u16*)*(void**)(o + 0x4c))[i];
        ((u16*)*(void**)(o + 0x54))[i] = 0xffff;
        ((u16*)*(void**)(o + 0x50))[i] = ((u16*)*(void**)(o + 0x54))[i];
    }

    *(void**)(o + 0x6c) = func_ov007_020c844c(o + 0x70, 0xa000);
    *(void**)(o + 0x94) = func_ov007_020c844c(o + 0x98, 0x1000);
    *(void**)(o + 0xa4) = func_ov007_020c80a4();
    func_ov007_020c8010(*(void**)(o + 0xa4), *(void**)(o + 0x94), *(void**)(o + 0x6c));

    *(void**)(o + 0x20) = (void*)func_ov007_020c2c68(0, a2, 1);
    *(void**)(o + 0x24) = func_ov007_020c3df4(0, a2 * 0xc);

    func_ov007_020c6724(o, 1);
    return o;
}
