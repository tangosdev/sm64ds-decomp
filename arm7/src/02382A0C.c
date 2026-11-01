/* storage 0x02382A0C runtime 0x037FA8A4 size 0x74 (listed 0x68 + pool) wram ARM.
 * Refresh display register from node key: call fn_02382A80, then fn_02380978,
 * fold key bits into volatile 0x4000004, set bit 0x20, tail-call fn_023808C0(4).
 * Three pools (two numeric consts, one address), three calls. */

extern int fn_02382A80(int a1);
extern int fn_02380978(int a1, int a2);
extern int fn_02382558(void);
extern int fn_023808C0(int a1);

int fn_02382A0C(int *a1) {
    short *h = (short *)a1;
    int io = *(volatile short *)0x4000006;
    int s;
    fn_02382A80(h[6] - io);
    fn_02380978(4, (int)fn_02382558);
    s = h[6];
    *(volatile unsigned short *)0x4000004 = (unsigned short)(
        (*(volatile unsigned short *)0x4000004 & 0x3F)
        | (unsigned)((s & 0xFF) << 8)
        | (unsigned)((s & 0x100) >> 1));
    {
        unsigned d2 = *(volatile unsigned short *)0x4000004;
        *(volatile unsigned short *)0x4000004 = (unsigned short)(d2 | 0x20);
    }
    return fn_023808C0(4);
}
