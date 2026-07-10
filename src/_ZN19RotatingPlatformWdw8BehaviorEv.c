typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef signed char s8;

extern int IsAreaShowing(int idx);
extern int _ZN16MeshColliderBase9IsEnabledEv(void *m);
extern void _ZN16MeshColliderBase7DisableEv(void *m);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void *m, void *actor);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned a, unsigned b, unsigned c, void *pos, unsigned e);
extern void func_ov029_021122b4(void *c);
extern void func_ov029_02112250(void *c);
extern void _ZN9Animation7AdvanceEv(void *a);
extern s16 data_02082214[];
extern int data_0209f32c;

int _ZN19RotatingPlatformWdw8BehaviorEv(u8 *c)
{
    int t;
    s16 *q;
    int i;

    *(s8 *)(c + 0xcc) = -1;

    /* area id at 0x340: ROM does add r0,r4,#0x300; ldrsb r0,[r0,#0x40] */
    if (IsAreaShowing(*(s8 *)((u8 *)(((int)c + 0x300) & 0xFFFFFFFFFFFFFFFFLL) + 0x40)) == 0) {
        *(s8 *)(c + 0xcc) = *(s8 *)((u8 *)(((unsigned)c + 0x300)) + 0x40);
        if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124) != 0) {
            _ZN16MeshColliderBase7DisableEv(c + 0x124);
        }
    } else {
        if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124) == 0) {
            _ZN16MeshColliderBase6EnableEP5Actor(c + 0x124, c);
        }
    }

    t = *(int *)(c + 0x334);
    if (*(int *)(c + 0x60) != t) {
        if (*(int *)(c + 0x60) < t) {
            int *p = (int *)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p + 0xa000;
            if (*(int *)(c + 0x60) > *(int *)(c + 0x334))
                *(int *)(c + 0x60) = *(int *)(c + 0x334);
        } else {
            int *p = (int *)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p - 0xa000;
            if (*(int *)(c + 0x60) < *(int *)(c + 0x334))
                *(int *)(c + 0x60) = *(int *)(c + 0x334);
        }
    }

    if (*(int *)(c + 0x60) != *(int *)(c + 0x6c)) {
        *(int *)(c + 0x33c) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(unsigned *)(c + 0x33c), 3, 0x96, c + 0x74, 0);
    }

    {
        /* angle at 0x342: pool offset 0x342 */
        s16 *q = (s16 *)(((int)c + 0x342) & 0xFFFFFFFFFFFFFFFFLL);
        *q = (s16)(*q + 0x200);
        /* index from (u16)angle at 0x342 via 0x300+0x42 */
        i = *(u16 *)((u8 *)(((unsigned)c + 0x300)) + 0x42) >> 4;
        *(int *)(c + 0x344) = (int)data_02082214[i * 2] * 0xf + *(int *)(c + 0x60);
    }
    data_0209f32c = *(int *)(c + 0x344);
    func_ov029_021122b4(c);
    func_ov029_02112250(c);
    *(int *)(c + 0x32c) = 0x1000;
    _ZN9Animation7AdvanceEv(c + 0x320);
    return 1;
}
