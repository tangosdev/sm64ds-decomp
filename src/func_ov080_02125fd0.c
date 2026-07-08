// NONMATCHING: different op / idiom (div=41). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned int u32;
typedef unsigned short u16;
typedef short s16;

extern void MulMat4x3Mat4x3(const void* m1, const void* m0, void* mF);
extern void func_020553a4(void* m);
extern void func_ov080_02125460(void* self);
extern int VIEW_MATRIX_ASR_3;

void func_ov080_02125fd0(char* self)
{
    int tmp[13];
    int i;
    char* p;

    *(int*)0x4000444 = 0;
    MulMat4x3Mat4x3((void*)(self + 0xd4), &VIEW_MATRIX_ASR_3, tmp);
    *(int*)0x4000440 = 2;
    func_020553a4(tmp);
    *(int*)0x40004c8 = 0xe0000000;
    *(int*)0x40004cc = 0xc0007fff;
    func_ov080_02125460(self);

    *(int*)0x400046c = 0x20000;
    *(int*)0x400046c = 0x20000;
    *(int*)0x400046c = 0x20000;
    *(int*)0x4000500 = 1;

    i = 0;
    p = self + 0x140;
    do {
        *(int*)0x4000488 = *(int*)(p + 0x14);
        *(int*)0x4000484 = *(int*)(p + 0x10);
        *(int*)0x400048c = (u16)(s16)((*(int*)(p + 0) << 8) >> 0x10)
                         | (s16)((*(int*)(p + 4) << 8) >> 0x10) << 0x10;
        *(int*)0x400048c = (u16)(s16)((*(int*)(p + 8) << 8) >> 0x10);
        i++;
        p += 0x18;
    } while (i < 4);

    *(int*)0x4000504 = 0;
    *(int*)0x4000448 = 1;
}
