// Gate-10 method bridges: C-named references -> MSVC method definitions.
//
// The Player closure's callers reference these at C linkage while the
// defining TUs compile them as real methods against the shared headers.
// Same hop as gate 9 (cxxname_bridge.cpp), split into its own TU because
// Player.h drags a wider include surface than the gate-9 file wants.
#include <cstdio>
#include <cstdlib>

#include "Animation.h"
#include "BgCh.h"
#include "NestedHeapIterator.h"
#include "Player.h"
#include "player_fields.h"   /* run mg16 lane MP4: the one place field offsets live */
#include "ShadowModel.h"
#include "TextureSequence.h"
#include "Heap.h"
#include "ModelAnim.h"

#include "dsstate_seg.h"

/* the geometry-engine polygon buffer, for the tongue render self-check
   (SM64DS_WINGS_PROBE); same forward decl cxxname_bridge.cpp uses so the
   header's wider surface does not have to come in here */
namespace ntr { struct GxTriangle; const GxTriangle *gx_polygons(std::size_t &n); }

/* how many times hal_call_state_fn fell off the end of its switch this run --
   read by the F3 overlay in port/tests/walk_window.cpp */
extern "C" unsigned g_port_unhosted_hits = 0;

extern "C" unsigned int _ZNK6Player14GetBodyModelIDEjb(char *, unsigned int, char);
extern "C" unsigned func_ov002_020becf4(char *self, unsigned j, int b);
extern "C" int _ZN6Player13InitResourcesEv(void *);

/* C++-linkage globals some slice TUs call under Itanium-style names */
int _ZNK9Animation12WillHitFrameEi(void *self, int f)
{ return ((Animation *)self)->Animation::WillHitFrame(f) ? 1 : 0; }
/* GetFlags is C linkage now: main's mangled-declaration sweep gave the Player
   state TUs an extern "C" declaration, so they call the plain name. The alias
   at the head of hal/cxx_aliases.cpp still serves the old C++ mangling. */
extern "C" int _ZN9Animation8GetFlagsEv(void *self)
{ return ((Animation *)self)->Animation::GetFlags(); }
void _ZN6Player4HealEi(Player *p, int amt)
{ p->Player::Heal(amt); }

unsigned int Player::GetBodyModelID(unsigned int a, bool b_) const
{ return _ZNK6Player14GetBodyModelIDEjb((char *)this, a, b_ ? 1 : 0); }

