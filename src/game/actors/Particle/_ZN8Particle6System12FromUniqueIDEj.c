#include "types.h"
// @symbol _ZN8Particle6System12FromUniqueIDEj
// Particle::System::FromUniqueID - looks up a Particle::System by unique ID
#include "Particle.h"

struct ParticleSysEntry {
    u32 unk0;
    u32 unk4;
    u32 unk8;
    void* system; // at +0xc: pointer to the Particle::System
};

extern struct Particle* data_0209ee74;

extern struct ParticleSysEntry* _ZNK8Particle10SysTracker8Contents8FindDataEj(void* contents, u32 uniqueID);

void* _ZN8Particle6System12FromUniqueIDEj(u32 uniqueID) {
    struct ParticleSysEntry* entry = _ZNK8Particle10SysTracker8Contents8FindDataEj(
        &data_0209ee74->mContents, uniqueID);
    return entry->system;
}
