// Gate-10 method bridges: C-named references -> MSVC method definitions.
//
// The Player closure's callers reference these at C linkage while the
// defining TUs compile them as real methods against the shared headers.
// Same hop as gate 9 (cxxname_bridge.cpp), split into its own TU because
// Player.h drags a wider include surface than the gate-9 file wants.
/* PORT_ALIGN / PORT_GROUPED_DECL live here. The Linux build force-includes
   this header from CMake, so a TU using those macros compiles there without
   naming it -- but MSVC never gets the force-include, so the include has to
   be explicit or the Windows build fails on an undefined macro. The header is
   include-guarded and its keyword shims are inside #ifndef _MSC_VER, so this
   is inert on Windows beyond the two macro definitions. */
#include "port_msvc_compat.h"
#include <cstdio>
#include <cstdlib>

#include "Animation.h"
#include "BgCh.h"
#include "NestedHeapIterator.h"
#include "Player.h"
#include "ShadowModel.h"
#include "TextureSequence.h"
#include "Heap.h"
#include "ModelAnim.h"

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
#ifdef _WIN32
int _ZNK9Animation12WillHitFrameEi(void *self, int f)
{ return ((Animation *)self)->Animation::WillHitFrame(f) ? 1 : 0; }
#endif /* _WIN32: on GCC Animation::WillHitFrame(int)const mangles to the SAME
   _ZNK9Animation12WillHitFrameEi this defines, so it self-recurses. The real
   impl is src/_ZNK9Animation12WillHitFrameEi.cpp (gate10); Linux binds to it. */
/* GetFlags is C linkage now: main's mangled-declaration sweep gave the Player
   state TUs an extern "C" declaration, so they call the plain name. The alias
   at the head of hal/cxx_aliases.cpp still serves the old C++ mangling. */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" int _ZN9Animation8GetFlagsEv(void *self)
{ return ((Animation *)self)->Animation::GetFlags(); }
#else
extern "C" int _ZN9Animation8GetFlagsEv(void *self);  /* Linux: real symbol from src/_ZN9Animation8GetFlagsEv */
#endif /* _WIN32 */
void _ZN6Player4HealEi(Player *p, int amt)
{ p->Player::Heal(amt); }

#ifdef _WIN32 /* LINUX: this method IS _ZNK6Player14GetBodyModelIDEjb (its own
   Itanium mangling), so the body forwards to itself -> infinite recursion. On
   MSVC the two mangle differently (this converts __cdecl->__thiscall); on Linux
   the real src/_ZNK6Player14GetBodyModelIDEjb TU owns the symbol and every
   `this->GetBodyModelID(...)` caller binds straight to it. */
unsigned int Player::GetBodyModelID(unsigned int a, bool b_) const
{ return _ZNK6Player14GetBodyModelIDEjb((char *)this, a, b_ ? 1 : 0); }
#endif /* _WIN32 */

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
#ifdef _WIN32
    /* MSVC folds the two Itanium dtor slots into one, so Model::Render is slot 4,
       and the object's own vtable thunks are __fastcall (this in ecx). */
    ((void(__fastcall *)(void *, void *, const void *))(
        ((void ***)head)[0][4]))(head, 0, 0);
#else
    /* Linux/Itanium keeps TWO dtor slots (D1@0,D0@1), so Model::Render is slot 5
       (slot 4 is Virtual10 -- dispatching 4 here handed Virtual10 a head model and
       faulted). The Linux Model vtable is filled with plain cdecl thunks
       (mvL_render, this on the stack), so call slot 5 cdecl, not __fastcall. */
    ((void(*)(void *, const void *))(
        ((void ***)head)[0][5]))(head, 0);
#endif
}

