//cpp
typedef int Fix12i;
struct Vector3 { int x, y, z; };

extern "C" void func_ov002_020efcf4(void* c);
struct PathLift { void BaseBehavior(); };
extern Fix12i Vec3_Dist(const Vector3* a, const Vector3* b);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
namespace Sound { unsigned int PlayLong(unsigned int, unsigned int, unsigned int, const Vector3&, unsigned int); }
extern "C" int func_ov100_0214700c(char* c);
struct Platform2 { void UpdateClsnPosAndRot(); };
extern "C" void func_ov100_02146e70(void* c);
extern "C" void func_020393a4(int *p, int v);
extern "C" void func_02039394(int *p, int v);
struct MeshColliderBase { int IsEnabled(); void Enable(void *a); };
struct Platform3 { int IsClsnInRange(Fix12i a, int b); };
extern unsigned char CURRENT_GAMEMODE;

extern "C" int func_ov100_021470f4(char* c)
{
    func_ov002_020efcf4(c);
    ((PathLift*)c)->BaseBehavior();
    if (Vec3_Dist((Vector3*)(c + 0x5c), (Vector3*)(c + 0x68)) != 0) {
        if (DecIfAbove0_Byte((unsigned char*)(c + 0x4b0)) == 0) {
            *(unsigned int*)(c + 0x4a8) = Sound::PlayLong(*(unsigned int*)(c + 0x4a8), 3, 0x82, *(Vector3*)(c + 0x74), 0);
        }
    }
    func_ov100_0214700c(c);
    ((Platform2*)c)->UpdateClsnPosAndRot();
    func_ov100_02146e70(c);
    func_020393a4((int*)(c + 0x124), 0x150000);
    func_02039394((int*)(c + 0x124), 0x1000);
    int b = (int)(CURRENT_GAMEMODE == 1);
    if (b != 0) {
        if (((MeshColliderBase*)(c + 0x124))->IsEnabled() == 0) {
            ((MeshColliderBase*)(c + 0x124))->Enable(c);
        }
    } else {
        ((Platform3*)c)->IsClsnInRange(0, 0);
    }
    return 1;
}
