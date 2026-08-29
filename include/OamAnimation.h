#ifndef OAMANIMATION_H
#define OAMANIMATION_H

#include "types.h"

/* A small OAM animation player used by the entry scene. The descriptive class
 * and member names are inferred from the data flow into OAM::Render; the
 * 0x2c-byte layout and its empty constructor/destructor are ROM-proven.
 */
class OamAnimation {
public:
    const void *frames;          /* 0x00 */
    s32 tile;                    /* 0x04 */
    s32 scaleX;                  /* 0x08 */
    s32 scaleY;                  /* 0x0c */
    s32 frameCount;              /* 0x10 */
    s32 frame;                   /* 0x14 */
    s32 frameDuration;           /* 0x18 */
    s32 elapsed;                 /* 0x1c */
    s16 rotation;                /* 0x20 */
    s8  palette;                 /* 0x22 */
    s8  priority;                /* 0x23 */
    u8  loops;                   /* 0x24 */
    u8  enabled;                 /* 0x25 */
    u8  affine;                  /* 0x26 */
    u8  pad_027;                 /* 0x27 */
    s32 unk_028;                 /* 0x28 */

    OamAnimation();
    ~OamAnimation();
};

typedef char OamAnimation_size_must_be_0x2c[sizeof(OamAnimation) == 0x2c ? 1 : -1];

#endif
