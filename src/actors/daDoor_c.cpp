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
 * that func_ov100_021453d8 stores and calls the enter member of, and Behavior
 * calls the execute member of every frame with the player
 * func_ov100_02145370 returns. The nine states are the 16-byte tables
 * data_ov100_021488a4 .. data_ov100_02148924, which the module's static
 * initializer fills from the pointer-to-member constants at
 * 0x021480d4..0x02148144; every helper from func_ov100_02144468 to
 * func_ov100_02144cf8 is one of those members, the rest are what they call.
 * Which members each table holds (enter / execute), read off that
 * initializer:
 *
 *   021488a4  none      / func_ov100_02144468   the exit sequence of
 *                                               func_ov100_02145080
 *   021488b4  none      / func_ov100_02144cf8   the closed door
 *   021488c4  02144ccc  / func_ov100_02144c6c   a message was shown
 *   021488d4  02144c64  / func_ov100_02144bf4   star door: TryTalkToDoor took
 *   021488e4  02144a38  / func_ov100_021449c8   key door: TryTalkToKeyDoor took
 *   021488f4  02144950  / func_ov100_02144730   the swing
 *   02148904  021446f8  / func_ov100_02144528   animation, character switch,
 *                                               then on into the swing
 *   02148914  0214491c  / func_ov100_02144730   the swing, starting 0x18
 *                                               frames from its end
 *   02148924  none      / func_ov100_021444e8   wait for the countdown, then
 *                                               back to the closed door
 * ("none" is the null pointer-to-member the initializer copies from
 * 0x02086b58.)
 *
 * This file is the whole linker unit 0x021443f4..0x021458d4, 32 functions:
 * D1 and D0 (daWanwan2_c_classInit, src/actors/daWanwan2_c.cpp, ends exactly at
 * 0x021443f4 below them), the 24 helpers func_ov100_02144468 through
 * func_ov100_021453d8, CleanupResources, OnPendingDestroy, Render,
 * Behavior, InitResources, and last the registry factory daDoor_c_classInit
 * (0x0214589c); daStarGate_c's first function
 * (src/game/actors/d_a_star_gate.cpp) starts exactly at 0x021458d4 above
 * it. The out-of-line destructor is the key function, so this TU also
 * emits the vtable and the RTTI.
 *
 * It replaces the one-function sources for _ZN8daDoor_cD1Ev,
 * _ZN8daDoor_cD0Ev, func_ov100_02144468 .. func_ov100_021453d8,
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
 * - the member names keep their addresses (nothing here says what the
 *   original called them), and func_ov100_02144f84 / func_ov100_02144fcc /
 *   func_ov100_02145014 / func_ov100_02145070 stay free -- the shared
 *   countdown helpers, which Player.cpp, daMugenBGM_c and d_a_star_gate.cpp
 *   also call and which take no door.
 * - the callees with Fix12<int> parameters (Sound, ModelAnim::SetAnim),
 *   the SaveData, Sound::PlayCharVoice and dExtFrameCtrl_c::LoadFile calls, and the
 *   Model loaders InitResources calls stay spelled as mangled extern-C free
 *   functions.
 * - dActor_c's mScaleX/Y/Z and mAngleX/Z carry other things for a door
 *   (the player's position in the door's frame, and two area ids), and
 *   unk_0a4/mVertSpeed/unk_0ac hold the message position; the base names
 *   cannot change on one derived class's evidence, so the functions that read
 *   them describe them in their own comments.
 * - the sound ids other than the two swing pairs, the message ids, camera
 *   flag bits 0xc00 (not in include/dCamera_c.h) and SaveData flags2 bit 16 stay
 *   numbers; their meanings are not recovered here.
 * - the static vectors guarded by data_ov100_0214870c ..
 *   data_ov100_02148720 have the shape of function-local statics (guard bit,
 *   construct, register Vector3's destructor through __register_global_object); they
 *   stay spelled out against the cartridge's own objects.
 * - the state tables and the helpers that take one are declared
 *   daDoor_StateRecord; mState stays void * because the nine tables are
 *   external data include/decl_common.h already declares as int for another
 *   TU (data_ov100_021488b4) and only their addresses are used.
 */

#pragma defer_codegen off

#include "daDoor_c.h"
#include "Player.h"
#include "types.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "dCamera_c.h"
#include "daObjKey_c.h"

/* Three plain words: the stack and static vectors of the helpers that were
   C, which carry none of Vector3's empty destructor. */
struct Vec3i { s32 x, y, z; };

/* The door's matrix, as twelve words: a copy through the C++ Matrix4x3 is
   the struct-copy DIFF of notes/cpp-class-form.md. */
struct M48 { int w[12]; };

/* One entry of data_ov100_02148204, indexed by param1 (the variant). */
struct DoorEntry {
    SharedFilePtr *file;     /* 0x0: the door model */
    SharedFilePtr *keyFile;  /* 0x4: a decoration model hung on the door, or
                                null (see mKeyModel) */
    s8  starsNeeded;         /* 0x8: >= 0: a star door, the star count it needs */
    s8  keyIndex;            /* 0x9: >= 0: a key door, the key's index */
    s8  flagShift;           /* 0xa: the door's bit position in the save flags
                                (DoorSaveFlag below); -1 for the variants that
                                carry no star or key */
    s8  pad_b;
    s16 msg;                 /* 0xc: the message shown when it stays shut */
    s16 pad_e;
};

/* Variants of the door (param1 once InitResources has shifted it down). */
enum DoorVariant {
    DOOR_FIRST_KEY_MODEL = 9,   /* 9..0xd: InitResources loads key models for
                                   these, and mKeyModelIdx is param1 - 8
                                   (zeroed again for 0xc) */
    DOOR_LAST_KEY_MODEL = 0xd,  /* the upper bound of that range; also the one
                                   variant that skips the character switch and
                                   uses TryExitWhiteDoorWithStar */
    DOOR_ALT_SOUND = 0x10       /* plays the _ALT swing sounds below */
};

/* Sound ids the swing plays through func_02012694 (the sound is placed at the
   door's camera-space position). Named by when they play; what the
   cartridge called them is not recovered. */
enum DoorSound {
    SND_SWING_START = 4,        /* func_ov100_02144950: the swing is starting */
    SND_SWING_END = 5,          /* the door animation has finished (also in
                                   func_ov100_02144730, as the area changes) */
    SND_SWING_START_ALT = 6,    /* the same two for DOOR_ALT_SOUND */
    SND_SWING_END_ALT = 7
};

/* Bits of the save flags (data_0209caa0.flags1) the door uses. The shift is
   DoorEntry::flagShift, except for the key bit, which is shifted by
   DoorEntry::keyIndex. */
enum DoorSaveFlag {
    DOOR_SAVE_HAVE_KEY = 2,          /* << keyIndex: with it set the door offers
                                        TryTalkToKeyDoor; read here as "the
                                        player holds that key" */
    DOOR_SAVE_KEY_UNLOCKED = 0x100,  /* << flagShift: set by the key door's
                                        execute state; func_ov100_02144cf8
                                        then lets the player straight through */
    DOOR_SAVE_STAR_UNLOCKED = 0x8000 /* << flagShift: set by the star door's
                                        execute state, with the same effect */
};

/* dCamera_c::mFlags bits the door sets when a swing starts (func_ov100_02144950)
   and clears near and at its end. Not documented in include/dCamera_c.h. */
#define DOOR_CAMERA_FLAGS 0xc00

/* ov089's OBJ_KEY (symbols/actor_debug_names.tsv: 282), which a key door
   spawns beside the player. */
enum { ACTOR_OBJ_KEY = 0x11a };

/* data_0209caa0, the save data: the head of include/SaveData.h's SaveData, as
   far as the door reads it. */
struct GlobCaa0 {
    int magic8000;              /* 0x000 */
    unsigned int flags1;        /* 0x004 */
    unsigned int flags2;        /* 0x008 */
    u8  pad_00c[0x35];
    u8  mCharacter;             /* 0x041 -- current character */
};

/* One state: the 16-byte {enter, execute} pointer-to-member pair
   data_ov100_021488a4 .. data_ov100_02148924 are built from. The player is
   handed over as an int. */
typedef int (daDoor_c::*daDoor_StatePmf)(int);
struct daDoor_StateRecord {
    daDoor_StatePmf enter;      /* 0x0 -- called by func_ov100_021453d8 */
    daDoor_StatePmf execute;    /* 0x8 -- called by Behavior every frame */
};

struct Vector3_16;

extern "C" {
/* Defined below, called before their definitions. These four are the shared
   countdown helpers -- free functions, not members: Player.cpp, daMugenBGM_c
   and d_a_star_gate.cpp call them too, and none takes the door. */
int func_ov100_02144f84(void);
int func_ov100_02144fcc(void);
int func_ov100_02145014(void);
void func_ov100_02145070(int v);

void _ZN8SaveData17SetCharacterIntroEi(int);
int _ZN8SaveData22NumGlowingRabbitsFoundEv(void);
void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int b);
int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int, unsigned int, unsigned int, int, int);
void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void *pos);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
void *_ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(void *fp);
/* local extern: include/Player.h declares Player::Unk_020ca488 void, and
   func_ov100_021449c8 tests the value it leaves in r0. */
