/* candidate for func_ov066_02118188 */
extern void func_ov066_021166c8(void *thiz);
extern void func_ov066_02116ac4(void *thiz, int a1);
extern void func_ov066_021165cc(void *thiz);
extern void func_02012694(int size, void *p);
extern void func_ov066_021164ec(void *thiz);
extern int func_ov066_02116390(void *thiz);
extern int func_ov066_0211603c(void *thiz);
extern int _ZN9Animation8FinishedEv(void *self);
extern void func_ov066_021162e8(void *thiz);
extern int _ZN16MeshColliderBase9IsEnabledEv(void *self);
extern void _ZN16MeshColliderBase7DisableEv(void *self);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int fix, short s, void *clps);
extern void func_020393d4(void *p, void *v);
extern void func_020393c4(void *p, void *v);
extern void func_020398fc(void *p);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void *self, void *actor);
extern int func_ov066_02119454(void *c, void *pmf);

extern unsigned char data_ov066_0211ae0c;
extern unsigned char data_ov066_0211ae08;
extern unsigned char data_ov066_0211abe0;
extern char data_ov066_0211ae14[];
extern char data_ov066_0211aeac[];
extern unsigned char data_ov066_0211ae04;
extern char data_ov066_0211b06c;
extern int func_02112c08;
extern int func_02112d48;
extern int _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
extern int func_ov066_0211a35c;

int func_ov066_02118188(void *thiz)
{
    char *c = (char *)thiz;

    switch (*(int *)(c + 0x4a0)) {
    case 0:
        if (*(unsigned short *)(c + 0x4d0) != 0)
            break;
        if (data_ov066_0211ae0c == *(int *)(c + 0x49c)) {
            *(int *)(c + 0x9c) = -0xa000;
            *(int *)(c + 0xa8) = 0x64000;
            *(int *)(c + 0xb0) = 0x2000000;
            *(int *)(c + 0x4a0) = 1;
        } else {
            func_ov066_021166c8(c);
            *(int *)(c + 0x4a0) = 2;
        }
        break;

    case 1:
        if (*(int *)(c + 0x9c) == 0)
            break;
        if (*(int *)(c + 0x4a8) < *(int *)(c + 0x60))
            break;
        *(int *)(c + 0x60) = *(int *)(c + 0x4a8);
        *(int *)(c + 0xa8) = 0;
        *(int *)(c + 0x9c) = 0;
        func_ov066_02116ac4(thiz, 0x7d0000);
        data_ov066_0211ae08 += 1;
        if (data_ov066_0211abe0 != 3)
            data_ov066_0211ae08 += 1;
        *(int *)(c + 0x4a0) = 5;
        break;

    case 2:
        if (*(int *)(c + 0x498) == 1) {
            if (data_ov066_0211ae08 != 0) {
                if (*(int *)(c + 0x494) > 0x14) {
                    *(int *)(c + 0x494) = 0;
                    func_ov066_021165cc(c);
                    *(int *)(c + 0x4a0) = 4;
                    break;
                }
                *(int *)(((int)c + 0x494) & 0xFFFFFFFFFFFFFFFF) += 1;
            }
        }

        if ((unsigned short)(*(int *)(c + 0x3b8) >> 0xc) == 0) {
            func_02012694(0x140, c + 0x74);
        }

        func_ov066_021164ec(c);

        if (*(int *)(c + 0x498) != 1)
            break;

        func_ov066_02116390(c);
        {
            int r = func_ov066_0211603c(c);
            if (r == 0)
                break;
            if (r == 1) {
                *(int *)(c + 0x4a0) = 3;
            }
        }
        break;

    case 3:
        if (_ZN9Animation8FinishedEv(c + 0x3b0) == 0)
            break;
        func_ov066_021165cc(c);
        *(int *)(c + 0x4a0) = 4;
        break;

    case 4:
        if (_ZN9Animation8FinishedEv(c + 0x3b0) == 0)
            break;
        func_ov066_021162e8(c);
        if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x674) != 0)
            _ZN16MeshColliderBase7DisableEv(c + 0x674);

        if (*(int *)(c + 0x49c) == 1) {
            _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                c + 0x674, *(void **)(data_ov066_0211ae14 + 4), c + 0x83c, 0x199,
                *(short *)(c + 0x8e), &func_02112c08);
        } else {
            _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                c + 0x674, *(void **)(data_ov066_0211aeac + 4), c + 0x83c, 0x199,
                *(short *)(c + 0x8e), &func_02112d48);
        }

        func_020393d4(c + 0x674, &_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
        func_020393c4(c + 0x674, &func_ov066_0211a35c);
        func_020398fc(c + 0x674);
        _ZN16MeshColliderBase6EnableEP5Actor(c + 0x674, c);

        data_ov066_0211ae08 += 1;
        *(int *)(c + 0x4a0) = 5;
        break;

    case 5:
        if (data_ov066_0211ae04 == 5)
            break;
        *(int *)(c + 0xb0) = 0;
        func_ov066_02119454(c, &data_ov066_0211b06c);
        break;
    }

    return 1;
}
