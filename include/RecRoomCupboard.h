/* RecRoomCupboard -- the Rec Room's minigame cupboard: the object you talk to
 * to open the minigame menu, and the one that puts up the "saving" banner.
 *
 * ov058, _ZTV15RecRoomCupboard at 0x02111a70.
 *
 * SIZE AND BASE come from the factory. RecRoomCupboard_Spawn (0x02111688)
 * allocates fBase_c::operator new(0x21c), calls _ZN8dActor_cC2Ev on it and then
 * stores _ZTV15RecRoomCupboard -- so the object is 0x21c bytes and the base is
 * dActor_c (0xd0). Spawn also runs func_020733a8 over `this + 0xd4` as
 * 5 x 0x40 with _ZN10dCcAcPos_cC1Ev / _ZN10dCcAcPos_cD1Ev, which is the
 * array-construct helper; ~RecRoomCupboard runs the matching
 * __destroy_arr(this + 0xd4, 5, 0x40, _ZN10dCcAcPos_cD1Ev). D1 and not D2, so
 * these are five dCcAcPos_c MEMBERS, not an inlined base.
 *
 * VTABLE, 31 slots, read out of ov058 at 0x02111a70 and diffed against
 * _ZTV8dActor_c: only three slots differ from the base table, plus the
 * destructor pair.
 *
 *   slot  0  0x0211161c  InitResources
 *   slot  3  0x02111234  CleanupResources
 *   slot  6  0x0211123c  Behavior
 *   slot 16  0x021111a0  ~RecRoomCupboard (D1)
 *   slot 17  0x021111e0  ~RecRoomCupboard (D0)
 *
 * Every other slot is dActor_c's or fBase_c's own entry unchanged; in
 * particular slot 9 is fBase_c::Render and slot 12 fBase_c::OnPendingDestroy,
 * so neither is redeclared here -- declaring them would invent overrides the
 * ROM does not have.
 *
 * LAYOUT. dActor_c ends at 0xd0 and the collider array starts at 0xd4, so four
 * bytes at 0xd0 are unaccounted for by anything that reads them; they stay a
 * pad. 0xd4 + 5*0x40 = 0x214, and Behavior reaches exactly three things past
 * that: a word at 0x214, a byte at 0x218 and a halfword at 0x21a, which fills
 * the object to its 0x21c.
 *
 * The five colliders are reached two ways in Behavior and both agree with
 * include/dCc_c.h's field list: `self + 0xf4 + i*0x40` is mColliders[i].hitFlags
 * (+0x20) tested against 0x8000000, `self + 0xf8 + i*0x40` is
 * mColliders[i].otherOwner (+0x24) fed to dActor_c::FindWithID, and
 * `self + 0x108/0x10c/0x110 + i*0x40` is mColliders[i].pos (+0x34), which
 * Behavior rewrites every frame before dCc_c::Clear/Update.
 *
 * Field NAMES cannot change codegen. mTalkingPlayer and mMessageID are spelled
 * as include/daObjHatenaSwitch_c.h, include/Rabbit.h and include/Toad.h already
 * spell the same two roles; none of the three shadows a name in fBase_c,
 * dBase_c or dActor_c.
 */
#ifndef RECROOMCUPBOARD_H
#define RECROOMCUPBOARD_H
#include "types.h"
#include "dActor_c.h"
#include "dCcAcPos_c.h"

struct RecRoomCupboard : dActor_c {
    u8  pad_0d0[0x4];
    /* Five attached cylinders, laid out left to right in front of the cupboard:
       Behavior places mColliders[j] at 100 * (2 - j) units along the actor's
       facing normal, so they span two units either side of it. Named by the
       _ZN10dCcAcPos_cC1Ev / _ZN10dCcAcPos_cD1Ev pair the factory and the
       destructor run over the array -- relocations the ROM build checks. */
    dCcAcPos_c mColliders[5];  /* 0x0d4 */
    /* The Player currently being talked to, or null. Behavior only ever fills
       it from dActor_c::FindWithID(mColliders[i].otherOwner) after checking that
       actor's actorID is 0xbf, and only when Player::StartTalk accepts; it
       clears it again when the talk ends. */
    Player *mTalkingPlayer;    /* 0x214 */
    /* Latch so the "menu" / "saving" jingle plays once per talk rather than
       once per frame: set the frame func_02012790 is called, cleared with
       mTalkingPlayer. */
    u8  mSoundStarted;         /* 0x218 */
    u8  pad_219[0x1];
    /* Seeded 0xb09 when the talk starts and re-checked before every branch of
       the talk state machine, so a message put up by something else does not
       drive this actor's menu. */
    u16 mMessageID;            /* 0x21a */

    /* --- vtable. Only the slots the ROM table actually overrides. --- */
    virtual ~RecRoomCupboard();        /* slots 16 (D1), 17 (D0) */
    virtual s32 InitResources();       /* slot  0 */
    virtual s32 CleanupResources();    /* slot  3 */
    virtual s32 Behavior();            /* slot  6 */
};

typedef char RecRoomCupboard_size_must_be_0x21c[sizeof(RecRoomCupboard) == 0x21c ? 1 : -1];

#endif
