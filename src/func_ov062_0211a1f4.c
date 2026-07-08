typedef signed char s8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;

struct Vec3 { int x, y, z; };
struct VVec3 { volatile int x, y, z; };

extern s8 LEVEL_ID;
extern s8 DISPLAY_TIMER;
extern int TIME_TIMER;
extern int data_ov062_0211e004[];
extern int data_ov062_0211e014[];
extern int data_ov062_0211e024[];
extern int data_ov062_0211e02c[];

extern int _ZN6Player12Unk_020c4f40Et(int p, u16 a);
extern void _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, int d, int e);
extern int _ZN6Player12GetTalkStateEv(int p);
extern void func_02012694(u32 a, void *b);
extern void _ZN5Sound22LoadAndSetMusic_Layer2Ej(u32 a);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *m, int f, int a, u32 b, int e);
extern void _ZN5Timer10StartTimerEv(void *t);
extern void func_0201267c(u32 a, void *b);
extern int func_ov062_02119af0(char *p);
extern void func_ov062_02119800(void *c);
extern char *_ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN5Timer9StopTimerEv(void *t);
extern int func_ov062_021199ac(void *c);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern int _ZN5Actor14GetSubtractionEss(void *c, s16 a, int b);
extern int _ZN6Player22IsBeingShotOutOfCannonEv(int p);
extern int Vec3_Dist(const void *a, const void *b);
extern void _Z14ApproachLinearRiii(int *r, int a, int b);
extern void _Z14ApproachLinearRsss(s16 *r, s16 a, s16 b);
extern int _ZN9Animation8FinishedEv(void *a);
extern void func_ov062_02119954(void *c);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *c);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 a, int x, int y, int z);

