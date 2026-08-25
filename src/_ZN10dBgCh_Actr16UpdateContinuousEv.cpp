//cpp
#include "types.h"
// @symbol _ZN10dBgCh_Actr16UpdateContinuousEv
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_dBgCh_SphCrr.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "dBgCh_Actr.h"
#define AT(p, off) ((void*)(int)((char*)(p) + (off)))

typedef struct Vec3 { int x, y, z; } Vec3;
typedef struct dBgPiLoc { char pad[0x28]; } dBgPiLoc;

/* extern "C" is required now this file is C++: these are already mangled ROM
   symbols, and without C linkage each would mangle a SECOND time and name
   something that exists nowhere. Relocations compare as wildcards, so the
   file would byte-match regardless -- only check_references sees it. */
extern "C" {
extern int func_02037938(void* p);
extern void func_02038324(int a, int* b, int c, int d);
extern void _ZN5dBgPiC1Ev(dBgPiLoc* r);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void* self, Vec3* a, Vec3* b, void* actor);
extern int _ZN9dBgCh_Lin10DetectClsnEv(void* self);
extern void _ZN9dBgCh_Lin10GetClsnPosEv(Vec3* out, void* self);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* self, Vec3* out);
extern void _ZNK5dBgPi6CopyToERS_(void* self, dBgPiLoc* dst);
extern void _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(void* self, Vec3* v, int rad, void* actor);
extern void _ZN12dBgCh_SphCrr14SetFloorResultERK5dBgPi(void* self, dBgPiLoc* r);
extern void _ZN5dBgPiaSERKS_(void* self, dBgPiLoc* r);
extern void func_02037888(void* dst, dBgPiLoc* src);
extern void func_020356d4(void* self);
extern void _ZN5dBgPiD1Ev(dBgPiLoc* r);
}

#pragma opt_common_subs off

void dBgCh_Actr::UpdateContinuous()
{
    int floorFlag;
    int wallFlag;
    int height;
    int onGround;
    int* pos;
    int* prev;
    int handled;
    dBgPiLoc res0;
    dBgPiLoc res1;
    Vec3 lineStart, lineEnd;
    Vec3 clsnPos, normal;
    Vec3 newStart, newEnd;
    Vec3 clsnPos2, normal2;
    Vec3 sphere;
    char* a;

    a = *(char**)((char*)&mActor);
    pos = (int*)(a + 0x5c);
    prev = (int*)(a + 0x68);

    if (IsOnGround() && func_020355a0(((char*)this)) && ShouldUpdatePos())
        func_02038324(func_02037938((char*)&mSphereClsn), pos, mSphereClsn.unk_10c, unk_130);

    floorFlag = 0;
    _ZN5dBgPiC1Ev(&res0);
    wallFlag = 0;
    _ZN5dBgPiC1Ev(&res1);

    height = unk_01c;
    {
        int tx = prev[0];
        int tz = prev[2];
        int ty = prev[1] + height;
        lineStart.x = tx;
        lineStart.y = ty;
        lineStart.z = tz;
    }
    {
        int tx = pos[0];
        int tz = pos[2];
        int ty = pos[1] + height;
        lineEnd.x = tx;
        lineEnd.y = ty;
        lineEnd.z = tz;
    }
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(((char*)this) + 0x134, &lineStart, &lineEnd, *(void**)((char*)&mActor));
    if (_ZN9dBgCh_Lin10DetectClsnEv((char*)&mRaycastLine))
    {
        int r;
        _ZN9dBgCh_Lin10GetClsnPosEv(&clsnPos, ((char*)this) + 0x134);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3(((char*)this) + 0x148, &normal);
        newStart.x = clsnPos.x + (normal.x >> 2);
        newStart.y = (normal.y >> 2) + clsnPos.y;
        newStart.z = clsnPos.z + (normal.z >> 2);
        newEnd.x = newStart.x;
        newEnd.y = newStart.y - height;
        newEnd.z = newStart.z;
        r = func_02039794(normal.y);
        if (r == 1) {
            wallFlag = 1;
            _ZNK5dBgPi6CopyToERS_(((char*)this) + 0x144, &res1);
        } else if (r == 0) {
            floorFlag = 1;
            _ZNK5dBgPi6CopyToERS_(((char*)this) + 0x144, &res0);
        }
        _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(((char*)this) + 0x134, &newStart, &newEnd, *(void**)((char*)&mActor));
        if (_ZN9dBgCh_Lin10DetectClsnEv((char*)&mRaycastLine)) {
            _ZN9dBgCh_Lin10GetClsnPosEv(&clsnPos2, ((char*)this) + 0x134);
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3(((char*)this) + 0x148, &normal2);
            if (func_02039794(normal2.y) == 0) {
                floorFlag = 1;
                _ZNK5dBgPi6CopyToERS_(((char*)this) + 0x144, &res0);
            }
            if (ShouldUpdatePos()) {
                pos[0] = clsnPos2.x - (normal2.x >> 2);
                pos[1] = clsnPos2.y - (normal2.y >> 2) - (height >> 1);
                pos[2] = clsnPos2.z - (normal2.z >> 2);
            }
        } else if (ShouldUpdatePos()) {
            pos[0] = newStart.x;
            pos[1] = newStart.y - height;
            pos[2] = newStart.z;
        }
    }

    onGround = IsOnGround();
    handled = 0;
    ClearAllGroundFlags();
    sphere.x = pos[0];
    sphere.y = pos[1];
    sphere.z = pos[2];
    sphere.y += height;
    _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(((char*)this) + 0x20, &sphere, unk_018, *(void**)((char*)&mActor));
    if (func_0203553c(((char*)this)) == 0)
        *(u8*)AT(((char*)this), 0x90) |= 0x40;
    mSphereClsn.unk_108 = unk_1b8;
    if (pos[1] - prev[1] > 0)
        *(u8*)AT(((char*)this), 0x90) |= 0x20;
    if (floorFlag != 0) {
        *(u8*)AT(((char*)this), 0x90) |= 4;
        _ZN12dBgCh_SphCrr14SetFloorResultERK5dBgPi(((char*)this) + 0x20, &res0);
        *(u8*)AT(((char*)this), 0x90) |= 1;
        _ZN5dBgPiaSERKS_(((char*)this) + 0x30, &res0);
        func_020371b0(((char*)this), onGround);
        handled = 1;
    }
    if (wallFlag != 0) {
        *(u8*)AT(((char*)this), 0x90) |= 8;
        func_02037888(((char*)this) + 0x20, &res1);
        *(u8*)AT(((char*)this), 0x90) |= 1;
        _ZN5dBgPiaSERKS_(((char*)this) + 0x30, &res1);
    }
    if (_ZN12dBgCh_SphCrr10DetectClsnEv((char*)&mSphereClsn)) {
        prev = (int*)((char*)&mSphereClsn.disp);
        if ((mSphereClsn.flags & 4) && handled == 0)
            func_020371b0(((char*)this), onGround);
        if (ShouldUpdatePos()) {
            pos[0] += prev[0];
            if (ShouldUpdatePosY())
                *(int*)AT(pos, 4) += prev[1];
            *(int*)AT(pos, 8) += prev[2];
        }
    }
    if (onGround && IsOnGround() == 0)
        func_020356d4(((char*)this));
    _ZN5dBgPiD1Ev(&res1);
    _ZN5dBgPiD1Ev(&res0);
}
