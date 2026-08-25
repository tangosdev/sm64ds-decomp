// Constructor bridges and vtable fillers for the gate-9 classes: dBgW_KcMbg
// (the moving-background collider) and ShadowModel.
//
// Gate 9 belongs to smoke_actor alone, so these cannot live in the gate-8
// bridge -- smoke_clsn links that one and never builds these classes.
//
// The traps here are the same bargain hal/clsn_vtable.cpp struck and
// hal/bg_ctor_bridge.cpp repeats: a migrated constructor makes MSVC emit the
// class vtable, which needs every virtual defined, and the real bodies drag in
// the collision tree (dBgCh_SphCrr, dBgPi, DotVec3, cstd::fdiv and a dozen
// func_0203xxxx). Defining them loudly keeps the gate honest -- the day one
// dispatches, it says so by name instead of returning a plausible zero.
#include "common.h"

#include <new>
#include <stdio.h>
#include <stdlib.h>

#include "ShadowModel.h"
#include "dBgW_KcMbg.h"

static void actor_trap(const char *who)
{
    fprintf(stderr, "FATAL: %s dispatched, but the port defines it only so "
                    "MSVC can emit the vtable (hal/actor_ctor_bridge.cpp).\n", who);
    abort();
}

#define ACTOR_TRAP(sig, who) sig { actor_trap(who); }

ACTOR_TRAP(void dBgW_KcMbg::Virtual08(), "dBgW_KcMbg::Virtual08")
ACTOR_TRAP(void dBgW_KcMbg::GetNormal(s16, Vector3 &), "dBgW_KcMbg::GetNormal")
ACTOR_TRAP(void dBgW_KcMbg::GetTriangleOrigin(s16, Vector3 &), "dBgW_KcMbg::GetTriangleOrigin")
ACTOR_TRAP(void dBgW_KcMbg::GetVelocity(Vector3 &), "dBgW_KcMbg::GetVelocity")
ACTOR_TRAP(s16 dBgW_KcMbg::GetAngularVelY(), "dBgW_KcMbg::GetAngularVelY")
ACTOR_TRAP(int dBgW_KcMbg::TransformPos(const Vector3 &, Vector3 &), "dBgW_KcMbg::TransformPos")
ACTOR_TRAP(int dBgW_KcMbg::DetectClsn(dBgCh_Gnd &), "dBgW_KcMbg::DetectClsn(dBgCh_Gnd&)")
ACTOR_TRAP(int dBgW_KcMbg::DetectClsn(dBgCh_Lin &), "dBgW_KcMbg::DetectClsn(dBgCh_Lin&)")
ACTOR_TRAP(int dBgW_KcMbg::DetectClsn(dBgCh_SphCrr &), "dBgW_KcMbg::DetectClsn(dBgCh_SphCrr&)")
ACTOR_TRAP(dBgW_KcMbg::~dBgW_KcMbg(), "dBgW_KcMbg::~dBgW_KcMbg")

ACTOR_TRAP(int ShadowModel::DoSetFile(char *, int, int), "ShadowModel::DoSetFile")
ACTOR_TRAP(ShadowModel::~ShadowModel(), "ShadowModel::~ShadowModel")

extern "C" {
void _ZN10dBgW_KcMbgC1Ev(void *self) { ::new (self) dBgW_KcMbg(); }
void _ZN11ShadowModelC1Ev(void *self) { ::new (self) ShadowModel(); }
}
