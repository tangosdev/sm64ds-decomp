//cpp
struct BMD_File; struct BCA_File; struct Actor; struct Vector3_16;
extern "C" void *ModelLoadFile(void *fp);
struct ModelBase { void SetFile(BMD_File *f, int b, int c); };
extern "C" void AnimLoadFile(void *fp);
struct ModelAnim { void SetAnim(BCA_File *f, int b, int c, unsigned int d); };
struct ShadowModel { int InitCylinder(); };
struct MovingCylinderClsn { void Init(Actor *a, int b, int c, unsigned int d, unsigned int e); };
struct WithMeshClsn { void Init(Actor *a, int b, int c, void *d, int e); void StartDetectingWater(); };
extern "C" char data_ov081_02128d60;
extern "C" int data_ov081_02128d68[];

extern "C" int _ZN9Spindrift13InitResourcesEv(Actor *self)
{
    char *s = (char*)self;
    void *mf = ModelLoadFile(&data_ov081_02128d60);
    ((ModelBase*)(s + 0x110))->SetFile((BMD_File*)mf, 1, -1);
    AnimLoadFile(data_ov081_02128d68);
    ((ModelAnim*)(s + 0x110))->SetAnim((BCA_File*)data_ov081_02128d68[1], 0, 0x1000, 0);
    if (((ShadowModel*)(s + 0x174))->InitCylinder() == 0)
        return 0;
    ((MovingCylinderClsn*)(s + 0x19c))->Init(self, 0x3c000, 0x78000, 0x200000, 0xa6efe0);
    ((WithMeshClsn*)(s + 0x1d0))->Init(self, 0x3c000, 0x3c000, 0, 0);
    ((WithMeshClsn*)(s + 0x1d0))->StartDetectingWater();
    *(int*)(s + 0x38c) = *(int*)(s + 0x5c);
    *(int*)(s + 0x390) = *(int*)(s + 0x60);
    *(int*)(s + 0x394) = *(int*)(s + 0x64);
    *(char*)(s + 0x39a) = 0;
    *(int*)(s + 0x9c) = -0x2000;
    *(int*)(s + 0xa0) = -0x3c000;
    return 1;
}
