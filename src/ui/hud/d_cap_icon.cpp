//cpp
/* dCapIcon_c -- the cap-icon list node and its per-character lists, ov001.
 * .text 0x020ab110..0x020ab41c.
 *
 * RTTI names dCapIcon_c. This TU owns Unlink, the link helper, the list
 * reset, the constructor, and the destructor pair.
 *
 * `#pragma defer_codegen` is positional and load-bearing. The link helper
 * re-issues the list-head load across the store that follows it; CSE on
 * folds that. Every other member needs CSE on. Deferred codegen reads
 * optimisation pragmas at end of file, last-wins, so the parse-time half
 * (Unlink, link) is generated with the opt_common_subs bracket bound to
 * the link helper alone, then defer_codegen on restores default for the
 * deferred half (ctor, dtor, reset) in reverse source order. Do not
 * flatten the split.
 *
 * deslop
 * Leftover: func_ov001_020ab2e4 stays the linker name. The image preserves
 *   no original mangled table; an existing label is not a barrier to a
 *   member -- a migration renames source and config together.
 *   func_ov001_020ab228 is a dCapIcon_c method; the address is the method
 *   name. Callers are outside this TU.
 * Leftover: func_ov001_020aa6b0 / func_ov001_020aa6cc are other TUs.
 * Leftover: owner uniqueID is fBase_c::uniqueID at +4; this TU keeps
 *   the +4 load.
 * Shared list state is declared in CapIconMgr.h and defined by the manager.
 * Leftover: data_0209f3e8 remains an external slot-table linker name.
 */
#pragma defer_codegen off

#include "CapIconMgr.h"
#include "SaveData.h"
#include "dThIcon_c.h"

extern "C" {

extern int data_0209f3e8[];



extern void func_ov001_020aa6b0(int *icon, int value);
extern void func_ov001_020aa6cc(int character);

}

// @symbol _ZN10dCapIcon_c6UnlinkEv
void dCapIcon_c::Unlink()
{
    if (((u32)mFlags << 29) >> 31)
        return;

    if (mSlot != -1) {
        if ((dActor_c *)data_0209f3e8[mSlot] == mOwner) {
            func_ov001_020aa6b0((int *)this, 0);
            data_0209f3e8[mSlot] = 0;
            func_ov001_020aa6cc(mCharacter);
        }
    }

    if (mPrev)
        mPrev->mNext = mNext;
    else if (gCapIconListHeads[mCharacter] == this)
        gCapIconListHeads[mCharacter] = mNext;

    if (mNext)
        mNext->mPrev = mPrev;

    func_ov001_020aa6b0((int *)this, 0);
    mOwner = 0;
    mOwnerUniqueID = 0;
    mPrev = 0;
    mNext = 0;
    mSlot = -1;
    mFlags = 0;
    mFlags |= 4;
    mCharacter = 3;
    mIconKind = 0;

    if (gCapIconCounts[mCharacter])
        --gCapIconCounts[mCharacter];
}

// @symbol _ZN10dCapIcon_c19func_ov001_020ab228EPciih
/* CSE off is load-bearing: the cartridge re-issues the list-head load
 * across the store into mNext. The bracket binds because codegen is
 * not deferred here. */
#pragma opt_common_subs off
void dCapIcon_c::func_ov001_020ab228(char *a1, int idx, int a3,
                                    unsigned char a5)
{
    mCharacter = (unsigned char)idx;
    mOwner = (dActor_c *)a1;
    mOwnerUniqueID = *(u32 *)(a1 + 4);
    mIconKind = (unsigned char)a3;
    mNext = gCapIconListHeads[idx];
    if (gCapIconListHeads[idx])
        gCapIconListHeads[idx]->mPrev = this;
    gCapIconListHeads[idx] = this;
    gCapIconCounts[idx]++;
    if (a3 == 3)
        gCapIconRequestFlags[idx] |= 8;
    mFlashFrames = 0;
    mFlags = 0;
    {
        unsigned char *p1b = (unsigned char *)(int)&mFlags;
        *p1b = (*p1b & ~1) | (a5 & 1);
    }
    if (a5 != 0 || a3 == 3)
        func_ov001_020aa6cc(idx);
    mSlot = -1;
}
#pragma opt_common_subs on
#pragma defer_codegen on

/* Deferred half, descending ROM order: ctor, dtor pair, then reset. The
 * absorbed helper below is defined first so reverse emission puts it
 * highest (0x020ab3f0). */

// @symbol func_ov001_020ab3f0
extern "C" void func_ov001_020ab3f0(void *r0) {
    /* The dThIcon_c press pulse (callers pass &mIcons[i]). */
    dThIcon_c *icon = (dThIcon_c *)r0;
    if (icon->mOn != 0) {
        icon->mOn = 0;
    } else {
        icon->mOn = 1;
    }
    icon->mReady = 0;
    icon->mPressTimer = icon->mPressTime;
}

// @symbol _ZN10dCapIcon_cC1Ev
dCapIcon_c::dCapIcon_c()
{
    mPrev = 0;
    mNext = 0;
    mFlags |= 4;
}

// @symbol _ZN10dCapIcon_cD1Ev
// @symbol _ZN10dCapIcon_cD0Ev
dCapIcon_c::~dCapIcon_c()
{
    Unlink();
}

// @symbol func_ov001_020ab2e4
extern "C" void func_ov001_020ab2e4(void)
{
    int i;
    for (i = 0; i < 3; i++) {
        gCapIconListHeads[i] = 0;
        gCapIconDispatchFlags[i] = 0;
        gCapIconRequestFlags[i] = 0;
        if (SaveData::IsCharacterUnlocked(i))
            gCapIconRequestFlags[i] |= 0x10;
        gCapIconCounts[i] = 0;
    }
    gCapIconFlashTimer = 0;
    gCapIconDispatchLocked = 0;
}
