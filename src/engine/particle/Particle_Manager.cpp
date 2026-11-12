//cpp
/* engine/particle/Particle_Manager.cpp -- Particle::Manager, arm9
 * 0x02049d60..0x0204a5bc.
 *
 * Contiguous manager run, one function per legacy file until this fold.
 * Above it, func_02049cd8 (0x02049cd8..0x02049d60) is the sound-context
 * switch Sound.cpp calls, not a Manager method. Below it, func_0204a5bc
 * (0x0204a5bc) is a tail veneer into func_0204c584, and the system update
 * and draw helpers continue from there. func_0204d958 / func_0204d9a0 /
 * func_0204ae2c, the list and init callees AddSystem names, sit later
 * (0x0204ae2c, 0x0204d958, 0x0204d9a0) and are not part of this span.
 * Particle_Behavior.cpp owns 0x0204d4d0..0x0204d8e8, so this run cannot
 * move into Particle.cpp (0x020226d4..0x020233d4) either.
 *
 * mwccarm emits .text in reverse source order, so the definitions below
 * run ROM-descending.
 *
 *   func_0204a4c8  construct the 0x3c manager and its free lists
 *   func_0204a17c  parse the resource into mDefinitions and mTextures
 *   func_0204a0dc  upload textures
 *   func_0204a028  upload palettes
 *   func_02049f58  retire expired systems (dPa_c::Update)
 *   func_02049ee8  draw active systems (RenderAll)
 *   AddSystem      spawn one system
 *   func_02049d78  return one system's particles and the system itself
 *   func_02049d60  hand a system the free-particle list
 */
#include "types.h"
#include "nitro/gx/gx_load.h"
#include "nitro/gx/g3x.h"
#include "Particle__Behavior.h"
#include "Particle__Manager.h"
#include "Particle__System.h"
#include "private/particle_resource_header.h"

extern "C" {
/* local extern: decl_common.h, a C catch-all, declares func_0204d958,
 * func_0204d9a0 and func_0204ae2c; no real header declares any of these. */
void *func_0204d958(void *list);
void func_0204d9a0(void *list, void *node);
void *func_0204d8e8(void *list, void *node);
void func_0204ae2c(Particle::System *system, Particle::SystemDefinition *definition,
                   Vector3 *position);
void func_0204a5bc(void *system, void *list);
void func_0204a5c8(void *manager);
void func_0204a730(void *manager, void *system);
}

typedef Particle::Manager::TextureEntry TextureEntry;

/* SystemDefinition::Data::flags as func_0204a17c reads it. Each set bit means
   one more record follows the 0x38-byte Data record, in bit order. */
struct DefinitionFlags {
    unsigned : 8;
    unsigned hasOptionalA : 1;
    unsigned hasOptionalB : 1;
    unsigned hasOptionalC : 1;
    unsigned hasOptionalD : 1;
    unsigned : 4;
    unsigned hasChild : 1;
    unsigned : 7;
    unsigned hasAcceleration : 1;
    unsigned hasJitter : 1;
    unsigned hasConverge : 1;
    unsigned hasTurn : 1;
    unsigned hasLimitPlane : 1;
    unsigned hasRadiusConverge : 1;
    unsigned : 2;
};

/* Particle::System::callbackFlags as the update and draw walks test it. */
struct SystemState {
    u32 stop : 1;
    u32 : 1;
    u32 skipUpdate : 1;
    u32 skipDraw : 1;
    u32 : 28;
};

static inline SystemState &StateOf(Particle::System *system) {
    return *(SystemState *)&system->callbackFlags;
}

