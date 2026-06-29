extern void func_02038420(void *p);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(void *p);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *p);
extern void _ZN12WithMeshClsn15ClearLimMovFlagEv(void *p);
extern void _ZN5Actor8PoofDustEv(void *p);
extern void _ZN9ActorBase18MarkForDestructionEv(void *p);
extern void _ZN9Animation7AdvanceEv(void *p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *p, void *cc);
extern void _ZN12CylinderClsn5ClearEv(void *p);
extern void _ZN12CylinderClsn6UpdateEv(void *p);
extern void func_02012694(int a, void *b);
extern void func_ov077_02125e94(void *p, int v);
extern void func_ov077_02124eb0(void *p);

int func_ov077_02125908(char *c)
{
    int v;

    func_02038420(c + 0x1e4);
    if (_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x1e4) != 0)
    {
        *(int *)(c + 0xa8) = *(int *)(c + 0xa8) * -50 / 100;
    }
    else if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x1e4) != 0)
    {
        *(int *)(c + 0xa8) = 0;
        _ZN12WithMeshClsn15ClearLimMovFlagEv(c + 0x1e4);
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
        _ZN5Actor8PoofDustEv(c);
        func_02012694(0x166, c + 0x74);
        _ZN9ActorBase18MarkForDestructionEv(c);
    }

    _ZN9Animation7AdvanceEv(c + 0x174);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x1b0);
    func_ov077_02124eb0(c);
    _ZN12CylinderClsn5ClearEv(c + 0x1b0);
    _ZN12CylinderClsn6UpdateEv(c + 0x1b0);
    return 1;
}
