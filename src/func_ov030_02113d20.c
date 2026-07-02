typedef int Fix12i;
typedef short s16;

extern void *_ZN5Actor13ClosestPlayerEv(void *c);
extern Fix12i Vec3_Dist(const void *a, const void *b);
extern s16 Vec3_HorzAngle(const void *v0, const void *v1);
extern void _Z14ApproachLinearRsss(s16 *dst, s16 target, s16 step);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *anim, void *file, int n, Fix12i speed, unsigned int flags);
extern void func_ov030_02111a00(void *c);
extern void _ZN9Animation7AdvanceEv(void *c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *c, void *clsn);
extern int func_ov030_02111dd0(void *c);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *c);
extern void func_ov030_02111f6c(void *c, void *w);
extern void func_ov030_02111bc4(void *c);
extern int func_ov030_02111ea4(void *c);
extern int _ZN6Player7IsInAirEv(void *p);
extern void func_ov030_021141a8(void *c, int mode);
extern void _ZN12CylinderClsn5ClearEv(void *c);
extern void _ZN12CylinderClsn6UpdateEv(void *c);
extern void func_ov030_02111890(void *c);
extern void *data_ov030_02115d18;

int func_ov030_02113d20(void *c) {
    void *p;
    Fix12i dist;
    s16 angle;
    int onWall;
    int b;

    p = *(void **)((char *)c + 0x3a8);
    if (p == 0)
        p = _ZN5Actor13ClosestPlayerEv(c);

    dist = Vec3_Dist((char *)c + 0x5c, (char *)p + 0x5c);

    b = (int)(*(unsigned short *)((char *)c + 0xc) == 0x10c);
    if ((b && Vec3_Dist((char *)c + 0x380, (char *)p + 0x5c) > 0x514000) ||
        *(int *)((char *)p + 0x60) < *(int *)((char *)c + 0x384) - 0x12c000) {
        *(unsigned char *)((char *)c + 0x3c7) = 2;
    }

    switch (*(unsigned char *)((char *)c + 0x3c7)) {
    case 0:
        angle = Vec3_HorzAngle((char *)c + 0x5c, (char *)p + 0x5c) + 0x8000;
        *(int *)((char *)c + 0x98) = 0x13000;
        if (dist >= 0x1f4000)
            *(unsigned char *)((char *)c + 0x3c7) = 2;
        break;
    case 1:
        angle = Vec3_HorzAngle((char *)c + 0x5c, (char *)p + 0x5c);
        *(int *)((char *)c + 0x98) = 0xc000;
        if (dist < 0x190000)
            *(unsigned char *)((char *)c + 0x3c7) = 0;
        else if (dist < 0x1f4000)
            *(unsigned char *)((char *)c + 0x3c7) = 2;
        break;
    case 2:
        angle = Vec3_HorzAngle((char *)c + 0x5c, (char *)p + 0x5c);
        *(int *)((char *)c + 0x98) = 0;
        if (dist < 0x190000)
            *(unsigned char *)((char *)c + 0x3c7) = 0;
        else if (dist >= 0x258000)
            *(unsigned char *)((char *)c + 0x3c7) = 1;
        break;
    }

    _Z14ApproachLinearRsss((s16 *)((char *)c + 0x8e), angle, 0xa28);
    *(s16 *)((char *)c + 0x94) = *(s16 *)((char *)c + 0x8e);
    if (*(int *)((char *)c + 0x98) != 0)
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)c + 0xd4,
            ((void **)&data_ov030_02115d18)[1], 0, 0x1000, 0);
    else
        func_ov030_02111a00(c);

    _ZN9Animation7AdvanceEv((char *)c + 0x124);

    b = (int)(*(unsigned short *)((char *)c + 0xc) == 0x10b);
    if (b) {
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, (char *)c + 0x160);
        onWall = (func_ov030_02111dd0(c) | _ZNK12WithMeshClsn8IsOnWallEv((char *)c + 0x194)) != 0;
        func_ov030_02111f6c(c, (char *)c + 0x194);
        func_ov030_02111bc4(c);
    } else {
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, (char *)c + 0x160);
        func_ov030_02111f6c(c, (char *)c + 0x194);
        func_ov030_02111bc4(c);
        onWall = (func_ov030_02111ea4(c) | _ZNK12WithMeshClsn8IsOnWallEv((char *)c + 0x194)) != 0;
    }

    if (*(int *)((char *)c + 0x3b4) == 1 && onWall && dist < 0x12c000) {
        if (*(int *)((char *)p + 0x98) > 0x9000 || _ZN6Player7IsInAirEv(p) != 0) {
            *(int *)((char *)c + 0x3b8) = 1;
            func_ov030_021141a8(c, 2);
        }
    }

    _ZN12CylinderClsn5ClearEv((char *)c + 0x160);
    if (*(int *)((char *)c + 0x3b4) == 1)
        _ZN12CylinderClsn6UpdateEv((char *)c + 0x160);
    func_ov030_02111890(c);
    return 1;
}
