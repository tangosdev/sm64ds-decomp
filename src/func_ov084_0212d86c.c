typedef unsigned int u32;
typedef unsigned char u8;
typedef unsigned short u16;

extern void func_02012694(u32 id, void *pos);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, void *pos);
extern void _ZN5Enemy9SpawnCoinEv(char *self);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(char *self);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *anim, int a, int fix, u32 flags);
extern char *_ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN6Player16IncMegaKillCountEv(void *p);
extern int _ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(char *self, void *clsn, char *player);
extern void _ZN6Player6BounceE5Fix12IiE(char *p, int fix);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(char *self, const void *pos, u32 a, int fix, u32 b, u32 c, u32 d);

extern int data_ov084_02130e24[];

void func_ov084_0212d86c(char *r5)
{
    char *r4;
    int t;
    int flags;
    int pos1[3];
    int pos2[3];
    u32 id;

    id = *(u32 *)(r5 + 0x198);
    if (id == 0)
        goto second;

    flags = *(int *)(r5 + 0x194) & 0x66ff0;
    if (flags != 0) {
        t = (int)(*(u16 *)(r5 + 0xc) == 0xfb);
        if (t != 0)
            func_02012694(0x1e, r5 + 0x74);
        else
            _ZN5Sound9PlayBank0EjRK7Vector3(0xa, r5 + 0x74);
    activate_path:
        t = (int)(*(u16 *)(r5 + 0xc) == 0xfc);
        if (t != 0) {
            *(u8 *)(r5 + 0x108) = 1;
            _ZN5Enemy9SpawnCoinEv(r5);
            _ZN5Actor24KillAndTrackInDeathTableEv(r5);
            _ZN5Sound9PlayBank0EjRK7Vector3(0xa, r5 + 0x74);
        } else {
            *(int *)(r5 + 0x1ec) = 1;
            *(u8 *)(r5 + 0x21d) = 0xa;
            *(u16 *)(r5 + 0x218) = 0x1f40;
            {
                u32 *p18c = (u32 *)(((long long)(int)(r5 + 0x18c)) & 0xFFFFFFFFFFFFFFFFLL);
                u32 *pb0 = (u32 *)(((long long)(int)(r5 + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL);
                *p18c |= 1u;
                *pb0 &= ~0x10000000u;
            }
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(r5 + 0x110, (void *)data_ov084_02130e24[1], 0x40000000, 0x1000, 0);
            *(u8 *)(r5 + 0x21e) = 0;
            *(int *)(r5 + 0x224) = 0;
        }
        if ((*(int *)(r5 + 0x194) & 0x10) == 0)
            goto second;
        _ZN5Sound9PlayBank0EjRK7Vector3(0xa, r5 + 0x74);
        r4 = _ZN5Actor10FindWithIDEj(*(u32 *)(r5 + 0x198));
        if (r4 == 0)
            goto second;
        _ZN6Player16IncMegaKillCountEv(r4);
        func_02012694(0x1d, r5 + 0x74);
        goto second;
    }

    r4 = _ZN5Actor10FindWithIDEj(id);
    if (r4 == 0)
        goto second;
    t = (int)(*(u16 *)(r4 + 0xc) == 0xbf);
    if (t == 0)
        goto second;
    if (*(u8 *)(r4 + 0x6f9) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(0xa, r5 + 0x74);
        goto activate_path;
    }
    t = (int)(*(u16 *)(r5 + 0xc) == 0xfc);
    if (t != 0) {
        if (_ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(r5, r5 + 0x174, r4) != 0) {
            _ZN5Sound9PlayBank0EjRK7Vector3(0xb6, r5 + 0x74);
            _ZN6Player6BounceE5Fix12IiE(r4, 0x28000);
            goto activate_path;
        }
    }
    if (*(u8 *)(r4 + 0x6fb) != 0)
        goto second;
    t = (int)(*(u16 *)(r5 + 0xc) == 0xfb);
    if (t != 0)
        goto second;
    pos1[0] = *(int *)(r5 + 0x5c);
    pos1[1] = *(int *)(r5 + 0x60);
    pos1[2] = *(int *)(r5 + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, pos1, 2, 0xc000, 1, 0, 1);

second:
    id = *(u32 *)(r5 + 0x1cc);
    if (id == 0)
        return;
    r4 = _ZN5Actor10FindWithIDEj(id);
    if (r4 == 0)
        return;
    t = (int)(*(u16 *)(r4 + 0xc) == 0xbf);
    if (t == 0)
        return;

    flags = *(int *)(r5 + 0x1c8) & 0x66ff0;
    if (flags != 0) {
        if ((flags & 0x10) != 0) {
            _ZN6Player16IncMegaKillCountEv(r4);
            func_02012694(0x1d, r5 + 0x74);
        } else {
            t = (int)(*(u16 *)(r5 + 0xc) == 0xfb);
            if (t != 0)
                func_02012694(0x1e, r5 + 0x74);
            else
                _ZN5Sound9PlayBank0EjRK7Vector3(0xa, r5 + 0x74);
        }
        *(int *)(r5 + 0x1ec) = 1;
        *(u8 *)(r5 + 0x21d) = 0xa;
        *(u16 *)(r5 + 0x218) = 0x1f40;
        {
            u32 *p18c = (u32 *)(((long long)(int)(r5 + 0x18c)) & 0xFFFFFFFFFFFFFFFFLL);
            u32 *pb0 = (u32 *)(((long long)(int)(r5 + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL);
            *p18c |= 1u;
            *pb0 &= ~0x10000000u;
        }
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(r5 + 0x110, (void *)data_ov084_02130e24[1], 0x40000000, 0x1000, 0);
        *(u8 *)(r5 + 0x21e) = 0;
        *(int *)(r5 + 0x224) = 0;
        return;
    }

    if (*(u8 *)(r4 + 0x6f9) != 0)
        return;
    if (*(u8 *)(r4 + 0x6fb) != 0)
        return;
    pos2[0] = *(int *)(r5 + 0x5c);
    pos2[1] = *(int *)(r5 + 0x60);
    pos2[2] = *(int *)(r5 + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, pos2, 2, 0xc000, 1, 0, 1);
}
