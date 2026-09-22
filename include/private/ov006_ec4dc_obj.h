#ifndef OV006_EC4DC_OBJ_H
#define OV006_EC4DC_OBJ_H
typedef int s32;
typedef short s16;
typedef struct { s32 x; s32 y; } Vec2i;
typedef struct Obj {
    Vec2i v0;
    Vec2i v1;
    s32 pad10[2];
    Vec2i arr[5];
    s32 f40;
    s32 f44;
    s32 pad48[8];
    s32 f68;
    s32 f6c;
    s32 pad70;
    s16 pad74;
    s16 s76;
    s16 pad78;
    s16 s7a[5];
    s16 s84;
} Obj;
#endif
