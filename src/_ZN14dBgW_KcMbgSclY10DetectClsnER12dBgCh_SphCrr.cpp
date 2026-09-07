//cpp
/* Slot 8: test a world-space sphere against a vertically scaled collider.
 * The local query is the real dBgCh_SphCrr, so its C1/D1 calls come from
 * ordinary automatic storage. SetObjAndSphere retains its measured raw-Fix12
 * call veneer: spelling that disputed parameter as Fix12<int> grows this
 * caller by 0xc bytes under the pinned compiler. */
#include "dBgW_KcMbgSclY.h"
#include "dBgCh_SphCrr.h"
#include "dBgPi.h"

extern "C" {
void func_0203abb0(dM3dGSph* sphere, Vector3* centre);
void func_0203aa74(dBgW_KcMbgSclY* self, Vector3* v, Vector3* res);
void _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(
    dBgCh_SphCrr *sphere, const Vector3 *pos, Fix12i radius, dActor_c *actor);
void func_02037940(dBgCh_SphCrr *sphere, u8 flags);
void func_02035394(dBgCh_SphCrr *dst, dBgCh_SphCrr *src);
void func_02037a04(dBgCh_SphCrr *sphere, int *first, int *second);
void func_02037a6c(dBgCh_SphCrr *sphere,
    int x1, int y1, int z1, int x2, int y2, int z2);
dBgPi *func_02037938(dBgCh_SphCrr *sphere);
void func_0203794c(dBgCh_SphCrr *sphere, s32 *result);
dBgPi *func_020378dc(dBgCh_SphCrr *sphere);
void func_02037888(dBgCh_SphCrr *sphere, dBgPi *result);
dBgPi *func_02037880(dBgCh_SphCrr *sphere);
void func_0203782c(dBgCh_SphCrr *sphere, dBgPi *result);
}

#pragma opt_common_subs off

#define FMUL(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))

int dBgW_KcMbgSclY::DetectClsn(dBgCh_SphCrr &sphere)
{
    Vector3 centre;
    Vector3 localCentre;
    int d[12];
    int inverseScale;
    int radius1;
    int radius2;
    int r;

    func_0203abb0(&(dM3dGSph &)sphere, &centre);
    func_0203aa74(this, &centre, &localCentre);

    inverseScale = invScale;
    radius1 = FMUL(sphere.radius, inverseScale);
    radius2 = FMUL(sphere.unk_0ec, inverseScale);

    dBgCh_SphCrr loc;
    _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(&loc, &localCentre, radius1, 0);
    loc.unk_0ec = radius2;
    func_02037940(&loc, sphere.flags);
    func_02035394(&loc, &sphere);
    r = dBgW_Kc::DetectClsn(loc);
    if (r) {
        func_02037a04(&loc, d, d + 3);
        d[6] = FMUL(d[0], scale);
        d[7] = FMUL(d[1], scale);
        d[8] = FMUL(d[2], scale);
        d[9] = FMUL(d[3], scale);
        d[10] = FMUL(d[4], scale);
        d[11] = FMUL(d[5], scale);
        func_02037a6c(&sphere, d[6], d[7], d[8], d[9], d[10], d[11]);
        (dBgPi &)sphere = (dBgPi &)loc;
        sphere.flags |= 1;
        if (loc.flags & 4) {
            if (sphere.flags & 4) {
                r &= ~1;
            } else {
                sphere.SetFloorResult(*(dBgPi*)func_02037938(&loc));
            }
            sphere.flags |= 4;
            if (sphere.unk_100 < loc.unk_100) {
                func_0203794c(&sphere, &loc.unk_0fc);
            }
        }
        if (loc.flags & 8) {
            func_02037888(&sphere, func_020378dc(&loc));
            sphere.flags |= 8;
        }
        if (loc.flags & 0x10) {
            func_0203782c(&sphere, func_02037880(&loc));
            sphere.flags |= 0x10;
        }
    }
    return r;
}
