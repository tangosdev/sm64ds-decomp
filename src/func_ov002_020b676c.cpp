//cpp
typedef short s16;
struct SharedFilePtr { int x; }; struct BMD_File; struct KCL_File; struct Matrix4x3; struct CLPS_Block;
struct Arg { struct SharedFilePtr *m[2]; struct CLPS_Block *clps; };
extern int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
extern "C" {
struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, struct BMD_File *f, int a, int b);
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
struct KCL_File *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, struct KCL_File *k, struct Matrix4x3 &m, int fx, short s, struct CLPS_Block &c);
void func_020393d4(int *p, int v);
}

extern "C" int func_ov002_020b676c(unsigned char *self, struct Arg *a, short arg2)
{
    struct BMD_File *bmd = _ZN5Model8LoadFileER13SharedFilePtr(*a->m[0]);
    short y;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, bmd, 1, -1);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(self);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(self);
    {
        struct KCL_File *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*a->m[1]);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            self + 0x124, kcl, *(struct Matrix4x3 *)(self + 0x2ec), 0x199,
            *(s16 *)(self + 0x8e), *a->clps);
    }
    func_020393d4((int *)(self + 0x124), (int)&_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    *(s16 *)(self + 0x96) = arg2;
    y = *(s16 *)(self + 0x90);
    if (y != 0) {
        *(s16 *)(self + 0x96) = y;
        *(s16 *)(self + 0x90) = 0;
    }
    return 1;
}
