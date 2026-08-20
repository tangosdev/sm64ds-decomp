//cpp
#include "types.h"
struct Vec3 { s32 x, y, z; Vec3() {} Vec3(const Vec3 &o) { x = o.x; y = o.y; z = o.z; } };

extern "C" {
extern char *_ZN8dActor_c10FindWithIDEj(u32 id);
extern void func_ov002_020ed63c(void *self, s32 a);
extern s32 _ZNK10dBgCh_Actr14GetResultFlag1Ev(void *self);
extern void func_ov002_020ec670(void *self, void *clsn);
extern void func_ov002_020edca4(void *self);
extern s32 _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(struct Vec3 *dst, struct Vec3 *src, s32 step);
extern void func_ov002_020ec728(void *self);
extern s16 Vec3_HorzAngle(struct Vec3 *a, struct Vec3 *b);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(s32 x, s32 y, s32 z);
extern s32 func_ov002_020eddc4(void *self);
extern s32 func_ov002_020ed6cc(void *self);
extern void _ZN5dCc_c5ClearEv(void *self);
extern void _ZN5dCc_c6UpdateEv(void *self);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *cc);
extern void dBgCh_Actr_UpdateContinuous_Veneer(void *self);
}
struct VObj {
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual struct Vec3 v30();
};


extern "C" void func_ov002_020ecd18(void *arg0);
void func_ov002_020ecd18(void *arg0)
{
    char *c = (char *)arg0;
    char *o;
    s32 flag;
    s32 b;
    u32 fb0;

    o = 0;
    if (*(u32 *)(c + 0x410) != 0)
        o = _ZN8dActor_c10FindWithIDEj(*(u32 *)(c + 0x410));
    if (o != 0) {
        b = (s32)((*(u32 *)(o + 0xb0) & 0x10000000) != 0);
        if (b == 0) {
            func_ov002_020ed63c(c, 1);
            return;
        }
    }
    if (*(s32 *)(c + 0x410) != 0 && o == 0) {
        func_ov002_020ed63c(c, 1);
        return;
    }
    fb0 = *(u32 *)(c + 0xb0);
    flag = 0;
    b = (s32)((fb0 & 0x100) != 0);
    if (b == 0) {
        b = (s32)((fb0 & 0x2000) != 0);
        if (b != 0) {
            func_ov002_020ed63c(c, 3);
        } else {
            b = (s32)((fb0 & 0x400) != 0);
            if (b != 0) {
                if (_ZNK10dBgCh_Actr14GetResultFlag1Ev(c + 0x144) != 0) {
                    func_ov002_020ec670(c, c + 0x144);
                    func_ov002_020edca4(c);
                    return;
                }
                if (o != 0) {
                    struct Vec3 tmp = ((struct VObj *)o)->v30();
                    if (_Z14ApproachLinearR7Vector3RKS_5Fix12IiE(
                            (struct Vec3 *)(c + 0x5c), &tmp, *(s32 *)(c + 0x98)) != 0) {
                        switch (*(u16 *)(o + 0xc)) {
                        case 0xbd:
                        case 0xbe:
                        case 0xc6:
                        case 0xca:
                        case 0xdb:
                        case 0x151:
                            func_ov002_020edca4(c);
                            return;
                        default:
                            func_ov002_020ec728(c);
                            func_ov002_020ed63c(c, 1);
                            return;
                        }
                    }
                    *(s16 *)(c + 0x8e) = Vec3_HorzAngle((struct Vec3 *)(c + 0x5c), &tmp);
                }
                _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(
                    *(s32 *)(c + 0x5c), *(s32 *)(c + 0x60), *(s32 *)(c + 0x64));
                flag = 1;
                if (func_ov002_020eddc4(c) != 0)
                    return;
            }
        }
    }
    if (func_ov002_020ed6cc(c) != 0) {
        func_ov002_020edca4(c);
        return;
    }
    _ZN5dCc_c5ClearEv(c + 0x110);
    if (flag == 1)
        _ZN5dCc_c6UpdateEv(c + 0x110);
    if (o == 0)
        _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x110);
    dBgCh_Actr_UpdateContinuous_Veneer(c + 0x144);
}
