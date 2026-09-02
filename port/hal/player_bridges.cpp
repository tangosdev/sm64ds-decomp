// Gate-10 method bridges: C-named references -> MSVC method definitions.
//
// The Player closure's callers reference these at C linkage while the
// defining TUs compile them as real methods against the shared headers.
// Same hop as gate 9 (cxxname_bridge.cpp), split into its own TU because
// Player.h drags a wider include surface than the gate-9 file wants.
#include <cmath>
#include "vs_width.h"   /* 0.3.2: the port's player width */
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Animation.h"
#include "BgCh.h"
#include "NestedHeapIterator.h"
#include "Player.h"
#include "player_fields.h"   /* run mg16 lane MP4: the one place field offsets live */
#include "host_settings.h"   /* port::adventure_ghost_mode() for the ghost pass */
#include "comms_seam.h"      /* port::sync_stats(): the local-write witness */
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
   head+0x1c to the player-root matrix by the time render runs. So the i==3 arm
   dispatches base Model::Render with nothing seated, exactly as the ROM does.

   THE PROBE THAT ONCE READ head+0x1c == body+0x1c == scene ON EVERY FRAME WAS
   READING A STANDING PLAYER, and the claim it left here was wrong in motion.
   The seat's matrix carries the X and Z rotations and a model offset that the
   old hand-built `scene` did not; all of them are zero at rest and none of them
   are at speed. That is where Yoshi's head-off-body separation came from, and
   hal_render_player_world now reads the seated matrix instead of rebuilding
   one -- see the banner there for the measurements. head+0x1c and body+0x1c are
   the same write on the ROM, and they are the same value here now.

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

/* run mg16 lane MP3: the two globals Player::Render's own gates read. The
   per-slot Player table and the local-player index are read by the adventure
   ghost pass below (declared here so hal_render_player_world can tell a remote
   body from the local one; also declared later at their first non-ghost use). */
extern "C" {
extern unsigned char data_0209f2d8;   /* VS mode flag */
extern unsigned char data_0209fc5c[]; /* per-slot "this slot is live"; BYTE
                                         stride, the ROM's own width */
extern void *data_0209f394[];         /* per-slot Player* */
extern unsigned char data_0209f250;   /* local player index */
}

/* ---- THE GHOST ALPHA -------------------------------------------------------
   The DS polygon alpha is 5 bits (0..31); func_02046208 writes it into the
   0x1f0000 field of every material's polygon attribute. 31 is fully opaque and
   1 is nearly gone; 0 is NOT invisible, it is the DS's wireframe mode, so the
   floor here is 1. The default is a ghostly ~40%. SM64DS_ADVENTURE_ALPHA tunes
   it for the visual check without a rebuild. */
static unsigned ghost_alpha()
{
    static int a = -1;
    if (a < 0) {
        const char *e = std::getenv("SM64DS_ADVENTURE_ALPHA");
        a = e ? std::atoi(e) : 12;
        if (a < 1) a = 1;
        if (a > 31) a = 31;
    }
    return (unsigned)a;
}

/* ModelBase::ApplyOpacity, the ROM's own one-argument body (method_faces.cpp
   forwards to the matched src/_ZN9ModelBase12ApplyOpacityEj.cpp). It walks the
   model's components and stamps the alpha into each material, the same call
   Player::Render makes on the wings. Re-applied every frame because the
   material flags are rebuilt by the per-frame update. */
extern "C" void _ZN9ModelBase12ApplyOpacityEj(void *self, unsigned a);
static void ghost_opacity(void *model)
{
    if (model) _ZN9ModelBase12ApplyOpacityEj(model, ghost_alpha());
}

/* ---- THE NO-COLLISION HALF OF THE GHOST PASS ------------------------------
   Once per frame, over every REMOTE live slot, re-assert the disable-
   interaction state (status/VSMERCY.md, player_fields::disable_interaction) so
   a ghost body cannot collide with, damage, or be damaged by the local player.
   RE-ASSERTED every frame because Player::ChangeState re-arms all three fields
   on every transition; a single set does not stick. The LOCAL body
   (data_0209f250) is never touched, so the player stays fully solid and fully
   interactive. Gated on the mode, so VS and solo are byte-unaffected. Called
   from the per-frame tick in tests/walk_window.cpp, beside sync_tick. */
