/* func_01ffdd08 at 0x01ffdd08 (ITCM). Re-reads the ARM9 and ARM7 binaries from the
 * cartridge back into the RAM addresses the boot loader used. Sits on the reboot path:
 * func_01ffdd98 tail-calls this and then func_01ffdb28.
 *
 * The six pool words are 0x027FFE20/28/2C and 0x027FFE30/38/3C. GBATEK's memory map puts
 * a 0x200-byte copy of the cartridge header at 0x027FFE00, and its header layout gives
 * +0x20/+0x28/+0x2C = ARM9 rom offset / ram address / size and +0x30/+0x38/+0x3C = the
 * same three for ARM7. Confirmed against this ROM's own header (build/sm64ds.nds):
 *   arm9 0x4000 -> 0x02004000, 0x5d504    arm7 0x1b4800 -> 0x02380000, 0x24b24
 * so the values really are those fields and nothing else.
 *
 * Each address is materialised as its own pool word rather than one base plus offsets,
 * so the source names six independent absolute locations - not a struct pointer.
 *
 * The 0x8000 clamp: cartridge offsets below 0x8000 are the header plus the KEY1 secure
 * area, which is not readable with the plain ROM read command this uses. The ARM9 image
 * starts at 0x4000, so the leading 0x4000 bytes are skipped - destination advanced, length
 * shortened, offset pulled up to 0x8000 - and only the tail is reloaded; the secure-area
 * part is already resident in RAM from boot. ARM7 starts at 0x1b4800 and is copied whole.
 *
 * func_02057020 / func_0205710c keep their func_ names from config/arm9/symbols.txt.
 * The low-halfword truncation of func_02057020's result is the same idiom already in
 * src/func_02058690.c and src/_ZN8SaveData16ReadDataFromCartEPcjj.cpp.
 */
#include "types.h"

extern int func_02057020(void);
extern void func_0205710c(int lockId);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEv(void);
extern void OSReadROMArea(u32 romOffset, u32 dest, u32 size);

#define ROM_HEADER_ARM9_ROM_OFFSET (*(u32 *)0x027FFE20)
#define ROM_HEADER_ARM9_RAM_ADDR   (*(u32 *)0x027FFE28)
#define ROM_HEADER_ARM9_SIZE       (*(u32 *)0x027FFE2C)
#define ROM_HEADER_ARM7_ROM_OFFSET (*(u32 *)0x027FFE30)
#define ROM_HEADER_ARM7_RAM_ADDR   (*(u32 *)0x027FFE38)
#define ROM_HEADER_ARM7_SIZE       (*(u32 *)0x027FFE3C)

#define ROM_SECURE_AREA_END 0x8000

void func_01ffdd08(void)
{
    u32 romOffset = ROM_HEADER_ARM9_ROM_OFFSET;
    u32 dest      = ROM_HEADER_ARM9_RAM_ADDR;
    u32 size      = ROM_HEADER_ARM9_SIZE;

    func_0205710c((u16)func_02057020());
    _ZN4CP1527FlushAndInvalidateDataCacheEv();

    if (romOffset < ROM_SECURE_AREA_END) {
        u32 skipped = ROM_SECURE_AREA_END - romOffset;
        dest += skipped;
        size -= skipped;
        romOffset = ROM_SECURE_AREA_END;
    }
    OSReadROMArea(romOffset, dest, size);

    OSReadROMArea(ROM_HEADER_ARM7_ROM_OFFSET, ROM_HEADER_ARM7_RAM_ADDR,
                  ROM_HEADER_ARM7_SIZE);
}
