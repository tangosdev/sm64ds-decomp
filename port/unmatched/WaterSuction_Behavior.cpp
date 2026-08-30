/* HOST COPY of src/_ZN12WaterSuction8BehaviorEv.cpp -- WATERSUCTION's
 * per-frame body (id 247, daWater_Suikomi_c, ov026, vtable slot 6).
 *
 * WHY A HOST COPY, and why this one is worse than the other three. The matched
 * src not only DISPATCHES a real C++ pointer-to-member on a forward-declared
 * class, it EMBEDS one in a struct and reads it at a fixed offset:
 *
 *     struct C; typedef void (C::*PMF)();
 *     struct Obj { char pad[8]; PMF pmf; };
 *     struct C { char pad[0x30c]; Obj *obj; };
 *     Obj *o = c->obj;
 *     if (*(int *)((char *)o + 8) != 0) { (c->*(o->pmf))(); }
 *
 * so MSVC's generalised pointer-to-member representation gets the member
 * OFFSET wrong before it gets the call wrong. The ROM's record is the 8-byte
 * Itanium {function, adjustment} pair and `o + 8` is the cell's TICK half.
 * Everything else here is transcribed statement for statement; only that
 * dispatch is spelled as a plain function-pointer call through the record
 * layout hal/actor_classes_ov026.cpp's port_ov026_states_seat() already
 * seated. The matched src stays in src/ as the byte proof and is absent from
 * port/slice_ov026cast.txt.
 *
 * NOT PLACED ON LEVEL 18. This class seats with the rest of ov026's cast and
 * proves by no-regression on the levels that do place it, not by a level-18
 * census move.
 */
#include "WaterSuction.h"

extern "C" {

struct PortOv026Cell { unsigned enter_fn, enter_adj, tick_fn, tick_adj; };
typedef int (*PortOv026Fn)(void *);

unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *thiz, void *clsn);
void _ZN12CylinderClsn5ClearEv(void *a);
void _ZN12CylinderClsn6UpdateEv(void *a);
void func_ov026_02112324(char *c);

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately
   incomplete class, here also as a STRUCT MEMBER (the SoundObject/Cap/
   MrBlizzard/BabyPenguin-state treatment); MSVC's PMF representation there
   reproduces neither the ROM's {function,adjustment} pair nor its offset. */
int _ZN12WaterSuction8BehaviorEv(void *selfv)
{
    WaterSuction *self = (WaterSuction *)selfv;
    char *c = (char *)selfv;

    DecIfAbove0_Short((unsigned short *)&self->unk_100);

    /* the state tick: `(c->*(o->pmf))()` over the cell's +8 record */
    {
        PortOv026Cell *cell = *(PortOv026Cell **)(c + 0x30c);
        if (cell->tick_fn != 0)
            ((PortOv026Fn)(size_t)cell->tick_fn)(c);
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
    self->unk_08c = self->unk_092;
    self->unk_08e = self->unk_094;
    self->unk_090 = self->unk_096;
    func_ov026_02112324(c);
    _ZN12CylinderClsn5ClearEv(&self->mMovingCylinderClsnWithPos);
    _ZN12CylinderClsn6UpdateEv(&self->mMovingCylinderClsnWithPos);
    return 1;
}

}
