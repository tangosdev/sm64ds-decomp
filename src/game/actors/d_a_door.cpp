//cpp
/* daDoor_c -- the plain warp door (registry profile DOOR).
 *
 * param1's high halfword picks the door's entry in the 16-byte table at
 * data_ov100_02148204 (its model and key-model files, the star count or key
 * it needs, the save-flag bit it sets and the message it shows). Read off the
 * ROM, that table has 19 entries, indexed by what InitResources leaves in
 * param1 (the "variant"): 0 and 1 carry no star or key; 2..6 need 1, 1, 3, 3
 * and 8 stars; 7..13 are key doors holding key indices 0..6; 14..18 carry
 * neither. A door is a state machine: the pointer at +0x140 (mState) names
 * the current state, a pair of pointers-to-member -- enter, then execute --
 * that SetState stores and calls the enter member of, and Behavior
 * calls the execute member of every frame with the player
 * func_ov100_02145370 returns. The nine states are the 16-byte tables
 * data_ov100_021488a4 .. data_ov100_02148924, which the module's static
 * initializer fills from the pointer-to-member constants at
 * 0x021480d4..0x02148144; the fourteen St_* members below are those
 * members, the rest are what they call. Which members each table holds
 * (enter / execute), read off that initializer:
 *
 *   021488a4  none                      / St_CharIntro_Main
 *               -- the exit sequence of func_ov100_02145080
 *   021488b4  none                      / St_StayClosed_Main
 *               -- the closed door
 *   021488c4  St_TalkingToPlayer_Init   / St_TalkingToPlayer_Main
 *               -- a message was shown
 *   021488d4  St_UnlockingWithStar_Init / St_UnlockingWithStar_Main
 *               -- star door: TryTalkToDoor took
 *   021488e4  St_UnlockingWithKey_Init  / St_UnlockingWithKey_Main
 *               -- key door: TryTalkToKeyDoor took
 *   021488f4  St_OpenClose_Init         / St_OpenClose_Main
 *               -- the swing
 *   02148904  St_CharChange_Init        / St_CharChange_Main
 *               -- animation, character switch, then on into the swing
 *   02148914  St_OpenClose_InitNearEnd  / St_OpenClose_Main
 *               -- the swing, starting 0x18 frames from its end
 *   02148924  none                      / St_Wait_Main
 *               -- wait for the countdown, then back to the closed door
 * ("none" is the null pointer-to-member the initializer copies from
 * 0x02086b58.)
 *
 * STATE NAMES. The fourteen state members and SetState carried their ROM
 * addresses until now; the names above are reconstructed, and the ROM records
 * none of them. They follow daStarGate_c (src/game/actors/d_a_star_gate.cpp,
 * include/daStarGate_c.h) -- the class whose code starts at 0x021458d4,
 * directly above this unit, and whose state machine is the same shape: a
 * 0x10 record of two pointers-to-member, a SetState that adopts a record
 * and runs its init half, a Behavior that runs the main half with the player,
 * and the same St_<state>_Init / St_<state>_Main spelling for the halves.
 * Four of its state names describe the same behaviour here and are reused
 * verbatim -- St_StayClosed_Main, St_OpenClose_Init/_Main,
 * St_TalkingToPlayer_Init/_Main and St_Wait_Main. This class splits
 * daStarGate_c's single St_Unlocking into St_UnlockingWithStar and
 * St_UnlockingWithKey because a door has both paths (one gated on the star
 * count, one on a held key) and they are separate state records here. Each
 * name is earned from the body below it; the ten functions that are still
 * address-named are the ones nothing in the unit names.
 *
 * This file is the whole linker unit 0x021443f4..0x021458d4, 32 functions:
 * D1 and D0 (daWanwan2_c_classInit, src/game/actors/d_a_wanwan2.cpp, ends exactly at
 * 0x021443f4 below them), the 24 helpers in 0x02144468..0x021453d8
 * (St_CharIntro_Main first, SetState last), CleanupResources, OnPendingDestroy, Render,
 * Behavior, InitResources, and last the registry factory daDoor_c_classInit
 * (0x0214589c); daStarGate_c's first function
 * (src/game/actors/d_a_star_gate.cpp) starts exactly at 0x021458d4 above
 * it. The out-of-line destructor is the key function, so this TU also
 * emits the vtable and the RTTI.
 *
 * It replaces the one-function sources for _ZN8daDoor_cD1Ev,
 * _ZN8daDoor_cD0Ev, the 24 helpers (each of those sources was named for the
 * address it held; config/tu_manifest.d/ov100/daDoor_c.json still lists them
 * under legacy_source),
 * _ZN8daDoor_c16CleanupResourcesEv, _ZN8daDoor_c16OnPendingDestroyEv,
 * _ZN8daDoor_c6RenderEv, _ZN8daDoor_c8BehaviorEv,
 * _ZN8daDoor_c13InitResourcesEv and daDoor_c_classInit. Each member keeps
 * the provenance notes its source carried.
 *
 * `#pragma defer_codegen off` keeps this file in ROM order.
 *
 * The helpers are real daDoor_c members now: every state-table entry is a
 * pointer-to-member into this class, and the rest are called on the door.
 *
 * comment leftovers:
 * - the ten members that are still address-named are the ones no state
 *   record, caller or callee in this unit names; see STATE NAMES above.
 *   func_ov100_02144f84 / func_ov100_02144fcc /
 *   func_ov100_02145014 / func_ov100_02145070 are static: they take no door.
 * - the callees with Fix12<int> parameters (Sound, dExtAnmModel_c::SetAnim),
 *   the SaveData, Sound::PlayCharVoice and dExtFrameCtrl_c::LoadFile calls, and the
 *   dExtSimpleModel_c loaders InitResources calls stay spelled as mangled extern-C free
 *   functions.
 * - dActor_c's mScale and mAngle.x/.z carry other things for a door
 *   (the player's position in the door's frame, and two area ids), and
 *   mSpeed holds the message position; the base name cannot change on
 *   one derived class's evidence, so the functions that read it describe
 *   it in their own comments.
 * - the sound ids other than the two swing pairs, the message ids, camera
 *   flag bits 0xc00 (not in include/dCamera_c.h) and SaveData flags2 bit 16 stay
 *   numbers; their meanings are not recovered here.
 * - the six door offsets are function-local static Vector3s: the player's
 *   front and back, the key spawn, the two enter offsets, and the spawn nudge.
 * - the state tables and the helpers that take one are declared
 *   daDoor_StateRecord; mState stays void * because func_ov100_021453d8
 *   installs the record by address.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daDoor_c_AssetId {
    ASSET_OBJ_DOOR0_STAR_BMD = 0x043e,
    ASSET_OBJ_DOOR0_STAR1_BMD = 0x043f,
    ASSET_OBJ_DOOR0_STAR10_BMD = 0x0440,
    ASSET_OBJ_DOOR0_STAR3_BMD = 0x0441,
    ASSET_OBJ_DOOR1_BMD = 0x0443,
    ASSET_OBJ_DOOR2_BORO_BMD = 0x0444,
    ASSET_OBJ_DOOR4_YAMI_BMD = 0x0445,
    ASSET_OBJ_DOOR5_HORROR_BMD = 0x0446
};

#pragma defer_codegen off

#include "daDoor_c.h"
#include "daPly_c.h"
#include "SaveData.h"
#include "types.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "dCamera_c.h"
#include "daObjKey_c.h"
#include "dExtSimpleModel_c.h"
#include "dExtFrameCtrl_c.h"
#include "dActor_c.h"
#include "Sound.h"
#include "nitro/fx/fx.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

/* The vector temps below are VecFx32: a stack Vector3 would run its destructor. */
/* One entry of data_ov100_02148204, indexed by param1 (the variant). */
struct DoorEntry {
    SharedFilePtr *file;    /* 0x0: the door model */
    SharedFilePtr *keyFile; /* 0x4: a decoration model hung on the door, or
                                null (see mKeyModel) */
    s8 starsNeeded;         /* 0x8: >= 0: a star door, the star count it needs */
    s8 keyIndex;            /* 0x9: >= 0: a key door, the key's index */
    s8 flagShift;           /* 0xa: the door's bit position in the save flags
                                (DoorSaveFlag below); -1 for the variants that
                                carry no star or key */
    s8 pad_b;
    s16 msg; /* 0xc: the message shown when it stays shut */
    s16 pad_e;
};

