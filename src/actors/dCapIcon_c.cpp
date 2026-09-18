//cpp
/* dCapIcon_c -- the cap-icon list node and its per-character lists, ov001.
 *
 * Reconstructed translation unit: the contiguous linker run
 * 0x020ab110..0x020ab3f0, ROM ordinals 0..5, six functions, assembled from the
 * six one-function legacy sources this change deletes and then reconciled by
 * hand. config/tu_manifest.d/ov001/dCapIcon_c.json names every one of them.
 *
 * `#pragma defer_codegen` IS LOAD-BEARING AND IT IS POSITIONAL, AND THAT IS
 * WHY THIS FILE IS IN TWO HALVES. The cartridge refutes one whole-unit
 * optimisation state for these six functions: ordinal 1 re-issues the
 * `data_ov001_020ad634[idx]` load across the store that follows it, and no
 * spelling of that member reproduces the re-issue with common-subexpression
 * elimination on -- three were measured, including the one whose store has the
 * array's own element type. Every other member needs it on, most visibly
 * ordinal 0's tail, which folds one address computation that a CSE-off build
 * repeats. With codegen deferred (the default) mwccarm 2004/b56 reads each
 * optimisation pragma once, at end of file, so the last state set anywhere
 * governs the whole unit and that split is unreachable. Generating at parse
 * time instead makes the `opt_common_subs` bracket below bind to ordinal 1
 * alone.
 *
 * SO THE EMISSION ORDER COMES FROM TWO RULES AT ONCE. mwccarm emits one .text
 * section per function; the parse-time half is emitted in source order as it
 * is parsed, and the deferred half is emitted at end of file in the REVERSE of
 * source order. Ordinals 0 and 1 are therefore written ascending, and ordinals
 * 5, 4/3 and 2 are written descending after `defer_codegen on` restores the
 * default. Do not reorder either half to "fix" a diff.
 *
 * ROM .text, ascending:
 *   0x020ab110  _ZN10dCapIcon_c6UnlinkEv    ordinal 0  parse-time half
 *   0x020ab228  func_ov001_020ab228         ordinal 1  parse-time half
 *   0x020ab2e4  func_ov001_020ab2e4         ordinal 2  deferred half
 *   0x020ab374  _ZN10dCapIcon_cD0Ev         ordinal 3  } ordinals 3 and 4 are
 *   0x020ab3a0  _ZN10dCapIcon_cD1Ev         ordinal 4  } one C++ definition
 *   0x020ab3c4  _ZN10dCapIcon_cC1Ev         ordinal 5  deferred half
 *
 * The destructor is the only virtual this class has and it is defined out of
 * line here, so this TU owns the key function and emits _ZTV/_ZTI/_ZTS as
 * vague-linkage passengers, together with the C2 and D2 variants nothing in
 * the ROM calls. The deferred half emits that group as D2, D0, D1 and puts C2
 * after C1, so the two homeless variants straddle the licensed run and have to
 * deadstrip for the range to close; the manifest's compiler_only_output block
 * licenses exactly those five symbols and linkcheck is what proves they go.
 */
#pragma defer_codegen off

#include "CapIcon.h"
#include "SaveData.h"

extern "C" {

/* The live cap-slot table, indexed by dCapIcon_c::mSlot: each entry is the
 * actor currently occupying that slot. Spelt `int[]` to agree with the three
 * other tree declarations; the actor view is an inference, so the cast lives at
 * the use site rather than in the type. */
extern int data_0209f3e8[];

/* Per-character list heads, node counts and flag words, three entries wide
 * each. The legacy shards spelt the head array `void *[]` and `int[]` because
 * they only ever stored into it; ordinal 0 walks the nodes, so the
 * node-pointer view is the complete one and is the one kept here. */
extern dCapIcon_c *data_ov001_020ad634[];
extern u8 data_ov001_020ad630[];
extern u8 data_ov001_020ad62c[];
extern u8 data_ov001_020ad628[];

/* Two single bytes, not arrays -- ordinal 2 clears both, and the rest of ov001
 * reads and writes them as scalars. */
extern u8 data_ov001_020ad624;
extern u8 data_ov001_020ad620;

extern void func_ov001_020aa6b0(int *icon, int value);
extern void func_ov001_020aa6cc(int character);

}

