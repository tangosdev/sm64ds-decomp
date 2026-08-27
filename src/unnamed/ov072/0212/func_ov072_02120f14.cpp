//cpp
// @symbol func_ov072_02120f14
/* recovered: shared common types */
#include "common.h"

struct dActor_c;
struct dBgCh_Gnd { char buf[0x54]; };

extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void *thiz);
extern "C" void _ZN9dBgCh_GndC1Ev(struct dBgCh_Gnd *thiz);
extern "C" void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(struct dBgCh_Gnd *thiz, const struct Vector3 *v, struct dActor_c *a);
extern "C" int _ZN9dBgCh_Gnd10DetectClsnEv(struct dBgCh_Gnd *thiz);
extern "C" void _ZN9dBgCh_GndD1Ev(struct dBgCh_Gnd *thiz);

extern "C" int func_ov072_02120f14(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    struct dBgCh_Gnd rg;
    struct Vector3 v;

    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x194)) {
        _ZN9dBgCh_GndC1Ev(&rg);
        {
            int z = *(int *)(c + 0x64);
            int y = *(int *)(c + 0x60) + 0x1e000;
            int x = *(int *)(c + 0x5c);
            v.x = x;
            v.y = y;
            v.z = z;
        }
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &v, (struct dActor_c *)c);
        if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg) != 0) {
            int d = *(int *)((char *)&rg + 0x44) - *(int *)(c + 0x60);
            if (d < 0) d = -d;
            if (d <= 0x1900) goto bail;
        }
        *(int *)(c + 0x5c) = *(int *)(c + 0x68);
        *(int *)(c + 0x60) = *(int *)(c + 0x6c);
        *(int *)(c + 0x64) = *(int *)(c + 0x70);
        _ZN9dBgCh_GndD1Ev(&rg);
        return 1;
    bail:
        _ZN9dBgCh_GndD1Ev(&rg);
    }
    return 0;
}
