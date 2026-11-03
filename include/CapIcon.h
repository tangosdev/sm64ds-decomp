#ifndef CAPICON_H
#define CAPICON_H

#include "types.h"

struct dActor_c;

/* The record an actor carries while it is holding one of the caps, and a
 * node in a per-character intrusive doubly-linked list. 0x1c bytes.
 *
 * RTTI names dCapIcon_c. Prev is +0x0c and next is +0x10: the link
 * helper writes the old list head into +0x10 and back-patches that
 * head's +0x0c. Unlink agrees.
 *
 * mFlags, bit by bit. Each line says what kind of evidence backs it.
 * - Bit 0: a bank cap. AddCap sets it for p >= 3 (code-read). In the
 *   running game (2026-11-02), setting it on a kind-0 Goomba's node while
 *   its list was free made that node real on the next pass, in its own
 *   slot, without the single-best pick. The cap appeared on that Goomba.
 *   The bit alone, or a free list alone, did nothing.
 * - Bit 1: this node's cap is the real one. In the running game (2026-11-02,
 *   EU ROM, tools/trace/ask.py), setting it on a Goomba's node made the cap
 *   appear on its head and drop when stomped. Clearing it removed both.
 *   Poking bits 3 and 5, or a same-value write, changed nothing. The map
 *   icon and mSlot did not follow the poke. Unlink clears it.
 *   GetCapState and dCapEnemy_c::UpdateCapPos test it. The same bit
 *   pokes on a cap-carrying Boo (daTrs_c, Big Boo's Haunt) gave the same
 *   result, blind 14/14, and bit 2 hid the Boo's cap too.
 *   Who gets it naturally depends on the level (2026-11-03, observed):
 *   - In Bob-omb Battlefield the manager set it at level entry on one
 *     kind-1 Goomba node per unlocked character.
 *   - In Big Boo's Haunt it never set it on any Boo, even with Luigi
 *     unlocked. All four Boo nodes are kind 0, and on the first tick the
 *     list's request went to state 2, after which it is not processed.
 *     Played as Luigi, the Luigi-cap Boos are deleted instead.
 *   - In Hazy Maze Cave Dorrie's cap node (daDossyCap_c, kind 2) got it on
 *     the first tick, with Wario still locked. daDossyCap_c::Render draws
 *     only while it is set (probe-counted, 3/3 each way). Setting bit 2
 *     there did not stop the draw: Dorrie tests bit 1 alone.
 * - Bit 2: not linked. The constructor and Unlink set it. Setting it at
 *   runtime hid a Goomba's or Boo's cap even with bit 1 set, which matches
 *   UpdateCapPos testing "bit 2 set or bit 1 clear". Dorrie's Render
 *   ignores it.
 * - Bit 3: passed over once by the manager's pick, eligible on the next
 *   pass. Every runtime set of bit 1 on a kind-0 node came the frame after
 *   bit 3. Nothing visible follows from it.
 * The original source likely used bitfields, not masks:
 * func_ov001_020aa420 has to read bits 0 and 3 as bitfields to match.
 * The local bit views in the .cpp files name them bank, real, unlinked
 * and skipped.
 */

namespace Memory {
void operator_delete2(void *);
}

/* Lifecycle declarations have no inline bodies: owners must emit calls
 * to the ROM-owned constructor and destructor rather than duplicating
 * their vtable stores and unlink logic. */
struct dCapIcon_c {
    /* The compiler-owned vptr occupies 0x00. */
    struct dActor_c *mOwner;      /* 0x04 */
    u32 mOwnerUniqueID;           /* 0x08 */
    struct dCapIcon_c *mPrev;     /* 0x0c */
    struct dCapIcon_c *mNext;     /* 0x10 */
    /* Index into the global slot table data_0209f3e8, or -1 for "not held". */
    s32 mSlot;                    /* 0x14 */
    /* Which character's list this node is in. Unlink parks it at 3. */
    u8 mCharacter;                /* 0x18 */
    /* The link helper's 4th argument (daObjMarioCap_c passes its mIconKind);
       Unlink zeros it. Kind 3 raises request flag 8 on link, kind 4 is the
       one the reset pass commits, and the dispatchers keep the highest kind
       of a list as its priority. */
    u8 mIconKind; /* 0x19 */
    /* Zeroed on link. The commit (func_ov001_020aa6e4) stores 0x3c here and
       copies it into gCapIconFlashTimer, the HUD flash countdown. */
    u8 mFlashFrames;              /* 0x1a */
    u8 mFlags;                    /* 0x1b */

    dCapIcon_c();
    virtual ~dCapIcon_c();
    void Unlink();
    void func_ov001_020ab228(char *a1, int idx, int a3, unsigned char a5);
    void operator delete(void *ptr) {
        Memory::operator_delete2(ptr);
    }
};

typedef dCapIcon_c CapIcon;
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dCapIcon_c_size_must_be_0x1c[sizeof(dCapIcon_c) == 0x1c ? 1 : -1];
#endif

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char CapIcon_size_must_be_0x1c[sizeof(CapIcon) == 0x1c ? 1 : -1];
#endif

#endif /* CAPICON_H */
