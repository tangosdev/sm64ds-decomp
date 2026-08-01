//cpp
// @symbol func_ov072_02120f14
/* recovered: shared common types */
#include "common.h"

struct Actor;
struct RaycastGround { char buf[0x54]; };

extern "C" int _ZNK12WithMeshClsn10IsOnGroundEv(void *thiz);
extern "C" void _ZN13RaycastGroundC1Ev(struct RaycastGround *thiz);
extern "C" void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(struct RaycastGround *thiz, const struct Vector3 *v, struct Actor *a);
extern "C" int _ZN13RaycastGround10DetectClsnEv(struct RaycastGround *thiz);
extern "C" void _ZN13RaycastGroundD1Ev(struct RaycastGround *thiz);

extern "C" int func_ov072_02120f14(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    struct RaycastGround rg;
    struct Vector3 v;

    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x194)) {
        _ZN13RaycastGroundC1Ev(&rg);
        {
            int z = *(int *)(c + 0x64);
            int y = *(int *)(c + 0x60) + 0x1e000;
            int x = *(int *)(c + 0x5c);
            v.x = x;
            v.y = y;
            v.z = z;
        }
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &v, (struct Actor *)c);
        if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0) {
            int d = *(int *)((char *)&rg + 0x44) - *(int *)(c + 0x60);
            if (d < 0) d = -d;
            if (d <= 0x1900) goto bail;
        }
        *(int *)(c + 0x5c) = *(int *)(c + 0x68);
        *(int *)(c + 0x60) = *(int *)(c + 0x6c);
        *(int *)(c + 0x64) = *(int *)(c + 0x70);
        _ZN13RaycastGroundD1Ev(&rg);
        return 1;
    bail:
        _ZN13RaycastGroundD1Ev(&rg);
    }
    return 0;
}
