//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
struct Vector3 { s32 x, y, z; };
extern "C" {
    void _ZN5Sound15PlaySecretSoundEP5ActorPt(void *actor, u16 *snd);
    void *_ZN5Actor10FindWithIDEj(unsigned id);
    void ApproachAngle(void *self, s32 a, s32 b, s32 c, s32 d);
    s16 _ZN5Actor18HorzAngleToCPlayerEv(void *self);
    s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
    s32 _Z14ApproachLinearRsss(void *dst, s32 target, s32 step);
    u16 DecIfAbove0_Short(void *p);
    s32 _Z14ApproachLinearRiii(void *dst, s32 target, s32 step);
    void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca, s32 a, s32 fix, unsigned b);
    s32 Vec3_ApproachHorz(Vector3 *out, Vector3 *target, s32 maxStep);
    void func_ov014_02112ea8(void *actor);
    void _ZN6Camera9SetFlag_3Ev(void *cam);
    void *_ZN5Actor13ClosestPlayerEv(void *self);
    s32 _ZN6Player12Unk_020ca150Eh(void *player, u8 a);
    void _ZN6Camera9SetLookAtERK7Vector3(void *cam, const Vector3 *v);
    void _ZN9ActorBase18MarkForDestructionEv(void *self);
    void _ZN9Animation7AdvanceEv(void *self);
    extern s16 data_02082214[];
    extern void *data_0209f318;
    extern void *data_ov014_02114970[];
    extern void *data_ov014_02114980[];
}
static inline void inc604(u8 *self) {
    u8 *p = (u8 *)(((long long)(int)(self + 0x604)) & 0xFFFFFFFFFFFFFFFFLL);
    *p = (u8)(*p + 1);
}
extern "C" void func_ov014_021115ec(u8 *self)
{
    Vector3 sp4;
    u8 *r4; s16 r7; s16 r6; u8 *r8;
    _ZN5Sound15PlaySecretSoundEP5ActorPt(self, (u16 *)(self + 0x5fe));
    r4 = (u8 *)_ZN5Actor10FindWithIDEj(*(unsigned *)(self + 0x60c));
    {
        s32 *src = (s32 *)(((long long)(int)(r4 + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        s32 fifth = 0x80;
        sp4.x = src[0];
        void *ap = self + 0x8c;
        sp4.y = src[1];
        s32 z = 0;
        sp4.z = src[2];
        ApproachAngle(ap, z, 4, 0x200, fifth);
    }
    r8 = *(u8 **)&data_0209f318;
    r7 = _ZN5Actor18HorzAngleToCPlayerEv(self);
    r6 = Vec3_HorzAngle((Vector3 *)(self + 0x5c), (Vector3 *)(self + 0x5ec));
    switch (*(u8 *)(self + 0x604)) {
    case 0:
        *(u8 **)(r8 + 0x118) = self;
        if (_Z14ApproachLinearRsss(self + 0x8e, r7, 0x320) != 0 && DecIfAbove0_Short(self + 0x5fc) == 0)
            inc604(self);
        *(s16 *)(self + 0x94) = *(s16 *)(self + 0x8e);
        break;
    case 1: case 2: case 3: case 4:
        *(u8 **)(r8 + 0x118) = self;
        _Z14ApproachLinearRiii(self + 0x98, 0, 0x400);
        if (*(u8 *)(self + 0x61c) != 0) {
            *(s16 *)(self + 0x94) = (s16)(r6 + 0x2000);
            *(s16 *)(self + 0x8e) = *(s16 *)(self + 0x94);
            *(s32 *)(self + 0xa8) = 0x32000;
            *(s32 *)(self + 0x98) = 0x1e000;
            inc604(self);
        }
        break;
    case 5:
        *(u8 **)(r8 + 0x118) = self;
        _Z14ApproachLinearRiii(self + 0x98, 0, 0x400);
        if (*(u8 *)(self + 0x61c) != 0) {
            *(s16 *)(self + 0x94) = r6;
            *(s16 *)(self + 0x8e) = *(s16 *)(self + 0x94);
            *(s32 *)(self + 0xa8) = 0x32000;
            *(s32 *)(self + 0x98) = 0x1e000;
            inc604(self);
            if (*(u8 *)(r4 + 0x31e) != 0) *(u8 *)(self + 0x604) = 7;
        }
        break;
    case 6: {
        *(u8 **)(r8 + 0x118) = self;
        _Z14ApproachLinearRiii(self + 0x618, 0x1000, 0x400);
        if (*(u8 *)(self + 0x61c) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x150, data_ov014_02114970[1], 0, 0x1000, 0);
            *(s16 *)(self + 0x8e) = Vec3_HorzAngle((Vector3 *)(self + 0x5c), &sp4);
            *(s16 *)(self + 0x94) = *(s16 *)(self + 0x8e);
            *(s32 *)(self + 0xa8) = 0x14000;
            *(s32 *)(self + 0x618) = 0x64000;
        }
        {
            Vector3 target;
            s32 x = sp4.x;
            target.x = x;
            s32 z = sp4.z;
            target.z = z;
            s32 y = sp4.y;
            target.y = y;
            {
                s16 *tbl = data_02082214;
                u16 ang = *(u16 *)(r4 + 0x8e);
                s32 s = tbl[(ang >> 4) << 1];
                s32 add = (s32)(((((long long)s) * 0x96000) + 0x800) >> 12);
                target.x = x + add;
            }
            {
                s16 *tbl = data_02082214;
                u16 ang = *(u16 *)(r4 + 0x8e);
                s32 s = tbl[(((ang >> 4) << 1) + 1)];
                s32 add = (s32)(((((long long)s) * 0x96000) + 0x800) >> 12);
                target.z = z + add;
            }
            if (Vec3_ApproachHorz((Vector3 *)(self + 0x5c), &target, *(s32 *)(self + 0x618)) != 0) {
                inc604(self);
                *(s16 *)(self + 0x94) = (s16)(Vec3_HorzAngle((Vector3 *)(self + 0x5c), &sp4) + 0x8000);
                *(s32 *)(self + 0x98) = 0x28000;
                *(s32 *)(self + 0xa8) = 0xa000;
                func_ov014_02112ea8(r4);
            }
        }
        break;
    }
    case 7:
        if (*(u8 *)(self + 0x61c) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x150, data_ov014_02114980[1], 0, 0x1000, 0);
            *(s16 *)(self + 0x94) = Vec3_HorzAngle((Vector3 *)(self + 0x5c), &sp4);
            *(u16 *)(self + 0x5fc) = 0x3c;
            *(s16 *)(self + 0x8e) = *(s16 *)(self + 0x94);
            if (*(u8 *)(r4 + 0x31e) != 0) {
                *(s32 *)(self + 0x98) = 0x28000; *(s32 *)(self + 0xa8) = 0x5a000;
            } else {
                *(s32 *)(self + 0x98) = 0x1e000; *(s32 *)(self + 0xa8) = 0x50000;
            }
            _ZN6Camera9SetFlag_3Ev(r8);
            inc604(self);
        }
        break;
    case 8:
        if (DecIfAbove0_Short(self + 0x5fc) == 0) {
            {
                unsigned *flag = (unsigned *)(((long long)(int)(r8 + 0x154)) & 0xFFFFFFFFFFFFFFFFLL);
                *flag &= ~8u;
            }
            if (_ZN6Player12Unk_020ca150Eh(_ZN5Actor13ClosestPlayerEv(self), 4) != 0) {
                inc604(self);
                *(u16 *)(self + 0x5fc) = 0x3c;
            }
        } else {
            _ZN6Camera9SetLookAtERK7Vector3(r8, (Vector3 *)(self + 0x5c));
        }
        break;
    case 9:
        if (DecIfAbove0_Short(self + 0x5fc) == 0)
            _ZN9ActorBase18MarkForDestructionEv(self);
        break;
    }
    _ZN9Animation7AdvanceEv(self + 0x1a0);
}
