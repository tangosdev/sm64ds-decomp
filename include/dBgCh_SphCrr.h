/* Sphere collision query: a dM3dGSph probe sphere swept against the KCL
 * collider registry, keeping a dBgPi best-hit record and three result slots
 * (floor / wall / underneath). The ROM's __vmi_class_type_info states the
 * base list outright:
 *
 *     dBgCh    @ 0x00   polymorphic
 *     dBgPi    @ 0x10   polymorphic   (secondary block VTable_dBgPi_dBgCh_SphCrrThunk)
 *     dM3dGSph @ 0x38   polymorphic   (secondary block VTable_dM3dGSph_dBgCh_SphCrrThunk;
 *                                     this IS the query sphere below)
 *
 * The C branch below stays field-for-field identical to the C++ layout:
 * .c translation units reach into these interiors by the old member names.
 */
#ifndef DBGCH_SPHCRR_H
#define DBGCH_SPHCRR_H
#include "types.h"
#include "dBgPi.h"

#ifdef __cplusplus

#include "math/Fix12.h"
#include "dBgCh.h"
#include "dM3dGSph.h"

/* fwd */
struct dActor_c;

struct dBgCh_SphCrr : dBgCh, dBgPi, dM3dGSph {
    /* 0x38..0x4b is the dM3dGSph base itself: vptr at 0x38, centre (pos) at
       0x3c, radius at 0x48 -- see include/dM3dGSph.h.

       THE QUERY SPHERE. dBgW_KcMbg::DetectClsn(dBgCh_SphCrr&) hands 0x03c
       to WorldToLocalPosition, which transforms a Vector3 into the collider's local
       frame, and scales 0x048 by the collider's uniform scale before passing it
       as the `Fix12i radius` argument of dBgCh_SphCrr::SetObjAndSphere -- so
       0x03c is twelve bytes of centre and 0x048 is the radius that follows it. */
    /* A Vector3 DISPLACEMENT. dBgCh_Actr's Update* methods read these three
       words (their Actr +0x6c) as a Vector3 and feed them to the movement
       helpers; named when Actr's flat blob was typed out. */
    Vector3 disp;           /* 0x04c */
    /* Per-axis range of the push-out vectors the contacts produce, not a
       spatial box: dBgW_Kc's sphere query tracks the lowest and highest push
       per axis and hands both to ExpandPushOutBounds, which widens this range to
       cover them. ResetDisplacement clears all six words, GetPushOutBounds reads
       them out, and UpdateDisplacement sums min + max into disp, so opposing
       pushes on one axis cancel and same-sign ones keep the largest. */
    Vector3 mPushOutMin; /* 0x058 */
    Vector3 mPushOutMax; /* 0x064 */
    /* Result flags, read and OR-ed a bit at a time by the same function:
       1 = any hit, 4 = floor, 8 = wall, 0x10 = from underneath. Each bit gates
       copying the matching dBgPi below it. */
    u8  flags;              /* 0x070 */
    u8  pad_071[0x3];
    dBgPi mClsnResult1;     /* 0x074 */
    dBgPi mClsnResult2;     /* 0x09c */
    dBgPi mClsnResult3;     /* 0x0c4 */
    /* Wall-rejection half extent. When positive, dBgW_Kc's sphere query
       rejects a wall triangle whose three axis projections all lie within
       [-(extent + radius), extent - radius] (the retail comparison at
       0x01ffcfa8); zero, the constructor's value, disables the test.
       dBgW_KcMbg rescales it into the collider's frame with the radius. */
    s32 mWallRejectHalfExtent; /* 0x0ec */
    u8  pad_0f0[0xc];
    /* Normal of the floor contact with the greatest Y component. The retail
       KCL query copies SurfaceInfo's normal at 0x01ffbe6c, then compares its
       Y against mFloorNormal.y before saving all three words at
       0x01ffd094..0x01ffd0a8 through SetFloorNormal. Reset (ResetQuery)
       uses (0, -0x1000, 0), so the first accepted floor replaces it;
       dBgCh_Actr copies it into its mGroundNormal on a grounded frame. */
    Vector3 mFloorNormal; /* 0x0fc */
    /* Contacts whose normal Y exceeds this are skipped: dBgW_Kc's sphere
       query tests it against every candidate face before classifying it.
       SetObjAndSphere sets 0x1000 (accept every face); dBgCh_Actr's Update*
       copy its own tail word (Actr +0x1b8) here each update. */
    s32 mMaxNormalY;       /* 0x108 */
    s32 mMotionAngPtr;     /* 0x10c - dBgCh_Actr::Init stores its fourth
                               argument, a Vector3_16 *, at Actr +0x12c = this
                               +0x10c, and the Update* bodies forward it and
                               Actr +0x130 to func_02038324, which hands them
                               to dBgW::BeforeClsn as `motionAng` and `ang` */

    /* --- vtable, in ROM order. Do not reorder. --- */
    /* Defined as real C++ in separate D1/D0 source files. Dedicated TUs with
     * that same definition enroll the compiler-emitted -0x10 and -0x38
     * adjustment thunks; objisolate keeps one ABI artifact per source and
     * binds its vptr references to the ROM's existing tables. */
    virtual ~dBgCh_SphCrr();

