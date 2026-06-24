//cpp
struct CylinderClsn;

extern "C" void func_020383fc(void *p);
extern "C" int _ZNK12WithMeshClsn13JustHitGroundEv(void *thiz);
extern "C" int _ZNK12WithMeshClsn10IsOnGroundEv(void *thiz);
extern "C" void _ZN12WithMeshClsn15ClearLimMovFlagEv(void *thiz);
extern "C" void func_ov072_02121d50(void *thiz, int a);
extern "C" void _ZN5Actor9UpdatePosEP12CylinderClsn(void *thiz, struct CylinderClsn *cc);
extern "C" void func_ov072_02120e50(void *thiz);
extern "C" void _ZN12CylinderClsn5ClearEv(void *thiz);
extern "C" void _ZN12CylinderClsn6UpdateEv(void *thiz);

extern "C" int func_ov072_021212c0(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    func_020383fc(c + 0x194);
    if (_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x194)) {
        *(int *)(c + 0xa8) = (*(int *)(c + 0xa8) * -0x28) / 100;
    } else if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x194)) {
        *(int *)(c + 0xa8) = 0;
        _ZN12WithMeshClsn15ClearLimMovFlagEv(c + 0x194);
        func_ov072_02121d50(c, 0);
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, (struct CylinderClsn *)(c + 0x160));
    func_ov072_02120e50(c);
    _ZN12CylinderClsn5ClearEv(c + 0x160);
    _ZN12CylinderClsn6UpdateEv(c + 0x160);
    return 1;
}
