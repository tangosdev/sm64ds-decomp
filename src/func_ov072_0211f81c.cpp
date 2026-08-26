//cpp
#include "SnowmanBody.h"
#include "common.h"

extern "C" {
void _Z14ApproachLinearRiii(int *p, int a, int b);
unsigned short DecIfAbove0_Short(unsigned short *p);
void func_0203568c(int *p, int v);
void func_02035684(int *p, int v);
void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(
    void *self, int a, int b, int c, int d);
void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int a, int b, int c);
int _ZN5Sound8PlayLongEjjjRK7Vector3s(
    unsigned a, unsigned b, unsigned c, const Vector3 *v, unsigned e);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *cc);
int Vec3_Dist(const void *a, const void *b);
extern Vector3 data_ov072_02122b40;
}

int SnowmanBody::State2()
{
    char *c = (char *)this;
    int t;
    int v;
    _Z14ApproachLinearRiii((int *)(c + 0x98), 0x28000, 0x400);
    if (DecIfAbove0_Short((unsigned short *)(c + 0x3a0)) == 0) {
        t = *(int *)(c + 0x80);
        _Z14ApproachLinearRiii(&t, 0x1800, 0x11);
        v = t;
        *(int *)(c + 0x80) = v;
        *(int *)(c + 0x84) = v;
        *(int *)(c + 0x88) = v;
        *(int *)(c + 0x398) =
            (int)(((long long)t * 0x82000 + 0x800) >> 12);
        *(int *)(c + 0x150) = *(int *)(c + 0x398);
        *(int *)(c + 0x154) = *(int *)(c + 0x398) << 1;
        func_0203568c((int *)(c + 0x180), *(int *)(c + 0x398));
        func_02035684((int *)(c + 0x180), *(int *)(c + 0x398));
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(
            c, *(int *)(c + 0x398), *(int *)(c + 0x398),
            0x1000000, 0x1000000);
    }
    if (AdvancePath() != 0) {
        if (*(unsigned char *)(c + 0x3a3) != 0 &&
            IsPlayerNearCenter() != 0)
            SetState(3);
        else
            SetState(4);
    }
    _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(
        *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
    *(int *)(c + 0x39c) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        *(unsigned *)(c + 0x39c), 3, 0x8a,
        (const Vector3 *)(c + 0x74), 0);
    UpdateRollAngle();
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x14c);
    UpdateGroundCollision((dBgCh_Actr *)(c + 0x180));
    HurtPlayer();
    if (*(unsigned char *)(c + 0x3a3) == 0) {
        if (Vec3_Dist(&data_ov072_02122b40,
                      *(char **)(c + 0x390) + 0x5c) < 0x300000)
            *(unsigned char *)(c + 0x3a3) = 1;
    }
    return 1;
}
