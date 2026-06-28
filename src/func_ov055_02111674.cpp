//cpp
struct M48 { int w[12]; };

extern "C" void *Model_LoadFile(void *fp);
extern "C" void ModelBase_SetFile(void *self, void *bmd, int a, int b);
extern "C" void func_02016acc(void *self, int v);
extern "C" void func_02016b24(void *self, int v);
extern "C" void TextureSequence_Prepare(void *bmd, void *btp);
extern "C" void TextureSequence_SetFile(void *self, void *btp, int a, int fix, unsigned int b);
extern "C" int ShadowModel_InitCylinder(void *self);
extern "C" void *Animation_LoadFile(void *fp);
extern "C" void ModelAnim_SetAnim(void *self, void *bca, int a, int fix, unsigned int b);
extern "C" void Vec3_Asr(void *d, void *s, int sh);
extern "C" void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationY(void *m, int angY);
extern "C" void func_ov055_021112c4(void *c, void *p, int a2);

extern void *data_ov002_0210ebb8;
extern void *data_ov002_0210eb20;
extern void *data_ov002_0210e8d0;
extern void *data_ov002_0210ebd8;
extern void *data_ov002_0210eaa0;
extern int data_020a0e68;
extern unsigned char data_0209f250;
extern int data_0209f394[];
extern void *data_ov055_02111b70;
extern int data_ov055_02111a90;
extern int data_ov055_02111b68;
extern int data_ov055_02111b6c;

extern "C" int func_ov055_02111674(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    int t[3];

    ModelBase_SetFile(c + 0xd4, Model_LoadFile(&data_ov002_0210ebb8), 1, -1);
    func_02016acc(c + 0xd4, 0x80);
    func_02016b24(c + 0xd4, 0x40);

    ModelBase_SetFile(c + 0x138, Model_LoadFile(&data_ov002_0210eb20), 1, -1);
    func_02016acc(c + 0x138, 0x80);
    func_02016b24(c + 0x138, 0x40);

    TextureSequence_Prepare((&data_ov002_0210ebb8)[1], (&data_ov002_0210e8d0)[1]);
    TextureSequence_SetFile(c + 0x1b0, (&data_ov002_0210e8d0)[1], 0, 0x1000, 0);
    TextureSequence_Prepare((&data_ov002_0210eb20)[1], (&data_ov002_0210ebd8)[1]);
    TextureSequence_SetFile(c + 0x1c4, (&data_ov002_0210ebd8)[1], 0, 0x1000, 0);

    ShadowModel_InitCylinder(c + 0x188);

    ModelAnim_SetAnim(c + 0xd4, Animation_LoadFile(&data_ov002_0210eaa0), 0, 0x1000, 0);

    Vec3_Asr(t, c + 0x5c, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t[0], t[1], t[2]);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short *)(c + 0x8e));

    *(M48 *)(c + 0xf0) = *(M48 *)&data_020a0e68;
    *(M48 *)(c + 0x154) = *(M48 *)&data_020a0e68;

    func_ov055_021112c4(c, &data_ov055_02111b70, data_0209f394[data_0209f250]);

    data_ov055_02111b68 = 0;
    data_ov055_02111a90 = 0x1ffff;
    data_ov055_02111b6c = 0;
    return 1;
}
