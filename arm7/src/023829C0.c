/* storage 0x023829C0 runtime 0x037FA858 size 0x4C (listed 0x44 + pool) wram ARM.
 * First-run init: if halfword flag is clear, set it, clear unk_3806FF0 pair,
 * tail-call fn_02380884(4). Two address pools, one call. */

extern unsigned short unk_3806FEC;
extern int unk_3806FF0[];
extern int fn_02380884(int a1);

int fn_023829C0(void) {
    if (unk_3806FEC == 0) {
        unk_3806FEC = 1;
        unk_3806FF0[0] = 0;
        unk_3806FF0[1] = 0;
        return fn_02380884(4);
    }
    return (int)&unk_3806FEC;
}