extern "C" void port_adventure_ghost_hold()
{
    if (!port::adventure_ghost_mode()) return;
    const int me = (int)data_0209f250;
    for (int i = 0; i < kPortMaxPlayers; ++i) {
        if (i == me) continue;
        if (data_0209fc5c[i] == 0) continue;   /* slot not live */
        if (void *a = data_0209f394[i])
            port::player::disable_interaction(a);
    }
}

/* ---- THE ADVENTURE-GHOST PROBE (SM64DS_ADVENTURE_PROBE) --------------------
   A headless, single-instance proof of the M1 capability, run against the REAL
   level-boot spawn (SM64DS_VS_PLAYERS=2 seats one extra body with no VS mode)
   rather than a synthetic construction. Called once per frame from the walk
   loop AFTER port_adventure_ghost_hold, so the three interaction flags it reads
   on the ghost are this frame's. It asserts, and prints one [advprobe] line at
   frame kAt+2:

     - a ghost body exists at a remote live slot (i != data_0209f250);
     - this is NOT VS mode (data_0209f2d8 == 0) and adventure mode is on;
     - the ghost is held non-colliding (all three flags via interaction_disabled)
       while the LOCAL body is not (it stays solid and interactive);
     - the ghost CARRIES A NETWORK POSE: a real wire snapshot (port_adventure_
       probe_apply, the actual apply_snapshot path) teleports the ghost to a
       chosen pose and the ghost lands there while the LOCAL body does not move.

   ALL PASS is the line the driver greps. Inert unless the env knob is set. */
extern "C" void port_adventure_probe_apply(int slot, int x, int y, int z,
                                           short yaw);
