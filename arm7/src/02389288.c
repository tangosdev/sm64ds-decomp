/* storage 0x02389288 runtime 0x03801120 size 0x1B4 + 24 pool wram ARM.
 * Constant pools (0x40001C0, 0x8A01, 0x40001C2, 0x8201, 0x4000136) + one
 * data reloc (unk_38094D8 = 0x038094D8, LDRH + CMP). BLs to the in-pack
 * thunk fn_02388CD8 (x7) and out-of-pack fn_023B3210. Seven wait sections
 * with per-section p/lo (pool/LDR re-materialized each time); the stores
 * after each wait reuse that section's p (no reload). All paths converge
 * on one epilogue (B / fall-through). */

extern void fn_023B3210(int a1);
extern void fn_02388CD8(int a1);
extern unsigned short unk_38094D8;

int fn_02389288(void) {
    fn_023B3210(0x8000);
    {
        volatile unsigned short *p = (volatile unsigned short *)0x40001C0;
        int lo = 1;
        while ((*p & 0x80) != 0)
            fn_02388CD8(lo);
        *p = 0x8A01;
        *(volatile unsigned short *)0x40001C2 = 132;
    }
    {
        volatile unsigned short *p = (volatile unsigned short *)0x40001C0;
        int lo = 1;
        while ((*p & 0x80) != 0)
            fn_02388CD8(lo);
        *(volatile unsigned short *)0x40001C2 = 0;
    }
    {
        volatile unsigned short *p = (volatile unsigned short *)0x40001C0;
        int lo = 1;
        while ((*p & 0x80) != 0)
            fn_02388CD8(lo);
        *p = 0x8201;
        *(volatile unsigned short *)0x40001C2 = 0;
    }
    {
        volatile unsigned short *p = (volatile unsigned short *)0x40001C0;
        int lo = 1;
        while ((*p & 0x80) != 0)
            fn_02388CD8(lo);
        if (unk_38094D8 == 0)
            return (*(volatile unsigned short *)0x4000136 & 0x40) == 0;
        if ((*(volatile unsigned short *)0x4000136 & 0x40) == 0)
            return 1;
        *p = 0x8A01;
        *(volatile unsigned short *)0x40001C2 = 132;
    }
    {
        volatile unsigned short *p = (volatile unsigned short *)0x40001C0;
        int lo = 1;
        while ((*p & 0x80) != 0)
            fn_02388CD8(lo);
        *(volatile unsigned short *)0x40001C2 = 0;
    }
    {
        volatile unsigned short *p = (volatile unsigned short *)0x40001C0;
        int lo = 1;
        while ((*p & 0x80) != 0)
            fn_02388CD8(lo);
        *p = 0x8201;
        *(volatile unsigned short *)0x40001C2 = 0;
    }
    {
        volatile unsigned short *p = (volatile unsigned short *)0x40001C0;
        int lo = 1;
        while ((*p & 0x80) != 0)
            fn_02388CD8(lo);
        return (*(volatile unsigned short *)0x4000136 & 0x40) ? 0 : 2;
    }
}
