//cpp
/* ov004/dScMgBase_c -- the behavior every minigame scene shares
 * (.text 0x020b04e8..0x020b0a38).
 *
 * Keep definition order: the compiler emits functions in reverse, so the
 * order below is what makes the ROM order.
 *
 * Still raw: unk_0a4, 0a8, 0ac, 0b8, 0c8, 462c and 465c are unnamed in the
 * header, and the func_ov004 and data_ov004 helpers they feed are unnamed
 * in symbols.txt.
 */

#include "dScMgBase_c.h"
#include "decl_common.h"
#include "Sound.h"

/* File-wide on purpose: it applies to every function in this file. It came
   with BeforeBehavior, the only function here whose source had it. */
#pragma opt_strength_reduction off

/* The 0x40 records at data_ov004_020bebe8 are 0x20 bytes each; only the s16
   at 0x1a (nonzero while the record is live) is known, so the type stays a
   local view. */
struct Ent { char pad[0x1a]; s16 f; char pad2[4]; };

/* Local view of the stylus owner at data_0209f5bc, also used by
 * dScMiniGm_c. Only slot 5 is known: it says whether input is live. The
 * owner's real class is not recovered. */
struct SceneVCall6 {
    virtual int v0(); virtual int v1(); virtual int v2();
    virtual int v3(); virtual int v4(); virtual int IsActive();
};

int ApproachLinear(int &value, int target, int step);

extern "C" {
extern void *data_ov004_020beb60;
extern void **data_0209d4a8;
int func_ov004_020b8ee0(char *p);
void func_ov004_020aeb24(char *c);
void func_ov004_020b321c(char *c);
void func_ov004_020adf2c(char *c);
extern SceneVCall6 *data_0209f5bc;
extern unsigned char data_020a0e40;
extern unsigned short data_020a0e5a[];
extern unsigned char data_020a0de8[];
extern int data_0208ee44;
extern void func_ov004_020ad90c(void);
extern void FreeGfxSlotsById(int arg);
extern void func_0203cbc0(void *ptr);
extern void func_02012e1c(void);
void func_ov004_020ae330();
extern void Enable3dEngines(void);
extern char data_0209b308[];
extern char data_0209f61c[];
extern unsigned char data_0209d460[];
extern unsigned char data_0209d458[];
}

// @symbol _ZN11dScMgBase_c19BeforeInitResourcesEv
/* Sets up the scene and both displays. Slots 26, 31 and 33 must stay
 * virtual calls so derived scenes can override them. The block at 0x4000 is
 * padding in the header, so it is still passed by offset. */
bool dScMgBase_c::BeforeInitResources()
{
    if (dScene_c::BeforeInitResources() == 0) return 0;
    if (OnHitByCannonBlastedChar() == 0)
        func_02019028();
    else
        Enable3dEngines();
    unk_0c8 = *(int*)(data_0209b308 + 0x28);
    if (data_ov004_020beb60 == 0)
        data_ov004_020beb60 = _ZN6Memory13operator_new2Ej(0x4000);
    if (data_ov004_020beb68 != 0)
        *(int*)((char*)data_ov004_020beb68 + 0xb0) = 0;
    mHudScore = 0;
    unk_0b8 = 0;
    unk_465c = 0;
    func_ov004_020b8a8c((char *)this + 0x4000);
    Virtual84();
    func_ov004_020b2cb8();
    dScene_c::SetFaders((FaderBrightness *)data_0209f61c);
    func_0202ec9c(data_0209f61c, 0);
    data_0209d460[0] = 0;
    data_0209d458[0] = 0;
    Virtual7C();
    return 1;
}

// @symbol _ZN11dScMgBase_c18AfterInitResourcesEj
/* Finishes the display and font setup, then reports to dScene_c. */
void dScMgBase_c::AfterInitResources(u32 vfSuccess)
{
    Virtual80();
    LoadFont(2);
    func_ov004_020ae330();
    dScene_c::AfterInitResources(vfSuccess);
}

