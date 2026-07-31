//cpp
// @symbol func_ov091_02131070
// @emits RotatingUpDownPlatformUtm_Kill
/* recovered: shared common types, renamed to Class_Method */
/* daObjRotateUpdownLift_c::Kill - recovered from vtable slot identity */
extern "C" {
struct Vector3 { int x,y,z; };
}
namespace Particle { struct System { static System* NewSimple(unsigned int, int, int, int); }; }
struct Actor { void PoofDustAt(const Vector3&); };
namespace Sound { void PlayBank3(unsigned int, const Vector3&); }
struct MeshColliderBase { int IsEnabled(); void Disable(); };
extern "C" void RotatingUpDownPlatformUtm_Kill(char* c){
    Particle::System::NewSimple(0xbb, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
    Vector3 v = { *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64) };
    ((Actor*)c)->PoofDustAt(v);
    Sound::PlayBank3(0xf, *(Vector3*)(c+0x74));
    *(unsigned char*)(c+0x3a0)=1;
    *(unsigned char*)(c+0x31c)=0;
    if(((MeshColliderBase*)(c+0x124))->IsEnabled()){
        ((MeshColliderBase*)(c+0x124))->Disable();
    }
}