extern "C" void port_adventure_probe(int frame)
{
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_ADVENTURE_PROBE") ? 1 : 0;
    if (!on) return;

    /* One shot, late enough that the level has booted and both bodies are past
       the level-entry no-control. Inject AND verify in the SAME frame so the
       ghost cannot drift under its own physics between set and read, and so the
       local-write witness reads exactly this one apply. */
    static bool done = false;
    const int kAt = 90;
    if (frame != kAt || done) return;

    const int me = (int)data_0209f250;
    void *localb = (me >= 0 && me < kPortMaxPlayers) ? data_0209f394[me] : 0;
    int ghost_slot = -1;
    for (int i = 0; i < kPortMaxPlayers; ++i) {
        if (i == me) continue;
        if (data_0209fc5c[i] == 0) continue;
        if (data_0209f394[i]) { ghost_slot = i; break; }
    }
    done = true;

    int fails = 0;
    const bool have_ghost = ghost_slot >= 0 && localb;
    const bool not_vs = data_0209f2d8 == 0;
    const bool adv_on = port::adventure_ghost_mode();
    bool ghost_held = false, local_free = false, pose_ok = false,
         local_untouched = false;

    if (have_ghost) {
        void *g = data_0209f394[ghost_slot];
        /* the hold already ran this frame (walk_window calls it first), so the
           three interaction flags are freshly set on the ghost and untouched on
           the local body. */
        ghost_held = port::player::interaction_disabled(g);
        local_free = !port::player::interaction_disabled(localb);

        /* drive one REAL wire snapshot into the ghost, 4 units off its own
           position with a distinct yaw. The local body must NOT move under it:
           the sync layer's own witness counts any change to the local body
           across the apply call, and it must stay zero. */
        const int tx = *port::player::pos_x(g) + 4 * 0x1000;
        const int ty = *port::player::pos_y(g);
        const int tz = *port::player::pos_z(g) + 4 * 0x1000;
        const short tyaw = (short)0x2000;
        const unsigned long long lw0 = port::sync_stats().local_writes;
        port_adventure_probe_apply(ghost_slot, tx, ty, tz, tyaw);
        const unsigned long long lw1 = port::sync_stats().local_writes;

        pose_ok = *port::player::pos_x(g) == tx &&
                  *port::player::pos_z(g) == tz &&
                  *port::player::facing(g) == tyaw;
        local_untouched = (lw1 == lw0);
    }

    if (!have_ghost) ++fails;
    if (!not_vs) ++fails;
    if (!adv_on) ++fails;
    if (!ghost_held) ++fails;
    if (!local_free) ++fails;
    if (!pose_ok) ++fails;
    if (!local_untouched) ++fails;
    std::fprintf(stderr,
        "[advprobe] ghost_slot=%d have_ghost=%d not_vs=%d adv_on=%d "
        "ghost_held=%d local_free=%d pose_carried=%d local_untouched=%d "
        "=> %s\n",
        ghost_slot, have_ghost, not_vs, adv_on, ghost_held, local_free,
        pose_ok, local_untouched, fails == 0 ? "ALL PASS" : "FAIL");
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

/* ---- SM64DS_YHD_PROBE: the head-vs-body matrix separation, per frame --------
   Prints the difference between the two matrices this function is about to draw
   with -- `scene`, the body's, and the head's at +0x1c -- next to the player
   fields func_ov002_020e444c builds both of them from. On the ROM those two are
   the SAME WRITE (src/func_ov002_020e444c.c lines 55 and 66 both store
   data_020a0e68), so every difference column here reads zero, and a nonzero one
   is the gap the head is being drawn away from the body by. That is the
   regression the banner in hal_render_player_world closed, so the probe stays.

   dtr = head translation - body translation, in scene units (world >> 3).
   drot = max |head rotation element - body rotation element|, Fix12 (0x1000=1).
   spd  = horizontal |dpos| per frame in world Fix12, the "certain speeds" axis. */
static double yhd_sqrt(double v) { return std::sqrt(v); }
static void yhd_probe(char *c, const int *scene, const char *head)
{
    static int on = -1;
    if (on < 0) {
        const char *e = getenv("SM64DS_YHD_PROBE");
        on = e ? atoi(e) : 0;
        if (on)
            printf("[yhd] frame spd sink u690 u694 angY angX angZ "
                   "dtrx dtry dtrz |dtr| drot\n");
    }
    if (!on || !head) return;

    static int frame = 0, px = 0, py = 0, pz = 0, primed = 0;
    const int x = *(const int *)(c + 0x5c);
    const int y = *(const int *)(c + 0x60);
    const int z = *(const int *)(c + 0x64);
    const int vx = primed ? x - px : 0;
    const int vz = primed ? z - pz : 0;
    px = x; py = y; pz = z; primed = 1;
    (void)py;

    const int *hm = (const int *)(head + 0x1c);
    const int dtx = hm[9] - scene[9];
    const int dty = hm[10] - scene[10];
    const int dtz = hm[11] - scene[11];
    int drot = 0;
    for (int i = 0; i < 9; ++i) {
        int d = hm[i] - scene[i];
        if (d < 0) d = -d;
        if (d > drot) drot = d;
    }
    const double spd = (double)vx * vx + (double)vz * vz;
    const double dtr = (double)dtx * dtx + (double)dty * dty + (double)dtz * dtz;

    printf("[yhd] %d %.1f %d %d %d %d %d %d %d %d %d %.1f %d\n", frame++,
           yhd_sqrt(spd), *(const int *)(c + 0x68c),
           *(const int *)(c + 0x690), *(const int *)(c + 0x694),
           (int)*(const short *)(c + 0x8e), (int)*(const short *)(c + 0x8c),
           (int)*(const short *)(c + 0x90), dtx, dty, dtz,
           yhd_sqrt(dtr), drot);
}

/* ---- THE VS COLOUR, AND WHY EVERY YOSHI WAS GREEN --------------------------

   In VS every player is Yoshi -- the spawn loop forces character 3 into every
   slot (src/_Z19LoadEntranceObjectsRN11LVL_Overlay11ObjSubTableEij.c:68-71) --
   and they are told apart by COLOUR. That is the ROM's own arrangement, not a
   mod: yoshi_model.bmd carries ONE palette, yoshi_all_16p_pl, 128 bytes = four
   stacked 16-colour rows, and a player selects his row by shifting the palette
   base the material records point at.

   The ROM does it in two steps, and this port ran the first and not the second.

     STEP 1, at resource load. func_ov002_020e5948 ends with
         Player+0x61C = body material[0]'s palette base + (mPlayerNo << 1)
     -- one 16-colour row per step of the player number. The port runs this: it
     is port/unmatched/TexSeq_Caller_ov002_020e5948.cpp:408, same line.

     STEP 2, at render. Player::Render (src/_ZN6Player6RenderEv.cpp:66-76 and
     :112-122) writes that one value into +0x20 of EVERY runtime material
     record, once over the body model's materials and once over the head
     model's, gated on data_0209f2d8 == 1 (VS) and param1 == 3 (Yoshi).
     +0x20 is the material's TEXPLTT_BASE: func_02044b30
     (src/func_02044b30.c:14) stores it straight to 0x040004AC on the way into
     each display list, and ntr binds the palette at PLTT_SLOT_BASE + base*16.

   src/_ZN6Player6RenderEv.cpp IS NOT IN THIS PORT'S LINK -- port/slice_w1l5.txt
   line 202, blocked by func_ov002_020e3e00's C2733 -- and the Player vtable's
   Render slot is a no-op (hal/level_boot.cpp's ps_render). Every draw a player
   gets comes from THIS function instead, and until now it reproduced the four
   gates, the body draw, the head group, the three texture sequences and the
   wings, but not step 2. So nothing in the port ever wrote a material record,
   every Yoshi kept the base his BMD was authored with -- row 0 -- and the owner
   saw two green Yoshis in a live two-window match.

   MEASURED, at 65bae498d with SM64DS_VS_COLOR_PROBE=1, arena 51, two windows,
   2201 frames x 2 players: Player+0x61C read 5539 for slot 0 and 5541 for slot
   1 (the ROM's selector, working), while the body material base read 5539 for
   BOTH and the bound colours were byte-identical -- 0d83,1224,1a86,2eab, which
   is row 0, the greens. The selector was right the whole time and nothing
   spent it.

   WHY IT IS SAFE TO WRITE THE HEAD'S RECORDS TOO, which looks like it should
   corrupt them: the value is the BODY's palette base, and the head files'
   own 16-colour palette is byte-identical to the body's row 0. Player 0
   shifts by nothing, so VS-green Yoshi samples the body's row 0 and looks
   exactly like adventure Yoshi -- which is only true because one step of
   mPlayerNo is exactly one 16-colour row. The head is not reloaded or
   resized; its own palette is simply never sampled in VS.

   THE GATES ARE THE ROM'S AND NOTHING MORE. Outside VS, or on a character
   that is not Yoshi, this writes nothing at all. */
static void hal_player_vs_palette(char *c, char *mdl)
{
    if (data_0209f2d8 != 1) return;         /* VS mode only */
    if (*(int *)(c + 0x8) != 3) return;     /* param1 == 3, Yoshi */
    if (!mdl) return;
    const int *p = (const int *)(mdl + 8);  /* ModelComponents at +0x8 */
    const unsigned char *hdr = (const unsigned char *)p[0];  /* BMD_File */
    char *rec = (char *)p[1];                                /* materials */
    if (!hdr || !rec) return;               /* port hygiene; the ROM faults */
    unsigned n = 0;
    std::memcpy(&n, hdr + 0x24, 4);         /* BMD_File::numMaterials */
    const int pal = *(const int *)(c + 0x61c);
    for (unsigned i = 0; i < n; ++i, rec += 0x30)
        *(int *)(rec + 0x20) = pal;
}

/* ---- SM64DS_VS_COLOR_PROBE=1: reading the BOUND palette back ----------------
   The VS colour question cannot be answered from Player+0x61C. That word is
   written UNCONDITIONALLY by the resource load (base + (mPlayerNo << 1), six
   lines above the VS test in port/unmatched/TexSeq_Caller_ov002_020e5948.cpp),
   so two MARIOS differ by 2 there and it says nothing about what got drawn.
   Review retired the arm that read it as proof, and the bar it left is a probe
   that reads a BOUND record back. This is that probe, in three layers, each
   one further down the pipe than the last:

     source   Player+0x61C          what the resource load computed
     record   material[i] + 0x20    the runtime material record the render
                                    walk spends -- what Player::Render writes
                                    and what this port did not
     bound    TEXPLTT_BASE latch    read back out of the mapped I/O window at
                                    0x040004AC AFTER the draw. func_02044b30
                                    (src/func_02044b30.c:14) stores material
                                    +0x20 there per material, and ntr's
                                    io_write mirrors every store into the
                                    window before dispatching it to the
                                    geometry engine (port/ntr/io.cpp:655), so
                                    this is the last palette the engine was
                                    actually handed during this player's draw.

   And the colours themselves: ntr/gx.cpp samples a 16-colour material at
   PLTT_SLOT_BASE + base*16 (bind_from_vram), so the same arithmetic here reads
   the exact BGR555 entries the decoder will use. Two players whose entries
   differ are two players painted different colours; two players whose entries
   match are the defect.

   The polygon delta rides along because a bind with no geometry behind it
   proves nothing -- a palette latched for a player who submitted zero
   triangles is not a colour. */
static int vscol_on()
{
    static int on = -1;
    if (on < 0) on = getenv("SM64DS_VS_COLOR_PROBE") ? 1 : 0;
    return on;
}

/* The port's texture-palette window base; the same constant ntr/gx.cpp calls
   PLTT_SLOT_BASE and hal/model_host.cpp points data_020a60b0 at. */
static const unsigned kVsColPlttWindow = 0x06880000u;

static void vscol_model(const char *what, int frame, int no, char *mdl)
{
    if (!mdl) {
        std::fprintf(stderr, "[vscol] f%d slot%d %s model=NULL\n",
                     frame, no, what);
        return;
    }
    const int *p = (const int *)(mdl + 8);      /* ModelComponents at +0x8 */
    const unsigned char *hdr = (const unsigned char *)p[0];  /* BMD_File */
    const unsigned char *rec = (const unsigned char *)p[1];  /* materials */
    if (!hdr || !rec) {
        std::fprintf(stderr, "[vscol] f%d slot%d %s hdr=%p rec=%p\n",
                     frame, no, what, (const void *)hdr, (const void *)rec);
        return;
    }
    unsigned n = 0;
    std::memcpy(&n, hdr + 0x24, 4);             /* BMD_File::numMaterials */
    if (n > 8) n = 8;
    for (unsigned i = 0; i < n; ++i) {
        int base = 0;
        std::memcpy(&base, rec + i * 0x30 + 0x20, 4);
        const unsigned short *pal =
            (const unsigned short *)(kVsColPlttWindow + (unsigned)base * 16u);
        std::fprintf(stderr,
                     "[vscol] f%d slot%d %s mat%u base=%d "
                     "col=%04x,%04x,%04x,%04x\n",
                     frame, no, what, i, base,
                     pal[0], pal[1], pal[2], pal[3]);
    }
}

static void vscol_probe(char *c, int frame, std::size_t tris_before)
{
    const int no = (int)*(unsigned char *)(c + 0x6d8);
    std::size_t tris_after = 0;
    ntr::gx_polygons(tris_after);
    /* the latch, read back out of the mapped window rather than assumed */
    const unsigned latched = *(volatile unsigned *)0x040004ACu;
    const unsigned short *lpal =
        (const unsigned short *)(kVsColPlttWindow + latched * 16u);
    std::fprintf(stderr,
                 "[vscol] f%d slot%d char=%d param1=%d pal61c=%d "
                 "TEXPLTT_BASE=%u col=%04x,%04x,%04x,%04x tris=%d\n",
                 frame, no, (int)(*(unsigned char *)(c + 0x6d9) & 7),
                 *(int *)(c + 8), *(int *)(c + 0x61c), latched,
                 lpal[0], lpal[1], lpal[2], lpal[3],
                 (int)(tris_after - tris_before));

    const unsigned id =
        _ZNK6Player14GetBodyModelIDEjb(c, *(int *)(c + 8) & 0xff, 0);
    vscol_model("body", frame, no, (char *)((ModelAnim **)(c + 0xdc))[id]);
    const unsigned hid =
        func_ov002_020becf4(c, *(unsigned char *)(c + 0x6db), 1);
    if (hid != 8 && hid != 9)
        vscol_model("head", frame, no, ((char **)(c + 0x154))[hid]);
}

void hal_render_player_world(void *player)
{
    char *c = (char *)player;
    /* ADVENTURE GHOSTS: a REMOTE body drawn see-through. This is the render
       half of the ghost pass; the no-collision half is port_adventure_ghost_hold
       below. Remote means "not the body at the local player's slot" -- the same
       body the per-slot render loop in tests/walk_window.cpp draws last and never
       ghosts. The mode gate keeps VS and solo byte-unaffected: adventure mode is
       its own flag, never data_0209f2d8. */
    const bool ghost = port::adventure_ghost_mode() &&
                       player != data_0209f394[data_0209f250];
    std::size_t vscol_tris0 = 0;
    static int vscol_frame = -1;
    if (vscol_on()) {
        ntr::gx_polygons(vscol_tris0);
        /* one frame counter for the function, bumped by slot 0 so both
           players' lines in the same frame carry the same number */
        if (*(unsigned char *)(c + 0x6d8) == 0) ++vscol_frame;
    }

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
        if (data_0209f2d8 == 1 && no < kPortMaxPlayers && data_0209fc5c[no] == 0)   /* 0.3.2: sixteen */
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
    /* THE BODY MATRIX IS THE ONE THE SEAT ALREADY WROTE, and this function
       must not build its own. That is the whole of the Yoshi head-vs-body
       separation the field report described as the head coming off at speed
       and on hard landings.

       func_ov002_020e444c (src/func_ov002_020e444c.c) composes ONE matrix in
       data_020a0e68 each frame inside Player::Behavior --

           translation(RotateY(pos, mAngleY) + [0, unk_690 - mSinkDepth,
                                                unk_694])
           then rotation Y (+0x8e), then X (+0x8c), then Z (+0x90)

       -- and stores that same matrix into the BODY model at +0x1c (line 55)
       and into the HEAD model at +0x1c (line 66), in one statement group.
       Player::Render (src/_ZN6Player6RenderEv.cpp:79 and :121) then draws both
       with a bare Model::Render and sets NO matrix at render time. On the ROM
       the two models cannot separate: they are literally the same write.

       This function used to rebuild a matrix here from mAngleY and the
       position alone and store it over the body's, leaving the head on the
       seated one. The two agree while Yoshi stands still -- which is why an
       earlier head-vs-body probe read them identical and the note above
       recorded them as bit-identical -- but the seat also carries the X and Z
       rotations, and +0x8c is the LEAN, driven by speed out of
       func_ov002_020cd550. So the faster he ran the further the head pitched
       away from a body that never pitched at all.

       Measured on this tree before the change, castle grounds as Yoshi, the
       largest rotation-element difference between the head's matrix and the
       body's over 300 frames (Fix12, 0x1000 = 1.0):

           idle       maxspeed      0    gap    0
           walk       maxspeed  65520    gap  682   (lean 9.5 deg)
           dash       maxspeed 131040    gap 1350   (lean 19.2 deg)
           longjump   maxspeed 196560    gap 1112

       and gap == 4096 * sin(lean) to within rounding at every sample, so the
       separation was the lean angle exactly. Against the matrix the seat had
       already left in the body, the head measured identical on every frame of
       every run -- the right matrix was there the whole time and was being
       thrown away.

       Both halves of the field report are this one fault. The lean only exists
       above a speed, and it grows with it, which is "at certain speeds". And it
       engages in a SINGLE FRAME at full size around jumps and landings -- 0 to
       1038 at frame 158 of the jump-spam run, 0 to 1050 at frame 98 of the
       dash-jump run, 0 to 1073 at frame 234 of the long-jump run -- so the head
       did not drift off, it popped off and back on, which is "when he hits the
       ground too hard".

       So: read it, do not build it. The scale and units are unchanged, because
       the seat writes the same scene convention this function used to build
       (its translation is `out >> 3`, the same world-to-scene shift, with the
       rotation rows at 1.0 out of Matrix4x3_FromTranslation). `scene` stays the
       name the neck-bone and wing composes below use, and they now compose
       against the body's real matrix rather than a stand-in. */
    int scene[12];
    for (int i = 0; i < 12; ++i) scene[i] = ((const int *)&ma->mat4x3)[i];
    ma->ModelAnim::UpdateVerts();
    hal_player_vs_palette(c, (char *)ma);
    /* see-through, last before the draw so the per-frame material rebuild
       (UpdateVerts, the palette stamp) is already done and the alpha is what
       the raster reads. */
    if (ghost) ghost_opacity(ma);
    ma->ModelAnim::Render(0);
    hal_player_texseq_body(c);

    unsigned hid = func_ov002_020becf4(c, *(unsigned char *)(c + 0x6db), 1);
    if (hid != 8 && hid != 9) {
        char *head = ((char **)(c + 0x154))[hid];
        if (head) {
            yhd_probe(c, scene, head);
            hal_player_vs_palette(c, head);
            if (ghost) ghost_opacity(head);
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
            /* Slot 5 holds hal/cxxname_bridge.cpp's ma2_virtual18, a
               __fastcall face with a DEAD edx parameter and TWO stack
               arguments: (self, dummy, unsigned mat, const void *scale).
               This call used to borrow the head's three-parameter render
               shape, which put `composed` in the dead edx and left the
               scale to an unwritten stack slot -- so the first time the
               gate ever opened (a wing-feather collect, or entering level
               31 winged) the callee rendered off a garbage scale pointer
               and popped 8 stack bytes where the caller pushed 4. The
               fault that pointed here read address 0x9 inside the model
               walk, three frames after SM64DS_SPAWN_ACTOR=345's feather
               was collected at the player's feet. */
            ((void(__fastcall *)(void *, void *, unsigned, const void *))(
                ((void ***)m4)[0][5]))(m4, 0, (unsigned)(uintptr_t)composed,
                                       c + 0x80);
        }
    }

    /* LAST, after every draw this function makes, so the latch it reads is the
       one this player's geometry was submitted under. */
    if (vscol_on()) vscol_probe(c, vscol_frame, vscol_tris0);
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
    /* THE HEALTH TABLE IS SIXTEEN WIDE, so mPlayerNo is read WHOLE. The `& 3`
       that used to be here predated 0.3.2's widening of data_02092144 to
       kPortMaxPlayers (hal/cxx_aliases.cpp:70), and it was wrong the moment
       slots 4..15 could exist: a swap in slot 5 saved slot 1's health and then
       wrote slot 1's health back over whatever slot 1 had done in between.
       mPlayerNo is a whole byte and the port writes the TRUE slot into it
       (hal/level_boot.cpp, right after Actor::Spawn returns), so the only
       thing owed here is a bound, not a mask. */
    unsigned pno = *(unsigned char *)(c + 0x6d8);
    if (pno >= (unsigned)kPortMaxPlayers) pno = 0;
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

    /* run pal16: THE TWO-BIT FIELD, AND THE TRUE SLOT CARRIED ACROSS IT.
       mPlayerNo is a whole byte and the port writes the TRUE slot into it
       (hal/level_boot.cpp), but the flag word's slot field is bits 6-7 and
       nothing else. Shifting a raw mPlayerNo in here was wrong twice over for
       a slot at or above four:

         - `5 << 6` is 0x140, so bit 8 went in SET, and this function's own
           note three paragraphs up promises bits 8+ go in as 0 because they
           feed func_ov002_020c7dd0's entrance type, which a mid-level swap
           has no business re-running. Masking is what makes that note true.
         - InitResources then unpacked `(a >> 6) & 3` back into +0x6d8 and
           recomputed the VS palette word at +0x61C from it, so a legacy swap
           in slot 5 came back as player 1 wearing row 1. That is exactly the
           defect run pal16 fixed at the spawn, reverted here.

       THE PORT HAS THREE CALLERS OF InitResources AND THIS IS THE ONLY ONE
       THAT NEEDED THIS. hal/level_boot.cpp's ps_init is the vtable slot, so it
       runs INSIDE Actor::Spawn and is exactly what the spawn-side repair
       already corrects on the way out. hal_player_init_resources just above is
       the gate-10 smoke's direct entry, one Mario at slot 0 in a harness with
       no VS and no second player, where this delta is zero by construction.
       This one is the only place a LIVE player at an arbitrary slot is rebuilt
       mid-level, which is why it is the only place the invariant could come
       back wrong.

       So the field is masked going in, and both halves of the identity are
       put back afterwards -- the same delta the spawn uses, for the same
       reason: one step of mPlayerNo is one sixteen-colour row. A slot 0..3
       masks to itself and the repair is a no-op, which is every run of the
       legacy path that has ever been taken. */
    const unsigned true_slot = *(unsigned char *)(c + 0x6d8);
    const unsigned packed_slot = true_slot & 3;

    param = ch | ((unsigned)*(unsigned char *)(c + 0x6da) << 3) |
            (packed_slot << 6);
    *(int *)(c + 8) = (int)param;
    *(unsigned char *)(c + 0x6d9) = (unsigned char)ch;

    _ZN6Player13InitResourcesEv(c);

    if (true_slot != packed_slot) {
        *(int *)(c + 0x61c) += ((int)true_slot - (int)packed_slot) << 1;
        *(unsigned char *)(c + 0x6d8) = (unsigned char)true_slot;
    }

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
