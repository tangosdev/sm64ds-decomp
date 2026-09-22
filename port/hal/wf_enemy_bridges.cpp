// GATES 64-69: the ov079 enemy cast's link bridges.
//
// The matched ov079 TUs compile as C++ (they are `//cpp` methods and recovered
// functions) and reference two kinds of name MSVC's mangling asks for but the
// port spells differently:
//
//   1. DATA the ov079 mount emits under a plain C name, referenced from a .cpp
//      TU with a file-scope `extern <Type> data_ov079_...`, which MSVC mangles
//      by the C++ type. One /alternatename per (symbol, type) pair -- the
//      ov078/ov084/ov102 case (hal/bob_enemy_bridges.cpp).
//   2. METHOD CALLS a recovered TU spells `self->Method(...)` on a LOCAL struct.
//      MSVC emits a __thiscall reference (this in ecx); the port's real body is
//      a __cdecl free function with `self` as its first STACK argument. An
//      alias would leave `this` in the wrong place -- a fault reading +0x5c off
//      garbage, measured on BILL_BLASTER's first Behavior frame. So each is a
//      REAL method against a shadow class whose decorated symbol matches the
//      caller's exactly, forwarding to the C-named body -- the bob_enemy_bridges
//      forwarding-face direction. Only the __cdecl statics/frees are aliased.
//
// Everything forwarded to is already linked from an existing slice or the ov079
// mount; this file adds only the names and the ecx->stack shuffle MSVC needs.
#include <cstdio>
#include <cstdlib>

extern "C" {
// the C-named __cdecl bodies the shadow methods forward to
void *_ZN8dActor_c13ClosestPlayerEv(void *self);
void *_ZN8dActor_c18ClosestWithActorIDEj(void *self, unsigned id);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
int _ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(void *self, short a,
                                                    short b, short c, int d);
}

// ---- __thiscall method faces (shadow classes, ROM names) -------------------
// Each shadow struct's name and method signature reproduce the caller's mangled
// symbol; the body forwards to the C __cdecl definition with `this` as the
// first argument.
struct Actor {
    Actor *ClosestWithActorID(unsigned id);
};
Actor *Actor::ClosestWithActorID(unsigned id)
{ return (Actor *)_ZN8dActor_c18ClosestWithActorIDEj(this, id); }

/* _ZN11BillBlaster8BehaviorEv spells ClosestPlayer on a local `Actor_s`. */
struct Actor_s {
    Actor *ClosestPlayer();
};
Actor *Actor_s::ClosestPlayer()
{ return (Actor *)_ZN8dActor_c13ClosestPlayerEv(this); }

/* The matched Platform::UpdateKillByMegaChar spells its fourth parameter
   Fix12<int> against a template it declares itself; the ov079 callers above
   spell the same method with a plain int. Two different decorated symbols, so
   both can be declared on one shadow class. This is the matched TU's spelling,
   reproduced so the call below decorates as
   ?UpdateKillByMegaChar@dBgActor_c@@QAEHFFFU?$Fix12@H@@@Z -- mangling depends on
   the template's NAME and argument, not on its members. */
template<class T> struct Fix12 { T v; };

struct Platform {
    int IsClsnInRange(int a, int b);
    int UpdateKillByMegaChar(short a, short b, short c, int d);
    int UpdateKillByMegaChar(short a, short b, short c, Fix12<int> d);
};
int Platform::IsClsnInRange(int a, int b)
{ return _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, a, b); }
int Platform::UpdateKillByMegaChar(short a, short b, short c, int d)
{ return _ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(this, a, b, c, d); }

/* THE GATE-16 FLIP for UpdateKillByMegaChar (lane w8-shadows). hal/megachar_stub.cpp
   owned the ROM's C name with a `return 0` stub and is now compiled only into
   the three narrow gate-9 harnesses (smoke_actor / smoke_savestate /
   smoke_persist), which have no Player gates and no RaycastLine. In the three
   real targets the C name is this face and the body is the matched TU, which
   was already sitting in slice_gate16.txt line 178, compiled, and dropped by
   /OPT:REF for want of a single reference edge -- the wave-1 finding, again.
   The two manglings never collided, which is why nothing ever failed loudly.

   WHAT IT COSTS, measured against walk_window.map at the lane base rather than
   estimated. The matched TU names nine externals and EIGHT are already linked:
   Matrix4x3_FromRotationY, MulVec3Mat4x3, Vec3_Add, DecIfAbove0_Byte,
   data_020a0e68, ?UpdatePos@dActor_c@@ (actor_class_faces.cpp), and the RaycastLine
   trio ??0/??1/?SetObjAndLine@ (reverse_bridges.cpp) plus
   ?DetectClsn@dBgCh_Lin@@QAEHXZ (its own matched TU). The ninth,
   func_ov002_020ee5d0, is in slice_gate16.txt line 302 and was dropped for the
   same reason; the matched body's call to it is the edge that seats it. So the
   flip seats TWO TUs and adds no slice line.

   THE COST NOTE IN megachar_stub.cpp WAS WRONG, and it is worth saying how. It
   listed three missing externals, two of them spelled
   _ZN11RaycastLine14SetObjAndLine... and _ZN11RaycastLine11DetectClsnEv. Those
   Itanium length prefixes do not match the identifiers they precede --
   SetObjAndLine is 13 characters and DetectClsn is 10 -- so neither name is any
   symbol. The real ones (13/10) were in the binary the whole time. A mistyped
   length prefix reads as a plausible symbol and greps to nothing, which is
   exactly how a seat stays parked: the ruling that it was work was right, the
   arithmetic under it was not.

   Vtable numbering is safe here, unlike the Bird/Flag renders: the matched TU
   dispatches ((PlatformVT*)this)->v31() through a THIRTY-TWO virtual local
   struct, and hal/actor_classes.cpp's hal_fill_platform_vtable writes
   _ZTV10dBgActor_c in ROM order with vt[31] = ac_kill. Index 31 is Platform::Kill
   in both numberings. */
