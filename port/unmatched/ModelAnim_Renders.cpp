/* HOST COPIES of src/_ZN9Butterfly6RenderEv.cpp, src/_ZN4Fish6RenderEv.cpp
 * and src/_ZN13QuestionBlock6RenderEv.cpp -- the ModelAnim slot-5 collision,
 * which gates 17 and 18 already wrote down and gates 21 and 23 walked into
 * again.
 *
 * hal/cxxname_bridge.cpp fills _ZTV9ModelAnim in MSVC order (dtor 0,
 * DoSetFile 1, UpdateVerts 2, Virtual10 3, Render 4, Virtual18 5), because
 * MSVC spends ONE slot on the destructor where Itanium spends two, and its
 * own comment says the rest out loud: "No dual-fill here: Render's ROM slot
 * (5) is Virtual18's MSVC slot, so shadow-TU Render dispatch cannot be served
 * by the same array." _ZTV5Model CAN be dual-filled and is (Render sits in
 * both slot 4 and slot 5); ModelAnim cannot, because something real already
 * occupies the slot.
 *
 * All three of these TUs dispatch through a LOCAL SHADOW CLASS with six
 * virtuals, which counts in the ROM's numbering, so their "slot 5" is Render
 * and the host array's slot 5 is Virtual18 -- which takes TWO arguments where
 * the shadow passes one, so it read its scale off the stack and then handed
 * Model::Virtual10 whatever that was.
 *
 * MEASURED, not reasoned: with Butterfly::Render compiled from src/ the walk
 * faulted c0000005 in Model::Virtual10 the first time a butterfly was close
 * enough to draw, through ModelAnim::Virtual18 -> ModelAnim::Virtual10 ->
 * Model::Virtual10 with a null matrix. It only showed up under a spawn
 * override because the default walk never brings the camera near one.
 *
 * Each body below is the matched source's control flow line for line; only
 * the dispatches are spelled as the qualified methods the ROM means.
 */
#include "Model.h"
#include "ModelAnim.h"

extern "C" {

/* ---- WHOMP (actor 164/165, ov079) ----------------------------------------
   The same ModelAnim slot-5 collision as the three below, one overlay over.
   src/_ZN5Whomp6RenderEv.cpp dispatches `((Sub *)&mModelAnim)->g5(0)` through a
   LOCAL six-virtual shadow, so its "slot 5" is the ROM's ModelAnim::Render;
   the host _ZTV9ModelAnim array's slot 5 is Virtual18, which reads a scale off
   the stack and hands Model::Virtual10 garbage -> a c0000005 in func_02045074
   the first frame a Whomp is drawn. Excluded from slice_gate64.txt and hosted
   here, the Butterfly reading exactly.

   mModelAnim is at 0x2cc, mTextureSequence at 0x330, mIsKing at 0x414 and
   unk_404 the draw guard; the king variant runs the texture scroll first (its
   ModelComponents arg is mModelAnim.data at 0x2cc+0x8 = 0x2d4). */
int _ZN15TextureSequence6UpdateER15ModelComponents(void *seq, void *comp);

/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish
 * case. */
int _ZN5Whomp6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if (*(unsigned char *)(c + 0x404) == 0)
        return 1;
    if (*(unsigned char *)(c + 0x414) != 0)
        _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x330, c + 0x2d4);
    /* ((Sub *)&mModelAnim)->g5(0) -- the ROM slot-5 Render, spelled qualified */
    ((ModelAnim *)(c + 0x2cc))->ModelAnim::Render(0);
    return 1;
}

/* ---- BUTTERFLY (actor 336, ov100) ----------------------------------------
   State 4 is the dormant one and does not draw. Past that it is the animated
   body while +0x3f1 is set and the plain wing Model at +0x138 otherwise, and
   the second one carries the actor's own scale Vector3 at +0x80. */
/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish
 * case. */
