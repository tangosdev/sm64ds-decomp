// Reset: forwards self to func_ov006_02114738, fills data_ov006_02142c1c with
// 0..8, calls func_ov006_0210f998, then zeroes the per-slot state arrays
// (+0x31/+0x3c/+0x60 x9, +0x6c/+0x8c x8) and +0x94/+0x98.
typedef unsigned char u8;

typedef struct Obj {
    char _pad0[0x31]; // 0x00
    u8 byteA[9];      // 0x31
    char _pad3a[2];   // 0x3a
    int intB[9];      // 0x3c
    u8 byteC[9];      // 0x60
    char _pad69[3];   // 0x69
    int intD[8];      // 0x6c
    u8 byteE[8];      // 0x8c
    int x94;          // 0x94
    int x98;          // 0x98
} Obj;

extern int data_ov006_02142c1c[9];
extern void func_ov006_02114738(int* p);
extern void func_ov006_0210f998(void);

void func_ov006_0210f914(Obj* self) {
    int i;
    func_ov006_02114738((int*)self);
    for (i = 0; i < 9; i++)
        data_ov006_02142c1c[i] = i;
    func_ov006_0210f998();
    for (i = 0; i < 9; i++) {
        self->byteA[i] = 0;
        self->intB[i] = 0;
        self->byteC[i] = 0;
    }
    for (i = 0; i < 8; i++) {
        self->intD[i] = 0;
        self->byteE[i] = 0;
    }
    self->x94 = 0;
    self->x98 = 0;
}
