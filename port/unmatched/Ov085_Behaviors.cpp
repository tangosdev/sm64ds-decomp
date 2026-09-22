/* HOST REIMPLEMENTATIONS of ov085 slot 6 for WALL_SIGN (183) and TOAD (185).
 *
 * These two are not host COPIES of a matched source the way the rest of
 * port/unmatched is. Neither body is decompiled at all: src/ has no
 * _ZN8WallSign8BehaviorEv and no _ZN4Toad8BehaviorEv, and neither address
 * appears in config/arm9/overlays/ov085/delinks.txt as the start of a TU. Both
 * are queued as decomp crack targets (run link60's worklist names them:
 * ov085 0x0212eea4, 0x30c bytes, and ov085 0x02129878, 0x204 bytes), and when
 * the matched bodies land these two functions come straight out and the slice
 * takes the src TUs instead. Nothing else in the seat has to change: the
 * registry rows, the vtable arrays and the fills already spell slot 6 by the
 * ROM's own name.
 *
 * ============================================================================
 * WHERE THE BYTES CAME FROM, and why not from the dsd export
 * ============================================================================
 *
 * Every instruction below was read out of extracted/overlays/overlay_0085.bin
 * at load base 0x02129020, and every call target out of
 * config/arm9/overlays/ov085/relocs.txt. NOT out of
 * extracted/dsd/arm9_overlays/ov085.bin: that export is STALE against the
 * config's re-addressing, exactly as port/tools/vtspan.py's header warns, and
 * it is stale in the one way that would quietly produce a wrong body here.
 *
 * THE STALENESS IS NOT CONFINED TO BRANCHES, and reading it that way is the
 * dangerous half-truth. Measured word by word over the two images: 1314 words
 * differ, of which only 636 are B/BL encodings. The other 678 are relocated
 * DATA literals, and 291 of the 1314 differ by more than their low halfword.
 * 0x0212905c is one of them, and it is Toad's own vtable pointer. So the
 * export is unsafe for a data read exactly as much as for a code read.
 *
 * relocs.txt covers 1255 of the 1314 differing words (636 arm_call, 619 load).
 * THE RAW OVERLAY AGREES WITH IT ON 1255 OF 1255. THE EXPORT AGREES ON 0.
 * Three worked examples, dsd export first, raw ROM second, relocs.txt third:
 *
 *   0x0212ef94   -> 0x020c6490    -> 0x020c524c    to:0x020c524c  (GetTalkState)
 *   0x0212efd4   -> 0x02044c10    -> 0x0203cf40    to:0x0203cf40  (Vec3_HorzDist)
 *   0x0212f008   -> 0x02042ac0    -> 0x0203adec    to:0x0203adec  (ApproachLinear)
 *
 * WallSign::Behavior carries 15 arm_calls (16 relocs with its literal load)
 * and Toad::Behavior 16 (17 with its own), and the raw image agrees with
 * relocs.txt on every one. A body written from the export would have called
 * SaveData::ReadDataFromCart where the ROM calls ApproachLinear.
 *
 * The two literal pools resolve to one datum each and both are hosted:
 *   0x0212f1a8 = 0x02082214  the {sin, cos} pair table (romdata.c), indexed
 *                            angle>>4, the src/Matrix4x3_FromRotationY idiom
 *   0x0212f1ac = 0x0000ffff  a plain constant, no reloc
 *   0x02129a74 = 0x0209f2f8  the level id
 *   0x02129a78 = 0x0000020d  a member offset the ROM materialised
 *
 * ============================================================================
 * THE TWO CONVENTION HAZARDS, and what each one costs if it is got wrong
 * ============================================================================
 *
 * (1) Toad::Behavior opens with a VIRTUAL dispatch of ROM slot 3 on the
 *     ModelAnim at +0x108:
 *         add r0, r8, #0x108 / ldr r1, [r0] / ldr r1, [r1, #0xc] / blx r1
 *     ROM slot 3 of _ZTV9ModelAnim is ModelAnim::UpdateVerts (0x0201686c,
 *     confirmed with port/tools/vtspan.py). The HOST _ZTV9ModelAnim numbers
 *     slot 3 as Virtual10, because MSVC spends one slot on the destructor
 *     where Itanium spends two -- the w19 collision
 *     (port/unmatched/W19_Slot5_Renders.cpp). Dispatching through the host
 *     vptr here would call Virtual10(Matrix4x3 &) with no argument. The call
 *     below is the qualified ModelAnim::UpdateVerts the ROM means.
 *
 * (2) WallSign::Behavior turns the PLAYER, not itself: three of its four
 *     ApproachLinear/Vec3_ApproachHorz calls take r4, the Player at
 *     this+0x360, as their receiver. That is the ROM's shape (the sign walks
 *     the player into reading position before the message box opens) and it
 *     is easy to "fix" into a self-turn on a casual read.
 *
 * Both bodies end the same way the ROM does: CylinderClsn::Clear then
 * CylinderClsn::Update on the collider, then return 1.
 *
 * ============================================================================
 * WHAT THE AUTOMATED PROOF DOES NOT REACH, and why that is not reassurance
 * ============================================================================
 *
 * A 300-frame FAULTS_FATAL boot on levels 2, 4, 5 and 50 runs both of these
 * bodies once per frame per instance, and every one of those frames takes the
 * SAME branch: nobody talks to a sign or a Toad, because the selftest does not
 * walk into one. So the covered half is the idle half --
 *
 *   WallSign  this+0x360 is null every frame, so only the toucher test runs
 *             (the +0x340 bit, Actor::FindWithID, the id-0xbf check and the
 *             facing test), and it never reaches Player::StartTalk.
 *   Toad      the seated state stays 0, so _ZN4Toad8RunStateEv dispatches the
 *             WAIT main every frame and the head tracking runs.
 *
 * -- and the TALK half is LINKED AND SEATED AND NEVER EXECUTED. Nothing
 * automated has ever run _ZN4Toad12St_Talk_InitEv (TALK enter) or
 * _ZN4Toad12St_Talk_MainEv (TALK main), which means the message id picker
 * (_ZN4Toad12GetMessageIDEv), the star spawn, the cap hand-back through
 * Actor::Spawn(0x10d) and Message::EndTalk are all unproven; and on the sign,
 * the three-step approach at this+0x364 and the Player::ShowMessage2 call are
 * unproven for the same reason. Both classes' talk paths end in the dialogue
 * box the port stops at for SIGN_POST anyway (hal/actor_vtables.cpp), so
 * reaching them needs a real player rather than a longer selftest.
 *
 * This is exactly the shape of the w19 bug: four Renders that were linked,
 * latent and faulting, hidden behind a green battery that never brought the
 * camera near one. Treat the numbers above as "these two classes boot and
 * tick", not as "these two classes work".
 */