int _ZN6Player12Unk_020ca488Ev(void *p);
void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    u32 actorID, u32 param1, const Vector3 *pos, const Vector3_16 *rot,
    s32 areaID, s32 deathTableID);
void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int a, int fix, unsigned int j);
void *_ZN5ModelC1Ev(void *self);
void *_Znwj(unsigned int sz);
/* func_0201277c(id) and func_02012790(id) play a sound id with no position
   (daJango_c.cpp says the same of func_02012790); func_02012694(id, pos)
   plays it at a position. */
void func_0201277c(int a);
void func_02012790(int a);
int func_02012694(int a, void *b);
unsigned char DecIfAbove0_Byte(void *p);
int DecIfAbove0_Short(void *p);
int func_ov002_020ca78c(void *p);
/* arm9:0x020731dc -- mwcc's runtime name for the atexit-style static-object
   destructor registrar. The synthesized static initializer calls it under
   this name too, so spelling it here keeps one shared undefined symbol. */
void __register_global_object(void *object, void *destructor, void *node);
void Vec3_RotateYAndTranslate(void *res, void *translation, short angY, void *v);
void Vec3_Sub(void *out, void *a, void *b);
void Vec3_Asr(void *d, void *s, int sh);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, short angY);
int AngleDiff(int a, int b);
unsigned char NumStars(void);
void ChangeArea(int areaID);
void ShowArea(int idx);
unsigned char IsAreaShowing(int idx);
void SetTouchScreenDelay(void);
void LoadKeyModels(int idx);
void UnloadKeyModels(int idx);
void *_ZN7Vector3D1Ev(void *object);

extern DoorEntry data_ov100_02148204[];
/* Indexed by mKeyModelIdx - 1: the character func_ov100_02144528 switches the
   player to (the first four bytes are 00 01 02 00). */
extern unsigned char data_ov100_021480d0[];
/* Latch: set (func_ov100_02144a38) when the door being opened has a nonzero
   mKeyModelIdx, cleared by func_ov100_02144468; the swing and exit states
   branch on it. */
extern unsigned char data_ov100_02148704;
/* The shared countdown: ticked by func_ov100_02145014, started by
   func_ov100_02145070. */
extern unsigned short data_ov100_02148708[];
extern unsigned int data_ov100_0214870c;
extern int data_ov100_02148710;
extern unsigned int data_ov100_02148714;
extern int data_ov100_02148718;
extern int data_ov100_0214871c;
extern int data_ov100_02148720;
/* The pointer-to-member records the nine states are built from
   (0x021480d4..0x02148144 in .data), and the null member the initializer
   copies for the states that have no enter member. */
extern daDoor_StatePmf data_02086b58;
extern daDoor_StatePmf data_ov100_021480d4;
extern daDoor_StatePmf data_ov100_021480dc;
extern daDoor_StatePmf data_ov100_021480e4;
extern daDoor_StatePmf data_ov100_021480ec;
extern daDoor_StatePmf data_ov100_021480f4;
extern daDoor_StatePmf data_ov100_021480fc;
extern daDoor_StatePmf data_ov100_02148104;
extern daDoor_StatePmf data_ov100_0214810c;
extern daDoor_StatePmf data_ov100_02148114;
extern daDoor_StatePmf data_ov100_0214811c;
extern daDoor_StatePmf data_ov100_02148124;
extern daDoor_StatePmf data_ov100_0214812c;
extern daDoor_StatePmf data_ov100_02148134;
extern daDoor_StatePmf data_ov100_0214813c;
extern daDoor_StatePmf data_ov100_02148144;

extern GlobCaa0 data_0209caa0;
/* The camera, and the player array indexed by the current player id. */
extern char *data_0209f318;
extern unsigned char data_0209f250;
extern char *data_0209f394[];
/* The current game mode. */
extern unsigned char data_0209f2d8;
/* The next-level id (signed byte). */
extern char data_02092110;
/* Music volume and message-sound volume, both in Fix12 (0x7f000 = 127.0). */
extern int data_0209b490[];
extern int data_0209b49c[];
extern int data_020a0ebc;
extern M48 data_020a0e68;
extern int data_ov002_0211094c;
extern int data_ov089_02132c50;
/* ov089's key-model file table, not a function: eight SharedFilePtr*
   entries in that overlay's .rodata, the table LoadKeyModels loads into
   (bounds-checked `idx >= 8`). src/actors/daObjKey_c.cpp, home of
   LoadKeyModels and UnloadKeyModels, declares it under this name with this
   element type. */
