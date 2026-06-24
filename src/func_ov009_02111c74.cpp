//cpp
typedef short s16;
struct SharedFilePtr { int x; }; struct BMD_File; struct BTA_File; struct KCL_File; struct Matrix4x3; struct CLPS_Block;
struct DataPtr { int f[2]; };
extern "C" {
struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, struct BMD_File *f, int a, int b);
void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void* bmd, struct BTA_File& bta);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void* self, struct BTA_File& bta, int a, int b, unsigned int c);
void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
struct KCL_File *_ZN12MeshCollider8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, struct KCL_File *k, struct Matrix4x3 &m, int fx, short s, struct CLPS_Block &c);
void _ZN16MeshColliderBase6EnableEP5Actor(void* self, void* actor);
int func_ov009_02111b1c(char* thiz);
extern unsigned char data_0209f2d8;
extern int data_0209caa0;
extern struct DataPtr data_ov009_02113c68;
extern struct BTA_File data_ov009_02112bc4;
extern struct SharedFilePtr data_ov009_02113c70;
extern struct CLPS_Block data_ov009_02112c38;
extern int data_0209f32c;

int func_ov009_02111c74(char *self){
    int b = (int)(data_0209f2d8 == 1);
    if (b == 0) {
        if (*(int*)((char*)&data_0209caa0 + 8) & 0x80000) {
            *(int*)(self + 0x60) = -0x2bc000;
        }
    }
    {
        struct BMD_File *bmd = _ZN5Model8LoadFileER13SharedFilePtr(*(struct SharedFilePtr*)&data_ov009_02113c68);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, bmd, 1, 0x14);
    }
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File((void*)data_ov009_02113c68.f[1], data_ov009_02112bc4);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(self + 0x320, data_ov009_02112bc4, 0, 0x1000, 0);
    _ZN8Platform21UpdateModelPosAndRotYEv(self);
    _ZN8Platform19UpdateClsnPosAndRotEv(self);
    {
        struct KCL_File *kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov009_02113c70);
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            self + 0x124, kcl, *(struct Matrix4x3 *)(self + 0x2ec), 0x1000,
            *(s16 *)(self + 0x8e), data_ov009_02112c38);
    }
    _ZN16MeshColliderBase6EnableEP5Actor(self + 0x124, self);
    {
        int v = *(int*)(self + 0x60) - 0x64000;
        if (data_0209f32c > v) data_0209f32c = v;
    }
    func_ov009_02111b1c(self);
    return 1;
}
}