extern "C" {

char *func_0204a4c8(char *(*alloc)(int), int numSystems, int numParticles, int minPolygonID,
                    unsigned short maxPolygonID) {
    int i;
    Particle::Manager *manager;
    char *system;
    char *particle;
    manager = (Particle::Manager *)alloc(sizeof(Particle::Manager));
    manager->mNumSystems = numSystems;
    manager->mNumParticles = numParticles;
    manager->mMinPolygonID = minPolygonID;
    manager->mMaxPolygonID = maxPolygonID;
    manager->allocator = alloc;
    manager->mActiveSystems.count = 0;
    manager->mFreeSystems.count = 0;
    manager->mFreeParticles.count = 0;
    manager->mActiveSystems.head = 0;
    manager->mFreeSystems.head = 0;
    manager->mFreeParticles.head = 0;
    manager->mPolygonAttr = 0;
    system = alloc(numSystems * sizeof(Particle::System));
    for (i = 0; i < numSystems; i++) {
        func_0204d9a0(&manager->mFreeSystems, system);
        system += sizeof(Particle::System);
    }
    particle = alloc(numParticles * 0x44);
    for (i = 0; i < numParticles; i++) {
        func_0204d9a0(&manager->mFreeParticles, particle);
        particle += 0x44;
    }
    manager->mDefinitions = 0;
    manager->mTextures = 0;
    manager->mNumTextures = 0;
    manager->mNumDefinitions = manager->mNumTextures;
    return (char *)manager;
}

void func_0204a17c(Particle::Manager *manager, char *resource) {
    int offset = sizeof(ParticleResourceHeader);
    int i;

    manager->mNumDefinitions = ((ParticleResourceHeader *)resource)->numDefinitions;
    manager->mNumTextures = ((ParticleResourceHeader *)resource)->numTextures;
    manager->mDefinitions = (Particle::SystemDefinition *)manager->allocator(
        manager->mNumDefinitions * sizeof(Particle::SystemDefinition));

    for (i = 0; i < manager->mNumDefinitions; i++) {
        Particle::SystemDefinition *definition = &manager->mDefinitions[i];
        definition->data = (Particle::SystemDefinition::Data *)(resource + offset);
        offset += 0x38;
        DefinitionFlags flags = *(DefinitionFlags *)definition->data;

        if (flags.hasOptionalA) {
            definition->optionalA = resource + offset;
            offset += 0xc;
        } else
            definition->optionalA = 0;

        if (flags.hasOptionalB) {
            definition->optionalB = resource + offset;
            offset += 0xc;
        } else
            definition->optionalB = 0;

        if (flags.hasOptionalC) {
            definition->optionalC = resource + offset;
            offset += 8;
        } else
            definition->optionalC = 0;

        if (flags.hasOptionalD) {
            definition->optionalD = resource + offset;
            offset += 0xc;
        } else
            definition->optionalD = 0;

        if (flags.hasChild) {
            definition->child = resource + offset;
            offset += 0x14;
        } else
            definition->child = 0;

        definition->numBehaviors = flags.hasAcceleration + flags.hasJitter + flags.hasConverge +
                                   flags.hasTurn + flags.hasLimitPlane + flags.hasRadiusConverge;

        if (definition->numBehaviors != 0) {
            definition->behaviors =
                (SPLBehavior *)manager->allocator(definition->numBehaviors * sizeof(SPLBehavior));
            SPLBehavior *behavior = definition->behaviors;

            if (flags.hasAcceleration) {
                behavior->object = resource + offset;
                behavior->applyFunc = SPLBehavior_ApplyGravity;
                offset += sizeof(SPLGravityBehavior);
                behavior++;
            }
            if (flags.hasJitter) {
                behavior->object = resource + offset;
                behavior->applyFunc = SPLBehavior_ApplyRandom;
                offset += sizeof(SPLRandomBehavior);
                behavior++;
            }
            if (flags.hasConverge) {
                behavior->object = resource + offset;
                behavior->applyFunc = SPLBehavior_ApplyMagnet;
                offset += sizeof(SPLMagnetBehavior);
                behavior++;
            }
            if (flags.hasTurn) {
                behavior->object = resource + offset;
                behavior->applyFunc = SPLBehavior_ApplySpin;
                offset += sizeof(SPLSpinBehavior);
                behavior++;
            }
            if (flags.hasLimitPlane) {
                behavior->object = resource + offset;
                behavior->applyFunc = SPLBehavior_ApplyCollisionPlane;
                offset += sizeof(SPLCollisionPlaneBehavior);
                behavior++;
            }
            if (flags.hasRadiusConverge) {
                behavior->object = resource + offset;
                behavior->applyFunc = SPLBehavior_ApplyConvergence;
                offset += sizeof(SPLConvergenceBehavior);
                behavior++;
            }
        } else {
            definition->behaviors = 0;
        }
    }

    manager->mTextures =
        (TextureEntry *)manager->allocator(manager->mNumTextures * sizeof(TextureEntry));

    for (i = 0; i < manager->mNumTextures; i++) {
        TextureEntry *texture = &manager->mTextures[i];
        TextureEntry::Resource *record = (TextureEntry::Resource *)(resource + offset);
        texture->resource = record;
        texture->width = 1 << (record->params.widthLog + 3);
        texture->height = 1 << (record->params.heightLog + 3);
        texture->params = record->params;
        offset += record->size;
    }
}

int func_0204a0dc(Particle::Manager *manager, u32 (*allocTexVram)(u32, u32)) {
    int i;
    GX_BeginLoadTex();
    for (i = 0; i < manager->mNumTextures; i++) {
        TextureEntry *texture = &manager->mTextures[i];
        TextureEntry::Resource *record = texture->resource;
        u32 isTexel4x4 = (record->params.format == 5) ? 1 : 0;
        u32 address = allocTexVram(record->texelSize, isTexel4x4);
        /* The texels follow the 0x20-byte record header. */
        GX_LoadTex(texture->resource + 1, address, record->texelSize);
        texture->texelAddress = address;
    }
    GX_EndLoadTex();
    return 1;
}

int func_0204a028(Particle::Manager *manager,
                  unsigned int (*allocPalVram)(unsigned int, unsigned int)) {
    int i;

    GX_BeginLoadTexPltt();

    for (i = 0; i < manager->mNumTextures; i++) {
        TextureEntry *texture = &manager->mTextures[i];
        TextureEntry::Resource *record = texture->resource;
        unsigned int address = 0;

        if (record->paletteSize != 0) {
            unsigned int fromLowAddress = record->params.format == 2 ? 1 : 0;
            address = allocPalVram(record->paletteSize, fromLowAddress);
            GX_LoadTexPltt((char *)texture->resource + record->paletteOffset, address,
                           record->paletteSize);
        }
        texture->paletteAddress = address;
    }

    GX_EndLoadTexPltt();
    return 1;
}

void func_02049f58(Particle::Manager *manager) {
    Particle::System *system = manager->mActiveSystems.head;
    Particle::System *next;
    if (system == 0)
        return;
    do {
        Particle::SystemDefinition::Data *data = system->mDefinition->data;
        next = system->next;
        if (StateOf(system).skipUpdate == 0) {
            func_0204a730(manager, system);
        }
        if (((data->flags << 17) >> 31 != 0 && data->callbackValue != 0 &&
             system->age > data->callbackValue) ||
            StateOf(system).stop != 0) {
            if (system->particles.count == 0 && system->particles2.count == 0) {
                func_0204d9a0(&manager->mFreeSystems,
                              func_0204d8e8(&manager->mActiveSystems, system));
            }
        }
        system = next;
    } while (next != 0);
}

void func_02049ee8(Particle::Manager *manager, Matrix4x3 *matrix) {
    Particle::System *system;
    G3X_AlphaBlend(true);
    manager->mDrawMatrix = matrix;
    system = manager->mActiveSystems.head;
    if (system == 0)
        return;
    do {
        manager->mCurrentSystem = system;
        if (StateOf(system).skipDraw == 0) {
            func_0204a5c8(manager);
        }
        system = system->next;
    } while (system != 0);
}
}