extern "C" {
/* gate-10 smoke drives the state machine directly (the ChangeState PMF
   dispatch reads DS-baked member-fn bytes MSVC cannot represent) */
int hal_player_init_resources(void *p)
{ return _ZN6Player13InitResourcesEv(p); }
int hal_player_st_wait_init(void *p)
{ return ((Player *)p)->Player::St_Wait_Init(); }
int hal_player_st_wait_main(void *p)
{ return ((Player *)p)->Player::St_Wait_Main(); }
int hal_player_st_walk_init(void *p)
{ return ((Player *)p)->Player::St_Walk_Init(); }
int hal_player_st_walk_main(void *p)
{ return ((Player *)p)->Player::St_Walk_Main(); }
int hal_player_behavior(void *p)
{ return ((Player *)p)->Player::Behavior(); }
/* the walk demo renders the Player's current body ModelAnim in place:
   identity model matrix, bones posed from the anim Behavior advanced */
/* level model render for the window: identity world matrix (stage models
   are authored in world space; the KCL shares it) */
/* SM64DS_TEX_LOG=1: the BMD's own texture/palette/material tables next to
   the runtime material records func_020462d0 built from them -- the
   ground truth a [texbind] line is checked against. One shot per file. */
extern "C" int hal_tex_log(void);
static void hal_dump_model_tables(Model *m)
{
    static BMD_File *done[16];
    static int ndone;
    /* ModelBase::modelFile is only filled by LoadAndSetFile; the harness
       drives SetFile directly, so read the file the components carry. */
    BMD_File *f = m->data.modelFile;
    if (!f) return;
    for (int i = 0; i < ndone; ++i)
        if (done[i] == f) return;
    if (ndone < 16) done[ndone++] = f;

    printf("[texmodel] file=%p tex=%u pal=%u mat=%u\n", (void *)f,
           f->numTextures, f->numPalettes, f->numMaterials);
    for (u32 i = 0; i < f->numTextures; ++i) {
        BMD_Texture *t = f->textures + i;
        printf("  tex %2u %-20s flags %08x fmt %u %3dx%-3d size %5u "
               "vramoff %05x\n",
               i, (const char *)t->unk_00, t->flags, (t->flags >> 26) & 7,
               8 << ((t->flags >> 20) & 7), 8 << ((t->flags >> 23) & 7),
               t->size, (t->flags & 0xffff) << 3);
    }
    for (u32 i = 0; i < f->numPalettes; ++i) {
        BMD_Palette *p = f->palettes + i;
        printf("  pal %2u %-20s size %5u vramoff %05x -> pltt %04x\n", i,
               (const char *)p->unk_00, p->size, p->vramOffset,
               p->vramOffset >> 4);
    }
    const unsigned char *mm = (const unsigned char *)m->data.materials;
    for (u32 i = 0; i < f->numMaterials && mm; ++i) {
        const u32 *e = (const u32 *)(mm + i * 0x30);
        printf("  mat %2u %-20s tex %3d pal %3d teximage %08x pltt %04x "
               "attr %08x difamb %08x\n",
               i, (const char *)f->materials[i].unk_00, (int)e[0], (int)e[1],
               e[7], e[8], e[9], e[10]);
    }
}

/* Stage::RenderModel's own scale argument, {125.0, 125.0, 125.0} Fix12i.
   Emitted by port/tools/romdata.py from the arm9 image. */
extern unsigned char data_020755d4[];

void hal_render_model(void *model, int scaleShift)
{
    Model *m = (Model *)model;
    if (hal_tex_log()) hal_dump_model_tables(m);
    /* THE STAGE RENDERS IN SCENE UNITS, and this is Stage::RenderModel's own
       shape: the model matrix is the Model constructor's identity
       (data_02082128) and the entire scale travels through Model::Render's
       Vector3 argument -- data_020755d4 -- which the ordinary part walk
       spends as an MTX_SCALE on top of its own 1 << (shift + 12). Scene is
       what everything else in the frame is now in: the view matrix
       Camera::Render hands LookAt_ as (v + 4) >> 3, the model matrices the
       actors' own Render methods fill, and the positions Actor::BeforeBehavior
       clips with. The old world-unit matrix (0x1000 << (shift + 10), x40/41)
       and the R6 view shim that paid for it are both gone.

       NOTHING IS CORRECTED HERE ANY MORE. The old world matrix carried a
       measured x40/41, and that 2.5% was the residue of forcing a power of two
       (0x1000 << (shift + 10) = x2048) onto a scale the ROM writes as 125.0.
       With the part walk's own MTX_SCALE reaching the geometry engine again
       (the hostgen MMIO_PTR hole, port/tools/hostgen.py) the ROM's vector is
       exact: the stage comes out x[-1000..1000] y[-225..953.5] scene, which is
       x[-8000..8000] y[-1799.8..7627.9] world, and the castle grounds' KCL --
       the same terrain, in the world units the Player and the object table use
       -- puts its lowest vertex at -1800.0 and its outer wall at +-8500.
       SM64DS_LEVEL_SCALE=N overrides the whole vector for the A/B. */
    /* This runs per model per frame, and MSVC's getenv scans the whole
       environment block, so both overrides are latched on first use. */
    int s = *(const int *)data_020755d4;
    static int scale_override = 0, scale_value = 0;
    if (!scale_override) {
        const char *e = std::getenv("SM64DS_LEVEL_SCALE");
        scale_override = e ? 1 : -1;
        if (e) scale_value = std::atoi(e);
    }
    if (scale_override > 0) s = scale_value;
    static int probe = -1;
    if (probe < 0) probe = std::getenv("SM64DS_MODEL_PROBE") ? 1 : 0;
    if (probe) {
        const BMD_File *f2 = m->data.modelFile;
        const int *t = (const int *)m->data.transforms;
        int lo[3] = {1 << 30, 1 << 30, 1 << 30},
            hi[3] = {-(1 << 30), -(1 << 30), -(1 << 30)};
        unsigned nb = f2 ? f2->numBones : 0;
        for (unsigned i = 0; i < nb && t; ++i)
            for (int k = 0; k < 3; ++k) {
                int v = t[i * 12 + 9 + k];
                if (v < lo[k]) lo[k] = v;
                if (v > hi[k]) hi[k] = v;
            }
        std::printf("[mprobe] shift %u (arg %d) bones %u scale %d (%.3f) "
                    "part.t x[%.3f..%.3f] y[%.3f..%.3f] z[%.3f..%.3f]\n",
                    f2 ? f2->scaleShift : 0u, scaleShift, nb, s, s / 4096.0,
                    lo[0] / 4096.0, hi[0] / 4096.0, lo[1] / 4096.0,
                    hi[1] / 4096.0, lo[2] / 4096.0, hi[2] / 4096.0);
    }
    Vector3 scale = {s, s, s};
    for (int i = 0; i < 12; ++i) ((int *)&m->mat4x3)[i] = 0;
    ((int *)&m->mat4x3)[0] = 0x1000;
    ((int *)&m->mat4x3)[4] = 0x1000;
    ((int *)&m->mat4x3)[8] = 0x1000;
    m->Model::Render(&scale);
}

void hal_render_player_body_ex(void *player, int with_head);
void hal_render_player_body(void *player)
{ hal_render_player_body_ex(player, 1); }
void hal_render_player_body_only(void *player)
{ hal_render_player_body_ex(player, 0); }
/* scene-space variant: mat4x3 = Y-rotation(mAngleY) at 1.0 + the scene
   translation, the head composed through the body's own matrix (the neck
   bone is model-space) */
extern "C" short data_02082214[];   /* s16 trig pairs [sin, cos], 4096 = 1 */
/* the three matrix helpers and the scratch matrix Player::Render uses to seat
   mModelAnim4 (+0x174); all four are matched and already in the link */
extern "C" {
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationZ(void *m, int ang);
void MulMat4x3Mat4x3(const void *a, const void *b, void *out);
extern int data_020a0e68;
/* base Model::Render, non-virtually -- the exact call Player::Render's i==3 arm
   makes on the Yoshi head. C-linkage face over the matched
   src/_ZN5Model6RenderEPK7Vector3.cpp, so the dispatch is non-virtual as in the
   ROM: index 3 is a ModelAnim, and going through its own vtable Render slot
   would re-run UpdateVerts, which is why Player::Render reaches past it to the
   base method and renders the head at the pose the anim system already left. */
void _ZN5Model6RenderEPK7Vector3(void *self, const void *pos);
}
static void m43_mul(const int *a, const int *b, int *out)
{
    for (int r = 0; r < 3; ++r)
        for (int c2 = 0; c2 < 3; ++c2)
            out[r * 3 + c2] = (int)(((long long)a[r * 3] * b[c2] +
                                     (long long)a[r * 3 + 1] * b[3 + c2] +
                                     (long long)a[r * 3 + 2] * b[6 + c2]) >>
                                    12);
    for (int c2 = 0; c2 < 3; ++c2)
        out[9 + c2] = (int)(((long long)a[9] * b[c2] +
                             (long long)a[10] * b[3 + c2] +
                             (long long)a[11] * b[6 + c2]) >>
                            12) +
                      b[9 + c2];
}

/* THE HEAD-MODEL GROUP, Player::Render's second draw. Player::Render walks one
   model out of the array at Player +0x154, indexed by
   func_ov002_020becf4(this, unk_6db, 1), and forks on that index
   (src/_ZN6Player6RenderEv.cpp lines 120-128):

       if (i == 3) {                                    // Yoshi's head
           Model::Render(mdl4, this+0x80);              // base, non-virtual, NO seat
       } else {                                         // Mario/Luigi/Wario heads
           *(M34*)mdl4->bones[0] = *(M34*)(bodyBones + 0x2d0);  // neck bone
           mdl4->Virtual10(&mScale);
       }

   The array is seated in func_ov002_020e5948 (InitResources): the four
   character heads load into indices 0-3 from data_ov002_0210a69c, and index 3
   -- Yoshi -- is a ModelAnim (0x64 bytes, a base BCA anim) while 0-2 are plain
   Models (0x50). func_ov002_020becf4 returns 3 for Yoshi in normal play (its
   v==3 branch adds 4 only when unk_714, "something in the mouth", is set), so
   i==3 is Yoshi's EVERY-FRAME head index, not a tongue-only case. That is why
   it is special-cased at all: Mario/Luigi/Wario wear a separate cap model that
   has to be pinned to the body's neck bone, but Yoshi's head is a full model
   authored in body space and drawn at the player root.

   THE MATRIX IS NOT SEATED HERE, and that is the whole correction over take 1.
   func_ov002_020e444c -- which the port already runs every frame inside
   Player::Behavior (func_ov002_020e4bb8 -> func_ov002_020e444c, and via
   func_ov002_020e3f90 when Yoshi is carrying something) -- has already written
   head+0x1c to the player-root matrix by the time render runs. In the port's
   scene convention that value is bit-identical to the `scene` matrix the body
   renders with (a head-vs-body matrix probe read head+0x1c == body+0x1c ==
   scene on every frame). So the i==3 arm dispatches base Model::Render with
   nothing seated, exactly as the ROM does.

   Take 1 seated head+0x1c = scene by hand -- redundant, it already was -- and,
   worse, believed i==3 meant "mid-tongue" and so it read as a new render on top
   of the else arm the port still ran; the real fault the human saw (head gone,
   a model upside down under the feet) is the else arm's neck-bone compose being
   run over Yoshi's index-3 head, which is not neck-parented. Routing i==3 to the
   ROM's own base Model::Render, and leaving every other head on the neck-bone
   compose, is the fix.

   SCOPE: this draws Yoshi's HEAD in the right place every frame. It does not on
   its own make the tongue reach: the head ModelAnim's currFrame does not advance
   in the port (St_YoshiPower_Main's Animation::Advance on +0x160 leaves it at 0
   here), so the mouth/tongue animation stays at frame 0. Extending the tongue is
   a separate open gap (the head anim never being advanced/posed), not this fork. */
static void hal_render_head_group(char *c, char *head, unsigned hid,
                                  ModelAnim *ma, const int *scene)
{
    if (hid == 3) {
        /* Yoshi's head: mat4x3 already seated by func_ov002_020e444c this frame;
           dispatch the BASE Model::Render (non-virtual) with c+0x80, exactly the
           ROM's i==3 arm. Base, not the object's own slot 4: index 3 is a
           ModelAnim, whose virtual Render would re-run UpdateVerts, and the ROM
           calls Model::Render directly to render the head at the pose the anim
           system already produced. */
        _ZN5Model6RenderEPK7Vector3(head, c + 0x80);
        return;
    }

    /* every other head: the port's scene-space stand-in for the ROM's neck-bone
       compose. The ROM copies the body's neck bone into the head's bone[0] and
       calls Virtual10; on host the head's mat4x3 is what Model::Render consumes,
       so compose the neck transform through `scene` into head+0x1c and render
       through the object's own slot 4 (Model::Render). */
    char *neck = *(char **)((char *)ma + 0x14) + 0x2d0;
    if (neck)
        m43_mul((const int *)neck, scene, (int *)(head + 0x1c));
    ((void(__fastcall *)(void *, void *, const void *))(
        ((void ***)head)[0][4]))(head, 0, 0);
}

/* run mg16 lane MP3: the two globals Player::Render's own gates read. */
extern "C" {
extern unsigned char data_0209f2d8;   /* VS mode flag */
extern int data_0209fc5c[];           /* per-slot "this slot is live" */
}

/* ---- THE ROM'S PER-FRAME TEXTURE-SEQUENCE UPDATES --------------------------
   Player::Render (src/_ZN6Player6RenderEv.cpp:80-83, 129-135) runs three
   TextureSequence::Update calls every frame, and this port's replacement
   render ran none of them, so every material a BTP drives stayed at its
   BMD-authored default forever. For Luigi that default is the WRONG variant:
   luigi_head_cap.bmd's head material is authored pointing at 'luigi_head_2'
   -- the vanish-power stipple -- and only the every-frame Update pulls it
   back to 'luigi_head_1' (L_tx_headtrans.btp frame 0). His body file is
   authored the other way round (default 'luigi_body_tr_1', the normal skin),
   which is why the body looked right while the head rendered see-through
   from the port's first boot. Measured: [btpprep] resolves the trans pairs at
   boot, and no [btp] material patch ever ran before this block existed.

   The three updates, the ROM's own gates and order:
     after the body render,  param1==1 (Luigi):  seq +0x254 -> body model
       (+0xe0, slot 1)'s components, then currFrame(+0x25c) = unk_6fc<<12
       (unk_6fc is the vanish-power progress; 0 in normal play = frame 0 =
       the normal texture)
     after the head render,  param1==1 (Luigi):  seq +0x268 -> head-with-cap
       (+0x158)'s components, then currFrame(+0x270) = unk_6fc<<12
     after the head render,  face timer +0x73c == 0 (func_ov002_020bea7c):
       the per-character face/blink seq (+0x1dc + char*0x14) -> the CURRENT
       head model's components (the eye-blink everyone gets, all characters)

   The null guards are port hygiene only (the ROM would crash there too); the
   gates and destinations are the matched TU's, byte for byte of intent. */
extern "C" void _ZN15TextureSequence6UpdateER15ModelComponents(void *seq,
                                                               void *mc);
extern "C" int func_ov002_020bea7c(char *self);

static void hal_player_texseq_body(char *c)
{
    if (*(int *)(c + 0x8) != 1) return;            /* param1: Luigi only */
    char *mdl = *(char **)(c + 0xe0);              /* body model, slot 1 */
    if (mdl)
        _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x254, mdl + 8);
    *(int *)(c + 0x25c) = (int)*(unsigned char *)(c + 0x6fc) << 12;
}

