//cpp
struct dCc_c;

extern "C" void dBgCh_Actr_UpdateContinuous_Veneer(void *p);
extern "C" int _ZNK10dBgCh_Actr13JustHitGroundEv(void *thiz);
extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(void *thiz);
extern "C" void _ZN10dBgCh_Actr15ClearLimMovFlagEv(void *thiz);
extern "C" void func_ov072_02121d50(void *thiz, int a);
extern "C" void _ZN8dActor_c9UpdatePosEP5dCc_c(void *thiz, struct dCc_c *cc);
extern "C" void func_ov072_02120e50(void *thiz);
extern "C" void _ZN5dCc_c5ClearEv(void *thiz);
extern "C" void _ZN5dCc_c6UpdateEv(void *thiz);

extern "C" int func_ov072_021212c0(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    dBgCh_Actr_UpdateContinuous_Veneer(c + 0x194);
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x194)) {
        *(int *)(c + 0xa8) = (*(int *)(c + 0xa8) * -0x28) / 100;
    } else if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x194)) {
        *(int *)(c + 0xa8) = 0;
        _ZN10dBgCh_Actr15ClearLimMovFlagEv(c + 0x194);
        func_ov072_02121d50(c, 0);
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, (struct dCc_c *)(c + 0x160));
    func_ov072_02120e50(c);
    _ZN5dCc_c5ClearEv(c + 0x160);
    _ZN5dCc_c6UpdateEv(c + 0x160);
    return 1;
}
