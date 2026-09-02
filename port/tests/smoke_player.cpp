// Gate-10/11 smoke: Mario comes to his feet and walks.
//
// The Player ctor chains everything the earlier gates hosted -- Actor,
// two ModelAnims (body and head), TextureSequence/MaterialChanger arrays,
// ShadowModel (deferred), cylinder and mesh collision clients. This first
// cut proves the CONSTRUCTION chain and grows toward InitResources +
// St_Wait as the link closure lands (the gate-9 method).
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ntr/gx.h"
#include "ntr/mmio.h"
#include "ntr/ppu.h"

#include "fault_probe.h"

typedef unsigned int u32;

extern "C" {
void *_ZN6PlayerC1Ev(void *self);
void *_ZN4Heap13SetupRootHeapEv(void);
void *_ZN9ActorBasenwEj(unsigned size);
extern int data_0209b3ec[12];
extern unsigned short data_020a4b54;
extern void **data_020a4bb8;
extern void *data_020a0eac_c;                /* Memory::gameHeapPtr */
extern void *data_020a0ea0;                  /* defaultHeapPtr (gate 3a) */
void _ZN4Heap18InitializeGameHeapEjPS_(unsigned size, void *root);
unsigned _ZN22ExpandingHeapAllocator10MemoryLeftEv(void *alloc);
void hal_fill_model_vtable(void);
void hal_fill_shadow_vtable(void);
void hal_fill_mmc_vtable(void);
void hal_fill_modelanim2_vtable(void);
int hal_player_init_resources(void *p);
int hal_player_st_wait_init(void *p);
int hal_player_st_wait_main(void *p);
int hal_player_st_walk_main(void *p);
int hal_player_behavior(void *p);
void hal_render_player_body(void *p);
void hal_render_player_body_only(void *p);
extern "C" int port_player_render_hidden(const void *p);  /* the render gate */
extern char data_0209f4a0[];                 /* per-player pad blocks, 0x18 */
extern unsigned char data_020a0e40[];        /* current player index */
extern unsigned char data_ov002_0211013c[];  /* St_Walk state object */
extern short data_02092144[];                /* per-player health words */
extern unsigned char data_ov002_02110424[];  /* St_Fall state object */
void port_ov002_patch(void);         /* rehome DS-baked data pointers */
void port_cross_patch(void);         /* and the ones that leave their mount */
/* ov002 static ctors: SharedFilePtr IDs, state tables */
void __sinit_ov002_02100560(void);
void __sinit_ov002_02100938(void);
void __sinit_ov002_02100adc(void);
void __sinit_ov002_02100c50(void);
void __sinit_ov002_02100d44(void);
void __sinit_ov002_02100e50(void);
void __sinit_ov002_02100ec4(void);
void __sinit_ov002_02100f84(void);
void __sinit_ov002_02101064(void);
void __sinit_ov002_02101478(void);
void __sinit_ov002_021014e4(void);
void __sinit_ov002_02101588(void);
void __sinit_ov002_02101738(void);
void __sinit_ov002_02101894(void);
void __sinit_ov002_02101900(void);
void __sinit_ov002_02101968(void);
void __sinit_ov002_021019d0(void);
void __sinit_ov002_02106e40(void);
void __sinit_ov002_02107118(void);
void __sinit_ov002_021071f4(void);
void __sinit_ov002_02107298(void);
void __sinit_ov002_02107304(void);
void __sinit_ov002_02107370(void);
void __sinit_ov002_02107f88(void);
void __sinit_ov002_0210804c(void);
void __sinit_ov002_02108094(void);
}

