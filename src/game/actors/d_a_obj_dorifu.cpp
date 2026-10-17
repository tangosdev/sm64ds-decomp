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
 * Behavior is the key function (first non-pure, non-inline virtual), so
 * this TU owns the RTTI/type name/vtable as compiler-only deadstrip output.
 *
 * The cartridge orders this run D0 (0x020b4a70) then D1 (0x020b4af8), but
 * the header-inline destructor always emits D1-then-D0 under deferred
 * codegen (every other lever measured and refuted, and moving it out of
 * line breaks the three descendants' inlined vptr stores). So this file
 * uses `#pragma defer_codegen off` with two parse-time forcing calls --
 * `delete p` pulls D0 first, `p->~daObjDorifu_c()` pulls D1 second -- and
 * reads low-address-first. Do not reorder.
 */

// leftovers: dBgW_KcMbg::SetFile stays ABI-exact (by-value Fix12<int> scale;
// the member spelling grows InitResources by eight bytes). InitResources'
// two matrix copies stay Matrix4x3Copy block copies (C++ struct assignment
// scalarizes the ROM's ldm/stm sequence). DecIfAbove0_Byte has no shared
// header.

#pragma defer_codegen off

#include "daObjDorifu_c.h"
#include "Event.h"

typedef struct { s32 words[12]; } Matrix4x3Copy;

/* Behavior's mState values. What each does is read off the code: 0 and 2 wait
   on Event::GetBit(mEventBit) going set and clear, 1 and 3 step mActivePlank
   down to 0 and back up to 4, one plank each time mStepTimer runs out. */
enum {
    STATE_AWAIT_EVENT_SET = 0,
    STATE_STEP_DOWN = 1,
    STATE_AWAIT_EVENT_CLEAR = 2,
    STATE_STEP_UP = 3
};

extern "C" {
unsigned char DecIfAbove0_Byte(unsigned char *p);
void Matrix4x3_FromRotationY(void *mat, s32 angle);

/* A genuine SetFile call with its by-value Fix12<int> parameter triggers the
 * known mwccarm by-value reload wall and grows InitResources by eight bytes.
 * Keep the ABI-exact declaration until that wall has a source-form solution. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mat, s32 scale, s16 angle, void *clps);
}

// @symbol _ZN13daObjDorifu_cD0Ev
// @symbol _ZN13daObjDorifu_cD1Ev
/* No source here: both destructor variants come from the ONE inline body in
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

// @symbol _ZN13daObjDorifu_c16CleanupResourcesEP20daObjDorifuResources
/* ov002 0x020b4b6c. Shared cleanup for the three concrete drifting-platform
 * descendants. Their slot-3 overrides pass their own five-entry resource table here. */
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

// @symbol _ZN13daObjDorifu_c8BehaviorEv
/* daObjDorifu_c::Behavior -- vtable slot 6, ov002 0x020b4bfc, and this class's
 * key function: the first non-pure, non-inline virtual the header declares, so
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
 * The `(int)` launder on &mActivePlank is load-bearing: without it mwcc
 * common-subexpressions the field address across the DecIfAbove0_Byte call and
 * emits one instruction fewer than the ROM has. */
s32 daObjDorifu_c::Behavior()
{
    int i;
    dBgW_KcMbg *clsn;
    unsigned char *plankPtr;

    switch (mState) {
    case STATE_AWAIT_EVENT_SET:
        if (Event::GetBit(mEventBit))
            mState = STATE_STEP_DOWN;
        break;
    case STATE_STEP_DOWN:
        if (DecIfAbove0_Byte(&mStepTimer) == 0) {
            plankPtr = (unsigned char *)(int)&mActivePlank;
            *plankPtr = *plankPtr - 1;
            mStepTimer = 2;
        }
        if (mActivePlank == 0) {
            mActivePlank = 0;
            mState = STATE_AWAIT_EVENT_CLEAR;
        }
        break;
    case STATE_AWAIT_EVENT_CLEAR:
        if (Event::GetBit(mEventBit) == 0)
            mState = STATE_STEP_UP;
        break;
    case STATE_STEP_UP:
        if (DecIfAbove0_Byte(&mStepTimer) == 0) {
            plankPtr = (unsigned char *)(int)&mActivePlank;
            *plankPtr = *plankPtr + 1;
            mStepTimer = 2;
        }
        if (mActivePlank >= 4) {
            mActivePlank = 4;
            mState = STATE_AWAIT_EVENT_SET;
        }
        break;
    }

    clsn = mPlankClsn;
    for (i = 0; i < 5; i++) {
        if (i == mActivePlank) {
            if (!clsn->IsEnabled())
                clsn->Enable(this);
        } else {
            if (clsn->IsEnabled())
                clsn->Disable();
        }
        clsn++;
    }
    return 1;
}

// @symbol _ZN13daObjDorifu_c13InitResourcesEP20daObjDorifuResources
/* ov002 0x020b4d58. Shared initialization for the three concrete
 * drifting-platform descendants. Each passes its own five-entry daObjDorifuResources table. The raw matrix
 * copies are load-bearing: ordinary C++ struct assignment scalarizes them
 * instead of emitting the ROM's ldm/stm sequence. */
s32 daObjDorifu_c::InitResources(daObjDorifuResources *resources)
{
    s32 i;
    daObjDorifuResources *resource;
    Model *model;
    s32 j;
    Matrix4x3 *sourceMat;
    s32 scale = 0x1000;
    Model *modelDst;
    dBgW_KcMbg *clsn;
    daObjDorifuResources *clsnResource = resources;

    resource = clsnResource;
    model = mPlankModels;
    for (i = 0; i < 5; ++i) {
        model->SetFile((BMD_File *)Model::LoadFile(*resource->modelFile), 1, -1);
        resource++;
        model++;
    }

    mActivePlank = 4;
    mStepTimer = 2;
    mEventBit = param1 & 0xf;
    Matrix4x3_FromRotationY(&mPlankModels[0].mat4x3, mAngleY);
    mPlankModels[0].mat4x3.m[9] = mPosX >> 3;
    mPlankModels[0].mat4x3.m[10] = mPosY >> 3;
    mPlankModels[0].mat4x3.m[11] = mPosZ >> 3;
    *(Matrix4x3Copy *)&mPlatformMat = *(Matrix4x3Copy *)&mPlankModels[0].mat4x3;
    mPlatformMat.m[9] = mPosX;
    mPlatformMat.m[10] = mPosY;
    mPlatformMat.m[11] = mPosZ;

    j = 0;
    sourceMat = &mPlankModels[0].mat4x3;
    modelDst = mPlankModels;
    clsn = mPlankClsn;
    for (; j < 5; ++j) {
        *(Matrix4x3Copy *)&modelDst->mat4x3 = *(Matrix4x3Copy *)sourceMat;
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            clsn, dBgW_Kc::LoadFile(*clsnResource->clsnFile), &mPlatformMat, scale, mAngleY,
            clsnResource->clps);
        if (j == mActivePlank)
            clsn->Enable(this);
        modelDst++;
        clsnResource++;
        clsn++;
    }
    return 1;
}

