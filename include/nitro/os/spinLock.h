#ifndef NITRO_OS_SPINLOCK_H
#define NITRO_OS_SPINLOCK_H

/*
 * Lock words for the resources the two CPUs share, under the NitroSDK's own
 * names (nitro/os/common/spinLock.h). The bodies are os_spinLock.c, in
 * src/engine/os/OS_SpinLock.cpp.
 *
 * A lock word's lockFlag holds the lock ID of whoever has it, or 0 when it is
 * free. Lock IDs come from OS_GetLockID (0x40..0x7f on the ARM9) and go back
 * with OS_ReleaseLockID. Every lock call returns 0 on success; a positive
 * value is the ID of the current owner.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "types.h"
#include "nitro/hw/mmap_shared.h" /* HW_LOCK_ID_FLAG_MAIN, HW_*_LOCK_BUF */

typedef volatile struct OSLockWord {
    u32 lockFlag;
    u16 ownerID;
    u16 extension;
} OSLockWord;

/* Set in a lock word's ownerID while the ARM9 holds it. */
#define OS_MAINP_LOCKED_FLAG 0x40

#define OS_ReadOwnerOfLockCartridge() OS_ReadOwnerOfLockWord((OSLockWord *)HW_CTRDG_LOCK_BUF)

#ifdef __cplusplus
extern "C" {
#endif

void OS_InitLock(void);
s32 OSi_DoLockByWord(u16, OSLockWord *, void (*)(void), u32);
s32 OS_TryLockByWord(u16, OSLockWord *, void (*)(void));
s32 OSi_DoUnlockByWord(u16, OSLockWord *, void (*)(void), u32);
s32 OS_UnlockByWord(u16, OSLockWord *, void (*)(void));
s32 OSi_DoTryLockByWord(u16, OSLockWord *, void (*)(void), u32);
s32 OS_UnlockCartridge(u16);
s32 OS_UnLockCartridge(u16);
s32 OS_TryLockCartridge(u16);
void OSi_AllocateCartridgeBus(void);
void OSi_FreeCartridgeBus(void);
s32 OS_TryLockCard(u16);
s32 OS_UnlockCard(u16);
void OSi_AllocateCardBus(void);
void OSi_FreeCardBus(void);
u16 OS_ReadOwnerOfLockWord(OSLockWord *);
void OSi_WaitByLoop(void);
s32 OS_GetLockID(void);
void OS_ReleaseLockID(u16);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_OS_SPINLOCK_H */
