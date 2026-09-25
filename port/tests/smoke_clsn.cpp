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

// THE BRING-UP ENTRY POINT MOVED, and this harness follows it (lane SMOKELINK3,
// run link100 wave 10 round 2, continuing SMOKELINK2's rule for
// port/tests/smoke_heap.cpp). It used to declare the entry as a flat extern "C"
// Itanium name returning an opaque pointer, which is what the ROM's own callers
// spell and what src/ emitted before the 09-14 sync. It is Heap::SetupRootHeap()
// now, a real static member (include/Heap.h:236, ?SetupRootHeap@Heap@@SAPAU1@XZ),
// so the flat spelling bought a link error. This file is HOST TEST CODE, not ROM
// code, so it may include the class header and call the member the way C++ calls
// it, which is also what the synced src/ TUs now do. It reaches the same object
// it always linked, src/_ZN4Heap13SetupRootHeapEv.cpp; only the spelling changed.
#include "types.h"
#include "Heap.h"

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

/* dBgCh_SphCrr as the slot-8 pass reads it. Offsets, not fields: the same
   discipline port/unmatched/MeshCollider_DetectClsn_Sphere.cpp uses, and the
   same reason (include/dBgCh_SphCrr.h is auto-generated and has neither the
   centre nor the radius). 0x140 bytes so the three ClsnResults at 0x74/0x9c/
   0xc4 and the watermarks at 0xfc..0x108 all land inside. */
struct SphereS {
    unsigned char raw[0x140];

    void seed(int x, int y, int z, int r)
    {
        memset(raw, 0, sizeof raw);
        raw[4] = 1;                         /* _ZN5dBgChC2Ev's BgCh default */
        at(0x3c) = x; at(0x40) = y; at(0x44) = z;
        at(0x48) = r;
        /* func_02037b5c: flags cleared, best-floor-normal seeded (0,-1,0) */
        at(0xfc) = 0; at(0x100) = -0x1000; at(0x104) = 0;
        /* SetObjAndSphere's own seed */
        at(0x108) = 0x1000;
    }
    int &at(int off) { return *(int *)(raw + off); }
    unsigned char flags() const { return raw[0x70]; }
    /* func_02037a38: the resolved push is pushMin + pushMax */
    int push(int c) { return at(0x58 + c * 4) + at(0x64 + c * 4); }
    int lastTri() const { return *(const unsigned short *)(raw + 0x10 + 0x18); }
};

extern "C" {
extern int g_walk_dbg[16];
int func_02039794(int normalY);     /* 0 floor / 1 wall / 2 ceiling */
struct SharedFilePtrC { u16 fileID; u8 numRefs; void *filePtr; };
SharedFilePtrC *_ZN13SharedFilePtr9ConstructEj(SharedFilePtrC *self, u32 ov0FileID);
void *_ZN7dBgW_KcC1Ev(void *self);
}

/* THE FILE/WORLD BOUNDARY, and it is these two functions only.
   Everything this smoke computes -- the octree box, the cell size, the
   max-depth-behind-plane, every probe offset -- is derived from the KCL
   header and is therefore in FILE units, which is the basis the walk's own
   arithmetic uses and the basis these checks are written in. The walk's
   ARGUMENTS are world Fix12i: it converts them with the ROM's plain <<6/>>6
   (port/unmatched/MeshCollider_DetectClsn_Sphere.cpp, BASIS CONVENTION).
   Before that shift became unconditional the smoke got the identity for free
   by leaving the collider at SetFile's 1.0 scale pair -- which was also what
   hid every actor-owned collider from every ray in the game. So the identity
   is spelled out here instead: lengths go in <<6 and come back >>6, and not
   one line of the tiers below has to change basis. */
enum { F2W = 6 };
static void v3_shl(int *v, int n) { v[0] <<= n; v[1] <<= n; v[2] <<= n; }
static void v3_shr(int *v, int n) { v[0] >>= n; v[1] >>= n; v[2] >>= n; }

