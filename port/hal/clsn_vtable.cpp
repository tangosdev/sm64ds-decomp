// The synthetic dBgW_Kc vtable (gate 8) -- the gate-3a mechanism at
// its second use, and the first with REAL slot fillers throughout the hot
// path: GetSurfaceInfo (matched, ITCM) calls GetNormal through the vtable
// (notes/itcm.md, "the one lever"), so slot 4 must dispatch for the octree
// walk to survive. Slots are __fastcall shims (ecx carries `this` exactly
// as __thiscall does; the dummy edx absorbs fastcall's second register),
// slot order per include/dBgW_Kc.h's ROM-read map: dtor 0/1,
// Virtual08 2, surface queries 3-5, the DetectClsn overloads 6-8.
// Unevidenced slots trap loudly.
#include <stdio.h>
#include <stdlib.h>

#include "dBgW_Kc.h"
#include "dBgCh.h"
#include "SurfaceInfo.h"

namespace cstd {
int sqrt(unsigned long long);
int fdiv_result();
}
extern "C" {
int _ZN4cstd4sqrtEy(unsigned long long value) { return cstd::sqrt(value); }
int _ZN4cstd11fdiv_resultEv() { return cstd::fdiv_result(); }
bool _ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(
    void *p, const CLPS &clps, const dBgCh &bg, bool flag)
{ return dBgCh::ShouldPassThroughImpl(p, clps, bg, flag); }
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(
    const SurfaceInfo *self, Vector3 *out)
{ self->SurfaceInfo::CopyNormalTo(*out); }
}

static void __fastcall slot_v08(void *self, void *)
{ ((dBgW_Kc *)self)->dBgW_Kc::Virtual08(); }
static void __fastcall slot_surf(void *self, void *, s16 tri, SurfaceInfo *res)
{ ((dBgW_Kc *)self)->dBgW_Kc::GetSurfaceInfo(tri, *res); }
static void __fastcall slot_norm(void *self, void *, s16 tri, Vector3 *res)
{ ((dBgW_Kc *)self)->dBgW_Kc::GetNormal(tri, *res); }
static void __fastcall slot_orig(void *self, void *, s16 tri, Vector3 *res)
{ ((dBgW_Kc *)self)->dBgW_Kc::GetTriangleOrigin(tri, *res); }
static int __fastcall slot_ray(void *self, void *, dBgCh_Lin *ray)
{ return ((dBgW_Kc *)self)->dBgW_Kc::DetectClsn(*ray); }

#define TRAP(n) \
    static void __fastcall slot_trap##n(void *, void *) { \
        fprintf(stderr, "FATAL: dBgW_Kc vtable slot %d dispatched " \
                        "with no filler (clsn_vtable.cpp)\n", n); \
        abort(); }
TRAP(0) TRAP(1) TRAP(6) TRAP(8) TRAP(9) TRAP(10) TRAP(11) TRAP(12)

// SLOT ORDER IS MSVC'S, NOT THE ROM'S. The dispatching code here is
// MSVC-compiled against include/dBgW_Kc.h, and MSVC lays the table
// with a ONE-slot destructor (the ROM's Itanium layout spends two). Filling
// the array in ROM order put GetSurfaceInfo where MSVC reads GetNormal, and
// its internal virtual call recursed into itself until the stack died --
// the exact D1/D0-vs-scalar-dtor skew the earlier gates dodged by never
// dispatching. One more MSVC quirk pinned here: adjacent overloads
// (the DetectClsn trio) are emitted in REVERSE declaration order.
extern "C" void *_ZTV7dBgW_Kc[13] = {
    (void *)slot_trap0,         /* 0: scalar deleting dtor */
    (void *)slot_v08,           /* 1: Virtual08 */
    (void *)slot_surf,          /* 2: GetSurfaceInfo */
    (void *)slot_norm,          /* 3: GetNormal -- the walk's hot slot */
    (void *)slot_orig,          /* 4: GetTriangleOrigin */
    (void *)slot_trap1,         /* 5: DetectClsn(dBgCh_SphCrr) -- unmatched */
    (void *)slot_ray,           /* 6: DetectClsn(dBgCh_Lin) */
    (void *)slot_trap6,         /* 7: DetectClsn(dBgCh_Gnd) -- unmatched */
    (void *)slot_trap8,
    (void *)slot_trap9, (void *)slot_trap10,
    (void *)slot_trap11, (void *)slot_trap12,
};

// dBgW_KcMbg inherits the surface queries; its table gets the same
// shims at runtime (its own DetectClsn overloads stay trapped until a
// consumer needs them).
extern "C" {
void *_ZTV10dBgW_KcMbg[16];
void hal_fill_mmc_vtable(void)
{
    for (int i = 0; i < 13; ++i)
        _ZTV10dBgW_KcMbg[i] = _ZTV7dBgW_Kc[i];
}
}
