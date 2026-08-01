//cpp
extern "C" void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *bca, int a, int b, int fix, unsigned short t);
extern "C" void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *btp, int a, int fix, unsigned int b);
extern "C" int _ZN16MeshColliderBase9IsEnabledEv(void *self);
extern "C" void _ZN16MeshColliderBase7DisableEv(void *self);
extern "C" void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *self, void *kcl, void *mtx, int fix, short s, void *clps);
extern "C" void func_020393d4(void *p, void *v);
extern "C" void func_020393c4(void *p, void *v);
extern "C" void func_020398fc(void *p);
extern "C" void _ZN16MeshColliderBase6EnableEP5Actor(void *self, void *actor);
extern "C" int _ZNK9Animation13GetFrameCountEv(void *self);

extern void **data_ov066_0211ae54;
extern void **data_ov066_0211ae2c;
extern void **data_ov066_0211ae94;
extern void **data_ov066_0211ae9c;
extern void **data_ov066_0211ae34;
extern void **data_ov066_0211ae1c;
extern void func_02112cc8();
extern void func_02112c88();
extern void func_02112cd8_updatepos();
extern void func_ov066_0211a35c();

extern "C" void func_ov066_021166c8(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;

    if (*(int *)(c + 0x49c) == 2) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x360, (&data_ov066_0211ae54)[1], 4, 0x40000000, 0x1000, 0);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x448, (&data_ov066_0211ae2c)[1], 0x40000000, 0x1000, 0);
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x360, (&data_ov066_0211ae94)[1], 4, 0x40000000, 0x1000, 0);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x448, (&data_ov066_0211ae9c)[1], 0x40000000, 0x1000, 0);
    }

    if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x674) != 0)
        _ZN16MeshColliderBase7DisableEv(c + 0x674);

    if (*(int *)(c + 0x49c) == 1) {
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c + 0x674, (&data_ov066_0211ae34)[1], c + 0x83c, 0x199,
                                   *(short *)(c + 0x8e), (void *)func_02112cc8);
    } else {
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c + 0x674, (&data_ov066_0211ae1c)[1], c + 0x83c, 0x199,
                                   *(short *)(c + 0x8e), (void *)func_02112c88);
    }

    func_020393d4(c + 0x674, (void *)func_02112cd8_updatepos);
    func_020393c4(c + 0x674, (void *)func_ov066_0211a35c);
    func_020398fc(c + 0x674);
    _ZN16MeshColliderBase6EnableEP5Actor(c + 0x674, c);

    {
        int n = _ZNK9Animation13GetFrameCountEv(c + 0x3b0);
        *(int *)(c + 0x3b8) = (int)(((unsigned int)((n - 1) << 0x10)) >> 4);
        *(int *)(c + 0x3bc) = -0x1000;
    }
}
