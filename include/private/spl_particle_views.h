#ifndef SPL_PARTICLE_VIEWS_H
#define SPL_PARTICLE_VIEWS_H

#include "types.h"

/* C views of the particle code's records, shared by the particle draw routines
 * (func_0204b028 .. func_0204be40), the child spawn (func_0204c304) and the
 * particle animation callbacks (SPLAnim_ChildScale .. SPLAnim_Scale). Each view
 * names the fields those files read. Unread bytes stay padding.
 *
 * The SPL field names here (such as dbbScale, textureTileCountS/T,
 * polygonRotAxis, polygonReferencePlane, polygonIDs) are pret's SPL
 * coinages, kept so the two can be read side by side. */

/* The animation records a definition points at. Each animation callback
 * defines the one it reads. */
struct ScaleAnim; /* SPLAnim_Scale */
struct ColorRamp; /* SPLAnim_Color */
struct AlphaAnim; /* SPLAnim_Alpha */
struct TexAnim;   /* SPLAnim_Texture */

/* The particle definition's header (func_0204a730's PtclData; SPL's
 * SPLResourceHeader). */
typedef struct PtclData {
    u32 emissionType : 4;    /* bits 0-3 -- the emitter shape */
    u32 drawType : 2;        /* 4-5 -- picks the per-particle draw routine */
    u32 circleAxis : 2;      /* 6-7 -- 3: the system's own basis */
    u32 hasScaleAnim : 1;    /* 8 */
    u32 hasColorAnim : 1;    /* 9 */
    u32 hasAlphaAnim : 1;    /* 10 */
    u32 hasTexAnim : 1;      /* 11 -- rebind each particle's own texture */
    u32 hasRotation : 1;     /* 12 -- random angular speed in [min, max] */
    u32 randomInitAngle : 1; /* 13 */
    u32 : 2;
    u32 hasChildResource : 1;      /* bit 16 -- also draw the child particles */
    u32 polygonRotAxis : 2;        /* bits 17-18 -- data_02099fbc index */
    u32 polygonReferencePlane : 1; /* bit 19 -- data_02099fb4 index */
    u32 : 12;
    int rate;             /* 0x04 */
    int emitRadius;       /* 0x08 -- System::emitRadius */
    u16 axis[3];          /* 0x0c -- System::direction; read unsigned (ldrh) */
    u16 color;            /* 0x12 -- the particle's base colour, the colour ramp's middle stop */
    int callbackParam;    /* 0x14 -- System::callbackParam */
    int callbackVelocity; /* 0x18 -- System::callbackVelocity */
    int callbackScale;    /* 0x1c -- System::callbackScale */
    s16 aspectRatio;      /* 0x20 -- x scale over y scale */
    char pad_22[2];
    s16 minRotation; /* 0x24 */
    s16 maxRotation; /* 0x26 */
    char pad_28[2];
    u16 particleLifetime; /* 0x2a -- System::particleLifetime */
    u8 scaleRandom;       /* 0x2c */
    u8 lifetimeRandom;    /* 0x2d */
    u8 velocityRandom;    /* 0x2e */
    char pad_2f[1];
    u8 emitInterval; /* 0x30 -- System::emitInterval */
    u8 baseAlpha;    /* 0x31 */
    char pad_32[1];
    u8 texture;                /* 0x33 -- index into the manager's texture records */
    u32 loopFrames : 8;        /* 0x34 */
    u32 dbbScale : 16;         /* bits 8-23 -- y stretch when flying across the view */
    u32 textureTileCountS : 2; /* bits 24-25 */
    u32 textureTileCountT : 2; /* bits 26-27 */
    u32 : 4;
} PtclData;

/* The child record's flags (SPL's SPLChildResourceFlags). rotationType 0
 * starts a child unrotated, 1 copies the parent's angle, 2 also its angular
 * speed. useChildColor takes the record's color over the parent's. */
typedef struct PtclChildFlags {
    u16 : 3;
    u16 rotationType : 2; /* bits 3-4 */
    u16 : 1;
    u16 useChildColor : 1;         /* bit 6 */
    u16 drawType : 2;              /* bits 7-8 -- picks the child draw routine */
    u16 polygonRotAxis : 2;        /* bits 9-10 -- data_02099fbc index */
    u16 polygonReferencePlane : 1; /* bit 11 -- data_02099fb4 index */
    u16 : 4;
} PtclChildFlags;

/* The child record (func_0204a730's PtclChild; SPL's SPLChildResource): the
 * particles a particle emits. */
