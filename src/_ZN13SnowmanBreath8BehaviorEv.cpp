//cpp
#include "types.h"
// @symbol _ZN13SnowmanBreath8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Player.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SnowmanBreath.h"
extern u8 data_0209f2d8[];

extern "C" {
extern int _ZN6Player9StartTalkER9ActorBaseb(void *self, void *actor, int b);
/* Moved INSIDE the extern "C" block. A bare `extern` on a mangled name in a
   //cpp file mangles it a second time -- this one was reaching the linker as
   _Z48_ZN6Player11ShowMessage... and sat in the unresolved baseline. Byte
   gates cannot see it, because relocations compare as wildcards; only
   check_references does. */
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(void *self, void *actor,
                                                            unsigned int msg,
                                                            const Vector3 *pos,
                                                            unsigned int a,
                                                            unsigned int b);
}
extern "C" {
extern int _ZN6Player12GetTalkStateEv(void *self);
/* Same double-mangle defect as ShowMessage above: outside this block the
   bare `extern` reached the linker as _Z33_ZN5Sound8PlayLongEjjjRK7Vector3sijjPvj. */
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(int handle, unsigned int a,
                                             unsigned int b, void *pos,
                                             unsigned int c);
}

int SnowmanBreath::Behavior()
{
    int b;
    char *slotBase;
    u8 *idx;
    char *base;
    Vector3 pos;
    int i;
    char *slot;
    int zero;

    b = data_0209f2d8[0];
    b = (b == 2);
    if (b) {
        return 1;
    }

    if (unk_13d0 == 0) {
        switch (unk_13d2) {
        case 0:
            if (func_ov027_02112618(((char *)this)) == 0) {
                break;
            }
            if (_ZN6Player9StartTalkER9ActorBaseb(*(void **)((char *)&unk_13c4), ((char *)this), 1) == 0) {
                break;
            }
            {
                u8 *state = (u8 *)(((int)((char *)this) + 0x13d2));
                *state = *state + 1;
            }
            break;
        case 1:
            pos.x = mPosX;
            zero = 0;
            pos.y = mPosY;
            pos.z = mPosZ;
            pos.y = pos.y + 0x12c000;
            if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(
                    *(void **)((char *)&unk_13c4), ((char *)this), 0xbb, &pos, zero, zero) == 0) {
                break;
            }
            {
                u8 *state = (u8 *)(((int)((char *)this) + 0x13d2));
                *state = *state + 1;
            }
            break;
        case 2:
            if (_ZN6Player12GetTalkStateEv(*(void **)((char *)&unk_13c4)) == -1) {
                _ZN6Player18HasFinishedTalkingEv(*(void **)((char *)&unk_13c4));
                unk_13d0 = 1;
            }
            break;
        }
    } else {
        if (func_ov027_02112618(((char *)this)) != 0) {
            int *timer = (int *)(((int)((char *)this) + 0x13c8));
            *timer = *timer + 1;
            base = ((char *)this) + 0x1000;
            if ((*(int *)(base + 0x3c8) & 7) != 0) {
                slotBase = ((char *)this) + 0xd4;
                idx = (u8 *)((char *)&unk_13d3);
                do {
                    slot = slotBase + *(u8 *)(base + 0x3d3) * 0x60;
                    b = func_ov027_021124e4(slot, *(void **)(base + 0x3c4));
                    *idx = *idx + 1;
                    *idx = *idx % 0x32;
                } while (b == 0);
            }
            unk_13cc =
                _ZN5Sound8PlayLongEjjjRK7Vector3s(unk_13cc, 3, 0x184,
                                                  ((char *)this) + 0x74, 0);
        }
    }

    slot = ((char *)this) + 0xd4;
    i = 0;
    do {
        func_ov027_02112480(slot);
        i++;
        slot += 0x60;
    } while (i < 0x32);

    return 1;
}
