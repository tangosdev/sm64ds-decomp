//cpp
// @symbol func_ov060_021182b0
// recovered name: daKpa3Bg_c_InitResources
/* recovered: renamed to Class_Method */
/* daKpa3Bg_c::InitResources - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
struct BMD_File; struct KCL_File; struct dActor_c; struct Matrix4x3;
struct CLPS_Block; struct SharedFilePtr;
struct Vector3;

struct Model { static BMD_File* LoadFile(SharedFilePtr& f); };
struct ModelBase { void SetFile(BMD_File* f, int b, int c); };
struct dBgActor_c { void UpdateClsnPosAndRot(); };
struct MeshCollider { static KCL_File* LoadFile(SharedFilePtr& f); };
struct MovingMeshCollider { };
/* Declared by its final name rather than as a shadow method: the ROM's symbol takes
   Fix12<int> where the call site has an int, and Fix12<int> is an aggregate with no
   converting constructor, so a shadow declaration cannot be both callable and mangle
   correctly. Spelling the symbol literally sidesteps the mangling entirely -- the
   parameter types then only decide the call ABI, and a Fix12 passes in one register
   bit-identical to an int. */
extern "C" void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void* self, KCL_File* f, const Matrix4x3& m, int fix, short sh, CLPS_Block& b);
struct MeshColliderBase { void Enable(dActor_c* a); };

extern "C" void CopyTexPalFromLevelModel(void* p);
extern "C" void func_020393d4(void* p, void* v);
extern "C" void func_020393c4(void* p, void* v);

extern "C" int data_0208e738;
extern "C" SharedFilePtr* data_ov060_02119514[];
extern "C" SharedFilePtr* data_ov060_0211953c[];
extern "C" CLPS_Block* data_ov060_0211a980[];
extern "C" void _ZN16MeshColliderBase22UpdatePosWithTransformERS_P8dActor_cR10ClsnResultR7Vector3P10Vector3_16S8_();
extern "C" void func_ov060_021183f4();

extern "C" int func_ov060_021182b0(char* self);
int func_ov060_021182b0(char* self)
{
    int idx = *(int*)(self + 8) & 0xf;
    *(unsigned char*)(self + 0x329) = (unsigned char)idx;
    data_0208e738 = 0;
    ((ModelBase*)(self + 0xd4))->SetFile(
        Model::LoadFile(*data_ov060_02119514[*(unsigned char*)(self + 0x329)]), 1, -1);
    CopyTexPalFromLevelModel(self + 0xdc);
    data_0208e738 = 1;
    ((dBgActor_c*)self)->UpdateClsnPosAndRot();
    {
        int i = *(unsigned char*)(self + 0x329);
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            self + 0x124, MeshCollider::LoadFile(*data_ov060_0211953c[i]),
            *(Matrix4x3*)(self + 0x2ec), 0x1000, *(short*)(self + 0x8e),
            *data_ov060_0211a980[i]);
    }
    func_020393d4(self + 0x124, (void*)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P8dActor_cR10ClsnResultR7Vector3P10Vector3_16S8_);
    func_020393c4(self + 0x124, (void*)&func_ov060_021183f4);
    ((MeshColliderBase*)(self + 0x124))->Enable((dActor_c *)((dActor_c*)self));
    *(int*)(self + 0x320) = 0;
    *(unsigned char*)(self + 0x32b) = 0;
    *(unsigned char*)(self + 0x328) = 0;
    *(unsigned char*)(self + 0x32a) = 0;
    *(unsigned short*)(self + 0x326) = 0;
    *(unsigned short*)(self + 0x324) = *(unsigned short*)(self + 0x326);
    *(int*)(self + 0x9c) = 0;
    *(int*)(self + 0xa0) = -0x1e000;
    return 1;
}
