//cpp
#include "types.h"
// @symbol func_ov002_020c8b78
/* recovered: shared common types */
#include "common.h"
struct ActorBase {};
struct Animation { char pad[0x50]; int WillHitFrame(int) const; };

struct Player : ActorBase {
    int dummy;
};

extern "C" void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int, int);
extern "C" {
extern int _ZNK6Player14GetBodyModelIDEjb(Player const *, unsigned int, bool);
extern int _ZNK9Animation12WillHitFrameEi(Animation const *, int);
}
extern "C" void func_02012790(int);
extern "C" {
extern int _ZN6Player12FinishedAnimEv(Player *);
}
extern "C" void func_020731dc(void *a, void *b, void *node);
extern "C" void func_020072c0(void);
extern "C" {
extern void Vec3_RotateYAndTranslate(Vector3 *, void *, int, void *);
extern void _ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(Player *, ActorBase &, unsigned int, Vector3 const *, unsigned int, unsigned int);
}

extern "C" int data_ov002_0210e154;
extern "C" int data_ov002_0210f824[];
extern "C" char data_ov002_0210f7f4[];

extern "C" int func_ov002_020c8b78(char *self) {
    Player *p = (Player *)self;
    Vector3 v;
    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
    {
        int id = _ZNK6Player14GetBodyModelIDEjb(p, *(int *)(self + 8) & 0xff, false);
        Animation *a = (Animation *)(*(int *)(self + id * 4 + 0xdc) + 0x50);
        if (_ZNK9Animation12WillHitFrameEi(a, 0x40))
            func_02012790(0x3a);
    }
    if (_ZN6Player12FinishedAnimEv(p) != 0) {
        if (!(data_ov002_0210e154 & 1)) {
            data_ov002_0210f824[0] = 0;
            data_ov002_0210f824[1] = 0x96000;
            data_ov002_0210f824[2] = 0x64000;
            func_020731dc(data_ov002_0210f824, (void *)func_020072c0, data_ov002_0210f7f4);
            data_ov002_0210e154 |= 1;
        }
        Vec3_RotateYAndTranslate(&v, self + 0x5c, *(s16 *)(self + 0x8e), data_ov002_0210f824);
        *(u8 *)(self + 0x722) = 1;
        {
            u16 *fl = (u16 *)(((int)self + 0x6ce));
            *fl |= 0x400;
        }
        {
            u8 saved = *(u8 *)(self + 0x70b);
            unsigned int msg = 0x188;
            if (*(u8 *)(self + 0x70c))
                msg = 0x29;
            _ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(p, *(ActorBase *)p, msg, &v, 0, 2);
            *(u8 *)(self + 0x70b) = saved;
        }
    }
    return 0;
}
