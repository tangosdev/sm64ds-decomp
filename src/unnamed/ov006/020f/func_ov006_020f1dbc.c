// Wraps two per-index 20.12 fixed-point coords (arrays at +0x47f8 and +0x49d8)
// around their ranges: >0x110 -> -0x10000 / < -0x10 -> 0x110000, and
// >0xd0 -> -0x10000 / < -0x10 -> 0xd0000. Leaf, no callees.
typedef struct Obj {
    char _pad0[0x47f8]; // 0x0000
    int arrA[120];      // 0x47f8
    int arrB[120];      // 0x49d8
} Obj;

void func_ov006_020f1dbc(Obj* self, int i) {
    int a = self->arrA[i] >> 12;
    int b = self->arrB[i] >> 12;
    if (a > 0x110) self->arrA[i] = -0x10000;
    if (a < -0x10) self->arrA[i] = 0x110000;
    if (b > 0xd0)  self->arrB[i] = -0x10000;
    if (b < -0x10) self->arrB[i] = 0xd0000;
}
