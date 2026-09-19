#ifndef DAMANTA_C_H
#define DAMANTA_C_H

#include "types.h"

/* Derives from dEnemyBase_c, and TWO INDEPENDENT WITNESSES agree on the layout:
 * the class's own destructor `_ZN9daManta_cD1Ev` destroys each member, and
 * `daManta_c_classInit` constructs the same types at the same offsets before
 * storing `_ZTV9daManta_c`. Everything this header used to restate below
 * 0x110 belongs to dEnemyBase_c and dActor_c and is inherited now.
 *
 * The members close on each other, which is what makes the layout a
 * reading rather than a guess:
 *
 *     0x110 dCcAcPos_c  0x40    -> 0x150
 *     0x150 dBgCh_Actr               0x1bc   -> 0x30c
 *     0x30c ModelAnim                  0x64    -> 0x370
 *
 * SIZE IS THE FACTORY'S LITERAL, NOT THE FIELD SPAN. `daManta_c_classInit` calls
 * `fBase_c::operator new(1028)` -- 0x404 -- and stores this class's vtable,
 * so that literal IS this class's sizeof. The evidenced fields reach only
 * 0x38c; the 0x94 between is trailing space no source reads, and a rounded-up
 * field span would have made this assert wrong by 120 bytes.
 *
 * THE CLASS NAME IS READ OUT OF THE CARTRIDGE, NOT COINED. `_ZTV9daManta_c`
 * sits at 0x0213423c; the word before it, at 0x02134238, is the type_info
 * pointer and resolves to `_ZTI9daManta_c` at 0x0213420c. That record's name
 * pointer reaches 0x021341f4, where the overlay image literally spells
 * `9daManta_c`. This header used to call the class MantaRay, which was a
 * readable invention; the cartridge had a name for it all along.
 *
 * The factory and profile spellings below are NOT read out of the cartridge.
 * They are reconstructed source-style names -- evidence-bounded proposals,
 * not recovered SM64DS symbols.
 *
 * daManta_c_classInit at 0x02132fe8 (historical alias MantaRay_Spawn)
 * allocates 0x404 and installs this class's cartridge vtable. It backs the
 * MANTA registry profile, whose descriptor at 0x02134218 is reconstructed as
 * g_profile_MANTA.
 */

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

struct daManta_c : dEnemyBase_c {
    dCcAcPos_c    mdCcAcPos_c; /* 0x110 */
    dBgCh_Actr                 mWithMeshClsn;         /* 0x150 */
    ModelAnim                    mModelAnim;            /* 0x30c */
    u8                           unk_370;               /* 0x370 */
    u8  pad_371[0xb];
    s32                          unk_37c;               /* 0x37c */
    s32                          unk_380;               /* 0x380 */
    s32                          mPathNode;             /* 0x384 */
    s32                          unk_388;               /* 0x388 */
    u8  pad_38c[0x78];

    /* --- vtable --- */
    virtual ~daManta_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daManta_c_size_must_be_0x404[sizeof(daManta_c) == 0x404 ? 1 : -1];
#endif

#endif /* DAMANTA_C_H */
