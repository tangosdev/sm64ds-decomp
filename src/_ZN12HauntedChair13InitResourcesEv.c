typedef unsigned int u32;
struct Actor; struct Vector3; struct Vector3_16; struct BMD_File;
extern struct BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(char* self, struct BMD_File* f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(char* self);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(char* self, struct Actor* a, int r, int h, struct Vector3_16* rot, int f);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(char* self, struct Actor* a, struct Vector3* pos, int r, int h, u32 f1, u32 f2);
extern char data_ov020_02114af0[];
struct M48 { int w[12]; };
extern struct M48 data_02082128;

int _ZN12HauntedChair13InitResourcesEv(char* c)
{
    struct BMD_File* f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov020_02114af0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, -1);
    _ZN11ShadowModel12InitCylinderEv(c+0x124);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c+0x1bc, (struct Actor*)c, 0x32000, 0x64000, 0, 0);
    *(int*)(c+0x38c) = 0;
    *(int*)(c+0x390) = 0;
    *(int*)(c+0x394) = 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c+0x17c, (struct Actor*)c, (struct Vector3*)(c+0x38c), 0x32000, 0x64000, 0x200004, 0);
    *(int*)(c+0x378) = 0;
    *(int*)(c+0x380) = *(int*)(c+0x5c);
    *(int*)(c+0x384) = *(int*)(c+0x60);
    *(int*)(c+0x388) = *(int*)(c+0x64);
    *(struct M48*)(c+0x14c) = data_02082128;
    return 1;
}
