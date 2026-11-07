extern char *unk_380FFF4;
extern unsigned short half_4808004;
extern int fn_023951A4(void *block);
extern void fn_02395160(void);
extern void fn_0239F228(void);
extern int fn_02395114(void *block);
extern void fn_0239E8C8(unsigned short value);

int fn_02395008(void) {
    char *base;
    char *blk;
    unsigned short mode;

    base = unk_380FFF4;
    blk = base + 0x3E4;
    mode = *(unsigned short *)(base + 0x314);
    switch (mode) {
    case 1:
        if (fn_023951A4(blk + 0x78) != 0) {
            fn_02395160();
            fn_0239F228();
        }
        if (fn_023951A4(blk + 0x28) != 0)
            fn_02395114(blk + 0x28);
        break;
    case 2:
        fn_023951A4(blk + 0x64);
        if (fn_023951A4(blk + 0x28) != 0) {
            if (*(unsigned short *)(blk + 0x28) != 0)
                fn_02395160();
            base = unk_380FFF4;
            fn_0239E8C8(*(unsigned short *)(base + 0x366));
            base = unk_380FFF4;
            {
                unsigned short *counter = (unsigned short *)(base + 0x3B4);
                *counter = (unsigned short)(*counter + 1);
            }
        }
        break;
    case 3:
        break;
    }
    if (fn_023951A4(blk + 0x14) != 0)
        fn_02395114(blk + 0x14);
    if (fn_023951A4(blk) != 0)
        fn_02395114(blk);
    {
        unsigned short flag = half_4808004;
        if (flag == 0) {
            flag = 1;
            half_4808004 = flag;
        }
        return flag;
    }
}
