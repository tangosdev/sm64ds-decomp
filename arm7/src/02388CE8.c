/* storage 0x02388CE8 runtime 0x03800B80 linked 0x25C wram ARM.
 * Touchscreen read routine. *a1 is a packed control word (X sample in bits
 * 0-11, Y sample in bits 12-23, bit 24, mode field in bits 25-26). Programs
 * the TSC registers at 0x40001C0/0x40001C2, samples 12 times, then updates
 * the mode bits and the unk_38094D8 flag from fn_02389288()'s state. */

extern unsigned short unk_38094D8;
extern int fn_02389288(void);
extern void fn_02388CD8(int);
extern int fn_02388F44(unsigned short *out, int a2, int a3, int sel);
extern void fn_02382B38(void);

volatile short *fn_02388CE8(int *a1, int a2, int a3) {
    unsigned short v16[4];
    int v4 = a2;
    if (a2 < 0) v4 = -a2;
    int v6 = fn_02389288();
    if (v6 == 0) {
        *a1 &= 0xFFFFF000;
        *a1 &= 0xFF000FFF;
        *a1 &= ~0x1000000;
        *a1 = *a1 & 0xF9FFFFFF | 0x6000000;
        unk_38094D8 = 0;
        return (volatile short *)&unk_38094D8;
    } else {
        *a1 = *a1 & 0xF9FFFFFF | ((fn_02388F44(v16, v4, a3, 0) & 3) << 25);
        *a1 = *a1 & 0xFFFFF000 | (v16[0] & 0xFFF);
        if (fn_02388F44(v16, v4, a3, 1) == 2) {
            struct { unsigned _pad:25; unsigned mode:2; unsigned _r:5; } *s =
                (void *)a1;
            if (s->mode == 1)
                s->mode = 3;
            else
                s->mode = 2;
        }
        *a1 = *a1 & 0xFF000FFF | ((v16[0] & 0xFFF) << 12);
        *(volatile unsigned short *)0x40001C0 = 0x8A01;
        {
            int i = 0;
            do {
                *(volatile unsigned short *)0x40001C2 = 0;
                while (*(volatile unsigned short *)0x40001C0 & 0x80)
                    fn_02388CD8(1);
                i++;
            } while (i < 12);
        }
        *(volatile unsigned short *)0x40001C0 = 0x8201;
        *(volatile unsigned short *)0x40001C2 = 0;
        while (*(volatile unsigned short *)0x40001C0 & 0x80)
            fn_02388CD8(1);
        if (v6 == 2)
            *a1 = *a1 & 0xF9FFFFFF | 0x6000000;
        {
            int v15 = fn_02389288();
            switch (v15) {
            case 2:
                *a1 |= 0x1000000;
                *a1 = *a1 & 0xF9FFFFFF | 0x6000000;
                unk_38094D8 = 0;
                return (volatile short *)&unk_38094D8;
            case 1:
                *a1 |= 0x1000000;
                unk_38094D8 = 1;
                return (volatile short *)&unk_38094D8;
            case 0:
                *a1 &= ~0x1000000;
                unk_38094D8 = 0;
                return (volatile short *)&unk_38094D8;
            default:
                fn_02382B38();
            }
        }
    }
}
