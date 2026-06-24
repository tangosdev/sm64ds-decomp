//cpp
struct BMD_File; struct Actor; struct Vector3_16;
extern "C" void *ModelLoadFile(void *fp);
struct ModelBase { void SetFile(BMD_File *f, int b, int c); };
extern "C" void AnimLoadFile(void *fp);
struct ShadowModel { int InitCylinder(); };
struct MovingCylinderClsn { void Init(Actor *a, int b, int c, unsigned int d, unsigned int e); };
struct WithMeshClsn { void Init(Actor *a, int b, int c, void *d, int e); };
extern "C" int data_ov085_0212f280[];
extern "C" char data_ov085_021304f4;
extern "C" void func_ov085_0212a4a4(char *c, int x);
extern "C" void func_ov085_02129fdc(char *c);

extern "C" int func_ov085_0212a588(Actor *self)
{
    char *s = (char*)self;
    void *f = ModelLoadFile(&data_ov085_021304f4);
    ((ModelBase*)(s + 0xd4))->SetFile((BMD_File*)f, 1, -1);
    for (int i = 0; i < 7; i++)
        AnimLoadFile((void*)data_ov085_0212f280[i]);
    if (((ShadowModel*)(s + 0x138))->InitCylinder() == 0)
        return 0;
    ((MovingCylinderClsn*)(s + 0x160))->Init(self, 0x90000, 0xc0000, 0x4800004, 0);
    ((WithMeshClsn*)(s + 0x194))->Init(self, 0x40000, 0x40000, 0, 0);
    *(int*)(s + 0x9c) = -0x2000;
    *(int*)(s + 0xa0) = -0x3c000;
    *(int*)(s + 0x80) = 0x1000;
    *(int*)(s + 0x84) = 0x1000;
    *(int*)(s + 0x88) = 0x1000;
    func_ov085_0212a4a4(s, 0);
    func_ov085_02129fdc(s);
    return 1;
}
