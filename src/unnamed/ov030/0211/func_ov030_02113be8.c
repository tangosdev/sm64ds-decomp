typedef int Fix12i;
typedef short s16;

extern void *_ZN8dActor_c13ClosestPlayerEv(void *c);
extern s16 Vec3_HorzAngle(const void *v0, const void *v1);
extern Fix12i Vec3_Dist(const void *a, const void *b);
extern void _ZN10dBgCh_Actr13SetLimMovFlagEv(void *c);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *anim, void *file, int n, Fix12i speed, unsigned int flags);
extern void func_0201267c(int a, void *b);
extern void *data_ov030_02115d08;

int func_ov030_02113be8(void *c) {
    void *p;
    int b;

    p = *(void **)((char *)c + 0x3a8);
    if (p == 0)
        p = _ZN8dActor_c13ClosestPlayerEv(c);

    b = (int)(*(unsigned short *)((char *)c + 0xc) == 0x10b);
    if (b != 0) {
        *(s16 *)((char *)c + 0x8e) = Vec3_HorzAngle((char *)c + 0x5c, (char *)c + 0x380);
    } else if (Vec3_Dist((char *)c + 0x380, (char *)c + 0x5c) < 0x514000 &&
               *(int *)((char *)c + 0x60) > *(int *)((char *)c + 0x384) - 0x12c000) {
        *(s16 *)((char *)c + 0x8e) = Vec3_HorzAngle((char *)c + 0x5c, (char *)p + 0x5c);
    } else {
        *(s16 *)((char *)c + 0x8e) = Vec3_HorzAngle((char *)c + 0x5c, (char *)c + 0x380);
    }

    *(s16 *)((char *)c + 0x94) = *(s16 *)((char *)c + 0x8e);
    *(int *)((char *)c + 0x98) = 0xd000;
    b = (int)(*(unsigned short *)((char *)c + 0xc) == 0x10b);
    *(int *)((char *)c + 0xa8) = b ? 0x23000 : 0x1e000;

    _ZN10dBgCh_Actr13SetLimMovFlagEv((char *)c + 0x194);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)c + 0xd4,
        ((void **)&data_ov030_02115d08)[1], 0x40000000, 0x1000, 0);
    func_0201267c(0xd1, (char *)c + 0x74);
    func_0201267c(0xf1, (char *)c + 0x74);
    *(int *)((char *)c + 0x3b4) = 2;
    return 1;
}
