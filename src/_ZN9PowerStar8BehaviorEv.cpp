//cpp
// @symbol _ZN9PowerStar8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PowerStar.h"
struct C;
typedef void (C::*PMF)();

struct V3 { int x, y, z; };

extern "C" {
extern int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(char *c, char *clsn);
extern void func_ov002_020d718c(void *p);
extern void _ZN5dCc_c5ClearEv(char *c);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char *c, void *clsn);
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(char *c, const void *v);
extern void _ZN5dCc_c6UpdateEv(char *c);
extern int data_0209b454;
extern int data_ov002_0210aa0c[3];
}

extern PMF data_ov002_021109d8[];

struct C { char pad[0x800]; };

int PowerStar::Behavior()
{
    func_ov002_020e700c(((char *)this));
    unk_4a8 = 0;
    unk_4ac = 0;
    unk_4b0 = 0;

    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(((char *)this), ((char *)this) + 0x150) != 0) {
        int state = unk_440;
        if (state >= 5 && state <= 7 && *(void **)((char *)&mEatingPlayer) != 0) {
            func_ov002_020d718c(*(void **)((char *)&mEatingPlayer));
            mEatingPlayer = 0;
            *(int *)((int)((char *)&mFlags)) &= ~0xe0000;
            func_ov002_020e84ec(((char *)this));
            _ZN5dCc_c5ClearEv((char *)&mdCc_c);
            return 1;
        }
        if ((data_0209b454 & 0x4000000) != 0) {
            if ((int)((mFlags & 0x4000000) != 0) != 0) {
                char *p = *(char **)((char *)&mEatingPlayer);
                if (p != 0)
                    *(int *)((int)(p + 0xb0)) |= 0x4000000;
            }
        }
        func_ov002_020e84ec(((char *)this));
        _ZN5dCc_c5ClearEv((char *)&mdCc_c);
        return 1;
    }

    mEatingPlayer = 0;
    func_ov002_020e763c(((char *)this));
    (((C *)((char *)this))->*data_ov002_021109d8[unk_440])();
    _ZN8dActor_c9UpdatePosEP5dCc_c(((char *)this), 0);
    func_ov002_020e84ec(((char *)this));
    _ZN5dCc_c5ClearEv((char *)&mdCc_c);
    {
        V3 v;
        v.x = data_ov002_0210aa0c[0];
        v.y = data_ov002_0210aa0c[1];
        v.z = data_ov002_0210aa0c[2];
        _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(((char *)this) + 0x110, &v);
    }
    if (unk_49f == 0)
        _ZN5dCc_c6UpdateEv((char *)&mdCc_c);
    func_ov002_020e7eb8(((char *)this));
    return 1;
}