int _ZN9Butterfly6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if (*(int *)(c + 0x3e4) == 4)
        return 1;
    if (*(unsigned char *)(c + 0x3f1) != 0)
        /* ((Base *)&mModelAnim)->m(0) */
        ((ModelAnim *)(c + 0xd4))->ModelAnim::Render(0);
    else
        /* ((Base2 *)&mModel)->m(&mScale) -- Model's slot 5, the dual-filled
           one, spelled qualified anyway so the file reads the same way twice */
        ((Model *)(c + 0x138))->Model::Render((const Vector3 *)(c + 0x80));
    return 1;
}

/* ---- FISH (actor 344, ov100) ---------------------------------------------
   One early out -- +0x159 is the flag its own spawner state sets while the
   shoal has not hatched -- and the draw. */
/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish
 * case. */
int _ZN4Fish6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if (*(unsigned char *)(c + 0x159) == 0)
        /* ((Base *)&mModelAnim)->m(0) */
        ((ModelAnim *)(c + 0xd4))->ModelAnim::Render(0);
    return 1;
}

/* ---- QUESTION_BLOCK (actor 20, ov102) ------------------------------------
   State 2 is the used-up block and does not draw. The animated ModelAnim at
   +0x320 is the question-mark block proper (actor id 0x14) while the save's
   second word does not carry the top bit; every other block in the class --
   the exclamation and cap blocks -- is the plain Model at +0xd4, drawn at the
   actor's own scale. */
extern int data_0209caa0;

/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish
 * case. */
int _ZN13QuestionBlock6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if (*(int *)(c + 0x3e8) == 2)
        return 1;
    if ((*(int *)((char *)&data_0209caa0 + 4) & 0x80000000) == 0) {
        if (*(unsigned short *)(c + 0xc) == 0x14) {
            /* ((Sub *)&mModelAnim)->m(0) */
            ((ModelAnim *)(c + 0x320))->ModelAnim::Render(0);
            return 1;
        }
    }
    /* ((Sub *)&mModel)->m(&mScaleX) */
    ((Model *)(c + 0xd4))->Model::Render((const Vector3 *)(c + 0x80));
    return 1;
}

/* ---- SCUTTLEBUG (actor 255, ov071, gate 176) ------------------------------
   Two early outs -- the 0x40000 flag at +0xb0 and the +0x39c state gate --
   then the draw through ModelAnim slot 5 with a null scale, which the matched
   TU dispatches through a ROM-order local shadow. The host _ZTV9ModelAnim is
   MSVC-ordered, so ROM slot 5 lands on Virtual18 and reads a null matrix (the
   frame-20 fault the first cave boot caught).
   PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int _ZN10Scuttlebug6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if (*(int *)(c + 0xb0) & 0x40000)
        return 1;
    if (*(int *)(c + 0x39c) == 0)
        return 1;
    /* ((Obj *)this)->sub.method5(0) */
    ((ModelAnim *)(c + 0xd4))->ModelAnim::Render(0);
    return 1;
}

/* ---- POWER_STAR (178, ov002, gate 89) ------------------------------------
   src/_ZN9PowerStar6RenderEv.cpp dispatches `sub.m5(&arg80)` through a LOCAL
   six-virtual shadow class (`Sub`), so its "slot 5" is the ROM's
   ModelAnim::Render; the host _ZTV9ModelAnim array's slot 5 is Virtual18,
   which takes TWO arguments where the shadow passes one. It read its scale off
   the stack and then handed Model::Virtual10 garbage -- MEASURED as a
   c0000005 EXECUTION fault at actor+0x38 (the render node) the first frame the
   debug-spawned star draws with its b1 draw-enable bit set (SM64DS_LEVEL=6
   SM64DS_SPAWN_ACTOR=178:0x10, frame 1). This blocked King Bob-omb's star drop.

   The body is the matched source line for line; only the two m5 dispatches are
   spelled as the qualified ModelAnim::Render the ROM means. arg80 is at +0x80
   (passed by address as the scale), the draw guards are fb0 & 0x40000 at +0xB0
   and the b1/b2 bits at +0x4A2, and the two ModelAnims are at +0x30C and +0x370
   (mSilverStarModel1/2). PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch,
   the Whomp/Fish case. */
