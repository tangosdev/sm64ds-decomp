//cpp
/* Inter-processor lock words (the Nitro SDK's os_spinLock.c),
 * arm9 .text 0x02057014..0x02057410.
 *
 * pret/pokediamond's NitroSDK OS_spinLock.c, compiled with its pinned
 * mwccarm 1.2/sp2p3, emits 19 functions; 18 of them land here in the same
 * order, 17 at the same size. OS_LockCartridge is not in this ROM (nothing
 * calls it). OSi_WaitByLoop, an inline in pret's newer SDK, is a function of
 * its own here and spins with OS_SpinWait instead of the SVC.
 *
 * mwccarm emits an asm function where it stands and defers the C bodies,
 * which come out in reverse. So the three asm functions open the run and the
 * C functions read bottom-up: OSi_WaitByLoop at 0x020570a8, OS_InitLock at
 * 0x02057320.
 *
 * The IRQ:: calls are the SDK's OS_DisableInterrupts/OS_RestoreInterrupts
 * (Disable/Restore) and their _IrqAndFiq forms (DisableAll/RestoreAll).
 */
#include "IRQ.h"
#include "nitro/hw/registers.h"
#include "nitro/os/spinLock.h"

extern "C" {

// local extern: no header declares these SDK helpers yet.
extern void OS_SpinWait(u32 cycles);
extern u32 MI_SwapWord(u32 setData, volatile u32 *destp);
extern void MultiStore_Int(int data, int *destp, int size); // MIi_CpuClear32
extern u32 data_020a6124; // OS_InitLock's isInitialized

// Run once from OS_Init. Takes the init word as ID 0x7e and waits for the
// ARM7 to clear its extension field, marks every ARM9 lock ID free, clears the
// shared lock words, hands the card and cartridge buses to the ARM7, and leaves
// the init word held by ID 0x7f.
void OS_InitLock(void)
{
    OSLockWord *lockp;
    // Measured: the ROM passes the fill value through a stack slot.
    volatile u32 zero;

    if (data_020a6124) {
        return;
    }
    data_020a6124 = 1;

    lockp = (OSLockWord *)HW_INIT_LOCK_BUF;
    lockp->lockFlag = 0;

    (void)OS_TryLockByWord(0x7e, lockp, 0);

    while (lockp->extension) {
        OSi_WaitByLoop();
    }

    ((u32 *)HW_LOCK_ID_FLAG_MAIN)[0] = 0xffffffff;
    ((u32 *)HW_LOCK_ID_FLAG_MAIN)[1] = 0xffff0000;

    zero = 0;
    MultiStore_Int(zero, (int *)HW_SHARED_LOCK_BUF, 0x28);

    // Card and cartridge buses to the ARM7.
    REG_EXMEMCNT |= 0x800;
    REG_EXMEMCNT |= 0x80;

    (void)OS_UnlockByWord(0x7e, lockp, 0);
    (void)OS_TryLockByWord(0x7f, lockp, 0);
}

s32 OSi_DoLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp)(void), u32 disableFiq)
{
    s32 lastLockFlag;

    while ((lastLockFlag = OSi_DoTryLockByWord(lockID, lockp, ctrlFuncp, disableFiq)) > 0) {
        OSi_WaitByLoop();
    }

    return lastLockFlag;
}

s32 OS_TryLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp)(void))
{
    return OSi_DoLockByWord(lockID, lockp, ctrlFuncp, 0);
}

s32 OSi_DoUnlockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp)(void), u32 disableFiq)
{
    u32 lastIntrMode;

    if (lockID != lockp->ownerID) {
        return -2;
    }

    lastIntrMode = disableFiq ? IRQ::DisableAll() : IRQ::Disable();

    lockp->ownerID = 0;
    if (ctrlFuncp) {
        ctrlFuncp();
    }
    lockp->lockFlag = 0;

    if (disableFiq) {
        IRQ::RestoreAll(lastIntrMode);
    } else {
        IRQ::Restore(lastIntrMode);
    }

    return 0;
}