/* Variants of the door (param1 once InitResources has shifted it down). */
enum DoorVariant {
    DOOR_FIRST_KEY_MODEL = 9,  /* 9..0xd: InitResources loads key models for
                                   these, and mKeyModelIdx is param1 - 8
                                   (zeroed again for 0xc) */
    DOOR_LAST_KEY_MODEL = 0xd, /* the upper bound of that range; also the one
                                   variant that skips the character switch and
                                   uses TryExitWhiteDoorWithStar */
    DOOR_ALT_SOUND = 0x10      /* plays the _ALT swing sounds below */
};

/* Sound ids the swing plays through func_02012694 (the sound is placed at the
   door's camera-space position). Named by when they play; what the
   cartridge called them is not recovered. */
enum DoorSound {
    SND_SWING_START = 4,     /* St_OpenClose_Init: the swing is starting */
    SND_SWING_END = 5,       /* the door animation has finished (also in
                                   St_OpenClose_Main, as the area changes) */
    SND_SWING_START_ALT = 6, /* the same two for DOOR_ALT_SOUND */
    SND_SWING_END_ALT = 7
};

/* Bits of the save flags (gSaveBlock.file.flags1) the door uses. The shift is
   DoorEntry::flagShift, except for the key bit, which is shifted by
   DoorEntry::keyIndex. */
enum DoorSaveFlag {
    DOOR_SAVE_HAVE_KEY = 2,          /* << keyIndex: with it set the door offers
                                        TryTalkToKeyDoor; read here as "the
                                        player holds that key" */
    DOOR_SAVE_KEY_UNLOCKED = 0x100,  /* << flagShift: set by the key door's
                                        execute state; St_StayClosed_Main
                                        then lets the player straight through */
    DOOR_SAVE_STAR_UNLOCKED = 0x8000 /* << flagShift: set by the star door's
                                        execute state, with the same effect */
};

/* dCamera_c::mFlags bits the door sets when a swing starts (St_OpenClose_Init)
   and clears near and at its end. Not documented in include/dCamera_c.h. */
#define DOOR_CAMERA_FLAGS 0xc00

/* ov089's OBJ_KEY (symbols/actor_debug_names.tsv: 282), which a key door
   spawns beside the player. */
enum { ACTOR_OBJ_KEY = 0x11a };

/* One state: the 16-byte {enter, execute} pointer-to-member pair
   data_ov100_021488a4 .. data_ov100_02148924 are built from. The player is
   handed over as an int. */
typedef int (daDoor_c::*daDoor_StatePmf)(int);
struct daDoor_StateRecord {
    daDoor_StatePmf enter;   /* 0x0 -- called by SetState */
    daDoor_StatePmf execute; /* 0x8 -- called by Behavior every frame */
};

struct Vector3_16;

/* Defined below, called before their definitions. These four are the shared
   countdown helpers -- free functions, not members: d_a_ply_methods.cpp, daMugenBGM_c
   and d_a_star_gate.cpp call them too, and none takes the door. */
extern "C" unsigned int func_0201277c(unsigned int a);
void func_02012694(unsigned int a, const Vector3 *b);
unsigned char NumStars(void);
extern "C" void ChangeArea(int areaID);
void ShowArea(int idx);
unsigned char IsAreaShowing(int idx);
void SetTouchScreenDelay(void);
void LoadKeyModels(int idx);
void UnloadKeyModels(int idx);
extern "C" {
int func_ov002_020ca78c(void *p);
int AngleDiff(int a, int b);
}

extern DoorEntry data_ov100_02148204[19];
/* Latch: set (St_UnlockingWithKey_Init) when the door being opened has a nonzero
   mKeyModelIdx, cleared by St_CharIntro_Main; the swing and exit states
   branch on it. */
extern unsigned char data_ov100_02148704;
/* The shared countdown: ticked by func_ov100_02145014, started by
   func_ov100_02145070. */
extern unsigned short data_ov100_02148708[];
/* The null member the initializer copies for the states that have no enter
   member. */
extern daDoor_StatePmf __ptmf_null;

/* Indexed by mKeyModelIdx - 1: the character func_ov100_02144528 switches the
   player to. */
unsigned char data_ov100_021480d0[4] = {0x00, 0x01, 0x02, 0x00};

/* The camera, and the player array indexed by the current player id. */
extern dCamera_c *gCamera;
extern unsigned char gLocalPlayerId;
extern daPly_c *gPlayers[];
/* The current game mode. */
extern unsigned char gGameMode;
/* The next-level id (signed byte). */
extern signed char gNextSublevel;
/* The music and sub-channel volumes, both in fx32 (0x7f000 = 127.0). */
extern int gMusicVolume;
extern int gSubVolume;
extern Vector3 data_020a0ebc;
extern SharedFilePtr gPowerStarModelFile;
extern SharedFilePtr gKoopaKeyModelFile;
/* ov089's key-model file table, not a function: eight SharedFilePtr*
   entries in that overlay's .rodata, the table LoadKeyModels loads into
   (bounds-checked `idx >= 8`). src/game/actors/d_a_obj_key.cpp, home of
   LoadKeyModels and UnloadKeyModels, declares it under this name with this
   element type. */
extern void *gKeyModelFiles[];

