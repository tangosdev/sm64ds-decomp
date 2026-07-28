typedef unsigned char u8;
typedef signed int s32;

typedef struct Entry {
    u8 unk00; u8 unk01; u8 unk02; u8 unk03;
    s32 unk04; s32 unk08; s32 unk0c; s32 unk10;
    s32 unk14; s32 unk18; s32 unk1c;
} Entry;

typedef struct Obj {
    u8 pad0[0x4f38];
    Entry entries[22];
    u8 pad1[0x5fc8 - 0x51f8];
    s32 unk5fc8;
    u8 pad2[1];
    u8 unk5fcd;
} Obj;

#define M(p) ((long long)(int)(p) & 0xffffffffffffffffLL)
#define A1(off) ((int)M((int)M((char *)self + (int)M(i) * 32) + (off)))
#define A2(off) ((int)M((int)M((char *)self + (unsigned)i * 32) + (off)))
#define B1(off) (*(u8 *)A1(off))
#define W1(off) (*(s32 *)A1(off))
#define W2(off) (*(s32 *)A2(off))

void func_ov006_0212a654(Obj *self)
{
    int i;

    self->unk5fcd = 1;
    for (i = 0; i < 22; i++) {
        if (B1(0x4f38) == 0) continue;
        if (self->entries[i].unk02 == 1) { self->unk5fcd = 0; continue; }
        if (self->entries[i].unk01 == 0) { self->unk5fcd = 0; continue; }
        if (i == self->unk5fc8) continue;
        W1(0x4f48) += 0x100;
        if (W1(0x4f48) >= 0x4000) W1(0x4f48) = 0x4000;
        W2(0x4f3c) += self->entries[i].unk0c;
        W2(0x4f40) += W1(0x4f48);
        if (W2(0x4f40) >= 0x100000) B1(0x4f38) = 0;
        else self->unk5fcd = 0;
    }
}