static void hal_player_texseq_head(char *c, unsigned hid)
{
    if (*(int *)(c + 0x8) == 1) {                  /* param1: Luigi only */
        char *mdl = *(char **)(c + 0x158);         /* head with cap */
        if (mdl)
            _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x268, mdl + 8);
        *(int *)(c + 0x270) = (int)*(unsigned char *)(c + 0x6fc) << 12;
    }
    if (func_ov002_020bea7c(c) == 0) {             /* face timer idle */
        char *head = ((char **)(c + 0x154))[hid];
        if (head)
            _ZN15TextureSequence6UpdateER15ModelComponents(
                c + 0x1dc + *(unsigned char *)(c + 0x6db) * 0x14, head + 8);
    }
}

void hal_render_player_world(void *player)
{
    char *c = (char *)player;

    /* ---- THE GATES Player::Render KEEPS, AND THIS DID NOT ------------------
     *
     * Run mg16 lane MP3, field failure 3. This function draws the body; the
     * SHADOW is a different path entirely -- Player::Behavior registers a
     * ShadowModel node (func_ov002_020e4bb8 -> func_ov002_020e444c ->
     * Actor::DropShadowRadHeight) and one global ShadowModel::RenderAll walk
     * draws every node. That path honours its OWN gates. This one honoured
     * none of src/_ZN6Player6RenderEv.cpp's, so the two could disagree about
     * whether a player should be drawn AT ALL -- and a caster whose body is
     * gated out is precisely "a shadow with no body".
     *
     * The four gates, in the ROM's own order (_ZN6Player6RenderEv.cpp:44-58):
     *   VS mode and this slot not live  -> not drawn
     *   the invincibility blink          -> not drawn on alternating frames
     *   fully transparent                -> not drawn  (NOT reproduced here,
     *                                       see the note at the gate: the
     *                                       field offset is unconfirmed and a
     *                                       guessed offset is worse than a
     *                                       named gap)
     *   mFlags & 0x10                    -> the ROM skips the body block
     *
     * The blink one matters most here: it is a PER-FRAME alternation, so a
     * body that ignores it while its shadow does not gives a shadow that
     * flickers away from a body that never does.
     *
     * NOT A NEW POLICY -- this is the matched TU's own logic, applied where the
     * port draws instead of it. hal_render_player_world exists because
     * Player::Render's vtable slot is a no-op in this port
     * (hal/level_boot.cpp's ps_render), so every gate that slot would have
     * applied has to be applied here or it is not applied anywhere. */
    {
        const unsigned char no = *(const unsigned char *)(c + 0x6d8);
        if (data_0209f2d8 == 1 && no < 4 && data_0209fc5c[no] == 0)
            return;
        /* THROUGH THE ACCESSOR, and this line is why the accessor block
           exists. It read 0x6a6 as a raw offset -- mStateWaitTimer, not
           mInvincibleTimer -- so the blink gate was culling the body on a
           timer that ticks constantly. One named constant, one place to fix,
           and the raw offset that hid the bug is gone. */
        const unsigned short inv = port::player::invincible_timer(c);
        /* The ROM picks bit 1 or bit 0 off Player::IsState(the hurt state).
           That state object is ov002's and is not reachable from here, so the
           port takes the conservative half: blink on bit 0, which is the arm
           the ROM uses everywhere except that one state. Stated rather than
           silently simplified. */
        if (inv & 1)
            return;
        /* mOpacity's offset is NOT confirmed from the header here, so the
           ROM's `if (mOpacity == 0) return` gate is deliberately NOT
           reproduced rather than guessed at an offset. Named so the gap is
           visible: a fully transparent player would still have its body
           drawn by this function. */
    }

    unsigned id = _ZNK6Player14GetBodyModelIDEjb(c, *(int *)(c + 8) & 0xff, 0);
    ModelAnim *ma = ((ModelAnim **)(c + 0xdc))[id];
    if (!ma) return;
    unsigned idx = ((unsigned short)*(short *)(c + 0x8e)) >> 4;
    /* THE ROM'S OWN MATRIX: rotation rows at 1.0 and the translation row in
       SCENE units, which is what an actor's Render writes and what
       Camera::Render's view matrix expects. `(v + 4) >> 3` is Camera::Render's
       own rounding for the same conversion.
       The x8 rotation rows this replaces were that same matrix carried into a
       world-unit frame; they came out the same size on screen and left Mario
       ~145 world units tall, which is what the migration has to preserve. */
    int s = data_02082214[idx * 2], co = data_02082214[idx * 2 + 1];
    int scene[12] = {co, 0, -s, 0, 0x1000, 0, s, 0, co,
                     (*(int *)(c + 0x5c) + 4) >> 3,
                     (*(int *)(c + 0x60) + 4) >> 3,
                     (*(int *)(c + 0x64) + 4) >> 3};
    for (int i = 0; i < 12; ++i) ((int *)&ma->mat4x3)[i] = scene[i];
    ma->ModelAnim::UpdateVerts();
    ma->ModelAnim::Render(0);
    hal_player_texseq_body(c);

    unsigned hid = func_ov002_020becf4(c, *(unsigned char *)(c + 0x6db), 1);
    if (hid != 8 && hid != 9) {
        char *head = ((char **)(c + 0x154))[hid];
        if (head) {
            hal_render_head_group(c, head, hid, ma, scene);
            hal_player_texseq_head(c, hid);
        }
    }

    /* THE WING MODEL: Player::mModelAnim4, EMBEDDED at +0x174.
       NOT the tongue, which is what an earlier pass here took it for. The gate
       says so outright -- func_ov002_020e4bb8 ends with the render latches

           *(u8*)(self + 0x6fe) = *(u8*)(self + 0x6fd);
           *(u8*)(self + 0x700) = *(u8*)(self + 0x6ff);   <- mHasWings

       so unk_700 is last frame's mHasWings, and the model it gates is the pair
       of wings. That also explains the Yoshi special case below: his wings
       hang off a different bone, with an offset and a quarter turn.

       Player::Render (src/_ZN6Player6RenderEv.cpp) draws it like this:

           if (unk_700 != 0) {
               ModelBase::ApplyOpacity(this + 0x174, mOpacity, 0);
               if (unk_6db == 3) {
                   Matrix4x3_FromTranslation(&data_020a0e68, -0x1b33, -0x666, 0);
                   Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, 0x4000);
                   MulMat4x3Mat4x3(&data_020a0e68, bodyBones + 0x180, &data_020a0e68);
                   mModelAnim4.Virtual18(&data_020a0e68, &mScale);
               } else {
                   mModelAnim4.Virtual18(bodyBones + 0x2d0, &mScale);
               }
           }

       and all four parts of that matter. An earlier pass here drew
       *(ModelAnim **)(c + 0x160) -- the pointer func_ov002_020d71ec SetAnims,
       which is NOT the object Render walks -- with the body's own scene matrix,
       through Render (slot 4), with no gate at all. What Tango saw was a
       model upside down under every character's feet, on Mario as much as on
       Yoshi: the probe said 198 polygons on frame 0 of a plain Mario boot,
       from the same object either way.

       THE TONGUE IS STILL NOT DRAWN. Removing this does not cost anything that
       was working -- the wings were never in the right place -- but it does not
       deliver the tongue either. Whatever draws the tongue, it is not
       mModelAnim4, and unk_700 never opens on a tongue (SM64DS_WINGS_PROBE=1
       through SM64DS_SELFTEST_TONGUE says the gate stays 0 the whole run).

       Faithful now: gated on unk_700, the ROM's own two matrices, and
       Virtual18 -- which is host slot 5 (_ZTV9ModelAnim is dtor 0, DoSetFile 1,
       UpdateVerts 2, Virtual10 3, Render 4, Virtual18 5; see
       hal/bob_enemy_bridges.cpp) and takes the matrix and the scale, unlike
       Render. The bone matrix is composed through `scene` the way the head at
       +0x154 is, because this path renders in scene space, not the ROM's
       world space. */
    {
        static int probe = -1;
        if (probe < 0) probe = std::getenv("SM64DS_WINGS_PROBE") ? 1 : 0;
        char *m4 = c + 0x174;
        unsigned char gate = *(unsigned char *)(c + 0x700);
        if (probe) {
            static int said, said_open;
            if (!said) {
                said = 1;
                std::fprintf(stderr, "[wings] +0x174 gate unk_700=%u "
                             "unk_6db=%u vptr=%p\n", gate,
                             *(unsigned char *)(c + 0x6db), *(void **)m4);
            }
            if (gate && !said_open) {
                said_open = 1;
                std::size_t n0 = 0, n1 = 0;
                ntr::gx_polygons(n0);
                std::fprintf(stderr, "[wings] gate OPENED (unk_700=%u, "
                             "unk_6db=%u, step=%u)\n", gate,
                             *(unsigned char *)(c + 0x6db),
                             *(unsigned char *)(c + 0x6e3));
                (void)n0; (void)n1;
            }
        }
        if (gate != 0) {
            char *bones = *(char **)((char *)ma + 0x14);
            int composed[12];
            const int *src;
            if (*(unsigned char *)(c + 0x6db) == 3) {
                Matrix4x3_FromTranslation(&data_020a0e68, -0x1b33, -0x666, 0);
                Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, 0x4000);
                MulMat4x3Mat4x3(&data_020a0e68, bones + 0x180, &data_020a0e68);
                src = (const int *)&data_020a0e68;
            } else {
                src = (const int *)(bones + 0x2d0);
            }
            m43_mul(src, scene, composed);
            ((void(__fastcall *)(void *, void *, const void *))(
                ((void ***)m4)[0][5]))(m4, composed, c + 0x80);
        }
    }
}

