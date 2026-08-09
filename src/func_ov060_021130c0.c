#include "types.h"
#define LAUND(p) ((void*)((((long long)(int)(p)))))

extern int _ZN6Player9StartTalkER9ActorBaseb(void* player, void* actorBase, int isTalk);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int b);
extern int _ZN6Player12GetTalkStateEv(void* player);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(void* player, void* actorBase, unsigned int msgId, const void* pos, unsigned int a, unsigned int b);
extern void func_ov060_02111cc0(void* c, int a, int b, int d);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int a);
extern int func_ov060_021132a4(void* c);
extern void func_ov060_02113260(void* c);
extern void func_ov060_021135fc(void* c);

int func_ov060_021130c0(char* c)
{
    int ret = 0;
    u16 outer = *(u16*)(c + 0x3fe);

    if (outer <= 1) {
        u8 inner;

        if (outer == 0) {
            u16* op = (u16*)LAUND(c + 0x3fe);
            *op = *op + 1;
            *(u8*)(c + 0x424) = 0;
        }

        inner = *(u8*)(c + 0x424);
        switch (inner) {
        case 0:
            if (_ZN6Player9StartTalkER9ActorBaseb(*(void**)(c + 0x3a0), c, 1)) {
                _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
                {
                    u8* p = (u8*)LAUND(c + 0x424);
                    *p = *p + 1;
                }
            }
            break;

        case 1:
            if (_ZN6Player12GetTalkStateEv(*(void**)(c + 0x3a0)) == 0) {
                int msg = (*(u8*)(c + 0x414) == 0) ? 0xcd : 0xcf;
                if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(
                        *(void**)(c + 0x3a0), c, msg, c + 0x5c, 0, 2)) {
                    u8* p = (u8*)LAUND(c + 0x424);
                    *p = *p + 1;
                }
            }
            break;

        case 2:
            if (_ZN6Player12GetTalkStateEv(*(void**)(c + 0x3a0)) == -1) {
                u16* op = (u16*)LAUND(c + 0x3fe);
                u8* ip = (u8*)LAUND(c + 0x424);
                int v;
                *op = *op + 1;
                v = *ip + 1;
                *ip = v;
                func_ov060_02111cc0(c, 4, 0x40000000, v);
                _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
                _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x7222);
            }
            break;
        }
    } else {
        if (func_ov060_021132a4(c)) {
            func_ov060_02113260(c);
            func_ov060_021135fc(c);
            ret = 1;
        }
    }

    return ret;
}
