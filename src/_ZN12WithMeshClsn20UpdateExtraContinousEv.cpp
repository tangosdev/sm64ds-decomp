//cpp
// @symbol _ZN12WithMeshClsn20UpdateExtraContinousEv
#include "WithMeshClsn.h"
typedef int s32;
typedef unsigned int u32;
typedef short s16;
typedef unsigned char u8;

struct SurfaceInfo { s32 w0, w1, w2, w3, w4; };
struct RaycastLine { Vector3 GetClsnPos(); };
struct ClsnResult
{
    u32 *vt;
    SurfaceInfo si;
    unsigned short tri;
    unsigned short clsn;
    u32 objID;
    void *obj;
    void *mesh;
};

extern "C" {

extern u32 data_02099368[];
#pragma opt_common_subs off

#define V3D(v, p, dy) { s32 _x, _y, _z; _z = (p)->z; _y = (p)->y; _x = (p)->x; _y = _y + (dy); (v).x = _x; (v).y = _y; (v).z = _z; }
#define FLAGP (*(u8 *)(((long long)(int)(t + 0x90))))
#define FLAGQ (*(u8 *)(((long long)(int)(t + 0x90)) | 0LL))
#define LNDR(T, a) ((T *)(((long long)(int)(a))))
#define LND32(a)   ((s32)(((long long)(int)(a))))
#define V3C(v, p) { s32 _x, _y, _z; _z = (p)->z; _y = (p)->y; _x = (p)->x; (v).x = _x; (v).y = _y; (v).z = _z; }

int  func_020355a0(void *);
void *func_02037938(void *);
void func_02038324(void *, Vector3 *, void *, void *);
void _ZN10ClsnResultC1Ev(ClsnResult *);
void _ZN10ClsnResultD1Ev(ClsnResult *);
void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void *, const Vector3 *, const Vector3 *, void *);
int  func_0203859c(void *);
void _ZN11RaycastLine10GetClsnPosEv(Vector3 *, void *);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(SurfaceInfo *, Vector3 *);
int  _ZN11RaycastLine10DetectClsnEv(void *);
int  func_02039794(s32);
void _ZNK10ClsnResult6CopyToERS_(const ClsnResult *, ClsnResult *);
Vector3 *func_02037dc4(SurfaceInfo *);
void _ZN10SphereClsn15SetObjAndSphereERK7Vector35Fix12IiEP5Actor(void *, const Vector3 *, s32, void *);
int  func_02035764(void *);
void _ZN10SphereClsn14SetFloorResultERK10ClsnResult(void *, const ClsnResult *);
void _ZN10ClsnResultaSERKS_(ClsnResult *, const ClsnResult *);
void func_020371b0(void *, s32);
void func_02037888(void *, const ClsnResult *);
void func_0203782c(void *, const ClsnResult *);
int  _ZN10SphereClsn10DetectClsnEv(void *);
int  func_020355dc(void *);
ClsnResult *func_020378dc(void *);
void func_020356d4(void *);

}  /* end extern "C" -- the definition below is a member, not a C symbol */

