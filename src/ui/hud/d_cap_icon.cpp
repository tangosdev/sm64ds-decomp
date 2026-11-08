//cpp
/* dCapIcon_c -- the cap-icon list node and its per-character lists, ov001.
 * .text 0x020ab110..0x020ab41c.
 *
 * RTTI names dCapIcon_c. This TU owns Unlink, Link, the list reset, the
 * constructor, and the destructor pair.
 *
 * `#pragma defer_codegen` is positional and load-bearing. Link re-issues
 * the list-head load across the store that follows it; CSE on folds that. Every other member needs CSE on. Deferred codegen reads
 * optimisation pragmas at end of file, last-wins, so the parse-time half
 * (Unlink, Link) is generated with the opt_common_subs bracket bound to
 * Link alone, then defer_codegen on restores default for the
 * deferred half (ctor, dtor, reset) in reverse source order. Do not
 * flatten the split.
 *
 * deslop
 * Leftover: func_ov001_020ab2e4 stays the linker name. The image preserves
 *   no original mangled table; an existing label is not a barrier to a
 *   member -- a migration renames source and config together.
 * Leftover: CapIconMgr_SetMapIconShown and func_ov001_020aa6cc are
 *   CapIconMgr's; this TU declares them locally.
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

extern void CapIconMgr_SetMapIconShown(int *icon, int value);
extern void func_ov001_020aa6cc(int character);

}

void dCapIcon_c::Unlink()
{
    if (((u32)mFlags << 29) >> 31)
        return;

    if (mSlot != -1) {
        if ((dActor_c *)data_0209f3e8[mSlot] == mOwner) {
            CapIconMgr_SetMapIconShown((int *)this, 0);
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

    CapIconMgr_SetMapIconShown((int *)this, 0);
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

/* Name coined from the body and its six callers (daDossy_c, daObjMarioCap_c,
 * dCapEnemy_c): the inverse of Unlink. It records the owner and its unique
 * ID, pushes this node onto the head of character's list, bumps the list's
 * count, keeps iconKind (kind 3 raises request flag 8) and the bank bit in
 * mFlags bit 0, and clears the list's dispatch state for a bank or kind-3
 * cap. */
void dCapIcon_c::Link(char *owner, int character, int iconKind, unsigned char bank) {
    mCharacter = (unsigned char)character;
    mOwner = (dActor_c *)owner;
    mOwnerUniqueID = *(u32 *)(owner + 4);
    mIconKind = (unsigned char)iconKind;
    mNext = gCapIconListHeads[character];
    if (gCapIconListHeads[character])
        gCapIconListHeads[character]->mPrev = this;
    gCapIconListHeads[character] = this;
    gCapIconCounts[character]++;
    if (iconKind == 3)
        gCapIconRequestFlags[character] |= 8;
    mFlashFrames = 0;
    mFlags = 0;
    {
        unsigned char *p1b = (unsigned char *)&mFlags;
        *p1b = (*p1b & ~1) | (bank & 1);
    }
    if (bank != 0 || iconKind == 3)
        func_ov001_020aa6cc(character);
    mSlot = -1;
}
#pragma defer_codegen on

/* Deferred half, descending ROM order: ctor, dtor pair, then reset. The
 * absorbed helper below is defined first so reverse emission puts it
 * highest (0x020ab3f0). */

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

dCapIcon_c::dCapIcon_c()
{
    mPrev = 0;
    mNext = 0;
    mFlags |= 4;
}

dCapIcon_c::~dCapIcon_c()
{
    Unlink();
}

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
