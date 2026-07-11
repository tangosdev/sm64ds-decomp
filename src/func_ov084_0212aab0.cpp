//cpp
struct WithMeshClsn { int JustHitGround() const; int IsOnGround() const; };
struct Actor { void HugeLandingDust(bool b); void LandingDust(bool b); };
void ApproachLinear(short &v, short t, short step);

#pragma optimize_for_size on
extern "C" void func_ov084_0212aab0(char *c)
{
    if (((WithMeshClsn *)(c + 0x1b4))->JustHitGround()) {
        int s = *(int *)(c + 0x460);
        switch (s) {
        case 2: ((Actor *)c)->HugeLandingDust(true); break;
        case 1: ((Actor *)c)->LandingDust(true); break;
        }
    }
    if (((WithMeshClsn *)(c + 0x1b4))->IsOnGround()) {
        *(int *)(c + 0x434) = 0;
        int *p = (int *)(((int)c + 0x198) & 0xFFFFFFFFFFFFFFFF);
        *p &= ~4;
    } else {
        ApproachLinear(*(short *)(c + 0x94), *(short *)(c + 0x45c), 0x800);
    }
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
}
