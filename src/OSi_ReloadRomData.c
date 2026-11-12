#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/os/spinLock.h"
#include "nitro/os/system.h"
#include "nitro/hw/registers.h"
#include "nitro/hw/mmap_shared.h"
#include "nitro/mb/mb.h"

#define ROM_HEADER_ARM9_ROM_OFFSET (*(u32 *)0x027FFE20)
#define ROM_HEADER_ARM9_RAM_ADDR   (*(u8 **)0x027FFE28)
#define ROM_HEADER_ARM9_SIZE       (*(u32 *)0x027FFE2C)
#define ROM_HEADER_ARM7_ROM_OFFSET (*(u32 *)0x027FFE30)
#define ROM_HEADER_ARM7_RAM_ADDR   (*(void **)0x027FFE38)
#define ROM_HEADER_ARM7_SIZE       (*(u32 *)0x027FFE3C)
#define ROM_SECURE_AREA_END 0x8000

extern void OSi_SendToPxi(u16 data);
extern void OSi_ReloadRomData(void);
extern void OSi_DoBoot(void);
extern vu16 OSi_IsResetOccurred;
extern void OSReadROMArea(u32 romOffset, void *dest, s32 size);

/* OS_ResetSystem at 0x01ffdd98 (ITCM) -- NitroSDK soft reset, with OSi_DoResetSystem
 * inlined. Identified against pret/pokediamond's OS_reset.c, whose OS_ResetSystem and
 * OSi_DoResetSystem run the same steps in the same order:
 *   - a multiboot child cannot reset (MB_IsMultiBootChild), and this SDK calls
 *     OS_Terminate there;
 *   - the parameter goes to HW_RESET_PARAMETER_BUF for the rebooted image;
 *   - OSi_SendToPxi(OS_PXI_COMMAND_RESET) sends 0x10 << 8 on PXI tag 0xc
 *     (PXI_FIFO_TAG_OS), retrying until the FIFO takes it;
 *   - spin until the ARM7 answers: OSi_IsResetOccurred is set by the PXI
 *     callback OSi_CommonCallback;
 *   - IME off, OSi_ReloadRomData(), OSi_DoBoot().
 * The later SDK's CARD_LockRom and MI_StopDma calls are not here; this build takes the
 * card lock inside OSi_ReloadRomData instead. It lives in ITCM because OSi_ReloadRomData
 * overwrites the ARM9 image in main RAM.
 *
 * Sole caller: src/func_02013f4c.cpp, which passes 0.
 */
void OS_ResetSystem(u32 resetArg) {
    if (MB_IsMultiBootChild())
        OS_Terminate();

    *(u32 *)HW_RESET_PARAMETER_BUF = resetArg;
    OSi_SendToPxi(0x10);

    while (OSi_IsResetOccurred == 0)
        ;

    REG_IME = 0;
    OSi_ReloadRomData();
    OSi_DoBoot();
}

/* OSi_ReloadRomData at 0x01ffdd08 (ITCM) -- NitroSDK. Re-reads the ARM9 and ARM7 binaries
 * from the card into the RAM addresses the boot loader used, from the rom offset / ram
 * address / size fields of the cartridge header copy at 0x027ffe00 (+0x20/+0x28/+0x2c for
 * ARM9, +0x30/+0x38/+0x3c for ARM7). Cartridge offsets below 0x8000 are the header and
 * the secure area, which the plain page-read command cannot reach, so the ARM9 read is
 * clamped to start at 0x8000 and the part below is left as it is in RAM. OSReadROMArea is
 * the SDK's OSi_ReadCardRom32.
 *
 * pret/pokediamond's later version adds a rom-base offset and disables interrupts around
 * the cache flush. This one starts with OS_TryLockCard((u16)OS_GetLockID()) (the SDK's
 * os_spinLock.c, src/engine/os/OS_SpinLock.cpp), the shape of the later OS_ResetSystem's
 * CARD_LockRom((u16)OS_GetLockID()).
 */
void OSi_ReloadRomData(void) {
    u32 romOffset = ROM_HEADER_ARM9_ROM_OFFSET;
    u8 *dest = ROM_HEADER_ARM9_RAM_ADDR;
    u32 size = ROM_HEADER_ARM9_SIZE;

    OS_TryLockCard((u16)OS_GetLockID());
    DC_FlushAll();

    if (romOffset < ROM_SECURE_AREA_END) {
        u32 skipped = ROM_SECURE_AREA_END - romOffset;
        dest += skipped;
        size -= skipped;
        romOffset = ROM_SECURE_AREA_END;
    }
    OSReadROMArea(romOffset, dest, size);

    OSReadROMArea(ROM_HEADER_ARM7_ROM_OFFSET, ROM_HEADER_ARM7_RAM_ADDR, ROM_HEADER_ARM7_SIZE);
}
