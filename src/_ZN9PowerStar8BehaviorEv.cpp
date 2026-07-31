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
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(char *c, char *clsn);
extern void func_ov002_020d718c(void *p);
extern void _ZN12CylinderClsn5ClearEv(char *c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(char *c, void *clsn);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(char *c, const void *v);
extern void _ZN12CylinderClsn6UpdateEv(char *c);
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

    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(((char *)this), ((char *)this) + 0x150) != 0) {
        int state = unk_440;
        if (state >= 5 && state <= 7 && *(void **)((char *)&mEatingPlayer) != 0) {
            func_ov002_020d718c(*(void **)((char *)&mEatingPlayer));
            mEatingPlayer = 0;
            *(int *)((int)((char *)&unk_0b0)) &= ~0xe0000;
            func_ov002_020e84ec(((char *)this));
            _ZN12CylinderClsn5ClearEv((char *)&mCylinderClsn);
            return 1;
        }
        if ((data_0209b454 & 0x4000000) != 0) {
            if ((int)((unk_0b0 & 0x4000000) != 0) != 0) {
                char *p = *(char **)((char *)&mEatingPlayer);
                if (p != 0)
                    *(int *)((int)(p + 0xb0)) |= 0x4000000;
            }
        }
        func_ov002_020e84ec(((char *)this));
        _ZN12CylinderClsn5ClearEv((char *)&mCylinderClsn);
        return 1;
    }

    mEatingPlayer = 0;
    func_ov002_020e763c(((char *)this));
    (((C *)((char *)this))->*data_ov002_021109d8[unk_440])();
    _ZN5Actor9UpdatePosEP12CylinderClsn(((char *)this), 0);
    func_ov002_020e84ec(((char *)this));
    _ZN12CylinderClsn5ClearEv((char *)&mCylinderClsn);
    {
        V3 v;
        v.x = data_ov002_0210aa0c[0];
        v.y = data_ov002_0210aa0c[1];
        v.z = data_ov002_0210aa0c[2];
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(((char *)this) + 0x110, &v);
    }
    if (unk_49f == 0)
        _ZN12CylinderClsn6UpdateEv((char *)&mCylinderClsn);
    func_ov002_020e7eb8(((char *)this));
    return 1;
}