extern void *data_ov089_02132894[];
}

/* The door's file-scope SharedFilePtr objects, spelled through the same
   wrapper idiom the landed folds use: the handles' code is the veneer pair
   the cartridge already carries, so the wrappers get no bodies of their
   own. The constructors alias func_02017acc (model) and
   SharedFilePtr::Construct (animation) in this unit's manifest, and the
   destructors func_02017ab4 and SharedFilePtr_Destruct_Anim. */
struct DoorModelFilePtr : SharedFilePtr {
    int unk0; void *file;
    DoorModelFilePtr(unsigned int id);
    ~DoorModelFilePtr();
};

struct DoorAnimationFileHandle : SharedFilePtr {
    int unk0; void *file;
    DoorAnimationFileHandle(unsigned int id);
    ~DoorAnimationFileHandle();
};

/* -------------------------------------------------------------------------- */
/* The eleven model handles and the animation handle, in static-initializer
   order: the initializer constructs each and registers its destructor.
   data_ov100_02148744 is the one the helpers release and hand to
   dExtFrameCtrl_c::LoadFile. */
/* -------------------------------------------------------------------------- */
// @symbol __sinit_daDoor_c.cpp
DoorModelFilePtr    data_ov100_0214875c(0x9c09);
DoorModelFilePtr    data_ov100_0214877c(0x9c0a);
DoorModelFilePtr    data_ov100_0214873c(0x43e);
DoorModelFilePtr    data_ov100_02148724(0x43f);
DoorModelFilePtr    data_ov100_0214872c(0x441);
DoorModelFilePtr    data_ov100_02148764(0x440);
DoorModelFilePtr    data_ov100_02148774(0x443);
DoorModelFilePtr    data_ov100_0214876c(0x444);
DoorModelFilePtr    data_ov100_0214874c(0x9c0b);
DoorModelFilePtr    data_ov100_02148754(0x445);
DoorModelFilePtr    data_ov100_02148734(0x446);
DoorAnimationFileHandle  data_ov100_02148744(0x9c08);

/* The function-local statics of the helpers above, spelled out against the
   cartridge's own objects: each is a {guard, vector, node} triple the
   helper guards and registers with __register_global_object itself. The helpers treat
   the vectors as Vec3i or three-integer records; only the addresses are
   touched by the book-keeping calls, so the nodes are three-void* records
   here. */
void *data_ov100_02148790[3];
Vec3i data_ov100_0214879c;
void *data_ov100_021487b4[3];
Vec3i data_ov100_021487c0;
void *data_ov100_021487cc[3];
void *data_ov100_021487d8[3];
int data_ov100_021487e4[3];
Vec3i data_ov100_021487f0;
void *data_ov100_021487fc[3];
int data_ov100_02148808[3];
void *data_ov100_02148850[3];
Vec3i data_ov100_02148880;

/* -------------------------------------------------------------------------- */
/* The nine door states, in static-initializer order. Each is a
   16-byte {enter, execute} pointer-to-member pair; the enterless states
   copy the null record arm9 keeps at data_02086b58. The .data records
   above carry the individual members. */
/* -------------------------------------------------------------------------- */
daDoor_StateRecord data_ov100_021488b4 = {data_02086b58, data_ov100_0214813c};
daDoor_StateRecord data_ov100_021488c4 = {data_ov100_021480dc, data_ov100_021480e4};
daDoor_StateRecord data_ov100_021488d4 = {data_ov100_021480f4, data_ov100_021480ec};
daDoor_StateRecord data_ov100_021488e4 = {data_ov100_0214810c, data_ov100_021480fc};
daDoor_StateRecord data_ov100_021488f4 = {data_ov100_0214811c, data_ov100_02148104};
daDoor_StateRecord data_ov100_02148904 = {data_ov100_02148124, data_ov100_0214812c};
daDoor_StateRecord data_ov100_02148914 = {data_ov100_02148144, data_ov100_021480d4};
daDoor_StateRecord data_ov100_02148924 = {data_02086b58, data_ov100_02148134};
daDoor_StateRecord data_ov100_021488a4 = {data_02086b58, data_ov100_02148114};

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- _ZN8daDoor_cD1Ev, 0x021443f4, size 0x30;
 *                         _ZN8daDoor_cD0Ev, 0x02144424, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_cD1Ev
// @symbol _ZN8daDoor_cD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 *
 * Store this class's vtable over the one the base constructor left, destroy
 * mModel, then run dActor_c's destructor; D0 adds the inline operator
 * delete. All of it follows from `struct daDoor_c : dActor_c` and the member
 * types in the header. */
daDoor_c::~daDoor_c()
{
}

#ifdef _MSC_VER
/* The host needs the ROM's flat D0 name, and MSVC never emits it: it folds
 * the Itanium destructor variants into the one ~daDoor_c() above. This arm
 * spells out what the deleting destructor does -- the D1 body, called
 * qualified so it is a direct call, then the class-specific operator
 * delete. Nothing here reaches mwccarm. */
extern "C" daDoor_c *_ZN8daDoor_cD0Ev(daDoor_c *thiz)
{
    thiz->daDoor_c::~daDoor_c();
    daDoor_c::operator delete(thiz);
    return thiz;
}
#endif

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov100_02144468, 0x02144468, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_02144468Ei
/* State 021488a4, execute member. Once func_ov100_02145080 lets the player
   out (it returns 0), clear the data_ov100_02148704 latch, call
   SaveData::SetCharacterIntro(mKeyModelIdx - 1) (or, for variant 0xd,
   Sound::ChangeMusicVolume(0x7f, 0xcb33 = 12.7 as Fix12)) and go to state
   data_ov100_021488b4. The player is taken as the int the member-pointer call
   passes. Was a C source. */
