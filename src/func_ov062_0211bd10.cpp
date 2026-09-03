//cpp
#include "types.h"
// @symbol func_ov062_0211bd10
/* recovered: shared common types */
#include "common.h"
struct PathPtr {
    int a;
    int b;
    PathPtr();
    void FromID(u32 id);
    void GetNode(Vector3 &out, u32 idx) const;
};

extern "C" {
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *self);
extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *anim, void *file, int a, int b, int spd, unsigned short t);
extern s16 Vec3_HorzAngle(const void *a, const void *b);
extern s16 Vec3_VertAngle(const void *a, const void *b);
extern int Vec3_Dist(const void *a, const void *b);
extern void _Z14ApproachLinearRsss(s16 *cur, s16 tgt, s16 step);
extern int ApproachAngle(void *p, int target, int a, int b, int c);
extern int AngleDiff(int a, int b);
extern int _ZN8SaveData16HasPlayerLostCapEv(void);
extern char *_ZN5Actor13ClosestPlayerEv(void *self);
extern void func_ov062_0211b2fc(void *c);
extern int func_ov062_0211b3ac(void *c);
extern void func_ov062_0211c658(void *c, void *p);

extern s8 data_0209f2f8;
extern void *data_ov062_0211e104[];
extern void *data_ov062_0211e114[];
extern int data_ov062_0211e15c;
extern int data_ov062_0211e18c;
}

extern "C" int func_ov062_0211bd10(char *self)
{
    Vector3 ppos;
    Vector3 node;
    s16 vert;
    s16 turn;
    char *pl;
    Vector3 *pp;
    int range;
    s8 level;

    if (*(u16 *)(self + 0x444) != 0) {
        if (_ZNK12WithMeshClsn8IsOnWallEv(self + 0x178) != 0)
            *(s16 *)(self + 0x44a) = *(s16 *)(self + 0x94) + 0x4000;
        else
            *(s16 *)(self + 0x44a) = Vec3_HorzAngle(self + 0x5c, self + 0x484);
        _Z14ApproachLinearRsss((s16 *)(self + 0x94), *(s16 *)(self + 0x44a), 0x1000);
        _Z14ApproachLinearRsss((s16 *)(self + 0x92), Vec3_VertAngle(self + 0x5c, self + 0x484), 0x500);
        _Z14ApproachLinearRsss((s16 *)(self + 0x96), 0, 0x500);
        if (Vec3_Dist(self + 0x5c, self + 0x484) < 0x1f4000)
            *(u16 *)(self + 0x444) = 0;
        else
            *(u16 *)(self + 0x444) = 0x1e;
        func_ov062_0211b2fc(self);
        return 1;
    }

    if (*(int *)(self + 0x468) == 0 && *(int *)(self + 0x44c) == 0) {
        vert = Vec3_VertAngle(self + 0x5c, self + 0x484);
        turn = AngleDiff(*(s16 *)(self + 0x44a), *(s16 *)(self + 0x94)) / 3;
    } else {
        vert = Vec3_VertAngle(self + 0x5c, self + 0x430);
        turn = AngleDiff(*(s16 *)(self + 0x44a), *(s16 *)(self + 0x94)) / 3;
    }

    if (data_0209f2f8 == 0x10)
        ApproachAngle(self + 0x92, vert, 0xa, 0x400, 0x200);
    else
        _Z14ApproachLinearRsss((s16 *)(self + 0x92), vert, 0x300);
    ApproachAngle(self + 0x96, turn, 0xa, 0x200, 0x100);

    if (data_0209f2f8 == 0x10 || *(u8 *)(self + 0x448) == 2)
        *(s16 *)(self + 0x94) -= 0x200;
    else
        *(s16 *)(self + 0x94) -= 0x400;

    if (*(int *)(self + 0x44c) == 0 && *(int *)(self + 0x468) == 0) {
        if (_ZN8SaveData16HasPlayerLostCapEv() == 0) {
            pl = _ZN5Actor13ClosestPlayerEv(self);
            if (pl != 0) {
                pp = (Vector3 *)(pl + 0x5c);
                ppos.x = pp->x;
                level = data_0209f2f8;
                ppos.y = pp->y;
                ppos.z = pp->z;
                if (level != 0x10) {
                    if (*(int *)(self + 0x60) + 0x28000 < ppos.y || *(u8 *)(pl + 0x706) != 0) {
                        func_ov062_0211b2fc(self);
                        return 1;
                    }
                }
                ppos.y = *(int *)(self + 0x60);
                range = 0x3e8000;
                if (level != 0x10)
                    range = 0x2bc000;
                if (Vec3_Dist(self + 0x5c, &ppos) < range) {
                    int b = *(u8 *)(pl + 0x709) != 0;
                    if (!b) {
                        if (*(u8 *)(pl + 0x6fb) == 0)
                            func_ov062_0211c658(self, &data_ov062_0211e18c);
                    }
                }
            }
        }
        func_ov062_0211b2fc(self);
        return 1;
    }

    if (turn < 0x300) {
        if (*(int *)(self + 0x43c) == 1) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(self + 0x334, data_ov062_0211e104[1], 4, 0, 0x1000, 0);
            if (*(u8 *)(self + 0x448) == 2 || (u8)(s8)(data_0209f2f8 - 0x18) <= 1) {
                (*(int *)(self + 0x440))--;
                if (*(int *)(self + 0x440) <= 0)
                    *(int *)(self + 0x440) = 0;
            }
            *(int *)(self + 0x43c) = 2;
        }
    } else {
        if (*(int *)(self + 0x43c) == 0) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(self + 0x334, data_ov062_0211e114[1], 4, 0, 0x1000, 0);
            (*(int *)(self + 0x43c))++;
        }
    }

    if (*(int *)(self + 0x43c) >= 2) {
        (*(int *)(self + 0x43c))++;
        if (*(int *)(self + 0x43c) > 0x18)
            *(int *)(self + 0x43c) = 0;
    }

    if (*(u8 *)(self + 0x448) != 2 && data_0209f2f8 != 0x18 && data_0209f2f8 != 0x19 &&
        (*(int *)(self + 0x468) != 0 || *(int *)(self + 0x44c) != 0)) {
        if (func_ov062_0211b3ac(self) != 0) {
            if (*(u16 *)(self + 0x100) != 0)
                *(u16 *)(self + 0x100) = 0;
        }
    }

    func_ov062_0211b2fc(self);
    if (*(u16 *)(self + 0x100) == 0 && *(int *)(self + 0x440) == 0) {
        PathPtr path;
        path.FromID(*(u32 *)(self + 0x464));
        path.GetNode(node, *(u32 *)(self + 0x474));
        if (AngleDiff(Vec3_HorzAngle(self + 0x5c, &node), *(s16 *)(self + 0x8e)) < 0x2000)
            func_ov062_0211c658(self, &data_ov062_0211e15c);
    }
    return 1;
}