void hal_render_player_body_ex(void *player, int with_head)
{
    char *c = (char *)player;
    unsigned id = _ZNK6Player14GetBodyModelIDEjb(c, *(int *)(c + 8) & 0xff, 0);
    ModelAnim *ma = ((ModelAnim **)(c + 0xdc))[id];
    if (!ma) return;
    for (int i = 0; i < 12; ++i) ((int *)&ma->mat4x3)[i] = 0;
    ((int *)&ma->mat4x3)[0] = 0x1000;
    ((int *)&ma->mat4x3)[4] = 0x1000;
    ((int *)&ma->mat4x3)[8] = 0x1000;
    ma->ModelAnim::UpdateVerts();
    ma->ModelAnim::Render(0);
    hal_player_texseq_body(c);

    /* the head is its own model; Player::Render seats it by copying the
       body's neck-bone matrix (+0x2d0 in the bone array) into the head's
       matrix slot, then renders through the object's own vtable */
    unsigned hid = func_ov002_020becf4(c, *(unsigned char *)(c + 0x6db), 1);
    if (with_head && hid != 8 && hid != 9) {
        char *head = ((char **)(c + 0x154))[hid];
        if (head) {
            char *src = *(char **)((char *)ma + 0x14) + 0x2d0;
            /* host Model::Render consumes mat4x3; seat the neck transform
               there (the game writes the bone array, but writing both
               double-transforms on host) */
            if (src)
                for (int i = 0; i < 12; ++i)
                    ((int *)(head + 0x1c))[i] = ((const int *)src)[i];
            ((void(__fastcall *)(void *, void *, const void *))(
                ((void ***)head)[0][4]))(head, 0, 0);
            hal_player_texseq_head(c, hid);
        }
    }
}

/* State-machine dispatch: the State objects come from the overlay image
   with DS code addresses baked into their function slots (mwcc PMFs). The
   host ChangeState (port/unmatched/Player_ChangeState.cpp) routes every
   dispatch here; the table grows one line per state the slice hosts.
   Unknown state = loud no-op success so the boot path keeps moving. */
extern "C" int _ZN6Player18St_LevelEnter_MainEv(int *c);
extern "C" int _ZN6Player12St_Jump_InitEv(char *c);
extern "C" void func_ov002_020e200c(char *c);
extern "C" void func_ov002_020c9c4c(char *c);
extern "C" void func_ov002_020c9de4(char *c);
extern "C" void func_ov002_020c9d68(char *c);
extern "C" void func_ov002_020c9d1c(char *c);
extern "C" void func_ov002_020c9cc0(char *c);
extern "C" void func_ov002_020c9c00(char *c);
extern "C" void func_ov002_020c9b7c(char *c);
extern "C" void func_ov002_020c9b5c(char *c);
extern "C" void func_ov002_020c9ac0(char *c);
extern "C" void func_ov002_020c9b10(char *c);
extern "C" void func_ov002_020c9a04(char *c);
extern "C" void func_ov002_020c9998(char *c);
extern "C" void func_ov002_020c990c(char *c);
extern "C" void func_ov002_020c98a4(char *c);
extern "C" void func_ov002_020c9840(char *c);
extern "C" void func_ov002_020c97f8(char *c);
extern "C" void func_ov002_020c97e0(char *c);
extern "C" void func_ov002_020e1e70(char *c);
extern "C" void func_ov002_020e1c20(char *c);
/* Three state slots the ROM fills with plain ov002 functions rather than
   Player methods: Null's Init, WallJump's Init, InYoshiMouth's Cleanup.
   The community St_ names at those addresses belong to ov006, not ov002. */