int daDoor_c::func_ov100_02144468(int p)
{
    if (!func_ov100_02145080((Player *)p)) {
        data_ov100_02148704 = 0;
        if (param1 != DOOR_LAST_KEY_MODEL) {
            _ZN8SaveData17SetCharacterIntroEi(mKeyModelIdx - 1);
        } else {
            _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0xcb33);
        }
        func_ov100_021453d8(&data_ov100_021488b4, p);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov100_021444e8, 0x021444e8, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_021444e8Ei
/* State 02148924, execute member. Go to state data_ov100_021488b4 once
   func_ov100_02144fcc reports the shared countdown done. Was a C source. */
int daDoor_c::func_ov100_021444e8(int a1)
{
    if (func_ov100_02144fcc()) {
        func_ov100_021453d8(&data_ov100_021488b4, a1);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov100_02144528, 0x02144528, size 0x1d0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_02144528EP6Player
/* State 02148904, execute member. Run the door animation; once it has
   finished, and while mTimer is zero:
     - variants other than 0xd switch the player to
       data_ov100_021480d0[mKeyModelIdx - 1] (character 3 when that is already
       the current character), Player::SetRealCharacter. With the
       data_ov100_02148704 latch set they also set the player's mHasNoCap,
       play sound 0x7e, set mTimer to 0x40 (64) and call
       Sound::ChangeMusicVolume(0, 0x19666 = 25.4 as Fix12); with it clear
       they play sound 0x7f;
     - variant 0xd sets mTimer to 0x40, plays sound 0xa1 (latch set) or 0xa2
       and calls the same ChangeMusicVolume(0, ...);
   then play the swing-end sound. Next count mTimer down: at zero, send the
   player through with func_ov100_021451c4 (given no state when the latch is
   set, which makes it skip CanEnterDoor) and, when that succeeds under the
   latch, open the big door; when it refuses, mTimer is left at 1. While it
   is still counting, variant 0xd with the latch clear calls
   ChangeMusicVolume(0x7f, 0xcb33) as mTimer reaches 0xa. Was a C source. */
int daDoor_c::func_ov100_02144528(Player *pl)
{
    mModel.Advance();
    if (mModel.Finished() != 0) {
        if (mTimer == 0) {
            if (param1 != DOOR_LAST_KEY_MODEL) {
                unsigned int ch = data_ov100_021480d0[mKeyModelIdx - 1];
                if (ch == data_0209caa0.mCharacter)
                    ch = 3;
                pl->SetRealCharacter(ch);
                if (data_ov100_02148704 != 0) {
                    pl->mHasNoCap = 1;
                    func_0201277c(0x7e);
                    mTimer = 0x40;
                    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0, 0x19666);  /* 25.4 as Fix12 */
                } else {
                    func_0201277c(0x7f);
                }
            } else {
                mTimer = 0x40;
                if (data_ov100_02148704 != 0)
                    func_02012790(0xa1);
                else
                    func_02012790(0xa2);
                _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0, 0x19666);
            }
            func_02012694(param1 == DOOR_ALT_SOUND ? SND_SWING_END_ALT : SND_SWING_END, &mCamSpacePosX);
        }
        if (DecIfAbove0_Byte(&mTimer) == 0) {
            void *r1 = data_ov100_02148704 != 0 ? 0 : (void *)&data_ov100_021488f4;
            if (func_ov100_021451c4(r1, pl) != 0) {
                if (data_ov100_02148704 != 0)
                    pl->OpenBigDoor();
            } else {
                mTimer += 1;
            }
        } else {
            if (data_ov100_02148704 == 0
                && mTimer == 0xa
                && param1 == DOOR_LAST_KEY_MODEL) {
                _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0xcb33);
            }
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov100_021446f8, 0x021446f8, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_021446f8EP6Player
/* State 02148904, enter member. Open the big door, zero mTimer and carry on
   through func_ov100_02144950. Was a C source. */
void daDoor_c::func_ov100_021446f8(Player *r1)
{
    r1->OpenBigDoor();
    mTimer = 0;
    func_ov100_02144950(r1, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov100_02144730, 0x02144730, size 0x1ec */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_02144730EP6Player
/* States 021488f4 and 02148914, execute member: the door swinging. Under the
   data_ov100_02148704 latch, once the current frame is past the animation's
   frame count minus 0x28 (40) it runs func_ov100_02145080 every frame. When
   the animation finishes it goes to state data_ov100_021488a4 (latch set) or
   data_ov100_021488b4, stores the area id on the far side (the low byte of
   mAngleX when mScaleZ is negative, else of mAngleZ -- see Render) as the
   player's mAreaId and ChangeArea's it, clears the DOOR_CAMERA_FLAGS camera
   bits and plays the swing-end sound. Before the end: with a nonzero
   mKeyModelIdx, on the frame 0x1c (28) from the end it clears the camera
   bits and plays the character voice (or, for variant 0xd with the latch
   clear, the player's Mamma Mia sound); otherwise, on the frame 0x18 (24)
   from the end, it sends the camera behind the player (dCamera_c::GoBehindPlayer
   with the current player id) and, when the next-level id is not negative,
   zeroes the animation's speed. */
int daDoor_c::func_ov100_02144730(Player *arg1)
{
    mModel.Advance();
    if (data_ov100_02148704 != 0) {
        int f = mModel.currFrame;
        if ((unsigned)(f << 4) >> 16 > (u16)(mModel.GetFrameCount() - 0x28))
            func_ov100_02145080(arg1);
    }

    if (mModel.Finished() != 0) {
        int t;
        if (data_ov100_02148704 != 0)
            func_ov100_021453d8(&data_ov100_021488a4, (int)arg1);
        else
            func_ov100_021453d8(&data_ov100_021488b4, (int)arg1);
        if (mScaleZ < 0)
            t = mAngleX;
        else
            t = mAngleZ;
        t = (s8)t;
        arg1->mAreaId = t;
        ChangeArea(t);
        {
            u32 *p = &((dCamera_c *)data_0209f318)->mFlags;
            *p &= ~DOOR_CAMERA_FLAGS;
        }
        func_02012694(param1 == DOOR_ALT_SOUND ? SND_SWING_END_ALT : SND_SWING_END, &mCamSpacePosX);
    } else if (mKeyModelIdx != 0) {
        if (mModel.WillHitFrame((u16)(mModel.GetFrameCount() - 0x1c)) != 0) {
            u32 *p = &((dCamera_c *)data_0209f318)->mFlags;
            *p &= ~DOOR_CAMERA_FLAGS;
            if (param1 != DOOR_LAST_KEY_MODEL) {
                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(data_0209caa0.mCharacter, 0x21, &mCamSpacePosX);
            } else if (data_ov100_02148704 == 0) {
                arg1->PlayMammaMiaSound();
            }
        }
    } else {
        if (mModel.WillHitFrame((u16)(mModel.GetFrameCount() - 0x18)) != 0) {
            if (*(s8 *)&data_02092110 >= 0)
                mModel.speed = 0;
            ((dCamera_c *)data_0209f318)->GoBehindPlayer(data_0209f250);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov100_0214491c, 0x0214491c, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_0214491cEv
/* State 02148914, enter member: start the door animation 0x18 (24) frames
   from its end (the current frame is 20.12, hence the << 12) and zero mTimer.
   Was a C source. */
int daDoor_c::func_ov100_0214491c()
{
    int n = mModel.GetFrameCount();
    int f = (int)((unsigned short)(n - 0x18)) << 12;
    mModel.currFrame = f;
    mTimer = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov100_02144950, 0x02144950, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_02144950EP6Playeri
/* State 021488f4, enter member (and what func_ov100_021446f8 carries on
   with): rewind the door animation, show both areas the door joins (the low
   bytes of mAngleX and mAngleZ), set the camera flag bits DOOR_CAMERA_FLAGS,
   play the swing-start sound and zero mTimer. Was a C source, which read the
   two area halfwords through an inline helper; func_ov100_021446f8 calls it
   with the player and a third argument it never reads. */
int daDoor_c::func_ov100_02144950(Player *pl, int unused)
{
    mModel.currFrame = 0;
    ShowArea((signed char)mAngleX);
    ShowArea((signed char)mAngleZ);
    ((dCamera_c *)data_0209f318)->mFlags |= DOOR_CAMERA_FLAGS;
    func_02012694((param1 == DOOR_ALT_SOUND) ? SND_SWING_START_ALT : SND_SWING_START, &mCamSpacePosX);
    mTimer = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov100_021449c8, 0x021449c8, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_021449c8EP6Player
/* State 021488e4, execute member. Once Player::Unk_020ca488 returns 0, set
   this door's DOOR_SAVE_KEY_UNLOCKED save-flag bit and go to state
   data_ov100_021488b4. */
int daDoor_c::func_ov100_021449c8(Player *a2)
{
    if (!_ZN6Player12Unk_020ca488Ev(a2)) {
        signed char sh = (((signed char *)data_ov100_02148204) + (param1 << 4))[0xa];  /* flagShift; the array-indexed spelling compiles to a different load */
        data_0209caa0.flags1 |= DOOR_SAVE_KEY_UNLOCKED << sh;
        func_ov100_021453d8(&data_ov100_021488b4, (int)a2);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov100_02144a38, 0x02144a38, size 0x1bc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_02144a38EP6Player
/* State 021488e4, enter member. Open the big door, place the player at the
   door's front or back (func_ov100_02145170, offsets (75, 0, 110) and
   (75, 0, -110) units from the door), and spawn an OBJ_KEY (param1 = the
   door's key index) at (-76, 0, 109) units from the player, rotated by the
   player's facing, then start the key's animation 2 through
   func_ov089_0213115c. A nonzero mKeyModelIdx sets the
   data_ov100_02148704 latch. Was a C source. */
int daDoor_c::func_ov100_02144a38(Player *p)
{
    Vec3i pos;
    DoorEntry *e;

    p->OpenBigDoor();

    if ((data_ov100_02148720 & 1) == 0) {
        data_ov100_02148880.x = 0x4b000;     /* 75 units */
        data_ov100_02148880.y = 0;
        data_ov100_02148880.z = 0x6e000;     /* 110 */
        __register_global_object(&data_ov100_02148880, (void *)_ZN7Vector3D1Ev, &data_ov100_02148850);
        data_ov100_02148720 |= 1;
    }

    if ((data_ov100_02148718 & 1) == 0) {
        data_ov100_0214879c.x = 0x4b000;
        data_ov100_0214879c.y = 0;
        data_ov100_0214879c.z = -0x6e000;
        __register_global_object(&data_ov100_0214879c, (void *)_ZN7Vector3D1Ev, &data_ov100_02148790);
        data_ov100_02148718 |= 1;
    }

    func_ov100_02145170(p, (Vector3 *)&data_ov100_02148880, (Vector3 *)&data_ov100_0214879c);

    if ((data_ov100_0214871c & 1) == 0) {
        data_ov100_021487f0.x = -0x4c000;    /* -76 units */
        data_ov100_021487f0.y = 0;
        data_ov100_021487f0.z = 0x6d000;     /* 109 */
        __register_global_object(&data_ov100_021487f0, (void *)_ZN7Vector3D1Ev, &data_ov100_021487d8);
        data_ov100_0214871c |= 1;
    }

    e = &data_ov100_02148204[param1];

    Vec3_RotateYAndTranslate(&pos, &p->mPosX, p->mAngleY, &data_ov100_021487f0);

    {
        void *actor = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            ACTOR_OBJ_KEY, (u32)e->keyIndex, (Vector3 *)&pos,
            (Vector3_16 *)&p->mAngleX, p->mAreaId, -1);
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
/* ROM ordinal 11 -- func_ov100_02144bf4, 0x02144bf4, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_02144bf4EP6Player
/* State 021488d4, execute member. Unless the player is opening the door
   with a star, set this door's DOOR_SAVE_STAR_UNLOCKED save-flag bit and
   send the player through (state data_ov100_021488f4). */
int daDoor_c::func_ov100_02144bf4(Player *a2)
{
    if (!a2->IsOpeningDoorWithStar()) {
        signed char sh = (((signed char *)data_ov100_02148204) + (param1 << 4))[0xa];  /* flagShift; the array-indexed spelling compiles to a different load */
        data_0209caa0.flags1 |= DOOR_SAVE_STAR_UNLOCKED << sh;
        func_ov100_021451c4(&data_ov100_021488f4, a2);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov100_02144c64, 0x02144c64, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_02144c64Ev
/* State 021488d4, enter member: does nothing. Was a C source. */
int daDoor_c::func_ov100_02144c64()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov100_02144c6c, 0x02144c6c, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_02144c6cEP6Player
/* State 021488c4, execute member. Tick the shared countdown; once
   Player::GetTalkState() returns -1 and the player has left the door's box,
   go to state data_ov100_02148924. Was a C source. */
int daDoor_c::func_ov100_02144c6c(Player *r1)
{
    func_ov100_02145014();
    if (r1->GetTalkState() == -1) {
        if (func_ov100_021452e4(r1) == 0) {
            func_ov100_021453d8(&data_ov100_02148924, (int)r1);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov100_02144ccc, 0x02144ccc, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_02144cccEv
/* State 021488c4, enter member. Play sound 0xb8 at the door and start the
   shared countdown at 0x87 (135). Was a C source. */
int daDoor_c::func_ov100_02144ccc()
{
    func_02012694(0xb8, &mCamSpacePosX);
    func_ov100_02145070(0x87);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov100_02144cf8, 0x02144cf8, size 0x28c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_02144cf8EP6Player
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
   goes to the last case. The message position is unk_0a4 (see
   InitResources). Was a C source. */
int daDoor_c::func_ov100_02144cf8(Player *b)
{
    DoorEntry *entry;

    if (func_ov100_021452e4(b)) {
        entry = &data_ov100_02148204[param1];
        if (entry->starsNeeded >= 0) {
            if (data_0209caa0.flags1 & (DOOR_SAVE_STAR_UNLOCKED << entry->flagShift)) goto L240;
            if (NumStars() < entry->starsNeeded) {
                if (func_ov100_02144f84() == 0) return 1;
                if (b->ShowMessage(*this, entry->msg, (Vector3 *)&unk_0a4, 0, 2) == 0) goto L240;
                func_ov100_021453d8(&data_ov100_021488c4, (int)b);
                return 1;
            } else {
                if (b->TryTalkToDoor(0) == 0) goto L240;
                func_ov100_021453d8(&data_ov100_021488d4, (int)b);
                return 1;
            }
        } else {
            int e9 = entry->keyIndex;
            int val;
            if (e9 < 0) goto L240;
            val = data_0209caa0.flags1;
            if (val & (DOOR_SAVE_KEY_UNLOCKED << entry->flagShift)) goto L240;
            if (val & (DOOR_SAVE_HAVE_KEY << e9)) goto L210;
            if (func_ov100_02144f84() == 0) return 1;
            {
                int msg;
                if (entry->keyIndex == 6) {
                    msg = _ZN8SaveData22NumGlowingRabbitsFoundEv() ? 0x28 : 0x23;
                } else {
                    int idx2 = param1;
                    int sel = 1;
                    int inRange = (unsigned)idx2 >= DOOR_FIRST_KEY_MODEL && (unsigned)idx2 <= 0xb;
                    if (!inRange) {
                        if (!(data_0209caa0.flags1 & 0xbe)) sel = 0;
                    }
                    msg = sel ? entry->msg : 0x17;
                }
                if (b->ShowMessage(*this, (short)msg, (Vector3 *)&unk_0a4, 0, 2) == 0) goto L240;
            }
            func_ov100_021453d8(&data_ov100_021488c4, (int)b);
            if (entry->keyIndex == 5)
                data_0209caa0.flags2 |= 0x10000;
            return 1;
        L210:
            if (b->TryTalkToKeyDoor() == 0) goto L240;
            func_ov100_021453d8(&data_ov100_021488e4, (int)b);
            return 1;
        }
    L240:
        func_ov100_021451c4((mKeyModelIdx != 0) ? (void *)&data_ov100_02148904 : (void *)&data_ov100_021488f4, b);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov100_02144f84, 0x02144f84, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov100_02144f84
/* 1 when the shared countdown is zero, the music volume data_0209b490 is at
   0x7f000 (127.0 as Fix12) and the message-sound volume data_0209b49c is
   zero. Was a C source. */
extern "C" int func_ov100_02144f84(void)
{
    if (*data_ov100_02148708 != 0) goto fail;
    if (*data_0209b490 != 0x7f000) goto fail;
    if (*data_0209b49c == 0) return 1;
fail:
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov100_02144fcc, 0x02144fcc, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov100_02144fcc
/* Tick the shared countdown (func_ov100_02145014); when it is done, call
   Sound::PlaySub(0x24, 0x7f, 0, 0x7222 = 7.13 as Fix12, 0) and return its
   result, else 0. */
extern "C" int func_ov100_02144fcc(void)
{
    int r = func_ov100_02145014();
    if (r == 0) return 0;
    return _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x24, 0x7f, 0, 0x7222, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov100_02145014, 0x02145014, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov100_02145014
/* Tick the shared countdown data_ov100_02148708 (DecIfAbove0_Short); while
   the ticked value is still nonzero, call Sound::PlaySub(0x24, 0x14, 0x7f,
   0x15666 = 21.4 as Fix12, 0). Returns 1 once the countdown is zero. Was a C
   source. */
extern "C" int func_ov100_02145014(void)
{
    int r = DecIfAbove0_Short(data_ov100_02148708);
    if (r) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x24, 0x14, 0x7f, 0x15666, 0);
    }
    return data_ov100_02148708[0] == 0 ? 1 : 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov100_02145070, 0x02145070, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov100_02145070
/* Start the shared countdown at v. Was a C source. */
extern "C" void func_ov100_02145070(int v)
{
    data_ov100_02148708[0] = v;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov100_02145080, 0x02145080, size 0xf0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_02145080EP6Player
/* The exit timing, branching on the variant. Variant 0xd stores the 0/1
   result of Player::TryExitWhiteDoorWithStar in mTimer. Every other variant
   is timed on mTimer: while it is zero it waits for func_ov002_020ca78c(player) to
   report the player ready, then plays sound 0x2b through Sound::PlaySub and
   sets mTimer to 0x78 (120); it then counts that down, plays sound 0x2b again
   and bumps mTimer to 1 on the frame the player is let out. Returns 0 on that
   frame (and, for variant 0xd, while mTimer is nonzero); 1 otherwise. Was a C
   source. */
int daDoor_c::func_ov100_02145080(Player *arg1)
{
    if (param1 != DOOR_LAST_KEY_MODEL) {
        if (mTimer == 0) {
            if (func_ov002_020ca78c(arg1) == 0)
                goto ret1;
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x2b, 0, 0x7f, 0x15666, 0);
            mTimer = 0x78;
            goto ret1;
        }
        if (DecIfAbove0_Byte(&mTimer) != 0)
            goto ret1;
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x2b, 0x7f, 0, 0x7222, 0);
        mTimer += 1;
        return 0;
    }
    if (mTimer != 0)
        goto ret0;
    mTimer = arg1->TryExitWhiteDoorWithStar();
    goto ret1;
ret0:
    return 0;
ret1:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov100_02145170, 0x02145170, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_02145170EP6PlayerP7Vector3S3_
/* Place the player at the door: offset a, facing the door's mAngleY turned
   half round, when mScaleZ > 0; offset b, facing mAngleY, otherwise -- the new facing also copied to the player's
   mPrevAngleY, and either offset rotated by mAngleY about the door's
   position. Was a C source taking the door and the player as char *. */
void daDoor_c::func_ov100_02145170(Player *pl, Vector3 *a, Vector3 *b)
{
    Vector3 *v;
    if (mScaleZ > 0) {
        pl->mAngleY = mAngleY + 0x8000;
        v = a;
    } else {
        pl->mAngleY = mAngleY;
        v = b;
    }
    pl->mPrevAngleY = pl->mAngleY;
    Vec3_RotateYAndTranslate(&pl->mPosX, &mPosX, mAngleY, v);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov100_021451c4, 0x021451c4, size 0x120 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_021451c4EPvP6Player
/* Send the player through: unless no state was given (then
   data_ov100_021488f4 is used without asking), the player must be able to
   enter from its side (Player::CanEnterDoor, told whether mScaleZ <= 0);
   place it through func_ov100_02145170 with the offsets (0, 0, 100) and
   (0, 0, -100) units, go to the given state and call SetTouchScreenDelay.
   Returns 0 when the player cannot enter. */
int daDoor_c::func_ov100_021451c4(void *r5, Player *r4)
{
    if (r5 == 0) {
        r5 = &data_ov100_021488f4;
    } else if (r4->CanEnterDoor(mScaleZ <= 0) == 0) {
        return 0;
    }
    if (!(data_ov100_0214870c & 1)) {
        data_ov100_021487e4[0] = 0;
        data_ov100_021487e4[1] = 0;
        data_ov100_021487e4[2] = 0x64000;    /* 100 units */
        __register_global_object(data_ov100_021487e4, (void *)_ZN7Vector3D1Ev, data_ov100_021487cc);
        data_ov100_0214870c |= 1;
    }
    if (!(data_ov100_02148714 & 1)) {
        data_ov100_02148808[0] = 0;
        data_ov100_02148808[1] = 0;
        data_ov100_02148808[2] = -0x64000;
        __register_global_object(data_ov100_02148808, (void *)_ZN7Vector3D1Ev, data_ov100_021487fc);
        data_ov100_02148714 |= 1;
    }
    func_ov100_02145170(r4, (Vector3 *)data_ov100_021487e4, (Vector3 *)data_ov100_02148808);
    func_ov100_021453d8(r5, (int)r4);
    SetTouchScreenDelay();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov100_021452e4, 0x021452e4, size 0x8c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_021452e4EP6Player
/* 1 when the player is inside the door's box (the player's door-relative
   offset in mScaleX/Y/Z within 75, 50 and 110 units, bounds inclusive) and
   AngleDiff between the player's mAngleY and the door's is below 0x2000 (45
   degrees) -- the door's mAngleY when mScaleZ is negative, half a turn from
   it otherwise. Was a C source. */
int daDoor_c::func_ov100_021452e4(Player *r1)
{
    int v, z, a;
    v = mScaleX; if (v < 0) v = -v; if (v > 0x4b000) goto fail;
    v = mScaleY; if (v < 0) v = -v; if (v > 0x32000) goto fail;
    z = mScaleZ;
    v = (z < 0) ? -z : z; if (v > 0x6e000) goto fail;
    if (z < 0) a = mAngleY;
    else a = mAngleY + 0x8000;
    a = (short)a;
    if (AngleDiff(a, r1->mAngleY) < 0x2000) return 1;
fail:
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov100_02145370, 0x02145370, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_02145370Ev
/* The player's position in the door's frame (the offset from the door,
   rotated by minus mAngleY), stored into the door's
   mScaleX/Y/Z, which a door does not use as a scale; returns the player
   (data_0209f394[data_0209f250], the current one). Was a C source. */
Player *daDoor_c::func_ov100_02145370()
{
    Vec3i v;
    Player *r5 = (Player *)data_0209f394[data_0209f250];
    Vec3_Sub(&v, &r5->mPosX, &mPosX);
    Vec3_RotateYAndTranslate(&mScaleX, &data_020a0ebc, (short)(-mAngleY), &v);
    return r5;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov100_021453d8, 0x021453d8, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c19func_ov100_021453d8EPvi
/* Enter a state: store the address of the state table at +0x140 (mState) and
   call its enter member with the player, if it has one. */
int daDoor_c::func_ov100_021453d8(void *p, int a2)
{
    mState = (daDoor_StateRecord *)p;
    daDoor_StateRecord *q = (daDoor_StateRecord *)mState;
    if (*(int *)q == 0) return 1;
    return (this->*q->enter)(a2);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- _ZN8daDoor_c16CleanupResourcesEv, 0x0214542c, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c16CleanupResourcesEv
/* vtable slot 3. `delete key` IS the ROM's `ldr r1,[r0]; ldr r1,[r1,#4];
 * blx r1` -- vtable slot 1, Model's deleting destructor -- and it is also
 * where the doubled null check comes from: the explicit `if` emits one and
 * `delete` emits its own. */
s32 daDoor_c::CleanupResources()
{
    DoorEntry *e = &data_ov100_02148204[param1];
    Model *key;
    e->file->Release();
    data_ov100_02148744.Release();
    key = mKeyModel;
    if (key != 0) {
        delete key;
        e->keyFile->Release();
    }
    if (mKeyFile != 0) {
        unsigned int v = param1;
        if (v >= DOOR_FIRST_KEY_MODEL && v <= DOOR_LAST_KEY_MODEL) UnloadKeyModels(v - 7);
        ((SharedFilePtr *)(mKeyFile))->Release();
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- _ZN8daDoor_c16OnPendingDestroyEv, 0x021454c4, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c16OnPendingDestroyEv
/* vtable slot 12. The ROM body is empty: the override exists only to
   occupy the slot. */
void daDoor_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- _ZN8daDoor_c6RenderEv, 0x021454c8, size 0x88 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c6RenderEv
/* vtable slot 9. Draw the door if either area it joins is showing, then
 * hand the door's bone transforms to the key model hanging off 0x138
 * (mKeyModel) and draw that too.
 *
 *   0x08c / 0x090   dActor_c::mAngleX / mAngleZ, which a daDoor_c does NOT
 *                   use as angles: each holds a signed BYTE area id, one
 *                   per side (`ldrsh`, then sign-extended through byte
 *                   width before IsAreaShowing sees it). Only mAngleY is a
 *                   real angle for this class.
 *   0x0e8           mModel.data.transforms, the door's own bone transform
 *                   array (ModelBase::data at +0x08, ModelComponents::
 *                   transforms at +0x0c).
 *
 * The two virtual calls are Model's slot 4 Virtual10(Matrix4x3&) and slot
 * 5 Render(const Vector3*), exactly what the ROM dispatches. */
s32 daDoor_c::Render()
{
    if (IsAreaShowing((char)mAngleX) == 0) {
        if (IsAreaShowing((char)mAngleZ) == 0) goto done;
    }
    {
        /* &mModel is taken HERE and not at the top of the function: hoisting
           it costs a callee-saved register and a stack adjust the ROM does
           not have. The ROM computes it lazily, so this does too. */
        ModelAnim *body = &mModel;
        Model *key;
        body->Render(0);
        key = mKeyModel;
        if (key == 0) goto done;
        key->Virtual10(*mModel.data.transforms);
        mKeyModel->Render(0);
    }
done:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- _ZN8daDoor_c8BehaviorEv, 0x02145550, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c8BehaviorEv
/* vtable slot 6. Run the current state's execute member with the player.
 *
 * THE CALLBACK IS A daDoor_c MEMBER FUNCTION, and that is measured rather
 * than assumed: spelt as `void (daDoor_c::*)(int)` and called on the
 * daDoor_c directly, mwcc emits the ROM's full member-pointer dispatch --
 * `asr #1` on the stored offset, bit 0 tested for "this is a vtable index
 * rather than a direct address", both arms present. A cheaper
 * representation would have collapsed that sequence. */
s32 daDoor_c::Behavior()
{
    int res = (int)func_ov100_02145370();
    daDoor_StateRecord *node = (daDoor_StateRecord *)mState;
    if (*(int *)&node->execute != 0) {
        (this->*(node->execute))(res);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- _ZN8daDoor_c13InitResourcesEv, 0x021455a0, size 0x2fc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daDoor_c13InitResourcesEv
/* vtable slot 0. Was a C source taking the object explicitly.
 *
 * Shift param1 down to the variant; offset the door by (75, 0, 0) units
 * (0x4b000, rotated by mAngleY); load the door model; in game
 * mode 0, pick and load the key-model file (mKeyFile, see the header); start
 * the door animation (speed 0x1000 = 1.0); build the model matrix from the
 * position >> 3 and mAngleY and give it to the door model and, when the entry
 * has a second file, a freshly built key model; then choose the starting
 * state: data_ov100_02148914 if the player stands inside |mScaleX| <= 75,
 * |mScaleY| <= 50, |mScaleZ| <= 500 units of the door (mScaleX/Y/Z are the
 * player's offset, set by func_ov100_02145370), else data_ov100_021488b4.
 *
 * 0x05c/0x060/0x064 are mPosX/Y/Z and 0x08e mAngleY, so the opening call
 * moves the door by (75, 0, 0) units rotated by its own facing angle; the two
 * matrix stores are the same store to the door model and the key model.
 *
 * The block near the end writes mPosX, mPosY + 0xb4000 (180 units up) and
 * mPosZ into 0x0a4/0x0a8/0x0ac. That triple is the Vector3 func_ov100_02144cf8
 * hands to Player::ShowMessage. dActor_c.h names the middle of those three
 * `mVertSpeed`, which cannot be what a daDoor_c is storing there -- three
 * consecutive words taking a position triple say 0x0a4..0x0af is a second
 * Vector3 for at least this class. Left spelt as dActor_c has it, because
 * renaming a base field on one derived class's evidence is a dActor_c
 * change, not a daDoor_c one. */
s32 daDoor_c::InitResources()
{
    unsigned int idx;
    DoorEntry *e;
    void *f;
    void *an;
    void *m;
    Vec3i tmp;
    int r4;
    int v;
    int b;
    int x;
    int y;
    int z;

    /* The spawn word arrives packed: this door's variant index is param1's
       high halfword, and every later read of param1 is that index. Spelt
       plainly (`param1 = param1 >> 0x10;`), 2004/b56 value-numbers both
       sides together and materialises the address once -- one instruction
       longer than the ROM, which keeps the offset folded into both
       accesses. The redundant cast on the read side reaches the folded
       form. */
    param1 = (u32)param1 >> 0x10;

    if (!(data_ov100_02148710 & 1)) {
        data_ov100_021487c0.x = 0x4b000;     /* 75 units */
        data_ov100_021487c0.y = 0;
        data_ov100_021487c0.z = 0;
        __register_global_object(&data_ov100_021487c0, (void *)_ZN7Vector3D1Ev, &data_ov100_021487b4);
        data_ov100_02148710 |= 1;
    }

    Vec3_RotateYAndTranslate(&mPosX, &mPosX, mAngleY, &data_ov100_021487c0);

    idx = param1;
    e = &data_ov100_02148204[idx];
    f = _ZN5Model8LoadFileER13SharedFilePtr(e->file);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, f, 1, 1);

    /* Only in game mode 0: pick the key-model file (mKeyFile) and load it. */
    b = data_0209f2d8;
    b = b == 0;
    if (b != 0) {
        if (e->starsNeeded > 0) {
            mKeyFile = &data_ov002_0211094c;
        } else if (e->keyIndex >= 0) {
            unsigned int t = param1;
            if (t >= DOOR_FIRST_KEY_MODEL && t <= DOOR_LAST_KEY_MODEL) {
                mKeyModelIdx = (signed char)(t - 8);
                LoadKeyModels(mKeyModelIdx + 1);
                mKeyFile = data_ov089_02132894[mKeyModelIdx + 1];
                if (param1 == 0xc)
                    mKeyModelIdx = 0;
            } else {
                mKeyFile = &data_ov089_02132c50;
            }
        }
        if (mKeyFile != 0)
            _ZN5Model8LoadFileER13SharedFilePtr(mKeyFile);
    }

    an = _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(&data_ov100_02148744);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModel, an, 0x40000000, 0x1000, 0);

    /* The model matrix takes the position >> 3. */
    Vec3_Asr(&tmp, &mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, tmp.x, tmp.y, tmp.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    *(M48 *)&mModel.mat4x3 = data_020a0e68;

    if (e->keyFile != 0) {
        m = _Znwj(0x50);                     /* sizeof(Model) */
        if (m != 0)
            m = _ZN5ModelC1Ev(m);
        mKeyModel = (Model *)m;
        f = _ZN5Model8LoadFileER13SharedFilePtr(e->keyFile);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(mKeyModel, f, 1, -1);
        *(M48 *)&mKeyModel->mat4x3 = data_020a0e68;
    }

    {
        int w;
        y = mPosY;
        z = mPosZ;
        x = mPosX;
        w = y + 0xb4000;                     /* 180 units up */
        unk_0a4 = x;
        mVertSpeed = w;
        {
            unsigned char bi = data_0209f250;
            unk_0ac = z;
            r4 = (int)data_0209f394[bi];
        }
    }
    func_ov100_02145370();

    v = mScaleX;
    if (v < 0)
        v = -v;
    if (v > 0x4b000)                         /* 75 units */
        goto big;
    v = mScaleY;
    if (v < 0)
        v = -v;
    if (v > 0x32000)                         /* 50 */
        goto big;
    v = mScaleZ;
    if (v < 0)
        v = -v;
    if (v > 0x1f4000)                        /* 500 */
        goto big;

    func_ov100_021453d8(&data_ov100_02148914, r4);
    goto done;
big:
    func_ov100_021453d8(&data_ov100_021488b4, r4);
done:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- daDoor_c_classInit, 0x0214589c, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol daDoor_c_classInit
/* Reconstructed source-style name: SM64DS proves daDoor_c through RTTI,
 * allocation size, vtable identity, and the DOOR registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Door_Spawn. */
extern "C" daDoor_c *daDoor_c_classInit()
{
    return new daDoor_c;
}
