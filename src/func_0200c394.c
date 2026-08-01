typedef struct { int x, y, z; } Vec3;

extern signed char data_0209f2f8;
extern int data_02087274, data_0208706c, data_0208715c, data_0208747c;
extern int data_02087184, data_020871fc, data_02087224, data_0208733c, data_02087314;

void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
int LenVec3(Vec3 *v);
int _ZN4cstd4fdivEii(int a, int b);
void Vec3_MulScalarInPlace(Vec3 *v, int s);
void Vec3_Add(Vec3 *out, Vec3 *a, Vec3 *b);
void AddVec3(Vec3 *a, Vec3 *b, Vec3 *c);
Vec3 *Vec3_AsrInPlace(Vec3 *v, int sh);

int func_0200c394(void *self_, int a1, void *a2_, Vec3 *a3, void **a5, int *a6)
{
    char *self = (char *)self_;
    unsigned char *a2 = (unsigned char *)a2_;
    void *p = *(void **)(self + 0x13c);
    Vec3 local, local2;
    signed char g;
    int flags = *(unsigned short *)((char *)p + 0x26);
    int v;
    short sz, sy, sx;

    if ((flags & 0x20) == 0) {
        v = *(int *)(self + 0x154) & 0x100;
        if (v == 0) goto body;
    }
    return 0;
body:
    {
            if (a1 == 7) {
                void *o = *(void **)(self + 0x114);
                if (o == 0) {
                    if (a2 == 0) goto other;
                    if (a2[0] != 3) goto other;
                }
                *a6 = 1;
                o = *(void **)(self + 0x114);
                if (o) {
                    int qb = (int)o + 0x5c;
                    *(int *)(self + 0x120) = *(int *)(qb);
                    *(int *)(self + 0x124) = *(int *)(qb + 4);
                    *(int *)(self + 0x128) = *(int *)(qb + 8);
                } else {
                    Vec3 t;
                    sx = *(short *)(a2 + 2);
                    sz = *(short *)(a2 + 6);
                    sy = *(short *)(a2 + 4);
                    t.x = sx << 12; t.z = sz << 12; t.y = sy << 12;
                    *(int *)(self + 0x120) = t.x;
                    *(int *)(self + 0x124) = t.y;
                    *(int *)(self + 0x128) = t.z;
                }
                Vec3_Sub(&local, (Vec3 *)(self + 0x120), a3);
                *(int *)(self + 0x12c) = LenVec3(&local) >> 1;
                g = data_0209f2f8;
                v = (g == 0x2d);
                if (v) {
                    if (*(int *)(self + 0x12c) < 0x100000) *(int *)(self + 0x12c) = 0x100000;
                    else if (*(int *)(self + 0x12c) > 0x180000) *(int *)(self + 0x12c) = 0x180000;
                } else {
                    if (*(int *)(self + 0x12c) < 0x180000) *(int *)(self + 0x12c) = 0x180000;
                }
                if (*(int *)(self + 0x154) & 2) {
                    *(int *)(self + 0x12c) = (int)(((long long)*(int *)(self + 0x12c) * 0x1800 + 0x800) >> 12);
                }
                if (g == 0x2f && *(int *)(self + 0x12c) > 0xc0000) {
                    int s = _ZN4cstd4fdivEii(0xc0000, *(int *)(self + 0x12c) << 1);
                    Vec3_MulScalarInPlace(&local, s);
                    Vec3_Add(&local2, a3, &local);
                    *(int *)(self + 0x120) = local2.x;
                    *(int *)(self + 0x124) = local2.y;
                    *(int *)(self + 0x128) = local2.z;
                    *(int *)(self + 0x12c) = 0xc0000;
                } else {
                    AddVec3((Vec3 *)(self + 0x120), a3, (Vec3 *)(self + 0x120));
                    Vec3_AsrInPlace((Vec3 *)(self + 0x120), 1);
                }
                *a5 = &data_02087274;
                return v;
            }

        other:
            if (flags & 4) {
                return (p == &data_0208706c) ? 0 : 1;
            }
            if (p == &data_0208715c) {
                if (v) { *a5 = &data_0208747c; }
                else if (a1 == 8) { *a5 = &data_0208715c; }
                else if (a1 == 6) { *a5 = &data_02087184; }
                else if (a1 == 0xa) { *a5 = &data_020871fc; }
                else if (a1 == 0xb) { *a5 = &data_02087224; }
                else if (a1 == 9) { *a5 = &data_0208733c; }
                else if (a1 == 2) { *a5 = &data_02087314; }
            }
            return 1;
    }
}
