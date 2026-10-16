/* storage 0x02389898 runtime 0x03801730 size 0xB8 + 8 pool wram ARM.
 * Two constant pools (0x040001C0, 0x040001C2), BLs to the in-pack thunk
 * (arg 1) and fn_023899E8. Three wait sections, each with its own pointer
 * and one locals (hence the pool/LDR re-materialization before sections
 * 2 and 3). All loops are while (condition-first B). */

extern void fn_02388CD8(int a1);
extern volatile unsigned short *fn_023899E8(int a1);

int fn_02389898(int a1) {
    int save = a1;
    {
        volatile unsigned short *p = (volatile unsigned short *)0x40001C0;
        int one = 1;
        while ((*p & 0x80) != 0)
            fn_02388CD8(one);
    }
    fn_023899E8(1);
    {
        unsigned char t = save | 0x80;
        *(volatile unsigned short *)0x40001C2 = t & 0xFF;
    }
    {
        volatile unsigned short *p = (volatile unsigned short *)0x40001C0;
        int one = 1;
        while ((*p & 0x80) != 0)
            fn_02388CD8(one);
    }
    fn_023899E8(0);
    *(volatile unsigned short *)0x40001C2 = 0;
    {
        volatile unsigned short *p = (volatile unsigned short *)0x40001C0;
        int one = 1;
        while ((*p & 0x80) != 0)
            fn_02388CD8(one);
    }
    return (unsigned char)(unsigned short)(*(volatile unsigned short *)0x40001C2 & 0xFF);
}
