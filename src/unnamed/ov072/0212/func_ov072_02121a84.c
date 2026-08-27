typedef int Fix12i;
typedef short s16;

extern void *_ZN8dActor_c13ClosestPlayerEv(void *c);
extern Fix12i Vec3_Dist(const void *a, const void *b);
extern s16 Vec3_HorzAngle(const void *v0, const void *v1);
extern void _Z14ApproachLinearRsss(s16 *dst, s16 target, s16 step);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *anim, void *file, int n, Fix12i speed, unsigned int flags);
extern void _ZN9Animation7AdvanceEv(void *anim);
extern void func_ov072_02120e20(void *c);
extern void func_ov072_02120ddc(void *c);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *c, void *clsn);
extern void func_ov072_02120fd4(void *c, void *clsn);
extern void func_ov072_02120f14(void *c);
extern void func_ov072_02120e50(void *c);
extern void _ZN5dCc_c5ClearEv(void *clsn);
extern void _ZN5dCc_c6UpdateEv(void *clsn);
extern void func_0201267c(int id, void *dst);

extern char data_ov072_02122cbc[];
extern char data_ov072_02122c9c[];

int func_ov072_02121a84(void *c)
{
    void *player;
    Fix12i dist;
    s16 angle;
    int frame;

    player = _ZN8dActor_c13ClosestPlayerEv(c);
    dist = Vec3_Dist((char *)c + 0x5c, (char *)player + 0x5c);
    switch (*(unsigned char *)((char *)c + 0x36e)) {
    case 0:
        angle = (s16)(Vec3_HorzAngle((char *)c + 0x5c, (char *)player + 0x5c) + 0x8000);
        _Z14ApproachLinearRsss((s16 *)((char *)c + 0x8e), angle, 0x514);
        *(s16 *)((char *)c + 0x94) = *(s16 *)((char *)c + 0x8e);
        *(int *)((char *)c + 0x98) = 0x5000;
        if (dist >= 0x1c2000) {
            *(unsigned char *)((char *)c + 0x36e) = 2;
        }
        break;
    case 1:
        angle = Vec3_HorzAngle((char *)c + 0x5c, (char *)player + 0x5c);
        _Z14ApproachLinearRsss((s16 *)((char *)c + 0x8e), angle, 0x514);
        *(s16 *)((char *)c + 0x94) = *(s16 *)((char *)c + 0x8e);
        *(int *)((char *)c + 0x98) = 0x5000;
        if (dist > 0x384000) {
            *(unsigned char *)((char *)c + 0x36e) = 2;
        } else if (dist < 0x1c2000) {
            *(unsigned char *)((char *)c + 0x36e) = 2;
        }
        break;
    case 2:
        *(int *)((char *)c + 0x98) = 0;
        if (dist < 0x15e000) {
            *(unsigned char *)((char *)c + 0x36e) = 0;
        } else if (dist >= 0x226000) {
            if (dist < 0x384000) {
                *(unsigned char *)((char *)c + 0x36e) = 1;
            }
        }
        break;
    }
    if (*(int *)((char *)c + 0x98) != 0) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)c + 0xd4, *(void **)(data_ov072_02122cbc + 4), 0, 0x1000, 0);
    } else {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)c + 0xd4, *(void **)(data_ov072_02122c9c + 4), 0, 0x1000, 0);
    }
    *(int *)((char *)c + 0x130) = 0x1000;
    _ZN9Animation7AdvanceEv((char *)c + 0x124);
    func_ov072_02120e20(c);
    func_ov072_02120ddc(c);
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, (char *)c + 0x160);
    func_ov072_02120fd4(c, (char *)c + 0x194);
    func_ov072_02120f14(c);
    func_ov072_02120e50(c);
    _ZN5dCc_c5ClearEv((char *)c + 0x160);
    _ZN5dCc_c6UpdateEv((char *)c + 0x160);
    if (*(int *)((char *)c + 0x134) == *(int *)(data_ov072_02122cbc + 4)) {
        frame = (int)((*(unsigned int *)((char *)c + 0x12c) << 4) >> 16);
        if (frame == 9 || frame == 0x15) {
            func_0201267c(0xf3, (char *)c + 0x74);
        }
    }
    return 1;
}
