/* storage 0x02380468 runtime 0x037F8300 size 0x114 + 24 pool wram ARM.
 * Six pools (one address twice), sixteen BL calls (three in-pack, two AM9's).
 * The doubled LDRH pair and the dead LDRH are volatile re-reads. */

extern int fn_02381888(void);
extern int fn_02381210(int a1);
extern int fn_023802D4(void);
extern int fn_02382EF8(int a1);
extern int fn_023803C8(void);
extern int fn_02383C74(int a1);
extern int fn_0238328C(int a1);
extern int fn_02380978(int a1, int a2);
extern int fn_023808C0(int a1);
extern int fn_02382AB0(void);
extern int fn_02387184(int a1);
extern int fn_023871B8(int a1);
extern int fn_0238A648(int a1);
extern int fn_0238E694(int *a1, int *a2);
extern int fn_023885A0(int a1);
extern int fn_02380594(void);
extern int fn_02382BB8(void);
extern int fn_0238DCEC(void);
extern int fn_0238C0B4(int a1);
extern int off_37F8420[];
extern int off_37F8424[];
extern int unk_3805F20[];
extern void sub_37F8000(void);

void fn_02380468(void) {
    int v5[13];
    int v6[3];
    int v0, v1, v2, v3, v4;
    v0 = fn_02381888();
    fn_02381210(v0);
    v1 = fn_023802D4();
    fn_02382EF8(v1);
    v2 = fn_023803C8();
    v3 = fn_02383C74(7);
    fn_0238328C(v3);
    fn_02380978(1, (int)&sub_37F8000);
    fn_023808C0(1);
    (void)*(volatile unsigned short *)0x4000004;
    *(volatile unsigned short *)0x4000004 |= 8;
    (void)*(volatile unsigned short *)0x4000208;
    *(volatile unsigned short *)0x4000208 = 1;
    fn_02382AB0();
    fn_02387184(-1);
    fn_023871B8(15);
    fn_0238A648(12);
    v5[0] = (int)off_37F8420;
    v5[1] = (int)off_37F8424;
    v5[2] = 1536;
    v5[3] = 5;
    v5[8] = 0;
    v5[9] = 8;
    v5[10] = v2;
    v5[11] = (int)unk_3805F20;
    v5[12] = 448;
    v6[0] = 3;
    v5[7] = 64;
    v6[1] = 4;
    v6[2] = 9;
    fn_0238E694(v5, v6);
    fn_023885A0(2);
    for (;;) {
        fn_02380594();
        v4 = fn_02382BB8();
        if (v4 != 0)
            v4 = fn_0238DCEC();
        fn_0238C0B4(v4);
    }
}