int _ZN9PowerStar6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if (*(int *)(c + 0x80) == 0)                    /* arg80.x == 0 */
        return 1;
    if ((*(unsigned int *)(c + 0xb0) & 0x40000) != 0)   /* fb0 locked */
        return 1;
    if ((*(unsigned short *)(c + 0x4a2) & 2) == 0)  /* b1 draw-enable clear */
        return 1;
    if ((*(unsigned short *)(c + 0x4a2) & 4) == 0)  /* b2 selects sub30c */
        /* ((Sub *)&sub30c)->m5(&arg80) */
        ((ModelAnim *)(c + 0x30c))->ModelAnim::Render((const Vector3 *)(c + 0x80));
    else
        /* ((Sub *)&sub370)->m5(&arg80) */
        ((ModelAnim *)(c + 0x370))->ModelAnim::Render((const Vector3 *)(c + 0x80));
    return 1;
}

/* ---- BULLY (215) / BIG_BULLY (216) / ROTATING_FIREBAR (81), ov064, gate 177.
   All three matched Renders are the bare slot-5 draw through a six-virtual
   ROM-order shadow -- no early outs. The bullies' ModelAnim sits at +0x110;
   the firebar draws Platform's own plain Model at +0xd4.
   PORT_HOST_ABI: ROM-order model slot-5 dispatch, the Whomp/Fish case. */
int _ZN5Bully6RenderEv(void *selfv)
{ ((ModelAnim *)((char *)selfv + 0x110))->ModelAnim::Render(0); return 1; }
/* PORT_HOST_ABI: ROM-order model slot-5 dispatch, the Whomp/Fish case. */
int _ZN8BigBully6RenderEv(void *selfv)
{ ((ModelAnim *)((char *)selfv + 0x110))->ModelAnim::Render(0); return 1; }
/* PORT_HOST_ABI: ROM-order model slot-5 dispatch, the Whomp/Fish case. */
int _ZN15RotatingFirebar6RenderEv(void *selfv)
{ ((Model *)((char *)selfv + 0xd4))->Model::Render(0); return 1; }

