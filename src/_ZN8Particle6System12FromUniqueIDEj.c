#include "types.h"
// Particle::System::FromUniqueID - looks up a Particle::System by unique ID
struct ParticleSysTracker {
    u32 unk0;
    u32 unk4;
    // Contents sub-object at +8
    u32 contents[1]; // placeholder
};

struct ParticleSysEntry {
    u32 unk0;
    u32 unk4;
    u32 unk8;
    void* system; // at +0xc: pointer to the Particle::System
};

extern struct ParticleSysTracker* PARTICLE_SYS_TRACKER;

extern struct ParticleSysEntry* _ZNK8Particle10SysTracker8Contents8FindDataEj(void* contents, u32 uniqueID);

void* _ZN8Particle6System12FromUniqueIDEj(u32 uniqueID) {
    struct ParticleSysEntry* entry = _ZNK8Particle10SysTracker8Contents8FindDataEj(
        (unsigned char*)PARTICLE_SYS_TRACKER + 8, uniqueID);
    return entry->system;
}