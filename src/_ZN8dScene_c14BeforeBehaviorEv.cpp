//cpp
/* dScene_c::BeforeBehavior() at 0x0202e3d4, 0x1fc bytes -- vtable slot 7.
 *
 * Chains to fBase_c's, then runs the scene-transition state machine: hold the
 * screen while a fade is in flight, and once the brightness fader reaches the end,
 * queue the next scene and mark this one for destruction.
 *
 * ONE DELIBERATE SHADOW REMAINS, AND IT IS EVIDENCE, NOT LAZINESS. The current
 * fader at 0x0209f5bc is reached through the file-local `FaderVTable` below rather
 * than through include/FaderBrightness.h, because the ROM disagrees with the fader's
 * own mangled names about how many arguments slots 3 and 4 take:
 *
 *     0202e4d8  ldr r0,[pc,#0xec]      ; &current fader
 *     0202e4dc  mov r1,#0x1e
 *     0202e4e4  mov r2,#0              <-- a SECOND argument
 *     0202e4ec  ldr r3,[r3,#0x10]      ; slot 4
 *     0202e4f0  blx r3
 *
 * and the same shape at 0x0202e58c for slot 3. But the functions those slots hold
 * are _ZN15FaderBrightness14SetForwardTimeEj and ...15SetBackwardTimeEj, and `Ej`
 * is one parameter. Calling them through the real class would drop the `mov r2,#0`
 * and stop matching, so the ROM's own dScene_c translation unit must have been built
 * against a two-argument prototype. The names below are the ROM's -- read out of
 * data_0208eb2c -- and only the arity is this file's.
 *
 * Recording it here rather than papering over it: the fader family's headers are
 * known to be wrong in the other direction too (they declare seven vtable slots
 * where the ROM has ten), and that is a separate, larger correction. */
#include "dScene_c.h"
#include "FaderBrightness.h"

/* The ROM's fader vtable at data_0208eb2c, with the arity the call sites above
   prove this TU was compiled against. Slots 0/1 are D1/D0. The slot names come
   from what the table's own entries resolve to -- the tree has no _ZTV symbol
   for this address, only the data_ placeholder. */
struct FaderVTable {
    void (*D1)(void *);
    void (*D0)(void *);
    void (*AdvanceFade)(void *);
    void (*SetBackwardTime)(void *, u32, u32);
    void (*SetForwardTime)(void *, u32, u32);
    int  (*IsAtStart)(void *);
    int  (*IsAtEnd)(void *);
};
struct FaderObject { FaderVTable *vt; };

extern "C" {
extern u8   data_0209f1e0;
extern void *data_0209f1e4;
extern fBase_c   *data_0209f5c0;
extern FaderObject *data_0209f5bc;   /* the currently installed fader */
extern FaderBrightness data_0209f5d0;
extern FaderObject  data_0209f5e8;   /* really a FaderColor */
extern u16  data_02092664;           /* pending scene ID; 0x187 means none */

extern void func_02023544(void);
extern void _ZN15FaderBrightness14SetForwardTimeEj(FaderBrightness *self, u32 frames);
extern int  _ZN15FaderBrightness7IsAtEndEv(FaderBrightness *self);
extern int  func_020431c4(fBase_c *self);
}

int dScene_c::BeforeBehavior()
{
    if (!fBase_c::BeforeBehavior())
        return 0;

    if (data_0209f1e0 != 0) {
        int noActor = (data_0209f5c0->actorID == 0);
        if (noActor != 0) {
            func_02023544();
        } else {
            if (data_0209f1e4 == 0) {
                data_0209f5d0.currInterp = 0;   /* 0x4, not speed at 0x8 */
                _ZN15FaderBrightness14SetForwardTimeEj(&data_0209f5d0, 0x10);
                data_0209f1e4 = &data_0209f5d0;
            } else if (_ZN15FaderBrightness7IsAtEndEv(&data_0209f5d0)) {
                StartSceneFade(1, 0, 0);
                data_0209f5e8.vt->SetForwardTime(&data_0209f5e8, 0, 0);
                MarkForDestruction();
            }
            return 0;
        }
    }

    if (data_02092664 != 0x187) {
        if (data_0209f5bc->vt->IsAtStart(data_0209f5bc) != 0) {
            data_0209f5bc->vt->SetForwardTime(data_0209f5bc, 0x1e, 0);
        } else if (data_0209f5bc->vt->IsAtEnd(data_0209f5bc) != 0) {
            MarkForDestruction();
        }
        return 1;
    }

    if ((pauseFlags & 1) != 0) {
        if (func_020431c4(this) == 0) {
            pauseFlags &= ~1;
            pauseFlags &= ~4;
        }
        return 0;
    } else {
        if (data_0209f5bc->vt->IsAtEnd(data_0209f5bc) != 0) {
            data_0209f5bc->vt->SetBackwardTime(data_0209f5bc, 0x1e, 0);
        }
        return 1;
    }
}
