#include "types.h"
typedef struct ParticleNode {
    void* next;
    void* prev;
} ParticleNode;

typedef struct Particle {
    ParticleNode node;         /* 0x00 */
    s32 posAsr3x;              /* 0x08 */
    s32 posAsr3y;              /* 0x0c */
    s32 posAsr3z;              /* 0x10 */
    s32 offsetAsr3x;           /* 0x14 */
    s32 offsetAsr3y;           /* 0x18 */
    s32 offsetAsr3z;           /* 0x1c */
} Particle;

typedef struct ParticleList {
    Particle* first;           /* 0x00 */
    u32 num;                   /* 0x04 */
} ParticleList;

typedef struct ParticleSysNode {
    void* prev;
    void* next;
} ParticleSysNode;

typedef struct ParticleSys {
    ParticleSysNode node;      /* 0x00 */
    ParticleList particleList; /* 0x08 */
} ParticleSys;

extern s32 WATER_HEIGHT;

extern int _ZN8Particle14SimpleCallback8OnUpdateERNS_6SystemEb(void* cb, ParticleSys* sys, int active);

int _ZN8Particle22FitWaterSimpleCallback8OnUpdateERNS_6SystemEb(void* cb, ParticleSys* sys, int active)
{
    s32 waterAsr3;
    Particle* p;

    p = sys->particleList.first;
    waterAsr3 = WATER_HEIGHT >> 3;
    if (p != 0)
    {
        do
        {
            p->offsetAsr3y = waterAsr3 - p->posAsr3y;
            p = (Particle*)p->node.next;
        } while (p != 0);
    }
    return _ZN8Particle14SimpleCallback8OnUpdateERNS_6SystemEb(cb, sys, active);
}