static int sphere_probe(dBgW_Kc *mc, SphereS *s)
{
    v3_shl(&s->at(0x3c), F2W);              /* centre */
    s->at(0x48) <<= F2W;                    /* radius */
    s->at(0xec) <<= F2W;                    /* wallHeight */
    int r = mc->dBgW_Kc::DetectClsn(*(dBgCh_SphCrr *)s->raw);
    v3_shr(&s->at(0x3c), F2W);
    s->at(0x48) >>= F2W;
    s->at(0xec) >>= F2W;
    v3_shr(&s->at(0x4c), F2W);              /* resolvedPush */
    v3_shr(&s->at(0x58), F2W);              /* pushMin */
    v3_shr(&s->at(0x64), F2W);              /* pushMax */
    return r;
}

static int probe_one(dBgW_Kc *mc, RayS *ray)
{
    v3_shl(&ray->sx, F2W);
    v3_shl(&ray->ex, F2W);
    int r = mc->dBgW_Kc::DetectClsn(*(dBgCh_Lin *)ray);
    v3_shr(&ray->sx, F2W);
    v3_shr(&ray->ex, F2W);                  /* the walk writes the hit here */
    return r;
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
    CHECK(Heap::SetupRootHeap() != NULL);

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
            ray.head[4] = 1;    /* the BgCh "collide with ordinary surfaces"
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

    /* ================= TIER 1: the sphere pass against the line walk =======
       The two passes read the SAME KCL through the SAME collider, so the line
       walk is the only ground truth available without an emulator -- and a
       strong one, because a disagreement here is a disagreement about where
       the level is. See port/unmatched/MeshCollider_DetectClsn_Sphere.cpp.

       UNITS. This smoke leaves the collider at SetFile's stock 1.0 pair, so
       its world space IS the KCL's file space and every length below is in
       file units: one octree cell is 0x40 of them, and the file's own
       max-depth-behind-plane (unk_10, in the dot basis) is unk_10/0x400 of
       them. Sizing a probe in world units instead is how the first draft of
       this block managed to place every "just below the floor" point deeper
       than the game will look. */
    const int maxDepth = f->unk_10 / 0x400;     /* file units */
    printf("  kcl: unk_10 %d dot basis = %d file units behind the plane; "
           "cell 0x40\n", f->unk_10, maxDepth);
    CHECK(maxDepth > 16);

    /* The columns Tier 1 works over: wherever a downward ray finds a floor.
       TWO rays per column, and the second one is not optional. The walk's
       crossing point comes out of `t = fdiv(dotS >> 4, denom) << 4`, which
       carries twelve bits of fraction, so the hit is quantised to
       raylength/4096 -- over the octree box's full 1,064,960-unit height that
       is ~260 units, and a "just below the floor" probe sized in tens of units
       lands on the WRONG SIDE of the plane. (The game never sees it: its rays
       are one frame of motion long. The harness's full-height probe is what
       exposes it.) A second 1024-unit ray around the first answer brings the
       quantisation to a quarter unit. */
    struct Column { int x, z, y, tri, ny; };
    static Column col[81];
    int ncol = 0;
    for (int gx = -4; gx <= 4; ++gx)
        for (int gz = -4; gz <= 4; ++gz) {
            RayS ray;
            memset(&ray, 0, sizeof ray);
            ray.head[4] = 1;
            ray.sx = cx + gx * (sx_step / 2); ray.sy = top;
            ray.sz = cz + gz * (sz_step / 2);
            ray.ex = ray.sx; ray.ey = bot; ray.ez = ray.sz;
            ray.clsnDist = 0x7FFFFFF;
            if (!probe_seh(mc, &ray) || !ray.hasClsn) continue;
            const int coarse = ray.ey;
            RayS fine;
            memset(&fine, 0, sizeof fine);
            fine.head[4] = 1;
            fine.sx = ray.sx; fine.sy = coarse + 512; fine.sz = ray.sz;
            fine.ex = ray.sx; fine.ey = coarse - 512; fine.ez = ray.sz;
            fine.clsnDist = 0x7FFFFFF;
            if (!probe_seh(mc, &fine) || !fine.hasClsn) continue;
            col[ncol].x = fine.sx;
            col[ncol].z = fine.sz;
            col[ncol].y = fine.ey;      /* the walk writes the hit as the end */
            col[ncol].tri = *(unsigned short *)(fine.result + 0x18);
            col[ncol].ny = *(int *)(fine.result + 0x10);
            ++ncol;
        }

    /* PORT_EXPLAIN_CLSN: replay the ROM's per-prism rejects by hand, from the
       KCL, for the triangle the line walk reported. Says which gate fires. */
    if (getenv("PORT_EXPLAIN_CLSN"))
        for (int i = 0; i < ncol && i < 6; ++i) {
            KCL_Tri *pr = &f->tris[col[i].tri];
            const int *v0 = (const int *)f->positions[pr->posIdx];
            const short *e1 = (const short *)f->normals[pr->edgeNormal1Idx];
            const short *e2 = (const short *)f->normals[pr->edgeNormal2Idx];
            const short *e3 = (const short *)f->normals[pr->edgeNormal3Idx];
            const short *fn = (const short *)f->normals[pr->normalIdx];
            const int rx = col[i].x - v0[0];
            const int ry = (col[i].y - 8) - v0[1];
            const int rz = col[i].z - v0[2];
            const int len = *(const int *)pr;
            printf("    explain col %d tri %d: v0(%d,%d,%d) len %d\n"
                   "      d1 %d d2 %d d3 %d faceDot %d   (R = 0)\n",
                   i, col[i].tri, v0[0], v0[1], v0[2], len,
                   e1[0]*rx + e1[1]*ry + e1[2]*rz,
                   e2[0]*rx + e2[1]*ry + e2[2]*rz,
                   e3[0]*rx + e3[1]*ry + e3[2]*rz - len,
                   fn[0]*rx + fn[1]*ry + fn[2]*rz);
        }

    /* 1.1  A radius-0 sphere is a point test. Just BELOW a floor the line walk
       found it must report that same triangle; just ABOVE it, with R = 0 and
       the `faceDot > R` reject strict, it must report nothing at all. */
    {
        int agree = 0, disagree = 0, missed = 0, above_hits = 0;
        const int eps = 8;                      /* file units, well inside
                                                   maxDepth */
        for (int i = 0; i < ncol; ++i) {
            SphereS sp;
            sp.seed(col[i].x, col[i].y - eps, col[i].z, 0);
            const int m = sphere_probe(mc, &sp);
            if (getenv("PORT_TRACE_CLSN"))
                printf("    col %2d (%d,%d,%d) tri %d ny %d -> mask %d tri %d\n",
                       i, col[i].x, col[i].y, col[i].z,
                       col[i].tri, col[i].ny, m, sp.lastTri());
            if (!m) ++missed;
            else if (sp.lastTri() == col[i].tri) ++agree;
            else ++disagree;

            sp.seed(col[i].x, col[i].y + eps, col[i].z, 0);
            if (sphere_probe(mc, &sp)) ++above_hits;
        }
        printf("  tier1.1: %d columns, agree %d, disagree %d, missed %d, "
               "false hits above %d\n", ncol, agree, disagree, missed,
               above_hits);
        CHECK(ncol >= 8);
        CHECK(disagree == 0);       /* never a DIFFERENT triangle */
        CHECK(agree >= ncol - ncol / 4);  /* a column landing on an edge can
                                             miss a zero-radius point test */
        CHECK(above_hits == 0);     /* a point above the surface touches nothing */
    }

    /* 1.2  RETIRED (run linkfull, lane RS5B). It compared the number of
       CONTACTS the walk made between radii that visited the same number of
       LEAVES, and both numbers were counters inside the sphere pass's host
       transcription (g_sphere_dbg). The pass is the ROM's own matched body
       now, which keeps no counters, and nothing outside the walk can see
       either number, so the check has nothing left to read. 1.1 and 1.3 to
       1.6 grade what the body answers, which is all a caller ever sees. */

    /* 1.3  The push identity, which is what actually pins R = radius<<4 and
       the >>16 push scaling. For a FACE contact on a level floor,
       depth = R - faceDot and push.y = depth*normal.y >> 16 collapse to
           push.y == radius - height
       in the collider's own world units, with no scale factor left over. A
       wrong shift shows up here as a factor of 4, 16 or 64, not as rounding.
       Only a LOWER bound is guaranteed: a higher ledge inside the radius
       pushes harder, so `got < expect` is the failure and the exact-match
       count is the evidence. */
    {
        int tested = 0, exact = 0, under = 0, worstUnder = 0;
        for (int i = 0; i < ncol; ++i) {
            if (col[i].ny < 0xff0) continue;    /* level triangles only */
            for (int k = 0; k < 4; ++k) {
                const int h = 8 << k;           /* 8, 16, 32, 64 file units */
                const int r = 0x80;             /* two cells */
                SphereS sp;
                sp.seed(col[i].x, col[i].y + h, col[i].z, r);
                if (!(sphere_probe(mc, &sp) & 1)) continue;
                if (sp.at(0x5c) != 0) continue; /* pushMin.y != 0: something
                                                   is pushing DOWN too */
                const int expect = r - h;
                const int got = sp.push(1);
                ++tested;
                if (got == expect) ++exact;
                else if (got < expect) {
                    ++under;
                    if (expect - got > worstUnder) worstUnder = expect - got;
                }
            }
        }
        printf("  tier1.3: %d level-floor samples, exact %d, under %d "
               "(worst shortfall %d)\n", tested, exact, under, worstUnder);
        CHECK(tested >= 8);
        CHECK(under == 0);              /* the identity is a hard lower bound */
        CHECK(exact >= tested / 2);     /* and usually exact */
    }

    /* 1.4  Actor-sized spheres, which is what actually reaches the edge and
       corner solves and the slope gate -- a small sphere resting on open floor
       only ever makes FACE contacts, so 1.1..1.3 leave two thirds of the
       function untested. The invariant here is a bound rather than an
       identity: depth = R - faceDot and faceDot >= -unk_10, so no single push
       component can exceed radius + unk_10, and the min/max pair cannot exceed
       twice that. Anything that overflows or reads the wrong basis blows this
       out by orders of magnitude, which is the point. */
    {
        int probes = 0, contacts = 0, overBound = 0, worstPush = 0;
        for (int i = 0; i < ncol; ++i)
            for (int k = 0; k < 3; ++k) {
                const int r = 0x40 << k;            /* 1, 2, 4 cells */
                for (int dy = -r / 2; dy <= r; dy += r / 2) {
                    SphereS sp;
                    sp.seed(col[i].x, col[i].y + dy, col[i].z, r);
                    ++probes;
                    if (!sphere_probe(mc, &sp)) continue;
                    ++contacts;
                    const int bound = 2 * (r + maxDepth);
                    for (int c = 0; c < 3; ++c) {
                        int p = sp.push(c);
                        if (p < 0) p = -p;
                        if (p > worstPush) worstPush = p;
                        if (p > bound) ++overBound;
                    }
                }
            }
        printf("  tier1.4: %d actor-sized probes, %d with contact, push bound "
               "violations %d (worst |push| %d)\n",
               probes, contacts, overBound, worstPush);
        CHECK(contacts >= ncol);
        CHECK(overBound == 0);
    }

    /* 1.5  EDGE and CORNER coverage. Castle grounds' prisms are hundreds of
       thousands of file units across, so an actor-sized sphere dropped on one
       is always deep inside it laterally and every contact classifies FACE --
       1.1..1.4 never reach the two thirds of the function that solve for the
       nearest point on an edge or at a vertex. So aim at them: d1, d2 and d3
       ARE the signed distances to the three edge planes in the dot basis, and
       the edge normals are unit vectors at 1.0 == 0x400 lying in the prism's
       plane, so the centre can be driven to a chosen distance from a chosen
       edge directly. One edge -> the EDGE region; two at once -> the VERTEX
       region and the corner solve. */
    {
        int aimed = 0, contacts = 0, overBound = 0;
        for (int i = 0; i < ncol; ++i) {
            KCL_Tri *pr = &f->tris[col[i].tri];
            const int *p0 = (const int *)f->positions[pr->posIdx];
            const short *e1 = (const short *)f->normals[pr->edgeNormal1Idx];
            const short *e2 = (const short *)f->normals[pr->edgeNormal2Idx];
            const short *fn = (const short *)f->normals[pr->normalIdx];
            for (int rk = 0; rk < 2; ++rk) {
                const int r = 0x100 << rk;          /* 4 and 8 cells */
                const int R = r * 0x400;            /* the dot basis radius */
                for (int both = 0; both < 2; ++both)
                    for (int dn = 1; dn <= 6; ++dn)
                        for (int fdn = 1; fdn <= 4; ++fdn) {
                            /* start on the prism at the ray's hit, then drive
                               d1 (and d2) to dn/8 of R and faceDot to fdn/5 */
                            int c[3] = { col[i].x, col[i].y, col[i].z };
                            const int wantD = (int)((long long)R * dn / 8);
                            const int wantF = (int)((long long)R * fdn / 5);
                            for (int it = 0; it < 6; ++it) {
                                const int rx = c[0] - p0[0], ry = c[1] - p0[1],
                                          rz = c[2] - p0[2];
                                const int d1 = e1[0]*rx + e1[1]*ry + e1[2]*rz;
                                const int d2 = e2[0]*rx + e2[1]*ry + e2[2]*rz;
                                const int fd = fn[0]*rx + fn[1]*ry + fn[2]*rz;
                                const int m1 = (wantD - d1) / 0x400;
                                const int m2 = both ? (wantD - d2) / 0x400 : 0;
                                const int mf = (wantF - fd) / 0x400;
                                for (int a = 0; a < 3; ++a)
                                    c[a] += (int)(((long long)m1 * e1[a]
                                                 + (long long)m2 * e2[a]
                                                 + (long long)mf * fn[a]) / 0x400);
                            }
                            SphereS sp;
                            sp.seed(c[0], c[1], c[2], r);
                            ++aimed;
                            if (!sphere_probe(mc, &sp)) continue;
                            ++contacts;
                            const int bound = 2 * (r + maxDepth);
                            for (int a = 0; a < 3; ++a) {
                                int p = sp.push(a);
                                if (p < 0) p = -p;
                                if (p > bound) ++overBound;
                            }
                        }
            }
        }
        printf("  tier1.5: %d aimed probes, %d with contact, push bound "
               "violations %d\n", aimed, contacts, overBound);
        CHECK(overBound == 0);
        /* The aimed probes sit in the EDGE and VERTEX regions by
           construction, so a contact here is an edge or corner contact the
           body accepted. Which region and which solve ran were counters in
           the retired host transcription (run linkfull, lane RS5B); the
           ROM's own body keeps none, so the answer is what is graded. */
        CHECK(contacts > 0);
    }

    /* 1.6  The WALL and CEILING arms of the record. Every probe so far has
       landed on a floor, so func_02039794's other two answers -- and with them
       the XZ-only push for a wall FACE contact and the XYZ push for a ceiling
       -- have never run. Walk the triangle table, classify each prism by its
       own normal, and aim a sphere at the first few of each kind: start at
       vertex 0 (where d1 and d2 are both zero by construction), drive well
       inside along -e1 and -e2, then out along the face normal to put the
       centre a half radius off the plane.

       The wall assertion is the sharp one. A wall FACE contact touches the X
       and Z accumulators and NOTHING else, so push.y must be zero to the
       unit; a non-zero Y on a flat wall face is the 0x01ffd0f0 class-1 branch
       transcribed as the class-2/3 branch at 0x01ffd170. */
    {
        const int ntri = (int)(((const char *)f->unk_0c
                              - (const char *)f->tris) / 0x10);
        printf("  tier1.6: %d prisms in the table\n", ntri);
        CHECK(ntri > 64 && ntri < 100000);
        int walls = 0, ceils = 0, wallOK = 0, ceilOK = 0;
        int wallFaceY = 0, wallFaceYbad = 0, ceilDownOK = 0;
        for (int t = 0; t < ntri && (walls < 24 || ceils < 24); ++t) {
            KCL_Tri *pr = &f->tris[t];
            const int *p0 = (const int *)f->positions[pr->posIdx];
            const short *e1 = (const short *)f->normals[pr->edgeNormal1Idx];
            const short *e2 = (const short *)f->normals[pr->edgeNormal2Idx];
            const short *fn = (const short *)f->normals[pr->normalIdx];
            const int kind = func_02039794(fn[1] << 2);   /* KCL 0x400 -> Fix12i */
            if (kind == 0) continue;
            if (kind == 1 && walls >= 24) continue;
            if (kind == 2 && ceils >= 24) continue;
            if (kind == 1) ++walls; else ++ceils;

            const int r = 0x100, R = r * 0x400;
            int c[3] = { p0[0], p0[1], p0[2] };
            for (int it = 0; it < 6; ++it) {
                const int rx = c[0]-p0[0], ry = c[1]-p0[1], rz = c[2]-p0[2];
                const int d1 = e1[0]*rx + e1[1]*ry + e1[2]*rz;
                const int d2 = e2[0]*rx + e2[1]*ry + e2[2]*rz;
                const int fd = fn[0]*rx + fn[1]*ry + fn[2]*rz;
                const int m1 = (-4*R - d1) / 0x400;
                const int m2 = (-4*R - d2) / 0x400;
                const int mf = (R/2 - fd) / 0x400;
                for (int a = 0; a < 3; ++a)
                    c[a] += (int)(((long long)m1 * e1[a]
                                 + (long long)m2 * e2[a]
                                 + (long long)mf * fn[a]) / 0x400);
            }
            SphereS sp;
            sp.seed(c[0], c[1], c[2], r);
            const int m = sphere_probe(mc, &sp);
            if (kind == 1) {
                if ((m & 2) && (sp.flags() & 8)) ++wallOK;
                /* only this wall. It used to be isolated to a FACE contact
                   too, by the edge / corner counters of the retired host
                   transcription (run linkfull, lane RS5B); the ROM body keeps
                   none, so an edge contact on a thin wall can land here and
                   its push.y is legitimately nonzero. Reported, not checked. */
                if (m == 2) {
                    ++wallFaceY;
                    if (sp.push(1) != 0) ++wallFaceYbad;
                }
            } else {
                if ((m & 4) && (sp.flags() & 0x10)) ++ceilOK;
                if (m == 4 && sp.push(1) < 0) ++ceilDownOK;
            }
        }
        printf("           walls aimed %d (mask bit1 + flag %d, isolated face "
               "contacts %d with %d bad push.y)\n",
               walls, wallOK, wallFaceY, wallFaceYbad);
        printf("           ceilings aimed %d (mask bit2 + flag %d, %d pushing "
               "DOWN)\n", ceils, ceilOK, ceilDownOK);
        CHECK(walls > 0);
        CHECK(ceils > 0);               /* castle grounds has ceilings */
        CHECK(wallOK >= walls / 2);
        CHECK(ceilOK >= ceils / 2);
        CHECK(ceilDownOK > 0);          /* a ceiling pushes down */
    }

    if (g_failures) {
        fprintf(stderr, "smoke_clsn: %d FAILURE(S)\n", g_failures);
        return 1;
    }
    printf("smoke_clsn: all checks passed (the game's octree walk answers "
           "ground queries over real KCL data)\n");
    return 0;
}