void hal_render_player_world(void *player)
{
    char *c = (char *)player;
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

    unsigned hid = func_ov002_020becf4(c, *(unsigned char *)(c + 0x6db), 1);
    if (hid != 8 && hid != 9) {
        char *head = ((char **)(c + 0x154))[hid];
        if (head) {
            hal_render_head_group(c, head, hid, ma, scene);
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
                             *(unsigned short *)(c + 0x6a6),
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

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN6Player16InitWingFeathersEb(void *self, unsigned char b_)
{ ((Player *)self)->Player::InitWingFeathers(b_ != 0); }
#else
void _ZN6Player16InitWingFeathersEb(void *self, unsigned char b_);  /* Linux: real symbol from src/_ZN6Player16InitWingFeathersEb */
#endif /* _WIN32 */
extern "C++" int ApproachLinear(int &ref, int target, int step);
#ifdef _WIN32 /* LINUX: `_Z14ApproachLinearRiii` IS the GCC Itanium mangling of the
   `ApproachLinear(int&,int,int)` this forwards to (src/_Z14ApproachLinearRiii.cpp),
   so on GCC this extern-C shim self-recurses infinitely. On MSVC the C++ name mangles
   differently, so the (int*) -> (int&) adapter is real and needed. On Linux the
   callers' `_Z14ApproachLinearRiii(int*,...)` bind straight to the real src TU
   (reference == pointer at the ABI). */
int _Z14ApproachLinearRiii(int *ref, int target, int step)
{ return ApproachLinear(*ref, target, step); }
#endif
int hal_anim_willhit(void *self, int f)
{ return ((Animation *)self)->Animation::WillHitFrame(f) ? 1 : 0; }
int hal_nhi_next(void *self, void *h)
{ return ((NestedHeapIterator *)self)->NestedHeapIterator::Next(
      (HeapAllocator *)h); }

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN9Animation7AdvanceEv(void *self)
{ ((Animation *)self)->Animation::Advance(); }
#else
void _ZN9Animation7AdvanceEv(void *self);  /* Linux: real symbol from src/_ZN9Animation7AdvanceEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN9Animation8FinishedEv(void *self)
{ return ((Animation *)self)->Animation::Finished(); }
#else
int _ZN9Animation8FinishedEv(void *self);  /* Linux: real symbol from src/_ZN9Animation8FinishedEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the method it forwards to -> self-recurse on GCC. On Linux the callers bind to the real src/ TU (the host NULL-fp log is Windows-only as a result). */
char *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp)
{
    if (!fp) {
        std::fprintf(stderr, "  [anim] LoadFile on NULL fileptr (table hole)\n");
        return 0;
    }
    return Animation::LoadFile(*(SharedFilePtr *)fp);
}
#else
char *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
#endif /* _WIN32 */

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player6IsAnimEj(void *self, unsigned a)
{ return ((Player *)self)->Player::IsAnim(a); }
#else
int _ZN6Player6IsAnimEj(void *self, unsigned a);  /* Linux: real symbol from src/_ZN6Player6IsAnimEj */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player12FinishedAnimEv(void *self)
{ return ((Player *)self)->Player::FinishedAnim(); }
#else
int _ZN6Player12FinishedAnimEv(void *self);  /* Linux: real symbol from src/_ZN6Player12FinishedAnimEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player17SetNoControlStateEhih(void *self, unsigned char a, int b,
                                      unsigned char c)
{ return ((Player *)self)->Player::SetNoControlState(a, b, c); }
#else
int _ZN6Player17SetNoControlStateEhih(void *self, unsigned char a, int b,
                                      unsigned char c);  /* Linux: real symbol from src/_ZN6Player17SetNoControlStateEhih */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player8HasNoCapEv(void *self)
{ return ((Player *)self)->Player::HasNoCap(); }
#else
int _ZN6Player8HasNoCapEv(void *self);  /* Linux: real symbol from src/_ZN6Player8HasNoCapEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN6Player9GetHealthEv(void *self)
{ return ((Player *)self)->Player::GetHealth(); }
#else
int _ZN6Player9GetHealthEv(void *self);  /* Linux: real symbol from src/_ZN6Player9GetHealthEv */
#endif /* _WIN32 */

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN4BgCh19StartDetectingWaterEv(void *self)
{ ((BgCh *)self)->BgCh::StartDetectingWater(); }
#else
void _ZN4BgCh19StartDetectingWaterEv(void *self);  /* Linux: real symbol from src/_ZN4BgCh19StartDetectingWaterEv */
#endif /* _WIN32 */

/* SHADOW SYSTEM DEFERRED (matches InitCuboid, cxxname_bridge.cpp, which now
   carries the full writeup). The template BMD at data_020ad560 is NOT
   runtime-built: it is static .data in overlay 1, a complete BMD_File with one
   bone at 0x020ad5dc and one material at 0x020ad4c4. Parsing the stub does
   walk garbage, but because the port never mounts ov001, not because a
   builder is missing.

   Note the stub is also UNDERSIZED where it is declared (actor_vtables.cpp):
   the ROM record is 0x3c bytes, 0x020ad560 to the bone at 0x020ad59c. */
void _ZN11ShadowModel12InitCylinderEv(void *) {}

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *self, void *bmd,
                                                       void *btp)
{ ((TextureSequence *)self)->TextureSequence::Prepare(*(BMD_File *)bmd,
                                                      *(BTP_File *)btp); }
#else
void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *self, void *bmd,
                                                       void *btp);  /* Linux: real symbol from src/_ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the method it forwards to -> self-recurse on GCC. On Linux bind to the real src/ TU. */