    /* DECLARED, defined out of line in src/engine/collision/d_bg_ch_sph_crr.cpp as
     * real C++ -- complete-object context for every ROM caller, hence C1.
     */
    dBgCh_SphCrr();

    /* ITS OWN COPY, TO RESOLVE AN AMBIGUITY THE MULTIPLE INHERITANCE CREATES.
       dBgCh_SphCrr derives from both dBgCh and dBgPi, and each of those declares
       an inline operator delete, so an inherited one is "ambiguous access to
       name found: dBgCh::operator delete and dBgPi::operator delete".
       Declaring it here picks the same deallocator both bases name
       (Memory::operator_delete2, 0x0203cbcc) and satisfies the rule in
       include/dActor_c.h that mwcc only inlines the member when it is in the
       class or its immediate base. This is load-bearing for the D0 at
       0x02037c40; a non-virtual inline member adds no field or vtable slot. */
    void operator delete(void *ptr) {
        Memory::operator_delete2(ptr);
    }

    /* methods */
    void SetFloorResult(const dBgPi &result);
    void SetObjAndSphere(const Vector3 &pos, Fix12i radius, dActor_c *actor);
    int  DetectClsn();

    /* Result-slot accessors. mClsnResult1/2/3 are the floor / wall /
       underneath hit records: dBgCh_Actr::GetFloorResult and
       ::GetWallResult return through func_02037938 / func_020378dc, and the
       dBgW_KcMbg DetectClsn merges into the same slots behind flag bits
       4 / 8 / 0x10. */
    dBgPi *GetFloorResult();
    dBgPi *GetWallResult();
    dBgPi *GetUnderResult();
    void SetWallResult(const dBgPi &result);
    void SetUnderResult(const dBgPi &result);

    /* Inferred names for the accumulated collision push-out range. */
    void ResetDisplacement();
    void ExpandPushOutBounds(s32 minX, s32 minY, s32 minZ, s32 maxX, s32 maxY, s32 maxZ);
    void GetPushOutBounds(Vector3 *outMin, Vector3 *outMax);
    void UpdateDisplacement();

    /* Inferred names for the per-class result writers. SetFloorNormal saves
       mFloorNormal; the rest forward to the floor, wall or under dBgPi
       (mClsnResult1/2/3). */
    void SetFloorNormal(const s32 *normal);
    void SetFloorCollider(int i, int clsnID, dActor_c *owner, dBgW *collider);
    void RecordFloorHit(int triID, void *src);
    void SetWallCollider(int i, int clsnID, dActor_c *owner, dBgW *collider);
    void RecordWallHit(int triID, void *src);
    void SetUnderCollider(int i, int clsnID, dActor_c *owner, dBgW *collider);
    void RecordUnderHit(int triID, void *src);

    /* Members the ROM keeps unnamed; each takes the SphCrr object as its
       first argument in the cartridge call graph and drives the evidenced
       fields above. */
    void SetQueryFlags(u8 flags_);
    /* Inferred: clear flags, hit records, displacement and floor normal. */
    void ResetQuery();
    int  func_02038824();
    int  func_02038a38();
};

/* SIZE PINNED AT 0x110 by dBgW_KcMbg::DetectClsn(dBgCh_SphCrr&): the ROM gives
   that function's local query object an exact 0x110 stack slot, and compiling
   it against a 0x10c declaration came out one word short of the frame. That
   local is now declared as this real type. */
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dBgCh_SphCrr_size_must_be_0x110[
    sizeof(dBgCh_SphCrr) == 0x110 ? 1 : -1];
#endif

#else

struct dBgCh_SphCrr {
    u8  pad_000[0x10];
    u8  mBgPiBase;            /* 0x010 */
    u8  pad_011[0x27];
    u8  mSphereBase;            /* 0x038 */
    u8  pad_039[0x3];
    Vector3 pos;            /* 0x03c */
    Fix12i radius;          /* 0x048 */
    Vector3 disp;           /* 0x04c */
    Vector3 mPushOutMin;    /* 0x058 */
    Vector3 mPushOutMax;    /* 0x064 */
    u8  flags;              /* 0x070 - see the C++ branch; Actr's accessors
                               call this byte their mClsnFlags home */
    u8  pad_071[0x3];
    struct dBgPi mClsnResult1; /* 0x074 */
    struct dBgPi mClsnResult2; /* 0x09c */
    struct dBgPi mClsnResult3; /* 0x0c4 */
    s32 mWallRejectHalfExtent; /* 0x0ec - see the C++ branch */
    u8 pad_0f0[0xc];
    Vector3 mFloorNormal;  /* 0x0fc - see the C++ branch */
    s32 mMaxNormalY;       /* 0x108 */
    s32 mMotionAngPtr;     /* 0x10c - see the C++ branch: Init's Vector3_16 * */
};

typedef struct dBgCh_SphCrr dBgCh_SphCrr;

/* Same pin on the C view -- the two branches must agree while anything can
   still substitute one for the other. */
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dBgCh_SphCrr_c_size_must_be_0x110[
    sizeof(struct dBgCh_SphCrr) == 0x110 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif
