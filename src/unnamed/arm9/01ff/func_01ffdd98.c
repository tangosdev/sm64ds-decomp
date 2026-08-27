/* func_01ffdd98 at 0x01ffdd98 (ITCM). Soft reset: hand the caller's argument to the
 * boot-info block, ask the ARM7 over IPC to stand down, wait for its ack, kill interrupts,
 * then reload both binaries from the card and re-enter.
 *
 * It lives in ITCM for a reason: func_01ffdd08 overwrites the ARM9 image in main RAM with a
 * fresh copy off the card, so the code driving that cannot itself be running from main RAM.
 *
 * Names, and where each one comes from:
 *   Crash            0x02019740, already named in config/arm9/symbols.txt (src/Crash.c).
 *                    Reached when the boot-info halfword is 2, i.e. this build refuses to
 *                    soft reset in that boot mode. src/func_02013f4c.c makes the same test
 *                    and routes the ==2 case to func_0205f958 instead of calling here.
 *   func_02059dd4    unnamed. src/func_02059dd4.c is IPCSend(0xc, param << 8, 0) retried
 *                    until it returns 0, so this is IPC command 0xc, subcommand 0x10.
 *   data_020a645c    bss symbol in config/arm9/symbols.txt; the `kind:load` reloc at
 *                    0x01ffddf8 in config/arm9/itcm/relocs.txt resolves this pool word to
 *                    it. Its only writer is src/func_02059e04.c, which sets it to 1 when an
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
 *                    unnamed: src/func_02013f4c.c already tests this exact literal, and no
 *                    name for the field is recoverable from ROM bytes or public docs here.
 *   0x027ffc20       word in the same block. This is its only reference anywhere in the
 *                    ARM9 (searched every extracted binary for the pool constant), so it is
 *                    a message left for the reloaded code. Left unnamed for the same reason.
 *
 * Sole caller: src/func_02013f4c.c at 0x02014090 (the only reloc targeting 0x01ffdd98),
 * which passes 0. Its forward declaration spells the parameter `int`; the store is a plain
 * word either way.
 */
#include "types.h"

extern void Crash(void);
extern void func_02059dd4(s32 param);
extern void func_01ffdd08(void);
extern void func_01ffdb28(void);

extern vu16 data_020a645c;

void func_01ffdd98(u32 resetArg)
{
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
