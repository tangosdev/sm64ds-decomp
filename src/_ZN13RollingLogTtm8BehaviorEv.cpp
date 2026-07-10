//cpp
struct Vector3;
struct Vector3_16;
struct Actor;

extern "C" {
typedef unsigned int u32;
extern int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(Actor *thiz, int d);
extern int _ZN8SaveData16HasPlayerLostCapEv(void);
extern Actor *_ZN5Actor13ClosestPlayerEv(Actor *thiz);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, const Vector3 *c, const Vector3_16 *d, int e, int f);
extern void func_ov030_021141a8(Actor *c);
extern void func_ov030_02111734(char *c);
extern void func_ov030_02114134(Actor *c);
extern void func_ov030_02112094(Actor *c);
}

struct Actor { char pad[0x800]; };

struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
};

extern "C" int _ZN13RollingLogTtm8BehaviorEv(Actor *thiz)
{
    char *c = (char *)thiz;
    if (_ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(thiz, 0x5dc000) != 0 &&
        *(int *)(c + 0x3b4) != 8) {
        int b = (*(unsigned short *)(c + 0xc) == 0x10b);
        if (b != 0 && *(unsigned char *)(c + 0x3c8) == 0 && _ZN8SaveData16HasPlayerLostCapEv() != 0) {
            Actor *pl = _ZN5Actor13ClosestPlayerEv(thiz);
            unsigned cp = *(unsigned *)((char *)pl + 8);
            if (cp < 3) {
                void *spawned;
                *(int *)(c + 0x3b0) = cp;
                spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                    0x10d,
                    ((unsigned)*(int *)(c + 0x3b0) << 8) | 2,
                    (const Vector3 *)(c + 0x5c),
                    (const Vector3_16 *)0,
                    (int)*(signed char *)(c + 0xcc),
                    -1);
                *(int *)(c + 0x3ac) = *(int *)((char *)spawned + 4);
                *(unsigned char *)(c + 0x3c8) = 1;
                func_ov030_021141a8(thiz);
            }
        }
        func_ov030_02111734(c);
    } else {
        func_ov030_02114134(thiz);
        ((VObj *)(c + 0xd4))->v3();
        func_ov030_02112094(thiz);
    }
    return 1;
}
