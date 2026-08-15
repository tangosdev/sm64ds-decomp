//cpp
// @symbol func_ov060_021182b0
// recovered name: daKpa3Bg_c_InitResources
/* recovered: renamed to Class_Method */
/* daKpa3Bg_c::InitResources - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
struct BMD_File; struct KCL_File; struct Actor; struct Matrix4x3;
struct CLPS_Block; struct SharedFilePtr;
struct Vector3;

struct Model { static BMD_File* LoadFile(SharedFilePtr& f); };
struct ModelBase { void SetFile(BMD_File* f, int b, int c); };
struct Platform { void UpdateClsnPosAndRot(); };
struct MeshCollider { static KCL_File* LoadFile(SharedFilePtr& f); };
struct MovingMeshCollider {
    void SetFile(KCL_File* f, const Matrix4x3& m, int fix, short sh, CLPS_Block& b);
};
struct MeshColliderBase { void Enable(Actor* a); };

extern "C" void CopyTexPalFromLevelModel(void* p);
extern "C" void func_020393d4(void* p, void* v);
extern "C" void func_020393c4(void* p, void* v);

extern "C" int data_0208e738;
extern "C" SharedFilePtr* data_ov060_02119514[];
extern "C" SharedFilePtr* data_ov060_0211953c[];
extern "C" CLPS_Block* data_ov060_0211a980[];
extern void _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_();
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
    ((Platform*)self)->UpdateClsnPosAndRot();
    {
        int i = *(unsigned char*)(self + 0x329);
        ((MovingMeshCollider*)(self + 0x124))->SetFile(
            MeshCollider::LoadFile(*data_ov060_0211953c[i]),
            *(Matrix4x3*)(self + 0x2ec), 0x1000, *(short*)(self + 0x8e),
            *data_ov060_0211a980[i]);
    }
    func_020393d4(self + 0x124, (void*)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    func_020393c4(self + 0x124, (void*)&func_ov060_021183f4);
    ((MeshColliderBase*)(self + 0x124))->Enable((Actor *)((Actor*)self));
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
