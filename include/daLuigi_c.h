/* daLuigi_c: the mirror-room Luigi reflection actor (ov055).
 *
 * The cartridge's RTTI names this class: _ZTS9daLuigi_c at ov055 0x02111aa4,
 * _ZTI9daLuigi_c at 0x02111ab0 (one zero-offset dActor_c base), and the
 * vtable address point _ZTV9daLuigi_c at 0x02111ae0. The tree called it
 * MirrorLuigi until it was renamed to that ROM name.
 *
 * The 0x20c allocation literal in daLuigi_c_classInit fixes the total size. Its
 * factory constructs the four owned member regions in declaration order and
 * D1/D0 destroy them in reverse order, independently proving the layout below:
 * ModelAnim @ 0x0d4, Model @ 0x138, ShadowModel @ 0x188, and two
 * TextureSequence objects @ 0x1b0. The remaining tail is a state descriptor
 * pointer followed by the shadow matrix used by Behavior.
 *
 * The ROM vtable has exactly dActor_c's 31 slots. Slots 0, 3, 6, 9, 12, 16
 * and 17 point at the overrides declared here; every other destination is
 * inherited unchanged.
 *
 * SM64DS RTTI names the implementation daLuigi_c. The reconstructed factory
 * daLuigi_c_classInit (historical alias MirrorLuigi_Spawn)
 * installs this class's cartridge vtable; the reconstructed profile
 * global g_profile_LUIGI (historical alias MirrorLuigi_SpawnInfo)
 * is its registry descriptor.
 */
#ifndef DALUIGI_C_H
#define DALUIGI_C_H

#include "dActor_c.h"
#include "ModelAnim.h"
#include "Model.h"
#include "ShadowModel.h"
#include "TextureSequence.h"
#include "math/Matrix.h"

struct Player;
struct daLuigiState;

struct daLuigi_c : dActor_c {
    u8 pad_0d0[0x4];
    ModelAnim mModelAnim;                     /* 0x0d4 */
    Model mModel;                             /* 0x138 */
    ShadowModel mShadowModel;                 /* 0x188 */
    TextureSequence mTextureSequences[2];     /* 0x1b0 */
    daLuigiState *mState;                     /* 0x1d8 */
    Matrix4x3 mShadowMatrix;                  /* 0x1dc */

    /* Inline and empty: the compiler writes the member teardown, emits D1
       then D0 in the cartridge's order, and InitResources becomes the key
       function that places the vtable in src/actors/daLuigi_c.cpp. */
    virtual ~daLuigi_c() {}                   /* slots 16, 17 */
    virtual s32 InitResources();              /* slot  0 */
    virtual s32 CleanupResources();           /* slot  3 */
    virtual s32 Behavior();                   /* slot  6 */
    virtual s32 Render();                     /* slot  9 */
    virtual void OnPendingDestroy();          /* slot 12 */

    /* Coined names. SetState (0x021112c4) enters a state; EnterMirror
       (0x021112bc) and ExecuteMirror (0x02111288) are the one state, which
       mirrors the player's position and facing. */
    int SetState(daLuigiState *state, Player *player);
    int EnterMirror(Player *player);
    int ExecuteMirror(Player *player);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daLuigi_c_size_must_be_0x20c[
    sizeof(daLuigi_c) == 0x20c ? 1 : -1];
#endif

/* One state: two members called with the player, `enter` once by SetState
 * and `execute` every frame by Behavior. The one table in the ROM (ov055
 * 0x02111a94, copied to 0x02111b70 by the static initializer) pairs
 * EnterMirror with ExecuteMirror. daLuigiState and daLuigiStateFn are
 * coined names. */
typedef int (daLuigi_c::*daLuigiStateFn)(Player *player);
struct daLuigiState {
    daLuigiStateFn enter;
    daLuigiStateFn execute;
};

#endif /* DALUIGI_C_H */
