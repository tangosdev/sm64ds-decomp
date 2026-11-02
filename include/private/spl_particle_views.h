#ifndef SPL_PARTICLE_VIEWS_H
#define SPL_PARTICLE_VIEWS_H

#include "types.h"

/* C views of the particle code's records, shared by the particle draw routines
 * (func_0204b028 .. func_0204be40), the child spawn (func_0204c304) and the
 * particle animation callbacks (func_0204d104 .. func_0204d440). Each view
 * names the fields those files read. Unread bytes stay padding.
 *
 * The SPL field names here (such as dbbScale, textureTileCountS/T,
 * polygonRotAxis, polygonReferencePlane, polygonIDs) are pret's SPL
 * coinages, kept so the two can be read side by side. */

/* The animation records a definition points at. Each animation callback
 * defines the one it reads. */
struct ScaleAnim; /* func_0204d440 */
struct ColorRamp; /* func_0204d294 */
struct AlphaAnim; /* func_0204d1b4 */
struct TexAnim;   /* func_0204d150 */

/* The particle definition's header (func_0204a730's PtclData; SPL's
 * SPLResourceHeader). */
typedef struct PtclData {
    u32 : 17;
    u32 polygonRotAxis : 2;        /* bits 17-18 -- data_02099fbc index */
    u32 polygonReferencePlane : 1; /* bit 19 -- data_02099fb4 index */
    u32 : 12;
    char pad_04[0x12 - 4];
    u16 color; /* 0x12 -- the particle's base colour, the colour ramp's middle stop */
    char pad_14[0x20 - 0x14];
    s16 aspectRatio; /* 0x20 -- x scale over y scale */
    char pad_22[0x34 - 0x22];
    u32 : 8;                   /* 0x34 -- loopFrames */
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
    u16 : 2;                       /* bits 7-8 -- func_0204a5c8's drawType */
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

/* Particle::System, named as func_0204c584.c's view. */
typedef struct ParticleSystem {
    char pad_00[0x18];
    PtclDef *mDefinition; /* 0x18 */
    char pad_1c[0x74 - 0x1c];
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
    char pad_00[8];        /* 0x00 -- the particle list link */
    Vector3 basePosition;  /* 0x08 */
    Vector3 offset;        /* 0x14 */
    Vector3 velocity;      /* 0x20 */
    u16 lifetime;          /* 0x2c */
    u16 age;               /* 0x2e */
    int scale;             /* 0x30 */
    s16 animScale;         /* 0x34 */
    u16 angle;             /* 0x36 */
    s16 angularSpeed;      /* 0x38 */
    u16 color;             /* 0x3a */
    u16 loopTimeFactor;    /* 0x3c */
    u16 lifeRateFactor;    /* 0x3e */
    Visibility visibility; /* 0x40 */
    u8 texture;            /* 0x42 -- the index of the particle's current texture */
} ParticleElement;

/* Particle::Manager (include/Particle__Manager.h is C++), named as there. */
typedef struct ManagerView {
    char pad_00[0x30];
    u32 mPolygonAttr;               /* 0x30 */
    ParticleSystem *mCurrentSystem; /* 0x34 -- the system being drawn */
    int *mDrawMatrix;               /* 0x38 */
} ManagerView;

#endif