/* The door's file-scope SharedFilePtr objects, spelled through the same
   wrapper idiom the landed folds use: the handles' code is the veneer pair
   the cartridge already carries, so the wrappers get no bodies of their
   own. The constructors alias func_02017acc (model) and
   SharedFilePtr::Construct (animation) in this unit's manifest, and the
   destructors func_02017ab4 and SharedFilePtr_Destruct_Anim. */
struct DoorModelFilePtr : SharedFilePtr {
    DoorModelFilePtr(unsigned int id);
    ~DoorModelFilePtr();
};

struct DoorAnimationFileHandle : SharedFilePtr {
    DoorAnimationFileHandle(unsigned int id);
    ~DoorAnimationFileHandle();
};

/* -------------------------------------------------------------------------- */
/* The eleven model handles and the animation handle, in static-initializer
   order: the initializer constructs each and registers its destructor.
   gDoorOpenAnimationFile is the one the helpers release and hand to
   dExtFrameCtrl_c::LoadFile. */
/* -------------------------------------------------------------------------- */
DoorModelFilePtr gRegularDoorModelFile(0x9c09);
DoorModelFilePtr gDoorKeyHoleModelFile(0x9c0a);
DoorModelFilePtr gObjDoor0StarModelFile(ASSET_OBJ_DOOR0_STAR_BMD);
DoorModelFilePtr gObjDoor0Star1ModelFile(ASSET_OBJ_DOOR0_STAR1_BMD);
DoorModelFilePtr gObjDoor0Star3ModelFile(ASSET_OBJ_DOOR0_STAR3_BMD);
DoorModelFilePtr gObjDoor0Star10ModelFile(ASSET_OBJ_DOOR0_STAR10_BMD);
DoorModelFilePtr gObjDoor1ModelFile(ASSET_OBJ_DOOR1_BMD);
DoorModelFilePtr gObjDoor2BoroModelFile(ASSET_OBJ_DOOR2_BORO_BMD);
DoorModelFilePtr gRustedMetalDoorModelFile(0x9c0b);
DoorModelFilePtr gObjDoor4YamiModelFile(ASSET_OBJ_DOOR4_YAMI_BMD);
DoorModelFilePtr gObjDoor5HorrorModelFile(ASSET_OBJ_DOOR5_HORROR_BMD);
DoorAnimationFileHandle gDoorOpenAnimationFile(0x9c08);

DoorEntry data_ov100_02148204[19] = {
    {&gRegularDoorModelFile, 0, -1, -1, -1, 0, -1, 0},
    {&gRegularDoorModelFile, &gObjDoor0StarModelFile, -1, -1, -1, 0, -1, 0},
    {&gRegularDoorModelFile, &gObjDoor0Star1ModelFile, 1, -1, 0, 0, 25, 0},
    {&gRegularDoorModelFile, &gObjDoor0Star1ModelFile, 1, -1, 1, 0, 25, 0},
    {&gRegularDoorModelFile, &gObjDoor0Star3ModelFile, 3, -1, 2, 0, 26, 0},
    {&gRegularDoorModelFile, &gObjDoor0Star3ModelFile, 3, -1, 3, 0, 26, 0},
    {&gRegularDoorModelFile, &gObjDoor0Star10ModelFile, 8, -1, 4, 0, 27, 0},
    {&gRegularDoorModelFile, &gDoorKeyHoleModelFile, -1, 0, 0, 0, 24, 0},
    {&gRegularDoorModelFile, &gDoorKeyHoleModelFile, -1, 1, 1, 0, 24, 0},
    {&gRegularDoorModelFile, &gDoorKeyHoleModelFile, -1, 2, 2, 0, 32, 0},
    {&gRegularDoorModelFile, &gDoorKeyHoleModelFile, -1, 3, 3, 0, 33, 0},
    {&gRegularDoorModelFile, &gDoorKeyHoleModelFile, -1, 4, 4, 0, 34, 0},
    {&gRegularDoorModelFile, &gDoorKeyHoleModelFile, -1, 5, 5, 0, 24, 0},
    {&gRegularDoorModelFile, &gDoorKeyHoleModelFile, -1, 6, 6, 0, 24, 0},
    {&gObjDoor1ModelFile, 0, -1, -1, -1, 0, -1, 0},
    {&gObjDoor2BoroModelFile, 0, -1, -1, -1, 0, -1, 0},
    {&gRustedMetalDoorModelFile, 0, -1, -1, -1, 0, -1, 0},
    {&gObjDoor4YamiModelFile, 0, -1, -1, -1, 0, -1, 0},
    {&gObjDoor5HorrorModelFile, 0, -1, -1, -1, 0, -1, 0},
};

/* -------------------------------------------------------------------------- */
/* The nine door states, in static-initializer order. Each is a 16-byte
   {enter, execute} pair; the enterless states copy the null record arm9
   keeps at __ptmf_null. mwcc writes the member images this initializer
   reads as the .data constants at 0x021480d4..0x02148144, as the
   cartridge has them. */
/* -------------------------------------------------------------------------- */
daDoor_StateRecord data_ov100_021488b4 = {__ptmf_null,
                                          (daDoor_StatePmf)&daDoor_c::St_StayClosed_Main};
daDoor_StateRecord data_ov100_021488c4 = {(daDoor_StatePmf)&daDoor_c::St_TalkingToPlayer_Init,
                                          (daDoor_StatePmf)&daDoor_c::St_TalkingToPlayer_Main};
daDoor_StateRecord data_ov100_021488d4 = {(daDoor_StatePmf)&daDoor_c::St_UnlockingWithStar_Init,
                                          (daDoor_StatePmf)&daDoor_c::St_UnlockingWithStar_Main};
daDoor_StateRecord data_ov100_021488e4 = {(daDoor_StatePmf)&daDoor_c::St_UnlockingWithKey_Init,
                                          (daDoor_StatePmf)&daDoor_c::St_UnlockingWithKey_Main};
daDoor_StateRecord data_ov100_021488f4 = {(daDoor_StatePmf)&daDoor_c::St_OpenClose_Init,
                                          (daDoor_StatePmf)&daDoor_c::St_OpenClose_Main};
daDoor_StateRecord data_ov100_02148904 = {(daDoor_StatePmf)&daDoor_c::St_CharChange_Init,
                                          (daDoor_StatePmf)&daDoor_c::St_CharChange_Main};
daDoor_StateRecord data_ov100_02148914 = {(daDoor_StatePmf)&daDoor_c::St_OpenClose_InitNearEnd,
                                          (daDoor_StatePmf)&daDoor_c::St_OpenClose_Main};
daDoor_StateRecord data_ov100_02148924 = {__ptmf_null, (daDoor_StatePmf)&daDoor_c::St_Wait_Main};
daDoor_StateRecord data_ov100_021488a4 = {__ptmf_null,
                                          (daDoor_StatePmf)&daDoor_c::St_CharIntro_Main};

