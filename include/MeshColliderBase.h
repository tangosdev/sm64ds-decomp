#ifndef MESHCOLLIDERBASE_H
#define MESHCOLLIDERBASE_H

#include "types.h"

/* The root of the mesh-collision hierarchy, vtable _ZTV16MeshColliderBase
 * at 0x02099388. The chain: MeshColliderBase -> MeshCollider (static KCL
 * mesh) and MeshColliderBase -> ... -> MovingMeshCollider ->
 * ExtendingMeshCollider.
 *
 * VTABLE, 13 slots, read out of the ROM:
 *
 *   slot  0/1  ~MeshColliderBase (see THE TWO DESTRUCTOR BODIES below)
 *   slot  2    Virtual08()                      - no-op here
 *   slot  3    null - pure; derived tables hold GetSurfaceInfo in ITCM
 *              (0x01ffd920, named in config/arm9/itcm/symbols.txt)
 *   slot  4    null - pure; GetNormal(s16, Vector3 &) in derived tables
 *   slot  5    null - pure; GetTriangleOrigin(s16, Vector3 &) in derived
 *   slot  6    DetectClsn(RaycastGround &)      - returns false here
 *   slot  7    DetectClsn(RaycastLine &)        - returns false here
 *   slot  8    DetectClsn(SphereClsn &)         - returns false here
 *   slot  9    BeforeClsn(...)                  - invokes the callback
 *   slot 10    TransformPos(const Vector3 &, Vector3 &) - false here
 *   slot 11    GetAngularVelY()                 - 0 here
 *   slot 12    GetVelocity(Vector3 &)           - zeroes it here
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- the
 * key-function arrangement from include/ModelBase.h.
 *
 * THE TWO DESTRUCTOR BODIES. Both are `ldr r1,[pc,#4]; str r1,[r0]; bx lr'
 * storing the vtable word, and they are byte-identical -- but the compiler
 * emitted BOTH, so this is not a one-body alias:
 *
 *   0x0203968c  sits in vtable slot 0, and nothing bl's it   -> D1
 *   0x02039658  bl'd by MeshCollider's D0/D1/D2, never in a
 *               vtable (currently named func_02039658)       -> D2
 *
 * By the #774 rule (a class vtable carries [D1, D0] and never D2; D2 is
 * reached by direct call alone) the symbol names are on the wrong bodies:
 * symbols.txt calls 0x0203968c `_ZN16MeshColliderBaseD2Ev'. Correcting that
 * pair is a symbols.txt rename, deliberately left to its own change so this
 * one stays byte-neutral. MeshCollider next door emits a byte-identical
 * D1/D2 pair too (0x02039864 in the vtable, 0x020397fc direct-called), so
 * "only one body is ever emitted" does not hold for this family.
 *
 * LAYOUT is pinned by the shared init func_02039624 (called from C2) and
 * Enable/Disable/IsEnabled: 0x18 in the slot byte means "not enabled",
 * and enabled colliders live in the 0x18-entry table at data_020a0c80.
 */

#ifdef __cplusplus

struct Actor;
struct ClsnResult;
struct RaycastGround;
struct RaycastLine;
struct SphereClsn;
struct SurfaceInfo;

#ifndef VECTOR3_16_DEFINED
#define VECTOR3_16_DEFINED
/* The game's own name for the s16 vector, as its mangled symbols spell it. */
struct Vector3_16 { s16 x, y, z; };
#endif

struct MeshColliderBase {
    /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
    Actor *owner;             /* 0x04 - set by Enable, null when free */
    s32 ownerUniqueID;        /* 0x08 - owner->uniqueID, -1 when free */
    Fix12i unk_0c;            /* 0x0c - init -0x1000 */
    s32 unk_10;               /* 0x10 */
    u8 slotIdx;               /* 0x14 - index into data_020a0c80; 0x18 = disabled */
    u8 pad_15[3];
    void (*beforeClsnCallback)(MeshColliderBase *self, Actor *actor,
                               ClsnResult *res, Vector3 *pos,
                               Vector3_16 *motionAng, Vector3_16 *ang); /* 0x18 */
    s32 unk_1c;               /* 0x1c */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~MeshColliderBase();                          /* slots 0/1 */
    virtual void Virtual08();                             /* slot 2 */
    virtual void GetSurfaceInfo(s16 triID, SurfaceInfo &res) = 0; /* slot 3 - ITCM in derived */
    virtual void GetNormal(s16 triID, Vector3 &res) = 0;  /* slot 4 */
    virtual void GetTriangleOrigin(s16 triID, Vector3 &res) = 0; /* slot 5 */
    virtual int DetectClsn(RaycastGround &ray);           /* slot 6 */
    virtual int DetectClsn(RaycastLine &ray);             /* slot 7 */
    virtual int DetectClsn(SphereClsn &sphere);           /* slot 8 */
    virtual void BeforeClsn(ClsnResult &res, Actor *actor, Vector3 &pos,
                            Vector3_16 *motionAng, Vector3_16 *ang); /* slot 9 */
    virtual int TransformPos(const Vector3 &pos, Vector3 &res);      /* slot 10 */
    virtual s16 GetAngularVelY();                         /* slot 11 */
    virtual void GetVelocity(Vector3 &res);               /* slot 12 */

    /* --- non-virtual --- */
    int Enable(Actor *actor);
    int Disable();
    int IsEnabled();

    /* --- static: the stock BeforeClsn callbacks. Call sites take their
       addresses and store them via func_020393d4; BeforeClsn invokes the
       stored one as (collider, actor, res, pos, motionAng, ang). They
       update POS from the collider's motion; the ClsnResult rides along
       untouched. --- */
    static void UpdatePosAndAngs(MeshColliderBase &clsn, Actor *clsnActor,
                                 ClsnResult &res, Vector3 &pos,
                                 Vector3_16 *motionAng, Vector3_16 *ang);
    static void UpdatePosWithVelocity(MeshColliderBase &clsn, Actor *clsnActor,
                                      ClsnResult &res, Vector3 &pos,
                                      Vector3_16 *motionAng, Vector3_16 *ang);
    static void UpdatePosWithTransform(MeshColliderBase &clsn, Actor *clsnActor,
                                       ClsnResult &res, Vector3 &pos,
                                       Vector3_16 *motionAng, Vector3_16 *ang);
    static void UpdateAngsWithAngularVelY(MeshColliderBase &clsn, Actor *clsnActor,
                                          ClsnResult &res, Vector3 &pos,
                                          Vector3_16 *motionAng, Vector3_16 *ang);
};

typedef char MeshColliderBase_size_must_be_0x20[sizeof(MeshColliderBase) == 0x20 ? 1 : -1];

#else

struct MeshColliderBase {
    void **vtable;            /* 0x00 */
    struct Actor *owner;      /* 0x04 */
    s32 ownerUniqueID;        /* 0x08 */
    s32 unk_0c;               /* 0x0c */
    s32 unk_10;               /* 0x10 */
    u8 slotIdx;               /* 0x14 */
    u8 pad_15[3];
    void *beforeClsnCallback; /* 0x18 */
    s32 unk_1c;               /* 0x1c */
};

#endif /* __cplusplus */

#endif /* MESHCOLLIDERBASE_H */
