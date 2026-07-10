typedef long long s64;
struct Vector3 { int x, y, z; };

extern signed char data_0209f2f8;
extern int data_02086f38[];
extern int data_02086f48[];
extern short data_02082214[];
extern char data_0209f43c[];

extern short Vec3_HorzAngle(struct Vector3 *v0, struct Vector3 *v1);
extern void _ZN7Clipper13Func_020156DCEv(void *self, int a, int b, int c, int d);

void func_0200af20(char *c, struct Vector3 *v1, struct Vector3 *v2, short *out)
{
    struct Vector3 *f = *(struct Vector3 **)(c + 0x148);
    int *b;
    unsigned short ang;
    int idx;
    int y;
    int z;

    if (f == 0)
        return;

    b = 0;
    {
        signed char g = data_0209f2f8;
        if (g == 6) {
            b = data_02086f38;
        } else if (g == 0x15) {
            if (*(int *)(c + 0x9c) < 0)
                return;
            b = data_02086f48;
        }
    }

    z = f->z;
    y = f->y;
    y = y + *(int *)(c + 0x9c);
    v1->x = f->x;
    v1->y = y >> 1;
    v1->z = z;
    ang = Vec3_HorzAngle(v1, (struct Vector3 *)(c + 0x98));
    idx = (ang >> 4) * 2;

    v2->y = *(int *)(c + 0x9c) + 0x8f7690;
    v2->x = *(int *)(c + 0x98) + (int)(((s64)data_02082214[idx] * 0x15a6f80 + 0x800) >> 12);
    v2->z = *(int *)(c + 0xa0) + (int)(((s64)data_02082214[idx + 1] * 0x15a6f80 + 0x800) >> 12);

    if (b != 0) {
        {
            int lo = b[1];
            int hi = b[0];
            int x = v2->x;
            if (x >= lo) {
                if (x <= hi)
                    hi = x;
                lo = hi;
            }
            v2->x = lo;
        }
        {
            int lo = b[3];
            int hi = b[2];
            int x = v2->z;
            if (x >= lo) {
                if (x <= hi)
                    hi = x;
                lo = hi;
            }
            v2->z = lo;
        }
    }

    *out = 0xe38;
    _ZN7Clipper13Func_020156DCEv(data_0209f43c, *(int *)(c + 0xf8), *out, *(int *)(c + 0xfc), *(int *)(c + 0x100));
    *(int *)(int)(((long long)(int)(c + 0x154)) & 0xFFFFFFFFFFFFFFFFLL) &= ~1;
}
