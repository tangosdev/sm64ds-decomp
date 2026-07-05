//cpp
typedef short s16;
struct SharedFilePtr { int x; }; struct BMD_File; struct KCL_File; struct Matrix4x3; struct CLPS_Block;
extern int _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
extern "C" {
struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, struct BMD_File *f, int a, int b);
void func_ov025_02111e30(char *t);
void func_ov025_02111dec(char *c);
struct KCL_File *_ZN12MeshCollider8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, struct KCL_File *k, struct Matrix4x3 &m, int fx, short s, struct CLPS_Block &c);
void func_020393d4(int *p, int v);
extern struct SharedFilePtr data_ov025_02113ab8;
extern struct SharedFilePtr data_ov025_02113ab0;
extern struct CLPS_Block data_ov025_02112ce8;

int func_ov025_02111fa0(char *self){
    struct BMD_File *bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov025_02113ab8);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0x320, bmd, 1, -1);
    func_ov025_02111e30(self);
    func_ov025_02111dec(self);
    {
        struct KCL_File *kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov025_02113ab0);
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            self + 0x124, kcl, *(struct Matrix4x3 *)(self + 0x374), 0x1000,
            *(s16 *)(self + 0x8e), data_ov025_02112ce8);
    }
    func_020393d4((int *)(self + 0x124), (int)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    {
        int v = 0x5000;
        int k = *(int*)(self + 8) & 3;
        *(int*)(self + 0xa8) = -v;
        *(unsigned char*)(self + 0x372) = 0;
        *(s16*)(self + 0x370) = 0;
        switch (k) {
        case 0:
            break;
        case 1:
            *(int*)(((int)self + 0x60) & 0xFFFFFFFFFFFFFFFF) -= 0xfa000;
            *(unsigned short*)(((int)self + 0x370) & 0xFFFFFFFFFFFFFFFF) += 0x32;
            break;
        case 2:
            *(int*)(((int)self + 0x60) & 0xFFFFFFFFFFFFFFFF) -= 0x1f4000;
            *(unsigned char*)(self + 0x372) = 1;
            *(int*)(self + 0xa8) = v;
            break;
        }
    }
    return 1;
}
}
