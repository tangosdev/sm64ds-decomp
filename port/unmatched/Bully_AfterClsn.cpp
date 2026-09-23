/* BULLY's AfterClsn extension virtual (daDonketu_c::AfterClsn, vtable SLOT 32;
 * hal/actor_classes_ov064.cpp fills it at vt[32], which is also where PathLift
 * carries the same extension virtual), the coin pop on the knocked-into-lava
 * path plus the group census bump on the Big Bully that spawned it.
 *
 * HOST COPY RETIRED (run linkfull wave 23, lane HGFRONT1). The body is the
 * matched TU src/_ZN5Bully16UpdateDeathStateEv.cpp again, on
 * port/slice_w23_hostgen.txt. It stood here because the old spelling of that TU
 * re-declared its shadow's two statics at namespace scope, which MSVC rejects
 * (C2761). The synced TU has no shadow and no redeclaration left: it is a real
 * member, `void Bully::UpdateDeathState()` against include/Bully.h:48, it
 * compiles clean under walk_window's own flags, and it is MATCHING at 2004/b56
 * with --strict-relocs against the cartridge (ov064 0x02117220, 0xf0 bytes).
 * The object MSVC builds from it reads and writes the same offsets the body
 * here did (+0x5c/+0x60/+0x64, +0x94, +0xcc, the coin's +0x98/+0xa4/+0xa8/+0xac,
 * +0x3fc, and the census byte at +0x3fe), in the same order.
 *
 * WHAT STAYS IS THE FACE. The slot-32 shim in hal/actor_classes_ov064.cpp
 * (bly_v32) calls this flat C name with the receiver on the stack, and the
 * matched body is __thiscall with the receiver in ECX, so an /alternatename
 * cannot bridge them (a NAME bridge, never an ABI bridge). This is the reverse
 * face port/faces_sync.txt generates for BigBully's own UpdateDeathState
 * (0x021175cc), in the same shape: a shadow class declaring only the member, so
 * the qualified call names ?UpdateDeathState@Bully@@UAEXXZ directly and needs no
 * header. It is written here rather than as a ledger row because that ledger
 * belongs to another lane this wave; moving it there is a one-row follow-up.
 * The binding was checked the way facegen checks a row: ROM 0x02117220 is
 * _ZN5Bully16UpdateDeathStateEv in config/arm9/overlays/ov064/symbols.txt and
 * no other symbol shares the address; the member's decorated name reads
 * `public: virtual void __thiscall Bully::UpdateDeathState(void)`, whose class,
 * method, arity (0) and constness are the Itanium name's; the return is void,
 * so there is nothing to widen.
 */

/* the shadow's static spellings onto the hosted C bodies; the Spawn one
   already exists twice (actor_faces_bob / bob_enemy_bridges), FindWithID's
   is new */
/* RETIRED at ALIAS2 (wave 8, the main -> port sync). DEAD RHS and an UNREFERENCED left hand side: nothing in the build defines __ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as, and nothing references ?Spawn@dActor_c@@SAPAU1@IIABUVector3@@PBUVector3_16@@HH@Z, so the row can never fire and nothing wants it to. */
// #pragma comment(linker, "/alternatename:?Spawn@dActor_c@@SAPAU1@IIABUVector3@@PBUVector3_16@@HH@Z=__ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as")
#pragma comment(linker, "/alternatename:?FindWithID@dActor_c@@SAPAU1@I@Z=__ZN8dActor_c10FindWithIDEj")

struct Bully {
    public: virtual void UpdateDeathState();
};

/* ROM 0x02117220 -> ?UpdateDeathState@Bully@@UAEXXZ */
extern "C" void _ZN5Bully16UpdateDeathStateEv(void *self)
{ ((Bully *)self)->Bully::UpdateDeathState(); }
