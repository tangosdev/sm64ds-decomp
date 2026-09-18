//cpp
/* Genuine production translation unit for ov002/daObjDorifu_c.
 *
 * The abstract base of the rickshaw drifting-platform family: five planks,
 * one of them solid at a time. `dorifu` is the katakana clipping of "drift".
 * include/daObjDorifu_c.h carries the layout evidence; this file carries the
 * six functions the cartridge places at ov002 0x020b4a70..0x020b4ed8.
 *
 * ROM class identity: _ZTI13daObjDorifu_c at ov002 0x02108d70 and
 * _ZTS13daObjDorifu_c at ov002 0x02108d7c ("13daObjDorifu_c"). The vtable
 * storage begins at 0x02108d8c and its public address point is
 * _ZTV13daObjDorifu_c at 0x02108d94, 32 slots, running to 0x02108e14.
 *
 * ABSTRACT. Vtable slots 0 and 3 are bare zero words with no relocation --
 * InitResources() and CleanupResources() are pure. The three concrete
 * descendants each supply the no-argument override and forward to the
 * resource-table overloads defined here; those descendants live in ov036,
 * ov043 and ov047, outside this module, so nothing of theirs is claimed.
 *
 * WHY `#pragma defer_codegen off` AND TWO FORCING FUNCTIONS
 *
 * The cartridge orders this run D0 (0x020b4a70) then D1 (0x020b4af8), and a
 * promoted object has to emit its licensed functions in ROM-ascending order
 * or the linker lays the range out wrong. Under mwccarm's default deferred
 * codegen the destructor variant group of a header-inline destructor always
 * comes out D1-then-D0, whatever the source does: a `delete` forcer, a
 * forcer pair in either order, moving the destructor declaration to the end
 * of the class, an out-of-class `inline` definition and leaving the
 * destructor implicit were each measured and each emitted D1 first.
 *
 * The one lever that does flip the pair is `#pragma defer_codegen off`, which
 * emits ordinary functions at PARSE time in SOURCE order. The two forcing
 * calls below are therefore parse-time triggers: the delete-expression pulls
 * D0 out of line first, the explicit destructor call pulls D1 out second, and
 * the four real members then follow in ascending ROM order. That is the whole
 * reason this file reads low-address-first while every deferred TU in the
 * tree reads high-address-first.
 *
 * Moving the destructor out of line also produces the ROM's order, and is
 * REFUTED: with the body removed from the class the three descendants stop
 * inlining the vptr store, and all six of their destructors change size. The
 * header's inline body is load-bearing for them, so it stays and this file
 * pays with the pragma instead.
 *
 * Behavior is this class's key function -- the first non-pure, non-inline
 * virtual the header declares -- so this TU also owns the class RTTI, type
 * name and vtable; all of that is compiler-only output with a ROM home
 * elsewhere and is licensed to deadstrip in the manifest.
 *
 * deslop leftovers:
 * - Event::GetBit is still reached through its mangled spelling; there is no
 *   Event header this TU can take without a shared-header campaign.
 * - dBgW_KcMbg::SetFile takes its scale as Fix12<int> BY VALUE. The natural
 *   member spelling trips the known by-value Fix12 reload wall and grows
 *   InitResources by eight bytes, so the ABI-exact declaration stays.
 * - InitResources still addresses the Model[5] and dBgW_KcMbg[5] arrays and
 *   the dBgActor_c transform words by raw offset: the ROM's ldm/stm matrix
 *   copies do not survive ordinary C++ struct assignment, which scalarizes
 *   them, and the 0x33c/0x360/0xdbc words belong to dBgActor_c rather than to
 *   this leaf.
 * - DecIfAbove0_Byte has no shared header.
 */

#pragma defer_codegen off

#include "daObjDorifu_c.h"

typedef struct { s32 words[12]; } Matrix4x3Copy;

