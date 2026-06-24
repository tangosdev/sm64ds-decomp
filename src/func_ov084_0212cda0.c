struct Vec3 { int x, y, z; };
extern void Vec3_Sub(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);
extern int LenVec3(struct Vec3 *v);
extern int Math_Function_0203b14c(int *a, int b, int c, int d, int e);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalarInPlace(int *v, int s);
extern void Vec3_Add(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);

int func_ov084_0212cda0(int arg0, struct Vec3 *out, struct Vec3 *target)
{
    struct Vec3 d;
    int len;
    int r5;
    int scratch;
    Vec3_Sub(&d, out, target);
    len = LenVec3(&d);
    if (len == 0) return 1;
    scratch = len;
    r5 = Math_Function_0203b14c(&scratch, 0, 0x400, 0x100000, 0x1000);
    Vec3_MulScalarInPlace((int*)&d, _ZN4cstd4fdivEii(scratch, len));
    {
        struct Vec3 res;
        Vec3_Add(&res, target, &d);
        out->x = res.x;
        out->y = res.y;
        out->z = res.z;
    }
    return r5 == 0 ? 1 : 0;
}
