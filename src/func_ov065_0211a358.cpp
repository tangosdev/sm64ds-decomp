//cpp
struct SharedFilePtr;
struct BMD_File;
struct KCL_File;
struct Matrix4x3;
struct CLPS_Block;

extern struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thisp, struct BMD_File *, int, int);
extern void _ZN11ShadowModel10InitCuboidEv(void *thisp);
extern "C" int func_ov065_0211a114(char *c);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *thisp);
extern struct KCL_File *_ZN12MeshCollider8LoadFileER13SharedFilePtr(struct SharedFilePtr &);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *thisp, struct KCL_File *, struct Matrix4x3 const &, int, short, struct CLPS_Block &);
extern "C" void func_020393a4(int *p, int v);
extern "C" void func_02039394(int *p, int v);
extern "C" void func_020393d4(int *p, int v);

extern struct SharedFilePtr data_ov065_0211d88c;
extern struct SharedFilePtr data_ov065_0211d894;
extern unsigned char LEVEL_SPECIFIC_SETTING;
extern short data_ov065_0211c0b0[];
extern int func_02112198;
extern void _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);

extern "C" int func_ov065_0211a358(char *self) {
    struct BMD_File *bmd;
    struct KCL_File *kcl;
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov065_0211d88c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii((void *)(self + 0xd4), bmd, 1, -1);
    _ZN11ShadowModel10InitCuboidEv((void *)(self + 0x330));

    *(short *)(self + 0x31e) = 1;
    *(short *)(self + 0x320) = data_ov065_0211c0b0[LEVEL_SPECIFIC_SETTING];
    *(short *)(self + 0x322) = 0x1964;
    *(short *)(self + 0x90) = *(short *)(self + 0x322);

    func_ov065_0211a114(self);
    _ZN8Platform19UpdateClsnPosAndRotEv(self);

    kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov065_0211d894);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        (void *)(self + 0x124), kcl, *(struct Matrix4x3 const *)(self + 0x2ec), 0x1000,
        *(short *)(self + 0x8e), *(struct CLPS_Block *)&func_02112198);

    func_020393a4((int *)(self + 0x124), 0x300000);
    func_02039394((int *)(self + 0x124), -0x200000);

    if (LEVEL_SPECIFIC_SETTING != 3) {
        func_020393d4((int *)(self + 0x124),
            (int)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    }
    return 1;
}
