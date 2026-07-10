typedef struct { int x, y, z; } Vector3;

extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void *f);
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void *f);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *bmd, void *btp);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *btp, int a, int b, unsigned int d);
extern void _ZN9Animation8SetFlagsEi(void *self, int flags);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *self, void *act, void *pos, int c3, int d, unsigned int e, unsigned int f);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *act, int a, int b, void *d1, void *d2);
extern void _ZN12WithMeshClsn13SetLimMovFlagEv(void *self);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, void *pos, void *dir, int e, int f);
extern void func_ov060_02111cc0(void *c, int a, int b);
extern void func_ov060_021123dc(void *c);
extern void func_02011d50(void *a);

extern int data_ov060_0211ac78[];
extern int data_ov060_021192dc[];
extern int data_ov060_0211927c[];
extern int data_ov060_0211b208[];
extern int data_ov089_02132c50[];
extern int data_ov060_0211ac28[];

int _ZN6Bowser13InitResourcesEv(char *c)
{
    int i;
    Vector3 pos;
    void *a1;
    void *a2;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4,
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov060_0211ac78), 1, 0x16);

    for (i = 0; i < 0x1c; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr((void *)data_ov060_021192dc[i]);

    for (i = 0; i < 6; i++)
        _ZN15TextureSequence8LoadFileER13SharedFilePtr((void *)data_ov060_0211927c[i]);

    _ZN5Model8LoadFileER13SharedFilePtr(data_ov060_0211b208);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov089_02132c50);

    if (_ZN11ShadowModel12InitCylinderEv(c + 0x308) == 0)
        return 0;

    func_ov060_02111cc0(c, 0x10, 0);

    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(
        (void *)data_ov060_0211ac78[1], (void *)data_ov060_0211ac28[1]);

    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        c + 0x138, (void *)data_ov060_0211ac28[1], 0, 0x1000, 0);

    _ZN9Animation8SetFlagsEi(c + 0x138, 0x40000000);

    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        c + 0x360, c, &pos, 0x78000, 0x118000, 0x200004, 0);

    *(int *)(c + 0x3b0) = *(int *)(c + 0x5c);
    *(int *)(c + 0x3b4) = *(int *)(c + 0x60);
    *(int *)(c + 0x3b8) = *(int *)(c + 0x64);
    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa0) = -0x3c000;
    *(int *)(c + 0x3a0) = 0;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
        c + 0x14c, c, 0x50000, 0x50000, 0, 0);
    _ZN12WithMeshClsn13SetLimMovFlagEv(c + 0x14c);

    *(int *)(c + 0x40c) = 0;
    *(char *)(c + 0x414) = (char)(*(int *)(c + 8) & 3);
    *(char *)(c + 0x416) = (char)((*(unsigned int *)(c + 8) >> 2) & 1);
    *(short *)(c + 0x3fc) = 0;
    *(char *)(c + 0x423) = 0;
    *(char *)(c + 0x426) = 1;
    *(char *)(c + 0x427) = 0;
    *(int *)(c + 0x80) = 0x1000;
    *(int *)(c + 0x84) = 0x1000;
    *(int *)(c + 0x88) = 0x1000;
    *(int *)(c + 0x3f8) = 0x1000;
    *(char *)(c + 0x429) = 1;
    func_ov060_021123dc(c);

    *(char *)(c + 0x424) = 0;
    *(char *)(c + 0x444) = 0;

    a1 = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        0x118, 0, (void *)(c + 0x5c), 0, (signed char)c[0xcc], -1);
    *(int *)((char *)a1 + 0x2cc) = *(int *)(c + 4);

    a2 = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        0x116, 0, (void *)(c + 0x5c), 0, (signed char)c[0xcc], -1);
    *(int *)(c + 0x3a8) = *(int *)((char *)a2 + 4);
    *(int *)((char *)a2 + 0x108) = *(int *)(c + 4);
    *(char *)(c + 0x42a) = 5;
    *(char *)(c + 0x42b) = 0;
    *(int *)(c + 0x448) = 0;
    *(char *)(c + 0x446) = 0;
    *(int *)(c + 0x44c) = 0;
    *(int *)(c + 0x450) = 0;
    func_02011d50(a2);
    return 1;
}
