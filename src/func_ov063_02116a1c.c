extern unsigned NumStars(void);
extern void _ZN9ActorBase18MarkForDestructionEv(void *o);
extern void func_0201267c(int a, void *b);
extern void _Z14ApproachLinearRiii(int *p, int t, int s);
extern int Vec3_HorzAngle(void *a, void *b, int c);
extern void _Z14ApproachLinearRsss(short *p, int t, int s);
extern void func_ov063_0211a964(void *c, int b);

typedef struct { int x, y, z; } V3;

#pragma opt_common_subs off
void func_ov063_02116a1c(void *cc)
{
    char *c = (char *)cc;
    V3 v;
    int a;
    int scale;

    v.x = *(int *)(c + 0x51c);
    v.y = *(int *)(c + 0x520);
    v.z = *(int *)(c + 0x524);
    *(int *)(c + 0x584) = 0x2000;

    a = *(unsigned char *)(c + 0x5cc);
    if (a == 0) {
        *(unsigned short *)(((int)c + 0x5d4)) &= ~8;
        if (NumStars() < 0xf) {
            _ZN9ActorBase18MarkForDestructionEv(c);
            return;
        }
        if (((unsigned)(*(unsigned short *)(c + 0x5d4)) << 0x1b) >> 0x1f) {
            unsigned short *fp = (unsigned short *)(((long long)(int)(c + 0x5d4)));
            unsigned char *st = (unsigned char *)(((long long)(int)(c + 0x5cc)));
            *fp |= 8;
            *(unsigned char *)(c + 0x5c8) = 0xb4;
            scale = *(int *)(c + 0x584);
            *(int *)(c + 0x80) = scale;
            *(int *)(c + 0x84) = scale;
            *(int *)(c + 0x88) = scale;
            *(int *)(c + 0x188) = *(int *)(c + 0x590) * *(int *)(c + 0x584);
            *(int *)(c + 0x18c) = *(int *)(c + 0x594) * *(int *)(c + 0x584);
            *st += 1;
        }
    } else if (a == 1) {
        if (*(int *)(c + 0x580) < 0x3e8000) {
            unsigned char *st = (unsigned char *)(((long long)(int)(c + 0x5cc)));
            *st += 1;
            func_0201267c(0xf8, c + 0x74);
        }
        *(int *)(c + 0x98) = 0;
    } else {
        int t;
        _Z14ApproachLinearRiii((int *)(c + 0x98), 0x30000, 0x1800);
        t = 0x3e8000;
        v.x = -t;
        v.z = (int)0xfdcd8000;
        if (*(int *)(c + 0x64) < (int)0xfec78000) {
            /* invert: laundered RMW as THEN, plain zero as ELSE
               -> movls/strbls + bls + unpredicated RMW */
            if (*(unsigned char *)(c + 0x5c8) > 0x14) {
                unsigned char *p = (unsigned char *)(((long long)(int)(c + 0x5c8)));
                *p = (unsigned char)(*p - 0x14);
            } else {
                *(unsigned char *)(c + 0x5c8) = 0;
            }
        }
    }

    *(int *)(c + 0xa8) = 0;
    a = Vec3_HorzAngle(c + 0x5c, &v, 0);
    _Z14ApproachLinearRsss((short *)(c + 0x94), a, 0x5a8);
    func_ov063_0211a964(c, 1);
}