s32 OS_UnlockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp)(void))
{
    return OSi_DoUnlockByWord(lockID, lockp, ctrlFuncp, 0);
}

s32 OSi_DoTryLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp)(void), u32 disableFiq)
{
    u32 lastIntrMode = disableFiq ? IRQ::DisableAll() : IRQ::Disable();

    s32 lastLockFlag = (s32)MI_SwapWord(lockID, &lockp->lockFlag);

    if (!lastLockFlag) {
        if (ctrlFuncp) {
            ctrlFuncp();
        }
        lockp->ownerID = lockID;
    }

    if (disableFiq) {
        IRQ::RestoreAll(lastIntrMode);
    } else {
        IRQ::Restore(lastIntrMode);
    }

    return lastLockFlag;
}

s32 OS_UnlockCartridge(u16 lockID)
{
    return OSi_DoUnlockByWord(lockID, (OSLockWord *)HW_CTRDG_LOCK_BUF, OSi_FreeCartridgeBus, 1);
}

s32 OS_TryLockCartridge(u16 lockID)
{
    return OSi_DoTryLockByWord(lockID, (OSLockWord *)HW_CTRDG_LOCK_BUF, OSi_AllocateCartridgeBus, 1);
}

// EXMEMCNT bit 7 gives the GBA slot to the ARM7 when set, to the ARM9 when clear.
void OSi_AllocateCartridgeBus(void)
{
    REG_EXMEMCNT &= ~0x80;
}

void OSi_FreeCartridgeBus(void)
{
    REG_EXMEMCNT |= 0x80;
}

s32 OS_TryLockCard(u16 lockID)
{
    return OS_TryLockByWord(lockID, (OSLockWord *)HW_CARD_LOCK_BUF, OSi_AllocateCardBus);
}

s32 OS_UnlockCard(u16 lockID)
{
    return OS_UnlockByWord(lockID, (OSLockWord *)HW_CARD_LOCK_BUF, OSi_FreeCardBus);
}

// EXMEMCNT bit 11 does the same for the DS card slot.
void OSi_AllocateCardBus(void)
{
    REG_EXMEMCNT &= ~0x800;
}

void OSi_FreeCardBus(void)
{
    REG_EXMEMCNT |= 0x800;
}

u16 OS_ReadOwnerOfLockWord(OSLockWord *lock)
{
    return lock->ownerID;
}

void OSi_WaitByLoop(void)
{
    OS_SpinWait(0x1000);
}

// The SDK's misspelled-name compatibility entry. A compiled forwarder would
// route the address through ip; the SDK writes this one as asm through r1.
asm s32 OS_UnLockCartridge(u16 lockID)
{
    ldr r1, =OS_UnlockCartridge
    bx r1
}

// Takes the lowest free ID from the two masks at HW_LOCK_ID_FLAG_MAIN:
// 0x40 + bit for the first word, 0x60 + bit for the second, -3 if none is left.
asm s32 OS_GetLockID(void)
{
    ldr r3, =HW_LOCK_ID_FLAG_MAIN
    ldr r1, [r3, #0]
    clz r2, r1
    cmp r2, #0x20
    movne r0, #0x40
    bne found
    add r3, r3, #4
    ldr r1, [r3, #0]
    clz r2, r1
    cmp r2, #0x20
    ldr r0, =0xfffffffd
    bxeq lr
    mov r0, #0x60
found:
    add r0, r0, r2
    mov r1, #0x80000000
    mov r1, r1, lsr r2
    ldr r2, [r3, #0]
    bic r2, r2, r1
    str r2, [r3, #0]
    bx lr
}

asm void OS_ReleaseLockID(u16 lockID)
{
    ldr r3, =HW_LOCK_ID_FLAG_MAIN
    cmp r0, #0x60
    addpl r3, r3, #4
    subpl r0, r0, #0x60
    submi r0, r0, #0x40
    mov r1, #0x80000000
    mov r1, r1, lsr r0
    ldr r2, [r3, #0]
    orr r2, r2, r1
    str r2, [r3, #0]
    bx lr
}

} // extern "C"
