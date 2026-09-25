#ifndef DACNN_C_H
#define DACNN_C_H

#include "types.h"
#include "dActor_c.h"
#include "Model.h"
#include "dCcAc_c.h"

struct Player;

/* The cannon (CANNON profile). The cartridge's RTTI names the class daCnn_c:
 * _ZTS at ov098 0x0213c664 reads "7daCnn_c", and the _ZTI at 0x0213c670 is
 * {__si_class_type_info vtable+8, &_ZTS, _ZTI8dActor_c}, so the direct base is
 * dActor_c. The tree called the class Cannon until then.
 *
 * daCnn_c_classInit constructs dActor_c first, then the owned Model and dCcAc_c;
 * D1/D0 destroy those members in reverse before the actor base. The two
 * directions pin this inheritance and layout. SIZE IS THE ROM'S OWN: the
 * factory calls fBase_c::operator new(0x198).
 *
 * mVariant is param1 & 3. Variant 2 starts sunk into the ground and waiting
 * for a player (state 2); the others start aiming (state 0). Variant 1 tilts
 * its barrel up and never runs its states. Behavior runs mState out of a pointer-to-member
 * table that the module's static initializer fills:
 *
 *   0 aim     ease the barrel toward mTargetPitch / mTargetAngleY
 *   1 fire    count down, fire a water bomb, recoil; back to aiming
 *   2 wait    sunk; wait for a player to climb in
 *   3 load    rise, turn, shoot the player out, sink again
 *
 * Field roles are read from the bodies in src/game/actors/d_a_cnn.cpp; the
 * names are inferred, not recovered.
 */
struct daCnn_c : dActor_c {
    u8 pad_0d0[0x4];          /* 0x0d0 */
    Model mModel;             /* 0x0d4 */
    dCcAc_c mdCcAc_c;         /* 0x124 */
    Player *mPlayer;          /* 0x158 -- the player riding the load sequence */
    s32 mSpawnPosX;           /* 0x15c */
    s32 mSpawnPosY;           /* 0x160 */
    s32 mSpawnPosZ;           /* 0x164 */
    s32 mMuzzlePosX;          /* 0x168 -- where the last ball left the barrel */
    s32 mMuzzlePosY;          /* 0x16c */
    s32 mMuzzlePosZ;          /* 0x170 */
    s32 mTimer;               /* 0x174 */
    s16 mTargetPitch;         /* 0x178 */
    s16 mTargetAngleY;        /* 0x17a */
    s16 mHomeAngleY;          /* 0x17c */
    u8 pad_17e[0x2];
    s32 mState;               /* 0x180 */
    u8 mVariant;              /* 0x184 */
    u8 mLoadStep;             /* 0x185 -- step of the state-3 load sequence */
    u8 pad_186[0x2];
    s32 mRecoil;              /* 0x188 -- barrel slide, <= 0 */
    s32 mRecoilSpeed;         /* 0x18c */
    u8 pad_190[0x4];
    s32 mBarrelRestY;         /* 0x194 -- the barrel bone's rest offset */

    /* Inline and empty, so an out-of-line virtual below is the key function
       and this TU emits the vtable and RTTI. Written out of line, mwccarm emits D0 ahead
       of D1 (the cartridge has D1 first). Keep the brace on this line:
       tools/check_header_offsets.py needs it. */
    virtual ~daCnn_c() {}

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daCnn_c_size_must_be_0x198[sizeof(daCnn_c) == 0x198 ? 1 : -1];
#endif

#endif /* DACNN_C_H */
