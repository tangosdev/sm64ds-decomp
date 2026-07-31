//cpp
// @symbol func_ov071_02120a48
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"

extern "C" char *_ZN5Actor22ClosestNonVanishPlayerEv(void *self);
extern "C" int Vec3_Dist(const void *a, const void *b);
extern "C" short Vec3_HorzAngle(const void *a, const void *b);
extern "C" int _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(void *self, Vector3 *a, Vector3 *b, bool c);
extern "C" void func_ov071_02120a48(char *c)
{
    char *p = _ZN5Actor22ClosestNonVanishPlayerEv(c);
    if (p == 0)
        return;
    if (Vec3_Dist(c + 0x5c, p + 0x5c) > 0x5dc000)
        return;
    if (AngleDiff(Vec3_HorzAngle(c + 0x5c, p + 0x5c), *(short *)(c + 0x8e)) > 0x190)
        return;
    int px = *(int *)(p + 0x5c);
    int pz = *(int *)(p + 0x64);
    int py = *(int *)(p + 0x60) + 0x8c000;
    Vector3 v;
    v.x = px;
    v.y = py;
    v.z = pz;
    if (_ZN5Actor17DetectRaycastClsnER7Vector3S1_b(c, &v, (Vector3 *)(c + 0x5c), false) != 0)
        return;
    *(char **)(c + 0x1ec) = p;
    *(unsigned char *)(c + 0x216) = 0x2e;
    func_ov071_02121634(c, 1);
}
