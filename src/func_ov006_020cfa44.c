typedef long long s64;
typedef short s16;
typedef unsigned short u16;

typedef struct { int x, y, z; } Vec3;

#define LA(p) ((int)(((s64)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))

extern s16 data_02082214[];
extern void func_ov006_020cfc74(char *o);
extern int _Z14ApproachLinearRiii(int *cur, int target, int step);
extern int _Z15ApproachLinear2Rsss(short *cur, short target, short step);
extern void func_ov006_020cf124(char *o);
extern void func_ov006_020cf804(char *o);

void func_ov006_020cfa44(char *o)
{
    int j, i;
    int a14, a18;
    int k;
    s16 s;
    int ns;
    int mulX, mulY;
    int zeroArg;
    int izero;
    int stepArg;
    Vec3 *src;
    Vec3 *dst;
    int *ratep;
    u16 *p320;

    func_ov006_020cfc74(o);

    p320 = (u16 *)LA(o + 0x320);
    *p320 = *p320 + *(u16 *)(o + 0x322);

    a14 = *(int *)(o + 0x14);
    a18 = *(int *)(o + 0x18);

    k = *(u16 *)(o + 0x320) >> 4;
    s = data_02082214[k * 2];
    ns = -(int)s;

    src = (Vec3 *)(o + 0x11c);
    dst = (Vec3 *)(o + 0x5c);

    mulX = (int)(((s64)a14 * ns + 0x800) >> 12);
    mulY = (int)(((s64)a18 * ns + 0x800) >> 12);

    ratep = (int *)(o + 0x29c);

    zeroArg = 0;
    izero = 0;
    stepArg = 0x100;

    for (j = 0; j < 4; j++) {
        for (i = izero; i < 4; i++) {
            dst->x = src->x;
            dst->y = src->y;
            dst->z = src->z;
            dst->x += (int)(((s64)(*ratep) * mulX + 0x800) >> 12);
            dst->y += (int)(((s64)(*ratep) * mulY + 0x800) >> 12);

            _Z14ApproachLinearRiii(ratep, zeroArg, stepArg);
            src++;
            dst++;
            ratep++;
        }
    }

    func_ov006_020cf124(o);

    if (*(s16 *)(o + 0x31e) == 3)
        return;

    if (_Z15ApproachLinear2Rsss((short *)(o + 0x31c), 0, 1) == 0)
        return;

    func_ov006_020cf804(o);
}
