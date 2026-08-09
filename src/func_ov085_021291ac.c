#pragma opt_propagation off
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
typedef struct { int x, y, z; } Vector3;
extern int func_ov085_021290b4(char *c);
extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern int _ZN6Player12GetTalkStateEv(char *p);
extern int _Z14ApproachLinearRsss(s16 *cur, s16 tgt, s16 step);
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, int fix, int loop);
extern void _ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(char *p, char *actor, u32 msg, const Vector3 *pos, u32 d, u32 e);
extern int IsStarCollectedInCurLevel(void);
extern u8 NumStars(void);
extern char *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 param, const void *pos, const void *rot, int a, int b);
extern int func_02013a44(void);
extern char *_ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN9ActorBase18MarkForDestructionEv(char *self);
extern void _ZN5Actor13SpawnSoundObjEj(char *self, u32 id);
extern void _ZN7Message7EndTalkEv(void);
extern void func_ov085_02129524(char *c, int i);
extern s8 data_0209f2f8;
extern int data_0209caa0[];
extern u8 data_ov085_0212f27c[];
#define M(p) ((long long)(int)(p))
void func_ov085_021291ac(char *c)
{
    char *p;
    int r4;
    s16 ang;
    Vector3 pv;
    Vector3 pos;
    Vector3 pos2;
    int st;

    p = *(char **)(c + 0x1f8);
    r4 = func_ov085_021290b4(c);
    {
        int *ps = (int *)(int)M(p + 0x5c);
        pv.x = ps[0];
        pv.y = ps[1];
        pv.z = ps[2];
    }
    ang = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &pv);
    st = _ZN6Player12GetTalkStateEv(p);
    switch (st) {
    case 0: {
        if (_Z14ApproachLinearRsss((s16 *)(c + 0x8e), ang, 0x800) == 0)
            return;
        if (data_0209f2f8 == 0x32) {
            if ((data_0209caa0[1] & 0x40000000) == 0)
                data_0209caa0[1] |= 0x40000000;
        }
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x25, 0x14, 0x7f, 0x15666, 1) == 0)
            return;
        {
            int y = *(int *)(c + 0x60);
            int z = *(int *)(c + 0x64);
            int x;
            y = y + 0x46000;
            pos.x = *(int *)(c + 0x5c);
            pos.y = y;
            pos.z = z;
            (void)x;
        }
        _ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(p, c, (u32)(s16)r4, &pos, 0, 0);
        return;
    }
    case 1:
        return;
    default:
        break;
    }

    if (*(u8 *)(c + 0x20a) == 0) {
        *(u8 *)(c + 0x20a) = 1;
        if (*(u8 *)(c + 0x20c) != 0xff) {
            if (IsStarCollectedInCurLevel() == 0) {
                if (NumStars() >= data_ov085_0212f27c[*(u8 *)(c + 0x20f)]) {
                    pos2.x = *(int *)(c + 0x5c);
                    pos2.y = *(int *)(c + 0x60);
                    pos2.z = *(int *)(c + 0x64);
                    pos2.y = pos2.y + 0xc8000;
                    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                        0xb2, *(u8 *)(c + 0x20c) | 0x20, &pos2, 0, *(s8 *)(c + 0xcc), -1);
                    {
                        u16 *p208 = (u16 *)(int)M(c + 0x208);
                        *p208 = (u16)(*p208 + 1);
                    }
                }
            }
        }
    }

    if (*(u8 *)(c + 0x20b) == 1) {
        if (func_02013a44() != 0) {
            u32 id = *(u32 *)(c + 0x1f4);
            if (id != 0) {
                char *found = _ZN5Actor10FindWithIDEj(id);
                if (found != 0) {
                    _ZN9ActorBase18MarkForDestructionEv(found);
                    *(int *)(c + 0x1f4) = 0;
                    _ZN5Actor13SpawnSoundObjEj(c, 1);
                    {
                        u32 k = 0x13;
                        u8 plb = *(u8 *)(p + 0x6d9);
                        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                            0x10d, k | ((u32)plb << 8), &pv, 0, *(s8 *)(c + 0xcc), -1);
                    }
                }
            }
        }
    }
    if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x25, 0x7f, 0, 0x7222, 0) == 0)
        return;
    _ZN7Message7EndTalkEv();
    func_ov085_02129524(c, 0);
}
