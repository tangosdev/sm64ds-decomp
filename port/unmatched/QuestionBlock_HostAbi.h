/* PORT_HOST_ABI. QuestionBlock's own fields, put back where the ROM keeps
 * them. Force-included ahead of the ten src/_ZN13QuestionBlock*.cpp
 * translation units so they address the ROM's member offsets instead of
 * MSVC's. Nothing in src/ or include/ changes. Run link100, lane CRASH8.
 *
 * The mechanism is the one port/unmatched/MgSmartball_HostAbi.h and
 * port/unmatched/Fader_HostAbi.h already use: pre-define a header's own guard
 * macro, so the src file still says #include "QuestionBlock.h", the include
 * still happens, and the body is skipped because the guard is already defined.
 *
 * ---- WHAT IS WRONG WITHOUT THIS ----------------------------------------
 *
 * MSVC does not let a derived class reuse its base class's TAIL PADDING. The
 * Itanium ABI that mwccarm implements does. dBgActor_c has a DATA size of
 * 0x31e and a sizeof of 0x320, so mwccarm starts QuestionBlock's own fields at
 * 0x31e, include/QuestionBlock.h's `u8 pad_31e[0x2]` fills 0x31e..0x31f, and
 * ModelAnim mModelAnim lands on the ROM's 0x320. MSVC starts them at 0x320
 * instead, the same two-byte pad fills 0x320..0x321, and mModelAnim is pushed
 * to 0x324. Every field of this class from mModelAnim down is then four bytes
 * high, and sizeof becomes 0x3fc against the 0x3f8 the ROM's own factory asks
 * fBase_c::operator new for.
 *
 * DROPPING THE PAD IS THE WHOLE FIX. With no pad_31e, MSVC starts the own
 * fields at sizeof(dBgActor_c) = 0x320, which is already four-aligned, so
 * mModelAnim lands on 0x320 and every later field follows: the member sizes
 * themselves agree between the two compilers (measured below), so one
 * four-byte correction at the base boundary realigns the entire class and
 * sizeof comes back to 0x3f8.
 *
 * ---- MEASURED IN THE SHIPPED ARTIFACT, NOT INFERRED --------------------
 *
 * The factory and the class disagree in the bytes. Both quoted from
 * build/port/walk_window.exe at fd50368ad (md5 4c0c76c8):
 *
 *   _daObjHatenaBlock_c_classInit_HATENA_BLOCK  00521c00, from the C TU
 *   src/d_a_obj_hatena_block_hatena_block.c, which uses the ROM's literals:
 *     00521c1a  lea ecx, [esi + 0x320]   -> __ZN9ModelAnimC1Ev
 *     00521c2c  lea eax, [esi + 0x384]   -> __ZN11ShadowModelC1Ev
 *
 *   ?InitResources@QuestionBlock@@UAEHXZ  00530240, the C++ TU:
 *     0053026b  lea esi, [edi + 0x324]   -> ModelBase::SetFile
 *     00530325  lea ecx, [edi + 0x388]   -> ShadowModel::InitCuboid
 *
 * So ModelAnim is CONSTRUCTED at 0x320 and USED at 0x324. Word 0 of the used
 * address is still the zero the allocator left, and
 * ?SetFile@ModelBase@@QAEHPAUBMD_File@@HH@Z's first act is
 * `mov eax,[ecx] / jmp dword ptr [eax+8]`, so the boot faults at 004b7986
 * with eax = 0 on the first question block the level loader spawns.
 *
 * The ROM's own code sits on the 0x320 side, which is what makes the C++ TU
 * the outlier rather than the factory: _func_ov102_02149da8 at 0052c620, the
 * state setter matched from ROM bytes, does `mov dword ptr [eax + 0x3e8], edx`
 * where the C++ TUs read mState at 0x3ec. Fixing only the crash would have
 * left the state machine writing into a hole.
 *
 * THE FULL SHIFT, every displacement the ten C++ TUs compute, read out of the
 * shipped bytes with tmp/crash8/qbscan.py. Base members agree, own members are
 * four high:
 *     mModel        0x0d4 = 0x0d4     mMeshCollider 0x124 = 0x124
 *     mClsnMat      0x2ec = 0x2ec     (all inherited from dBgActor_c)
 *     mModelAnim    0x320 -> 0x324    mShadowModel  0x384 -> 0x388
 *     mShadowMat    0x3ac -> 0x3b0    mHomePosY     0x3e0 -> 0x3e4
 *     mState        0x3e8 -> 0x3ec    mStarTracked  0x3f0 -> 0x3f4
 *     mStarId       0x3f1 -> 0x3f5    mHitterParam  0x3f2 -> 0x3f6
 *     mContentType  0x3f3 -> 0x3f7
 *
 * ---- WHY THIS SHAPE ----------------------------------------------------
 *
 * THIS IS A CLASS-SHAPED DEFECT AND THIS IS A ONE-CLASS FIX. port/CMakeLists
 * already records the same mechanism biting dScMgSmartball_c ("it neither
 * reuses the base's tail padding nor skips a vfptr for a base that declares no
 * virtual of its own; measured 0x47d0 against the ROM's 0x47c8"), and any
 * header whose first own field follows a `u8 pad_NNN[k]` with NNN not
 * four-aligned has it. The complete fix belongs in include/QuestionBlock.h and
 * its peers, as
 *
 *     #ifndef _MSC_VER
 *         u8 pad_31e[0x2];
 *     #endif
 *
 * which is byte-neutral on the NDS side because mwccarm does not define
 * _MSC_VER. That is a decomp-side edit, which this lane does not own, so it is
 * written up in the lane report and handed over instead.
 *
 * Everything below is include/QuestionBlock.h's own C++ declaration verbatim,
 * with pad_31e removed and nothing else touched, so the member types, the
 * member order, the class name and every method signature are unchanged and no
 * mangled name moves.
 */
#ifndef PORT_QUESTIONBLOCK_HOSTABI_H
#define PORT_QUESTIONBLOCK_HOSTABI_H

#include "types.h"
#include "dBgW_KcMbg.h"
#include "Model.h"
#include "dBgActor_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"

/* shadows include/QuestionBlock.h */
#define QUESTIONBLOCK_H

struct QuestionBlock : dBgActor_c {
    /* u8 pad_31e[0x2] lives here on the NDS side. See the header comment:
       MSVC has already spent those two bytes as the base's tail padding. */
    ModelAnim mModelAnim;             /* 0x320 */
    ShadowModel mShadowModel;         /* 0x384 */
    Matrix4x3 mShadowMat;             /* 0x3ac */
    u8  pad_3dc[0x4];
    s32 mHomePosY;                    /* 0x3e0 */
    u8  pad_3e4[0x4];
    s32 mState;                       /* 0x3e8 */
    u8  pad_3ec[0x4];
    u8 mStarTracked;                  /* 0x3f0 */
    u8 mStarId;                       /* 0x3f1 */
    u8 mHitterParam;                  /* 0x3f2 */
    u8 mContentType;                  /* 0x3f3 */

    /* --- vtable --- */
    virtual ~QuestionBlock();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    void OnGroundPounded(dActor_c &other);     /* slot 21 */
    int OnAttacked1(dActor_c &other);          /* slot 22 */
    void OnKicked(dActor_c &other);            /* slot 24 */
    void OnHitByMegaChar(Player &player);      /* slot 27 */
    int OnHitFromUnderneath(dActor_c &other);  /* slot 28 */

    u8 pad_3f4[0x4];      /* 0x3f4, to the ROM's 0x3f8 */
};

#endif
