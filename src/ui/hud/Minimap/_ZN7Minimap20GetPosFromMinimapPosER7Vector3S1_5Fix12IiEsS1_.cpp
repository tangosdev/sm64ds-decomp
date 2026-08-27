//cpp
// @symbol _ZN7Minimap20GetPosFromMinimapPosER7Vector3S1_5Fix12IiEsS1_
/* recovered: named members + shared header */
#include "Minimap.h"
struct Vec3 { int x, y, z; };
extern "C" {
void Vec3_Sub(struct Vec3* out, struct Vec3* a, struct Vec3* b);
void Vec3_DivScalarInPlace(struct Vec3* v, int s);
void Vec3_RotateYAndTranslate(struct Vec3* out, struct Vec3* in, short ang, struct Vec3* t);
extern struct Vec3 data_ov002_0211116c;
}
extern "C" void _ZN7Minimap20GetPosFromMinimapPosER7Vector3S1_5Fix12IiEsS1_(
    struct Vec3& a, struct Vec3& b, int c, short d, struct Vec3& e)
{
    struct Vec3 local;
    Vec3_Sub(&local, &a, &data_ov002_0211116c);
    Vec3_DivScalarInPlace(&local, c);
    Vec3_RotateYAndTranslate(&e, &b, d, &local);
}
