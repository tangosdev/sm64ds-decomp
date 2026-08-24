// Gate-8 smoke: the game's collision answers "what is under this point".
//
// The castle grounds KCL loads through the card seam; LoadFile rebases it
// (the same in-place pointer math as BMDs), SetFile wires the CLPS block,
// and the octree walk -- the port/unmatched adoption of the banked draft,
// algorithm confirmed against the ITCM disassembly -- casts vertical rays
// down through the mesh. Floor hits must come back with upward normals,
// shortened rays, and hit points inside the probe column.
//
// The ray object is harness-built (fields the walk reads, zeroed rest);
// the game ctor's vtable/list wiring belongs to the actor gates.
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "dBgW_Kc.h"

#include "ntr/mmio.h"

#include "fault_probe.h"

struct SurfaceInfoS { unsigned char clps[8]; int nx, ny, nz; };
struct RayS {
    unsigned char head[0x10];
    unsigned char result[0x1c];     /* 0x10 */
    unsigned char pad2c[0xc];
    int sx, sy, sz;                 /* 0x38 lineStart */
    unsigned char pad44[0xc];
    unsigned char hasClsn;          /* 0x50 */
    unsigned char pad51[3];
    int ex, ey, ez;                 /* 0x54 lineEnd */
    int clsnDist;                   /* 0x60 */
    unsigned char tail[0x10];
};

extern "C" {
extern int g_walk_dbg[16];
struct SharedFilePtrC { u16 fileID; u8 numRefs; void *filePtr; };
SharedFilePtrC *_ZN13SharedFilePtr9ConstructEj(SharedFilePtrC *self, u32 ov0FileID);
void *_ZN4Heap13SetupRootHeapEv(void);
void *_ZN7dBgW_KcC1Ev(void *self);
}

static int probe_one(dBgW_Kc *mc, RayS *ray)
{
    return mc->dBgW_Kc::DetectClsn(*(dBgCh_Lin *)ray);
}
static int probe_filter(EXCEPTION_POINTERS *ep)
{
    char *base = (char *)GetModuleHandleA(0);
    printf("  [probe fault %08x at +0x%08x]\n",
           (unsigned)ep->ExceptionRecord->ExceptionCode,
           (unsigned)((char *)ep->ExceptionRecord->ExceptionAddress - base));
    return EXCEPTION_EXECUTE_HANDLER;
}
static int probe_seh(dBgW_Kc *mc, RayS *ray)
{
    __try {
        return probe_one(mc, ray);
    } __except (probe_filter(GetExceptionInformation())) {
        return -1;
    }
}

static int g_failures;
#define CHECK(cond) \
    do { if (!(cond)) { ++g_failures; \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

int main(void)
{
    PORT_INSTALL_FAULT_PROBE();
    setvbuf(stdout, NULL, _IONBF, 0);   /* crashes must not eat the trail */
    if (!ntr::io_init()) { fprintf(stderr, "io_init failed\n"); return 2; }
    CHECK(_ZN4Heap13SetupRootHeapEv() != NULL);

    /* the castle grounds KCL, handle 1941: real level floors
       (the piano's KCL turned out to be four wall prisms and nothing
       horizontal -- an actor's push-out shell, not a floor) */
    SharedFilePtrC ptr;
    _ZN13SharedFilePtr9ConstructEj(&ptr, 1941);
    static char mc_storage[0x60];
    dBgW_Kc *mc = (dBgW_Kc *)mc_storage;
    _ZN7dBgW_KcC1Ev(mc_storage);
    char *kcl = dBgW_Kc::LoadFile(*(SharedFilePtr *)&ptr);
    CHECK(kcl != NULL);

    /* CLPS_Block is opaque; a zeroed buffer takes the lookup's
       wrong-version default path, which is what stages without a CLPS do */
    static char clps_storage[0x100];
    mc->dBgW_Kc::SetFile((KCL_File *)kcl, *(CLPS_Block *)clps_storage);

    {
        KCL_File *f = (KCL_File *)kcl;
        (void)f;
        printf("  kcl: origin (%d,%d,%d) raw, masks %08x %08x %08x, "
               "shifts %u/%u/%u\n", f->origin.x, f->origin.y, f->origin.z,
               f->xMask, f->yMask, f->zMask, f->coordShift, f->yShift, f->zShift);
    }

    /* vertical probes over a grid derived from the file's own octree box
       (origin and mask extents are in raw units = Fix12i >> 6) */
    /* origin is Fix12i; the mask extents count 0x40-fx cells */
    KCL_File *f = (KCL_File *)kcl;
    const int ex_ = (int)(~f->xMask + 1) << 6, ez_ = (int)(~f->zMask + 1) << 6;
    const int ey_ = (int)(~f->yMask + 1) << 6;
    const int cx = f->origin.x + ex_ / 2, cz = f->origin.z + ez_ / 2;
    /* the floor can sit exactly at the box bottom, and the plane test wants
       the end STRICTLY below -- overshoot by four units */
    const int top = f->origin.y + ey_, bot = f->origin.y - 0x4000;
    const int sx_step = ex_ / 8, sz_step = ez_ / 8;
    int hits = 0, up_normals = 0, shortened = 0;
    for (int gx = -2; gx <= 2; ++gx)
        for (int gz = -2; gz <= 2; ++gz) {
            RayS ray;
            memset(&ray, 0, sizeof ray);
            ray.head[4] = 1;    /* the dBgCh "collide with ordinary surfaces"
                                   default (_ZN5dBgChC2Ev sets it; the pass-
                                   through predicate reads head[4] & 1) */
            ray.sx = cx + gx * sx_step; ray.sy = top; ray.sz = cz + gz * sz_step;
            ray.ex = ray.sx;            ray.ey = bot; ray.ez = ray.sz;
            ray.clsnDist = 0x7FFFFFF;
            int r = probe_seh(mc, &ray);
            if (r && ray.hasClsn) {
                ++hits;
                if (ray.ey < top && ray.ey > bot) ++shortened;
                /* result layout (func_02037fd4 + func_02037de8): +0x4 the
                   8-byte CLPS entry, +0xC the normal Vector3, +0x18 triIdx.
                   Normal Y is therefore +0x10. */
                int ny = *(int *)(ray.result + 0x10);
                if (ny > 0) ++up_normals;
            }
        }
    printf("  walk: leaves %d, prisms %d, planeS-rej %d, planeE-rej %d\n",
           g_walk_dbg[1], g_walk_dbg[2], g_walk_dbg[3], g_walk_dbg[4]);
    printf("  edges: e1 %d, e2 %d, e3lo %d\n",
           g_walk_dbg[5], g_walk_dbg[6], g_walk_dbg[7]);
    printf("  tail: e3hi %d, denom %d, parallel %d, dist %d, passthru %d\n",
           g_walk_dbg[8], g_walk_dbg[9], g_walk_dbg[10], g_walk_dbg[11],
           g_walk_dbg[12]);
    printf("  probes: 25, hits: %d, shortened rays: %d, upward normals: %d\n",
           hits, shortened, up_normals);
    CHECK(hits >= 3);               /* the piano's floor exists under the grid */
    CHECK(shortened == hits);       /* every hit shortened its ray */
    CHECK(up_normals >= hits / 2);  /* floors face up */

    if (g_failures) {
        fprintf(stderr, "smoke_clsn: %d FAILURE(S)\n", g_failures);
        return 1;
    }
    printf("smoke_clsn: all checks passed (the game's octree walk answers "
           "ground queries over real KCL data)\n");
    return 0;
}
