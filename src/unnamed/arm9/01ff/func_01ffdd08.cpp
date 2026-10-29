//cpp
#include "types.h"
#include "CP15.h"

extern "C" {

#define ROM_HEADER_ARM9_ROM_OFFSET (*(u32 *)0x027FFE20)
#define ROM_HEADER_ARM9_RAM_ADDR   (*(u32 *)0x027FFE28)
#define ROM_HEADER_ARM9_SIZE       (*(u32 *)0x027FFE2C)
#define ROM_HEADER_ARM7_ROM_OFFSET (*(u32 *)0x027FFE30)
#define ROM_HEADER_ARM7_RAM_ADDR   (*(u32 *)0x027FFE38)
#define ROM_HEADER_ARM7_SIZE       (*(u32 *)0x027FFE3C)
#define ROM_SECURE_AREA_END 0x8000

extern void Crash(void);
extern void func_02059dd4(s32 param);
extern void func_01ffdd08(void);
extern void func_01ffdb28(void);
extern vu16 data_020a645c;
extern int func_02057020(void);
extern void func_0205710c(int lockId);
extern void OSReadROMArea(u32 romOffset, u32 dest, u32 size);

/* func_01ffdd98 at 0x01ffdd98 (ITCM). Soft reset: hand the caller's argument to the
 * boot-info block, ask the ARM7 over IPC to stand down, wait for its ack, kill interrupts,
 * then reload both binaries from the card and re-enter.
 *
 * It lives in ITCM for a reason: func_01ffdd08 overwrites the ARM9 image in main RAM with a
 * fresh copy off the card, so the code driving that cannot itself be running from main RAM.
 *
 * Names, and where each one comes from:
 *   Crash            0x02019740, already named in config/arm9/symbols.txt (src/Crash.cpp).
 *                    Reached when the boot-info halfword is 2, i.e. this build refuses to
 *                    soft reset in that boot mode. src/func_02013f4c.cpp makes the same test
 *                    and routes the ==2 case to func_0205f958 instead of calling here.
 *   func_02059dd4    unnamed. src/unnamed/arm9/0205/func_02059dd4.c is IPCSend(0xc, param << 8, 0) retried
 *                    until it returns 0, so this is IPC command 0xc, subcommand 0x10.
 *   data_020a645c    bss symbol in config/arm9/symbols.txt; the `kind:load` reloc at
 *                    0x01ffddf8 in config/arm9/itcm/relocs.txt resolves this pool word to
 *                    it. Its only writer is src/unnamed/arm9/0205/func_02059e04.c, which sets it to 1 when an
 *                    incoming IPC word carries subcommand 0x10 -- so it is the ARM7's ack
 *                    for the send above, and the spin below is waiting on that ack.
 *   func_01ffdd08    unnamed. Pools 0x027ffe20/28/2c and 0x027ffe30/38/3c -- the ROM offset,
 *                    RAM address and size fields of the ARM9 and ARM7 binaries in the
 *                    cartridge header copy (GBATEK, DS cartridge header) -- and feeds each
 *                    triple to func_01ffdbd8. It reloads both binaries from the card.
 *   func_01ffdb28    unnamed. Handshakes over REG_IPCSYNC (0x04000180, GBATEK), then reads
 *                    the ARM9 entry address from 0x027ffe24 of that same header copy and
 *                    branches to it.
 *   0x04000208       REG_IME, the interrupt master enable (GBATEK). Written as this bare
 *                    literal in ~20 existing files under src/.
 *   0x027ffc40       boot-info halfword in main RAM's 0x027ffc00 block. Deliberately left
 *                    unnamed: src/func_02013f4c.cpp already tests this exact literal, and no
 *                    name for the field is recoverable from ROM bytes or public docs here.
 *   0x027ffc20       word in the same block. This is its only reference anywhere in the
 *                    ARM9 (searched every extracted binary for the pool constant), so it is
 *                    a message left for the reloaded code. Left unnamed for the same reason.
 *
 * Sole caller: src/func_02013f4c.cpp at 0x02014090 (the only reloc targeting 0x01ffdd98),
 * which passes 0. Its forward declaration spells the parameter `int`; the store is a plain
 * word either way.
 */
void func_01ffdd98(u32 resetArg) {
    if (*(vu16 *)0x27ffc40 == 2)
        Crash();

    *(vu32 *)0x27ffc20 = resetArg;
    func_02059dd4(0x10);

    while (data_020a645c == 0)
        ;

    *(vu16 *)0x4000208 = 0;
    func_01ffdd08();
    func_01ffdb28();
}

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
 * src/unnamed/arm9/0205/func_0205865c.cpp and src/engine/save/SaveData2.cpp.
 */
void func_01ffdd08(void) {
    u32 romOffset = ROM_HEADER_ARM9_ROM_OFFSET;
    u32 dest = ROM_HEADER_ARM9_RAM_ADDR;
    u32 size = ROM_HEADER_ARM9_SIZE;

    func_0205710c((u16)func_02057020());
    CP15::FlushAndInvalidateDataCache();

    if (romOffset < ROM_SECURE_AREA_END) {
        u32 skipped = ROM_SECURE_AREA_END - romOffset;
        dest += skipped;
        size -= skipped;
        romOffset = ROM_SECURE_AREA_END;
    }
    OSReadROMArea(romOffset, dest, size);

    OSReadROMArea(ROM_HEADER_ARM7_ROM_OFFSET, ROM_HEADER_ARM7_RAM_ADDR, ROM_HEADER_ARM7_SIZE);
}
}