/* ---- UP_DOWN_LIFT_BBH (32/33/131, ov095, gate 173) -----------------------
   The tenth walk into the ModelAnim slot-5 collision. src/_ZN13UpDownLiftBbh6RenderEv.cpp
   dispatches through a LOCAL six-virtual shadow (`Base b; b->m(0)`) over a
   `Derived { char pad[0xd4]; Base base; }`, so its "slot 5" is the ROM's
   ModelAnim::Render off the ModelAnim at +0xd4. The host _ZTV9ModelAnim array's
   slot 5 is Virtual18, a two-arg method called with the shadow's one arg -- it
   read a scale off the stack and handed Model::Virtual10 a null matrix.
   MEASURED as a c0000005 in Model::Virtual10 reached UpDownLiftBbh::Render(+0x11)
   -> ModelAnim::Virtual18 -> ModelAnim::Virtual10 -> Model::Virtual10 through
   port_actor_process, the first frame a lift draws on the natural king-defeat
   path (SM64DS_LEVEL=6 SM64DS_KING_FORCE_DEFEAT=30, the defeat cutscene renders
   the level's lifts). No early outs; the source is `b->m(0)`, one null-scale draw.
   Excluded from slice_gate173.txt (the _ZN13UpDownLiftBbh6RenderEv line) and
   hosted here, the Bully/Scuttlebug reading exactly.
   PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int _ZN13UpDownLiftBbh6RenderEv(void *selfv)
{ ((ModelAnim *)((char *)selfv + 0xd4))->ModelAnim::Render(0); return 1; }

/* ---- HEALING_HEART via SEAWEED (297, ov002, gate 33) ---------------------
   THE measured Bob-omb Battlefield fault once UpDownLiftBbh above stopped
   faulting. HEALING_HEART (297) shares Seaweed's vtable (_ZTV7Seaweed, RTTI
   daObjHeart_c) and so renders through Seaweed::Render, the same bare `b->m(0)`
   off its ModelAnim at +0xd4 through a local six-virtual ROM-order shadow.
   src/_ZN7Seaweed6RenderEv.cpp is byte-identical to eleven other bare +0xd4
   draws (TowerStep/SeesawBob/MetalNet/MontyMole/...), so MSVC's ICF folds them
   all to one body -- and that shared body lands on the host _ZTV9ModelAnim's
   Virtual18 (a two-arg method) with the shadow's one arg, handing
   Model::Virtual10 a null matrix. MEASURED as a c0000005 (reads null 00000000)
   in Model::Virtual10 reached Seaweed::Render(+0x11) -> ModelAnim::Virtual18 ->
   ModelAnim::Virtual10 -> Model::Virtual10 through port_actor_process, the first
   frame the HEALING_HEART drew (actor 04811C74, SM64DS_ACTOR_PROBE named it) on
   the natural king-defeat path (SM64DS_LEVEL=6 SM64DS_SPAWN=1501,4192,-5100
   SM64DS_KING_FORCE_DEFEAT=90, frame 354). No early outs; the source is `b->m(0)`,
   one null-scale draw. Excluded from slice_gate33.txt and dispatched by C name
   from the HealingHeart vtable fill in hal/actor_classes_bob_world.cpp; fixes both
   HealingHeart and any on-screen SEAWEED. The UpDownLiftBbh reading exactly.
   PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int _ZN7Seaweed6RenderEv(void *selfv)
{ ((ModelAnim *)((char *)selfv + 0xd4))->ModelAnim::Render(0); return 1; }

/* ---- SEESAW_BOB (39, ov095, gate 83) -------------------------------------
   A latent sibling of the collision above: SeesawBob (Bob-omb Battlefield's
   seesaw bridge, in the king's arena sub-area at -2250,700,1000) draws the same
   bare `b->m(0)` off its ModelAnim at +0xd4 through a local six-virtual ROM-order
   shadow, dispatched as the C++ method at actor_classes_bob_world.cpp's ssb_render.
   src/_ZN9SeesawBob6RenderEv.cpp is byte-identical to the fold group above, so its
   own vtable call would land on the host _ZTV9ModelAnim's Virtual18 the first
   frame the seesaw draws close enough. Hosted here pre-emptively with the same
   qualified ModelAnim::Render dispatch as its siblings (the seesaw sits away from
   the king so the SM64DS_SPAWN=1501,... camera never brought it on-screen before
   the HEALING_HEART faulted first, but the collision is identical). Excluded from
   slice_gate83.txt, dispatched by C name from the SeesawBob vtable fill.
   PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int _ZN9SeesawBob6RenderEv(void *selfv)
{ ((ModelAnim *)((char *)selfv + 0xd4))->ModelAnim::Render(0); return 1; }

/* ---- UNCHAINED_CHOMP (actor 337, ov100, gate 21) --------------------------
   The eleventh+ walk into the ModelAnim slot-5 collision. src/_ZN14Unchained
   Chomp6RenderEv.cpp dispatches through a LOCAL SIX-VIRTUAL SHADOW CLASS
   (`struct B { ... virtual void m5(A* arg); }`) off mModelAnim at +0x30c
   (include/UnchainedChomp.h, evidenced, not +0xd4 -- the ModelAnim sits deep in
   this class's layout), so its "slot 5" is the ROM's ModelAnim::Render; the host
   _ZTV9ModelAnim array's slot 5 is Virtual18, a two-argument method called with
   the shadow's one arg -- the Whomp/Fish case. The first call passes the actor's
   own scale (&mScaleX, +0x80); the second is a 5-iteration loop over the +0x370
   Model array (stride 0x50, 6 constructed by Spawn/D1 but Render's own loop bound
   stops at 5) with a NULL scale each time. The loop half needs no extra wiring:
   _ZTV5Model[5] is dual-filled (hal/cxxname_bridge.cpp), so the qualified
   Model::Render below is byte-faithful to what ROM slot 5 does there.
   PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int _ZN14UnchainedChomp6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    ((ModelAnim *)(c + 0x30c))->ModelAnim::Render((const Vector3 *)(c + 0x80));
    {
        int j = 0;
        char *p2 = c + 0x370;
        for (;;) {
            ((Model *)p2)->Model::Render(0);
            j++;
            p2 += 0x50;
            if (j >= 5) break;
        }
    }
    return 1;
}

/* ---- BABY_PENGUIN (actor 256, ov072, gate 193) ----------------------------
   The collision that excluded BABY_PENGUIN from gate 193 -- root-caused by the
   babypenguin-crash lane, MEASURED not reasoned: with src/_ZN11BabyPenguin6Render
   Ev.cpp compiled from the slice, the first frame instance 1 drew (frame 126 of
   the L10 selftest, actor 0x0481d620) died on a DEP execute-violation with
   EIP == the actor pointer ITSELF. Raw-stack forensics (cdb at the first-chance
   AV): the faulting instruction was a RET that popped `self` off the stack --
   the 4-byte esp skew the bridge's own "trap-by-Virtual18" comment predicts.
   The matched TU dispatches `sub.m(&mScaleX)` through a LOCAL SIX-VIRTUAL
   ROM-order shadow off mModelAnim at +0xd4, so its "slot 5" is the ROM's
   ModelAnim::Render; the host _ZTV9ModelAnim's slot 5 is Virtual18, whose
   __fastcall trampoline pops TWO stack words where the shadow call pushed one.
   The skewed caller then rets into its own spilled `self`. (The earlier
   "corrupted-instruction-pointer jump in the shared actor-Process pipeline"
   reading was the VICTIM frame, not the culprit: port_actor_process's frame is
   simply where the skewed ret surfaces. daBgSnwmn_c is NOT exposed: its +0xd4/
   +0x124 members are plain 0x50-stride Models, and _ZTV5Model[5] is dual-filled.)
   Excluded from slice_gate193.txt, dispatched by C name from bp_render in
   hal/actor_classes_ov072.cpp. Matched-source control flow line for line:
   flags +0xb0 bit 0x40000 is the draw guard, the scale is the actor's own
   Vector3 at +0x80 (mScaleX).
   PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int _ZN11BabyPenguin6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if (*(unsigned int *)(c + 0xb0) & 0x40000)
        return 1;
    ((ModelAnim *)(c + 0xd4))->ModelAnim::Render((const Vector3 *)(c + 0x80));
    return 1;
}

/* ---- HOOT_THE_OWL (234, ov094, gate 194) ---------------------------------
   src/_ZN10HootTheOwl6RenderEv.cpp dispatches through a LOCAL six-virtual
   ROM-order shadow (`struct O { ... virtual void m5(int); }; ((O*)&mModelAnim)
   ->m5(0)`) over mModelAnim at +0x30c -- the Bully/BigBully bare-call shape,
   no scale argument. The one early-out compares the state-cell pointer at
   +0x3c8 (mCurrentState) against &data_ov094_02136b40 (the state table's
   base, seated by this class's own InitResources); a real state means the
   comparison is false and the draw runs. Excluded from slice_gate194.txt,
   the Bully/Scuttlebug reading exactly.
   PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int _ZN10HootTheOwl6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    extern char data_ov094_02136b40[];
    if (*(void **)(c + 0x3c8) == (void *)data_ov094_02136b40)
        return 1;
    ((ModelAnim *)(c + 0x30c))->ModelAnim::Render(0);
    return 1;
}

/* ---- SWITCH_PILLAR (34, ov012, gate 199) ---------------------------------
   src/func_ov012_02111324.cpp (the REAL SwitchPillar's own Render, the
   daObjC0_Switch_c body -- see port/ov012_syms.txt's class-identity note)
   dispatches through a LOCAL six-virtual ROM-order shadow
   (`struct Base{...virtual void m(int);}; struct Derived{char pad[0xd4];
   Base base;}; b->m(0);`) over a plain Model at +0xd4 -- the
   RotatingFirebar bare-call shape exactly (no scale argument, a Platform's
   own plain Model, not a ModelAnim). Excluded from slice_gate199.txt.
   PORT_HOST_ABI: ROM-order model slot-5 dispatch, the Whomp/Fish case. */
