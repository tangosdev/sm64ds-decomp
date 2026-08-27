//cpp
typedef short s16;
typedef unsigned int u32;

template<class T> struct Fix12 { int val; };

struct SharedFilePtr { unsigned short fileID; unsigned char numRefs; char* filePtr; };
struct BMD_File; struct BMA_File; struct BCA_File; struct Vector3_16; struct dActor_c;

struct Model { static void LoadFile(SharedFilePtr&); };
struct Animation { static void LoadFile(SharedFilePtr&); };
struct ModelBase { int SetFile(BMD_File*, int, int); };
struct ShadowModel { int InitCylinder(); };
struct MaterialChanger {
    static void Prepare(BMD_File&, BMA_File&);
};
    
extern "C" int _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(
    void*, BMA_File&, int, int, u32);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void*, BCA_File*, int, int, u32);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void*, dActor_c*, int, int, Vector3_16*, int);

extern SharedFilePtr data_ov084_02130cf8;
extern SharedFilePtr* data_ov074_0212292c[];
extern SharedFilePtr data_ov084_0213089c;
extern SharedFilePtr data_ov084_02130cc8;

extern "C" int func_ov074_02122634(char* self)
{
    int i;

    Model::LoadFile(data_ov084_02130cf8);
    for (i = 0; i < 7; i++)
        Animation::LoadFile(*data_ov074_0212292c[i]);

    if (((ModelBase*)(self + 0x210))->SetFile(
            (BMD_File*)data_ov084_02130cf8.filePtr, 1, 1) == 0)
        return 0;

    if (((ShadowModel*)(self + 0x274))->InitCylinder() == 0)
        return 0;

    MaterialChanger::Prepare(
        *(BMD_File*)data_ov084_02130cf8.filePtr,
        *(BMA_File*)&data_ov084_0213089c);

    _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(
        (void*)(self + 0x3d0),
        *(BMA_File*)&data_ov084_0213089c,
        0x40000000, 0x1000, 0);

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        (void*)(self + 0x210),
        (BCA_File*)data_ov084_02130cc8.filePtr,
        0, 0x1000, 0);

    *(s16*)(self + 0x92) = *(s16*)(self + 0x8c);
    *(s16*)(self + 0x94) = *(s16*)(self + 0x8e);
    *(s16*)(self + 0x96) = *(s16*)(self + 0x90);
    *(int*)(self + 0xa0) = -0x3c000;
    *(int*)(self + 0x5dc) = 0;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        (void*)(self + 0x40c),
        (dActor_c*)self,
        0x14000, 0x14000,
        (Vector3_16*)0, 0);

    return 1;
}
