typedef unsigned char u8;
typedef unsigned short u16;
typedef struct { int x, y, z; } Vec3;

extern u8 IsAreaShowing(int idx);
extern char *_ZN5Actor13ClosestPlayerEv(void *self);
extern int Vec3_Dist(const Vec3 *a, const Vec3 *b);
extern short Vec3_HorzAngle(const Vec3 *a, const Vec3 *b);
extern int _ZN5Sound15PlaySecretSoundEP5ActorPt(void *actor, u16 *p);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, const Vec3 *pos, unsigned int d);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);

void func_ov063_0211c89c(char *c) {
    Vec3 pp;
    char *p;

    *(char*)(c + 0xcc) = ~0;
    if (IsAreaShowing(*(signed char*)(c + 0x100 + 0x58)) == 0) {
        *(u8*)(c + 0x157) = 0;
        *(char*)(c + 0xcc) = *(signed char*)(c + 0x100 + 0x58);
        return;
    }
    p = _ZN5Actor13ClosestPlayerEv(c);
    {
        int *q = (int*)(((long long)(int)(p + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        pp.x = q[0];
        pp.y = q[1];
        pp.z = q[2];
    }
    if (pp.x >= (int)0xffa24000)
        *(u8*)(c + 0x156) = 0;
    else
        *(u8*)(c + 0x156) = 1;

    switch (*(u8*)(c + 0x150)) {
    case 0:
        if ((*(u8*)(c + 0x157) & 8) == 0 &&
            Vec3_Dist((Vec3*)(c + 0x5c), &pp) < 0x320000) {
            short a = Vec3_HorzAngle((Vec3*)(c + 0x5c), &pp);
            if (a <= -0x7000 || a >= 0x7000)
                *(u8*)(((long long)(int)(c + 0x157)) & 0xFFFFFFFFFFFFFFFFLL) |= 8;
        }
        {
            u8 m = *(u8*)(c + 0x157) & 7;
            if (m == 7) {
                *(u8*)(c + 0x150) = 1;
                return;
            }
            if (m == 1) return;
            if (m == 3) return;
            *(u8*)(((long long)(int)(c + 0x157)) & 0xFFFFFFFFFFFFFFFFLL) &= 8;
        }
        return;
    case 1:
        {
            u16 t = *(u16*)(c + 0x100 + 0x4c);
            if (t > 0x64) {
                *(u8*)(c + 0x150) = 2;
                return;
            }
            if (t < 0x1e) return;
            _ZN5Sound15PlaySecretSoundEP5ActorPt(c, (u16*)(c + 0x14e));
        }
        return;
    case 2:
        *(int*)(((long long)(int)(c + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL) += 0x5000;
        *(int*)(c + 0x148) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int*)(c + 0x148), 3, 0x8d, (Vec3*)(c + 0x74), 0);
        _ZN5Sound15PlaySecretSoundEP5ActorPt(c, (u16*)(c + 0x14e));
        if (*(u16*)(c + 0x100 + 0x4c) > 0x65)
            *(u8*)(c + 0x150) = 3;
        return;
    case 3:
        if (_ZN5Sound15PlaySecretSoundEP5ActorPt(c, (u16*)(c + 0x14e)) == 0)
            return;
        _ZN9ActorBase18MarkForDestructionEv(c);
        return;
    }
}
