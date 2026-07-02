typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct WarpEntry {
    u8 a;
    u8 b;
    u8 c;
    u8 d;
} WarpEntry;

extern u8 data_020a0e40;
extern WarpEntry data_020a0de8[];

void func_ov006_02105730(char* thiz) {
    u8 idx;
    s32 b;
    s32 dx;
    s32 dy;

    if (*(u8*)(thiz + 0x4fe9) != 0) {
        return;
    }
    idx = data_020a0e40;
    b = 0;
    if (data_020a0de8[idx].a != 0) {
        b = (data_020a0de8[idx].b != 0);
    }
    if (b == 0) {
        return;
    }
    dx = data_020a0de8[idx].c - 0xe8;
    dy = data_020a0de8[idx].d - 0x10;
    if (dx < -15) {
        return;
    }
    if (dx > 15) {
        return;
    }
    if (dy < -12) {
        return;
    }
    if (dy > 12) {
        return;
    }
    *(u8*)(thiz + 0x4fe0) = 1;
    *(u32*)(thiz + 0x4ca8) = 5;
    *(u16*)(thiz + 0x4ec4) = 0x20;
}
