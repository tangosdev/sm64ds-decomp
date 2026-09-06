/* Pure C version — angle reset only on IsOnGround path (not JustHitGround). */
extern void func_02038414(void *p);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *w);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *w);
extern void _ZN10dBgCh_Actr15ClearLimMovFlagEv(void *w);
extern void func_ov081_021265c8(void *c);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *a, void *clsn);
extern void func_ov081_02126758(void *c);
extern void _ZN5dCc_c5ClearEv(void *cl);
extern void _ZN5dCc_c6UpdateEv(void *cl);

int func_ov081_02126d64(char *s)
{
    func_02038414(s + 0x1e4);
    *(short *)(s + 0x8c) = *(short *)(s + 0x8c) + 0x1000;
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(s + 0x1e4) != 0) {
        *(int *)(s + 0xa8) = *(int *)(s + 0xa8) * -0x3c / 100;
    } else if (_ZNK10dBgCh_Actr10IsOnGroundEv(s + 0x1e4) != 0) {
        *(int *)(s + 0xa8) = 0;
        _ZN10dBgCh_Actr15ClearLimMovFlagEv(s + 0x1e4);
        {
            short v94 = *(short *)(s + 0x94);
            *(short *)(s + 0x8c) = 0;
            *(short *)(s + 0x8e) = v94;
            *(short *)(s + 0x90) = 0;
            func_ov081_021265c8(s);
        }
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(s, s + 0x1b0);
    func_ov081_02126758(s);
    _ZN5dCc_c5ClearEv(s + 0x1b0);
    _ZN5dCc_c6UpdateEv(s + 0x1b0);
    return 1;
}