#include "ModelAnim.h"

extern "C" {

/* ---- the shared ring, all already linked -------------------------------- */
struct PortVec3 { int x, y, z; };

int Vec3_HorzDist(const void *a, const void *b);
short Vec3_HorzAngle(const void *v0, const void *v1);
short Vec3_VertAngle(const void *v1, const void *v0);
int Vec3_ApproachHorz(void *out, const void *target, int maxStep);
int AngleDiff(int a, int b);
int _Z14ApproachLinearRsss(short *cur, short target, short step);
void *_ZN8dActor_c10FindWithIDEj(unsigned id);
void *_ZN8dActor_c13ClosestPlayerEv(void *self);
int _ZN6Player12GetTalkStateEv(void *self);          /* face: method_faces */
int _ZN6Player9StartTalkER7fBase_cb(void *self, void *actor, int b);
void _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh(
        void *self, void *actor, unsigned msg, const void *pos,
        unsigned d, unsigned e);
int func_ov002_020bec9c(void *player, unsigned a, int b, int d,
                        unsigned short e);
void _ZN5dCc_c5ClearEv(void *self);
void _ZN5dCc_c6UpdateEv(void *self);
void _ZN9Animation7AdvanceEv(void *self);
void _ZN9ModelBase12ApplyOpacityEj(void *self, unsigned opacity);

/* ov085's own, all matched and in port/slice_gate205.txt */
void _ZN4Toad8RunStateEv(void *self);    /* the state machine's Main half */
void _ZN4Toad15UpdateModelPoseEv(void *self);    /* the matrices and the shadow  */

extern short data_02082214[];            /* {sin, cos} pairs, angle>>4 */
extern signed char data_0209f2f8;        /* the level id */

}  /* extern "C" */

/* ApproachLinear2 is reached by its C++ name on purpose: the matched TU
   src/_Z15ApproachLinear2Riii.cpp defines `int ApproachLinear2(int &, int,
   int)` with no extern "C", so MSVC decorates it, and there is no C-name face
   for the int overload the way there is for the short one. Declaring the
   signature is what links the body. */
int ApproachLinear2(int &ref, int target, int step);

