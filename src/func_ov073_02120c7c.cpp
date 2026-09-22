//cpp
typedef int Fix12i;
struct PMF;
extern "C" void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int a, int b, int c);
extern "C" void ChiefChilly_ChangeState(void* c, PMF* p);
extern "C" void _Z14ApproachLinearRiii(int& v, int a, int b);
extern "C" int _ZN9Animation8FinishedEv(void* anim);
extern PMF data_ov073_021233c0;
extern PMF data_ov073_021233d0;
extern PMF data_ov073_02123360;

struct C {
    char pad0[0x8e];
    short field_8e;
    char pad90[0x94-0x90];
    short field_94;
    char pad96[0x98-0x96];
    int field_98;
    char pad9c[0x35c-0x9c];
    char anim_35c;
    char pad35d[0x4c9-0x35d];
    unsigned char field_4c9;
    char pad4ca[0x4cb-0x4ca];
    unsigned char field_4cb;
    char pad4cc[0x4d4-0x4cc];
    int field_4d4, field_4d8, field_4dc;
};

extern "C" int func_ov073_02120c7c(C* c)
{
    int a = c->field_98; if (a < 0) a = -a;
    if (a > 0xa) {
        int i = 0;
        C* p = c;
        do {
            _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(p->field_4d4, p->field_4d8, p->field_4dc);
            i++;
            p = (C*)((char*)p + 0xc);
        } while (i < 2);
    }
    if (c->field_4c9 == 1) {
        *(unsigned char*)((int)c + 0x4cb) -= 1;
        if (c->field_4cb != 0)
            ChiefChilly_ChangeState(c, &data_ov073_021233c0);
        else
            ChiefChilly_ChangeState(c, &data_ov073_021233d0);
        return 1;
    }
    _Z14ApproachLinearRiii(c->field_98, 0, 0x1000);
    if (_ZN9Animation8FinishedEv(&c->anim_35c)) {
        int b = c->field_98; if (b < 0) b = -b;
        if (b < 0xa) {
            c->field_98 = 0;
            c->field_94 = c->field_8e;
            ChiefChilly_ChangeState(c, &data_ov073_02123360);
        }
    }
    return 1;
}
