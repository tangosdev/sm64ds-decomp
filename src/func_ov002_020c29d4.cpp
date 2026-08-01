//cpp
struct State {};
struct WithMeshClsn {
    int IsOnGround() const;
    int GetFloorResult() const;
};
struct Player {
    int IsState(State &s);
};

extern "C" int Player_ScaleByCharFactor(void *c, int a);
extern "C" int func_02037e38(unsigned int *p);
extern State data_ov002_0211013c;
extern State data_ov002_021101b4;

extern "C" void func_ov002_020c29d4(Player *self);
extern "C" void func_ov002_020c29d4(Player *self)
{
    char *base = (char *)self;
    if (*(int *)(base + 8) != 1) return;
    if (*(unsigned char *)(base + 0x703) != 0) return;
    unsigned char flags = *(unsigned char *)(base + 0x6eb);
    if ((flags & 1) && !(flags & 0x80)) {
        if (*(int *)(base + 0x98) >= Player_ScaleByCharFactor(self, 0x28000)) {
            if (self->IsState(data_ov002_0211013c)) {
                *(short *)(base + 0x6bc) = 0x1e;
            } else if (self->IsState(data_ov002_021101b4)
                       && *(int *)(base + 0xa8) < 0
                       && (*(int *)(base + 0x684) - *(int *)(base + 0x60)) < 0x64000) {
                *(short *)(base + 0x6bc) = 0x1e;
            }
        }
        *(unsigned char *)(base + 0x6eb) = 0x80;
    } else if (flags & 0x80) {
        if (((WithMeshClsn *)(base + 0x380))->IsOnGround()) {
            int r = func_02037e38((unsigned int *)((char *)((WithMeshClsn *)(base + 0x380))->GetFloorResult() + 4));
            if (r >= 6 && r <= 9) return;
            *(unsigned char *)(base + 0x6eb) = 0;
            *(short *)(base + 0x6bc) = 0;
        } else {
            *(unsigned char *)(base + 0x6eb) = 0;
            *(short *)(base + 0x6bc) = 0;
        }
    }
}
