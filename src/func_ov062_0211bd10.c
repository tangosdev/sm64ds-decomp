#include "types.h"

extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *self);
extern s16 Vec3_HorzAngle(const void *a, const void *b);
extern s16 Vec3_VertAngle(const void *a, const void *b);
extern int _Z14ApproachLinearRsss(s16 *cur, s16 target, s16 step);
extern int Vec3_Dist(const void *a, const void *b);
extern int AngleDiff(s16 a, s16 b);
extern int ApproachAngle(s16 *cur, s16 target, int div, int band, int maxStep);
extern int _ZN8SaveData16HasPlayerLostCapEv(void);
extern void *_ZN8dActor_c13ClosestPlayerEv(void *self);
extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
    void *anim, void *file, int a, int b, int speed, unsigned short flags);
extern int func_ov062_0211b2fc(void *self);
extern int func_ov062_0211b3ac(void *self);
extern int func_ov062_0211c658(void *self, void *state);
extern void _ZN7PathPtrC1Ev(void *self);
extern void _ZN7PathPtr6FromIDEj(void *self, unsigned int id);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *self, Vector3 *v, unsigned int i);

extern s8 data_0209f2f8;
extern struct { int w[2]; } data_ov062_0211e104;
extern struct { int w[2]; } data_ov062_0211e114;
extern void *data_ov062_0211e18c;
extern void *data_ov062_0211e15c;

int func_ov062_0211bd10(char *c)
{
    s16 angV;
    s16 angD;
    void *player;
    int ppos[3];
    Vector3 node;
    char path[8];
    int thr;
    s8 stage;
    int *p43c;
    s16 *p94;
    int *pp;
    int tmp;
    int flag;

    if (*(u16 *)(c + 0x444) != 0) {
        if (_ZNK10dBgCh_Actr8IsOnWallEv(c + 0x178) != 0) {
            *(s16 *)(c + 0x44a) = (s16)(*(s16 *)(c + 0x94) + 0x4000);
        } else {
            *(s16 *)(c + 0x44a) = Vec3_HorzAngle(c + 0x5c, c + 0x484);
        }
        _Z14ApproachLinearRsss((s16 *)(c + 0x94), *(s16 *)(c + 0x44a), 0x1000);
        _Z14ApproachLinearRsss((s16 *)(c + 0x92), Vec3_VertAngle(c + 0x5c, c + 0x484), 0x500);
        _Z14ApproachLinearRsss((s16 *)(c + 0x96), 0, 0x500);
        if (Vec3_Dist(c + 0x5c, c + 0x484) < 0x1f4000) {
            *(u16 *)(c + 0x444) = 0;
        } else {
            *(u16 *)(c + 0x444) = 0x1e;
        }
        func_ov062_0211b2fc(c);
        return 1;
    }

    if (*(int *)(c + 0x468) == 0 && *(int *)(c + 0x44c) == 0) {
        angV = Vec3_VertAngle(c + 0x5c, c + 0x484);
        angD = (s16)(AngleDiff(*(s16 *)(c + 0x44a), *(s16 *)(c + 0x94)) / 3);
    } else {
        angV = Vec3_VertAngle(c + 0x5c, c + 0x430);
        angD = (s16)(AngleDiff(*(s16 *)(c + 0x44a), *(s16 *)(c + 0x94)) / 3);
    }

    if (data_0209f2f8 == 0x10) {
        ApproachAngle((s16 *)(c + 0x92), angV, 0xa, 0x400, 0x200);
    } else {
        _Z14ApproachLinearRsss((s16 *)(c + 0x92), angV, 0x300);
    }
    ApproachAngle((s16 *)(c + 0x96), angD, 0xa, 0x200, 0x100);

    if (data_0209f2f8 == 0x10 || *(u8 *)(c + 0x448) == 2) {
        p94 = (s16 *)(c + 0x94);
        *p94 = (s16)(*p94 - 0x200);
    } else {
        p94 = (s16 *)(c + 0x94);
        *p94 = (s16)(*p94 - 0x400);
    }

    if (*(int *)(c + 0x44c) == 0 && *(int *)(c + 0x468) == 0) {
        if (_ZN8SaveData16HasPlayerLostCapEv() == 0) {
            player = _ZN8dActor_c13ClosestPlayerEv(c);
            if (player != 0) {
                pp = (int *)((char *)player + 0x5c);
                tmp = pp[0];
                ppos[0] = tmp;
                tmp = pp[1];
                stage = data_0209f2f8;
                ppos[1] = tmp;
                tmp = pp[2];
                ppos[2] = tmp;
                if (stage != 0x10 &&
                    (*(int *)(c + 0x60) + 0x28000 < ppos[1] ||
                     *(u8 *)((char *)player + 0x706) != 0)) {
                    func_ov062_0211b2fc(c);
                    return 1;
                }
                thr = 0x3e8000;
                ppos[1] = *(int *)(c + 0x60);
                if (stage != 0x10) {
                    thr = 0x2bc000;
                }
                if (Vec3_Dist(c + 0x5c, ppos) < thr) {
                    flag = ((u8 *)player)[0x709] ? 1 : 0;
                    if (flag == 0) {
                        if (((u8 *)player)[0x6fb] == 0) {
                            func_ov062_0211c658(c, &data_ov062_0211e18c);
                        }
                    }
                }
                /* fall through to shared b2fc + return */
            }
        }
        func_ov062_0211b2fc(c);
        return 1;
    }

    if (angD < 0x300) {
        if (*(int *)(c + 0x43c) == 1) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
                c + 0x334, (void *)data_ov062_0211e104.w[1], 4, 0, 0x1000, 0);
            if (*(u8 *)(c + 0x448) == 2 ||
                (unsigned)(u8)(s8)((s8)data_0209f2f8 - 0x18) <= 1u) {
                p43c = (int *)(c + 0x440);
                *p43c = *p43c - 1;
                if (*(int *)(c + 0x440) <= 0) {
                    *(int *)(c + 0x440) = 0;
                }
            }
            *(int *)(c + 0x43c) = 2;
        }
    } else if (*(int *)(c + 0x43c) == 0) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            c + 0x334, (void *)data_ov062_0211e114.w[1], 4, 0, 0x1000, 0);
        p43c = (int *)(c + 0x43c);
        *p43c = *p43c + 1;
    }

    if (*(int *)(c + 0x43c) >= 2) {
        p43c = (int *)(c + 0x43c);
        *p43c = *p43c + 1;
        if (*(int *)(c + 0x43c) > 0x18) {
            *(int *)(c + 0x43c) = 0;
        }
    }

    if (*(u8 *)(c + 0x448) != 2 && data_0209f2f8 != 0x18 && data_0209f2f8 != 0x19 &&
        (*(int *)(c + 0x468) != 0 || *(int *)(c + 0x44c) != 0) &&
        func_ov062_0211b3ac(c) != 0 && *(u16 *)(c + 0x100) != 0) {
        *(u16 *)(c + 0x100) = 0;
    }

    func_ov062_0211b2fc(c);

    if (*(u16 *)(c + 0x100) == 0 && *(int *)(c + 0x440) == 0) {
        _ZN7PathPtrC1Ev(path);
        _ZN7PathPtr6FromIDEj(path, *(unsigned int *)(c + 0x464));
        _ZNK7PathPtr7GetNodeER7Vector3j(path, &node, *(unsigned int *)(c + 0x474));
        if (AngleDiff(Vec3_HorzAngle(c + 0x5c, &node), *(s16 *)(c + 0x8e)) < 0x2000) {
            func_ov062_0211c658(c, &data_ov062_0211e15c);
        }
    }

    return 1;
}