typedef struct PtclChild {
    PtclChildFlags flags; /* 0x00 */
    s16 spread;           /* 0x02 -- random velocity spread */
    s16 endScale;         /* 0x04 -- a child's scale when its life ends, 0x1000 = 1.0 */
    u16 lifetime;         /* 0x06 */
    u8 velocityRatio;     /* 0x08 -- a child inherits (velocityRatio + 1) / 256 of the velocity */
    u8 scaleRatio;        /* 0x09 -- and (scaleRatio + 1) / 64 of the scale */
    u16 color;            /* 0x0a */
    u8 count;             /* 0x0c */
    u8 pad_0d[2];         /* 0x0d -- func_0204a730's emission delay and interval */
    u8 texture;           /* 0x0f */
    u32 textureTileCountS : 2; /* 0x10 bits 0-1 */
    u32 textureTileCountT : 2; /* bits 2-3 */
    u32 : 28;
} PtclChild;

/* The particle definition (func_0204a730's PtclDef; SPL's SPLResource). */
typedef struct PtclDef {
    PtclData *data;              /* 0x00 */
    struct ScaleAnim *scaleAnim; /* 0x04 */
    struct ColorRamp *colorAnim; /* 0x08 */
    struct AlphaAnim *alphaAnim; /* 0x0c */
    struct TexAnim *texAnim;     /* 0x10 */
    PtclChild *child;            /* 0x14 -- present when data's flags bit 16 is set */
} PtclDef;

/* The polygon IDs a system's particles cycle through, one per particle. */
typedef struct PolygonIDs {
    u32 min : 6;
    u32 max : 6;
    u32 current : 6;
    u32 : 14;
} PolygonIDs;

/* One of a system's particle lists (Particle::ParticleList). */
typedef struct ParticleList {
    struct ParticleElement *head; /* 0x00 */
    int count;                    /* 0x04 */
} ParticleList;

/* Particle::System (include/Particle__System.h), whose names this reuses. */
typedef struct ParticleSystem {
    void *next;              /* 0x00 */
    void *previous;          /* 0x04 */
    ParticleList particles;  /* 0x08 */
    ParticleList particles2; /* 0x10 -- the children */
    PtclDef *mDefinition;    /* 0x18 */
    int callbackFlags;       /* 0x1c */
    Vector3 position;        /* 0x20 */
    Vector3 velocity;        /* 0x2c */
    u16 age;                 /* 0x38 */
    s16 callbackValue;       /* 0x3a */
    s16 direction[3];        /* 0x3c */
    char pad_42[2];
    int emitRadius;       /* 0x44 */
    int callbackParam;    /* 0x48 */
    int callbackVelocity; /* 0x4c */
    int callbackScale;    /* 0x50 */
    u16 particleLifetime; /* 0x54 */
    char pad_56[2];
    u8 emitInterval; /* 0x58 */
    u8 baseAlpha;    /* 0x59 */
    char pad_5a[2];
    Vector3s basisU;       /* 0x5c -- with basisV, perpendicular to direction */
    Vector3s basisV;       /* 0x62 */
    void *callback;        /* 0x68 */
    int m6c;               /* 0x6c */
    int m70;               /* 0x70 */
    PolygonIDs polygonIDs; /* 0x74 */
} ParticleSystem;

/* The particle's 0x40 halfword (SPL's visibility). */
typedef struct Visibility {
    u16 baseAlpha : 5;
    u16 animAlpha : 5;
    u16 : 6;
} Visibility;

/* Particle::Element (include/Particle__Element.h) and the fields past its
 * 0x30 bytes, named as in func_0204c584.c. */
typedef struct ParticleElement {
    struct ParticleElement *next;     /* 0x00 */
    struct ParticleElement *previous; /* 0x04 */
    Vector3 basePosition;             /* 0x08 */
    Vector3 offset;                   /* 0x14 */
    Vector3 velocity;                 /* 0x20 */
    u16 lifetime;                     /* 0x2c */
    u16 age;                          /* 0x2e */
    int scale;                        /* 0x30 */
    s16 animScale;                    /* 0x34 */
    u16 angle;                        /* 0x36 */
    s16 angularSpeed;                 /* 0x38 */
    u16 color;                        /* 0x3a */
    u16 loopTimeFactor;               /* 0x3c */
    u16 lifeRateFactor;               /* 0x3e */
    Visibility visibility;            /* 0x40 */
    u8 texture;                       /* 0x42 -- the index of the particle's current texture */
} ParticleElement;

/* One 0x14-byte texture record: what func_0204af3c (texture, palette, size)
 * binds. */
typedef struct TextureView {
    u32 pad[5];
} TextureView;

/* Particle::Manager (include/Particle__Manager.h is C++), named as there. */
typedef struct ManagerView {
    char pad_00[0x20];
    TextureView *mTextures; /* 0x20 */
    char pad_24[0x30 - 0x24];
    u32 mPolygonAttr;               /* 0x30 */
    ParticleSystem *mCurrentSystem; /* 0x34 -- the system being drawn */
    int *mDrawMatrix;               /* 0x38 */
} ManagerView;

#endif
