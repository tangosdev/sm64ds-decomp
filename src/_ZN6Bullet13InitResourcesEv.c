typedef unsigned int u32;
typedef int s32;
typedef short s16;
struct Actor;
struct Vector3_16;
struct BMD_File;
extern struct BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(char* self, struct BMD_File* f, int a, int b);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(char* self, struct Actor* a, int r, int h, u32 f1, u32 f2);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(char* self, struct Actor* a, int r, int h, struct Vector3_16* rot, int f);
extern void func_ov002_020fed2c(char* c, void* p);
extern char func_0211d610[];
extern char data_ov002_02111190[];

int _ZN6Bullet13InitResourcesEv(char* c)
{
    struct BMD_File* f = _ZN5Model8LoadFileER13SharedFilePtr(func_0211d610);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c+0x300, f, 1, -1) == 0) return 0;
    *(int*)(c+0x358) = *(int*)(c+8) & 0xff;
    if (*(int*)(c+0x358) > 1) *(int*)(c+0x358) = 0;
    *(int*)(c+0xa0) = -0x64000;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c+0x110, (struct Actor*)c, 0xa000, 0xa000, 0x200004, 0);
    *(s16*)(c+0x8e) = *(s16*)(c+0x94);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c+0x144, (struct Actor*)c, 0xa000, 0xa000, 0, 0);
    func_ov002_020fed2c(c, data_ov002_02111190);
    return 1;
}