extern "C" int func_ov002_020cac30(void);
extern "C" int func_ov002_020d6084(char *c);
extern "C" int func_ov002_020e17f8(void *c);
extern "C" int _ZN6Player16St_BurnLava_MainEv(char *c);
/* gate 14: the level-boot state and the seven entrance-step handlers */
extern "C" int func_ov002_020c6f3c(void *c);
extern "C" void func_ov002_020c75f0(char *c);
extern "C" void func_ov002_020c7350(char *c);
extern "C" void func_ov002_020c71e0(char *c);
extern "C" void func_ov002_020c7194(char *c);
extern "C" void func_ov002_020c72a4(void *c);
extern "C" void func_ov002_020c70ac(char *c);
extern "C" void func_ov002_020c6fe4(char *c);
/* SM64DS_TRACE_STATE: every state function the dispatcher runs. The state
   machine is otherwise opaque from outside -- the Player carries a State* and
   the port switches on the DS address inside it -- and this is how the water
   test reads: walking into the moat has to show 0x020ce550 (St_Swim_Init) and
   then 0x020cd94c (St_Swim_Main), and climbing out has to come back through
   0x020cd1e4 (St_Swim_Cleanup) to St_Walk.
     =1  once per distinct DS address (the quiet census)
     =2  on every CHANGE, with the Player's height, which is what a
         walk-in/swim-across/climb-out run has to be read from */
/* ST_CLIMB's two host entries, port/unmatched/Player_St_Climb.cpp */
extern "C" int port_player_st_climb_init(void *self);
extern "C" int port_player_st_climb_main(void *self);
/* Player::St_EndingFly_Main, under the flat name the ov002 world gives it
   (the sinit's PMF table pairs it with the EndingFly state; see the case). */
extern "C" int func_ov002_020c3d1c(char *self);

extern "C" int hal_call_state_fn(void *self, unsigned ds_addr)
{
    {
        static int on = -1;
        if (on < 0) {
            const char *e = std::getenv("SM64DS_TRACE_STATE");
            on = e ? std::atoi(e) : 0;
            if (e && !on) on = 1;
        }
        if (on >= 2) {
            static unsigned prev;
            if (ds_addr != prev) {
                prev = ds_addr;
                std::printf("  [state] 0x%08x y=%.1f\n", ds_addr,
                            *(int *)((char *)self + 0x60) / 4096.0);
            }
            /* =3: the LevelEnter entrance-step spin, byte by byte. The
               step exits on FinishedAnim with the hold byte clear, so
               those two plus the mode/step pair are the whole question. */
            /* =3: the LevelEnter entrance-step spin, whichever step it is.
               Every exit gate the seven step handlers use is here: the hold
               byte, the global countdown, the no-control timer, and the
               animation's cursor against its length. */
            if (on >= 3 &&
                (ds_addr == 0x020c75f0 || ds_addr == 0x020c7350 ||
                 ds_addr == 0x020c71e0 || ds_addr == 0x020c7194 ||
                 ds_addr == 0x020c72a4 || ds_addr == 0x020c70ac ||
                 ds_addr == 0x020c6fe4)) {
                extern int data_0209f2bc[];
                char *c = (char *)self;
                const unsigned id =
                    _ZNK6Player14GetBodyModelIDEjb(c, *(int *)(c + 8) & 0xff, 0);
                char *anim = *(char **)(c + 0xdc + id * 4) + 0x50;
                std::fprintf(stderr,
                             "  [enter] fn=%08x mode=%u step=%u hold6de=%u "
                             "f2bc=%u hold6a6=%u cur=%d nff=0x%08x fin=%d\n",
                             ds_addr,
                             *(unsigned char *)(c + 0x6e3),
                             *(unsigned char *)(c + 0x6e5),
                             *(unsigned char *)(c + 0x6de),
                             *(unsigned char *)data_0209f2bc,
                             port::player::invincible_timer(c),
                             *(int *)(anim + 8), *(unsigned *)(anim + 4),
                             ((Animation *)anim)->Animation::Finished());
            }
        } else if (on) {
            static unsigned said[64];
            int seen = 0;
            for (int i = 0; i < 64 && said[i]; ++i)
                if (said[i] == ds_addr) seen = 1;
            if (!seen) {
                for (int i = 0; i < 64; ++i)
                    if (!said[i]) { said[i] = ds_addr; break; }
                std::printf("  [state] 0x%08x\n", ds_addr);
            }
        }
    }
    switch (ds_addr) {
#include "player_states.inc"
    /* run linkw / lane l5: the last state-fn addresses in ov002's symbol
       table that the generated .inc does not carry. All three are matched
       src TUs, seated through port/slice_w1l5.txt, and all three are real
       __thiscall methods, so the call goes through the Player class exactly
       the way every method case in the .inc above does.

       Addresses read out of config/arm9/overlays/ov002/symbols.txt, not
       guessed from a neighbouring case:
         _ZN6Player17St_EndingFly_InitEv       0x020c3d6c
         _ZN6Player22St_SwingPlayer_CleanupEv  0x020d9fc4
         _ZN6Player19St_SwingPlayer_InitEv     0x020da3b0

       SwingPlayer still has an unhosted Main, and that is a stated hole
       rather than an oversight: St_SwingPlayer_Main 0x020d9fec has no
       matched src TU at all, and the miss keeps announcing itself through
       g_port_unhosted_hits below. Climb is the worked example of what a
       silently skipped state body costs (see port/unmatched/
       Player_St_Climb.cpp -- the anim never changes and the entry speed is
       never zeroed, which reads as a freeze that then slides).

       EndingFly's Main is NOT the ov007-named body an earlier version of
       this comment blamed. The ov002 sinit's own PMF state table pairs the
       EndingFly state object (0x0211058c) with func_ov002_020c3d1c: the
       wave-2 mount lane derived that from the relocation triple, and its
       reviewer re-derived it by reconstructing all 81 state objects from
       the sinit's store sequence. The ov007 body at the same address is a
       different overlay's function that 86 ov007-internal calls reach and
       no PMF cell anywhere names. The case below dispatches the ov002 body;
       its data (the 27-record rising-spiral step table data_ov002_0210a8b8
       and the kuppa script data_02088610) rode in with the wave-2 mounts. */
    case 0x020c3d1c: return func_ov002_020c3d1c((char *)self);
    case 0x020c3d6c: return ((Player *)self)->Player::St_EndingFly_Init();
    case 0x020d9fc4: return ((Player *)self)->Player::St_SwingPlayer_Cleanup();
    case 0x020da3b0: return ((Player *)self)->Player::St_SwingPlayer_Init();
    }
    /* Every miss here is a state the port silently does not run, and the only
       record of it used to be a line in the flight recorder that nobody reads
       until after the glitch. The counter is what the F3 overlay shows live,
       so a state hole announces itself while it is happening. */
    ++g_port_unhosted_hits;
    std::fprintf(stderr, "  [state] unhosted state fn 0x%08x (no-op)\n",
                 ds_addr);
    return 1;
}

/* ---- LIVE CHARACTER SWAP (port mod) ------------------------------------
   The mod is the ENTRY POINT, not the swap. Player::SetRealCharacter is the
   game's own permanent change -- the one the character doors call -- and it is
   already instant: it starts the hat morph and then writes unk_73c = 0, which
   cancels the morph's state machine before a single stage of it runs. So there
   is nothing to reimplement here and nothing to animate away. What the port
   owes it is a caller with guards, because the ROM only ever reaches it from
   one place that has already checked everything.

   All four body and head models are resident from InitResources
   (func_ov002_020e5948), so no model loading happens. One thing does load: the
   BCA for (current animation, new character). mCharFileBase is animId * 4, not
   a per-character base, so ANIM_PTRS[base + chr] is a 2D lookup and only the
   current row's Mario column is resident at boot.

   Return codes exist so the caller can print WHY nothing happened rather than
   looking broken: 0 done, 1 no player, 2 out of range, 3 already that one. */
extern void *data_0209f394[];        /* per-player Actor*, cxxname_bridge's */
extern short data_02092144[];        /* per-player health, high byte = HP */
extern void *data_ov002_020ff480[];  /* ANIM_PTRS[anim*4 + char] */
extern signed char data_02092114;    /* queued in-level swap, -1 = none */
void _ZN10ModelAnim24CopyERKS_Pcj(void *self, const void *src, char *nf,
                                  unsigned nof);