/* Particle::Manager::AddSystem -- spawn a System, arm9
 * 0x02049e0c..0x02049ee8. func_0204d958 pops one off mFreeSystems,
 * func_0204ae2c initializes it from the indexed SystemDefinition, the
 * manager's polygon-ID range seeds the system's, func_0204d9a0 pushes
 * it onto mActiveSystems, and a definition whose flags carry the
 * self-destruct bit fails the spawn.
 *
 * leftovers: `(flags << 17) >> 31` stays the arithmetic-shift bit-14 test
 * -- `& 0x4000` emits a different sequence. The polygon-ID names come from
 * the draw routines (func_0204be40 and its siblings), which put
 * polygonIDs.current into POLYGON_ATTR bits 24-29 and wrap it from max to
 * min; dPa_c::Initialise passes 0x1a and 0x3e, both 6-bit IDs.
 */
namespace Particle {

System *Manager::AddSystem(int definitionID, Vector3 &position) {
    System *result;
    System *system;

    result = 0;
    if (mFreeSystems.head != 0) {
        system = (System *)func_0204d958(&mFreeSystems);
        result = system;
        func_0204ae2c(system, &mDefinitions[definitionID], &position);

        system->polygonIDs.min = mMinPolygonID;
        system->polygonIDs.max = mMaxPolygonID;
        system->polygonIDs.current = system->polygonIDs.min;
        system->polygonIDs.unused = 0;

        func_0204d9a0(&mActiveSystems, system);
        if ((system->mDefinition->data->flags << 17) >> 31)
            result = 0;
    }

    return result;
}

}

extern "C" {

void func_02049d78(Particle::Manager *manager, Particle::System *system) {
    void *particle;
    particle = func_0204d958(&system->particles);
    while (particle != 0) {
        func_0204d9a0(&manager->mFreeParticles, particle);
        particle = func_0204d958(&system->particles);
    }
    particle = func_0204d958(&system->particles2);
    while (particle != 0) {
        func_0204d9a0(&manager->mFreeParticles, particle);
        particle = func_0204d958(&system->particles2);
    }
    func_0204d8e8(&manager->mActiveSystems, system);
    func_0204d9a0(&manager->mFreeSystems, system);
}

void func_02049d60(Particle::Manager *manager, Particle::System *system) {
    func_0204a5bc(system, &manager->mFreeParticles);
}
}
