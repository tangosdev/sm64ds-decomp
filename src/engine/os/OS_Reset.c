/* The reset handshake with the ARM7 (the Nitro SDK's os_reset.c), arm9
 * .text 0x02059dd4..0x02059eb0.
 *
 * OS_InitReset installs OSi_CommonCallback on the OS PXI channel. The
 * ARM7's answer to OS_PXI_COMMAND_RESET sets OSi_IsResetOccurred, which
 * OS_ResetSystem spins on; any other command stops the system.
 *
 * pret's pokediamond OS_reset.c defines OS_InitReset, OSi_CommonCallback
 * and OSi_SendToPxi in this order, then OS_ResetSystem and its helpers.
 * mwccarm emits the bodies in reverse, so the ROM runs from OSi_SendToPxi
 * at 0x02059dd4 up to OS_InitReset at 0x02059e48. The rest of the file is
 * ITCM code: config/arm9/itcm/symbols.txt has OSi_DoBoot, OSReadROMArea,
 * OSi_ReloadRomData and OS_ResetSystem at 0x01ffdb28..0x01ffde00, and
 * src/OSi_ReloadRomData.cpp defines OS_ResetSystem.
 */
#include "types.h"

#define PXI_FIFO_TAG_OS 12
#define PXI_PROC_ARM7 1

#define OS_PXI_COMMAND_MASK 0x00007f00
#define OS_PXI_COMMAND_SHIFT 8
#define OS_PXI_COMMAND_RESET 0x10

/* local extern: no PXI header in include/ yet (src/engine/pxi/PXI_Fifo.cpp). */
extern void PXI_Init(void);
extern int PXI_IsCallbackReady(int tag, int proc);
extern void PXI_SetFifoRecvCallback(int tag, void *callback);
extern int PXI_SendWordByFifo(unsigned int tag, unsigned int data, unsigned int err);

extern void Crash(void); /* OS_Terminate */

extern u16 OSi_IsInitReset;
extern vu16 OSi_IsResetOccurred;

void OSi_CommonCallback(int tag, u32 data, int err);

void OS_InitReset(void) {
    if (OSi_IsInitReset != 0)
        return;
    OSi_IsInitReset = 1;
    PXI_Init();
    while (PXI_IsCallbackReady(PXI_FIFO_TAG_OS, PXI_PROC_ARM7) == 0)
        ;
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_OS, (void *)OSi_CommonCallback);
}

void OSi_CommonCallback(int tag, u32 data, int err) {
    u16 command = (u16)((data & OS_PXI_COMMAND_MASK) >> OS_PXI_COMMAND_SHIFT);
    if (command == OS_PXI_COMMAND_RESET) {
        OSi_IsResetOccurred = 1;
        return;
    }
    Crash();
}

/* Sends one OS command to the ARM7, retrying until the FIFO takes it.
 * OS_ResetSystem (ITCM, src/OSi_ReloadRomData.cpp) calls it with
 * OS_PXI_COMMAND_RESET. */
void OSi_SendToPxi(u16 data) {
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_OS, (u32)data << 8, 0))
        ;
}