extern "C" {

/* ==========================================================================
 * WALL_SIGN (actor 183), ov085 0x0212eea4, 0x30c bytes
 * ==========================================================================
 *
 * The readable wall sign in the castle. Its whole body is one talk sequence
 * with a three-step approach:
 *
 *   NOT TALKING   look for a Player standing in front. The touch record the
 *                 MovingCylinderClsnWithPos at +0x320 leaves is +0x340 bit
 *                 0x8000000 (who) and +0x344 (that actor's id); the toucher
 *                 has to really be a Player (actor id 0xbf) and has to be
 *                 within 0x4000 of the sign's own facing before StartTalk.
 *   TALKING       a three-state machine at +0x364 that walks the player onto
 *                 the reading spot 0x5a000 in front of the sign, turns him to
 *                 face it, and then opens the message.
 *
 * The message id is the actor's own spawn parameter (+0x8, 0xffff meaning
 * "none") stashed at +0x366 before the call.
 */
/* PORT_HOST_ABI: none of its own. Every call below is a plain cdecl C name or
 * an already-hosted face; the body has no vtable dispatch at all. */
int _ZN8WallSign8BehaviorEv(void *selfv)
{
    char *c = (char *)selfv;
    char *pl = *(char **)(c + 0x360);      /* the Player being talked to */

    if (pl != 0) {
        PortVec3 selfPos, front, playerPos;
        unsigned a = (unsigned)*(unsigned short *)(c + 0x8e) >> 4;

        /* the sign's own position, which is also the message's anchor */
        selfPos.x = *(int *)(c + 0x5c);
        selfPos.y = *(int *)(c + 0x60);
        selfPos.z = *(int *)(c + 0x64);

        /* and the reading spot: 0x5a000 along the sign's facing. The ROM
           does the fixed-point multiply as a signed 64-bit product with a
           0x800 round and a >>12, which is what this spells. */
        front.x = *(int *)(c + 0x5c)
                + (int)(((long long)data_02082214[a * 2] * 0x5a000 + 0x800) >> 12);
        front.y = *(int *)(c + 0x60);
        front.z = *(int *)(c + 0x64)
                + (int)(((long long)data_02082214[a * 2 + 1] * 0x5a000 + 0x800) >> 12);

        playerPos.x = *(int *)(pl + 0x5c);
        playerPos.y = *(int *)(pl + 0x60);
        playerPos.z = *(int *)(pl + 0x64);

        switch (_ZN6Player12GetTalkStateEv(pl)) {
        case 0:
            switch (*(unsigned char *)(c + 0x364)) {
            case 0:
                /* close enough already: skip the walk */
                if (Vec3_HorzDist(&playerPos, &front) < 0x32000) {
                    *(unsigned char *)(c + 0x364) += 1;
                    break;
                }
                /* otherwise turn the PLAYER toward the reading spot first,
                   and start his walk animation once he is facing it */
                if (_Z14ApproachLinearRsss((short *)(pl + 0x8e),
                                           Vec3_HorzAngle(&playerPos, &front),
                                           0x800)) {
                    *(unsigned char *)(c + 0x364) += 1;
                    func_ov002_020bec9c(pl, 1, 0, 0x1000, 0);
                }
                break;
            case 1:
                /* walk him onto the spot, then back to the idle animation */
                if (Vec3_ApproachHorz(pl + 0x5c, &front, 0xa000)) {
                    func_ov002_020bec9c(pl, 0, 0, 0x1000, 0);
                    *(unsigned char *)(c + 0x364) += 1;
                }
                break;
            case 2:
                /* turn him to face the sign (its facing + 0x8000) and open
                   the box */
                if (_Z14ApproachLinearRsss(
                        (short *)(pl + 0x8e),
                        (short)(*(short *)(c + 0x8e) + 0x8000), 0x800)) {
                    unsigned param = *(unsigned *)(c + 8);
                    *(unsigned short *)(c + 0x366) = 0;
                    if (param != 0xffff)
                        *(unsigned short *)(c + 0x366) = (unsigned short)param;
                    _ZN6Player12ShowMessage2ER7fBase_cjPK7Vector3hh(
                        pl, c, (unsigned)*(short *)(c + 0x366), &selfPos, 0, 1);
                    *(unsigned char *)(c + 0x364) = 0;
                }
                break;
            }
            break;
        case 1:
            break;
        default:
            /* he walked away or the box closed: drop the reference */
            *(void **)(c + 0x360) = 0;
            break;
        }
    } else if ((*(unsigned *)(c + 0x340) & 0x8000000) != 0) {
        char *o = (char *)_ZN8dActor_c10FindWithIDEj(*(unsigned *)(c + 0x344));
        if (o != 0 && *(unsigned short *)(o + 0xc) == 0xbf) {
            PortVec3 pv;
            pv.x = *(int *)(o + 0x5c);
            pv.y = *(int *)(o + 0x60);
            pv.z = *(int *)(o + 0x64);
            if (AngleDiff(Vec3_HorzAngle(c + 0x5c, &pv),
                          *(short *)(c + 0x8e)) < 0x4000
                && _ZN6Player9StartTalkER7fBase_cb(o, c, 0))
                *(void **)(c + 0x360) = o;
        }
    }

    _ZN5dCc_c5ClearEv(c + 0x320);
    _ZN5dCc_c6UpdateEv(c + 0x320);
    return 1;
}

/* ==========================================================================
 * TOAD (actor 185), ov085 0x02129878, 0x204 bytes
 * ==========================================================================
 *
 * The Toads standing around the castle. Behavior is the per-frame half of a
 * two-state machine (WAIT and TALK, the names are in the ROM at
 * data_ov085_0212fe20 and _0212fe28) plus the head tracking that makes a Toad
 * look at whoever walks past:
 *
 *   +0x202  the head's horizontal offset from the body's own facing
 *   +0x206  the head's vertical angle
 *   +0x200  the horizontal angle actually applied, approached at 0x250/frame
 *   +0x204  the vertical one, at 0x100/frame
 *   +0x20d  the model's fade level, approached toward +0x20e at 6/frame
 *   +0x20e  the fade target: 0xff visible, 0x3c faded
 *
 * The tracking range is 0xfa000 normally and 0x1f4000 on level 0x32, the rec
 * room, where the Toads are the whole point of the room. Level 0x32 is also
 * the only place a Toad turns its BODY to follow: everywhere else an
 * out-of-range Toad just zeroes its head offsets.
 */
/* PORT_HOST_ABI: ROM-order ModelAnim slot-3 dispatch. See hazard (1) in this
 * file's header -- the host table numbers that slot as Virtual10. */
int _ZN4Toad8BehaviorEv(void *selfv)
{
    char *c = (char *)selfv;
    char *p;
    int dist, range;
    short horz = 0, vert = 0;

    _ZN4Toad8RunStateEv(c);
    /* ((Sub *)&mModelAnim)->g3(): ModelAnim at +0x108, ROM slot 3. */
    ((ModelAnim *)(c + 0x108))->ModelAnim::UpdateVerts();

    p = (char *)_ZN8dActor_c13ClosestPlayerEv(c);
    if (p == 0) {
        *(unsigned char *)(c + 0x20e) = 0x3c;
    } else {
        PortVec3 playerPos, aim;

        playerPos.x = *(int *)(p + 0x5c);
        playerPos.y = *(int *)(p + 0x60);
        playerPos.z = *(int *)(p + 0x64);

        range = (data_0209f2f8 == 0x32) ? 0x1f4000 : 0xfa000;
        dist = Vec3_HorzDist(c + 0x5c, &playerPos);

        /* the eye line, not the feet */
        aim.x = playerPos.x;
        aim.y = playerPos.y + 0x1e000;
        aim.z = playerPos.z;

        horz = Vec3_HorzAngle(c + 0x5c, &aim);
        vert = Vec3_VertAngle(c + 0x5c, &aim);

        *(unsigned char *)(c + 0x20e) = (dist < 0x190000) ? 0xff : 0x3c;

        if (dist < range
            && AngleDiff(horz, *(short *)(c + 0x8e)) < 0x3000) {
            *(short *)(c + 0x202) = (short)(horz - *(short *)(c + 0x8e));
            *(short *)(c + 0x206) = vert;
        } else {
            /* out of range, or behind: on the rec room the body turns to
               follow, everywhere else the head just recentres */
            if (data_0209f2f8 == 0x32)
                _Z14ApproachLinearRsss((short *)(c + 0x8e), horz, 0x100);
            *(short *)(c + 0x202) = 0;
            *(short *)(c + 0x206) = 0;
        }
    }

    /* the cap this Toad is holding, if it still exists, keeps him visible */
    if (*(unsigned *)(c + 0x1f4) != 0
        && _ZN8dActor_c10FindWithIDEj(*(unsigned *)(c + 0x1f4)) != 0)
        *(unsigned char *)(c + 0x20e) = 0xff;
    if (data_0209f2f8 == 0x32)
        *(unsigned char *)(c + 0x20e) = 0xff;

    _Z14ApproachLinearRsss((short *)(c + 0x200), *(short *)(c + 0x202), 0x250);
    _Z14ApproachLinearRsss((short *)(c + 0x204), *(short *)(c + 0x206), 0x100);
    ApproachLinear2(*(int *)(c + 0x20d), *(unsigned char *)(c + 0x20e), 6);

    _ZN9Animation7AdvanceEv(c + 0x158);
    _ZN4Toad15UpdateModelPoseEv(c);
    *(int *)(c + 0x164) = 0x1000;
    _ZN9ModelBase12ApplyOpacityEj(c + 0x108,
                                  (unsigned)((*(unsigned char *)(c + 0x20d) >> 3) & 0xff));

    _ZN5dCc_c5ClearEv(c + 0xd4);
    _ZN5dCc_c6UpdateEv(c + 0xd4);
    return 1;
}

}  /* extern "C" */
