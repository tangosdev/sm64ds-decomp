// Particle::System::NewSimple - creates a new particle system at position
typedef int s32;
typedef unsigned int u32;

typedef struct { s32 x, y, z; } Vector3;

struct ParticleSysTracker {
    u32 unk0;
    void* manager; // at +4
};

extern struct ParticleSysTracker* data_0209ee74;
extern void* _ZN8Particle7Manager9AddSystemEiR7Vector3(void* mgr, u32 uniqueID, Vector3* pos);

void* _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 uniqueID, s32 x, s32 y, s32 z) {
    Vector3 pos;
    s32 xv = x >> 3;
    s32 yv = y >> 3;
    s32 zv = z >> 3;
    pos.x = xv;
    pos.y = yv;
    pos.z = zv;
    void* mgr = data_0209ee74->manager;
    return _ZN8Particle7Manager9AddSystemEiR7Vector3(mgr, uniqueID, &pos);
}