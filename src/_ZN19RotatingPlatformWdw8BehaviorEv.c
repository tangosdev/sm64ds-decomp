// NONMATCHING: different op / idiom (div=35). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef short s16;

extern u8 IsAreaShowing(int idx);
extern int _ZN16MeshColliderBase9IsEnabledEv(void *m);
extern void _ZN16MeshColliderBase7DisableEv(void *m);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void *m, void *actor);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(u32 a, u32 b, u32 c, void *pos, u32 e);
extern void func_ov029_021122b4(char *c);
extern void func_ov029_02112250(char *c);
extern void _ZN9Animation7AdvanceEv(void *a);
extern s16 data_02082214[];
extern int data_0209f32c;

int _ZN19RotatingPlatformWdw8BehaviorEv(char *c)
{
    int t;

    *(signed char*)(c + 0xcc) = -1;
    if (IsAreaShowing(*(signed char*)(c + 0x340)) == 0) {
        *(signed char*)(c + 0xcc) = *(signed char*)(c + 0x340);
        if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124) != 0) {
            _ZN16MeshColliderBase7DisableEv(c + 0x124);
        }
    } else {
        if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124) == 0) {
            _ZN16MeshColliderBase6EnableEP5Actor(c + 0x124, c);
        }
    }

    t = *(int*)(c + 0x334);
    if (*(int*)(c + 0x60) != t) {
        int *p = (int*)(c + 0x60);
        if (*(int*)(c + 0x60) < t) {
            *p += 0xa000;
            if (*(int*)(c + 0x60) > *(int*)(c + 0x334))
                *(int*)(c + 0x60) = *(int*)(c + 0x334);
        } else {
            *p -= 0xa000;
            if (*(int*)(c + 0x60) < *(int*)(c + 0x334))
                *(int*)(c + 0x60) = *(int*)(c + 0x334);
        }
    }

    if (*(int*)(c + 0x60) != *(int*)(c + 0x6c)) {
        *(int*)(c + 0x33c) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(u32*)(c + 0x33c), 3, 0x96, c + 0x74, 0);
    }

    {
        s16 *q = (s16*)(c + 0x342);
        int i;
        *q = (s16)(*q + 0x200);
        i = (u16)*(s16*)(c + 0x300 + 0x42) >> 4;
        *(int*)(c + 0x344) = (int)data_02082214[i * 2] * 0xf + *(int*)(c + 0x60);
    }
    data_0209f32c = *(int*)(c + 0x344);
    func_ov029_021122b4(c);
    func_ov029_02112250(c);
    *(int*)(c + 0x32c) = 0x1000;
    _ZN9Animation7AdvanceEv(c + 0x320);
    return 1;
}