static int g_failures;
#define CHECK(cond) \
    do { if (!(cond)) { ++g_failures; \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

static void ident_fx(void *m)
{
    memset(m, 0, 48);
    ((int *)m)[0] = ((int *)m)[4] = ((int *)m)[8] = 0x1000;
}

/* SM64DS_WALK_BMP_DIR: render Mario's body ModelAnim each walk frame
   (smoke_model's fit-pass recipe) and write frame BMPs there. */
static void render_walk_frame(void *player, const char *dir, int frame)
{
    ntr::gx_reset();
    NTR_MMIO(uint32_t, 0x04000440) = 0;   /* projection = identity */
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    NTR_MMIO(uint32_t, 0x04000440) = 1;   /* position = identity */
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    NTR_MMIO(uint32_t, 0x04000580) = 0u | (0u << 8) | (255u << 16) |
                                     (191u << 24);
    ntr::gx_set_light(0, -0.4f, -0.6f, -0.7f, 0x7FFF);
    ntr::gx_enable_lights(0x1);
    hal_render_player_body_only(player);   /* measure on the body alone */

    size_t tris = 0;
    const ntr::GxTriangle *tarr = ntr::gx_polygons(tris);
    if (!tris) return;
    float minx = 1e30f, maxx = -1e30f, miny = 1e30f, maxy = -1e30f;
    for (size_t i = 0; i < tris; ++i)
        for (int v = 0; v < 3; ++v) {
            minx = fminf(minx, tarr[i].v[v].x);
            maxx = fmaxf(maxx, tarr[i].v[v].x);
            miny = fminf(miny, tarr[i].v[v].y);
            maxy = fmaxf(maxy, tarr[i].v[v].y);
        }
    const float cx0 = 2.0f * minx / ntr::SCREEN_W - 1.0f;
    const float cx1 = 2.0f * maxx / ntr::SCREEN_W - 1.0f;
    const float cy0 = 1.0f - 2.0f * maxy / ntr::SCREEN_H;
    const float cy1 = 1.0f - 2.0f * miny / ntr::SCREEN_H;
    const float w = fmaxf(cx1 - cx0, 1e-6f), h = fmaxf(cy1 - cy0, 1e-6f);
    /* fixed scale keeps the walk cycle from zoom-pumping frame to frame */
    static float s = 0.0f;
    if (s == 0.0f) s = fminf(1.5f / w, 1.5f / h);
    const float tx = -0.5f * (cx0 + cx1) * s, ty = -0.5f * (cy0 + cy1) * s;
    const int32_t m[16] = {(int32_t)(s * 4096), 0, 0, 0,
                           0, (int32_t)(s * 4096), 0, 0,
                           0, 0, 4096, 0,
                           (int32_t)(tx * 4096), (int32_t)(ty * 4096), 0,
                           4096};
    ntr::gx_reset();
    NTR_MMIO(uint32_t, 0x04000580) = 0u | (0u << 8) | (255u << 16) |
                                     (191u << 24);
    ntr::gx_set_light(0, -0.4f, -0.6f, -0.7f, 0x7FFF);
    ntr::gx_enable_lights(0x1);
    NTR_MMIO(uint32_t, 0x04000440) = 0;
    for (int i = 0; i < 16; ++i)
        NTR_MMIO(uint32_t, 0x04000458) = (uint32_t)m[i];
    NTR_MMIO(uint32_t, 0x04000440) = 1;
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    hal_render_player_body(player);
    {
        size_t t2 = 0;
        ntr::gx_polygons(t2);
        if (frame == 0)
            printf("  render: body-only %zu tris, body+head %zu tris\n",
                   tris, t2);
    }

    ntr::Framebuffer fb;
    for (int y = 0; y < ntr::SCREEN_H; ++y)
        for (int x = 0; x < ntr::SCREEN_W; ++x) fb.px[y][x] = 0xFF101820u;
    ntr::gx_render(fb);
    char path[512];
    snprintf(path, sizeof path, "%s/walk_%02d.bmp", dir, frame);
    ntr::ppu_write_bmp(path, fb);
}

int main(void)
{
    PORT_INSTALL_FAULT_PROBE();
    setvbuf(stdout, NULL, _IONBF, 0);
    if (!ntr::io_init()) { fprintf(stderr, "io_init failed\n"); return 2; }
    CHECK(_ZN4Heap13SetupRootHeapEv() != NULL);
    ident_fx(data_0209b3ec);
    hal_fill_model_vtable();
    hal_fill_shadow_vtable();
    hal_fill_mmc_vtable();
    hal_fill_modelanim2_vtable();

    port_ov002_patch();
    port_cross_patch();
    /* the overlay's static ctors give every static SharedFilePtr its ID */
    __sinit_ov002_02100560();
    __sinit_ov002_02100938();
    __sinit_ov002_02100adc();
    __sinit_ov002_02100c50();
    __sinit_ov002_02100d44();
    __sinit_ov002_02100e50();
    __sinit_ov002_02100ec4();
    __sinit_ov002_02100f84();
    __sinit_ov002_02101064();
    __sinit_ov002_02101478();
    __sinit_ov002_021014e4();
    __sinit_ov002_02101588();
    __sinit_ov002_02101738();
    __sinit_ov002_02101894();
    __sinit_ov002_02101900();
    __sinit_ov002_02101968();
    __sinit_ov002_021019d0();
    __sinit_ov002_02106e40();
    __sinit_ov002_02107118();
    __sinit_ov002_021071f4();
    __sinit_ov002_02107298();
    __sinit_ov002_02107304();
    __sinit_ov002_02107370();
    __sinit_ov002_02107f88();
    __sinit_ov002_0210804c();
    __sinit_ov002_02108094();

    data_02092144[0] = 8 << 8;               /* full health, high byte */

    /* spawn context: Player is actor 0 in the spawn table */
    data_020a4b54 = 0;
    static unsigned short spawn_info[4] = { 0, 0, 100, 100 };
    data_020a4bb8[0] = spawn_info;

    /* THE GAME HEAP, the ROM's own chain instead of an alias -- the same
       bring-up tests/walk_window.cpp does, so the smoke and the game configure
       the heap the same way. This line used to be
       `data_020a0eac_c = data_020a0ea0;`, which pointed the game-heap word
       straight at the root heap: every allocation came out of the whole host
       arena and the ROM's own heap object never existed. func_0201a054, the
       main.c boot spine, calls Heap::InitializeGameHeap(0x3b000, 0) instead --
       a hard immediate and a NULL parent, no arena arithmetic. See
       walk_window.cpp for the disassembly and slice_w1l3.txt for the verified
       encoding. ActorBase::operator new below takes the Player out of this
       heap by name -- Memory::Allocate(size, -4, data_020a0eac), a negative
       alignment, so from the tail -- which is what makes the carve exercised
       here and not merely performed. */
    _ZN4Heap18InitializeGameHeapEjPS_(0x3b000, 0);
    CHECK(data_020a0eac_c != NULL);
    if (!data_020a0eac_c) {
        fprintf(stderr, "InitializeGameHeap returned null -- no game heap\n");
        return 2;
    }
    fprintf(stderr, "[heap] game heap %p, 0x%x bytes, %u free after carve\n",
            data_020a0eac_c, 0x3b000u,
            _ZN22ExpandingHeapAllocator10MemoryLeftEv(
                *(void **)((char *)data_020a0eac_c + 0x14)));

    void *player = _ZN9ActorBasenwEj(0x800);
    CHECK(player != NULL);
    _ZN6PlayerC1Ev(player);
    printf("  player constructed at %p, vtable %p\n", player, *(void **)player);
    CHECK(*(void **)player != NULL);

    /* phase 2: resources + standing. The state machine is driven directly
       (ChangeState's PMF dispatch reads DS-baked bytes; see player_bridges) */
    int ir = hal_player_init_resources(player);
    printf("  InitResources -> %d\n", ir);
    CHECK(ir == 1);

    int wi = hal_player_st_wait_init(player);
    printf("  St_Wait_Init -> %d\n", wi);
    CHECK(wi == 1);
    for (int f = 0; f < 4; ++f) {
        int wm = hal_player_st_wait_main(player);
        printf("  St_Wait_Main frame %d -> %d\n", f, wm);
    }

    /* eaten-gate proof: exercise the REAL port_player_render_hidden on this
       constructed player, the predicate hal_render_player_world early-returns
       on. Not VS mode and no invincibility here, so a visible opaque player
       must read shown (0); setting Actor mFlags&0x10 (+0xb0, the ROM's
       held/eaten hide, _ZN6Player6RenderEv.cpp:58) or mOpacity==0 (+0x6f5,
       :56) must flip it to hidden. This is the St_InYoshiMouth victim's case
       the port used to draw riding the eater. State is saved and restored. */
    {
        char *pc = (char *)player;
        unsigned  saved_flags = *(unsigned *)(pc + 0xb0);
        unsigned char saved_op = *(unsigned char *)(pc + 0x6f5);

        *(unsigned char *)(pc + 0x6f5) = 0x1f;              /* opaque */
        *(unsigned *)(pc + 0xb0) = saved_flags & ~0x10u;    /* not held */
        int shown = port_player_render_hidden(player);

        *(unsigned *)(pc + 0xb0) = (saved_flags & ~0x10u) | 0x10u;  /* eaten */
        int hidden_flag = port_player_render_hidden(player);

        *(unsigned *)(pc + 0xb0) = saved_flags & ~0x10u;
        *(unsigned char *)(pc + 0x6f5) = 0;                 /* transparent */
        int hidden_op = port_player_render_hidden(player);

        *(unsigned *)(pc + 0xb0) = saved_flags;             /* restore */
        *(unsigned char *)(pc + 0x6f5) = saved_op;

        printf("  eaten-gate: opaque+unheld shown=%d, mFlags&0x10 hidden=%d, "
               "mOpacity==0 hidden=%d\n", shown, hidden_flag, hidden_op);
        CHECK(shown == 0);
        CHECK(hidden_flag != 0);
        CHECK(hidden_op != 0);
    }

    /* gate 11: a real floor first -- the castle grounds KCL through the
       gate-8 recipe, enabled in the global collision registry so the
       Player's ground probes find it */
    {
        static struct { unsigned short id; unsigned char refs; void *p; } kcl_ptr;
        extern void *_ZN13SharedFilePtr9ConstructEj(void *, unsigned);
        extern void _ZN12MeshColliderC1Ev(void *);
        extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *);
        extern void _ZN12MeshCollider7SetFileEP8KCL_FileR10CLPS_Block(
            void *, void *, void *);
        extern int _ZN16MeshColliderBase6EnableEP5Actor(void *, void *);
        _ZN13SharedFilePtr9ConstructEj(&kcl_ptr, 1941);
        static char mc_storage[0x60];
        _ZN12MeshColliderC1Ev(mc_storage);
        char *kcl = (char *)_ZN12MeshCollider8LoadFileER13SharedFilePtr(&kcl_ptr);
        CHECK(kcl != NULL);
        static char clps_storage[0x100];
        _ZN12MeshCollider7SetFileEP8KCL_FileR10CLPS_Block(mc_storage, kcl,
                                                          clps_storage);
        /* ROOT CAUSE (found 2026-08-02): the level collider's OWNER feeds
           func_02035354's self-collision exclusion. Enabling it with the
           player as owner makes every player ground/wall probe skip the
           level -- which is why the game's own tracking never grounded and
           the harness snap exists. A neutral stage owner turns the REAL
           collision on, but the newly-live grounded-physics paths hang
           (suspect: the div-52 walk-physics draft's ground branches), so
           real collision stays opt-in until that is run down. */
        static char stage_owner[0x200];
        _ZN16MeshColliderBase6EnableEP5Actor(
            mc_storage, getenv("SM64DS_REAL_CLSN") ? (void *)stage_owner
                                                   : (void *)player);
        /* stand Mario inside the octree box, above the floor plane */
        int ox = *(int *)(kcl + 0), oy = *(int *)(kcl + 4), oz = *(int *)(kcl + 8);
        unsigned xm = *(unsigned *)(kcl + 0x10), zm = *(unsigned *)(kcl + 0x18);
        int ex = (int)(~xm + 1) << 6, ez = (int)(~zm + 1) << 6;
        char *pc = (char *)player;
        *(int *)(pc + 0x5c) = ox + ex / 2;
        *(int *)(pc + 0x60) = oy + 0x8000;    /* 8 units up; he lands */
        *(int *)(pc + 0x64) = oz + ez / 2;
        printf("  floor staged: kcl origin (%d,%d,%d), mario at (%d,%d,%d)\n",
               ox, oy, oz, *(int *)(pc + 0x5c), *(int *)(pc + 0x60),
               *(int *)(pc + 0x64));
    }

    /* gate 11: stick forward, wait ticks into walk, position moves */
    char *c = (char *)player;
    data_020a0e40[0] = 0;
    *(short *)(data_0209f4a0 + 0) = 0x1000;      /* full stick magnitude */
    *(short *)(c + 0x69c) = 0x0000;              /* desired heading */
    int x0 = *(int *)(c + 0x5c), z0 = *(int *)(c + 0x64);
    hal_player_st_wait_main(player);             /* the tick that transitions */
    void *st = *(void **)(c + 0x370);
    printf("  after stick: state=%p (walk=%p)\n", st,
           (void *)data_ov002_0211013c);
    CHECK(st == (void *)data_ov002_0211013c);
    const char *bmp_dir = getenv("SM64DS_WALK_BMP_DIR");
    for (int f = 0; f < 30; ++f) {
        int r = hal_player_behavior(player);   /* the real frame tick */
        if (bmp_dir)
            render_walk_frame(player, bmp_dir, f);
        if (f < 3 || f == 29)
            printf("  Behavior frame %d -> %d pos=(%d, %d)\n", f, r,
                   *(int *)(c + 0x5c), *(int *)(c + 0x64));
    }
    int x1 = *(int *)(c + 0x5c), z1 = *(int *)(c + 0x64);
    printf("  walked: dx=%d dz=%d  y=%d  final state=%p (walk=%p fall=%p)\n",
           x1 - x0, z1 - z0, *(int *)(c + 0x60), *(void **)(c + 0x370),
           (void *)data_ov002_0211013c, (void *)data_ov002_02110424);
    CHECK(x1 != x0 || z1 != z0);

    if (g_failures) {
        fprintf(stderr, "smoke_player: %d FAILURE(S)\n", g_failures);
        return 1;
    }
    printf("smoke_player: Mario walks on the castle grounds "
           "(gates 10+11 GREEN)\n");
    return 0;
}