/* -------------------------------------------------------------------------- */
/* ordinal 0 -- 0x020ab110  _ZN10dCapIcon_c6UnlinkEv                          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dCapIcon_c6UnlinkEv
/* Unlinks this cap icon from its per-character list and restores the detached
 * defaults. The role is ROM-proven; the member spelling `Unlink` is inferred. */
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

/* -------------------------------------------------------------------------- */
/* ordinal 1 -- 0x020ab228  func_ov001_020ab228                               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov001_020ab228
/* Links an icon into the list for one character. The cartridge loads the list
 * head at 0x020ab248 and again at 0x020ab254, across the store that puts the
 * first load into the node, which is why this one member is generated with
 * common-subexpression elimination off. The bracket binds because codegen is
 * not deferred here. */
#pragma opt_common_subs off
extern "C" void func_ov001_020ab228(char* c, char* a1, int idx, int a3, unsigned char a5){
    *(unsigned char*)(c+0x18) = (unsigned char)idx;
    *(int*)(c+4) = (int)a1;
    *(int*)(c+8) = *(int*)(a1+4);
    *(unsigned char*)(c+0x19) = (unsigned char)a3;
    *(int*)(c+0x10) = (int)data_ov001_020ad634[idx];
    if(data_ov001_020ad634[idx]) *(int*)((char*)data_ov001_020ad634[idx]+0xc) = (int)c;
    data_ov001_020ad634[idx] = (dCapIcon_c*)c;
    data_ov001_020ad630[idx]++;
    if(a3==3) data_ov001_020ad628[idx] |= 8;
    *(unsigned char*)(c+0x1a) = 0;
    *(unsigned char*)(c+0x1b) = 0;
    {
        unsigned char *p1b = (unsigned char *)(int)(c + 0x1b);
        *p1b = (*p1b & ~1) | (a5 & 1);
    }
    if(a5!=0 || a3==3) func_ov001_020aa6cc(idx);
    *(int*)(c+0x14) = -1;
}
#pragma opt_common_subs on
#pragma defer_codegen on

/* ==========================================================================
 * Everything below is the DEFERRED half and is written in DESCENDING ROM
 * order, because mwccarm emits deferred functions at end of file in the
 * reverse of source order: ordinal 5, then ordinals 4 and 3 as one
 * definition, then ordinal 2.
 * ========================================================================== */

/* -------------------------------------------------------------------------- */
/* ordinal 5 -- 0x020ab3c4  _ZN10dCapIcon_cC1Ev                               */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dCapIcon_cC1Ev
/* The compiler supplies the dCapIcon_c vptr store. The constructor body clears
 * the intrusive links and marks the icon as not linked, exactly as in ROM. */
dCapIcon_c::dCapIcon_c()
{
    mPrev = 0;
    mNext = 0;
    mFlags |= 4;
}

/* -------------------------------------------------------------------------- */
/* ordinals 4 and 3 -- 0x020ab3a0 _ZN10dCapIcon_cD1Ev,                        */
/*                     0x020ab374 _ZN10dCapIcon_cD0Ev                         */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dCapIcon_cD1Ev
// @symbol _ZN10dCapIcon_cD0Ev
/* The compiler supplies the vptr store around the one class-specific action:
 * unlinking the icon. The deleting variant's Memory::operator_delete2 tail
 * comes from the inline operator delete in the header. */
dCapIcon_c::~dCapIcon_c()
{
    Unlink();
}

/* -------------------------------------------------------------------------- */
/* ordinal 2 -- 0x020ab2e4  func_ov001_020ab2e4                               */
/* -------------------------------------------------------------------------- */
// @symbol func_ov001_020ab2e4
/* Resets all three per-character lists, re-reading the save file for which
 * characters are unlocked. */
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
