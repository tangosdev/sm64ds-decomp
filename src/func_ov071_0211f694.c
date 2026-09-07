extern int _ZN9Animation7AdvanceEv(char *a);
extern int _ZN8dActor_c9UpdatePosEP5dCc_c(char *t, char *c);
extern int dBgCh_Actr_UpdateDiscreteNoLava_veneer(char *a);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(char *a);
extern int func_ov071_0211f498(char *t);
int func_ov071_0211f694(char *t)
{
    *(short *)(t + 0x8c) = *(short *)(t + 0x8c) - 0x1000;
    _ZN9Animation7AdvanceEv(t + 0x124);
    _ZN8dActor_c9UpdatePosEP5dCc_c(t, t + 0x160);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(t + 0x194);
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(t + 0x194) != 0 || *(unsigned short *)(t + 0x3a8) == 0)
        func_ov071_0211f498(t);
    return 1;
}
