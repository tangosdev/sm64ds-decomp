#ifndef CAPENEMY_H
#define CAPENEMY_H

#include "types.h"

/* THE INTERMEDIATE CLASS BETWEEN Enemy AND THE ENEMIES THAT CAN WEAR MARIO'S
 * CAP. It was hiding in plain sight: this header restated Actor's layout and
 * stopped at a one-byte marker for 0x164, so it never reached 0x180 and nothing
 * could derive from it.
 *
 * The ROM's own name is `dCapEnemy_c` (`_ZTI11dCapEnemy_c`, ov002 0x02108260),
 * and the extracted RTTI graph records `dCapEnemy_c -> dEnemyBase_c` -- Enemy.
 * EXACTLY TWO classes derive from it in that graph: `daKrb_c` (Goomba) and
 * `daTrs_c` (Teresa, this tree's Boo). Their destructors agree: both chain to
 * the same function rather than to Enemy::~Enemy directly.
 *
 * The layout closes exactly, which is what makes 0x180 a fact rather than a
 * rounding:
 *
 *     0x000 Enemy                            -> 0x110
 *     0x110 four bytes of its own (mCapId at 0x113)
 *     0x114 Model    mModel      0x50        -> 0x164
 *     0x164 CapIcon  mCapIcon    0x1c        -> 0x180
 *
 * and Goomba's first own member is at 0x180, with Goomba_Spawn calling
 * `_ZN8CapEnemyC2Ev(p)` and then constructing at `p + 0x180`.
 *
 * NONE OF THIS IS NEW EVIDENCE -- it was already written down twice, in the
 * local shadow structs of `_ZN8CapEnemyC2Ev.c` and `_ZN8CapEnemyD2Ev.c`, the
 * latter's comment even saying "CapEnemy : Enemy. Members: capModel (Model)
 * @0x114, capIcon (CapIcon) @0x164". It just never reached a shared header,
 * so no subclass could use it.
 */

#include "CapIcon.h"
#include "Enemy.h"
#include "Model.h"

struct CapEnemy : Enemy {
    u8  unk_110;                      /* 0x110 */
    u8  unk_111;                      /* 0x111 */
    u8  unk_112;                      /* 0x112 */
    u8  mCapId;                       /* 0x113 */
    Model mModel;                     /* 0x114 */
    CapIcon mCapIcon;                 /* 0x164 */

    /* --- vtable --- */
    virtual ~CapEnemy();

    /* --- non-virtual --- */
    int AddCap(unsigned int param);
    int DestroyIfCapNotNeeded();
    int GetCapEatenOffIt(const Vector3 & v_);
    struct Actor * ReleaseCap(const Vector3 & v_);
    struct Actor * RespawnIfHasCap();
    void RenderCapModel(const Vector3 * v);
    void UnloadCapModel();
    void Unk_02005d94();
};

typedef char CapEnemy_size_must_be_0x180[sizeof(CapEnemy) == 0x180 ? 1 : -1];

#endif /* CAPENEMY_H */