int func_ov012_02111324(void *selfv)
{ ((Model *)((char *)selfv + 0xd4))->Model::Render(0); return 1; }

/* ---- BASEMENT_WATER (35, ov012, gate 199) --------------------------------
   src/_ZN12SwitchPillar6RenderEv.cpp -- despite the filename, BasementWater's
   REAL Render (see port/ov012_syms.txt's class-identity note) -- calls
   TextureTransformer::Update first (the Whomp king-variant texture-scroll
   precedent, mTextureTransformer at +0x320, the ModelComponents arg at
   +0xdc per the matched src's own offsets), then dispatches through the
   SAME LOCAL six-virtual ROM-order shadow over the plain Model at +0xd4.
   Excluded from slice_gate199.txt.
   PORT_HOST_ABI: ROM-order model slot-5 dispatch, the Whomp/Fish case. */
int _ZN18TextureTransformer6UpdateER15ModelComponents(void *tt, void *mc);
int _ZN12SwitchPillar6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    _ZN18TextureTransformer6UpdateER15ModelComponents(c + 0x320, c + 0xdc);
    ((Model *)(c + 0xd4))->Model::Render(0);
    return 1;
}

/* ---- SPINDRIFT (actor 312, ov081, gate 192) -------------------------------
   The same collision, one overlay over. src/_ZN9Spindrift6RenderEv.cpp guards
   on the +0xb0 & 0x40000 draw bit, then dispatches `((Obj*)&mModelAnim)->m5(0)`
   through a LOCAL six-virtual ROM-order shadow off mModelAnim at +0x110 (the
   render probe's own offset), so its "slot 5" is the ROM's ModelAnim::Render;
   the host _ZTV9ModelAnim's slot 5 is Virtual18, a two-arg method called with
   the shadow's one arg -- it read a scale off the stack and handed
   Model::Virtual10 a null matrix. MEASURED as a c0000005 in Model::Virtual10
   (module offset +0x31fbc) the first frame a spindrift drew on the L10 selftest
   -- the deterministic quarantine 7c3e71d86's .text-layout shift exposed. The
   src is `o->m5(0)`: one null-scale draw, no scale arg. Excluded from
   slice_gate192.txt, dispatched by C name from spd_render (actor_classes_ov081).
   PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int _ZN9Spindrift6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if (*(unsigned int *)(c + 0xb0) & 0x40000)
        return 1;
    ((ModelAnim *)(c + 0x110))->ModelAnim::Render(0);
    return 1;
}

/* ---- MONEYBAG (actor 261, ov081, lane w3-b) ------------------------------
   The same collision as SPINDRIFT above, one class over in the same overlay.
   src/_ZN8Moneybag6RenderEv.cpp dispatches through a LOCAL six-virtual
   ROM-order shadow (`struct Sub { virtual int v0..v4(); virtual int m(void*);
   }`) TWICE: once off mModelAnim at +0xd4 and once off mModel at +0x138. Only
   the FIRST is exposed -- the host _ZTV9ModelAnim's slot 5 is Virtual18, a
   two-arg method called with the shadow's one arg; _ZTV5Model IS dual-filled
   at [4] and [5], so the second draw would be correct either way and is
   spelled qualified here anyway so the file reads the same way twice.

   The member types are adjudicated FROM CONSTRUCTION, not from a header name
   (the MontyMoleRock rule): Moneybag_Spawn calls _ZN9ModelAnimC1Ev(p + 0xd4)
   and _ZN5ModelC1Ev(p + 0x138); _ZN8MoneybagD0Ev calls _ZN9ModelAnimD1Ev(t +
   0xd4) and _ZN5ModelD1Ev(t + 0x138); InitResources feeds ModelBase::SetFile
   at both. +0xd4 is a ModelAnim, so it is the Whomp/Fish case.

   Control flow line for line: the +0xb0 & 0x40000 cull bit, then the +0x3f0
   crossfade counter -- the same byte func_ov081_02126a20 hands
   ModelBase::ApplyOpacity as the anim model's opacity and (0x20 - it) as the
   plain model's, so the two draws overlap across the fade. Both dispatches
   pass a null scale.
   PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int _ZN8Moneybag6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if (*(unsigned int *)(c + 0xb0) & 0x40000)
        return 1;
    if (*(unsigned char *)(c + 0x3f0) > 1)
        ((ModelAnim *)(c + 0xd4))->ModelAnim::Render(0);
    if (*(unsigned char *)(c + 0x3f0) <= 0x1f)
        ((Model *)(c + 0x138))->Model::Render(0);
    return 1;
}

/* ---- PUSH_BLOCK (306, ov002, gate 200) -----------------------------------
   src/_ZN9PushBlock6RenderEv.cpp dispatches through a LOCAL six-virtual
   ROM-order shadow (`struct Sub { virtual int g0..g4(); virtual void
   g5(void*); }`) over the class's two plain Models -- mModel1 at +0xd4 for
   state 0, mModel2 at +0x124 for states 1 and 2 -- WITH the scale pointer
   (&mScaleX, +0x80), the BabyPenguin argument shape over the SwitchPillar
   plain-Model member. Draw guard +0xb0 & 0x40000, then the blink gate: the
   +0x3ca counter under 0x2d skips odd frames. State selector +0x3c0 is the
   same idx the host state dispatch (PushBlock_StateDispatch.cpp) stores.
   Excluded from slice_gate200.txt, dispatched from hal/actor_classes_
   ov002g200.cpp.
   PORT_HOST_ABI: ROM-order model slot-5 dispatch, the Whomp/Fish case. */
