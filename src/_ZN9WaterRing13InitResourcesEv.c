typedef unsigned int u32;
typedef int s32;
typedef short s16;

struct Actor;
struct BMD_File;
struct BTA_File;

typedef struct Vector3 { int x, y, z; } Vector3;

extern struct BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(char* self, struct BMD_File* f, int a, int b);
extern void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(struct BMD_File* f, struct BTA_File* b);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(char* self, struct BTA_File* b, int a, int fix, u32 f);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(char* self, struct Actor* a, struct Vector3* v, int r, int h, u32 f1, u32 f2);
extern void func_ov064_02119ecc(char* c, void* p);

extern char data_ov002_0210da10[];
extern char data_ov002_0210d6dc[];
extern int data_ov064_0211c3d0[3];
extern char data_ov064_0211c954[];

int _ZN9WaterRing13InitResourcesEv(char* c)
{
    struct BMD_File* f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da10);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x30c, f, 1, -1) == 0)
        return 0;

    *(int*)(c + 0x37c) = *(int*)(c + 8) & 0xff;
    if (*(int*)(c + 0x37c) > 2 || *(int*)(c + 0x37c) == 0xff)
    {
        *(int*)(c + 0x37c) = 0;
    }

    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(*(struct BMD_File**)(data_ov002_0210da10 + 4), (struct BTA_File*)data_ov002_0210d6dc);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0x35c, (struct BTA_File*)data_ov002_0210d6dc, 0, 0x1000, 0);

    *(int*)(c + 0x368) = 0x1000;
    *(int*)(c + 0x80) = 0x1000;
    *(int*)(c + 0x84) = 0x1000;
    *(int*)(c + 0x88) = 0x1000;

    int* src = data_ov064_0211c3d0;
    Vector3 v;
    v.x = src[0];
    v.y = src[1];
    v.z = src[2];
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0x110, (struct Actor*)c, &v, 0x88000, 0xe8000, 0x800006, 0);

    *(char*)(c + 0x380) = 0x1f;
    func_ov064_02119ecc(c, data_ov064_0211c954);
    return 1;
}
