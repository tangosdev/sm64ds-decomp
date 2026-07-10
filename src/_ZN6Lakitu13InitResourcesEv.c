//cpp
// NONMATCHING: register allocation (div=10). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct SharedFilePtr;
struct BMD_File;
struct BTP_File;
struct Actor;
struct Vector3 { int x, y, z; };
struct Vector3_16;

extern "C" {
BMD_File* Model_LoadFile(SharedFilePtr& p);
int ModelBase_SetFile(void* self, BMD_File* f, int a, int b);
void Animation_LoadFile(SharedFilePtr& p);
void TextureSequence_LoadFile(SharedFilePtr& p);
void TextureSequence_Prepare(BMD_File& a, BTP_File& b);
int ShadowModel_InitCylinder(void* self);
void MovingCylinderClsnWithPos_Init(void* self, Actor* a, const Vector3& v, int b, int c, unsigned int d, unsigned int e);
void WithMeshClsn_Init(void* self, Actor* a, int b, int c, Vector3_16* d, int e);
int func_ov077_0212478c(void* c);
int func_ov077_02123d40(void* c);

extern SharedFilePtr data_ov077_02127b38;
extern unsigned char data_ov077_02127b50[];
extern SharedFilePtr data_ov077_02127b48;
extern SharedFilePtr* data_ov077_02127238[];
extern unsigned char* data_ov077_02127230[];
extern Vector3 data_ov077_02127b88;
struct M48 { int w[12]; };
extern M48 data_02082128;
}

extern "C" int _ZN6Lakitu13InitResourcesEv(unsigned char* thiz)
{
    Model_LoadFile(data_ov077_02127b38);
    ModelBase_SetFile(thiz + 0xd4, Model_LoadFile(*(SharedFilePtr*)data_ov077_02127b50), 1, 1);
    ModelBase_SetFile(thiz + 0x138, Model_LoadFile(data_ov077_02127b48), 1, 1);

    for (int i = 0; i < 2; i++) {
        Animation_LoadFile(*data_ov077_02127238[i]);
    }
    for (int i = 0; i < 2; i++) {
        unsigned char* ts = data_ov077_02127230[i];
        TextureSequence_LoadFile(*(SharedFilePtr*)ts);
        TextureSequence_Prepare(*(BMD_File*)*(void**)(data_ov077_02127b50 + 4), *(BTP_File*)*(void**)(ts + 4));
    }

    if (ShadowModel_InitCylinder(thiz + 0x188) == 0) return 0;

    MovingCylinderClsnWithPos_Init(thiz + 0x1c4, (Actor*)thiz, data_ov077_02127b88,
        0x41000, 0x78000, 0x200002, 0x6eff0);
    WithMeshClsn_Init(thiz + 0x204, (Actor*)thiz, 0x2d000, 0x2d000, (Vector3_16*)0, 0);

    *(int*)(thiz + 0x9c) = 0;
    *(int*)(thiz + 0xa0) = 0;
    *(int*)(thiz + 0x80) = 0x1000;
    *(int*)(thiz + 0x84) = 0x1000;
    *(int*)(thiz + 0x88) = 0x1000;
    *(int*)(thiz + 0x3f8) = *(int*)(thiz + 0x5c);
    *(int*)(thiz + 0x3fc) = *(int*)(thiz + 0x60);
    *(int*)(thiz + 0x400) = *(int*)(thiz + 0x64);
    *(int*)(thiz + 0x410) = 0;
    func_ov077_0212478c(thiz);

    *(M48*)(thiz + 0x3c0) = data_02082128;
    func_ov077_02123d40(thiz);
    return 1;
}