/* THE DOOR SWAP, operating on a given player. This is the game's own in-place
   change (Player::SetRealCharacter, the path the character doors take), which
   loads the incoming character's files through data_ov002_020ff480 and arms the
   hat morph BEFORE it moves param1, then cancels the morph so the change is
   instant. It replaces nothing about the Player except who it is -- the model,
   the animation cursor, the state and the position all carry across untouched,
   so there is no cold restart and no stand-freeze. Returns the same codes as
   port_set_character (0 done, 1 no player, 2 out of range, 3 already that one).
   port_set_character wraps this on the local player; port_player_set_character
   routes to it as the default swap. */
static int port_door_swap(char *c, int chr)
{
    if (!c) return 1;
    /* SetRealCharacter masks with & 3 in two places but uses chr RAW in the
       other two (the ANIM_PTRS index and the chr + 0xc4 default-anim index),
       so an out-of-range value would read past the table into whatever ov002
       packed next. Reject it -- masking would silently hand back a different
       character than the caller asked for. */
    if (chr < 0 || chr > 3) return 2;
    /* the body ModelAnim2 slots: null until InitResources has run, and
       SetRealCharacter dereferences one with no check of its own */
    if (!*(void **)(c + 0xdc)) return 1;
    if ((int)(*(unsigned *)(c + 8) & 0xff) == chr) return 3;

    /* A cap collect in flight is CANCELLED, not refused. Cancelling is what an
       instant swap means, and refusing would be a trap: the debug menu pauses
       the tick, so unk_73c can never advance while someone is holding the menu
       open. Say it happened so an odd-looking result is attributable. */
    if (((Player *)c)->Player::IsCollectingCap())
        std::fprintf(stderr, "[chr] swap during a cap collect (unk_73c=%04x);"
                             " the morph is cancelled\n",
                     *(unsigned short *)(c + 0x73c));

    /* SetRealCharacter ends in Heal(0x880) unconditionally. Save the whole
       short, not just the HP byte -- the low byte is a fraction GiveHealth
       works in. Without this every swap visibly refills the HUD hearts, which
       makes the row useless for anything you were testing damage against. */
    const unsigned pno = *(unsigned char *)(c + 0x6d8) & 3;
    const short saved_hp = data_02092144[pno];
    /* which body model is animating RIGHT NOW, before param1 moves */
    const unsigned old_id =
        _ZNK6Player14GetBodyModelIDEjb(c, *(unsigned *)(c + 8) & 0xff, 0);

    ((Player *)c)->Player::SetRealCharacter((unsigned)chr);

    /* KNOWN, LEFT ALONE DELIBERATELY: Luigi's head draws with the toon/flash
       shading the power flower uses. The swap does not turn it on -- unk_73c
       = 0 inside SetRealCharacter cancels func_ov002_020be3b0 before stage 1's
       SetPolygonMode(m, 2) can run, which is what makes the swap instant. It
       means stage 7, the only thing that turns the flash OFF, never runs
       either, so a model already in that polygon mode keeps it. A
       Player::TurnOffToonShading(chr) here clears it, but that is a rendering
       question about Luigi's head model rather than part of the swap, and it
       has not been eyeballed. Chip it separately. */

    /* THE OTHER THING THE ROM'S CALLER ALREADY HAD RIGHT. data_02092114 is the
       QUEUED in-level swap -- the file-select picker writes the character it
       wants and func_ov002_020be008 performs it on the next landing, spawning
       the cap actor and freezing the player in a no-control state while the
       poof plays. -1 means "nothing queued", and the port never seats it: it
       is plain zero-initialised storage, which reads as "a swap to Mario is
       pending". Inert for as long as you ARE Mario, which is why nothing has
       ever noticed. The moment param1 becomes anything else the poll fires and
       drags you straight back, ~28 frames after the swap, through a state the
       port does not host -- so you end up frozen as the character you asked
       for. SetRealCharacter is a direct swap, so the honest value afterwards is
       the one 020be008 itself writes when it has consumed the request. */
    data_02092114 = -1;

    data_02092144[pno] = saved_hp;
    /* The body renders off param1, which the call above wrote, but the HEAD
       renders off unk_6db, and the only thing that re-syncs unk_6db from
       param1 is the tail of func_ov002_020e4bb8 -- inside Player::Behavior.
       With the menu open the tick is skipped, so without this the new body
       wears the old head for exactly as long as somebody is looking at it.
       This is that same assignment, one tick early, and it is idempotent once
       the tick resumes. */
    *(unsigned char *)(c + 0x6db) = (unsigned char)chr;

    /* THE ONE THING SetRealCharacter LEAVES TO ITS CALLER, and the reason a
       swap that looked fine faulted on the next tick.

       Its tail is ModelAnim2::Copy(body[m1], body[m2], newFile, 0) -- but it
       assigns param1 = chr BEFORE computing both ids, so m1 == m2 and the copy
       is a model onto itself. That cannot carry an animation across, and
       body[chr] has no base Animation to begin with: InitResources
       (func_ov002_020e5948) seats only otherAnim for all four models and the
       base Animation for whoever you actually are. So the new model comes out
       with numFramesAndFlags == 0, and Animation::Advance's `% len` on the
       very next Player_AdvanceAnims is an integer divide by zero.

       On the ROM this is invisible, because SetRealCharacter has exactly one
       caller -- the character door -- and the door's state immediately calls
       Player::SetAnim with a DIFFERENT animation id, which takes SetAnim's
       slow path and seats the frame count off the new file. A swap that does
       not change animation never gets that. Calling SetAnim here would not
       help either: ModelAnim::SetAnim fast-paths when the file pointer already
       matches, and SetRealCharacter's own Copy already stored it.

       So do what the hat morph does. This is func_ov002_020be3b0's stage-3
       call with the operands it has: dst != src, which carries the live
       cursor, speed and frame count off the character who was walking a moment
       ago onto the one who is now. */
    {
        const unsigned base = *(unsigned *)(c + 0x63c);
        const unsigned new_id =
            _ZNK6Player14GetBodyModelIDEjb(c, (unsigned)chr, 0);
        void *dst = *(void **)(c + 0xdc + new_id * 4);
        void *src = *(void **)(c + 0xdc + old_id * 4);
        /* SharedFilePtr's layout is not recovered; +4 is the file pointer every
           caller in src/ indexes, SetRealCharacter's own Copy argument included.
           It is a real load off the file seam, so it can come back empty. */
        char *bca = *(char **)((char *)data_ov002_020ff480[base + chr] + 4);
        if (dst && src && dst != src && bca)
            _ZN10ModelAnim24CopyERKS_Pcj(dst, src, bca, 0);
        /* and say so if it still came out unplayable, rather than leaving the
           divide to announce it */
        if (!bca || !dst ||
            (*(unsigned *)((char *)dst + 0x54) & ~0xc0000000u) == 0)
            std::fprintf(stderr, "[chr] character %d came out with no playable "
                                 "body animation (anim base %u, file %p) -- "
                                 "Advance will divide by zero\n",
                         chr, base, (void *)bca);
    }
    return 0;
}

/* the local-player wrapper the debug menu reached for first */
int port_set_character(int chr)
{ return port_door_swap((char *)data_0209f394[0], chr); }

void _ZN6Player16InitWingFeathersEb(void *self, unsigned char b_)
{ ((Player *)self)->Player::InitWingFeathers(b_ != 0); }
extern "C++" int ApproachLinear(int &ref, int target, int step);
int _Z14ApproachLinearRiii(int *ref, int target, int step)
{ return ApproachLinear(*ref, target, step); }
int hal_anim_willhit(void *self, int f)
{ return ((Animation *)self)->Animation::WillHitFrame(f) ? 1 : 0; }
int hal_nhi_next(void *self, void *h)
{ return ((NestedHeapIterator *)self)->NestedHeapIterator::Next(
      (HeapAllocator *)h); }

