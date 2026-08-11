#ifndef CAPENEMY_H
#define CAPENEMY_H

#include "types.h"
#include "Enemy.h"
#include "CapIcon.h"
#include "Model.h"

struct Vector3;

/* The base for the enemies that can wear one of the caps. The ROM's RTTI names it
 * dCapEnemy_c and gives it two children, daKrb_c (Goomba) and daTrs_c (Boo).
 *
 * DERIVES FROM Enemy, and the class's own constructor and destructor are the two
 * witnesses. CapEnemy::CapEnemy calls _ZN5EnemyC2Ev, stores the vtable, then
 * constructs Model at 0x114 and the CapIcon at 0x164; the destructor tears the
 * same two down in the opposite order and chains to _ZN5EnemyD2Ev. Forward in one,
 * backward in the other, at the same offsets: a layout read twice.
 *
 * SIZE 0x180. This class is abstract in practice -- nothing allocates a plain
 * CapEnemy -- so there is no `operator new` literal to read the size off, the way
 * a leaf class has. Two other facts close it instead: the CapIcon at 0x164 is 0x1c
 * bytes and so ends exactly at 0x180, and Goomba, which derives from this class,
 * puts its own first member at 0x180. A derived member cannot start inside its
 * base, so 0x180 is both the floor and the ceiling.
 *
 * VTABLE. CapEnemy overrides exactly one thing: the destructor, at slots 16 and
 * 17. All 31 slots were diffed against Enemy's and every other one is identical,
 * which is why this class declares no virtual but its destructor.
 */
struct CapEnemy : Enemy {
    /* Which BANK of caps this enemy draws from. AddCap sets it when the spawn
       param is >= 3, and both ReleaseCap and GetCapEatenOffIt branch on it --
       a set flag releases the cap differently and skips the model re-bind. */
    u8  unk_110;                  /* 0x110 */
    u8  unk_111;                  /* 0x111 */
    /* Latched to 1 the first time a bank-1 cap is added, and never cleared;
       passed straight through to func_ov001_020ab228. */
    u8  unk_112;                  /* 0x112 */
    /* The cap, packed. Low 3 bits index the six-entry model table and 6 means
       "no cap" -- AddCap writes param % 3 and falls back to 6 on any failure,
       and UnloadCapModel/ReleaseCap both mask with & 7 before indexing.
       Bit 3 and bit 7 are set by ReleaseCap as release markers, which is why
       the reads mask and the writes OR. */
    u8  mCapId;                   /* 0x113 */
    Model   mModel;               /* 0x114 */
    CapIcon mCapIcon;             /* 0x164 */

    CapEnemy();
    virtual ~CapEnemy();

    /* methods */
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