void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *fp)
{ return TextureSequence::LoadFile(*(SharedFilePtr *)fp); }
#else
void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *fp);
#endif /* _WIN32 */

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(void *self, void *node)
{ ((NestedHeapIterator *)self)->NestedHeapIterator::Remove(
      (HeapAllocator *)node); }
#else
void _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(void *self, void *node);  /* Linux: real symbol from src/_ZN18NestedHeapIterator6RemoveEP13HeapAllocator */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN18NestedHeapIterator8PreviousEP13HeapAllocator(void *self, void *h)
{ return ((NestedHeapIterator *)self)->NestedHeapIterator::Previous(
      (HeapAllocator *)h); }
#else
int _ZN18NestedHeapIterator8PreviousEP13HeapAllocator(void *self, void *h);  /* Linux: real symbol from src/_ZN18NestedHeapIterator8PreviousEP13HeapAllocator */
#endif /* _WIN32 */

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN4Heap6RescueEv(void *self)
{ return ((Heap *)self)->Heap::Rescue(); }
#else
int _ZN4Heap6RescueEv(void *self);  /* Linux: real symbol from src/_ZN4Heap6RescueEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN4Heap21MaxAllocationUnitSizeEv(void *self)
{ return ((Heap *)self)->Heap::MaxAllocationUnitSize(); }
#else
int _ZN4Heap21MaxAllocationUnitSizeEv(void *self);  /* Linux: real symbol from src/_ZN4Heap21MaxAllocationUnitSizeEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN4Heap6IntactEv(void *self)
{ return ((Heap *)self)->Heap::Intact() ? 1 : 0; }
#else
int _ZN4Heap6IntactEv(void *self);  /* Linux: real symbol from src/_ZN4Heap6IntactEv */
#endif /* _WIN32 */

/* gate-10 BSS, second ring */
/* data_0209b44c is NOT here: it is the one-byte area id, hosted in
   hal/actor_vtables.cpp so the smoke targets that link no player bridge
   still get it, and so writer and reader share one object. */
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
/* DTCM scratch the timer list walker anchors at */
PORT_ALIGN(8) unsigned char data_023c0000[64];
int data_02099e94[4], data_02099ebc[4], data_02099ec4[4], data_02099fcc[4];
int data_020a6084[4], data_020a6088[2], data_020a8114[4];

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
