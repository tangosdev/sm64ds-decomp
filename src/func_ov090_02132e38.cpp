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
void MovingCylinderClsnWithPos_Init(void* self, Actor* a, const Vector3& v, int b, int c, unsigned int d, unsigned int e);
void func_ov090_02132ac4(unsigned char* c, void* p);

extern SharedFilePtr data_ov090_02134524;
extern SharedFilePtr WATER_RING_MODEL_PTR;
extern SharedFilePtr RED_NUMBER_MODEL_PTR;
extern SharedFilePtr data_ov090_0213452c;
extern Vector3 data_ov090_02134200;
extern int data_ov090_0213454c;
extern unsigned char CURRENT_GAMEMODE;
}

extern "C" int func_ov090_02132e38(unsigned char* thiz)
{
    ModelBase_SetFile(thiz + 0x30c, Model_LoadFile(data_ov090_02134524), 1, -1);
    Model_LoadFile(WATER_RING_MODEL_PTR);
    Model_LoadFile(RED_NUMBER_MODEL_PTR);
    Animation_LoadFile(data_ov090_0213452c);

    *(int*)(thiz + 0x37c) = *(int*)(thiz + 8) & 0xff;
    *(int*)(thiz + 0x388) = (*(unsigned int*)(thiz + 8) >> 0xc) & 0xf;
    if (*(int*)(thiz + 0x37c) < 0) *(int*)(thiz + 0x37c) = 0;

    {
        PathPtr pp;
        pp.FromID(*(unsigned int*)(thiz + 0x37c));
        *(int*)(thiz + 0x380) = pp.NumNodes();
    }

    *(int*)(thiz + 0xa0) = -0x3c000;

    {
        Vector3 v;
        v.x = data_ov090_02134200.x;
        v.y = data_ov090_02134200.y;
        v.z = data_ov090_02134200.z;
        MovingCylinderClsnWithPos_Init(thiz + 0x110, (Actor*)thiz, v,
            0x150000, 0xc8000, 0x200004, 0);
    }

    {
        PathPtr pp;
        pp.FromID(*(unsigned int*)(thiz + 0x37c));
        *(int*)(thiz + 0x384) = 1;
        pp.GetNode(*(Vector3*)(thiz + 0x5c), *(unsigned int*)(thiz + 0x384));
    }

    {
        int b = (int)(CURRENT_GAMEMODE == 2);
        if (b != 0) {
            *(int*)(thiz + 0x384) = 3;
            *(short*)(thiz + 0x92) = (short)0xf303;
            *(short*)(thiz + 0x94) = 0xb50;
            *(short*)(thiz + 0x96) = 0;
            *(int*)(thiz + 0x5c) = (int)0xfdfb8000;
            *(int*)(thiz + 0x60) = (int)0xff8f8000;
            *(int*)(thiz + 0x64) = 0x29a000;
            *(int*)(thiz + 0xb0) = 0;
        }
    }

    func_ov090_02132ac4(thiz, &data_ov090_0213454c);
    return 1;
}
