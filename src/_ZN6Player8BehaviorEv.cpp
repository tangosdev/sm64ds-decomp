//cpp
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct Vector3 { s32 x, y, z; };
struct C3;
typedef int (C3::*PMF)();
struct State {
    PMF onEnter;
    PMF main;
    PMF onExit;
};
struct G_ee90 {
    char pad[0x26c];
    s32 flag26c;
};

#define LAU(p) ((long long)(int)(p) & 0xFFFFFFFFFFFFFFFFLL)

extern "C" {
extern u16 DecIfAbove0_Short(u16 *p);
extern s16 GetAngleToCamera(u8 playerID);
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void *self, const Vector3 *v);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 charID, u32 soundID, const Vector3 *pos);
extern int _ZN6Player11ChangeStateERNS_5StateE(void *self, State *s);
extern int _ZN6Player7IsStateERNS_5StateE(void *self, State *s);
extern void _ZN9Animation7AdvanceEv(void *self);
extern void func_02035684(void *p, s32 v);
extern void func_0203568c(void *p, s32 v);
extern void func_ov002_020bdd2c(void *self);
extern void func_ov002_020bdd9c(void *self);
extern void func_ov002_020bdef0(void *self);
extern void func_ov002_020bf13c(void *self);
extern void func_ov002_020bf36c(void *self, void *cyl);
extern void func_ov002_020c2db8(void *self);
extern void func_ov002_020c2e78(void *self);
extern int func_ov002_020c4188(void *self);
extern void func_ov002_020ca940(void *self);
extern void func_ov002_020ce8bc(void *self, s32 arg);
extern void func_ov002_020d6790(void *self);
extern void func_ov002_020d80d0(void *self);
extern void func_ov002_020d8158(void *self);
extern void func_ov002_020d869c(void *self);
extern void func_ov002_020db704(void *self);
extern void func_ov002_020e032c(void *self);
extern void func_ov002_020e4bb8(void *self);

extern u8 data_0209f250;
extern u8 data_0209f254;
extern s8 data_0209f284;
extern u8 data_0209f2d8;
extern s8 data_0209f2f8;
extern s16 data_0209f4a6;
extern s32 data_0209fc48;
extern u8 data_0209fc5c[];
extern s32 data_0209fc68;
extern u8 data_020a0e40;
extern G_ee90 data_0209ee90;

extern State data_ov002_0210ffec;
extern State data_ov002_021101b4;
extern State data_ov002_0211031c;
extern State data_ov002_02110334;
extern State data_ov002_02110454;
extern State data_ov002_021104b4;
extern State data_ov002_021104e4;
extern State data_ov002_021104fc;
extern State data_ov002_02110514;
extern State data_ov002_021105bc;
extern State data_ov002_0211061c;
extern State data_ov002_02110634;
extern State data_ov002_021106f4;
}

extern "C" int _ZN6Player8BehaviorEv(char *c)
{
    s32 r0;
    s32 scale;
    s32 r2;
    s32 temp;
    s32 mul;
    s32 fallFlag;
    volatile Vector3 v0;
    Vector3 v1;

    if (data_0209f2d8 == 1)
        r0 = 1;
    else
        r0 = 0;
    if (r0 != 0 && data_0209fc5c[*(u8 *)(c + 0x6d8)] == 0)
        return 1;

    scale = 0x1000;
    r2 = scale;
    if (*(u8 *)(c + 0x703) != 0) {
        scale = 0x3000;
        r2 = 0x4200;
    } else if (*(u8 *)(c + 0x6fd) != 0) {
        scale = 0x1400;
        r2 = 0x2000;
    }
    temp = (s32)(((s64)r2 * 0x32000 + 0x800) >> 12);
    func_0203568c(c + 0x380, temp);
    func_02035684(c + 0x380, temp);
    *(s32 *)(c + 0x2d8) = scale * 0x28;
    mul = 0x96;
    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021104e4)
        || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110514)
        || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110334)
        || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211031c)
        || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021105bc)
        || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0211061c)
        || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110634)) {
        mul = 0x5a;
    }
    *(s32 *)(c + 0x2dc) = mul * scale;

    if (*(u8 *)(c + 0x703) != 0)
        *(u32 *)LAU(c + 0x2ec) |= 0x10;
    else
        *(u32 *)LAU(c + 0x2ec) &= ~0x10;

    if (data_0209fc68 == 0) {
        if (data_0209fc48 != 0)
            r0 = 1;
        else
            r0 = 0;
        if (r0 == 0) {
            data_020a0e40 = 0;
            goto after_player_slot;
        }
    }
    data_020a0e40 = *(u8 *)(c + 0x6d8);
