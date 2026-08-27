#include "types.h"
struct Particle;

struct ParticleList {
    struct Particle* first;
    u32 num;
};

struct SystemNode {
    void* prev;
    void* next;
};

struct System {
    struct SystemNode node;
    struct ParticleList particleList;
    struct ParticleList particleList2;
};

struct CheckLavaCallback {
    u32 vtable;
};

typedef struct CheckLavaCallback CheckLavaCallback;
typedef struct System System;

extern int _ZN8Particle14SimpleCallback8OnUpdateERNS_6SystemEb(CheckLavaCallback* self, System* sys, int active);

int _ZN8Particle17CheckLavaCallback8OnUpdateERNS_6SystemEb(CheckLavaCallback* self, System* sys, int active) {
    if (!active) {
        if (sys->particleList.first == (struct Particle*)0) {
            return 0;
        }
    }
    return _ZN8Particle14SimpleCallback8OnUpdateERNS_6SystemEb(self, sys, active);
}
