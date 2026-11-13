//cpp
/* CapIconMgr -- the cap-icon request manager, ov001.
 *
 * This free-function module runs the per-character cap-icon lists.
 * The shared request flags, dispatch state, counts and heads are defined
 * here for this reconstruction; their original TU placement is unknown.
 *
 * The per-frame tick (func_ov001_020aaf40) counts down the flash timer,
 * then dispatches: CapIconMgr_DispatchVs in versus mode, the reset pass
 * CapIconMgr_DispatchNoCap when the local player has no cap, or the main
 * dispatcher CapIconMgr_Dispatch. CapIconMgr_ShouldDispatch gates one
 * character's list, CapIconMgr_ShouldCommit and CapIconMgr_ShouldCommitVs
 * decide whether the picked node is committed, CapIconMgr_IsDispatched
 * tests a list's dispatch bits (0 and 1), CapIconMgr_Commit the commit
 * (sound + 60-frame flash), CapIconMgr_SetMapIconShown sets a node's
 * map-icon byte and CapIconMgr_ClearDispatched clears a list's dispatch
 * bits so the list is dispatched again.
 *
 * The CapIconMgr_* names are coined from the bodies and their callers; the
 * cartridge records no names for this module, and nothing in the SDK or
 * NSMBW lineage matches it. They keep C linkage.
 *
 * deslop leftovers:
 * - func_ov001_020aaf40 stays a linker name: its caller sits in a file
 *   another branch holds.
 * - gPlayers and data_0209f3e8 are arm9 globals this TU does not own;
 *   the player array's type remains outside this reconstruction.
 * - CapIconMgr_Dispatch and CapIconMgr_DispatchVs read mFlags bits 0, 1
 *   and 3 through the CapIconFlags view below; a mask test on the u8
 *   changes the code (8 bytes shorter in CapIconMgr_DispatchVs).
 */

#include "types.h"
#include "CapIconMgr.h"
#include "dActor_c.h"
#include "daPly_c.h"
#include "Sound.h"



/* Per cap slot: dMap_c::Render draws the slot's cap icon while it is set. */
extern unsigned char gMapCapIconShown[];
extern unsigned char data_0208a0e0;
extern signed char data_02092114;
extern int data_0209f3e8[];
/* The daPly_c*[] array. The shards disagreed scalar-vs-array; the array
 * spelling gives the same ldr for both uses ([0] = first element). */
extern daPly_c *gPlayers[];
extern unsigned char gShowTouchScreenArrows;
extern unsigned char gGameMode;
extern signed char gCurrentSublevel;

/* local extern: func_0202a8e0's first parameter is an address the callers
 * pass as object pointers; the definition spells it int. */
extern "C" int func_0202a8e0(int a, u8 b);
extern "C" int func_02029600(int x);
extern "C" int SublevelToLevel(int i);

extern "C" void CapIconMgr_DispatchVs(void);
void CapIconMgr_SetMapIconShown(dCapIcon_c *icon, int shown);
void CapIconMgr_ClearDispatched(int character);
void CapIconMgr_Commit(int character, unsigned iconKind, dCapIcon_c *icon);
int CapIconMgr_IsDispatched(int character);
int CapIconMgr_ShouldCommitVs(int character, dCapIcon_c *best);
int CapIconMgr_ShouldCommit(int character, dCapIcon_c *best, int requestState, int realSeen,
                            int bankSeen);
int CapIconMgr_ShouldDispatch(int character, daPly_c *player);
void CapIconMgr_Dispatch(void);
void CapIconMgr_DispatchNoCap(void);
void func_ov001_020aaf40(void);

/* gGameMode is 1 in versus mode. This int-returning predicate is what
 * makes the ROM materialize 0/1 before testing it. */
static inline int IsVsMode(void) {
    return gGameMode == 1;
}

/* dCapIcon_c::mFlags, read bit by bit (see include/CapIcon.h). */
typedef struct {
    unsigned char bank : 1;
    unsigned char real : 1;
    unsigned char unlinked : 1;
    unsigned char skipped : 1;
    unsigned char : 4;
} CapIconFlags;

/* Definitions emit in reverse source order. */