// @symbol _ZN11dScMgBase_c21AfterCleanupResourcesEj
void dScMgBase_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2) {
        if (((int *)data_0209b308)[4] == 0)
            func_ov004_020ad90c();
        data_0209d4a8 = 0;
        data_ov004_020beb74[1] = 0;
        FreeGfxSlotsById(0x1d);
        if (data_ov004_020beb60 != 0) {
            func_0203cbc0(data_ov004_020beb60);
            data_ov004_020beb60 = 0;
        }
        if (mMenuOpen != 0) {
            func_02012e1c();
            Sound::StopLoadedMusic_Layer1(1);
        }
        func_ov004_020b2c84();
    }
    dScene_c::AfterCleanupResources(vfSuccess);
}

// @symbol _ZN11dScMgBase_c14BeforeBehaviorEv
/* Handles input and the menu, runs the UI state controller and the shared
 * records, approaches unk_0ac toward unk_0a8, and advances the 40-frame
 * counter. Returns 0 to stop the derived scene's behavior this frame. */
int dScMgBase_c::BeforeBehavior()
{
    int mode;
    unsigned short flags;

    if (dScene_c::BeforeBehavior() == 0)
        return 0;

    if (data_0209f5bc->IsActive()) {
        mode = data_020a0e40;
        flags = data_020a0e5a[mode * 2];
        if ((flags & 8) || (flags & 4) || (flags & 1) || (flags & 2)) {
            if (mMenuOpen != 0) {
                if (mMenuCursor < 0) {
                    if (func_ov004_020b8ee0((char *)&mTouchOptions))
                        OnHitFromUnderneath();
                }
            } else {
                if (data_020a0de8[mode * 4] == 0)
                    OnHitByMegaChar();
            }
        }
    }

    if (mMenuOpen != 0) {
        func_ov004_020aeb24((char *)this);
        return 0;
    }
    if (unk_462c != 0)
        return 0;

    if (mStateController.unk_024 == 0) {
        int i;
        char *g;
        mStateController.Behavior();
        g = data_ov004_020bf648;
        for (i = 0; i < 3; i++) {
            if (*(int *)(g + 0x20) != 0x1d)
                func_ov004_020b321c(g);
            g += 0x134;
        }
    }

    {
        int j;
        char *p = data_ov004_020bebe8;
        char *base = data_ov004_020bebe8;
        for (j = 0; j < 0x40; j++) {
            if (((struct Ent *)base)[j].f != 0)
                func_ov004_020adf2c(p);
            p += 0x20;
        }
    }

    if (unk_0a4 == 0) {
        if (data_0209f5bc->IsActive() == 0)
            return 0;
    }

    ApproachLinear(unk_0ac, unk_0a8, data_0208ee44);

    mFrameCounter++;
    if (mFrameCounter >= 0x28)
        mFrameCounter = 0;

    return 1;
}

// @symbol _ZN11dScMgBase_c8BehaviorEv
s32 dScMgBase_c::Behavior()
{
    return 1;
}

// @symbol _ZN11dScMgBase_c12BeforeRenderEv
/* Draws the menu instead of the scene while it is open. The two loops draw
 * the three UI records in two passes, those with 0x30 clear first. */
int dScMgBase_c::BeforeRender()
{
    int i; char *p; int i2; char *p2; int j;

    if (dScene_c::BeforeRender() == 0)
        return 0;

    if (mMenuOpen != 0) {
        func_ov004_020ae858(this);
        return 0;
    }

    if (mStateController.unk_024 == 0) {
        p = data_ov004_020bf648;
        for (i = 0; i < 3; i++, p += 0x134) {
            if (*(int *)(p + 0x20) == 0x1d) continue;
            if (*(s16 *)(p + 0x30) != 0) continue;
            func_ov004_020b31b4(p);
        }
        p2 = data_ov004_020bf648;
        for (i2 = 0; i2 < 3; i2++, p2 += 0x134) {
            if (*(int *)(p2 + 0x20) == 0x1d) continue;
            if (*(s16 *)(p2 + 0x30) == 0) continue;
            func_ov004_020b31b4(p2);
        }
        mStateController.Render();
    }

    p = data_ov004_020bebe8;
    for (j = 0; j < 0x40; j++, p += 0x20) {
        if (((struct Ent *)data_ov004_020bebe8)[j].f != 0)
            func_ov004_020add88(p);
    }

    func_ov004_020b0de0(this);
    return 1;
}

// @symbol _ZN11dScMgBase_c6RenderEv
s32 dScMgBase_c::Render()
{
    return 1;
}

// @symbol _ZN11dScMgBase_c16OnPendingDestroyEv
void dScMgBase_c::OnPendingDestroy()
{
}
