//cpp
typedef signed short s16;
typedef unsigned int u32;

struct SomeGlobal { char pad[4]; void* p; };
extern SomeGlobal* PARTICLE_SYS_TRACKER;

extern void func_02049d60(void* x);

namespace Particle {
    struct System {
        char pad_00[0x1c];
        u32 field_1c;
        char pad_20[0x3a - 0x20];
        s16 field_3a;
    };
    struct SimpleCallback {
        char pad_00[4];
        s16 field_4;
        void SpawnParticles(System& sys);
    };
}

void Particle::SimpleCallback::SpawnParticles(System& sys)
{
    *(u32*)(((int)&sys + 0x1c) & 0xFFFFFFFFFFFFFFFFLL) |= 2;
    sys.field_3a = this->field_4;
    func_02049d60(PARTICLE_SYS_TRACKER->p);
}
