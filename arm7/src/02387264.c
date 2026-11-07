/* storage 0x02387264 runtime 0x037FF0FC size 0x78 + pool 0x8C wram ARM.
 * Zero the control block, set mode word, then three calls (one 6-arg with two
 * stack slots). a3 re-reads v0[4], banking on the visible zero store. */
extern int fn_02381110(int *, int, int, int *, int, int);
extern int fn_02380FAC(int);
extern int fn_023830C8(int, void *);
extern int dword_38087A0[];
extern int unk_38087E0[];
extern int dword_3808D80;
extern void fn_023872F0(void);
extern void fn_02387410(void);
int fn_02387264(void) {
    int *v0 = dword_38087A0;
    v0[3] = -3;
    {
        int z = 0;
        v0[4] = z;
        v0[6] = z;
        v0[0] = z;
        v0[2] = z;
        *(unsigned short *)((char *)v0 + 0x14) = z;
        *(unsigned short *)((char *)v0 + 0xBC) = z;
        v0[15] = 4;
        fn_02381110(unk_38087E0, (int)fn_023872F0, z, &dword_3808D80, 1024, v0[15]);
    }
    fn_02380FAC((int)unk_38087E0);
    return fn_023830C8(11, fn_02387410);
}
