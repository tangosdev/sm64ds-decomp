//cpp
struct State {};
extern State _ZN6Player7ST_FALLE;
extern State _ZN6Player16ST_CEILING_GRATEE;
extern State _ZN6Player13ST_LEDGE_HANGE;

struct Player {
    int GetHealth();
    int IsState(State &);
    int ChangeState(State &);
};

extern "C" int func_ov002_020bf30c(Player *c, int a);
extern "C" int func_ov002_020d0178(Player *c, int a, int b);

extern "C" int func_ov002_020d06c0(Player *p)
{
    int thresh, b354, b358;

    if (!p->IsState(_ZN6Player7ST_FALLE))
        return 0;

    if (p->GetHealth() != 0 &&
        *(unsigned short *)((char *)p + 0x6b4) == 0 &&
        *(unsigned char *)((char *)p + 0x706) == 0)
    {
        b354 = (*(int *)((char *)p + 0x354) != 0);
        if (b354 == 0 &&
            *(int *)((char *)p + 0x37c) == 0)
        {
            b358 = (*(int *)((char *)p + 0x358) != 0);
            if (b358 == 0 &&
                *(unsigned char *)((char *)p + 0x708) == 0 &&
                *(unsigned char *)((char *)p + 0x6fd) == 0 &&
                p->IsState(_ZN6Player16ST_CEILING_GRATEE) == 0 &&
                *(unsigned char *)((char *)p + 0x6de) != 0)
            {
                goto cont;
            }
        }
    }
    return 0;

cont:
    if (*(int *)((char *)p + 0x98) > func_ov002_020bf30c(p, 0x14000))
        return 0;

    thresh = 0xa0;
    if (*(unsigned char *)((char *)p + 0x703) != 0)
        thresh = 0x1c2;

    if (*(int *)((char *)p + 0x644) >
        *(int *)((char *)p + 0x60) - thresh * 0x1000)
        return 0;

    if (func_ov002_020d0178(p, *(int *)((char *)p + 0x60), 1) == 0)
        return 0;

    *(unsigned char *)((char *)p + 0x6e3) = 0;
    p->ChangeState(_ZN6Player13ST_LEDGE_HANGE);
    return 1;
}
