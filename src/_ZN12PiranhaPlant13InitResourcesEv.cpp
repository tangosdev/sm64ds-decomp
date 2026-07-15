//cpp
typedef int Fix12;
typedef short s16;
struct Vector3 { int x, y, z; };
struct SharedFilePtr { int id; void* file; };
extern "C" {
void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
void* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
void LoadBlueCoinModel(void* c);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, void* a, Fix12 r, Fix12 h, unsigned int e, unsigned int g);
void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void* self, void* a, const Vector3* v, Fix12 r, Fix12 h, unsigned int e, unsigned int g);
void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, void* a, Fix12 b, Fix12 cc, void* d, void* e);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* f, int a, Fix12 b, unsigned int cc);
extern SharedFilePtr* data_ov084_021302f4[];
extern SharedFilePtr data_ov084_02130dfc;
extern SharedFilePtr data_ov002_0210da38;
extern SharedFilePtr data_ov084_02130df4;
extern s16 data_02082214[];
}
extern "C" int _ZN12PiranhaPlant13InitResourcesEv(char* c)
{
    int i;
    Vector3 v;
    for (i = 0; i < 6; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov084_021302f4[i]);
    LoadBlueCoinModel(c);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov084_02130dfc);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da38);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x110, data_ov084_02130dfc.file, 1, -1) == 0)
        return 0;
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x174, data_ov002_0210da38.file, 1, -1) == 0)
        return 0;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x380, c, 0x32000, 0x64000, 0x200004, 0x423e0);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x3b4, c, 0x82000, 0x64000, 2, 0x423e0);
    v.x = 0; v.y = 0; v.z = 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0x3e8, c, &v, 0x46000, 0x64000, 0x200002, 0);
    *(int*)(c + 0x80) = 0x1000;
    *(int*)(c + 0x84) = 0x1000;
    *(int*)(c + 0x88) = 0x1000;
    *(int*)(c + 0x458) = 0;
    *(short*)(c + 0x468) = *(short*)(c + 0x94);
    *(int*)(c + 0x464) = 0x7fffffff;
    *(int*)(c + 0x460) = 0;
    *(unsigned char*)(c + 0x45c) = 0;
    *(unsigned char*)(c + 0x45d) = 1;
    *(short*)(c + 0x100) = 0;
    *(unsigned char*)(c + 0x108) = 3;
    *(int*)(c + 0x46c) = 0;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x1c4, c, 0x64000, 0x64000, 0, 0);
    *(int*)(c + 0x440) = *(int*)(c + 0x5c);
    *(int*)(c + 0x444) = *(int*)(c + 0x60);
    *(int*)(c + 0x448) = *(int*)(c + 0x64);
    {
        s16 *tbl = data_02082214;
        Vector3* home = (Vector3*)(((int)c + 0x440) & 0xFFFFFFFFFFFFFFFFLL);
        *(Vector3*)(c + 0x44c) = *home;
        unsigned short angh = *(unsigned short*)(c + 0x8e);
        int ang = angh >> 4;
        int y0 = *(int*)(c + 0x60);
        int z0 = *(int*)(c + 0x64);
        int cosv = tbl[(ang << 1) + 1];
        int sinv = tbl[ang << 1];
        int z = cosv * 0xe0 + z0;
        int y = y0 + 0x37800;
        int x = sinv * 0xe0 + *(int*)(c + 0x5c);
        *(int*)(c + 0x434) = x;
        *(int*)(c + 0x438) = y;
        *(int*)(c + 0x43c) = z;
    }
    *(int*)(c + 0x474) = 0;
    *(int*)(c + 0x470) = *(int*)(c + 0x474);
    *(int*)(c + 0x478) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, data_ov084_02130df4.file, 0, 0x1000, 0);
    return 1;
}
