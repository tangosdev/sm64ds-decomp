extern void dBgCh_Actr_UpdateDiscreteNoLava_veneer(void *p);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *p);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *p);
extern void _ZN10dBgCh_Actr15ClearLimMovFlagEv(void *p);
extern void _ZN8dActor_c8PoofDustEv(void *p);
extern void _ZN7fBase_c18MarkForDestructionEv(void *p);
extern void _ZN9Animation7AdvanceEv(void *p);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *p, void *cc);
extern void _ZN5dCc_c5ClearEv(void *p);
extern void _ZN5dCc_c6UpdateEv(void *p);
extern void func_02012694(int a, void *b);
extern void func_ov077_02125e94(void *p, int v);
extern void func_ov077_02124eb0(void *p);

int func_ov077_02125908(char *c)
{
    int v;

    dBgCh_Actr_UpdateDiscreteNoLava_veneer(c + 0x1e4);
    if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x1e4) != 0)
    {
        *(int *)(c + 0xa8) = *(int *)(c + 0xa8) * -50 / 100;
    }
    else if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x1e4) != 0)
    {
        *(int *)(c + 0xa8) = 0;
        _ZN10dBgCh_Actr15ClearLimMovFlagEv(c + 0x1e4);
        *(int *)(c + 0x3d4) = 0;
        *(short *)(c + 0x94) = *(short *)(c + 0x8e);
        func_ov077_02125e94(c, 1);
    }

    if (*(int *)(c + 0x3dc) != 0)
        v = *(int *)(c + 0x60) - *(int *)(c + 0x3dc);
    else
        v = 0;

    if (v < -0xc8000)
    {
        _ZN8dActor_c8PoofDustEv(c);
        func_02012694(0x166, c + 0x74);
        _ZN7fBase_c18MarkForDestructionEv(c);
    }

    _ZN9Animation7AdvanceEv(c + 0x174);
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x1b0);
    func_ov077_02124eb0(c);
    _ZN5dCc_c5ClearEv(c + 0x1b0);
    _ZN5dCc_c6UpdateEv(c + 0x1b0);
    return 1;
}
