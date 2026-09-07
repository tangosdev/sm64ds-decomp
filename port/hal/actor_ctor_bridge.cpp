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
#include "dBgActor_c.h"

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

// ---------------------------------------------------------------------------
// dBgActor_c's constructor is real C++ on this branch, which makes this the TU
// MSVC emits the dActor_c / fBase_c vtable into -- and that vtable wants every
// one of the 30 virtuals dActor_c declares. Slicing their real bodies pulls in
// the whole actor runtime, so they take the shape the rest of this file uses:
// present for the vtable, loud if anything dispatches. ArrowSignRight, the
// class gate 9 actually exercises, overrides what it uses, so a trap firing
// here is news -- it means the slice is missing a body, not that the port is
// wrong.

// dActor_c's own constructor is sliced, as the flat extern "C" spelling.
extern "C" void _ZN8dActor_cC2Ev(void *self);
dActor_c::dActor_c() { _ZN8dActor_cC2Ev(this); }

ACTOR_TRAP(dActor_c::~dActor_c(), "dActor_c::~dActor_c")

ACTOR_TRAP(int fBase_c::InitResources(), "fBase_c::InitResources")
ACTOR_TRAP(int fBase_c::CleanupResources(), "fBase_c::CleanupResources")
ACTOR_TRAP(int fBase_c::Behavior(), "fBase_c::Behavior")
ACTOR_TRAP(int fBase_c::Render(), "fBase_c::Render")
ACTOR_TRAP(int fBase_c::Virtual34(u32, u32), "fBase_c::Virtual34")
ACTOR_TRAP(int fBase_c::Virtual38(u32, u32), "fBase_c::Virtual38")

ACTOR_TRAP(bool dActor_c::BeforeInitResources(), "dActor_c::BeforeInitResources")
ACTOR_TRAP(void dActor_c::AfterInitResources(u32), "dActor_c::AfterInitResources")
ACTOR_TRAP(int dActor_c::BeforeCleanupResources(), "dActor_c::BeforeCleanupResources")
ACTOR_TRAP(void dActor_c::AfterCleanupResources(u32), "dActor_c::AfterCleanupResources")
ACTOR_TRAP(int dActor_c::BeforeBehavior(), "dActor_c::BeforeBehavior")
ACTOR_TRAP(void dActor_c::AfterBehavior(u32), "dActor_c::AfterBehavior")
ACTOR_TRAP(int dActor_c::BeforeRender(), "dActor_c::BeforeRender")
ACTOR_TRAP(void dActor_c::AfterRender(u32), "dActor_c::AfterRender")
ACTOR_TRAP(int dActor_c::OnYoshiTryEat(), "dActor_c::OnYoshiTryEat")
ACTOR_TRAP(int dActor_c::OnTurnIntoEgg(Player &), "dActor_c::OnTurnIntoEgg")
ACTOR_TRAP(int dActor_c::Virtual50(), "dActor_c::Virtual50")
ACTOR_TRAP(void dActor_c::OnGroundPounded(dActor_c &), "dActor_c::OnGroundPounded")
ACTOR_TRAP(int dActor_c::OnAttacked1(dActor_c &), "dActor_c::OnAttacked1")
ACTOR_TRAP(int dActor_c::OnAttacked2(dActor_c &), "dActor_c::OnAttacked2")
ACTOR_TRAP(void dActor_c::OnKicked(dActor_c &), "dActor_c::OnKicked")
ACTOR_TRAP(int dActor_c::OnPushed(dActor_c &), "dActor_c::OnPushed")
ACTOR_TRAP(int dActor_c::OnHitByCannonBlastedChar(dActor_c &), "dActor_c::OnHitByCannonBlastedChar")
ACTOR_TRAP(void dActor_c::OnHitByMegaChar(Player &), "dActor_c::OnHitByMegaChar")
ACTOR_TRAP(int dActor_c::OnHitFromUnderneath(dActor_c &), "dActor_c::OnHitFromUnderneath")
ACTOR_TRAP(int dActor_c::OnAimedAtWithEgg(), "dActor_c::OnAimedAtWithEgg")
ACTOR_TRAP(Vector3 dActor_c::OnAimedAtWithEggReturnVec(), "dActor_c::OnAimedAtWithEggReturnVec")

ACTOR_TRAP(void dBgActor_c::Kill(), "dBgActor_c::Kill")

// The raw Itanium spelling ArrowSignRight_Spawn.c still calls: cdecl, `this`
// as an ordinary first argument, where the real method is __thiscall.
extern "C" void *_ZTV5Model[8];
extern "C" void _ZN10dBgActor_cC2Ev(void *self)
{
    ::new (self) dBgActor_c();
    // The real constructor builds its Model sub-object at +0xd4 by calling
    // Model::Model() directly, which stores MSVC's own five-slot table -- it
    // never goes through the _ZN5ModelC1Ev bridge in hal/ctor_bridge.cpp, so
    // the dual-numbered synthetic table that bridge installs is bypassed here.
    // Re-stamp it: gate 9 dispatches Model's virtuals by ROM slot number, and
    // ROM numbering carries two destructor slots where MSVC carries one.
    *(void **)((char *)self + 0xd4) = _ZTV5Model;
}

// dActor_c::operator delete reaches the two-argument Deallocate; the port's
// Memory layer spells it with one.
extern "C" void _ZN6Memory10DeallocateEPv(void *p);
extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *p, void *) { _ZN6Memory10DeallocateEPv(p); }

// dBase_c sits between fBase_c and dActor_c; its own two undefined slots come
// along with the same vtable emission.
ACTOR_TRAP(fBase_c::~fBase_c(), "fBase_c::~fBase_c")
ACTOR_TRAP(void dBase_c::AfterInitResources(u32), "dBase_c::AfterInitResources")
