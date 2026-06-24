//cpp
struct SharedFilePtr;
struct BMD_File;

extern "C" struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr *f);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, struct BMD_File *f, int a, int b);
extern "C" void Matrix4x3_FromRotationY(void *m, int angle);
extern "C" int func_ov100_02145f68(void *c, void *p, int a2);
extern struct SharedFilePtr data_ov100_02148934;
extern unsigned char data_0209f250[];
extern int data_0209f394[];
extern int data_ov100_02148974[];

extern "C" int func_ov100_0214612c(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    struct BMD_File *f;

    *(int *)(c + 8) = (unsigned)*(int *)(c + 8) >> 0x10;
    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov100_02148934);
    if (!_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, f, 1, 1)) return 0;

    Matrix4x3_FromRotationY(c + 0xe0, *(short *)(c + 0x8e));
    *(int *)(c + 0x104) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x108) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x10c) = *(int *)(c + 0x64) >> 3;
    func_ov100_02145f68(c, data_ov100_02148974, data_0209f394[data_0209f250[0]]);
    return 1;
}
