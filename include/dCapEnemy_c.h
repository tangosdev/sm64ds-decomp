#ifndef DCAPENEMY_C_H
#define DCAPENEMY_C_H

#include "types.h"
#include "dEnemyBase_c.h"
#include "CapIcon.h"
#include "Model.h"

struct Vector3;

/* The base for the enemies that can wear one of the caps. The ROM's RTTI names it
 * dCapEnemy_c and gives it two children, daKrb_c (once coined Goomba) and daTrs_c (once coined Boo).
 *
 * DERIVES FROM dEnemyBase_c, and the class's own constructor and destructor are the two
 * witnesses. dCapEnemy_c::dCapEnemy_c calls _ZN12dEnemyBase_cC2Ev, stores the vtable, then
 * constructs Model at 0x114 and the CapIcon at 0x164; the destructor tears the
 * same two down in the opposite order and chains to _ZN12dEnemyBase_cD2Ev. Forward in one,
 * backward in the other, at the same offsets: a layout read twice.
 *
 * SIZE 0x180. This class is abstract in practice -- nothing allocates a plain
 * dCapEnemy_c -- so there is no `operator new` literal to read the size off, the way
 * a leaf class has. Two other facts close it instead: the CapIcon at 0x164 is 0x1c
 * bytes and so ends exactly at 0x180, and daKrb_c, which derives from this class,
 * puts its own first member at 0x180. A derived member cannot start inside its
 * base, so 0x180 is both the floor and the ceiling.
 *
 * VTABLE. dCapEnemy_c overrides exactly one thing: the destructor, at slots 16 and
 * 17. All 31 slots were diffed against dEnemyBase_c's and every other one is identical,
 * which is why this class declares no virtual but its destructor.
 */
struct dCapEnemy_c : dEnemyBase_c {
    /* Which BANK of caps this enemy draws from. AddCap sets it when the spawn
       param is >= 3, and both ReleaseCap and GetCapEatenOffIt branch on it --
       a set flag releases the cap differently and skips the model re-bind. */
    u8  mCapBank;                 /* 0x110 */
    /* Dormant marker. RespawnIfHasCap sets it to 1 on the REPLACEMENT actor it
       spawns; AddCap clears it; GetCapState returns 2 while it is clear (the
       ordinary enemy), and when it is set it returns 0 or -- once the cap's
       release bit is up -- clears it, restores mFlags from field 0xf4 and
       returns 1. daKrb_c::Render draws nothing while it is set. */
    u8  mIsDormant;               /* 0x111 */
    /* Latched to 1 the first time a bank-1 cap is added, and never cleared;
       passed straight through to func_ov001_020ab228. */
    u8  mHadBank1Cap;             /* 0x112 */
    /* The cap, packed. Low 3 bits index the six-entry model table and 6 means
       "no cap" -- AddCap writes param % 3 and falls back to 6 on any failure,
       and UnloadCapModel/ReleaseCap both mask with & 7 before indexing.
       Bit 3 and bit 7 are set by ReleaseCap as release markers, which is why
       the reads mask and the writes OR. */
    u8  mCapId;                   /* 0x113 */
    Model   mModel;               /* 0x114 */
    dCapIcon_c mCapIcon;          /* 0x164 */

    dCapEnemy_c();
    virtual ~dCapEnemy_c();

    /* methods */
    int AddCap(unsigned int param);
    int DestroyIfCapNotNeeded();
    int GetCapEatenOffIt(const Vector3 & v_);
    struct dActor_c * ReleaseCap(const Vector3 & v_);
    struct dActor_c * RespawnIfHasCap();
    void RenderCapModel(const Vector3 * v);
    void UnloadCapModel();
    void Unk_02005d94();
};

typedef char dCapEnemy_c_size_must_be_0x180[sizeof(dCapEnemy_c) == 0x180 ? 1 : -1];

#endif /* DCAPENEMY_C_H */