void WithMeshClsn::UpdateExtraContinous()
{
    char *t = (char *)this;
    s32 f0, f1, f2, vo, wasOnGround, didHit;
    Vector3 *pos, *prev;
    char *ac;

    ac = *(char **)(t + 0x14);
    pos = (Vector3 *)(ac + 0x5c);
    prev = (Vector3 *)(ac + 0x68);

    if (IsOnGround() && func_020355a0(t))
        func_02038324(func_02037938(t + 0x20), pos,
                      *(void **)(t + 0x12c), *(void **)(t + 0x130));

    {
        ClsnResult res0;
        f0 = 0;
        _ZN10ClsnResultC1Ev(&res0);
        ClsnResult res1;
        f1 = 0;
        _ZN10ClsnResultC1Ev(&res1);
        ClsnResult res2;
        f2 = 0;
        _ZN10ClsnResultC1Ev(&res2);

        vo = *(s32 *)(t + 0x1c);

        Vector3 v7c;


        V3D(v7c, prev, vo)
        Vector3 v88;

        V3C(v88, prev)
        _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(t + 0x134, &v88, &v7c, *(void **)(t + 0x14));

        if (func_0203859c(t + 0x134))
        {
            v7c = ((RaycastLine *)(t + 0x134))->GetClsnPos();
            Vector3 v94;
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3((SurfaceInfo *)(t + 0x148), &v94);
            v7c.x = v7c.x + (v94.x >> 2);
            v7c.y = v7c.y + (v94.y >> 2);
            v7c.z = v7c.z + (v94.z >> 2);
        }

        Vector3 va0;


        V3D(va0, pos, vo)
        _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(t + 0x134, &v7c, &va0, *(void **)(t + 0x14));

        if (_ZN11RaycastLine10DetectClsnEv(t + 0x134))
        {
            Vector3 vac;
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3((SurfaceInfo *)(t + 0x148), &vac);
            s32 kind; kind = func_02039794(vac.y);
            if (kind == 1)
            {
                f1 = 1;
                _ZNK10ClsnResult6CopyToERS_((ClsnResult *)(t + 0x144), &res1);
            }
            else if (kind == 2)
            {
                f2 = 1;
                _ZNK10ClsnResult6CopyToERS_((ClsnResult *)(t + 0x144), &res2);
            }
            else if (kind == 0)
            {
                f0 = 1;
                _ZNK10ClsnResult6CopyToERS_((ClsnResult *)(t + 0x144), &res0);
            }

            Vector3 vb8;
                _ZN11RaycastLine10GetClsnPosEv(&vb8, t + 0x134);
            Vector3 vc4;
            vc4.x = vb8.x + (vac.x >> 2);
            vc4.y = vb8.y + (vac.y >> 2);
            vc4.z = vb8.z + (vac.z >> 2);
            Vector3 vd0;
            vd0.x = vc4.x;
            vd0.y = vc4.y - vo;
            vd0.z = vc4.z;
            _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(t + 0x134, &vc4, &vd0, *(void **)(t + 0x14));

            if (_ZN11RaycastLine10DetectClsnEv(t + 0x134))
            {
                Vector3 vdc;
                _ZN11RaycastLine10GetClsnPosEv(&vdc, t + 0x134);
                Vector3 ve8;
                _ZNK11SurfaceInfo12CopyNormalToER7Vector3((SurfaceInfo *)(t + 0x148), &ve8);
                if (func_02039794(ve8.y) == 0)
                {
                    f0 = 1;
                    _ZNK10ClsnResult6CopyToERS_((ClsnResult *)(t + 0x144), &res0);
                }
                vdc.x = vdc.x + (ve8.x << 2);
                vdc.y = vdc.y + (ve8.y << 2);
                vdc.z = vdc.z + (ve8.z << 2);
                Vector3 vf4;
                vf4.x = vdc.x;
                vf4.y = vdc.y + (vo << 1);
                vf4.z = vdc.z;
                _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(t + 0x134, &vdc, &vf4, *(void **)(t + 0x14));

                if (_ZN11RaycastLine10DetectClsnEv(t + 0x134))
                {
                    Vector3 v100;
                _ZN11RaycastLine10GetClsnPosEv(&v100, t + 0x134);
                    pos->x = v100.x;
                    pos->y = ((v100.y + vdc.y) >> 1) - vo;
                    pos->z = v100.z;
                }
                else
                {
                    pos->x = vdc.x - (ve8.x >> 2);
                    pos->y = vdc.y - (ve8.y >> 2) - (vo >> 1);
                    pos->z = vdc.z - (ve8.z >> 2);
                }
            }
            else
            {
                pos->x = vb8.x + (vac.x >> 2);
                pos->y = vb8.y + (vac.y >> 2) - vo;
                pos->z = vb8.z + (vac.z >> 2);
            }

            Vector3 v10c;


            V3D(v10c, prev, vo)
            Vector3 v118;

            V3D(v118, pos, vo)
            _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(t + 0x134, &v10c, &v118, *(void **)(t + 0x14));

            if (_ZN11RaycastLine10DetectClsnEv(t + 0x134) &&
                func_02037dc4((SurfaceInfo *)(t + 0x148))->y >= 0)
            {
                Vector3 *cp = LNDR(Vector3, t + 0x188);
                pos->x = cp->x;
                pos->y = cp->y;
                pos->z = cp->z;
            }
        }

        wasOnGround = IsOnGround();
        ClearAllGroundFlags();
        didHit = 0;

        Vector3 v124;
        v124.x = pos->x;
        v124.y = pos->y;
        v124.z = pos->z;
        v124.y = v124.y + vo;
        _ZN10SphereClsn15SetObjAndSphereERK7Vector35Fix12IiEP5Actor(t + 0x20, &v124, *(s32 *)(t + 0x18), *(void **)(t + 0x14));

        *(s32 *)(t + 0x128) = *(s32 *)(t + 0x1b8);
        if (func_02035764(t))
            FLAGP |= 2;
        if (pos->y - prev->y > 0)
            FLAGP |= 0x20;

        if (f0)
        {
            FLAGP |= 4;
            _ZN10SphereClsn14SetFloorResultERK10ClsnResult(t + 0x20, &res0);
            FLAGQ |= 1;
            _ZN10ClsnResultaSERKS_((ClsnResult *)(t + 0x30), &res0);
            func_020371b0(t, wasOnGround);
            didHit = 1;
        }
        if (f1)
        {
            FLAGP |= 8;
            func_02037888(t + 0x20, &res1);
            FLAGQ |= 1;
            _ZN10ClsnResultaSERKS_((ClsnResult *)(t + 0x30), &res1);
        }
        if (f2)
        {
            FLAGP |= 0x10;
            func_0203782c(t + 0x20, &res2);
            FLAGQ |= 1;
            _ZN10ClsnResultaSERKS_((ClsnResult *)(t + 0x30), &res2);
        }

        if (_ZN10SphereClsn10DetectClsnEv(t + 0x20))
        {
            Vector3 *pb = (Vector3 *)(t + 0x6c);
            if ((*(u8 *)(t + 0x90) & 4) && didHit == 0)
            {
                func_020371b0(t, wasOnGround);
                didHit = 1;
            }
            pos->x = pos->x + pb->x;
            if (ShouldUpdatePosY())
                *LNDR(s32, (char *)pos + 4) = *LNDR(s32, (char *)pos + 4) + pb->y;
            *LNDR(s32, (char *)pos + 8) = *LNDR(s32, (char *)pos + 8) + pb->z;

            if (func_020355dc(t))
            {
                Vector3 v130;

                V3D(v130, pos, (vo << 1))
                Vector3 v13c;

                V3C(v13c, pos)
                _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(t + 0x134, &v130, &v13c, *(void **)(t + 0x14));
                if (_ZN11RaycastLine10DetectClsnEv(t + 0x134))
                {
                    Vector3 *cp = LNDR(Vector3, t + 0x188);
                    Vector3 *n = func_02037dc4((SurfaceInfo *)(t + 0x148));
                    pos->x = cp->x + (n->x >> 2);
                    pos->y = cp->y + (n->y >> 2);
                    pos->z = cp->z + (n->z >> 2);
                }
            }

            s32 fl; fl = *(u8 *)(t + 0x90);
            if (!(fl & 4) && (fl & 8))
            {
                ClsnResult tmp;
                ClsnResult *src = func_020378dc(t + 0x20);
                SurfaceInfo *dsi = &tmp.si;
                {
                    s32 c0, c1;
                    c0 = *(volatile s32 *)&src->si.w0;
                    c1 = *(volatile s32 *)&src->si.w1;
                    dsi->w0 = c0;
                    dsi->w1 = c1;
                    dsi->w2 = src->si.w2;
                    dsi->w3 = src->si.w3;
                    dsi->w4 = src->si.w4;
                }
                tmp.vt = data_02099368;
                tmp.tri = src->tri;
                tmp.clsn = src->clsn;
                tmp.objID = src->objID;
                tmp.obj = src->obj;
                tmp.mesh = src->mesh;

                if (func_02037dc4(dsi)->y >= 0)
                {
                    Vector3 v170;

                    V3D(v170, pos, (vo << 1))
                    Vector3 v17c;

                    V3C(v17c, pos)
                    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(t + 0x134, &v170, &v17c, *(void **)(t + 0x14));
                    if (_ZN11RaycastLine10DetectClsnEv(t + 0x134))
                    {
                        Vector3 *cp = LNDR(Vector3, t + 0x188);
                        Vector3 *n = func_02037dc4((SurfaceInfo *)(t + 0x148));
                        pos->x = cp->x + (n->x >> 2);
                        pos->y = cp->y + (n->y >> 2);
                        pos->z = cp->z + (n->z >> 2);
                        s32 k3; k3 = func_02039794(n->y);
                        if (k3 == 1 && !(*(u8 *)(t + 0x90) & 8))
                        {
                            ClsnResult *lr = (ClsnResult *)(t + 0x144);
                            FLAGP |= 8;
                            func_02037888(t + 0x20, lr);
                            FLAGQ |= 1;
                            _ZN10ClsnResultaSERKS_((ClsnResult *)(t + 0x30), lr);
                        }
                        else if (k3 == 0 && !(*(u8 *)(t + 0x90) & 4))
                        {
                            ClsnResult *lr = (ClsnResult *)(t + 0x144);
                            FLAGP |= 4;
                            _ZN10SphereClsn14SetFloorResultERK10ClsnResult(t + 0x20, lr);
                            FLAGQ |= 1;
                            _ZN10ClsnResultaSERKS_((ClsnResult *)(t + 0x30), lr);
                            if (didHit == 0)
                                func_020371b0(t, wasOnGround);
                        }
                        else if (k3 == 2 && !(*(u8 *)(t + 0x90) & 0x10))
                        {
                            ClsnResult *lr = (ClsnResult *)(t + 0x144);
                            FLAGP |= 0x10;
                            func_0203782c(t + 0x20, lr);
                            FLAGQ |= 1;
                            _ZN10ClsnResultaSERKS_((ClsnResult *)(t + 0x30), lr);
                        }
                    }
                }
                else
                {
                    s32 vo2 = LND32(vo << 1);
                    Vector3 v188;

                    V3C(v188, pos)
                    Vector3 v194;

                    V3D(v194, pos, (vo << 1))
                    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(t + 0x134, &v188, &v194, *(void **)(t + 0x14));
                    if (_ZN11RaycastLine10DetectClsnEv(t + 0x134))
                    {
                        Vector3 *cp = (Vector3 *)(t + 0x188);
                        Vector3 *n = func_02037dc4((SurfaceInfo *)(t + 0x148));
                        pos->x = cp->x + (n->x >> 2);
                        pos->y = cp->y + (n->y >> 2) - vo2;
                        pos->z = cp->z + (n->z >> 2);
                    }
                }
                _ZN10ClsnResultD1Ev(&tmp);
            }
            else if (*(s32 *)(t + 0x7c) <= -vo)
            {
                Vector3 v1a0;

                V3D(v1a0, prev, vo)
                Vector3 v1ac;

                V3D(v1ac, pos, vo)
                _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(t + 0x134, &v1a0, &v1ac, *(void **)(t + 0x14));
                if (func_0203859c(t + 0x134))
                {
                    Vector3 *cp = LNDR(Vector3, t + 0x188);
                    Vector3 *n = func_02037dc4((SurfaceInfo *)(t + 0x148));
                    pos->x = cp->x + (n->x >> 2);
                    pos->y = cp->y + (n->y >> 2);
                    pos->z = cp->z + (n->z >> 2);
                }
            }
        }

        if (wasOnGround && !IsOnGround())
            func_020356d4(t);

        _ZN10ClsnResultD1Ev(&res2);
        _ZN10ClsnResultD1Ev(&res1);
        _ZN10ClsnResultD1Ev(&res0);
    }
}
