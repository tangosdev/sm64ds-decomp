//cpp
/* Shared minigame scene behavior. Production TU ov004/dScMgBase_c;
 * the manifest owns eight functions in [0x020b04e8, 0x020b0a38).
 * Keep definition order: the pinned compiler reverses ordinary function
 * sections. Strength reduction remains disabled for BeforeBehavior.
 *
 * deslop
 * Leftover: the func_ov004_* helpers and data_ov004_* homes come from
 *   decl_common.h, not here; naming belongs at their definitions.
 * Leftover: unk_0a4/0a8/0ac/0b8/0c8/462c/465c are unrecovered scene
 *   state (header); the Ent and SceneVCall6 local views stand in for
 *   unrecovered types and do not establish bases.
 */

#include "dScMgBase_c.h"
#include "decl_common.h"
#include "Sound.h"

/* FILE-GLOBAL, and deliberately so: the last state set anywhere applies to
   every function in the TU. Carried over from
   src/_ZN11dScMgBase_c14BeforeBehaviorEv.cpp, the only member of the real
   0x020b04e8..0x020b2adc run that had one. */
#pragma opt_strength_reduction off

/* shadow struct 'Ent' */
struct Ent { char pad[0x1a]; s16 f; char pad2[4]; };

/* Local view of the unresolved stylus owner, also used by dScMiniGm_c.
 * Slot 5 gates input handling; this does not establish a dScene_c base. */
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
extern void func_0203cbc0(void);
extern void func_02012e1c(void);
void func_ov004_020ae330();
extern void Enable3dEngines(void);
extern char data_0209b308[];
extern char data_0209f61c[];
extern unsigned char data_0209d460[];
extern unsigned char data_0209d458[];
}

// @symbol _ZN11dScMgBase_c19BeforeInitResourcesEv
/* Initialize the scene and displays through the existing virtual interface.
 * Slots 26, 31 and 33 must remain virtual calls so derived scenes can override
 * them; the actor-style slot-26 name is inherited, not recovered scene meaning. */
bool dScMgBase_c::BeforeInitResources()
{
    char *c = (char *)this;
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
    func_ov004_020b8a8c(c + 0x4000);
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
/* Complete the display/font setup, then report initialization to dScene_c. */
void dScMgBase_c::AfterInitResources(u32 vfSuccess)
{
    Virtual80();
    LoadFont(2);
    func_ov004_020ae330();
    dScene_c::AfterInitResources(vfSuccess);
}

// @symbol _ZN11dScMgBase_c21AfterCleanupResourcesEj
/* dScMgBase_c::AfterCleanupResources - recovered from vtable slot identity. */
void dScMgBase_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2) {
        if (((int *)data_0209b308)[4] == 0)
            func_ov004_020ad90c();
        data_0209d4a8 = 0;
        data_ov004_020beb74[1] = 0;
        FreeGfxSlotsById(0x1d);
        if (data_ov004_020beb60 != 0) {
            func_0203cbc0();
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
/* Gate scene behavior on input/menu state, update the shared UI records,
 * approach the tracked value, and advance the 40-frame animation counter.
 * The stylus owner's slot-5 predicate uses the local view above; its complete
 * class and inheritance remain unresolved. */
int dScMgBase_c::BeforeBehavior()
{
    char *self = (char *)this;
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
        func_ov004_020aeb24(self);
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
            if (*(short *)(base + j * 0x20 + 0x1a) != 0)
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
/* dScMgBase_c::Behavior - recovered from vtable slot identity. */
s32 dScMgBase_c::Behavior()
{
    return 1;
}

// @symbol _ZN11dScMgBase_c12BeforeRenderEv
/* dScMgBase_c::BeforeRender - recovered from vtable slot identity. */
int dScMgBase_c::BeforeRender()
{
    char *c = (char *)this;
    int i; char *p; int i2; char *p2; int j;

    if (dScene_c::BeforeRender() == 0)
        return 0;

    if (mMenuOpen != 0) {
        func_ov004_020ae858(c);
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

    func_ov004_020b0de0(c);
    return 1;
}

// @symbol _ZN11dScMgBase_c6RenderEv
/* dScMgBase_c::Render - recovered from vtable slot identity. */
s32 dScMgBase_c::Render()
{
    return 1;
}

// @symbol _ZN11dScMgBase_c16OnPendingDestroyEv
/* dScMgBase_c::OnPendingDestroy - recovered from vtable slot identity. */
void dScMgBase_c::OnPendingDestroy()
{
}
