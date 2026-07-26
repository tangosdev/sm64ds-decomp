typedef unsigned char u8;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

typedef struct Vector3 { int x, y, z; } Vector3;

extern void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern int func_020091f8(void *a, void *b, int c, int d);
extern unsigned int func_020093f4(void *p, int x);
extern int func_02009138(int *thiz, int arg);
extern void func_020089d8(void *p);

extern s32 data_02086e90[3];
extern unsigned char data_020a0e40[];
extern short data_0209f4a2[];
extern short data_0209f4a4[];

int func_02009aa8(char *self)
{
    s32 tmp[3];
    Vector3 delta;
    Vector3 src;

    src.x = data_02086e90[0];
    src.y = data_02086e90[1];
    src.z = data_02086e90[2];

    Vec3_RotateYAndTranslate(tmp,
        (int *)(*(char **)(self + 0x110) + 0x5c),
        *(s16 *)(*(char **)(self + 0x110) + 0x8e),
        (int *)&src);

    if (*(u8 *)(self + 0x1a6) != 0) {
        char *base;
        int r;

        Vec3_Sub(&delta,
            (Vector3 *)(*(char **)(self + 0x110) + 0x5c),
            (Vector3 *)(self + 0x98));
        AddVec3((Vector3 *)(self + 0x80), &delta, (Vector3 *)(self + 0x80));
        AddVec3((Vector3 *)(self + 0x8c), &delta, (Vector3 *)(self + 0x8c));

        base = (char *)(((long long)(int)(*(char **)(self + 0x110) + 0x5c)) & ~0ULL);
        *(s32 *)(self + 0x98) = *(s32 *)(base + 0);
        *(s32 *)(self + 0x9c) = *(s32 *)(base + 4);
        *(s32 *)(self + 0xa0) = *(s32 *)(base + 8);

        r = func_020091f8(self, tmp, *(s16 *)(self + 0x186), 0);
        if (r != 0) *(u8 *)(self + 0x1a6) = 0;

        *(s32 *)(self + 0xa4) = 0;
        *(s32 *)(self + 0xac) = 0;
    } else {
        s16 newAngle;
        int idx;
        s16 t1, t2;
        s16 rem;
        int diff2;
        int clamped;
        s16 v17e;

        newAngle = *(s16 *)(*(char **)(self + 0x110) + 0x8e) + 0x8000;
        *(s16 *)(((long long)(int)(self + 0x17c)) & ~0ULL) += newAngle - *(s16 *)(self + 0x186);
        *(s16 *)(self + 0x186) = newAngle;

        *(s32 *)(self + 0x80) = tmp[0];
        *(s32 *)(self + 0x84) = tmp[1];
        *(s32 *)(self + 0x88) = tmp[2];

        idx = data_020a0e40[0] * 0x18;
        t1 = *(s16 *)((char *)data_0209f4a2 + idx);
        *(s16 *)(((long long)(int)(self + 0x17c)) & ~0ULL) -= (int)(((long long)t1 * 0x200 + 0x800) >> 12);

        diff2 = (s16)(*(s16 *)(self + 0x17c) - newAngle);
        if (diff2 < -0x2000) {
            clamped = -0x2000;
        } else {
            if (diff2 > 0x2000) {
                clamped = 0x2000;
            } else {
                clamped = diff2;
            }
        }
        rem = diff2 - clamped;

        *(s16 *)(((long long)(int)(*(char **)(self + 0x110) + 0x8e)) & ~0ULL) += rem;
        *(s16 *)(((long long)(int)(self + 0x186)) & ~0ULL) += rem;

        t2 = *(s16 *)((char *)data_0209f4a4 + idx);
        *(s16 *)(((long long)(int)(self + 0x17e)) & ~0ULL) += (int)(((long long)t2 * 0x200 + 0x800) >> 12);

        v17e = *(s16 *)(self + 0x17e);
        if (v17e < -0x1c00) {
            v17e = -0x1c00;
        } else if (v17e > 0x1c00) {
            v17e = 0x1c00;
        }
        *(s16 *)(self + 0x17e) = v17e;
    }

    func_02009138((int *)self, func_020093f4(self, 0x48000));
    *(u32 *)(((long long)(int)(self + 0x154)) & ~0ULL) |= 0x80000;
    func_020089d8(self);

    return 1;
}
