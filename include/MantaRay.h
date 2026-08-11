#ifndef MANTARAY_H
#define MANTARAY_H

#include "types.h"

/* Derives from Enemy, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN8MantaRayD1Ev` destroys each member, and
 * `MantaRay_Spawn` constructs the same types at the same offsets before
 * storing `_ZTV8MantaRay`. Everything this header used to restate below
 * 0x110 belongs to Enemy and Actor and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 MovingCylinderClsnWithPos  0x40    -> 0x150
 *     0x150 WithMeshClsn               0x1bc   -> 0x30c
 *     0x30c ModelAnim                  0x64    -> 0x370
 *
 * SIZE IS THE FACTORY'S LITERAL, NOT THE FIELD SPAN. `MantaRay_Spawn` calls
 * `ActorBase::operator new(1028)` -- 0x404 -- and stores this class's vtable,
 * so that literal IS this class's sizeof. The evidenced fields reach only
 * 0x38c; the 0x94 between is trailing space no source reads, and a rounded-up
 * field span would have made this assert wrong by 120 bytes.
 */

#include "Enemy.h"
#include "ModelAnim.h"
#include "MovingCylinderClsnWithPos.h"
#include "WithMeshClsn.h"

struct MantaRay : Enemy {
    MovingCylinderClsnWithPos    mMovingCylinderClsnWithPos; /* 0x110 */
    WithMeshClsn                 mWithMeshClsn;         /* 0x150 */
    ModelAnim                    mModelAnim;            /* 0x30c */
    u8                           unk_370;               /* 0x370 */
    u8  pad_371[0xb];
    s32                          unk_37c;               /* 0x37c */
    s32                          unk_380;               /* 0x380 */
    s32                          mPathNode;             /* 0x384 */
    s32                          unk_388;               /* 0x388 */
    u8  pad_38c[0x78];

    /* --- vtable --- */
    virtual ~MantaRay();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

typedef char MantaRay_size_must_be_0x404[sizeof(MantaRay) == 0x404 ? 1 : -1];

#endif /* MANTARAY_H */
