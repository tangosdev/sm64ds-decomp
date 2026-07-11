// Wraps the five stride-8 fixed-point values at +0x18 by +/-0x170000 when the
// signed drift at +0x76 pushes entry 0 past -0x60000 / +0x160000. Leaf, no callees.
typedef short s16;

typedef struct Ent {
    int val;  // 0x0
    int unk4; // 0x4
} Ent;

typedef struct Obj {
    char _pad0[0x18];  // 0x00
    Ent entries[5];    // 0x18
    char _pad40[0x36]; // 0x40
    s16 unk76;         // 0x76
} Obj;

#pragma opt_strength_reduction off
void func_ov006_020ec458(Obj* self) {
    int i;
    s16 v = self->unk76;
    if (v < 0 && self->entries[0].val < -0x60000) {
        for (i = 0; i < 5; i++)
            (*(int*)(((long long)(int)((char*)self + (i << 3) + 0x18)) & 0xFFFFFFFFFFFFFFFFLL)) += 0x170000;
        return;
    }
    if (v > 0 && self->entries[0].val > 0x160000) {
        for (i = 0; i < 5; i++)
            (*(int*)(((unsigned long long)(unsigned int)((char*)self + (i << 3) + 0x18)) & 0xFFFFFFFFFFFFFFFFLL)) -= 0x170000;
    }
}
