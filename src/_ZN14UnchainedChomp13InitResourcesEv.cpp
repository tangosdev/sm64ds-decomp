//cpp
struct SharedFilePtr;
struct BMD_File;
struct Actor;
struct Vector3 { int x, y, z; };

struct PathPtr {
    char pad[8];
    PathPtr();
    void FromID(unsigned int id);
    int NumNodes() const;
    void GetNode(Vector3& out, unsigned int idx) const;
};

extern "C" {
BMD_File* Model_LoadFile(SharedFilePtr& p);
int ModelBase_SetFile(void* self, BMD_File* f, int a, int b);
void Animation_LoadFile(SharedFilePtr& p);
void LoadSilverStarAndNumber(void);
void ShadowModel_InitCylinder(void* self);
void MovingCylinderClsnWithPos_Init(void* self, Actor* a, const Vector3& v, int fix12, int c, unsigned int d, unsigned int e);
void func_ov100_02143b18(unsigned char* c, void* p);

extern SharedFilePtr data_ov002_0211092c;
extern SharedFilePtr data_ov100_021486bc;
extern SharedFilePtr data_ov100_021486a4;
extern SharedFilePtr data_ov100_021486ac;
extern SharedFilePtr data_ov100_021486b4;
extern int data_ov100_02148008[3];
extern int data_ov100_021486f4;
}

extern "C" int _ZN14UnchainedChomp13InitResourcesEv(unsigned char* thiz)
{
    Model_LoadFile(data_ov002_0211092c);
    ModelBase_SetFile(thiz + 0x30c, Model_LoadFile(data_ov100_021486bc), 1, -1);
    Model_LoadFile(data_ov100_021486a4);
    Animation_LoadFile(data_ov100_021486ac);
    Animation_LoadFile(data_ov100_021486b4);
    LoadSilverStarAndNumber();

    {
        int i = 0;
        unsigned char* p = thiz + 0x370;
        do {
            ModelBase_SetFile(p, *(BMD_File**)((char*)&data_ov100_021486a4 + 4), 1, -1);
            i++;
            p += 0x50;
        } while (i < 6);
    }

    ShadowModel_InitCylinder(thiz + 0x640);
    {
        int i = 0;
        unsigned char* p = thiz + 0x550;
        do {
            ShadowModel_InitCylinder(p);
            i++;
            p += 0x28;
        } while (i < 6);
    }

    *(int*)(thiz + 0x6ac) = *(int*)(thiz + 8) & 0xff;
    *(int*)(thiz + 0x6b8) = (*(unsigned int*)(thiz + 8) >> 8) & 0xf;
    if (*(int*)(thiz + 0x6ac) == 0xff) *(int*)(thiz + 0x6ac) = 0;

    {
        PathPtr pp;
        pp.FromID(*(unsigned int*)(thiz + 0x6ac));
        *(int*)(thiz + 0x6b0) = pp.NumNodes();
    }

    *(int*)(thiz + 0x9c) = -0x2000;
    *(int*)(thiz + 0xa0) = -0x3c000;

    {
        Vector3 v;
        v.x = data_ov100_02148008[0];
        v.y = data_ov100_02148008[1];
        v.z = data_ov100_02148008[2];
        MovingCylinderClsnWithPos_Init(thiz + 0x110, (Actor*)thiz, v,
            0xaa000, 0x140000, 0x200004, 0x6010);
    }

    *(unsigned char*)(thiz + 0x6c9) = 0x1f;
    *(int*)(thiz + 0x6cc) = 3;

    *(short*)(thiz + 0x8e) = *(short*)(thiz + 0x94);
    *(short*)(thiz + 0x600 + 0xa4) = *(short*)(thiz + 0x8e);

    *(int*)(thiz + 0x6d0) = 0;
    *(int*)(thiz + 0x6d4) = 0;

    {
        int i = 0;
        unsigned char* dst = thiz;
        do {
            *(int*)(dst + 0x6d8) = *(int*)(thiz + 0x5c);
            i++;
            *(int*)(dst + 0x6dc) = *(int*)(thiz + 0x60);
            *(int*)(dst + 0x6e0) = *(int*)(thiz + 0x64);
            dst += 0xc;
        } while (i < 6);
    }

    *(short*)(thiz + 0x600 + 0xca) = 0xc8;

    {
        PathPtr pp;
        pp.FromID(*(unsigned int*)(thiz + 0x6ac));
        *(int*)(thiz + 0x6b4) = 1;
        pp.GetNode(*(Vector3*)(thiz + 0x5c), *(unsigned int*)(thiz + 0x6b4));
    }

    /* u64-mask launder: materialize add r1,r4,#0x60; ldr/str [r1] for y += 0x64000 */
    *(int *)(((int)thiz + 0x60) & 0xFFFFFFFFFFFFFFFF) += 0x64000;
    *(int*)(thiz + 0x80) = 0x1000;
    *(int*)(thiz + 0x84) = 0x1000;
    *(int*)(thiz + 0x88) = 0x1000;

    func_ov100_02143b18(thiz, &data_ov100_021486f4);
    return 1;
}
