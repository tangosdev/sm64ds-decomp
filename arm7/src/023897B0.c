/* storage 0x023897B0 runtime 0x03801648 size 0x34 + 8 pool wram ARM.
 * Two pool data refs, no calls. */

extern int unk_38094E4;
extern unsigned short unk_38094E8[];

unsigned short *fn_023897B0(void) {
    unk_38094E4 = 1;
    ((int *)unk_38094E8)[8] = 0;
    {
        int i = 0;
        do {
            unk_38094E8[i] = 0;
            i++;
        } while (i < 16);
    }
    return unk_38094E8;
}
