//cpp
typedef unsigned short u16;

extern "C" void _ZN6Player11ChangeStateERNS_5StateE(char *c, void *st);
extern int _ZN6Player10ST_SWALLOWE;

extern "C" void _ZN6Player13InitFireYoshiEv(char *c)
{
    u16 *p = (u16 *)(((int)c + 0x6ce) & 0xFFFFFFFFFFFFFFFFLL);
    *p |= 0x1000;
    if (*(int *)(c + 0x360) == 0)
        return;
    _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player10ST_SWALLOWE);
}
