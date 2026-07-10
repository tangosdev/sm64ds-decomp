// Latches +0x8/+0xc into +0x10/+0x14, then unless byte +0x44 == 1 or +0x40 <= 0
// eases the s16 angle at +0x32 toward 0x3000 by 0x200 steps (clamped), and tail
// calls func_ov006_0210d93c.
typedef short s16;
typedef unsigned char u8;

typedef struct Obj {
    char _pad0[8];     // 0x00
    int x8;            // 0x08
    int xc;            // 0x0c
    int x10;           // 0x10
    int x14;           // 0x14
    char _pad18[0x1a]; // 0x18
    s16 x32;           // 0x32
    char _pad34[0xc];  // 0x34
    int x40;           // 0x40
    u8 x44;            // 0x44
} Obj;

extern void func_ov006_0210d93c(Obj* self);

void func_ov006_0210e014(Obj* self) {
    self->x10 = self->x8;
    self->x14 = self->xc;
    if (self->x44 != 1 && self->x40 > 0) {
        if (self->x32 < 0x3000) {
            (*(s16*)(((long long)(int)((char*)self + 0x32)) & 0xFFFFFFFFFFFFFFFFLL)) += 0x200;
            if (self->x32 >= 0x3000) self->x32 = 0x3000;
        } else {
            (*(s16*)(((unsigned long long)(unsigned int)((char*)self + 0x32)) & 0xFFFFFFFFFFFFFFFFLL)) -= 0x200;
            if (self->x32 < 0x3000) self->x32 = 0x3000;
        }
    }
    func_ov006_0210d93c(self);
}
