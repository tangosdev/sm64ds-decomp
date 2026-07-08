//cpp
struct SharedFilePtr;
struct BMD_File;

extern "C" struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr *f);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, struct BMD_File *f, int a, int b);
extern "C" void Matrix4x3_FromRotationY(void *m, int angle);
extern "C" int func_ov100_02145f68(void *c, void *p, int a2);
extern struct SharedFilePtr STAR_DOOR_MODEL_PTR;
extern unsigned char CURR_PLAYER_ID[];
extern int PLAYER_ARR[];
extern int data_ov100_02148974[];

extern "C" int func_ov100_0214612c(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    struct BMD_File *f;

    *(int *)(c + 8) = (unsigned)*(int *)(c + 8) >> 0x10;
    f = _ZN5Model8LoadFileER13SharedFilePtr(&STAR_DOOR_MODEL_PTR);
    if (!_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, f, 1, 1)) return 0;

    Matrix4x3_FromRotationY(c + 0xe0, *(short *)(c + 0x8e));
    *(int *)(c + 0x104) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x108) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x10c) = *(int *)(c + 0x64) >> 3;
    func_ov100_02145f68(c, data_ov100_02148974, PLAYER_ARR[CURR_PLAYER_ID[0]]);
    return 1;
}
