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
       to func_02039e48, which transforms a Vector3 into the collider's local
       frame, and scales 0x048 by the collider's uniform scale before passing it
       as the `Fix12<int> radius` argument of dBgCh_SphCrr::SetObjAndSphere -- so
       0x03c is twelve bytes of centre and 0x048 is the radius that follows it. */
    /* A Vector3 DISPLACEMENT. dBgCh_Actr's Update* methods read these three
       words (their Actr +0x6c) as a Vector3 and feed them to the movement
       helpers; named when Actr's flat blob was typed out. */
    Vector3 disp;           /* 0x04c */
    /* Broad-phase box the query methods maintain: func_02037a04 reads both
       vectors out, func_02037a6c unions two points in, func_02037b1c clears
       all six words, and func_02037a38 sums them into disp. */
    Vector3 aabbMin;        /* 0x058 */
    Vector3 aabbMax;        /* 0x064 */
    /* Result flags, read and OR-ed a bit at a time by the same function:
       1 = any hit, 4 = floor, 8 = wall, 0x10 = from underneath. Each bit gates
       copying the matching dBgPi below it. */
    u8  flags;              /* 0x070 */
    u8  pad_071[0x3];
    dBgPi mClsnResult1;     /* 0x074 */
    dBgPi mClsnResult2;     /* 0x09c */
    dBgPi mClsnResult3;     /* 0x0c4 */
    s32 unk_0ec;            /* 0x0ec */
    u8  pad_0f0[0xc];
    /* An adjacent pair, kept unnamed. All that is evidenced is the shape of
       their use in dBgW_KcMbg::DetectClsn(dBgCh_SphCrr&): 0x100 is
       compared `<` against the local query's own 0x100, and on winning, 0x0fc
       is handed BY ADDRESS to func_0203794c -- i.e. a score at 0x100 selecting
       a payload that starts at 0x0fc. What the payload is is not settled here. */
    s32 unk_0fc;            /* 0x0fc */
    s32 unk_100;            /* 0x100 */
    s32 unk_104;            /* 0x104 - third payload word: func_0203794c writes
                               0x0fc..0x104 as one three-word copy and
                               func_02037b5c clears it */
    s32 mScale;            /* 0x108 - dBgCh_Actr's Update* copy its tail word
                               (Actr +0x128) here each update */
    s32 unk_10c;            /* 0x10c - named 2026-08-24 when the size pin
                               landed: dBgCh_Actr::Init stores its fourth
                               argument, a Vector3_16 *, at Actr +0x12c = this
                               +0x10c, and UpdateContinuous forwards both it
                               and the next word to func_02038324 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    /* Defined as real C++ in separate D1/D0 source files. Dedicated TUs with
     * that same definition enroll the compiler-emitted -0x10 and -0x38
     * adjustment thunks; objisolate keeps one ABI artifact per source and
     * binds its vptr references to the ROM's existing tables. */
    virtual ~dBgCh_SphCrr();

    /* DECLARED, defined out of line in src/engine/collision/dBgCh_SphCrr.cpp as
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
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }

    /* methods */
    void SetFloorResult(const dBgPi & src_);
    void SetObjAndSphere(const Vector3 &pos, Fix12<int> radius, dActor_c *actor);
    int  DetectClsn();

    /* Result-slot accessors. mClsnResult1/2/3 are the floor / wall /
       underneath hit records: dBgCh_Actr::GetFloorResult and
       ::GetWallResult return through func_02037938 / func_020378dc, and the
       dBgW_KcMbg DetectClsn merges into the same slots behind flag bits
       4 / 8 / 0x10. */
    dBgPi *GetFloorResult();
    dBgPi *GetWallResult();
    dBgPi *GetUnderResult();
    void SetWallResult(const dBgPi &src_);
    void SetUnderResult(const dBgPi &src_);

    /* Members the ROM keeps unnamed; each takes the SphCrr object as its
       first argument in the cartridge call graph and drives the evidenced
       fields above. */
    void func_02037940(u8 flags_);
    void func_0203794c(const s32 *payload);
    void func_02037968(int i, int clsnID, dActor_c *owner, dBgW *collider);
    void func_0203798c(int triID, void *src);
    void func_0203799c(int i, int clsnID, dActor_c *owner, dBgW *collider);
    void func_020379c0(int triID, void *src);
    void func_020379d0(int i, int clsnID, dActor_c *owner, dBgW *collider);
    void func_020379f4(int triID, void *src);
    void func_02037a04(Vector3 *outMin, Vector3 *outMax);
    void func_02037a38();
    void func_02037a6c(s32 minX, s32 minY, s32 minZ, s32 maxX, s32 maxY, s32 maxZ);
    void func_02037b1c();
    void func_02037b5c();
    int  func_02038824();
    int  func_02038a38();
};

/* SIZE PINNED AT 0x110 by dBgW_KcMbg::DetectClsn(dBgCh_SphCrr&): the ROM gives
   that function's local query object an exact 0x110 stack slot, and compiling
   it against a 0x10c declaration came out one word short of the frame. That
   measurement is what let the byte stand-in there stay honest; growing this
   class is what will eventually let the stand-in become the real type. */
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
    Vector3 aabbMin;        /* 0x058 */
    Vector3 aabbMax;        /* 0x064 */
    u8  flags;              /* 0x070 - see the C++ branch; Actr's accessors
                               call this byte their mClsnFlags home */
    u8  pad_071[0x3];
    struct dBgPi mClsnResult1; /* 0x074 */
    struct dBgPi mClsnResult2; /* 0x09c */
    struct dBgPi mClsnResult3; /* 0x0c4 */
    s32 unk_0ec;            /* 0x0ec */
    u8  pad_0f0[0xc];
    s32 unk_0fc;            /* 0x0fc */
    s32 unk_100;            /* 0x100 */
    s32 unk_104;            /* 0x104 */
    s32 mScale;            /* 0x108 */
    s32 unk_10c;            /* 0x10c - see the C++ branch: Init's Vector3_16 * */
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
