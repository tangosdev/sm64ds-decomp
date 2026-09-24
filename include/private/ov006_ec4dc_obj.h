#ifndef OV006_EC4DC_OBJ_H
#define OV006_EC4DC_OBJ_H
typedef int s32;
typedef short s16;
typedef struct { s32 x; s32 y; } Vec2i;
/* Wiggler view used by func_ov006_020ec4dc. Same 0x98 object as struct C. */
typedef struct Obj {
    Vec2i state;       /* 0x00 */
    Vec2i saved;       /* 0x08 */
    s32 sub[2];        /* 0x10 */
    Vec2i seg[5];      /* 0x18 */
    s32 speed;         /* 0x40 */
    s32 speedTarget;   /* 0x44 */
    s32 pad48[8];
    s32 voice;         /* 0x68 */
    s32 voiceId;       /* 0x6c */
    s32 pad70;
    s16 pad74;
    s16 aim;           /* 0x76 */
    s16 pad78;
    s16 ang[5];        /* 0x7a */
    s16 bob;           /* 0x84 */
} Obj;
#endif