int _ZN9PushBlock6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if (*(unsigned int *)(c + 0xb0) & 0x40000)
        return 1;
    unsigned char st = *(unsigned char *)(c + 0x3ca);
    if (st < 0x2d && (st & 1))
        return 1;
    switch (*(int *)(c + 0x3c0)) {
    case 0: ((Model *)(c + 0xd4))->Model::Render((const Vector3 *)(c + 0x80)); break;
    case 1: ((Model *)(c + 0x124))->Model::Render((const Vector3 *)(c + 0x80)); break;
    case 2: ((Model *)(c + 0x124))->Model::Render((const Vector3 *)(c + 0x80)); break;
    }
    return 1;
}

/* ---- BOO (209) / BIG_BOO (210), ov063, wave 5a -----------------------------
   The collision that held the wave-5 close. src/actors/Boo/_ZN3Boo6RenderEv.cpp
   dispatches `((Model*)&mModelAnim)->Render(&unk_080)` through a LOCAL
   six-virtual shadow (`struct Model { f0..f4; virtual Render(const Vector3*)
   }`), so its "slot 5" is the ROM's ModelAnim::Render off mModelAnim at
   +0x380; the host _ZTV9ModelAnim's slot 5 is Virtual18, a two-arg method
   called with the shadow's one arg -- the Whomp/Fish case. MEASURED as
   c0000005 at func_0204488c+0x10a accessing 00000009 on frame 0 of the L2
   selftest (SM64DS_LEVEL=2 SM64DS_FAULTS_FATAL=1): Virtual18 read its scale
   off the stack (the fault dump's eax=1) and passed it down as
   func_0204488c's color pointer, so color[2] read address 1+8=9. The
   faulting Boo is level 2's variant-4 one (objTable entry id 0x43, param
   0xff04); its variant-0xb twin at the same position self-destructs at 0
   stars (the NumStars<3 gate) before it can draw. BBH's six Boos never
   showed the collision: the L12 selftest keeps them all behind the +0xb0
   0x40000 cull bit (unshown rooms), so level 12 passing was vacuous for
   this path. Level 3 (castle garden) has two surviving Boos on the same
   body and inherits this fix. The held-item draw (mModel, a plain 0x50
   Model at +0x3e4) is safe through the dual-filled _ZTV5Model, spelled
   qualified like its siblings; BIG_BOO shares _ZTV3Boo and this body.
   Excluded from slice_w5a.txt, dispatched by C name from boo_render
   (hal/actor_classes_ov063.cpp). Matched-source control flow line for
   line: cull +0xb0 & 0x40000, the +0x5d4 flag word's bit 3 (0x8) is the
   draw-enable and bit 1 (0x2) the held-item draw, +0x5c8 < 8 skips the
   body draw, states >= 0xc except 0xf hide material pair (0, 2), and the
   +0x10c != 8 / +0x5cc == 3 branch is the ROM's own DIRECT (non-virtual)
   Model::Render on the ModelAnim, kept direct here.
   PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
void _ZN8CapEnemy14RenderCapModelEPK7Vector3(void *thiz, const void *v);
void _ZN5Model12HideMaterialEii(void *self, int boneID, int listIdx);
int _ZN3Boo6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if (*(unsigned int *)(c + 0xb0) & 0x40000)
        return 1;
    {
        unsigned short f = *(unsigned short *)(c + 0x5d4);
        if (!(f & 8))
            return 1;
        if (f & 2)
            /* ((Model *)&mModel)->Render(&unk_510) -- Model's slot 5, the
               dual-filled one */
            ((Model *)(c + 0x3e4))->Model::Render((const Vector3 *)(c + 0x510));
        _ZN8CapEnemy14RenderCapModelEPK7Vector3(c, 0);
    }
    if (*(unsigned char *)(c + 0x5c8) < 8)
        return 1;
    {
        unsigned char st = *(unsigned char *)(c + 0x5cf);
        if (st >= 0xc && st != 0xf)
            _ZN5Model12HideMaterialEii(c + 0x380, 0, 2);
    }
    if (*(int *)(c + 0x10c) != 8 && *(unsigned char *)(c + 0x5cc) == 3)
        /* the matched TU's own direct _ZN5Model6RenderEPK7Vector3 call */
        ((Model *)(c + 0x380))->Model::Render((const Vector3 *)(c + 0x80));
    else
        /* ((Model *)&mModelAnim)->Render(&unk_080) -- ROM slot 5, spelled
           qualified */
        ((ModelAnim *)(c + 0x380))->ModelAnim::Render((const Vector3 *)(c + 0x80));
    return 1;
}

