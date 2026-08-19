typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct { s32 x, y, z; } Vector3;

extern void *_ZN8dActor_c10FindWithIDEj(u32 id);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int i, int fx, u32 j);
extern void func_ov074_0212087c(Vector3 *out, void *player, u8 flag);
extern int ApproachAngle(s16 *cur, s16 target, int divisor, int band, int maxStep);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
extern int Vec3_HorzDist(void *a, void *b);
extern int DecIfAbove0_Byte(void *p);
extern s16 _ZN8dActor_c18HorzAngleToCPlayerEv(void *self);
extern s32 _ZN8dActor_c13DistToCPlayerEv(void *self);
extern s32 AngleDiff(s16 a, s16 b);
extern int _ZNK10dBgCh_Actr13JustHitGroundEv(void *self);
extern void _ZN8dActor_c11LandingDustEb(void *self, int b);
extern int DecIfAbove0_Short(void *p);
extern u32 RandomIntInternal(int *seed);
extern int _Z14ApproachLinearRiii(int *v, int target, int step);
extern s16 Vec3_HorzAngle(void *a, void *b);
extern void Math_Function_0203b14c(int *v, int target, int a, int b, int c);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);

extern int data_ov084_02130cc8[];
extern int data_ov084_02130ce8[];
extern int data_ov084_02130228[];
extern int data_ov084_02130248[];
extern int data_ov084_02130268[];
extern int data_0209e650;

void func_ov084_0212af74(char *c)
{
    Vector3 sp4;
    void *player;
    s32 dist;
    s32 flag;
    s16 ang;
    u32 rnd;
    s32 lvl;
    u32 id;

    id = *(u32 *)(c + 0x43c);
    if (id == 0) {
        *(s32 *)(c + 0x460) = 1;
        return;
    }
    player = _ZN8dActor_c10FindWithIDEj(id);
    if (player == 0) {
        *(s32 *)(c + 0x460) = 1;
        return;
    }

    if (*(s32 *)(c + 0x444) == 0) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, (void *)data_ov084_02130cc8[1], 0, 0x1000, 0);
    } else {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, (void *)data_ov084_02130ce8[1], 0, 0x1000, 0);
    }

    func_ov074_0212087c(&sp4, player, *(u8 *)(c + 0x474));

    if (ApproachAngle((s16 *)(c + 0x94), *(s16 *)(c + 0x45a), 4, 0x1000, 0x400) == 0 &&
        _ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x1b4) != 0)
    {
        Vec3_HorzDist(c + 0x5c, c + 0x41c);
        dist = Vec3_HorzDist(c + 0x5c, &sp4);

        if ((*(s32 *)((char *)player + 0x5cc) == 4 && *(s16 *)((char *)player + 0x5f6) == 0) ||
            DecIfAbove0_Byte(c + 0x475) != 0 ||
            dist > 0x3e8000)
        {
            ang = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
            flag = 1;
            if (_ZN8dActor_c13DistToCPlayerEv(c) < 0x3e8000 &&
                AngleDiff(ang, *(s16 *)(c + 0x8e)) < 0x3000)
            {
                *(s16 *)(c + 0x45a) = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
                *(s16 *)(c + 0x45c) = *(s16 *)(c + 0x45a);
                lvl = *(s32 *)(c + 0x460);
                if (*(s32 *)(c + 0x444) == data_ov084_02130228[lvl]) {
                    *(s32 *)(c + 0x444) = data_ov084_02130268[lvl];
                }
                lvl = *(s32 *)(c + 0x460);
                if (*(s32 *)(c + 0x444) != data_ov084_02130268[lvl]) {
                    flag = 0;
                    *(s32 *)(c + 0xa8) = data_ov084_02130248[lvl];
                    *(s32 *)(c + 0x98) = 0;
                    *(s32 *)(c + 0x444) = data_ov084_02130268[*(s32 *)(c + 0x460)];
                }
                if (_ZNK10dBgCh_Actr13JustHitGroundEv(c + 0x1b4) != 0) {
                    _ZN8dActor_c11LandingDustEb(c, 1);
                }
            } else {
                if (DecIfAbove0_Short(c + 0x454) == 0) {
                    rnd = RandomIntInternal(&data_0209e650);
                    *(s16 *)(c + 0x454) = (s16)((rnd >> 0x1b) + 0x1e);
                    *(s16 *)(c + 0x45a) = (s16)(rnd >> 0x10);
                }
                *(s32 *)(c + 0x444) = 0;
            }
            if (flag != 0) {
                _Z14ApproachLinearRiii((int *)(c + 0x98), *(s32 *)(c + 0x444), 0x500);
            }
        } else {
            *(s16 *)(c + 0x45a) = Vec3_HorzAngle(c + 0x5c, &sp4);
            if (Vec3_HorzDist(c + 0x5c, &sp4) < 0x32000) {
                *(s32 *)(c + 0x444) = *(s32 *)((char *)player + 0x5e8) >> 2;
            } else if (Vec3_HorzDist(c + 0x5c, &sp4) < 0x64000) {
                *(s32 *)(c + 0x444) = *(s32 *)((char *)player + 0x5e8) >> 1;
            } else if (Vec3_HorzDist(c + 0x5c, &sp4) < 0x96000) {
                *(s32 *)(c + 0x444) = *(s32 *)((char *)player + 0x5e8);
            } else {
                s32 idx = *(s32 *)(c + 0x460);
                s32 v = data_ov084_02130268[idx];
                *(s32 *)(c + 0x444) = v + *(s32 *)((char *)player + 0x5e8);
            }
            Math_Function_0203b14c((int *)(c + 0x98), *(s32 *)(c + 0x444), 0x800, 0x10000, 4);
        }
    }

    if (*(s32 *)(c + 0x60) < *(s32 *)(c + 0x420) - 0x3e8000) {
        _ZN7fBase_c18MarkForDestructionEv(c);
    }

    if (*(s32 *)((char *)player + 0x5cc) != 4)
        return;
    if (*(s16 *)((char *)player + 0x5f6) == 0)
        *(u8 *)(c + 0x475) = 0x1e;
}
