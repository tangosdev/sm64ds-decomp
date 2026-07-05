//cpp
extern "C" void *Model_LoadFile(void *fp);
extern "C" void ModelBase_SetFile(void *self, void *bmd, int a, int b);
extern "C" void *Animation_LoadFile(void *fp);
extern "C" void *TextureSequence_LoadFile(void *fp);
extern "C" void TextureSequence_Prepare(void *bmd, void *btp);
extern "C" int ShadowModel_InitCylinder(void *self);
extern "C" unsigned char NumStars(void);
extern "C" void MovingCylinderClsn_Init(void *self, void *actor, int fix, int t, unsigned int a, unsigned int b);
extern "C" void WithMeshClsn_Init(void *self, void *actor, int fix, int t, void *v16, int u);
extern "C" void Actor_SetRanges(void *self, int a, int b, int cc, int d);
extern "C" unsigned char Actor_TrackStar(void *self, unsigned int a, unsigned int b);
extern "C" void PathPtr_FromID(void *self, unsigned int id);
extern "C" void PathPtr_GetNode(void *self, void *v, unsigned int j);
extern "C" void func_ov019_021114ec(void *c);
extern "C" void func_ov019_021122dc(void *c, int i);
extern "C" void func_ov019_021113b0(void *c);

extern void *data_ov019_02113498;
extern void *data_ov019_02112788[];
extern void *data_ov019_0211277c[];

extern "C" int func_ov019_021123d4(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    int i;

    ModelBase_SetFile(c + 0xd4, Model_LoadFile(&data_ov019_02113498), 1, 1);

    for (i = 0; i < 7; i++)
        Animation_LoadFile(data_ov019_02112788[i]);

    for (int j = 0; j < 3; j++) {
        void *t = data_ov019_0211277c[j];
        TextureSequence_LoadFile(t);
        TextureSequence_Prepare((&data_ov019_02113498)[1], *(void **)((char *)t + 4));
    }

    if (ShadowModel_InitCylinder(c + 0x14c) == 0)
        return 0;

    if (NumStars() == 0x96) {
        MovingCylinderClsn_Init(c + 0x174, c, 0xd0000, 0x12c000, 0x800004, 0);
        *(int *)(c + 0x80) = 0x1999;
        *(int *)(c + 0x84) = 0x1000;
        *(int *)(c + 0x88) = 0x1999;
        WithMeshClsn_Init(c + 0x1a8, c, 0xd0000, 0xd0000, 0, 0);
        Actor_SetRanges(c, 0xf0000, 0xf0000, 0x1c20000, 0x1c20000);
    } else {
        MovingCylinderClsn_Init(c + 0x174, c, 0x82000, 0x12c000, 0x800004, 0);
        *(int *)(c + 0x80) = 0x1000;
        *(int *)(c + 0x84) = 0x1000;
        *(int *)(c + 0x88) = 0x1000;
        WithMeshClsn_Init(c + 0x1a8, c, 0x82000, 0x82000, 0, 0);
    }

    *(unsigned char *)(c + 0x396) =
        Actor_TrackStar(c, (unsigned char)((*(unsigned int *)(c + 8) >> 8) & 0xf), 2);
    func_ov019_021122dc(c, 0);
    PathPtr_FromID(c + 0x364, *(unsigned int *)(c + 8) & 0xff);
    *(int *)(c + 0x36c) = 0;
    PathPtr_GetNode(c + 0x364, c + 0x5c, *(unsigned int *)(c + 0x36c));
    func_ov019_021113b0(c);
    func_ov019_021114ec(c);
    return 1;
}
