typedef unsigned int u32;
struct Vector3 { int x, y, z; };
struct Actor; struct Vector3_16; struct BMD_File;
extern struct BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(char* self, struct BMD_File* f, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(char* self);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(char* self, struct Actor* a, struct Vector3* pos, int r, int h, u32 f1, u32 f2);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(char* self, struct Actor* a, int r, int h, struct Vector3_16* rot, int f);
extern char data_ov079_02128300[];
extern char data_ov079_021282f0[];

int func_ov079_02126c44(char* c)
{
    struct BMD_File* f1 = _ZN5Model8LoadFileER13SharedFilePtr(data_ov079_02128300);
    struct BMD_File* f2 = _ZN5Model8LoadFileER13SharedFilePtr(data_ov079_021282f0);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c+0x30c, *(struct BMD_File**)(data_ov079_02128300+4), 1, -1) == 0)
        return 0;
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c+0x35c, *(struct BMD_File**)(data_ov079_021282f0+4), 1, -1) == 0)
        return 0;
    if (_ZN11ShadowModel12InitCylinderEv(c+0x3ac) == 0)
        return 0;
    {
        struct Vector3 v;
        v.x = 0; v.y = -0x50000; v.z = 0;
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c+0x110, (struct Actor*)c, &v, 0x78000, 0xa0000, 0x200005, 0x3c0);
    }
    *(int*)(c+0x3d4) = 0;
    *(short*)(c+0x100) = 0;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c+0x150, (struct Actor*)c, 0x32000, 0, 0, 0);
    *(int*)(c+0x3d8) = 0;
    return 1;
}
