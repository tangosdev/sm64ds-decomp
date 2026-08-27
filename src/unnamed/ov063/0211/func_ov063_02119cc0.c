typedef short s16;
typedef unsigned short u16;
typedef struct { int x, y, z; } Vector3;

extern int func_ov063_0211a8a4(char *thiz);
extern int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
extern int RandomIntInternal(int *seed);
extern void _Z14ApproachLinearRsss(s16 *val, s16 target, s16 step);
extern void func_ov063_0211a030(char *c, int a, int b);
extern void func_ov063_0211a964(char *c, int arg1);
extern void func_ov063_0211a960(char *c);
extern int data_0209e650;

void func_ov063_02119cc0(char *c, int unused, s16 a2, int a3)
{
    if (func_ov063_0211a8a4(c) == 0) goto reset;

    if (Vec3_HorzDist((Vector3 *)(c + 0x5c), (Vector3 *)(c + 0x51c)) >= *(int *)(c + 0x59c)) {
        *(s16 *)(c + 0x5bc) = Vec3_HorzAngle((Vector3 *)(c + 0x5c), (Vector3 *)(c + 0x51c));
        *(u16 *)(c + 0x100) = 0;
    } else if (Vec3_HorzDist((Vector3 *)(c + 0x51c), (Vector3 *)(*(char **)(c + 0x484) + 0x5c)) < *(int *)(c + 0x59c)) {
        *(s16 *)(c + 0x5bc) = *(s16 *)(c + 0x5b0);
        *(u16 *)(c + 0x100) = 0;
    } else {
        unsigned rnd = (unsigned)RandomIntInternal(&data_0209e650) >> 16 & 0x3f;
        if (*(u16 *)(c + 0x100) >= rnd + 0x5a) {
            int rnd2 = (int)((unsigned)RandomIntInternal(&data_0209e650) >> 16 & 0x3fff) - 0x2000;
            *(s16 *)(c + 0x5bc) = *(s16 *)(c + 0x94) + rnd2;
            *(u16 *)(c + 0x100) = 0;
        }
    }

    _Z14ApproachLinearRsss((s16 *)(c + 0x94), *(s16 *)(c + 0x5bc), a2);
    *(int *)(c + 0xa8) = 0;
    func_ov063_0211a030(c, 0xa000 - *(int *)(c + 0x588), a3);

    if (*(int *)(c + 0x98) != 0) {
        func_ov063_0211a964(c, 0);
    } else {
        func_ov063_0211a960(c);
    }
    return;

reset:
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0xa8) = 0;
    func_ov063_0211a960(c);
}