extern "C" {
int  _ZN5Event6GetBitEj(unsigned int bit);
unsigned char DecIfAbove0_Byte(unsigned char *p);
void Matrix4x3_FromRotationY(void *mat, s32 angle);

/* A genuine SetFile call with its by-value Fix12<int> parameter triggers the
 * known mwccarm by-value reload wall and grows InitResources by eight bytes.
 * Keep the ABI-exact declaration until that wall has a source-form solution. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mat, s32 scale, s16 angle, void *clps);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN13daObjDorifu_cD0Ev, 0x020b4a70, size 0x88              */
/* ROM ordinal 1 -- _ZN13daObjDorifu_cD1Ev, 0x020b4af8, size 0x74              */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjDorifu_cD0Ev
// @symbol _ZN13daObjDorifu_cD1Ev
/* NO SOURCE HERE. Both destructor variants come from the ONE inline body in
 * include/daObjDorifu_c.h -- the class's descendants inline its vptr store,
 * which the compiler can only do from a visible body, so the definition
 * cannot move into this file.
 *
 * The two calls below are never executed and exist only to make the compiler
 * emit the out-of-line copies at parse time, in the cartridge's order. The
 * delete-expression is required for D0: a plain `p->~daObjDorifu_c()` asks
 * only for the complete-object half, because dBgActor_c declares Kill, a key
 * function reachable from this class. The explicit destructor call then asks
 * for D1 by name. Both spellings are carried over verbatim from the
 * per-function sources this file replaces. */

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void daObjDorifu_c_EmitDeletingDestructor(daObjDorifu_c *p)
{
    delete p;
}

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjDorifu_c_EmitDestructor(daObjDorifu_c *p)
{
    p->~daObjDorifu_c();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- 0x020b4b6c, size 0x58                                      */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjDorifu_c16CleanupResourcesEP20daObjDorifuResources
/* Shared cleanup for the three concrete drifting-platform descendants. Their
 * slot-3 overrides pass their own five-entry resource table here. */
s32 daObjDorifu_c::CleanupResources(daObjDorifuResources *resources)
{
    for (s32 i = 0; i < 5; ++i) {
        if (mPlankClsn[i].IsEnabled())
            mPlankClsn[i].Disable();
        resources[i].modelFile->Release();
        resources[i].clsnFile->Release();
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- 0x020b4bc4, size 0x38                                      */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjDorifu_c6RenderEv
/* daObjDorifu_c::Render -- vtable slot 9, ov002 0x020b4bc4.
 * reloc: _ZTV13daObjDorifu_c+0x24 -> 0x020b4bc4, where
 * _ZTV10dBgActor_c+0x24 -> 0x02043af0, so this is a real override.
 *
 * Indexes the Model[5] at 0x320 with the byte at 0xdc8 and calls the element's
 * vtable slot 5 with a literal 0 -- Model::Render(const Vector3 *), NULL. */
s32 daObjDorifu_c::Render()
{
    mPlankModels[mActivePlank].Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- 0x020b4bfc, size 0x15c                                     */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjDorifu_c8BehaviorEv
/* daObjDorifu_c::Behavior -- vtable slot 6, ov002 0x020b4bfc, and this class's
 * KEY FUNCTION: the first non-pure, non-inline virtual the header declares, so
 * this definition is what anchors _ZTV13daObjDorifu_c / _ZTI13daObjDorifu_c
 * into this translation unit.
 *
 * The state machine include/daObjDorifu_c.h's banner describes, and the
 * function the four bytes at 0xdc8 were named from. Slot 6 of
 * _ZTV13daObjDorifu_c holds 0x020b4bfc where _ZTV10dBgActor_c holds something
 * else, so it is this class's own override of the virtual fBase_c declares.
 *
 * The three collider calls are spelled as methods: dBgW_KcMbg inherits
 * IsEnabled, Enable and Disable from dBgW, which declares them. A bare
 * `extern int _ZN4dBgW9IsEnabledEv(void *)` was emitted verbatim from C but
 * would be mangled a SECOND time from C++, which this file is.
 *
 * THE `(int)` LAUNDER ON &mActivePlank IS LOAD-BEARING: without it mwcc
 * common-subexpressions the field address across the DecIfAbove0_Byte call and
 * emits one instruction fewer than the ROM has. */
s32 daObjDorifu_c::Behavior()
{
    int i;
    dBgW_KcMbg *m;
    unsigned char *p;

    switch (mState) {
    case 0:
        if (_ZN5Event6GetBitEj(mEventBit))
            mState = 1;
        break;
    case 1:
        if (DecIfAbove0_Byte(&mStepTimer) == 0) {
            p = (unsigned char *)(int)&mActivePlank;
            *p = *p - 1;
            mStepTimer = 2;
        }
        if (mActivePlank == 0) {
            mActivePlank = 0;
            mState = 2;
        }
        break;
    case 2:
        if (_ZN5Event6GetBitEj(mEventBit) == 0)
            mState = 3;
        break;
    case 3:
        if (DecIfAbove0_Byte(&mStepTimer) == 0) {
            p = (unsigned char *)(int)&mActivePlank;
            *p = *p + 1;
            mStepTimer = 2;
        }
        if (mActivePlank >= 4) {
            mActivePlank = 4;
            mState = 0;
        }
        break;
    }

    m = mPlankClsn;
    for (i = 0; i < 5; i++) {
        if (i == mActivePlank) {
            if (!m->IsEnabled())
                m->Enable(this);
        } else {
            if (m->IsEnabled())
                m->Disable();
        }
        m++;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- 0x020b4d58, size 0x180                                     */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjDorifu_c13InitResourcesEP20daObjDorifuResources
/* Shared initialization for the three concrete drifting-platform descendants.
 * Each passes its own five-entry daObjDorifuResources table. The raw matrix
 * copies are load-bearing: ordinary C++ struct assignment scalarizes them
 * instead of emitting the ROM's ldm/stm sequence. */
s32 daObjDorifu_c::InitResources(daObjDorifuResources *resources)
{
    s32 i;
    char *resourcePos;
    char *modelPos;
    s32 j;
    char *sourceMat;
    s32 scale = 0x1000;
    char *modelDst;
    char *clsnPos;
    char *self = (char *)this;
    char *resourceBytes = (char *)resources;

    resourcePos = resourceBytes;
    modelPos = self + 0x320;
    for (i = 0; i < 5; ++i) {
        ((Model *)modelPos)->SetFile(
            (BMD_File *)Model::LoadFile(**(SharedFilePtr **)resourcePos), 1, -1);
        resourcePos += 0xc;
        modelPos += 0x50;
    }

    mActivePlank = 4;
    mStepTimer = 2;
    mEventBit = param1 & 0xf;
    Matrix4x3_FromRotationY(self + 0x33c, mAngleY);
    *(s32 *)(self + 0x360) = mPosX >> 3;
    *(s32 *)(self + 0x364) = mPosY >> 3;
    *(s32 *)(self + 0x368) = mPosZ >> 3;
    *(Matrix4x3Copy *)&mPlatformMat = *(Matrix4x3Copy *)(self + 0x33c);
    *(s32 *)(self + 0xdbc) = mPosX;
    *(s32 *)(self + 0xdc0) = mPosY;
    *(s32 *)(self + 0xdc4) = mPosZ;

    j = 0;
    sourceMat = self + 0x33c;
    modelDst = self + 0x320;
    clsnPos = self + 0x4b0;
    for (; j < 5; ++j) {
        *(Matrix4x3Copy *)(modelDst + 0x1c) = *(Matrix4x3Copy *)sourceMat;
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            clsnPos,
            dBgW_Kc::LoadFile(**(SharedFilePtr **)(resourceBytes + 4)),
            &mPlatformMat, scale, mAngleY,
            *(void **)(resourceBytes + 8));
        if (j == mActivePlank)
            ((dBgW_KcMbg *)clsnPos)->Enable(this);
        modelDst += 0x50;
        resourceBytes += 0xc;
        clsnPos += 0x1c8;
    }
    return 1;
}