void _ZN9Animation7AdvanceEv(void *self)
{ ((Animation *)self)->Animation::Advance(); }
int _ZN9Animation8FinishedEv(void *self)
{ return ((Animation *)self)->Animation::Finished(); }
char *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp)
{
    if (!fp) {
        std::fprintf(stderr, "  [anim] LoadFile on NULL fileptr (table hole)\n");
        return 0;
    }
    return Animation::LoadFile(*(SharedFilePtr *)fp);
}

int _ZN6Player6IsAnimEj(void *self, unsigned a)
{ return ((Player *)self)->Player::IsAnim(a); }
int _ZN6Player12FinishedAnimEv(void *self)
{ return ((Player *)self)->Player::FinishedAnim(); }
int _ZN6Player17SetNoControlStateEhih(void *self, unsigned char a, int b,
                                      unsigned char c)
{ return ((Player *)self)->Player::SetNoControlState(a, b, c); }
int _ZN6Player8HasNoCapEv(void *self)
{ return ((Player *)self)->Player::HasNoCap(); }
int _ZN6Player9GetHealthEv(void *self)
{ return ((Player *)self)->Player::GetHealth(); }

void _ZN4BgCh19StartDetectingWaterEv(void *self)
{ ((BgCh *)self)->BgCh::StartDetectingWater(); }

/* THE CYLINDER SHADOW IS NO LONGER DEFERRED (run linkw wave 4, lane w4-a).
   This was `void _ZN11ShadowModel12InitCylinderEv(void *) {}`, the stub that
   kept the matched body out of the image; it is now the ordinary C-name-to-
   method bridge, the same shape as _ZN11ShadowModel10InitCuboidEv in
   hal/cxxname_bridge.cpp, and the body it reaches is the matched
   src/_ZN11ShadowModel12InitCylinderEv.cpp.

   The three things that had to be true first, and are:
     - the template BMD at data_020ad560 is static .data in overlay 1, 0x3c
       bytes with one bone at 0x020ad5dc and one material at 0x020ad4c4, and
       wave 3 named that chain in port/ov001_syms.txt;
     - the host zero copy of data_020ad560 that would have shadowed the mount
       is gone from hal/actor_vtables.cpp, which now aliases the arm9 spelling
       onto the ov001 one;
     - _ZTV11ShadowModel[1] holds ShadowModel::DoSetFile (hal/model_dtor_seat
       .cpp, seated in wave 1 and called from port_stage_a2_seat), so the
       vtable dispatch ModelBase::SetFile makes lands on a real body.

   IT RETURNS int, AND THE VALUE IS DERIVED RATHER THAN CHOSEN. The ROM's
   InitCylinder is a TAIL BRANCH into SetFile, which tail-branches into
   DoSetFile, so r0 leaves this function carrying DoSetFile's result -- and
   BobOmb::InitResources tests it (`if (InitCylinder() == 0) return 0;`, see
   the ride-through block in hal/bob_enemy_bridges.cpp). The matched
   ShadowModel::InitCylinder and ModelBase::SetFile are both declared void by
   the decomp, so the value cannot ride out of a host call the way it rides out
   of the ARM one. It does not have to be guessed at either: matched
   src/_ZN11ShadowModel9DoSetFileEPcii.cpp returns 0 exactly when it has just
   stored data = 0 and 1 in every other path, so `data != 0` read back off the
   object after the call IS that return value, not a stand-in for it.

   THE SEAT CHECK IS NOT DEFENSIVE PROGRAMMING, IT IS A MEASURED HARNESS GAP,
   and it is the wave-3 abort-vs-no-op lesson (hal/cxxname_bridge.cpp) landing
   in a new place. The matched body's SetFile dispatches DoSetFile through
   _ZTV11ShadowModel[1], which hal/model_dtor_seat.cpp seats inside
   hal_seat_model_family_dtors -- and that runs from port_stage_a2_seat, so
   walk_window and walk_window_hires have it from process start. smoke_player
   does NOT: it hand-fills the host vtables and calls hal_fill_shadow_vtable,
   which by its own comment seats slot 0 only. Measured, not assumed -- with
   the bridge unguarded, smoke_player faults c0000005 accessing 0 with
   ?InitCylinder@ShadowModel@@QAEXXZ + 0xe on the stack under
   func_ov002_020e5948 + 0x6d7 (Mario's own drop shadow, Player + 0x2ac), while
   walk_window runs 1000 frames through that same call.

   So the guard fires only where the host never filled the slot, and there it
   reproduces that harness's baseline exactly: an InitCylinder that installs
   nothing, which is what the stub gave smoke_player for its whole green
   history. It returns 0 for the same reason the line below returns data != 0
   -- no DoSetFile ran, so no data was attached -- and the one caller in that
   target ignores the value anyway (port/unmatched/TexSeq_Caller_ov002_
   020e5948.cpp declares this void). It complains on stderr the first time
   rather than staying silent, because in a target that DOES seat the slot the
   same line would mean the seat had regressed and the shadows had stopped
   drawing. The real fix is one call in tests/smoke_player.cpp
   (hal_seat_model_family_dtors, beside the four hal_fill_*_vtable calls) or
   one line in hal/cxxname_bridge.cpp's hal_fill_shadow_vtable; neither file is
   this lane's. */
int _ZN11ShadowModel12InitCylinderEv(void *self)
{
    void **vt = *(void ***)self;
    if (!vt || !vt[1]) {
        static int told;
        if (!told) {
            told = 1;
            std::fprintf(stderr, "InitCylinder: _ZTV11ShadowModel[1] "
                         "(DoSetFile) is null in this target -- no shadow "
                         "installed\n");
        }
        return 0;
    }
    ((ShadowModel *)self)->ShadowModel::InitCylinder();
    return ((ShadowModel *)self)->data != 0;
}

void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *self, void *bmd,
                                                       void *btp)
{ ((TextureSequence *)self)->TextureSequence::Prepare(*(BMD_File *)bmd,
                                                      *(BTP_File *)btp); }
void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *fp)
{ return TextureSequence::LoadFile(*(SharedFilePtr *)fp); }

void _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(void *self, void *node)
{ ((NestedHeapIterator *)self)->NestedHeapIterator::Remove(
      (HeapAllocator *)node); }
int _ZN18NestedHeapIterator8PreviousEP13HeapAllocator(void *self, void *h)
{ return ((NestedHeapIterator *)self)->NestedHeapIterator::Previous(
      (HeapAllocator *)h); }

int _ZN4Heap6RescueEv(void *self)
{ return ((Heap *)self)->Heap::Rescue(); }
int _ZN4Heap21MaxAllocationUnitSizeEv(void *self)
{ return ((Heap *)self)->Heap::MaxAllocationUnitSize(); }
int _ZN4Heap6IntactEv(void *self)
{ return ((Heap *)self)->Heap::Intact() ? 1 : 0; }

/* gate-10 BSS, second ring */
/* data_0209b44c is NOT here: it is the one-byte area id, hosted in
   hal/actor_vtables.cpp so the smoke targets that link no player bridge
   still get it, and so writer and reader share one object. */
DSSTATE_BEGIN
int data_0208e428[8], data_0209b480[4];
int data_020a4d60[8], data_020a6438[8], data_020a6488[4], data_020a648c[4];
int data_020a6490[4], data_020a649c[4], data_020a64a0[4], data_020a64a4[4];
/* The sound command queue's two SIZED objects. Placeholders while sound was
   stubbed; the hosted ARM7 in hal/sdat/ needs their real extents.
   data_020a6760 is the node pool func_0205b070 cache-flushes as 0x1800
   bytes = 256 nodes x 0x18, and the seeding chains all 256 of them.
   data_020a64a8 is the batch ring that func_0205b070 and func_0205b274 index
   with "idx++; if (idx > 8) idx = 0", so it needs nine slots. At the old
   sizes, seeding the pool walked off the end of a 32-byte object. */
