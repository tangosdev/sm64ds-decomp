//cpp
// NONMATCHING: register allocation (div=17). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef short s16;

struct Scene;
extern "C" int _ZN5Scene12BeforeRenderEv(struct Scene *);
extern "C" int func_ov004_020ae858(void *c);
extern "C" void func_ov004_020b31b4(void *c);
extern "C" void func_ov004_020b8714(void *c);
extern "C" void func_ov004_020add88(void *c);
extern "C" void func_ov004_020b0de0(void *c);

extern char data_ov004_020bf648[];
extern char data_ov004_020bebe8[];

struct Ent { char pad[0x1a]; s16 f; char pad2[4]; };

extern "C" int func_ov004_020b04f4(void *c)
{
    int i;
    char *p;
    int i2;
    char *p2;
    int j;

    if (_ZN5Scene12BeforeRenderEv((struct Scene *)c) == 0)
        return 0;

    if (*(int *)((char *)c + 0x4628) != 0) {
        func_ov004_020ae858(c);
        return 0;
    }

    if (*(int *)((char *)c + 0xf0) == 0) {
        p = data_ov004_020bf648;
        for (i = 0; i < 3; i++, p += 0x134) {
            if (*(int *)(p + 0x20) == 0x1d) continue;
            if (*(s16 *)(p + 0x30) != 0) continue;
            func_ov004_020b31b4(p);
        }
        p2 = data_ov004_020bf648;
        for (i2 = 0; i2 < 3; i2++, p2 += 0x134) {
            if (*(int *)(p2 + 0x20) == 0x1d) continue;
            if (*(s16 *)(p2 + 0x30) == 0) continue;
            func_ov004_020b31b4(p2);
        }

        func_ov004_020b8714((char *)c + 0xcc);
    }

    p = data_ov004_020bebe8;
    for (j = 0; j < 0x40; j++, p += 0x20) {
        if (((struct Ent *)data_ov004_020bebe8)[j].f != 0)
            func_ov004_020add88(p);
    }

    func_ov004_020b0de0(c);
    return 1;
}