/* SM64DS_MEGACHAR_PROBE=1 counts the calls and how many of them get past the
   matched body's first line. That first line is `if (this->f_31c == 0) return
   0;`, which is also exactly what the retired stub did, so a matrix that goes
   green proves nothing on its own -- the seat would look identical to the old
   stub if every platform early-outs. The probe is what separates "linked" from
   "running", and it reports both halves: reached, and past-the-early-out. */
extern "C" int _ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(
    void *self, short a, short b, short c, int d)
{
    Fix12<int> fd;
    fd.v = d;
    static int probe = -1;
    if (probe < 0) probe = std::getenv("SM64DS_MEGACHAR_PROBE") ? 1 : 0;
    if (!probe)
        return ((Platform *)self)->UpdateKillByMegaChar(a, b, c, fd);
    static long calls, live;
    ++calls;
    /* f_31c is the matched TU's own offset for the enable byte it early-outs
       on; read it here only to classify the call, never to change it. */
    if (*((unsigned char *)self + 0x31c)) ++live;
    int r = ((Platform *)self)->UpdateKillByMegaChar(a, b, c, fd);
    if ((calls % 600) == 0 || (live && live % 60 == 0))
        std::fprintf(stderr, "[megachar] %ld calls, %ld past the early-out\n",
                     calls, live);
    return r;
}

extern "C" {

// ---- data symbols the ov079 mount emits under a plain C name --------------
// Each line is one (symbol, C++-type-mangling) pair a matched TU references.
// The mangling is read straight off the linker's own error text.
#pragma comment(linker, "/alternatename:?data_ov079_021275ec@@3PAPAXA=_data_ov079_021275ec")
#pragma comment(linker, "/alternatename:?data_ov079_021275ec@@3PAPAUAnim@@A=_data_ov079_021275ec")
#pragma comment(linker, "/alternatename:?data_ov079_02128058@@3PADA=_data_ov079_02128058")
#pragma comment(linker, "/alternatename:?data_ov079_02128058@@3PAUE12@@A=_data_ov079_02128058")
#pragma comment(linker, "/alternatename:?data_ov079_0212805c@@3PADA=_data_ov079_0212805c")
#pragma comment(linker, "/alternatename:?data_ov079_0212805c@@3PAUE12@@A=_data_ov079_0212805c")
#pragma comment(linker, "/alternatename:?data_ov079_02128060@@3PAUE12@@A=_data_ov079_02128060")
#pragma comment(linker, "/alternatename:?data_ov079_02128168@@3USFP@@A=_data_ov079_02128168")
#pragma comment(linker, "/alternatename:?data_ov079_02128170@@3PAPAXA=_data_ov079_02128170")
#pragma comment(linker, "/alternatename:?data_ov079_02128178@@3USFP@@A=_data_ov079_02128178")

// ---- __cdecl static / free calls (a direct alias is exact) -----------------
// Sound::PlaySecretSound is a free function (YAH... = __cdecl), and the two
// Actor_s statics have no `this`, so the arg layout already matches the body.
/* RETIRED at ALIAS2 (wave 8, the main -> port sync). DEAD RHS and an UNREFERENCED left hand side: nothing in the build defines __ZN5Sound15PlaySecretSoundEP8dActor_cPt, and nothing references ?PlaySecretSound@Sound@@YAHPAUActor@@PAG@Z, so the row can never fire and nothing wants it to. */
// #pragma comment(linker, "/alternatename:?PlaySecretSound@Sound@@YAHPAUActor@@PAG@Z=__ZN5Sound15PlaySecretSoundEP8dActor_cPt")
#pragma comment(linker, "/alternatename:?FindWithID@Actor_s@@SAPAUActor@@I@Z=__ZN8dActor_c10FindWithIDEj")
#pragma comment(linker, "/alternatename:?Spawn@Actor_s@@SAPAUActor@@IIABUVector3@@PBUVector3_16@@HH@Z=__ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as")

// Whomp::InitResources takes the ADDRESS of MeshColliderBase::UpdatePosAndAngs
// (declared _ZN... at file scope without extern "C", so MSVC mangles the
// mangled name). It is only ever used as a function pointer, so the alias onto
// the real body is exact.
/* RETIRED at ALIAS2 (wave 8, the main -> port sync). DEAD RHS and an UNREFERENCED left hand side: nothing in the build defines __ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_, and nothing references ?_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_@@YAXXZ, so the row can never fire and nothing wants it to. */
// #pragma comment(linker, "/alternatename:?_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_@@YAXXZ=__ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_")

// ---- func_021135d4: a CROSS-OVERLAY CLPS block -----------------------------
// Whomp::InitResources passes &func_021135d4 as a CLPS_Block* to
// MovingMeshCollider::SetFile. The reloc at ov079 0x02126570 is
// `to:0x021135d4 module:overlays(9,14,15,16,18,19,...)` -- the shared level-
// overlay base, so the address names whatever level is loaded. WHOMP only
// spawns in Whomp's Fortress (level 7 = ov015), so it is ov015's copy, which the
// ov015 per-symbol mount already hosts as data_ov015_021135d4. The matched src
// spells the generic address label `func_021135d4`; bind it to that copy.
#pragma comment(linker, "/alternatename:_func_021135d4=_data_ov015_021135d4")

}  /* extern "C" */
