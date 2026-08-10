#include "types.h"
#define LAUND(p) ((void *)(p))

extern void *_ZN5Actor13ClosestPlayerEv(void *o);
extern int _ZN6Player9StartTalkER9ActorBaseb(void *pl, void *a, int b);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned a, int fx);
extern int _ZN6Player12GetTalkStateEv(void *pl);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(void *pl, void *a, unsigned m, void *v, unsigned d, unsigned e);
extern void func_0201277c(int a);
extern void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
extern void func_02011cfc(void);
extern s16 data_ov063_0211e1c8[];
extern char *data_0209f318;

void func_ov063_02117b0c(char *c)
{
    int v[3];

    switch (*(u8 *)(c + 0x5ce)) {
    case 0:
        *(void **)(c + 0x488) = _ZN5Actor13ClosestPlayerEv(c);
        if (!_ZN6Player9StartTalkER9ActorBaseb(*(void **)(c + 0x488), c, 1))
            return;
        {
            u8 *st = (u8 *)LAUND(c + 0x5ce);
            *st = *st + 1;
        }
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
        break;
    case 1:
        {
            u8 *st = (u8 *)LAUND(c + 0x5ce);
            *st = *st + 1;
        }
        break;
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x488)) != 0)
            return;
        {
            int y = *(int *)(c + 0x60);
            int x = *(int *)(c + 0x5c);
            int z = *(int *)(c + 0x64);
            int ny = y + 0xc8000;
            int nx = -x;
            v[0] = nx;
            v[1] = ny;
            v[2] = z;
        }
        {
            void *pl = *(void **)(c + 0x488);
            unsigned m = (unsigned)(int)data_ov063_0211e1c8[*(int *)((char *)pl + 8)];
            if (!_ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(
                    *(void **)(c + 0x484), c, m, v, 0, 2))
                return;
            func_0201277c(0x151);
            {
                u8 *st = (u8 *)LAUND(c + 0x5ce);
                *st = *st + 1;
            }
        }
        break;
    case 3:
        if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x488)) != -1)
            return;
        *(u8 *)(c + 0x5cc) = 5;
        _ZN5Sound22StopLoadedMusic_Layer3Ev();
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x7222);
        func_02011cfc();
        {
            u16 *fl = (u16 *)LAUND(c + 0x5d4);
            *fl = (u16)(*fl & ~0x400);
        }
        {
            char *base = data_0209f318;
            int *gp = (int *)LAUND(base + 0x154);
            *gp = *gp & ~8;
        }
        break;
    }
}
