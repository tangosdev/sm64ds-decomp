#ifndef OAMANIMATION_H
#define OAMANIMATION_H

#include "types.h"

/* One entry of the frame table: the OAM attributes drawn for the frame and
 * how many ticks it is shown. */
typedef struct OamAnimFrame {
    s32 tile;     /* 0x00 */
    s32 duration; /* 0x04 */
} OamAnimFrame;

/* A small OAM animation player used by the entry scene. Names follow the
 * data flow into OAM::Render; the 0x2c-byte layout and empty lifecycle are
 * ROM-proven. This declaration is shared with the C animation helpers. */
struct OamAnimation {
    const OamAnimFrame *frames;  /* 0x00 */
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
    /* 0x28 -- which engine draws it: func_0202048c renders on the main
       screen for 0, the sub screen for 1, and both for 2 (the main copy
       0xc0 lower). func_02020820 stores its third argument here. Was
       unk_028. */
    s32 screen;

#ifdef __cplusplus
    OamAnimation();
    ~OamAnimation();
#endif
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char OamAnimFrame_size_must_be_8[sizeof(OamAnimFrame) == 8 ? 1 : -1];
typedef char OamAnimation_size_must_be_0x2c[sizeof(struct OamAnimation) == 0x2c ? 1 : -1];
#endif

#endif