after_player_slot:

    if (func_ov002_020c4188(c) != 0)
        return 1;

    DecIfAbove0_Short((u16 *)(c + 0x6a4));
    DecIfAbove0_Short((u16 *)(c + 0x6a6));
    DecIfAbove0_Short((u16 *)(c + 0x6a0));
    DecIfAbove0_Short((u16 *)(c + 0x6a8));
    DecIfAbove0_Short((u16 *)(c + 0x6aa));
    DecIfAbove0_Short((u16 *)(c + 0x6ac));
    DecIfAbove0_Short((u16 *)(c + 0x6b0));
    DecIfAbove0_Short((u16 *)(c + 0x6b2));
    DecIfAbove0_Short((u16 *)(c + 0x6b4));
    DecIfAbove0_Short((u16 *)(c + 0x6b6));
    DecIfAbove0_Short((u16 *)(c + 0x6b8));
    DecIfAbove0_Short((u16 *)(c + 0x6ba));
    DecIfAbove0_Short((u16 *)(c + 0x6bc));
    DecIfAbove0_Short((u16 *)(c + 0x6c4));
    DecIfAbove0_Short((u16 *)(c + 0x6c8));

    if (*(u16 *)(c + 0x6c8) == 1)
        data_0209f284 = 0;

    if (*(u8 *)(c + 0x6f6) == 0) {
        DecIfAbove0_Short((u16 *)(c + 0x6c0));
        if (DecIfAbove0_Short((u16 *)(c + 0x6be)) == 0)
            func_ov002_020d80d0(c);
        if (DecIfAbove0_Short((u16 *)(c + 0x6ae)) == 0) {
            func_ov002_020e032c(c);
            func_ov002_020bdef0(c);
            func_ov002_020bdd9c(c);
        }
        if (DecIfAbove0_Short((u16 *)(c + 0x6c2)) == 0)
            func_ov002_020bdd2c(c);

        if (*(u16 *)(c + 0x6a2) != *(s8 *)(c + 0xcc)) {
            *(u16 *)(c + 0x6a2) = *(s8 *)(c + 0xcc);
            *(u16 *)(c + 0x6c6) = 0;
        }
        if (DecIfAbove0_Short((u16 *)(c + 0x6c6)) == 0 && *(s32 *)(c + 0x360) != 0)
            func_ov002_020d6790(c);
    }

    {
        s16 ang = *(s16 *)(c + 0x6d2);
        s32 stride = 0x18;
        *(s16 *)(c + 0x6d4) = ang;
        *(s16 *)(c + 0x6d2) = *(s16 *)((char *)&data_0209f4a6 + data_020a0e40 * stride);
        if (data_0209fc68 == 0) {
            s16 add = GetAngleToCamera(0);
            s16 *p = (s16 *)LAU(c + 0x6d2);
            *p = (s16)(*p + add);
        } else {
            s16 add = GetAngleToCamera(*(u8 *)(c + 0x6d8));
            s16 *p = (s16 *)LAU(c + 0x6d2);
            *p = (s16)(*p + add);
        }
    }

    func_ov002_020c2e78(c);
    func_ov002_020ca940(c);
    func_ov002_020d8158(c);

    {
        State *st = *(State **)(c + 0x370);
        if (*(void **)&st->main != 0)
            (((C3 *)c)->*(st->main))();
    }

    if ((u16)(*(u16 *)(c + 0x6ce) & 0x80) != 0) {
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021104fc);
        *(u16 *)LAU(c + 0x6ce) &= ~0x80;
    }

    if (*(u8 *)(c + 0x709) != 0)
        fallFlag = 1;
    else
        fallFlag = 0;
    if (data_0209f2f8 == 0x1f
        && _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021104b4) != 0)
        fallFlag = 0;

    if (*(u8 *)(c + 0x6de) == 0 || fallFlag != 0 || *(s32 *)(c + 0x684) < *(s32 *)(c + 0x60))
        *(s32 *)(c + 0x684) = *(s32 *)(c + 0x60);

    if (*(u8 *)(c + 0x6de) == 0 && *(u8 *)(c + 0x70d) != 0) {
        _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(c + 0x6d9), 0x2e, (const Vector3 *)(c + 0x74));
        *(u8 *)(c + 0x70d) = 0;
    }
    if (*(u8 *)(c + 0x6de) != 0 && *(u8 *)(c + 0x70d) == 0
        && _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110454) == 0
        && (*(s32 *)(c + 0x684) - *(s32 *)(c + 0x60)) > 0x47e000) {
        *(u8 *)(c + 0x70d) = 1;
        _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(c + 0x6d9), 0x1d, (const Vector3 *)(c + 0x74));
    }

    if (*(u8 *)(c + 0x707) != 0 && *(u8 *)(c + 0x706) == 0 && *(u8 *)(c + 0x6de) == 0)
        func_ov002_020ce8bc(c, *(s32 *)(c + 0x98));

    *(s16 *)(c + 0x6d6) = *(s16 *)(c + 0x8e);
    *(u16 *)LAU(c + 0x6ce) &= ~0x440;

    if (*(u8 *)(c + 0x6ff) != 0)
        _ZN9Animation7AdvanceEv(c + 0x1c4);

    *(s32 *)(c + 0x548) = *(s32 *)(c + 0x5c);
    *(s32 *)(c + 0x54c) = *(s32 *)(c + 0x60);
    *(s32 *)(c + 0x550) = *(s32 *)(c + 0x64);
    func_ov002_020bf36c(c, c + 0x2d4);
    func_ov002_020bf13c(c);

    {
        s32 *p = (s32 *)LAU((int)&v0);
        s32 z = 0;
        p[0] = z;
        p[1] = z;
        {
            State *st = &data_ov002_0210ffec;
            p[2] = z;
            if (_ZN6Player7IsStateERNS_5StateE(c, st) != 0) {
                if (*(u8 *)(c + 0x703) != 0) {
                    s32 t = 0x12c000;
                    v0.y = -t;
                } else {
                    s32 t = 0x64000;
                    v0.y = -t;
                }
            } else if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021106f4) != 0) {
                v0.y = 0x64000;
            }
        }
    }
    v1.x = v0.x;
    v1.y = v0.y;
    v1.z = v0.z;
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x2d4, &v1);
    func_ov002_020c2db8(c);

    if (*(u8 *)(c + 0x707) != 0)
        *(s32 *)(c + 0x66c) = 3;

    if (data_0209ee90.flag26c != 0) {
        data_0209ee90.flag26c = 0;
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021101b4);
        *(u8 *)(c + 0x6e3) = 0;
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0210ffec);
    }

    func_ov002_020db704(c);
    func_ov002_020d869c(c);

    {
        u16 t = *(u16 *)(c + 0x6a0);
        if (t != 0) {
            if (t == 1)
                *(u32 *)LAU(c + 0x2f0) |= 0x10000000;
            else
                *(u32 *)LAU(c + 0x2f0) &= ~0x10000000;
        }
    }

    _ZN12CylinderClsn5ClearEv(c + 0x2d4);
    if (*(u8 *)(c + 0x713) != 0)
        _ZN12CylinderClsn6UpdateEv(c + 0x2d4);

    if (data_0209fc68 == 0)
        data_020a0e40 = 0;
    else
        data_020a0e40 = data_0209f250;

    func_ov002_020e4bb8(c);

    {
        s32 r2 = *(s32 *)(c + 0x654);
        if (r2 != 0) {
            u8 ch = *(u8 *)(c + 0x714);
            char *t4 = (char *)(*(int *)(*(int *)(c + ((ch << 2) + 3) * 4 + 0x154) + 0x14) + 0x90);
            if (r2 < 0)
                r2 = -r2;
            *(s32 *)(t4 + 0x2c) = r2 >> 3;
        }
    }

    data_0209f254 = 0;
    if (*(s32 *)(c + 8) == 3) {
        void *p = *(void **)(*(void **)(c + 0x588));
        if (p != 0)
            data_0209f254 = *(u8 *)((char *)p + 0x428);
    }

    return 1;
}
