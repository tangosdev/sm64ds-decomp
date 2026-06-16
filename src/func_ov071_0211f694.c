extern int _ZN9Animation7AdvanceEv(char *a);
extern int _ZN5Actor9UpdatePosEP12CylinderClsn(char *t, char *c);
extern int func_02038420(char *a);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(char *a);
extern int func_ov071_0211f498(char *t);
int func_ov071_0211f694(char *t)
{
    *(short *)(t + 0x8c) = *(short *)(t + 0x8c) - 0x1000;
    _ZN9Animation7AdvanceEv(t + 0x124);
    _ZN5Actor9UpdatePosEP12CylinderClsn(t, t + 0x160);
    func_02038420(t + 0x194);
    if (_ZNK12WithMeshClsn13JustHitGroundEv(t + 0x194) != 0 || *(unsigned short *)(t + 0x3a8) == 0)
        func_ov071_0211f498(t);
    return 1;
}
