// Lane DTOR-FACES-CPP (LINKSCOPE lane 10): the real-C++ destructor TUs of
// seated classes, linked from src/ and seated in slot 16.
//
// THE SHAPE. src/_ZN4CoinD1Ev.cpp and its 60-odd siblings are the shadow-
// class C++ form: the TU declares its own `struct Actor { char pad[..];
// virtual ~Actor(); }`, its own `struct CommonModel { char pad[..];
// ~CommonModel(); }` and so on, derives the class, and writes `Coin::~Coin()
// {}`. Under mwcc that byte-matches the ROM's D1 (the member chain and the
// Actor D2 call are what the compiler synthesises). Under MSVC the same TU
// compiles to ??1Coin@@UAE@XZ whose body calls
//
//     ??1WithMeshClsn@@QAE@XZ  ??1MovingCylinderClsn@@QAE@XZ
//     ??1ShadowModel@@QAE@XZ   ??1CommonModel@@QAE@XZ   ??1Actor@@UAE@XZ
//
// decorated __thiscall names that nothing in the port defined, because the
// port carries those bodies under the ROM's C names (_ZN12WithMeshClsnD1Ev,
// _ZN5ActorD2Ev, ...). Every fill so far answered that with a transcription:
// a static thunk spelling the ROM D1's own chain by hand (coin_d1 in
// hal/actor_classes_bob_world.cpp, btail_d1 in hal/actor_classes_ov060.cpp,
// hal/StarMarker_D1.cpp), and the matched TU stayed out of the link.
//
// THE MECHANISM. Two halves, both precedented:
//
//   1. FACES for the shadow-class destructors, one definition per decorated
//      name, forwarding `this` to the C body. This is exactly
//      unmatched/Mg3DEsp_Faces.cpp section 2 (??1Model@@QAE@XZ and friends)
//      and unmatched/RecRoomCupboard_ActorDtorSeam.cpp (??1Actor@@QAE@XZ):
//      an /alternatename would be a RECEIVER-SHAPE mismatch (ecx vs stack),
//      so each is a real __thiscall definition. The shadow class is declared
//      here exactly as the src TUs declare it (same name, same virtual-ness,
//      no members) so the mangle is byte for byte the one the TU asks for.
//      Which body each face forwards to is read off the ROM relocations of
//      the 67 class-form D1 TUs (config/arm9/overlays/*/relocs.txt), not
//      assumed:
//
//        every base call    -> 0x020112c8 _ZN5ActorD2Ev            (67 of 67)
//        every member call  -> that member class's D1, a plain bl:
//          _ZN18MovingCylinderClsnD1Ev        0x020149a4   (32 TUs)
//          _ZN11ShadowModelD1Ev               0x02015ff8   (28)
//          _ZN12WithMeshClsnD1Ev              0x02014a60   (24)
//          _ZN25MovingCylinderClsnWithPosD1Ev              (11)
//          _ZN11CommonModelD1Ev                            (4)
//          _ZN21ExtendingMeshColliderD1Ev                  (1, PoleLift)
//          _ZN5ModelD1Ev / _ZN9ModelAnimD1Ev / _ZN15TextureSequenceD1Ev /
//          _ZN18TextureTransformerD1Ev: faces already defined in
//          unmatched/Mg3DEsp_Faces.cpp, linked in the same four targets.
//
//      All the member calls are plain `bl`s to the D1 (no vptr load), which is
//      what makes the non-virtual Q mangle the right one to define; the base
//      call is to Actor's D2, and the src TUs declare ~Actor VIRTUAL (the U
//      mangle), so ??1Actor@@UAE@XZ is a second, distinct face beside the
//      seam's ??1Actor@@QAE@XZ.
//
//   2. ADAPTERS, one per class, in the vtable-law thunk shape, whose body is
//      the qualified call `((Coin *)s)->Coin::~Coin()`. Qualified, so it is a
//      direct `call ??1Coin@@UAE@XZ` and not a dispatch through slot 16 back
//      into itself. The class is declared here with only its virtual
//      destructor; the mangle depends on nothing else.
//
// WHAT CHANGES AT RUN TIME, honestly. The transcribed thunk stored the port's
// host table (_ZTV4Coin) into the vptr before the chain; MSVC's body stores
// its own one-slot ??_7Coin@@6B@ instead. Nothing dispatches through the
// object between that store and Actor's D2, which stores _ZTV5Actor itself
// (src/_ZN5ActorD2Ev.cpp, first statement), so the window is unobservable.
// Otherwise the chain is the same calls at the same offsets, because MSVC
// lays the shadow structs out the way mwcc does (vptr at 0, char pads, 4-byte
// members): the offsets in the src TUs' comments are the ones both
// compilers produce.
//
// WHAT IS NOT DONE. The D0 half of each pair is a flat .c TU carrying the
// ROM's C name; lane DTOR-PAIRS-C (status/DTORPAIRS.md) already seats those
// and they are untouched. The unseated classes with a class-form D1 (Cloud:
// ov039 not mounted; CameraTag: no table, no edge; InvisiblePole: its D1
// stores _ZTV13InvisiblePole, which is CameraTag's table, the same wall
// DTORPAIRS.md records for its D0) are skipped, see status/DTORFACES.md.

#include "dtor_faces_cpp.h"

extern "C" {
void *_ZN5ActorD2Ev(void *self);
void _ZN18MovingCylinderClsnD1Ev(void *self);
void _ZN25MovingCylinderClsnWithPosD1Ev(void *self);
void _ZN11ShadowModelD1Ev(void *self);
void _ZN12WithMeshClsnD1Ev(void *self);
void _ZN11CommonModelD1Ev(void *self);
void _ZN21ExtendingMeshColliderD1Ev(void *self);
}

/* ---- 1. the shadow-class faces ------------------------------------------ */

struct Actor { virtual ~Actor(); };                          /* ??1Actor@@UAE@XZ */
struct MovingCylinderClsn { ~MovingCylinderClsn(); };
struct MovingCylinderClsnWithPos { ~MovingCylinderClsnWithPos(); };
struct ShadowModel { ~ShadowModel(); };
struct WithMeshClsn { ~WithMeshClsn(); };
struct CommonModel { ~CommonModel(); };
struct ExtendingMeshCollider { ~ExtendingMeshCollider(); };

Actor::~Actor()                                 { _ZN5ActorD2Ev(this); }
MovingCylinderClsn::~MovingCylinderClsn()       { _ZN18MovingCylinderClsnD1Ev(this); }
MovingCylinderClsnWithPos::~MovingCylinderClsnWithPos()
                                                { _ZN25MovingCylinderClsnWithPosD1Ev(this); }
ShadowModel::~ShadowModel()                     { _ZN11ShadowModelD1Ev(this); }
WithMeshClsn::~WithMeshClsn()                   { _ZN12WithMeshClsnD1Ev(this); }
CommonModel::~CommonModel()                     { _ZN11CommonModelD1Ev(this); }
ExtendingMeshCollider::~ExtendingMeshCollider() { _ZN21ExtendingMeshColliderD1Ev(this); }

/* ---- 2. the per-class adapters ------------------------------------------ */

#define CPPD1(Class) \
    struct Class { virtual ~Class(); }; \
    int __fastcall hal_cppd1_##Class(void *s, void *) \
    { ((Class *)s)->Class::~Class(); return (int)(size_t)s; }

/* group 1 (the proof): Coin, hal/actor_classes_bob_world.cpp.
   ROM 0x020b0f54 (ov002): WithMeshClsn, MovingCylinderClsn, ShadowModel,
   CommonModel x2, Actor D2. */
CPPD1(Coin)
