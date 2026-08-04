// Integrates entry i (stride 0x1c, array at +0x5bb4): pos += vel; below the
// -0xc0 floor keep accelerating vel by -0x60, otherwise snap pos to -0xc0000
// and zero vel. Tail work delegated to func_ov006_020fb230.
typedef struct Ent {
    volatile int a; // 0x00 position (20.12); volatile: ROM reloads after the store
    int unk4;       // 0x04
    int b;          // 0x08 velocity (20.12)
    char _padC[0x10];
} Ent;

typedef struct Obj {
    char _pad0[0x5bb4]; // 0x0000
    Ent entries[8];     // 0x5bb4
} Obj;

extern void func_ov006_020fb230(Obj* self, int i);

void func_ov006_020fb45c(Obj* self, int i) {
    self->entries[i].a += self->entries[i].b;
    if (self->entries[i].a >> 12 >= -0xc0) {
        self->entries[i].a = -0xc0000;
        self->entries[i].b = 0;
    } else {
        self->entries[i].b -= 0x60;
    }
    func_ov006_020fb230(self, i);
}
