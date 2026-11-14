#ifndef PARTICLE__SYSTEM_H
#define PARTICLE__SYSTEM_H

#include "Particle__Manager.h"
#include "dPa_c.h"

/* System::New's compiler-produced name distinguishes this six-byte particle
 * direction from the actor-facing Vector3_16 type. */
#ifndef VECTOR3_16F_DEFINED
#define VECTOR3_16F_DEFINED
struct Vector3_16f { s16 x, y, z; };
#endif

namespace Particle {

/* The list element's exact source type and member spellings are not
 * RTTI-backed. The water callbacks prove the intrusive link and the two
 * ASR-3 position vectors below; the names are descriptive. */
struct ParticleNode {
    ParticleNode *next;          /* 0x00 */
    ParticleNode *previous;      /* 0x04 */
    Vector3 positionAsr3;        /* 0x08 */
    Vector3 offsetAsr3;          /* 0x14 */
    /* Splash callbacks test the y component before expiring the particle. */
    Vector3 velocityAsr3;        /* 0x20 */
    /* Water/lava boundary callbacks expire a particle by copying the first
     * value into the second. Roles and spellings are descriptive/inferred. */
    u16 lifetime;                /* 0x2c */
    u16 age;                     /* 0x2e */
};

struct ParticleList {
    ParticleNode *head;
    s32 count;
};

struct System {
    /* The polygon IDs the system's particles cycle through, one per drawn
       particle: the draw routines put `current` into POLYGON_ATTR bits
       24-29 and wrap it from max back to min. AddSystem seeds min and max
       from the manager. */
    struct PolygonIDs {
        u32 min : 6;
        u32 max : 6;
        u32 current : 6;
        u32 unused : 14;
    };

    /* The manager's free/active list link: func_0204d958/func_0204d9a0
       maintain it and the update and draw walks follow it. */
    System *next;                /* 0x00 */
    System *previous;            /* 0x04 */
    ParticleList particles;      /* 0x08 */
    ParticleList particles2;     /* 0x10 */
    SystemDefinition *mDefinition; /* 0x18 */
    /* Bit 0 retires the system once both particle lists are empty, bit 2
       skips its update (func_0204a730) and bit 3 its draw (func_0204a5c8).
       Callback virtuals toggle bit 1 while spawning or suppressing
       particles. */
    u32 callbackFlags;             /* 0x1c */
    Vector3 position;              /* 0x20 */
    /* Added to every particle's offset each frame (func_0204a730); the
       spawn (func_0204ae2c) clears it. */
    Vector3 velocity; /* 0x2c */
    /* func_0204a730 advances it; the update walk retires the system once it
       passes the definition's callbackValue. */
    u16 age; /* 0x38 */
    /* simpleCallback_c copies this value in both directions. */
    s16 callbackValue;             /* 0x3a */
    Vector3_16f direction;         /* 0x3c */
    u8 pad_042[0x02];
    /* func_0204ae2c copies it from the definition and func_0204c584 scales
       the spawn offset of every emitter shape by it. */
    s32 emitRadius; /* 0x44 */
    /* Descriptive names for callback-owned, body-proven fields. */
    s32 callbackParam;             /* 0x48 */
    s32 callbackVelocity;          /* 0x4c */
    s32 callbackScale;             /* 0x50 */
    /* func_0204ae2c copies it from the definition; func_0204c584 seeds each
       particle's lifetime from it. */
    u16 particleLifetime; /* 0x54 */
    u8 pad_056[0x02];
    /* func_0204ae2c copies it from the definition and func_0204a730 spawns
       when age % emitInterval == 0. Callers overwrite it: ov007's figure
       trail stores 1..5, fewer the faster the figure moves, and the
       trampoline minigame ramps it. */
    u8 emitInterval; /* 0x58 */
    /* func_0204ae2c copies it from the definition; func_0204c584 stores it
       in each new particle's visibility. */
    u8 baseAlpha; /* 0x59 */
    u8 pad_05a[0x02];
    /* Two axes spanning the plane perpendicular to `direction`, rebuilt by
       func_0204cebc as cross products: basisU = direction x reference,
       basisV = direction x basisU. func_0204cd5c maps a 2D emitter offset
       onto that plane through them. */
    Vector3_16f basisU; /* 0x5c */
    Vector3_16f basisV; /* 0x62 */
    /* func_0204a730 calls it each update when set; func_0204ae2c clears it. */
    void (*callback)(void *); /* 0x68 */
    u8 pad_06c[0x08];
    PolygonIDs polygonIDs; /* 0x74 */

    static System *FromUniqueID(u32 uniqueID);
    /* Unnamed parameters: a name here numbers @NNN in every includer. */
    static void *NewSimple(u32, fx32, fx32, fx32);
    /* The callback is stored in the level emitter New() starts (level_c::emitter_c::mCallback). */
    static int New(u32, u32, fx32, fx32, fx32, const Vector3_16f *, dPa_c::level_c::callback_c *);
    static u32 NewRipple(fx32, fx32, fx32);
    static void NewBigSplash(fx32, fx32, fx32);
    /* One wrapper per dPa_c callback: each starts a level emitter with that
       callback and returns its id (the caller keeps it). Named for the
       ROM-RTTI callback class they pass (dPa_c::level_c::*Callback_c). */
    static u32 NewCheckWater(u32, u32, fx32, fx32, fx32, const Vector3_16f *);
    static u32 NewCheckWaterRipple(u32, u32, fx32, fx32, fx32, const Vector3_16f *);
    static u32 NewFitWater(u32, u32, fx32, fx32, fx32, const Vector3_16f *);
    static u32 NewFitWaterRaised(u32, u32, fx32, fx32, fx32, const Vector3_16f *);
    static u32 NewCheckYogan(u32, u32, fx32, fx32, fx32, const Vector3_16f *);
    static u32 NewCleanParticle(u32, u32, fx32, fx32, fx32, const Vector3_16f *);
    /* One fixed effect each, kept in dPa_c's own id slot (see the bank in
       dPa_c.h); named for the effect. */
    static u32 NewSplash(fx32, fx32, fx32);
    static u32 NewBubble(fx32, fx32, fx32);
    static u32 NewRedFlame(fx32, fx32, fx32);
    static u32 NewBlueFlame(fx32, fx32, fx32);
    static void NewFlamethrowerRedA(fx32, fx32, fx32, s16, s16);
    static void NewFlamethrowerRedB(fx32, fx32, fx32, s16, s16);
    static void NewFlamethrowerBlueA(fx32, fx32, fx32, s16, s16);
    static void NewFlamethrowerBlueB(fx32, fx32, fx32, s16, s16);
    /* Last parameter is u8. unsigned int shrinks a caller and reloads the
       argument as a word; the matched body loads a byte. */
    static u32 NewWeather(u32, u32, fx32, fx32, fx32, const Vector3_16f *, u8);
};

void RunningSlidingDustAt(fx32, fx32, fx32);
void MegaRunningSlidingDustAt(fx32, fx32, fx32);

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char System_size_must_be_0x78[sizeof(System) == 0x78 ? 1 : -1];
#endif

}

extern "C" dPa_c *gParticleControl;

#endif
