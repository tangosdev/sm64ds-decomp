typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef short s16;

enum { false, true };

typedef struct { int x, y, z; } Vector3;

extern void func_ov073_0212157c(void *c, void *p);
extern s16 _ZN5Actor18HorzAngleToCPlayerEv(void *self);
extern int AngleDiff(int a, int b);
extern void func_02012694(int a, void *p);
extern void _Z14ApproachLinearRiii(int *p, int a, int b);
extern int _ZN5Actor13DistToCPlayerEv(void *self);
extern unsigned int RandomIntInternal(void *seed);
extern void _Z14ApproachLinearRsss(s16 *p, s16 a, s16 b);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int a, int b, int cc);
extern void *_ZN5Actor13ClosestPlayerEv(void *self);
extern int _ZN6Player12GetHurtStateEv(void *self);
extern s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *bca, int a, int b, int frame, u16 flags);
extern int _ZNK9Animation12WillHitFrameEi(void *self, int f);
extern void func_ov073_0211f2c0(void *c, int a);

extern void *data_ov073_02123390;
extern void *data_0209e650;
extern int data_ov073_02123288[];

int func_ov073_02120ed0(void *self)
{
    u8 *c = (u8 *)self;

    if (c[0x4c9] == 1) {
        func_ov073_0212157c(c, &data_ov073_02123390);
        return 1;
    }

    switch (c[0x4c8]) {
    case 0:
        if (*(u16 *)(c + 0x100) == 0) {
            *(s16 *)(c + 0x4c6) = _ZN5Actor18HorzAngleToCPlayerEv(c);
            *(int *)(c + 0x4b4) = 0;
            if (AngleDiff(*(s16 *)(c + 0x94), *(s16 *)(c + 0x4c6)) <= 0x2000) {
                *(s16 *)(c + 0x100) = 0x1e;
                break;
            } else {
                *(s16 *)(c + 0x100) = 0;
                *(int *)(c + 0x4d0) = 0x1000;
                (*(u8 *)(((int)c + 0x4c8) & 0xFFFFFFFFFFFFFFFF))++;
                func_02012694(0x169, c + 0x74);
                break;
            }
        } else {
            _Z14ApproachLinearRiii((int *)(c + 0x98), 0x1e000, 0x3000);
            if (*(int *)(c + 0x4b4) == 0) {
                if (_ZN5Actor13DistToCPlayerEv(c) < 0x1f4000) {
                    *(int *)(c + 0x4b4) = 1;
                    if (((RandomIntInternal(&data_0209e650) >> 0x18) & 7) == 0) {
                        c[0x4c8] = 3;
                        *(int *)(c + 0x98) = 0;
                        *(s16 *)(c + 0x100) = 0xf;
                    }
                }
            }
            _Z14ApproachLinearRsss((s16 *)(c + 0x94), *(s16 *)(c + 0x4c6), 0x1d0);
            *(s16 *)(c + 0x8c) = *(s16 *)(c + 0x92);
            *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);
            *(s16 *)(c + 0x90) = *(s16 *)(c + 0x96);
            break;
        }

    case 3:
        if (*(u16 *)(c + 0x100) == 0) {
            *(int *)(c + 0x4d0) = 0x1000;
            c[0x4c8] = 1;
        }
        break;

    case 1:
    case 2:
    {
        int d;
        int i;
        u8 *r5;
        *(int *)(c + 0x4b4) = 0;
        d = *(int *)(c + 0x98);
        if (d < 0) d = -d;
        if (d > 0xa) {
            r5 = c;
            i = 0;
            do {
                _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(
                    *(int *)(r5 + 0x4d4), *(int *)(r5 + 0x4d8), *(int *)(r5 + 0x4dc));
                i++;
                r5 += 0xc;
            } while (i < 2);
        }
        _Z14ApproachLinearRiii((int *)(c + 0x98), 0, *(int *)(c + 0x4d0));
        if (c[0x4c8] == 1) {
            *(s16 *)(c + 0x4c6) = _ZN5Actor18HorzAngleToCPlayerEv(c);
            _Z14ApproachLinearRsss((s16 *)(c + 0x8e), *(s16 *)(c + 0x4c6), 0x500);
        }
        d = *(int *)(c + 0x98);
        if (d < 0) d = -d;
        if (d >= 0xa) break;
        if (*(u16 *)(c + 0x100) != 0) break;
        if (c[0x4c8] == 2) {
            u8 *p = (u8 *)_ZN5Actor13ClosestPlayerEv(c);
            if (p != 0) {
                int t;
                if (_ZN6Player12GetHurtStateEv(p) == 4) goto hz;
                if (_ZN6Player12GetHurtStateEv(p) == 5) goto hz;
                t = p[0x709] ? 1 : 0;
                if (t == 1) {
hz:
                    *(s16 *)(c + 0x4c6) = Vec3_HorzAngle((Vector3 *)(c + 0x5c), (Vector3 *)(c + 0x3d8));
                } else {
                    *(s16 *)(c + 0x4c6) = _ZN5Actor18HorzAngleToCPlayerEv(c);
                }
            }
        }
        *(int *)(c + 0x98) = 0;
        *(s16 *)(c + 0x94) = *(s16 *)(c + 0x8e);
        *(s16 *)(c + 0x100) = 0x1e;
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            c + 0x30c, (void *)data_ov073_02123288[1], 4, 0, 0x1000, 0);
        *(int *)(c + 0x368) = 0x2000;
        *(int *)(c + 0x4b4) = 0;
        c[0x4c8] = 0;
        break;
    }
    }

    if (c[0x4c8] == 0) {
        if (_ZNK9Animation12WillHitFrameEi(c + 0x35c, 0) != 0 ||
            _ZNK9Animation12WillHitFrameEi(c + 0x35c, 0xe) != 0) {
            func_ov073_0211f2c0(c, 0x3e8000);
            func_02012694(0x168, c + 0x74);
        }
    }
    return 1;
}