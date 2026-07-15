typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef long long s64;

struct Vec3 { int x, y, z; };

extern int _Z14ApproachLinearRiii(s32 *p, int a, int b);
extern int _ZN9Animation8FinishedEv(void *a);
extern void func_0201267c(int a, void *p);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *o, void *f, int a, int fx, int j);
extern void *_ZN5Actor13ClosestPlayerEv(void *c);
extern s16 Vec3_HorzAngle(const void *v0, const void *v1);
extern int _Z14ApproachLinearRsss(s16 *p, short a, short b);
extern int _ZNK9Animation12WillHitFrameEi(void *o, int f);
extern void _ZN5Actor13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
    void *self, const void *pos, const void *v16, int a, int b, u32 g);

extern void *data_ov084_02130e1c[];
extern s16 data_02082214[];

void func_ov084_0212ddbc(char *c)
{
    struct Vec3 pos;
    s16 ang;
    int b;
    int speed;
    int idx;
    int dx;
    int dz;
    int y;
    int tmp;
    void *player;

    if (_Z14ApproachLinearRiii((s32 *)(c + 0x204), *(s32 *)(c + 0x210), *(s32 *)(c + 0x214)) == 0)
        goto cold;

    if (_ZN9Animation8FinishedEv(c + 0x160) != 0) {
        b = (int)(*(u16 *)(c + 0xc) == 0xfc);
        if (b != 0)
            func_0201267c(0xe3, c + 0x74);
        else
            func_0201267c(0x120, c + 0x74);
        *(s32 *)(c + 0x1ec) = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, data_ov084_02130e1c[1], 0, 0x1000, 0);
    } else {
        if (*(u16 *)(c + 0x100) < 0x3a) {
            ang = *(s16 *)(c + 0x8e);
            player = _ZN5Actor13ClosestPlayerEv(c);
            if (player != 0)
                ang = Vec3_HorzAngle(c + 0x5c, (char *)player + 0x5c);
            _Z14ApproachLinearRsss((s16 *)(c + 0x8e), ang, 0x400);
        }
    }

    if (_ZNK9Animation12WillHitFrameEi(c + 0x160, 0x3a) == 0)
        return;

    b = (int)(*(u16 *)(c + 0xc) == 0xfc);
    if (b != 0)
        func_0201267c(0x105, c + 0x74);
    else
        func_0201267c(0x122, c + 0x74);

    pos.x = *(s32 *)(c + 0x5c);
    y = *(s32 *)(c + 0x60);
    pos.y = y;
    pos.z = *(s32 *)(c + 0x64);
    speed = *(s32 *)(c + 0x210);
    idx = *(u16 *)(c + 0x8e) >> 4;
    tmp = speed * 0x3c;
    dx = (s32)(((s64)tmp * data_02082214[idx * 2] + 0x800) >> 12);
    pos.x = pos.x + dx;

    idx = *(u16 *)(c + 0x8e) >> 4;
    speed = *(s32 *)(c + 0x210);
    tmp = speed * 0x3c;
    dz = (s32)(((s64)tmp * data_02082214[idx * 2 + 1] + 0x800) >> 12);
    pos.z = pos.z + dz;

    speed = *(s32 *)(c + 0x210);
    pos.y = y + speed * 0x5a;
    *(s16 *)(c + 0x8c) = 0x1000;

    _ZN5Actor13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
        c, &pos, c + 0x8c, 0x1e000, 0xa000, 3);
    return;

cold:
    if (*(s32 *)(c + 0x204) <= (*(s32 *)(c + 0x210) >> 1))
        return;
    {
        u32 *p18c = (u32 *)(((long long)(int)(c + 0x18c)) & 0xFFFFFFFFFFFFFFFFLL);
        u32 *pb0 = (u32 *)(((long long)(int)(c + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL);
        *p18c &= ~1u;
        *pb0 |= 0x10000000u;
    }
}
