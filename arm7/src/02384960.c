#pragma exceptions off

/* storage 0x02384960 runtime 0x037FC7F8 size 0x1C4 wram ARM.
 * Loop 1: dispatch on f04 bits -> fn_02383634 / f01 cases / fn_02383598,
 * fn_023835B0, fn_0238353C. Loop 2: clear f04, set 0x04000403+i*16 bit7.
 * Pools: =chans_38075C4, =0x04000403. */

struct Chan {
    unsigned char f00, f01, f02, f03, f04;
    unsigned char pad05[0x1E];
    unsigned char f23;
    unsigned short f24, f26;
    unsigned char pad28[0x10];
    unsigned char f38, f39, f3A, f3B;
    unsigned char pad3C[2];
    unsigned short f3E;
    int f40, f44;
    unsigned char pad48[0xC];
};

extern struct Chan chans_38075C4[];
extern int fn_0238353C(int, int);
extern int fn_02383598(int, int);
extern int fn_023835B0(int, int, int);
extern int fn_02383634(int, int);
extern int fn_02383658(int, int, int, int, int);
extern int fn_023836E0(int, int, int, int, int, int);
extern int fn_02383770(int, int, int, int, int, int, int, int, int, int);

void fn_02384960(void) {
    struct Chan *v4;
    int i;

    for (i = 0; i < 16; ++i) {
        v4 = &chans_38075C4[i];
        if (v4->f04 != 0) {
            if (v4->f04 & 2)
                fn_02383634(i, 0);
            if (v4->f04 & 1) {
                switch (v4->f01) {
                case 0:
                    goto L0;
                case 1:
                    goto L1;
                case 2:
                    goto L2;
                }
                goto Ld;
L0:             fn_02383770(i, v4->f44, v4->f38, v4->f39 != 0 ? 1 : 2,
                            v4->f3E, v4->f40, v4->f24 & 0xFF, v4->f24 >> 8,
                            v4->f26, v4->f23);
                goto Ld;
L1:             fn_023836E0(i, v4->f44, v4->f24 & 0xFF, v4->f24 >> 8,
                            v4->f26, v4->f23);
                goto Ld;
L2:             fn_02383658(i, v4->f24 & 0xFF, v4->f24 >> 8, v4->f26, v4->f23);
Ld:;
            } else {
                if (v4->f04 & 4)
                    fn_02383598(i, v4->f26);
                if (v4->f04 & 8)
                    fn_023835B0(i, v4->f24 & 0xFF, v4->f24 >> 8);
                if (v4->f04 & 0x10)
                    fn_0238353C(i, v4->f23);
            }
        }
    }
    for (i = 0; i < 16; ++i) {
        v4 = &chans_38075C4[i];
        if (v4->f04 != 0) {
            if (v4->f04 & 1)
                ((unsigned char *)0x4000403)[i * 16] |= 0x80;
            v4->f04 = 0;
        }
    }
}
