#include "types.h"
/* func_0200897c @ 0x0200897c, size 0x5c, ARM.
 * Camera-cluster member taking (Camera *self, void *arg).
 * Always runs func_02035414(arg). Then, if the camera's owner dActor_c
 * (self->owner @0x110) has actorID 0xbf (a specific actor type) AND a byte
 * flag at owner+0x6fb is set, runs func_02035428(arg). Finally always runs
 * func_02035468(arg).
 *
 * Codegen note: the (actorID == 0xbf) test is held in an enum-typed boolean.
 * Under -O4 -enum int this materializes the comparison as a full 32-bit 0/1
 * value (cmp; moveq #1; movne #0; cmp #0; beq) instead of folding it into a
 * single branch -- that is what the ROM does. A plain `int` bool folds to a
 * bare `bne`; a `u8` bool masks with `ands #0xff`; only the (int-width) enum
 * bool gives the exact `cmp r0, #0`.
 */
enum Bool { FALSE, TRUE };

typedef struct dActor_c {
    char _pad0[0xc];
    u16 actorID;        /* 0x0c */
    char _pad1[0x6fb - (0xc + 2)];
    u8 flag6fb;         /* 0x6fb */
} dActor_c;

typedef struct Camera {
    char _pad0[0x110];
    dActor_c *owner;       /* 0x110 */
} Camera;

extern void func_02035414(void *arg);
extern void func_02035428(void *arg);
extern void func_02035468(void *arg);

void func_0200897c(Camera *self, void *arg) {
    dActor_c *owner;
    enum Bool isType;
    func_02035414(arg);
    owner = self->owner;
    isType = (enum Bool)(owner->actorID == 0xbf);
    if (isType && owner->flag6fb != 0) {
        func_02035428(arg);
    }
    func_02035468(arg);
}
