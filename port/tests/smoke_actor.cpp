// Gate-9 smoke: a real actor lives its lifecycle on host.
//
// ArrowSignRight_Spawn allocates from the game heap and runs the ctor
// chain (Platform -> Actor -> ActorBase, Model and MovingMeshCollider
// subobjects, ShadowModel); then every lifecycle step dispatches THROUGH
// THE VTABLE exactly as the game's processing lists do: InitResources
// loads the sign's model and collider through the full asset pipeline,
// Behavior ticks, Render walks the model into the GX. This is the vtable
// law's first full-class workout -- the rung before Player.
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
int *ArrowSignRight_Spawn(void);
void *_ZN4Heap13SetupRootHeapEv(void);
extern int data_0209b3ec[12];       /* camera matrix */
/* the spawn context ActorDerived::Spawn would have staged */
struct SharedFilePtrC { unsigned short fileID; unsigned char numRefs;
                        unsigned char pad; void *filePtr; };
SharedFilePtrC *_ZN13SharedFilePtr9ConstructEj(SharedFilePtrC *s, u32 id);
extern unsigned short data_020a4b54;    /* pending actor ID */
extern void **data_020a4bb8;            /* actorID -> SpawnInfo* */
extern void *data_ov098_0213c380[6];    /* {model,kcl,?} entry table */
extern char data_ov098_0213c384[0x18];  /* column-b view of the same */
extern void *data_020a0eac_c;           /* actor heap = root heap */
extern void *data_020a0ea0;             /* defaultHeapPtr (gate 3a) */
extern void *data_0209f394[];           /* the player array */
extern unsigned char data_0209f21c;     /* player count */
void hal_fill_model_vtable(void);
void hal_fill_shadow_vtable(void);
void hal_fill_mmc_vtable(void);
}

/* the ActorBase virtual surface, MSVC view: dispatch helpers */
typedef int (__thiscall *Fn0)(void *);
static int vcall0(void *actor, int slot)
{
    void **vt = *(void ***)actor;
    return ((Fn0)vt[slot])(actor);
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

static void reset_scene()
{
    ntr::gx_reset();
    NTR_MMIO(uint32_t, 0x04000440) = 0;
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    NTR_MMIO(uint32_t, 0x04000440) = 1;
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    NTR_MMIO(uint32_t, 0x04000580) = 0u | (0u << 8) | (255u << 16) | (191u << 24);
    ntr::gx_set_light(0, -0.4f, -0.6f, -0.7f, 0x7FFF);
    ntr::gx_enable_lights(0x1);
}

int main(void)
{
    PORT_INSTALL_FAULT_PROBE();
    setvbuf(stdout, NULL, _IONBF, 0);
    if (!ntr::io_init()) { fprintf(stderr, "io_init failed\n"); return 2; }
    CHECK(_ZN4Heap13SetupRootHeapEv() != NULL);
    ident_fx(data_0209b3ec);

    /* stage the spawn context: actor 0x12b = ArrowSignRight, its SpawnInfo
       priorities, the actor heap, and the ov098 file entries (handle 1177 =
       yajirusi_r.bmd, 1178 = its kcl) in both column views */
    hal_fill_model_vtable();
    hal_fill_shadow_vtable();
    hal_fill_mmc_vtable();
    data_020a4b54 = 0x12b;
    static unsigned short spawn_info[4] = { 0, 0, 100, 100 };
    data_020a4bb8[0x12b] = spawn_info;
    data_020a0eac_c = data_020a0ea0;
    static SharedFilePtrC sign_model, sign_kcl;
    _ZN13SharedFilePtr9ConstructEj(&sign_model, 1177);
    _ZN13SharedFilePtr9ConstructEj(&sign_kcl, 1178);
    data_ov098_0213c380[0] = &sign_model;
    data_ov098_0213c380[1] = &sign_kcl;
    *(void **)(data_ov098_0213c384 + 0) = &sign_kcl;

    /* a stand-in player: the actor world assumes at least one exists
       (ClosestPlayer callers deref without a null check -- on the DS
       Mario always exists). Position at +0x5c, near the sign. */
    static char fake_player[0x800];
    data_0209f394[0] = fake_player;
    *(unsigned char *)&data_0209f21c = 1;

    int *actor = ArrowSignRight_Spawn();
    CHECK(actor != NULL);
    printf("  spawned at %p, vtable %p\n", (void *)actor, *(void **)actor);

    /* the lifecycle, dispatched as the processing lists would */
    int init_ok = vcall0(actor, 0);         /* InitResources */
    printf("  InitResources -> %d\n", init_ok);
    CHECK(init_ok == 1);

    reset_scene();
    for (int f = 0; f < 4; ++f) {
        printf("  tick %d\n", f);
        vcall0(actor, 6);                   /* Behavior */
    }
    printf("  render\n");
    int render_ok = vcall0(actor, 9);       /* Render */
    printf("  Render -> %d\n", render_ok);

    size_t tris = 0;
    ntr::gx_polygons(tris);
    printf("  triangles from the actor's render: %zu\n", tris);
    CHECK(tris > 0);

    if (g_failures) {
        fprintf(stderr, "smoke_actor: %d FAILURE(S)\n", g_failures);
        return 1;
    }
    printf("smoke_actor: all checks passed (an actor spawned, initialized, "
           "behaved and rendered through vtable dispatch)\n");
    return 0;
}
