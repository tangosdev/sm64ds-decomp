//cpp
typedef short s16;
struct SharedFilePtr; struct BMD_File; struct KCL_File; struct Matrix4x3; struct CLPS_Block;
extern struct SharedFilePtr data_ov002_0210e12c;
extern struct SharedFilePtr data_ov002_0210e124;
extern struct CLPS_Block data_ov002_0210d7f4;
extern "C" {
struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, struct BMD_File *f, int a, int b);
void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
struct KCL_File *_ZN12MeshCollider8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, struct KCL_File *k, struct Matrix4x3 &m, int fx, short s, struct CLPS_Block &c);
int IsCannonOpenInCurLevel(void);
}

extern "C" int _ZN11CannonHatch13InitResourcesEv(unsigned char *self)
{
    struct BMD_File *bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210e12c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, bmd, 1, -1);
    _ZN8Platform21UpdateModelPosAndRotYEv(self);
    _ZN8Platform19UpdateClsnPosAndRotEv(self);
    {
        struct KCL_File *kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov002_0210e124);
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            self + 0x124, kcl, *(struct Matrix4x3 *)(self + 0x2ec), 0x199,
            *(s16 *)(self + 0x8e), data_ov002_0210d7f4);
    }
    *(int *)(self + 0x320) = *(int *)(self + 0x5c);
    *(int *)(self + 0x324) = *(int *)(self + 0x60);
    *(int *)(self + 0x328) = *(int *)(self + 0x64);
    if (IsCannonOpenInCurLevel() != 0) {
        *(unsigned char *)(self + 0x32e) = 1;
    }
    return 1;
}
