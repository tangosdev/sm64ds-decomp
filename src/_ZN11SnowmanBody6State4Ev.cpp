//cpp
#include "SnowmanBody.h"
#include "common.h"

extern "C" void _Z14ApproachLinearRiii(int *p, int a, int b);
extern "C" void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(
    int a, int b, int c);
extern "C" unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(
    unsigned int a, unsigned int b, unsigned int c, const Vector3 *v,
    unsigned int d);
extern "C" void _ZN8dActor_c9UpdatePosEP5dCc_c(char *c, void *cc);

int SnowmanBody::State4()
{
    char *c = (char *)this;
    _Z14ApproachLinearRiii((int *)(c + 0x98), 0x28000, 0x400);
    _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(
        *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
    *(int *)(c + 0x39c) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        *(unsigned int *)(c + 0x39c), 3, 0x8a,
        (Vector3 *)(c + 0x74), 0);
    UpdateRollAngle();
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x14c);
    UpdateGroundCollision((dBgCh_Actr *)(c + 0x180));
    HurtPlayer();
    if (*(int *)(c + 0x60) < (int)0xfe363c80)
        SetState(5);
    return 1;
}
