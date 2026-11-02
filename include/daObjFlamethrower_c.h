#ifndef DAOBJFLAMETHROWER_C_H
#define DAOBJFLAMETHROWER_C_H
#include "types.h"

#include "dActor_c.h"
#include "dCcPos_c.h"
#include "math/Matrix.h"

/* The flame jet actor. The destructor supplies the otherwise-missing layout
 * evidence: the ROM destroys Vector3[12] at 0x3a4, then dCcPos_c[12] at 0xd4,
 * before entering dActor_c's base destructor. The factory's 0x46c allocation
 *
 * SM64DS proves this class as daObjFlamethrower_c through RTTI, allocation
 * size and vtable identity. The factory and profile spellings below are
 * reconstructed source-style names -- evidence-bounded proposals, not
 * recovered SM64DS symbols.
 *
 * daObjFlamethrower_c_classInit at 0x02136ed4 (historical alias
 * Flamethrower_Spawn) installs this class's cartridge vtable. It backs the
 * OBJ_FLAMETHROWER registry profile, whose descriptor at 0x021376cc is
 * reconstructed as g_profile_OBJ_FLAMETHROWER.
 * literal closes the object after the state fields below. */
struct daObjFlamethrower_c : dActor_c {
    u8 pad_0d0[0x4];
    dCcPos_c mColliders[12];     /* 0x0d4 */
    Vector3 mFlamePositions[12]; /* 0x3a4 */
    Matrix4x3 mTransform;        /* 0x434 */
    u8 mState;                   /* 0x464 */
    u8 mActiveFlames;            /* 0x465 */
    u16 mTimer;                  /* 0x466 */
    u32 mSoundHandle;            /* 0x468 */

    virtual ~daObjFlamethrower_c() {}
    virtual int InitResources();
    virtual int Behavior();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char Flamethrower_size_must_be_0x46c[sizeof(struct daObjFlamethrower_c) == 0x46c ? 1 : -1];
#endif

#endif
