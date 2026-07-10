//cpp
typedef int Fix12;
struct Vector3 { int x, y, z; };

extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *ref);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *base, void *file, int a, int b);
extern void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *ref);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *bmd, void *btp);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *t, void *a, Fix12 b, Fix12 c, unsigned int d, unsigned int e);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *t, void *a, Fix12 b, Fix12 c, void *d, void *e);
extern void _ZN13RaycastGroundC1Ev(void *t);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *t, const struct Vector3 *pos, void *actor);
extern int _ZN13RaycastGround10DetectClsnEv(void *t);
extern void func_ov072_021205d4(void *c, int v);
extern void func_ov072_0211ffd8(char *c);
extern void _ZN13RaycastGroundD1Ev(void *t);

extern void *data_ov072_02122bc4;
extern void *data_ov072_02121ffc;
}

extern "C" int _ZN11SnowmanHead13InitResourcesEv(char *c) {
    struct Vector3 pos;
    char ray[0x54];
    int i;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4,
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov072_02122bc4), 1, -1);

    for (i = 0; i < 2; i++) {
        void *tex = (&data_ov072_02121ffc)[i];
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(tex);
        _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(
            ((void **)&data_ov072_02122bc4)[1], ((void **)tex)[1]);
    }

    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x138, c, 0x96000, 0x12c000, 0x800004, 0);
    *(int *)(c + 0x80) = 0x1800;
    *(int *)(c + 0x84) = 0x1800;
    *(int *)(c + 0x88) = 0x1800;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x16c, c, 0x96000, 0x96000, 0, 0);

    pos.x = *(int *)(c + 0x5c);
    pos.y = *(int *)(c + 0x60);
    pos.z = *(int *)(c + 0x64);
    pos.y += 0x14000;
    _ZN13RaycastGroundC1Ev(ray);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(ray, &pos, 0);
    if (_ZN13RaycastGround10DetectClsnEv(ray) != 0)
        *(int *)(c + 0x60) = *(int *)(ray + 0x44);
    else
        *(int *)(c + 0x60) = pos.y;

    func_ov072_021205d4(c, 0);
    func_ov072_0211ffd8(c);
    _ZN13RaycastGroundD1Ev(ray);
    return 1;
}
