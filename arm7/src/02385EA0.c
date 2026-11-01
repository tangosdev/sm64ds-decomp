/* storage 0x02385EA0 runtime 0x037FDD38 linked 0x50 wram ARM.
 * Byte from the 16-byte window fn_02385EF0 publishes at unk_38075A8.
 * Out of range, refresh the window from addr and then read.
 * Pool: unk_38075A8, byte_38075B0. */

extern int unk_38075A8[];
extern unsigned char byte_38075B0[];
extern int fn_02385EF0(unsigned int addr);

int fn_02385EA0(unsigned int addr) {
    if (addr < (unsigned int)unk_38075A8[0] || addr >= (unsigned int)unk_38075A8[1])
        fn_02385EF0(addr);
    return byte_38075B0[addr - (unsigned int)unk_38075A8[0]];
}