void func_ov062_0211a1f4(char *a)
{
    int r6;
    int r5;
    int r4;
    int r7;
    volatile int tmp[3];

    switch (*(u8 *)(a + 0x390)) {
    case 0:
        if (_ZN6Player12Unk_020c4f40Et(*(int *)(a + 0x398), 0x5a) != 0)
            (*(u8 *)(((int)a + 0x390) & 0xFFFFFFFFFFFFFFFF))++;
        return;
    case 1:
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1f, 0x14, 0x7f, 0x6b000, 0);
        if (_ZN6Player12GetTalkStateEv(*(int *)(a + 0x398)) != -1)
            return;
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1f, 0x7f, 0, 0x7f000, 0);
        func_02012694(0x4d, a + 0x74);
        _ZN5Sound22LoadAndSetMusic_Layer2Ej(0x41);
        *(u8 *)(a + 0x3b5) = 1;
        (*(u8 *)(((int)a + 0x390) & 0xFFFFFFFFFFFFFFFF))++;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(a + 0x300, data_ov062_0211e014[1], 0x40000000, 0x1000, 0);
        DISPLAY_TIMER = 1;
        _ZN5Timer10StartTimerEv(&TIME_TIMER);
        func_0201267c(0x4d, a + 0x74);
        return;
    case 2:
        r6 = func_ov062_02119af0(a);
        if (*(int *)(a + 0x360) == data_ov062_0211e024[1])
            func_ov062_02119800(a);
        if (r6 == -1) {
            char *o;
            if (*(int *)(a + 0x394) == 0)
                return;
            o = _ZN5Actor10FindWithIDEj(*(int *)(a + 0x394));
            if (o == 0)
                return;
            *(int *)(a + 0x38c) = 3;
            *(u8 *)(a + 0x390) = 0;
            _ZN5Timer9StopTimerEv(&TIME_TIMER);
            *(u8 *)(a + 0x3af) = (*(u8 *)(o + 0x16e) != 0) ? 1 : 0;
            *(u8 *)(o + 0x16e) = 1;
            return;
        }
        r4 = func_ov062_021199ac(a);
        if (_ZN5Actor14GetSubtractionEss(a, *(s16 *)(a + 0x94),
                _ZN4cstd5atan2E5Fix12IiES1_(*(int *)(a + 0xd4), *(int *)(a + 0xdc))) >= 0x6000)
            r7 = *(int *)(a + 0xd8) * 7 - 0x6000;
        else
            r7 = 0x1000;
        if (*(u8 *)(a + 0x3ac) == 0)
            *(u8 *)(a + 0x3ac) = _ZN6Player22IsBeingShotOutOfCannonEv(*(int *)(a + 0x398));
        r5 = 4;
        if (*(int *)(a + 0x394) != 0) {
            char *o = _ZN5Actor10FindWithIDEj(*(int *)(a + 0x394));
            if (o != 0) {
                if (*(u8 *)(o + 0x16e) != 0 &&
                    Vec3_Dist(a + 0x5c, *(char **)(a + 0x398) + 0x5c) > 0x7d0000)
                    r5 = 8;
                else if (LEVEL_ID == 0x18)
                    r5 = 6;
            }
        }
        {
            int acc = r5 * 0x6000;
            _Z14ApproachLinearRiii((int *)(a + 0x98),
                (int)(((long long)acc * r7 + 0x800) >> 12), r5 * 0x19a);
        }
        _Z14ApproachLinearRsss((s16 *)(a + 0x94), *(s16 *)(a + 0x3a8), 0x800);
        if (*(int *)(a + 0x360) == data_ov062_0211e024[1]) {
            *(int *)(a + 0x35c) = *(int *)(a + 0x98) >> 3;
        } else {
            *(int *)(a + 0x35c) = 0x1000;
            if (_ZN9Animation8FinishedEv(a + 0x350) != 0 &&
                (*(int *)(a + 0x360) == data_ov062_0211e014[1] || *(int *)(a + 0x360) == data_ov062_0211e004[1]))
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(a + 0x300, data_ov062_0211e024[1], 0, 0x1000, 0);
        }
        if (r6 == 1 && (*(int *)(a + 0x3bc) == *(u8 *)(a + 0x3ad) || *(int *)(a + 0x3bc) == *(u8 *)(a + 0x3ae))) {
            func_ov062_02119954(a);
            return;
        }
        if (r4 != 0) {
            if (r4 < 0)
                *(int *)(a + 0x98) = 0;
            if (r4 == 0)
                return;
            func_ov062_02119954(a);
            return;
        }
        if (_ZNK12WithMeshClsn10IsOnGroundEv(a + 0x144) != 0)
            return;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(a + 0x300, data_ov062_0211e02c[1], 0x40000000, 0x1000, 0);
        *(int *)(a + 0xa8) = 0xa000;
        *(int *)(a + 0x98) = (int)(((long long)*(int *)(a + 0x98) * 0xd00 + 0x800) >> 12);
        *(u8 *)(a + 0x390) = 3;
        return;
    case 3:
        if (func_ov062_02119af0(a) == -1) {
            char *o;
            *(int *)(a + 0x38c) = 3;
            *(u8 *)(a + 0x390) = 0;
            _ZN5Timer9StopTimerEv(&TIME_TIMER);
            o = _ZN5Actor10FindWithIDEj(*(int *)(a + 0x394));
            *(u8 *)(a + 0x3af) = (*(u8 *)(o + 0x16e) != 0) ? 1 : 0;
            *(u8 *)(o + 0x16e) = 1;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(a + 0x300, data_ov062_0211e004[1], 0x40000000, 0x1000, 0);
            return;
        }
        _Z14ApproachLinearRsss((s16 *)(a + 0x94), *(s16 *)(a + 0x3a8), 0x800);
        if (_ZNK12WithMeshClsn10IsOnGroundEv(a + 0x144) == 0)
            return;
        *(u8 *)(a + 0x390) = 2;
        *(int *)(a + 0x9c) = -0x2000;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(a + 0x300, data_ov062_0211e004[1], 0x40000000, 0x1000, 0);
        *(int *)(a + 0x358) = 0;
        tmp[0] = *(int *)(a + 0x5c);
        tmp[1] = *(int *)(a + 0x60);
        tmp[2] = *(int *)(a + 0x64);
        tmp[1] = *(int *)(a + 0x60) + 0x28000;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb2, *(int *)(a + 0x5c),
            *(int *)(a + 0x60) + 0x28000, *(int *)(a + 0x64));
        return;
    default:
        return;
    }
}
