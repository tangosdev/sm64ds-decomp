//cpp
struct Vector3 { int x,y,z; };
typedef int Fix12;
extern "C" {
extern void Vec3_Sub(struct Vector3* dst, struct Vector3* a, struct Vector3* b);
extern void Vec3_MulScalarInPlace(struct Vector3* v, Fix12 s);
extern void Vec3_RotateYAndTranslate(struct Vector3* res, const struct Vector3* trans, short angY, const struct Vector3* v);
extern struct Vector3 data_ov002_0211116c;
void _ZN7Minimap15GetPosOnMinimapER7Vector3S1_5Fix12IiEsS1_(struct Vector3* out, struct Vector3* a, Fix12 s, short ang, struct Vector3* res)
{
    struct Vector3 v;
    Vec3_Sub(&v, out, a);
    Vec3_MulScalarInPlace(&v, s);
    Vec3_RotateYAndTranslate(res, &data_ov002_0211116c, (short)(-ang), &v);
}
}
