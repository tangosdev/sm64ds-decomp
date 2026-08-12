/* Hand-written from the class's own spawner and destructor, in the shape
 * tools/gen_header.py produces for its siblings. Nothing here is guessed:
 *
 *   sizeof 0x368       MetalNetLift_Spawn asks ActorBase::operator new for
 *                      872 bytes and constructs a Platform in them.
 *   Model     0x0d4    ~TiltingPlatformLll calls _ZN5ModelD1Ev at +0xd4, and D1
 *                      rather than D2, so it is the type and not a base.
 *   MovingMeshCollider 0x124   same, _ZN18MovingMeshColliderD1Ev at +0x124.
 *                      Its own size assert (0x1c8) closes it at 0x2ec.
 *   PathPtr   0x360    MetalNetLift_Spawn calls _ZN7PathPtrC1Ev at +0x360,
 *                      and PathPtr is 8 bytes, which closes the object.
 *
 * Every one of those offsets is a relocation the ROM build checks. The two
 * spans left as padding are genuinely unreached: the only methods this class
 * still has are cross-overlay veneers that touch no field at all.
 *
 * Flat rather than `: Platform`, matching MetalNet.h and the rest of the
 * generated headers -- the spawner proves a Platform is constructed in the
 * storage, but Platform's own layout is not recovered here.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef TILTINGPLATFORMLLL_H
#define TILTINGPLATFORMLLL_H
#include "types.h"
#include "Model.h"
#include "MovingMeshCollider.h"
#include "PathPtr.h"

struct TiltingPlatformLll {
    u8  pad_000[0xd4];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. */
    Model mModel;            /* 0x0d4 */
    /* MovingMeshCollider member, named by the class's own destructor calling
       MovingMeshCollider's D1 at +0x124 -- a relocation the ROM build
       checks. [_ZN18TiltingPlatformLllD1Ev.c] */
    MovingMeshCollider mMovingMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x74];
    /* PathPtr member, named by MetalNetLift_Spawn calling _ZN7PathPtrC1Ev at
       +0x360 -- a relocation the ROM build checks. */
    PathPtr mPath;            /* 0x360 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int InitResources();
#endif
};

#endif
