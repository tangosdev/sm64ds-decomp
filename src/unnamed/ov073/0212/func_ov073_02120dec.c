extern void _Z14ApproachLinearRsss(short *, short, short);
extern int AngleDiff(short, short);
extern int ChiefChilly_ChangeState(void *c, void *p);
extern int data_ov073_02123360[];
int func_ov073_02120dec(char *c) {
    _Z14ApproachLinearRsss((short *)(c + 0x94), *(short *)(c + 0x4c6), 0x500);
    if (AngleDiff(*(short *)(c + 0x4c6), *(short *)(c + 0x8e)) < 0x100) {
        ChiefChilly_ChangeState(c, data_ov073_02123360);
        *(short *)(c + 0x100) = 0x1e;
    }
    *(short *)(c + 0x8c) = *(short *)(c + 0x92);
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(short *)(c + 0x90) = *(short *)(c + 0x96);
    return 1;
}
