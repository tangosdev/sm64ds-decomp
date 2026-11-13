#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/os/spinLock.h"
#include "nitro/os/system.h"
#include "nitro/os/reset.h"
#include "nitro/hw/registers.h"
#include "nitro/hw/mmap_shared.h"
#include "nitro/mb/mb.h"

extern void OSi_SendToPxi(u16 data);
extern void OSi_ReloadRomData(void);
extern void OSi_DoBoot(void);
extern vu16 OSi_IsResetOccurred;
extern void OSReadROMArea(u32 src, void *dst, int len);

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
        _Z12OS_Terminatev();

    *(u32 *)HW_RESET_PARAMETER_BUF = resetArg;
    OSi_SendToPxi(0x10);

    while (OSi_IsResetOccurred == 0)
        ;

    REG_IME = 0;
    OSi_ReloadRomData();
    OSi_DoBoot();
}

/* OSi_ReloadRomData at 0x01ffdd08 (ITCM) -- NitroSDK os_reset.c. Re-reads the ARM9 and
 * ARM7 binaries from the card into the RAM the boot loader put them in, using the rom
 * offset / ram address / size fields of the cartridge header copy at HW_ROM_HEADER_BUF.
 * Card offsets below 0x8000 are the header and the secure area, which the page-read
 * command cannot reach, so the ARM9 read starts at 0x8000 and the RAM below that is left
 * as it is. OSReadROMArea (src/OSReadROMArea.c) is the SDK's OSi_ReadCardRom32.
 *
 * The SDK's later version adds HW_ROM_BASE_OFFSET_BUF and reads all six header fields
 * after the cache maintenance. This one reads the ARM9 fields first, then takes the card
 * lock (OS_TryLockCard((u16)OS_GetLockID()), the shape of the later OS_ResetSystem's
 * CARD_LockRom) and flushes the data cache with DC_FlushAll.
 */
void OSi_ReloadRomData(void) {
    u32 p = (u32)HW_ROM_HEADER_BUF;
    u32 src_arm9 = *(u32 *)(p + 0x20);
    u32 dst_arm9 = *(u32 *)(p + 0x28);
    u32 len_arm9 = *(u32 *)(p + 0x2c);

    _Z14OS_TryLockCardt((u16)_Z12OS_GetLockIDv());
    DC_FlushAll();

    if (src_arm9 < 0x8000) {
        u32 diff = 0x8000 - src_arm9;
        src_arm9 = 0x8000;
        dst_arm9 += diff;
        len_arm9 -= diff;
    }

    OSReadROMArea(src_arm9, (void *)dst_arm9, (int)len_arm9);

    {
        u32 src_arm7 = *(u32 *)(p + 0x30);
        u32 dst_arm7 = *(u32 *)(p + 0x38);
        u32 len_arm7 = *(u32 *)(p + 0x3c);

        OSReadROMArea(src_arm7, (void *)dst_arm7, (int)len_arm7);
    }
}