void func_ov001_020aaf40(void)
{
    int character;
    int i;

    if (gCapIconFlashTimer != 0) {
        gCapIconFlashTimer -= 1;
        for (i = 0; i < 3; i++) {
            dCapIcon_c *node = gCapIconListHeads[i];
            while (node) {
                if (node->mSlot != -1 && node->mFlashFrames != 0) {
                    int frames;
                    node->mFlashFrames -= 1;
                    frames = node->mFlashFrames;
                    if (frames == 0) {
                        CapIconMgr_SetMapIconShown(node, 1);
                    } else if (frames % 10 == 0) {
                        CapIconMgr_SetMapIconShown(node, ((frames / 10) & 1) == 0 ? 1 : 0);
                    }
                }
                node = node->mNext;
            }
        }
        if (gCapIconFlashTimer == 0)
            gShowTouchScreenArrows = 0;
    }

    if (IsVsMode()) {
        CapIconMgr_DispatchVs();
        return;
    }

    {
        daPly_c *player = gPlayers[0];
        if (player->HasNoCap() && !player->IsCollectingCap()) {
            CapIconMgr_DispatchNoCap();
            gCapIconDispatchLocked = 1;
            return;
        }
        for (character = 0; character < 3; character++) {
            if (gCapIconDispatchFlags[character] & 2)
                gCapIconDispatchFlags[character] &= ~3;
        }
        CapIconMgr_Dispatch();
        gCapIconDispatchLocked = 0;
    }
}

void CapIconMgr_DispatchNoCap(void) {
    daPly_c *player = gPlayers[0];
    dCapIcon_c *node;
    u8 character;
    u8 i;

    for (character = 0; character < 3; character++) {
        if (gCapIconDispatchFlags[character] & 2)
            continue;
        CapIconMgr_ClearDispatched(character);
        node = gCapIconListHeads[character];
        while (node != 0) {
            node->mFlags &= ~2;
            if (node->mSlot != -1) {
                CapIconMgr_SetMapIconShown(node, 0);
                data_0209f3e8[node->mSlot] = 0;
            }
            node = node->mNext;
        }
        if (character != player->mCharacter) {
            gCapIconDispatchFlags[character] |= 2;
        }
    }

    for (i = 0; i < 3; i++) {
        u8 *dispatchFlags = &gCapIconDispatchFlags[i];
        if (*dispatchFlags & 2)
            continue;
        node = gCapIconListHeads[i];
        while (node != 0) {
            void *found = dActor_c::FindWithID(node->mOwnerUniqueID);
            if (found != 0 || found != node->mOwner) {
                if (node->mIconKind == 4) {
                    node->mFlags |= 2;
                    node->mSlot = func_0202a8e0((int)node->mOwner, node->mCharacter);
                    CapIconMgr_SetMapIconShown(node, 1);
                    CapIconMgr_Commit(i, node->mIconKind, 0);
                    *dispatchFlags |= 2;
                }
            } else {
                node->Unlink();
            }
            node = node->mNext;
        }
    }
}

