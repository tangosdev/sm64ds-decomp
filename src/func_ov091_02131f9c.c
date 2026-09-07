typedef short s16;
extern int func_ov091_02131db8(void *c);
extern void func_020393d4(int *p, int v);
extern int _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
void func_ov091_02131f9c(char *c)
{
    if (func_ov091_02131db8(c) == -1) {
        *(int *)(c + 0x320) = 2;
    }
    s16 old = *(s16 *)(c + 0x8e);
    *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x300 + 0x50);
    if (old != *(s16 *)(c + 0x8e)) {
        func_020393d4((int *)(c + 0x124), 0);
    } else {
        func_020393d4((int *)(c + 0x124), (int)&_ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    }
}
