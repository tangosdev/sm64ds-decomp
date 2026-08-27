//cpp
struct State {};
struct dBgCh_Actr {
    int IsOnGround() const;
    int GetFloorResult() const;
};
struct Player {
    int IsState(State &s);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int _ZN6Player7IsStateERNS_5StateE(void *, State &s);


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
            if (_ZN6Player7IsStateERNS_5StateE(self, data_ov002_0211013c)) {
                *(short *)(base + 0x6bc) = 0x1e;
            } else if (_ZN6Player7IsStateERNS_5StateE(self, data_ov002_021101b4)
                       && *(int *)(base + 0xa8) < 0
                       && (*(int *)(base + 0x684) - *(int *)(base + 0x60)) < 0x64000) {
                *(short *)(base + 0x6bc) = 0x1e;
            }
        }
        *(unsigned char *)(base + 0x6eb) = 0x80;
    } else if (flags & 0x80) {
        if (((dBgCh_Actr *)(base + 0x380))->IsOnGround()) {
            int r = func_02037e38((unsigned int *)((char *)((dBgCh_Actr *)(base + 0x380))->GetFloorResult() + 4));
            if (r >= 6 && r <= 9) return;
            *(unsigned char *)(base + 0x6eb) = 0;
            *(short *)(base + 0x6bc) = 0;
        } else {
            *(unsigned char *)(base + 0x6eb) = 0;
            *(short *)(base + 0x6bc) = 0;
        }
    }
}