/* recovered: real C++ destructor -- the compiler emits the whole body.
 *
 * Store this class's vtable over the one the base constructor left, destroy
 * mModel, then run dActor_c's destructor; D0 adds the inline operator
 * delete. All of it follows from `struct daDoor_c : dActor_c` and the member
 * types in the header. */
daDoor_c::~daDoor_c() {}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- St_CharIntro_Main, 0x02144468, size 0x80 */
/* -------------------------------------------------------------------------- */
/* State 021488a4, execute member. Once func_ov100_02145080 lets the player
   out (it returns 0), clear the data_ov100_02148704 latch, call
   SaveData::SetCharacterIntro(mKeyModelIdx - 1) (or, for variant 0xd,
   Sound::ChangeMusicVolume(0x7f, 0xcb33 = 12.7 as fx32)) and go to state
   data_ov100_021488b4. The player is taken as the int the member-pointer call
   passes. Was a C source. */
int daDoor_c::St_CharIntro_Main(int p) {
    if (!func_ov100_02145080((daPly_c *)p)) {
        data_ov100_02148704 = 0;
        if (param1 != DOOR_LAST_KEY_MODEL) {
            SaveData::SetCharacterIntro(mKeyModelIdx - 1);
        } else {
            Sound::ChangeMusicVolume(0x7f, 0xcb33);
        }
        SetState(&data_ov100_021488b4, p);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- St_Wait_Main, 0x021444e8, size 0x40 */
/* -------------------------------------------------------------------------- */
/* State 02148924, execute member. Go to state data_ov100_021488b4 once
   func_ov100_02144fcc reports the shared countdown done. Was a C source. */
int daDoor_c::St_Wait_Main(int a1) {
    if (daDoor_c::func_ov100_02144fcc()) {
        SetState(&data_ov100_021488b4, a1);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- St_CharChange_Main, 0x02144528, size 0x1d0 */
/* -------------------------------------------------------------------------- */
/* State 02148904, execute member. Run the door animation; once it has
   finished, and while mTimer is zero:
     - variants other than 0xd switch the player to
       data_ov100_021480d0[mKeyModelIdx - 1] (character 3 when that is already
       the current character), daPly_c::SetRealCharacter. With the
       data_ov100_02148704 latch set they also set the player's mHasNoCap,
       play sound 0x7e, set mTimer to 0x40 (64) and call
       Sound::ChangeMusicVolume(0, 0x19666 = 25.4 as fx32); with it clear
       they play sound 0x7f;
     - variant 0xd sets mTimer to 0x40, plays sound 0xa1 (latch set) or 0xa2
       and calls the same ChangeMusicVolume(0, ...);
   then play the swing-end sound. Next count mTimer down: at zero, send the
   player through with func_ov100_021451c4 (given no state when the latch is
   set, which makes it skip CanEnterDoor) and, when that succeeds under the
   latch, open the big door; when it refuses, mTimer is left at 1. While it
   is still counting, variant 0xd with the latch clear calls
   ChangeMusicVolume(0x7f, 0xcb33) as mTimer reaches 0xa. Was a C source. */
int daDoor_c::St_CharChange_Main(daPly_c *pl) {
    mModel.Advance();
    if (mModel.Finished() != 0) {
        if (mTimer == 0) {
            if (param1 != DOOR_LAST_KEY_MODEL) {
                unsigned int ch = data_ov100_021480d0[mKeyModelIdx - 1];
                if (ch == gSaveBlock.file.mCharacter)
                    ch = 3;
                pl->SetRealCharacter(ch);
                if (data_ov100_02148704 != 0) {
                    pl->mHasNoCap = 1;
                    func_0201277c(0x7e);
                    mTimer = 64;
                    Sound::ChangeMusicVolume(0, 0x19666); /* 25.4 as fx32 */
                } else {
                    func_0201277c(0x7f);
                }
            } else {
                mTimer = 64;
                if (data_ov100_02148704 != 0)
                    func_02012790(0xa1);
                else
                    func_02012790(0xa2);
                Sound::ChangeMusicVolume(0, 0x19666);
            }
            func_02012694(param1 == DOOR_ALT_SOUND ? SND_SWING_END_ALT : SND_SWING_END,
                          &mCamSpacePos);
        }
        if (DecIfAbove0_Byte(&mTimer) == 0) {
            void *r1 = data_ov100_02148704 != 0 ? 0 : &data_ov100_021488f4;
            if (func_ov100_021451c4(r1, pl) != 0) {
                if (data_ov100_02148704 != 0)
                    pl->OpenBigDoor();
            } else {
                mTimer += 1;
            }
        } else {
            if (data_ov100_02148704 == 0 && mTimer == 10 && param1 == DOOR_LAST_KEY_MODEL) {
                Sound::ChangeMusicVolume(0x7f, 0xcb33);
            }
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- St_CharChange_Init, 0x021446f8, size 0x38 */
/* -------------------------------------------------------------------------- */
/* State 02148904, enter member. Open the big door, zero mTimer and carry on
   through St_OpenClose_Init. Was a C source. */
void daDoor_c::St_CharChange_Init(daPly_c *r1) {
    r1->OpenBigDoor();
    mTimer = 0;
    St_OpenClose_Init(r1, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- St_OpenClose_Main, 0x02144730, size 0x1ec */
/* -------------------------------------------------------------------------- */
/* States 021488f4 and 02148914, execute member: the door swinging. Under the
   data_ov100_02148704 latch, once the current frame is past the animation's
   frame count minus 0x28 (40) it runs func_ov100_02145080 every frame. When
   the animation finishes it goes to state data_ov100_021488a4 (latch set) or
   data_ov100_021488b4, stores the area id on the far side (the low byte of
   mAngle.x when mScale.z is negative, else of mAngle.z -- see Render) as the
   player's mAreaId and ChangeArea's it, clears the DOOR_CAMERA_FLAGS camera
   bits and plays the swing-end sound. Before the end: with a nonzero
   mKeyModelIdx, on the frame 0x1c (28) from the end it clears the camera
   bits and plays the character voice (or, for variant 0xd with the latch
   clear, the player's Mamma Mia sound); otherwise, on the frame 0x18 (24)
   from the end, it sends the camera behind the player (dCamera_c::GoBehindPlayer
   with the current player id) and, when the next-level id is not negative,
   zeroes the animation's speed. */
int daDoor_c::St_OpenClose_Main(daPly_c *player) {
    mModel.Advance();
    if (data_ov100_02148704 != 0) {
        int f = mModel.currFrame;
        if ((unsigned)(f << 4) >> 16 > (u16)(mModel.GetFrameCount() - 0x28))
            func_ov100_02145080(player);
    }

    if (mModel.Finished() != 0) {
        int t;
        if (data_ov100_02148704 != 0)
            SetState(&data_ov100_021488a4, (int)player);
        else
            SetState(&data_ov100_021488b4, (int)player);
        if (mScale.z < 0)
            t = mAngle.x;
        else
            t = mAngle.z;
        t = (s8)t;
        player->mAreaId = t;
        ChangeArea(t);
        {
            gCamera->mFlags &= ~DOOR_CAMERA_FLAGS;
        }
        func_02012694(param1 == DOOR_ALT_SOUND ? SND_SWING_END_ALT : SND_SWING_END, &mCamSpacePos);
    } else if (mKeyModelIdx != 0) {
        if (mModel.WillHitFrame((u16)(mModel.GetFrameCount() - 0x1c)) != 0) {
            gCamera->mFlags &= ~DOOR_CAMERA_FLAGS;
            if (param1 != DOOR_LAST_KEY_MODEL) {
                Sound::PlayCharVoice(gSaveBlock.file.mCharacter, 0x21, mCamSpacePos);
            } else if (data_ov100_02148704 == 0) {
                player->PlayMammaMiaSound();
            }
        }
    } else {
        if (mModel.WillHitFrame((u16)(mModel.GetFrameCount() - 0x18)) != 0) {
            if (gNextSublevel >= 0)
                mModel.speed = 0;
            gCamera->GoBehindPlayer(gLocalPlayerId);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- St_OpenClose_InitNearEnd, 0x0214491c, size 0x34 */
/* -------------------------------------------------------------------------- */
/* State 02148914, enter member: start the door animation 0x18 (24) frames
   from its end (the current frame is 20.12, hence the << 12) and zero mTimer.
   Was a C source. */
int daDoor_c::St_OpenClose_InitNearEnd() {
    int n = mModel.GetFrameCount();
    int f = ((unsigned short)(n - 0x18)) << 12;
    mModel.currFrame = f;
    mTimer = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- St_OpenClose_Init, 0x02144950, size 0x78 */
/* -------------------------------------------------------------------------- */
/* State 021488f4, enter member (and what St_CharChange_Init carries on
   with): rewind the door animation, show both areas the door joins (the low
   bytes of mAngle.x and mAngle.z), set the camera flag bits DOOR_CAMERA_FLAGS,
   play the swing-start sound and zero mTimer. Was a C source, which read the
   two area halfwords through an inline helper; St_CharChange_Init calls it
   with the player and a third argument it never reads. */
int daDoor_c::St_OpenClose_Init(daPly_c *pl, int unused) {
    mModel.currFrame = 0;
    ShowArea((signed char)mAngle.x);
    ShowArea((signed char)mAngle.z);
    gCamera->mFlags |= DOOR_CAMERA_FLAGS;
    func_02012694((param1 == DOOR_ALT_SOUND) ? SND_SWING_START_ALT : SND_SWING_START,
                  &mCamSpacePos);
    mTimer = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- St_UnlockingWithKey_Main, 0x021449c8, size 0x70 */
/* -------------------------------------------------------------------------- */
/* State 021488e4, execute member. Once daPly_c::Unk_020ca488 returns 0, set
   this door's DOOR_SAVE_KEY_UNLOCKED save-flag bit and go to state
   data_ov100_021488b4. */
int daDoor_c::St_UnlockingWithKey_Main(daPly_c *a2) {
    if (!a2->Unk_020ca488()) {
        signed char sh =
            (((signed char *)data_ov100_02148204) +
             (param1
              << 4))[0xa]; /* flagShift; the array-indexed spelling compiles to a different load */
        gSaveBlock.file.flags1 |= DOOR_SAVE_KEY_UNLOCKED << sh;
        SetState(&data_ov100_021488b4, (int)a2);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- St_UnlockingWithKey_Init, 0x02144a38, size 0x1bc */
/* -------------------------------------------------------------------------- */
/* State 021488e4, enter member. Open the big door, place the player at the
   door's front or back (func_ov100_02145170, offsets (75, 0, 110) and
   (75, 0, -110) units from the door), and spawn an OBJ_KEY (param1 = the
   door's key index) at (-76, 0, 109) units from the player, rotated by the
   player's facing, then start the key's animation 2 through
   func_ov089_0213115c. A nonzero mKeyModelIdx sets the
   data_ov100_02148704 latch. Was a C source. */
int daDoor_c::St_UnlockingWithKey_Init(daPly_c *p) {
    Vector3 pos;
    DoorEntry *e;

    p->OpenBigDoor();

    /* 75 units forward and 110 to the side, or 110 the other way. */
    static Vector3 sFrontOff(0x4b000, 0, 0x6e000);
    static Vector3 sBackOff(0x4b000, 0, -0x6e000);

    func_ov100_02145170(p, &sFrontOff, &sBackOff);

    /* Key sits 76 units back and 109 to the side of the player. */
    static Vector3 sKeyOff(-0x4c000, 0, 0x6d000);

    e = &data_ov100_02148204[param1];

    Vec3_RotateYAndTranslate(&pos, &p->mPos, p->mAngle.y, &sKeyOff);

    {
        void *actor = dActor_c::Spawn(ACTOR_OBJ_KEY, e->keyIndex, pos, &p->mAngle, p->mAreaId, -1);
        if (actor != 0) {
            ((daObjKey_c *)actor)->func_ov089_0213115c(2);
        }
    }

    if (mKeyModelIdx != 0) {
        data_ov100_02148704 = 1;
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- St_UnlockingWithStar_Main, 0x02144bf4, size 0x70 */
/* -------------------------------------------------------------------------- */
/* State 021488d4, execute member. Unless the player is opening the door
   with a star, set this door's DOOR_SAVE_STAR_UNLOCKED save-flag bit and
   send the player through (state data_ov100_021488f4). */
int daDoor_c::St_UnlockingWithStar_Main(daPly_c *a2) {
    if (!a2->IsOpeningDoorWithStar()) {
        signed char sh =
            (((signed char *)data_ov100_02148204) +
             (param1
              << 4))[0xa]; /* flagShift; the array-indexed spelling compiles to a different load */
        gSaveBlock.file.flags1 |= DOOR_SAVE_STAR_UNLOCKED << sh;
        func_ov100_021451c4(&data_ov100_021488f4, a2);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- St_UnlockingWithStar_Init, 0x02144c64, size 0x8 */
/* -------------------------------------------------------------------------- */
/* State 021488d4, enter member: does nothing. Was a C source. */
int daDoor_c::St_UnlockingWithStar_Init() {
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- St_TalkingToPlayer_Main, 0x02144c6c, size 0x60 */
/* -------------------------------------------------------------------------- */
/* State 021488c4, execute member. Tick the shared countdown; once
   daPly_c::GetTalkState() returns -1 and the player has left the door's box,
   go to state data_ov100_02148924. Was a C source. */
int daDoor_c::St_TalkingToPlayer_Main(daPly_c *r1) {
    daDoor_c::func_ov100_02145014();
    if (r1->GetTalkState() == -1) {
        if (func_ov100_021452e4(r1) == 0) {
            SetState(&data_ov100_02148924, (int)r1);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- St_TalkingToPlayer_Init, 0x02144ccc, size 0x2c */
/* -------------------------------------------------------------------------- */
/* State 021488c4, enter member. Play sound 0xb8 at the door and start the
   shared countdown at 0x87 (135). Was a C source. */
int daDoor_c::St_TalkingToPlayer_Init() {
    func_02012694(0xb8, &mCamSpacePos);
    daDoor_c::func_ov100_02145070(0x87);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- St_StayClosed_Main, 0x02144cf8, size 0x28c */
/* -------------------------------------------------------------------------- */
/* State 021488b4, execute member: the closed door. When the player is in its
   box (func_ov100_021452e4), decide:
     - a star door (starsNeeded >= 0) that is not yet unlocked, with fewer
       stars than it needs: show its message (when func_ov100_02144f84
       allows one) and go to state data_ov100_021488c4; with enough stars,
       call TryTalkToDoor(0) and go to data_ov100_021488d4;
     - a key door (keyIndex >= 0) that is not yet unlocked: with
       DOOR_SAVE_HAVE_KEY << keyIndex set, call TryTalkToKeyDoor and go to
       data_ov100_021488e4; without it, show a message and go to
       data_ov100_021488c4. The message is 0x28 or 0x23 for key index 6 (by
       whether NumGlowingRabbitsFound is nonzero); otherwise the entry's
       message when the variant is 9..0xb or any of the flags1 bits 0xbe is
       set, else message 0x17. After the message for key index 5, flags2 bit
       16 is also set;
     - anything else: the player goes through (func_ov100_021451c4), into
       data_ov100_02148904 for a nonzero mKeyModelIdx, else
       data_ov100_021488f4.
   Where ShowMessage, TryTalkToDoor or TryTalkToKeyDoor returns 0, control
   goes to the last case. The message position is mSpeed (see
   InitResources). Was a C source. */
int daDoor_c::St_StayClosed_Main(daPly_c *b) {
    DoorEntry *entry;

    if (func_ov100_021452e4(b)) {
        entry = &data_ov100_02148204[param1];
        if (entry->starsNeeded >= 0) {
            if ((gSaveBlock.file.flags1 & (DOOR_SAVE_STAR_UNLOCKED << entry->flagShift)) == 0) {
                if (NumStars() < entry->starsNeeded) {
                    if (daDoor_c::func_ov100_02144f84() == 0)
                        return 1;
                    if (b->ShowMessage(*this, entry->msg, &mSpeed, 0, 2) != 0) {
                        SetState(&data_ov100_021488c4, (int)b);
                        return 1;
                    }
                } else if (b->TryTalkToDoor(0) != 0) {
                    SetState(&data_ov100_021488d4, (int)b);
                    return 1;
                }
            }
        } else {
            int e9 = entry->keyIndex;
            if (e9 >= 0) {
                int val = gSaveBlock.file.flags1;
                if ((val & (DOOR_SAVE_KEY_UNLOCKED << entry->flagShift)) == 0) {
                    if ((val & (DOOR_SAVE_HAVE_KEY << e9)) == 0) {
                        if (daDoor_c::func_ov100_02144f84() == 0)
                            return 1;
                        {
                            int msg;
                            if (entry->keyIndex == 6) {
                                msg = SaveData::NumGlowingRabbitsFound() ? 0x28 : 0x23;
                            } else {
                                int idx2 = param1;
                                int sel = 1;
                                int inRange =
                                    (unsigned)idx2 >= DOOR_FIRST_KEY_MODEL && (unsigned)idx2 <= 0xb;
                                if (!inRange) {
                                    if (!(gSaveBlock.file.flags1 & 0xbe))
                                        sel = 0;
                                }
                                msg = sel ? entry->msg : 0x17;
                            }
                            if (b->ShowMessage(*this, (short)msg, &mSpeed, 0, 2) != 0) {
                                SetState(&data_ov100_021488c4, (int)b);
                                if (entry->keyIndex == 5)
                                    gSaveBlock.file.flags2 |= 0x10000;
                                return 1;
                            }
                        }
                    } else if (b->TryTalkToKeyDoor() != 0) {
                        SetState(&data_ov100_021488e4, (int)b);
                        return 1;
                    }
                }
            }
        }
        func_ov100_021451c4((mKeyModelIdx != 0) ? &data_ov100_02148904 : &data_ov100_021488f4, b);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov100_02144f84, 0x02144f84, size 0x48 */
/* -------------------------------------------------------------------------- */
/* 1 when the shared countdown is zero, the music volume gMusicVolume is at
   0x7f000 (127.0 as fx32) and the sub-channel volume gSubVolume is
   zero. Was a C source. */
int daDoor_c::func_ov100_02144f84() {
    return *data_ov100_02148708 == 0 && gMusicVolume == 0x7f000 && gSubVolume == 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov100_02144fcc, 0x02144fcc, size 0x48 */
/* -------------------------------------------------------------------------- */
/* Tick the shared countdown (func_ov100_02145014); when it is done, call
   Sound::PlaySub(0x24, 0x7f, 0, 0x7222 = 7.13 as fx32, 0) and return its
   result, else 0. */
int daDoor_c::func_ov100_02144fcc() {
    int r = daDoor_c::func_ov100_02145014();
    if (r == 0)
        return 0;
    return Sound::PlaySub(0x24, 0x7f, 0, 0x7222, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov100_02145014, 0x02145014, size 0x5c */
/* -------------------------------------------------------------------------- */
/* Tick the shared countdown data_ov100_02148708 (DecIfAbove0_Short); while
   the ticked value is still nonzero, call Sound::PlaySub(0x24, 0x14, 0x7f,
   0x15666 = 21.4 as fx32, 0). Returns 1 once the countdown is zero. Was a C
   source. */
int daDoor_c::func_ov100_02145014() {
    int r = DecIfAbove0_Short(data_ov100_02148708);
    if (r) {
        Sound::PlaySub(0x24, 0x14, 0x7f, 0x15666, 0);
    }
    return data_ov100_02148708[0] == 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov100_02145070, 0x02145070, size 0x10 */
/* -------------------------------------------------------------------------- */
/* Start the shared countdown at v. Was a C source. */
void daDoor_c::func_ov100_02145070(int v) {
    data_ov100_02148708[0] = v;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov100_02145080, 0x02145080, size 0xf0 */
/* -------------------------------------------------------------------------- */
/* The exit timing, branching on the variant. Variant 0xd stores the 0/1
   result of daPly_c::TryExitWhiteDoorWithStar in mTimer. Every other variant
   is timed on mTimer: while it is zero it waits for func_ov002_020ca78c(player) to
   report the player ready, then plays sound 0x2b through Sound::PlaySub and
   sets mTimer to 0x78 (120); it then counts that down, plays sound 0x2b again
   and bumps mTimer to 1 on the frame the player is let out. Returns 0 on that
   frame (and, for variant 0xd, while mTimer is nonzero); 1 otherwise. Was a C
   source. */
int daDoor_c::func_ov100_02145080(daPly_c *player) {
    if (param1 != DOOR_LAST_KEY_MODEL) {
        if (mTimer == 0) {
            if (func_ov002_020ca78c(player) != 0) {
                Sound::PlaySub(0x2b, 0, 0x7f, 0x15666, 0);
                mTimer = 120;
            }
        } else if (DecIfAbove0_Byte(&mTimer) == 0) {
            Sound::PlaySub(0x2b, 0x7f, 0, 0x7222, 0);
            mTimer += 1;
            return 0;
        }
    } else if (mTimer == 0) {
        mTimer = player->TryExitWhiteDoorWithStar();
    } else {
        return 0;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov100_02145170, 0x02145170, size 0x54 */
/* -------------------------------------------------------------------------- */
/* Place the player at the door: offset a, facing the door's mAngle.y turned
   half round, when mScale.z > 0; offset b, facing mAngle.y, otherwise -- the new facing also copied to the player's
   mPrevAngle.y, and either offset rotated by mAngle.y about the door's
   position. Was a C source taking the door and the player as char *. */
void daDoor_c::func_ov100_02145170(daPly_c *pl, Vector3 *a, Vector3 *b) {
    Vector3 *v;
    if (mScale.z > 0) {
        pl->mAngle.y = mAngle.y + 0x8000;
        v = a;
    } else {
        pl->mAngle.y = mAngle.y;
        v = b;
    }
    pl->mPrevAngle.y = pl->mAngle.y;
    Vec3_RotateYAndTranslate(&pl->mPos, &mPos, mAngle.y, v);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov100_021451c4, 0x021451c4, size 0x120 */
/* -------------------------------------------------------------------------- */
/* Send the player through: unless no state was given (then
   data_ov100_021488f4 is used without asking), the player must be able to
   enter from its side (daPly_c::CanEnterDoor, told whether mScale.z <= 0);
   place it through func_ov100_02145170 with the offsets (0, 0, 100) and
   (0, 0, -100) units, go to the given state and call SetTouchScreenDelay.
   Returns 0 when the player cannot enter. */
int daDoor_c::func_ov100_021451c4(void *r5, daPly_c *r4) {
    if (r5 == 0) {
        r5 = &data_ov100_021488f4;
    } else if (r4->CanEnterDoor(mScale.z <= 0) == 0) {
        return 0;
    }
    /* 100 units to either side of the door. */
    static Vector3 sEnterPos(0, 0, 0x64000);
    static Vector3 sEnterNeg(0, 0, -0x64000);
    func_ov100_02145170(r4, &sEnterPos, &sEnterNeg);
    SetState(r5, (int)r4);
    SetTouchScreenDelay();
    return 1;
}

static inline int Abs(int v) {
    return v < 0 ? -v : v;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov100_021452e4, 0x021452e4, size 0x8c */
/* -------------------------------------------------------------------------- */
/* 1 when the player is inside the door's box (the player's door-relative
   offset in mScale within 75, 50 and 110 units, bounds inclusive) and
   AngleDiff between the player's mAngle.y and the door's is below 0x2000 (45
   degrees) -- the door's mAngle.y when mScale.z is negative, half a turn from
   it otherwise. Was a C source. */
int daDoor_c::func_ov100_021452e4(daPly_c *r1) {
    int a;
    if (Abs(mScale.x) <= 0x4b000 && Abs(mScale.y) <= 0x32000 && Abs(mScale.z) <= 0x6e000) {
        if (mScale.z < 0)
            a = mAngle.y;
        else
            a = mAngle.y + 0x8000;
        a = (short)a;
        if (AngleDiff(a, r1->mAngle.y) < 0x2000)
            return 1;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov100_02145370, 0x02145370, size 0x68 */
/* -------------------------------------------------------------------------- */
/* The player's position in the door's frame (the offset from the door,
   rotated by minus mAngle.y), stored into the door's
   mScale, which a door does not use as a scale; returns the player
   (gPlayers[gLocalPlayerId], the current one). Was a C source. */
daPly_c *daDoor_c::func_ov100_02145370() {
    Vector3 v;
    daPly_c *r5 = gPlayers[gLocalPlayerId];
    Vec3_Sub(&v, &r5->mPos, &mPos);
    Vec3_RotateYAndTranslate(&mScale, &data_020a0ebc, (-mAngle.y), &v);
    return r5;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- SetState, 0x021453d8, size 0x54 */
/* -------------------------------------------------------------------------- */
/* Enter a state: store the address of the state table at +0x140 (mState) and
   call its enter member with the player, if it has one. */
int daDoor_c::SetState(void *p, int a2) {
    mState = p;
    daDoor_StateRecord *q = (daDoor_StateRecord *)mState;
    if (q->enter == 0)
        return 1;
    return (this->*q->enter)(a2);
}

/* vtable slot 3. `delete key` IS the ROM's `ldr r1,[r0]; ldr r1,[r1,#4];
 * blx r1` -- vtable slot 1, dExtSimpleModel_c's deleting destructor -- and it is also
 * where the doubled null check comes from: the explicit `if` emits one and
 * `delete` emits its own. */
s32 daDoor_c::CleanupResources() {
    DoorEntry *e = &data_ov100_02148204[param1];
    dExtSimpleModel_c *key;
    e->file->Release();
    gDoorOpenAnimationFile.Release();
    key = mKeyModel;
    if (key != 0) {
        delete key;
        e->keyFile->Release();
    }
    if (mKeyFile != 0) {
        unsigned int v = param1;
        if (v >= DOOR_FIRST_KEY_MODEL && v <= DOOR_LAST_KEY_MODEL)
            UnloadKeyModels(v - 7);
        ((SharedFilePtr *)(mKeyFile))->Release();
    }
    return 1;
}

/* vtable slot 12. The ROM body is empty: the override exists only to
   occupy the slot. */
void daDoor_c::OnPendingDestroy() {}

/* vtable slot 9. Draw the door if either area it joins is showing, then
 * hand the door's bone transforms to the key model hanging off 0x138
 * (mKeyModel) and draw that too.
 *
 *   0x08c / 0x090   dActor_c::mAngle.x / mAngle.z, which a daDoor_c does NOT
 *                   use as angles: each holds a signed BYTE area id, one
 *                   per side (`ldrsh`, then sign-extended through byte
 *                   width before IsAreaShowing sees it). Only mAngle.y is a
 *                   real angle for this class.
 *   0x0e8           mModel.data.transforms, the door's own bone transform
 *                   array (dExtModel_c::data at +0x08, ModelComponents::
 *                   transforms at +0x0c).
 *
 * The two virtual calls are dExtSimpleModel_c's slot 4 Virtual10(Matrix4x3&) and slot
 * 5 Render(const Vector3*), exactly what the ROM dispatches. */
s32 daDoor_c::Render() {
    if (IsAreaShowing((char)mAngle.x) != 0 || IsAreaShowing((char)mAngle.z) != 0) {
        dExtSimpleModel_c *key;
        mModel.Render(0);
        key = mKeyModel;
        if (key != 0) {
            key->Virtual10(*mModel.data.transforms);
            mKeyModel->Render(0);
        }
    }
    return 1;
}

/* vtable slot 6. Run the current state's execute member with the player.
 *
 * THE CALLBACK IS A daDoor_c MEMBER FUNCTION, and that is measured rather
 * than assumed: spelt as `void (daDoor_c::*)(int)` and called on the
 * daDoor_c directly, mwcc emits the ROM's full member-pointer dispatch --
 * `asr #1` on the stored offset, bit 0 tested for "this is a vtable index
 * rather than a direct address", both arms present. A cheaper
 * representation would have collapsed that sequence. */
s32 daDoor_c::Behavior() {
    int res = (int)func_ov100_02145370();
    daDoor_StateRecord *node = (daDoor_StateRecord *)mState;
    if (*&node->execute != 0) {
        (this->*(node->execute))(res);
    }
    return 1;
}

/* InitResources shifts param1 down in place with a read through this
   accessor. Spelt plainly (`param1 = param1 >> 0x10;`), 2004/b56
   value-numbers both sides together and materialises the address once --
   one instruction longer than the ROM, which keeps the offset folded into
   both accesses. */
inline u32 daDoor_c::GetParam1() const {
    return param1;
}

/* vtable slot 0. Was a C source taking the object explicitly.
 *
 * Shift param1 down to the variant; offset the door by (75, 0, 0) units
 * (0x4b000, rotated by mAngle.y); load the door model; in game
 * mode 0, pick and load the key-model file (mKeyFile, see the header); start
 * the door animation (speed 0x1000 = 1.0); build the model matrix from the
 * position >> 3 and mAngle.y and give it to the door model and, when the entry
 * has a second file, a freshly built key model; then choose the starting
 * state: data_ov100_02148914 if the player stands inside |mScale.x| <= 75,
 * |mScale.y| <= 50, |mScale.z| <= 500 units of the door (mScale is the
 * player's offset, set by func_ov100_02145370), else data_ov100_021488b4.
 *
 * 0x05c/0x060/0x064 are mPos.x/.y/.z and 0x08e mAngle.y, so the opening call
 * moves the door by (75, 0, 0) units rotated by its own facing angle; the two
 * matrix stores are the same store to the door model and the key model.
 *
 * The block near the end writes mPos.x, mPos.y + 0xb4000 (180 units up) and
 * mPos.z into mSpeed. That vector is the one St_StayClosed_Main hands to
 * daPly_c::ShowMessage: a door reuses the velocity it never moves by as
 * the message position. Left spelt as dActor_c has it, because renaming
 * a base field on one derived class's evidence is a dActor_c change, not
 * a daDoor_c one. */
s32 daDoor_c::InitResources() {
    unsigned int idx;
    DoorEntry *e;
    void *f;
    void *an;
    Vector3 scaledPos;
    int r4;
    int b;
    int x;
    int y;
    int z;

    /* The spawn word arrives packed: this door's variant index is param1's
       high halfword, and every later read of param1 is that index. */
    param1 = GetParam1() >> 0x10;

    /* 75 units along the door's facing. */
    static Vector3 sDoorOff(0x4b000, 0, 0);

    Vec3_RotateYAndTranslate(&mPos, &mPos, mAngle.y, &sDoorOff);

    idx = param1;
    e = &data_ov100_02148204[idx];
    f = dExtSimpleModel_c::LoadFile(*e->file);
    mModel.SetFile((BMD_File *)f, 1, 1);

    /* Only in game mode 0: pick the key-model file (mKeyFile) and load it. */
    b = gGameMode;
    b = b == 0;
    if (b != 0) {
        if (e->starsNeeded > 0) {
            mKeyFile = &gPowerStarModelFile;
        } else if (e->keyIndex >= 0) {
            unsigned int t = param1;
            if (t >= DOOR_FIRST_KEY_MODEL && t <= DOOR_LAST_KEY_MODEL) {
                mKeyModelIdx = (t - 8);
                LoadKeyModels(mKeyModelIdx + 1);
                mKeyFile = gKeyModelFiles[mKeyModelIdx + 1];
                if (param1 == 0xc)
                    mKeyModelIdx = 0;
            } else {
                mKeyFile = &gKoopaKeyModelFile;
            }
        }
        if (mKeyFile != 0) {
            SharedFilePtr *keyFile = (SharedFilePtr *)mKeyFile;
            dExtSimpleModel_c::LoadFile(*keyFile);
        }
    }

    an = dExtFrameCtrl_c::LoadFile(gDoorOpenAnimationFile);
    mModel.SetAnim((BCA_File *)an, 0x40000000, 0x1000, 0);

    /* The model matrix takes the position >> 3. */
    Vec3_Asr(&scaledPos, &mPos, 3);
    MtxStack::transS(scaledPos.x, scaledPos.y, scaledPos.z);
    MtxStack::YrotM(mAngle.y);
    mModel.mat4x3 = gScratchMtx;

    if (e->keyFile != 0) {
        mKeyModel = new dExtSimpleModel_c;
        f = dExtSimpleModel_c::LoadFile(*e->keyFile);
        mKeyModel->SetFile((BMD_File *)f, 1, -1);
        mKeyModel->mat4x3 = gScratchMtx;
    }

    {
        int w;
        y = mPos.y;
        z = mPos.z;
        x = mPos.x;
        w = y + 0xb4000; /* 180 units up */
        mSpeed.x = x;
        mSpeed.y = w;
        {
            unsigned char bi = gLocalPlayerId;
            mSpeed.z = z;
            r4 = (int)gPlayers[bi];
        }
    }
    func_ov100_02145370();

    if (Abs(mScale.x) <= 0x4b000 && Abs(mScale.y) <= 0x32000 && Abs(mScale.z) <= 0x1f4000)
        SetState(&data_ov100_02148914, r4);
    else
        SetState(&data_ov100_021488b4, r4);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- daDoor_c_classInit, 0x0214589c, size 0x38 */
/* -------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves daDoor_c through RTTI,
 * allocation size, vtable identity, and the DOOR registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Door_Spawn. */
daDoor_c *daDoor_c_classInit() {
    return new daDoor_c;
}
