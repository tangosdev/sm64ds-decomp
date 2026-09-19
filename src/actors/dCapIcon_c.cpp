//cpp
/* dCapIcon_c -- the cap-icon list node and its per-character lists, ov001.
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
 * Leftover: func_ov001_020ab228 / func_ov001_020ab2e4 stay the linker
 *   names. The image preserves no original mangled table; an existing
 *   label is not a barrier to a member -- a migration renames source and
 *   config together. Callers are outside this TU. The link helper keeps
 *   a char* first parameter so those declarations stay ABI-identical.
 * Leftover: func_ov001_020aa6b0 / func_ov001_020aa6cc are other TUs.
 * Leftover: owner uniqueID is fBase_c::uniqueID at +4; this TU keeps
 *   the +4 load.
 * Leftover: unk_19 / unk_1a. The link helper stores its 4th argument
 *   into unk_19 and special-cases it against 3; Unlink zeros unk_19.
 *   Original names are not in the cartridge.
 * Leftover: data_0209f3e8 / data_ov001_020ad* stay the linker names
 *   (S14 data outside this .text).
 */
#pragma defer_codegen off

#include "CapIcon.h"
#include "SaveData.h"

extern "C" {

extern int data_0209f3e8[];

extern dCapIcon_c *data_ov001_020ad634[];
extern u8 data_ov001_020ad630[];
extern u8 data_ov001_020ad62c[];
extern u8 data_ov001_020ad628[];

extern u8 data_ov001_020ad624;
extern u8 data_ov001_020ad620;

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
    else if (data_ov001_020ad634[mCharacter] == this)
        data_ov001_020ad634[mCharacter] = mNext;

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
    unk_19 = 0;

    if (data_ov001_020ad630[mCharacter])
        --data_ov001_020ad630[mCharacter];
}

// @symbol func_ov001_020ab228
/* CSE off is load-bearing: the cartridge re-issues the list-head load
 * across the store into mNext. The bracket binds because codegen is
 * not deferred here. */
#pragma opt_common_subs off
extern "C" void func_ov001_020ab228(char *c_, char *a1, int idx, int a3,
                                    unsigned char a5)
{
    dCapIcon_c *c = (dCapIcon_c *)c_;
    c->mCharacter = (unsigned char)idx;
    c->mOwner = (dActor_c *)a1;
    c->mOwnerUniqueID = *(u32 *)(a1 + 4);
    c->unk_19 = (unsigned char)a3;
    c->mNext = data_ov001_020ad634[idx];
    if (data_ov001_020ad634[idx])
        data_ov001_020ad634[idx]->mPrev = c;
    data_ov001_020ad634[idx] = c;
    data_ov001_020ad630[idx]++;
    if (a3 == 3)
        data_ov001_020ad628[idx] |= 8;
    c->unk_1a = 0;
    c->mFlags = 0;
    {
        unsigned char *p1b = (unsigned char *)(int)&c->mFlags;
        *p1b = (*p1b & ~1) | (a5 & 1);
    }
    if (a5 != 0 || a3 == 3)
        func_ov001_020aa6cc(idx);
    c->mSlot = -1;
}
#pragma opt_common_subs on
#pragma defer_codegen on

/* Deferred half, descending ROM order: ctor, dtor pair, then reset. */

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
        data_ov001_020ad634[i] = 0;
        data_ov001_020ad62c[i] = 0;
        data_ov001_020ad628[i] = 0;
        if (SaveData::IsCharacterUnlocked(i))
            data_ov001_020ad628[i] |= 0x10;
        data_ov001_020ad630[i] = 0;
    }
    data_ov001_020ad620 = 0;
    data_ov001_020ad624 = 0;
}