void *data_020a64a8[16];
int data_020a6760[256 * 0x18 / sizeof(int)];
int data_020a0f1c[4], data_020a4d54[4], data_020a6440[4], data_020a6444[4];
int data_020a6484[4], data_020a6494[4], data_020a6498[4];
int data_0209cdd0, data_0209cdd4, data_0209cdd8, data_0209cddc, data_0209cde0;
int data_0209f220[8], data_0209f264[8], data_020a0d90[8], data_020a0f38[8];
int data_020a4b58[4], data_020a4b68[4], data_020a60f4[4];
/* DTCM, AND IT IS 16 KB AND NOT 64 BYTES. This used to read "DTCM scratch the
   timer list walker anchors at" and be sized for that one reader, which is the
   undersized-hosted-global shape: a span decided by the first caller found
   rather than by the ROM.
   THE SPAN IS THE ARM9's DATA TCM, 0x023c0000..0x023c4000, and what fixes the
   size is the BIOS interrupt check flag at the top of it, DTCM_END - 8, the
   NitroSDK OSi_IrqCheckFlag slot. FIVE handlers in src write that word by
   literal offset, and only two of them are in this link:
       func_0202f2c4            |= 2       slice_fdr, AND REACHED, 191x a frame
       func_02059834            |= 0x10    slice_gate10, linked and undriven
       _ZN3IRQ13VBlankHandlerEv |= 1       not compiled
       _ZN3IRQ13DmaTimHandlerEv |= mask    not compiled
       func_ov006_020efcf8      |= 2       not compiled
   At 64 bytes that store landed 16,312 bytes past the object, 56 bytes into
   _hal_area_table and so INSIDE .dsstate, which means a save state would have
   captured the corruption and a restore replayed it. It was invisible only
   because no host path had ever RUN one of the five: the ntr layer stored no
   handler for mask 1, 2 or 0x10, so none was ever dispatched. Raising IRQ 2
   drives the first one for real, and the second goes live the day anyone
   models timer interrupts, so the span has to be real before either.
   See port/irq2_map.txt section 8. */
__declspec(align(8)) unsigned char data_023c0000[0x4000];
int data_02099e94[4], data_02099ebc[4], data_02099ec4[4], data_02099fcc[4];
/* data_020a6088 is NOT here any more. It is the head of the GX bank-state
   block, whose members the SetBankFor* family reaches by STRUCT OFFSET out to
   +0x18 and which func_02053d9c clears 26 bytes at a time, and the `int[2]`
   that used to sit here was both too small for that and not adjacent to the
   twelve u16 symbols the ROM has after it. Eight bytes was already generous:
   the symbol's own span in config/arm9/symbols.txt is two. The whole band is
   laid out in ROM order in hal/cxx_aliases.cpp; read the banner there. */
int data_020a6084[4], data_020a8114[4];
DSSTATE_END

}  /* extern "C" */

/* ---- THE GAME'S OWN CHARACTER CHANGE ---------------------------------------
   Player::SetRealCharacter is the cap-block path, the thing that runs when you
   break a Luigi or Wario cap, hat animation and all. It is also the only
   correct way to change character in place, because it does the part a bare
   index write cannot: it Releases the outgoing character's file, LoadFiles the
   incoming one, re-points the ModelAnim2 at it, swaps the voice bank through
   func_ov002_020e6330 and writes the save byte at data_0209caa0[0x41] so the
   choice survives. All of it is matched src already in the slices, gate 22 for
   this and gate 10 for the rest.

   THIS WRAPPER EXISTS ONLY FOR THE CALLING CONVENTION. The Itanium name is
   /alternatename'd onto ?SetRealCharacter@Player@@QAEXI@Z, which is
   __thiscall, so calling it through the C name from a TU without Player.h
   passes `this` on the stack and leaves ecx holding whatever was there. That
   is the garbage-ecx trap the port has already been bitten by three times.
   Here the real header is in scope and the call is an ordinary method call. */
extern "C" { extern void *data_ov002_020ff480[]; }

/* THE LEGACY SWAP: rebuild the whole Player by re-running InitResources with a
   rewritten spawn param. It works, but it is a COLD restart -- InitResources
   re-runs func_ov002_020e5948 (the model/anim resource init), re-seats the
   collision, and re-places the Player at the entrance, so the animation system
   comes back at frame 0 and the Player stands frozen for a second or two while
   it settles. That freeze is the whole reason for the door path below. Kept
   behind SM64DS_SWAP_LEGACY=1 as a fallback while the door path proves out.

   The param's layout is InitResources' own unpacking, read back out of the
   fields it wrote: bits 0-2 character, 3-5 the sub value at +0x6da, 6-7 the
   entrance index at +0x6d8. Bits 8+ feed func_ov002_020c7dd0's entrance type,
   which a mid-level swap has no business re-running, so they go in as 0.
   Position, angle and speed are carried across because InitResources places the
   Player at the entrance and the point is to change who you are, not where. */
static void port_legacy_set_character(void *player, unsigned ch)
{
    char *c = (char *)player;
    int pos[3], spd[4];
    short ang;
    unsigned param;

    ch &= 3;
    pos[0] = *(int *)(c + 0x5c);
    pos[1] = *(int *)(c + 0x60);
    pos[2] = *(int *)(c + 0x64);
    ang = *(short *)(c + 0x8e);
    spd[0] = *(int *)(c + 0x98);
    spd[1] = *(int *)(c + 0xa4);
    spd[2] = *(int *)(c + 0xa8);
    spd[3] = *(int *)(c + 0xac);

    param = ch | ((unsigned)*(unsigned char *)(c + 0x6da) << 3) |
            ((unsigned)*(unsigned char *)(c + 0x6d8) << 6);
    *(int *)(c + 8) = (int)param;
    *(unsigned char *)(c + 0x6d9) = (unsigned char)ch;

    _ZN6Player13InitResourcesEv(c);

    *(int *)(c + 0x5c) = pos[0];
    *(int *)(c + 0x60) = pos[1];
    *(int *)(c + 0x64) = pos[2];
    *(short *)(c + 0x8e) = ang;
    *(int *)(c + 0x98) = spd[0];
    *(int *)(c + 0xa4) = spd[1];
    *(int *)(c + 0xa8) = spd[2];
    *(int *)(c + 0xac) = spd[3];
}

/* CHANGE CHARACTER ON THE SPOT. Every entry point in the port -- F4, the debug
   menu row, SM64DS_SWITCH, SM64DS_SELFTEST_SWAP -- comes through here, so the
   default swap path is chosen in one place.

   THE DEFAULT IS NOW THE DOOR PATH (port_door_swap). It is the game's own
   in-place change: it loads the incoming character's files through
   data_ov002_020ff480 and arms the hat morph engine (func_ov002_020be3b0, which
   Player::Behavior already ticks every frame) BEFORE it moves param1, then
   cancels the morph so the change is instant. The model, the animation cursor,
   the current state and the position all carry across, so there is no cold
   restart and no stand-freeze -- which is the whole point of preferring it.

   The old worry was that the door path could not do Yoshi, because
   SetRealCharacter arms the CAP morph and there is no Yoshi cap, leaving a
   zero-length Animation that divides by zero on the next Advance. port_door_swap
   already closes that: after SetRealCharacter it does the hat morph's own
   stage-3 ModelAnim::Copy with dst != src, carrying the live cursor and frame
   count off the outgoing character onto the incoming one, so the new body has a
   playable animation whether or not a cap was involved.

   SM64DS_SWAP_LEGACY=1 forces the old InitResources rebuild for an A/B. */
extern "C" void port_player_set_character(void *player, unsigned ch)
{
    static int legacy = -1;
    if (legacy < 0) legacy = std::getenv("SM64DS_SWAP_LEGACY") ? 1 : 0;
    if (legacy) { port_legacy_set_character(player, (int)(ch & 3)); return; }

    int rc = port_door_swap((char *)player, (int)(ch & 3));
    /* the door path can decline (no player, already that character); the codes
       are informational, but on an outright "no player" fall back to the legacy
       rebuild rather than leaving the caller with nothing happening. */
    if (rc == 1)
        port_legacy_set_character(player, (int)(ch & 3));
}