void CapIconMgr_Dispatch(void) {
    unsigned bankKind;
    int requestState;
    int bankHadSlot;
    daPly_c *player = gPlayers[0];
    dCapIcon_c *node;
    dCapIcon_c *best;
    unsigned maxKind;
    int bankSeen;
    u8 character;
    int realSeen;
    u8 *requestFlags;

    if (player->IsCollectingCap() != 0) {
        u8 i;
        for (i = 0; i < 3; i++) {
            if (gCapIconRequestFlags[i] & 0x40)
                gCapIconRequestFlags[i] |= 0x80;
        }
    }

    for (character = 0; character < 3; character++) {
        u8 state = gCapIconRequestFlags[character] & 3;
        maxKind = 0;
        bankHadSlot = 0;
        requestState = state;
        best = 0;
        bankSeen = 0;
        requestFlags = &gCapIconRequestFlags[character];
        realSeen = 0;

        if (CapIconMgr_ShouldDispatch(character, player) != 0) {
            node = gCapIconListHeads[character];
            while (node != 0) {
                void *found = dActor_c::FindWithID(node->mOwnerUniqueID);
                if (found != 0 || found != node->mOwner) {
                    CapIconFlags *flags = (CapIconFlags *)&node->mFlags;
                    if (!flags->bank && flags->real) {
                        realSeen = 1;
                        maxKind = node->mIconKind;
                    } else {
                        int request = *requestFlags;
                        if ((request & 0x30) || node->mIconKind >= 2) {
                            if (flags->bank) {
                                if (node->mSlot == -1) {
                                    node->mSlot =
                                        func_0202a8e0((int)node->mOwner, node->mCharacter);
                                    node->mFlags |= 2;
                                    CapIconMgr_SetMapIconShown(node, 1);
                                } else {
                                    bankHadSlot = 1;
                                }
                                bankSeen = 1;
                                bankKind = node->mIconKind;
                                if ((*requestFlags & 3) == 0)
                                    *requestFlags |= 1;
                                if (node->mIconKind >= 2)
                                    *requestFlags |= 0x20;
                            } else {
                                if (node->mIconKind != 0) {
                                    if (maxKind < node->mIconKind) {
                                        maxKind = node->mIconKind;
                                        best = node;
                                    }
                                    if ((request & 3) == 0)
                                        *requestFlags |= 1;
                                    if (node->mIconKind >= 2)
                                        *requestFlags |= 0x20;
                                } else {
                                    if ((request & 3) == 1 && maxKind == 0) {
                                        if (((CapIconFlags *)&node->mFlags)->skipped) {
                                            best = node;
                                        } else {
                                            node->mFlags |= 8;
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    node->Unlink();
                }
                node = node->mNext;
            }

            if (CapIconMgr_ShouldCommit(character, best, requestState, realSeen, bankSeen) != 0) {
                best->mFlags |= 2;
                best->mSlot = func_0202a8e0((int)best->mOwner, best->mCharacter);
                CapIconMgr_SetMapIconShown(best, 1);
                CapIconMgr_Commit(character, best->mIconKind, best);
            } else {
                if (bankSeen != 0 &&
                    (player->IsCollectingCap() != 0 || (*requestFlags & 0x40) != 0)) {
                    if (bankHadSlot != 0) {
                        CapIconMgr_Commit(character, 3, 0);
                    } else {
                        CapIconMgr_Commit(character, bankKind, 0);
                    }
                } else {
                    if (realSeen != 0) {
                        CapIconMgr_Commit(character, 3, 0);
                    }
                }
            }
        }

        if ((*requestFlags & 3) == 0) {
            int level = SublevelToLevel(gCurrentSublevel);
            if ((unsigned)(level - 0xf) <= 2)
                *requestFlags |= 1;
            else
                *requestFlags |= 2;
        }
    }
}

int CapIconMgr_ShouldDispatch(int character, daPly_c *player) {
    if ((gCapIconRequestFlags[character] & 0xc0) == 0xc0)
        gCapIconRequestFlags[character] &= ~0x40;
    unsigned char request = gCapIconRequestFlags[character];
    if (request & 8) {
        gCapIconRequestFlags[character] = request & ~8;
        return 1;
    }
    if (gCapIconCounts[character] == 0)
        return 0;
    if (request & 0x40)
        return 0;
    if (character == player->mCharacter)
        return 0;
    if (CapIconMgr_IsDispatched(character))
        return 0;
    if ((gCapIconRequestFlags[character] & 3) == 2)
        return 0;
    if (!player->IsCollectingCap() && character == player->param1 &&
        (gCapIconRequestFlags[character] & 3))
        return 0;
    return 1;
}

/* The fifth parameter is passed by the caller and never read. */
int CapIconMgr_ShouldCommit(int character, dCapIcon_c *best, int requestState, int realSeen,
                            int bankSeen) {
    if (requestState == 0 && character == data_02092114) {
        gCapIconRequestFlags[character] |= 0x40;
        gCapIconDispatchFlags[character] |= 0x80;
        return 0;
    }
    if (best == 0)
        return 0;
    unsigned char kind = best->mIconKind;
    if (kind >= 3)
        return 1;
    if (realSeen != 0)
        return 0;
    daPly_c *player = gPlayers[0];
    if (kind != 4) {
        if (player->IsCollectingCap())
            return 0;
    }
    if (!player->IsCollectingCap()) {
        if (character == player->param1)
            return 0;
    }
    return 1;
}

int CapIconMgr_ShouldCommitVs(int character, dCapIcon_c *best) {
    if (best == 0)
        return 0;
    if (best->mIconKind == 3)
        return 1;
    int i = 0;
    for (i = 0; i < data_0208a0e0; i++) {
        daPly_c *player = gPlayers[i];
        if (player == 0)
            continue;
        if (player->IsCollectingCap())
            return 0;
        if (character == player->param1)
            return 0;
    }
    return 1;
}

int CapIconMgr_IsDispatched(int character) {
    return (gCapIconDispatchFlags[character] & 3) != 0;
}

void CapIconMgr_Commit(int character, unsigned iconKind, dCapIcon_c *icon) {
    gCapIconDispatchFlags[character] |= 1;
    if ((gCapIconDispatchFlags[character] & 0x80) && gCapIconDispatchLocked == 0) {
        if (icon == 0)
            return;
        if (iconKind >= 3)
            return;
        if (func_02029600((int)icon->mOwner) == 0)
            return;
        gShowTouchScreenArrows = 1;
        func_02012790(0x24);
        icon->mFlashFrames = 0x3c;
        gCapIconFlashTimer = icon->mFlashFrames;
        return;
    }
    gCapIconDispatchFlags[character] |= 0x80;
}

void CapIconMgr_ClearDispatched(int character) {
    gCapIconDispatchFlags[character] &= ~3;
}

void CapIconMgr_SetMapIconShown(dCapIcon_c *icon, int shown) {
    int slot = icon->mSlot;
    if (slot != -1) {
        gMapCapIconShown[slot] = shown;
    }
}

extern "C" void CapIconMgr_DispatchVs(void) {
    daPly_c *player;
    dCapIcon_c *best;
    int bankSeen;
    u8 *requestFlags;
    u8 request;
    dCapIcon_c *node;
    u8 maxKind;
    u8 character;
    void *found;
    u8 numPlayers;
    int idx;

    maxKind = 0;
    for (character = 0; character < 3; character++) {
        best = 0;
        node = gCapIconListHeads[character];
        bankSeen = 0;
        requestFlags = &gCapIconRequestFlags[character];
        request = gCapIconRequestFlags[character];

        if (request == 0) {
            while (node != 0) {
                node->mFlags &= ~2;
                if (node->mIconKind == 1) {
                    *requestFlags = 1;
                }
                node = node->mNext;
            }
            if (*requestFlags == 0) {
                *requestFlags = 2;
            } else {
                *requestFlags = 1;
            }
        }

        node = gCapIconListHeads[character];

        if (CapIconMgr_IsDispatched(character) != 0)
            continue;
        if (*requestFlags == 2)
            continue;
        if (gCapIconCounts[character] == 0)
            continue;

        while (node != 0) {
            found = dActor_c::FindWithID(node->mOwnerUniqueID);
            if (found != 0 || found != node->mOwner) {
                if (((CapIconFlags *)&node->mFlags)->bank) {
                    if (node->mSlot == -1) {
                        node->mSlot = func_0202a8e0((int)node->mOwner, node->mCharacter);
                        node->mFlags |= 2;
                        CapIconMgr_SetMapIconShown(node, 1);
                    }
                    bankSeen = 1;
                    maxKind = node->mIconKind;
                } else if (node->mIconKind != 0) {
                    if (maxKind < node->mIconKind) {
                        maxKind = node->mIconKind;
                        best = node;
                    }
                } else if (maxKind == 0) {
                    if (((CapIconFlags *)&node->mFlags)->skipped) {
                        best = node;
                    } else {
                        node->mFlags |= 8;
                    }
                }
            } else {
                node->Unlink();
            }
            node = node->mNext;
        }

        if (CapIconMgr_ShouldCommitVs(character, best) != 0) {
            best->mFlags |= 2;
            best->mSlot = func_0202a8e0((int)best->mOwner, best->mCharacter);
            CapIconMgr_SetMapIconShown(best, 1);
            CapIconMgr_Commit(character, best->mIconKind, best);
            continue;
        }

        if (bankSeen == 0)
            continue;

        numPlayers = data_0208a0e0;
        for (idx = 0; idx < numPlayers; idx++) {
            player = gPlayers[idx];
            if (character == player->param1)
                break;
            player = 0;
        }
        if (player != 0 && player->IsCollectingCap() != 0) {
            CapIconMgr_Commit(character, maxKind, 0);
        }
    }
}


u8 gCapIconFlashTimer;
u8 gCapIconDispatchLocked;
u8 gCapIconRequestFlags[3];
u8 gCapIconDispatchFlags[3];
u8 gCapIconCounts[4];
dCapIcon_c *gCapIconListHeads[3];
