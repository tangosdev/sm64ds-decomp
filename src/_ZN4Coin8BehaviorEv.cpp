//cpp
// @symbol _ZN4Coin8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Coin.h"
/* _ZN4Coin8BehaviorEv @ 0x020b2324 (ov002, size 0x1e4)
 * Coin main behavior: pickup-flag sound, magnet flag, physics helpers,
 * state PMF-table dispatch, collision clear/update paths.
 */
struct C {};
typedef void (C::*PMF)();

extern PMF data_ov002_0210dc70[];
extern unsigned char data_0209f2d8;

extern "C" {
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, char *pos);
extern void _ZN12CylinderClsn5ClearEv(char *c);
extern int LenVec3(char *v);
extern void _ZN12CylinderClsn6UpdateEv(char *c);
}

int Coin::Behavior()
{
    if ((unsigned int)(unk_3ae << 0x1e) >> 0x1f) {
        *(unsigned char *)(((int)((char *)this) + 0x3ae)) &= ~2;
        _ZN5Sound9PlayBank3EjRK7Vector3(0x30, ((char *)this) + 0x74);
    }
    if ((int)(mActorID == 0x122) != 0) {
        if ((unsigned int)(unk_3ae << 0x1f) >> 0x1f)
            mAreaId = -1;
    }
    if (func_ov002_020b10a0(((char *)this)) != 0) return 1;
    *(short *)(((int)((char *)this) + 0x8e)) += 0xc00;
    if (func_ov002_020b12ec(((char *)this)) != 0) {
        func_ov002_020b14d8(((char *)this));
        _ZN12CylinderClsn5ClearEv((char *)&mCylinderClsn);
        return 1;
    }
    func_ov002_020b10e4(((char *)this));
    mEatingPlayer = 0;
    if (func_ov002_020b19dc(((char *)this)) != 0) return 1;
    (((C *)((char *)this))->*data_ov002_0210dc70[mBehaviorType])();
    if ((int)(data_0209f2d8 == 1) == 0 && (int)((unk_0b0 & 8) != 0) != 0) {
        _ZN12CylinderClsn5ClearEv((char *)&mCylinderClsn);
        if (unk_3aa == 0 && LenVec3((char *)&unk_074) < 0x64000) {
            if (mCoinType != 1 || unk_3b0 == 0)
                _ZN12CylinderClsn6UpdateEv((char *)&mCylinderClsn);
        }
    } else {
        func_ov002_020b14d8(((char *)this));
        _ZN12CylinderClsn5ClearEv((char *)&mCylinderClsn);
        if (unk_3aa == 0) {
            if (mCoinType != 1 || unk_3b0 == 0)
                _ZN12CylinderClsn6UpdateEv((char *)&mCylinderClsn);
        }
    }
    return 1;
}