/* ---- FLY_GUY (actor 232, ov070) -------------------------------------------
   The same ModelAnim slot-5 collision, a fourth overlay over, and this one is
   the real thing rather than a false alarm on a plain Model.
   src/_ZN6FlyGuy6RenderEv.cpp declares a LOCAL six-virtual ROM-order shadow
   (m0..m4 then Target) and calls Target(0) on &mModelAnim, so its "slot 5" is
   the ROM's ModelAnim::Render(const Vector3*). The receiver really is a
   ModelAnim, checked two ways that agree: _ZN6FlyGuyD1Ev.c and
   _ZN6FlyGuyD0Ev.c both call _ZN9ModelAnimD1Ev on this+0x300, and FlyGuy.h
   puts mModelAnim at 0x300. _ZTV9ModelAnim's slot 5 is Virtual18 and the
   array CANNOT be dual-filled for the reason this file's header gives, so the
   matched TU is dropped from slice_w5c.txt and this is the port body.

   Control flow is the matched source line for line -- the 0x40000 bit at
   +0xb0 is FlyGuy.h's unk_0b0, and the argument really is a null Vector3
   pointer (the ROM passes r1 = 0), not a scale this copy invented.
   PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int _ZN6FlyGuy6RenderEv(void *self)
{
    char *c = (char *)self;
    if (*(unsigned int *)(c + 0xb0) & 0x40000)
        return 1;
    /* ((ModelAnim *)&mModelAnim)->Render(0) -- ROM slot 5, spelled qualified */
    ((ModelAnim *)(c + 0x300))->ModelAnim::Render((const Vector3 *)0);
    return 1;
}

}  /* extern "C" */
