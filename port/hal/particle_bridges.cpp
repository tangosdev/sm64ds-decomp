// Gate-29 seams: C-linkage faces for the particle callbacks that are METHODS,
// and host bodies for the two fx-math asm primitives the effect VM reaches.
//
// FACES, and why an alias would not do. Four of the thirteen callback bodies
// are compiled from .cpp files that declare a real class, so MSVC emits them
// __thiscall (`this` in ECX). Everything that calls them -- the vtables in
// hal/particle_vtable.cpp and the tracker's own walk in func_02021bec --
// reaches them by Itanium C name through a plain cdecl function pointer, with
// self pushed on the stack. A /alternatename: alias would link cleanly and
// then read `this` out of ECX on every call, so each one gets a real
// forwarding face instead. The shadow classes below are copied from the
// defining TUs; only the layout that the forwarded call needs is spelled.
#include "Model.h"

namespace Memory {
void *Allocate(unsigned size);   /* defined in src/_ZN6Memory8AllocateEj.cpp */
}

namespace Particle {
struct System;
/* Particle::CheckLavaCallback::SpawnParticles chains to its SimpleCallback
   base as a C++ method, and the src file that used to define that method is
   gone (an ARM ride-through -- see port/unmatched/Particle_RideThroughs.cpp).
   So the METHOD face forwards into the C-named host copy: the one direction
   this file does not otherwise go. */
struct SimpleCallback {
    char pad_00[4];
    short field_4;
    void SpawnParticles(System &sys);
};
struct CheckLavaCallback {
    void SpawnParticles(System &sys);
};
struct CleanParticleCallback {
    int OnUpdate(System &sys, bool done);
};
struct CheckWaterRippleCallback {
    bool OnUpdate(System &sys, bool b);
};
}  // namespace Particle

extern "C" {

void _ZN8Particle14SimpleCallback14SpawnParticlesERNS_6SystemE(void *, void *);

/* NO C-NAMED FACE for Particle::SimpleCallback::SpawnParticles -- slot 0 of the
   SimpleCallback, SplashCallback, BubbleCallback and FitWaterSimpleCallback
   vtables, the one the ROM shares across all four. Its src body passes an
   argument it never names (an ARM r1 ride-through), so it is a host copy in
   port/unmatched/Particle_RideThroughs.cpp, already C-named and cdecl. The
   METHOD face for it is below, outside this extern "C" block. */

/* slot 0 of the CheckLavaCallback vtable */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
void _ZN8Particle17CheckLavaCallback14SpawnParticlesERNS_6SystemE(void *self,
                                                                  void *sys)
{
    ((Particle::CheckLavaCallback *)self)
        ->SpawnParticles(*(Particle::System *)sys);
}
#else
void _ZN8Particle17CheckLavaCallback14SpawnParticlesERNS_6SystemE(void *self,
                                                                  void *sys);  /* Linux: real symbol from src/_ZN8Particle17CheckLavaCallback14SpawnParticlesERNS_6SystemE */
#endif /* _WIN32 */

/* slot 1 of the CleanParticleCallback vtable */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN8Particle21CleanParticleCallback8OnUpdateERNS_6SystemEb(void *self,
                                                               void *sys,
                                                               int done)
{
    return ((Particle::CleanParticleCallback *)self)
        ->OnUpdate(*(Particle::System *)sys, done != 0);
}
#else
int _ZN8Particle21CleanParticleCallback8OnUpdateERNS_6SystemEb(void *self,
                                                               void *sys,
                                                               int done);  /* Linux: real symbol from src/_ZN8Particle21CleanParticleCallback8OnUpdateERNS_6SystemEb */
#endif /* _WIN32 */

/* slot 1 of the CheckWaterRippleCallback vtable */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN8Particle24CheckWaterRippleCallback8OnUpdateERNS_6SystemEb(void *self,
                                                                  void *sys,
                                                                  int b)
{
    return ((Particle::CheckWaterRippleCallback *)self)
               ->OnUpdate(*(Particle::System *)sys, b != 0)
               ? 1
               : 0;
}
#else
int _ZN8Particle24CheckWaterRippleCallback8OnUpdateERNS_6SystemEb(void *self,
                                                                  void *sys,
                                                                  int b);  /* Linux: real symbol from src/_ZN8Particle24CheckWaterRippleCallback8OnUpdateERNS_6SystemEb */
#endif /* _WIN32 */

/* Both are static/namespace-scope and therefore already cdecl, but the
   defining TUs are C++ so the Itanium name the particle sources spell has
   nothing behind it. Two-line faces beat guessing a decorated name. */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
unsigned _ZN5Model13GetVramOffsetEj(unsigned size)
{
    return Model::GetVramOffset(size);
}
#else
unsigned _ZN5Model13GetVramOffsetEj(unsigned size);  /* Linux: real symbol from src/_ZN5Model13GetVramOffsetEj */
#endif /* _WIN32 */

void *_ZN6Memory8AllocateEj(unsigned size) { return Memory::Allocate(size); }

/* cstd::div wears its pre-naming address in func_0204dab4, the effect VM's
   scale ramp: `0x1000 - func_02052f4c(f2e << 12, f2c)`. 0x02052f4c IS
   cstd::div -- symbols.txt names it, that source file just predates the
   naming. The body is src/_ZN4cstd3divEii.c in this gate's slice (it drives
   the DS hardware divider through raw MMIO, so it needs the hostgen
   routing); this is only the name the caller spells. */
int _ZN4cstd3divEii(int a, int b);
int func_02052f4c(int a, int b) { return _ZN4cstd3divEii(a, b); }

/* ---- two fx-math asm primitives ------------------------------------------
   Both are Thumb `asm` blocks in src/ (asm-primitive policy: Nintendo shipped
   them as assembly, so there is no C to recover and no match to chase), which
   makes them exactly the thing a host compiler can never consume. They build
   a 3x3 fixed-point rotation matrix, 4096 = 1.0, and the particle render
   family calls them per billboard to face the camera. Transcribed from the
   register writes in the asm blocks, offset for offset. */

/* PORT_HOST_ABI: ARM/Thumb asm primitive (3x3 Z-rotation), MSVC cannot assemble.
   Matrix3x3_SetRotationZ @ 0x02052588: {c, s, 0, -s, c, 0, 0, 0, 1} */
void Matrix3x3_SetRotationZ(int *m, int s, int c)
{
    m[0] = c;  m[1] = s;  m[2] = 0;
    m[3] = -s; m[4] = c;  m[5] = 0;
    m[6] = 0;  m[7] = 0;  m[8] = 0x1000;
}

/* func_0205256c @ 0x0205256c: the Y sibling, {c, 0, -s, 0, 1, 0, s, 0, c}.
   The asm writes by byte offset (m[8] is [r0,#32], m[6] is [r0,#24], m[2] is
   [r0,#8], m[4] is [r0,#16]); this is that, in order.
   PORT_HOST_ABI: ARM/Thumb asm primitive (3x3 Y-rotation), MSVC cannot assemble. */
void func_0205256c(int *m, int s, int c)
{
    m[0] = c;  m[1] = 0;      m[2] = -s;
    m[3] = 0;  m[4] = 0x1000; m[5] = 0;
    m[6] = s;  m[7] = 0;      m[8] = c;
}

}  /* extern "C" */

/* the method face named above: CheckLavaCallback's own SpawnParticles calls
   this on its base, and the host copy is what actually runs */
void Particle::SimpleCallback::SpawnParticles(System &sys)
{
    _ZN8Particle14SimpleCallback14SpawnParticlesERNS_6SystemE(this, &sys);
}

// ---- the lifecycle seams ---------------------------------------------------
//
// WHERE THE ROM PUTS THEM, read out of the relocation table rather than
// guessed. Three call sites, all on the Stage:
//
//   0x0202d3dc  Stage::InitResources    -> Particle::SysTracker::Initialise
//   0x0202b930  Stage::Render           -> Particle::SysTracker::Update
//   0x02029840  Stage::GraphCallback1   -> Particle::RenderAll
//
// The port drives the frame by hand, so the two per-frame calls are made
// where those two Stage methods would have run: both inside the render pass,
// Update before RenderAll. Initialise goes at the end of port_stage_a_boot,
// which is where Stage::InitResources calls it -- after the level's model,
// collision, objects, fog and skybox are all loaded, because the particle
// archive's textures are uploaded into VRAM the level has already banked.
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "ntr/gx.h"

extern "C" {
void _ZN8Particle10SysTracker10InitialiseEv(void *self);
void _ZN8Particle10SysTracker6UpdateEv(void *self);
void _ZN8Particle9RenderAllEv(void);
void port_particle_vtables_check(void);
void *port_stage_object(void);
extern int data_0209b3ec[];    /* the engine view matrix, ROM scene units */
/* the two VRAM cursor sets Initialise bridges between (hal/model_host.cpp) */
extern unsigned data_020a4bc8, data_020a4bcc, data_020a4bd8, data_020a4be0;
extern unsigned data_0209ee88, data_0209ee84, data_0209ee8c;
extern void *data_0209ee74;    /* the tracker, set by SysTracker::SysTracker */
}

/* Stage+0x50 is the SysTracker sub-object; Stage::InitResources spells the
   same `(char *)this + 0x50`. */
static char *tracker(void)
{
    char *stage = (char *)port_stage_object();
    return stage ? stage + 0x50 : 0;
}

/* SM64DS_FX_TRACE=1 prints the live counts, =2 adds a per-system dump of the
   fields the emitter reads (the pair that found the ride-through). */
static int fx_trace(void)
{
    static int lvl = -1;
    if (lvl < 0) {
        const char *s = std::getenv("SM64DS_FX_TRACE");
        lvl = s ? std::atoi(s) : 0;
        if (s && lvl < 1) lvl = 1;
    }
    return lvl;
}

extern "C" void port_particle_boot(void)
{
    char *t = tracker();
    if (!t) {
        std::fprintf(stderr, "  [fx] NO STAGE: the particle tracker has no "
                     "home, subsystem left down\n");
        return;
    }
    /* the vtables the constructor already installed, checked before the
       first dispatch can reach one */
    port_particle_vtables_check();
    if ((char *)data_0209ee74 != t)
        std::fprintf(stderr, "  [fx] TRACKER GLOBAL MISMATCH: data_0209ee74 "
                     "is %p, Stage+0x50 is %p\n", data_0209ee74, (void *)t);

    _ZN8Particle10SysTracker10InitialiseEv(t);

    /* RESERVE WHAT THE ARCHIVE JUST TOOK. Particle::Texture::AllocTexVram and
       AllocPalVram allocate out of their OWN cursors -- data_0209ee88/84/8c,
       which Initialise seeds by copying the Model allocator's data_020a4bc8/
       cc/d8 (func_02045d10/cf0/ce0) and then advances in place. They never
       write back, so as far as Model::LoadTexAndPal is concerned the space is
       still free and the next model handed a palette is given the particle
       archive's address.

       On the ROM that is harmless because Stage::InitResources runs this LAST,
       after every model the level is going to load; nothing allocates
       afterwards. The port loads actor models lazily when the actors spawn,
       which is after this point, so the second half of that guarantee does not
       hold and the actor palettes land on top of the particle ones. Measured:
       Mario's dust binds palette 0x122a0 and so do two of the level's
       compressed textures, and the puffs come out grass green instead of the
       white-to-grey the archive actually ships.

       Pushing the cursors forward is the smallest fix that restores the ROM's
       invariant rather than reordering the boot. The archive's non-4x4
       textures need nothing: they go through Model::GetVramOffset, which
       already moves the shared texture cursors itself.

       THE 4x4 CURSOR MOVES IN LOCKSTEP WITH ITS INDEX CURSOR. Compressed
       textures are two streams: LoadCompressedTextureToVram writes the blocks
       at data_020a4bc8 and the index words at data_020a4be0, then advances
       them by size and size>>1, which is what keeps the hardware's
       "index address = slot1 + block_offset/2" rule true. Reserving the block
       cursor without pushing the index cursor by half as much breaks that
       pairing for every compressed texture loaded afterwards, and they decode
       against the wrong index words. Caught by Luigi's head coming out as
       speckle while Yoshi and Wario were clean. */
    const unsigned blocks = data_0209ee88 - data_020a4bc8;
    data_020a4bc8 = data_0209ee88;   /* 4x4 texture blocks */
    data_020a4be0 += blocks >> 1;    /* their index words, same rule as above */
    data_020a4bcc = data_0209ee84;   /* small palettes, rising */
    data_020a4bd8 = data_0209ee8c;   /* large palettes, falling */

    char *engine = *(char **)(t + 4);
    if (!engine) {
        std::fprintf(stderr, "  [fx] Initialise left no engine at +4; the "
                     "particle subsystem is down\n");
        return;
    }
    /* func_0204a17c parsed the blob into these: the PEntry array at +0x1c
       (stride 0x20, the stride Manager::AddSystem indexes by) with its count
       at +0x24, and the secondary array at +0x20 counted at +0x26. Every
       effect id the game spawns is an index into the first array, so the
       count is the one number that says whether the parse worked: the
       Player's running/sliding dust is id 0xda, and the ground pound's are
       0xd3..0xd5. */
    const unsigned numA = *(unsigned short *)(engine + 0x24);
    const unsigned numB = *(unsigned short *)(engine + 0x26);
    std::printf("  [fx] particle subsystem up: engine %p, %d systems, %d "
                "particles, defs %p (%u effects, %u secondary)\n",
                (void *)engine, (int)*(short *)(engine + 0x28),
                (int)*(short *)(engine + 0x2a), *(void **)t, numA, numB);
    if (numA <= 0xda)
        std::fprintf(stderr, "  [fx] ONLY %u EFFECTS PARSED: the Player's "
                     "dust is id 0xda, so the blob did not parse\n", numA);

    if (fx_trace()) {
        /* byte 0x30 of a definition record is the emission interval that
           func_0204ae2c copies into a live system's +0x58, and +0x58 is the
           divisor in func_0204a730's `counter % interval` gate. Count how
           many of the ROM's own definitions ship a zero there. */
        char *base = *(char **)(engine + 0x1c);
        unsigned zero = 0, live = 0;
        for (unsigned i = 0; i < numA; ++i) {
            char *rec = *(char **)(base + i * 0x20);
            if (!rec) continue;
            ++live;
            if (*(unsigned char *)(rec + 0x30) == 0) ++zero;
        }
        char *dust = *(char **)(base + 0xda * 0x20);
        std::printf("  [fx] definitions: %u with a record, %u of them with a "
                    "ZERO emission interval; id 0xda (the Player's dust) "
                    "interval=%u\n", live, zero,
                    dust ? (unsigned)*(unsigned char *)(dust + 0x30) : 0u);

        /* THE EMISSION RATE, def+0x04. func_0204c584 emits
           floor((def->rate + sys->3a) / 0x1000) particles a call and nothing
           else feeds that sum, so a definition with rate 0 can never emit --
           the callback only carries the fractional remainder across frames.
           Count them, and dump the dust's record next to one that is known to
           work (id 0x24 emits at 409/4096), so a mis-parsed blob shows up as a
           structural difference rather than a guess. */
        unsigned zrate = 0;
        for (unsigned i = 0; i < numA; ++i) {
            char *rec = *(char **)(base + i * 0x20);
            if (rec && *(int *)(rec + 4) == 0) ++zrate;
        }
        std::printf("  [fx] rates: %u of %u definitions have rate 0\n",
                    zrate, live);
        /* THE TEXTURE TABLE. func_0204a0dc parks the texel VRAM offset at
           entry+4 and func_0204a028 the palette offset at entry+8, which is
           exactly the pair func_0204af3c binds (TEXIMAGE_PARAM then
           TEXPLTT_BASE). Print both against what the archive says the palette
           should hold, so a wrong colour is attributed to the bind, the upload
           or the decode rather than guessed at. */
        if (fx_trace() >= 2) {
            char *list = *(char **)(engine + 0x20);
            const unsigned ntex = *(unsigned short *)(engine + 0x26);
            std::printf("  [fx] %u particle textures\n", ntex);
            for (unsigned i = 0; i < ntex; ++i) {
                char *e = list + i * 0x14;
                const unsigned tex = *(unsigned *)(e + 4);
                const unsigned pal = *(unsigned *)(e + 8);
                const unsigned par = *(unsigned *)(e + 0xc);
                std::printf("  [fx]   tex %u: texAddr=%05x palAddr=%05x fmt=%u "
                            "%ux%u pltt=%04x  pal:", i, tex, pal, par & 0xf,
                            (unsigned)*(unsigned short *)(e + 0x10),
                            (unsigned)*(unsigned short *)(e + 0x12),
                            pal >> 4);
                /* the palette bytes actually sitting in host texture-palette
                   VRAM at the address the bind will point the hardware at */
                const unsigned short *pv =
                    (const unsigned short *)(0x06880000u + pal);
                for (int k = 0; k < 4; ++k)
                    std::printf(" %04X", pv[k]);
                std::printf("\n");
            }
        }
        static const unsigned probe[] = {0xda, 0x24, 0xb3};
        for (unsigned k = 0; k < 3; ++k) {
            char *e = base + probe[k] * 0x20;
            char *r = *(char **)e;
            std::printf("  [fx] def %02X: rec=%p rate=%d interval=%u\n",
                        probe[k], (void *)r, r ? *(int *)(r + 4) : -1,
                        r ? (unsigned)*(unsigned char *)(r + 0x30) : 0u);
            std::printf("  [fx]   entry:");
            for (int b = 0; b < 0x20; ++b)
                std::printf(" %02X", (unsigned char)e[b]);
            std::printf("\n");
            if (!r) continue;
            std::printf("  [fx]   rec  :");
            for (int b = 0; b < 0x38; ++b)
                std::printf(" %02X", (unsigned char)r[b]);
            std::printf("\n");
        }
    }
}

/* Live counts straight out of the engine's own free lists, which is why this
   costs nothing: func_0204a4c8 threads every system node onto the free list
   at +0xc and every particle node onto the one at +0x14, and each list keeps
   its length in the word after the head. Live = capacity - free, and the
   active system list at +4 keeps its own count at +8. */
extern "C" void port_particle_counts(int *systems, int *particles)
{
    char *t = tracker();
    char *engine = t ? *(char **)(t + 4) : 0;
    if (!engine) { if (systems) *systems = 0; if (particles) *particles = 0; return; }
    if (systems) *systems = *(int *)(engine + 8);
    if (particles)
        *particles = (int)*(unsigned short *)(engine + 0x2a) -
                     *(int *)(engine + 0x18);
}

extern "C" void port_particle_frame(void)
{
    char *t = tracker();
    if (!t || !*(char **)(t + 4))
        return;                       /* subsystem down; say nothing per frame */

    if (fx_trace() >= 2) {
        /* the view matrix Particle::RenderAll hands the engine, which every
           particle's world position is multiplied by before the billboard
           matrix is loaded (func_0204be40: MulVec3Mat4x3(&trans, mtx, &trans)) */
        static int said_mtx;
        if (!said_mtx) {
            said_mtx = 1;
            extern int data_0209b3ec[];
            std::printf("[fx] view matrix data_0209b3ec:");
            for (int i = 0; i < 12; ++i)
                std::printf(" %d", data_0209b3ec[i]);
            std::printf("\n");
        }
    }
    if (fx_trace() >= 9) {
        /* superseded by the post-update census below; kept behind a level
           nothing sets so the old walk is still there to fall back on */
        char *engine = *(char **)(t + 4);
        char *node = *(char **)(engine + 4);
        for (int guard = 0; node && guard < 64; ++guard) {
            char *pentry = *(char **)(node + 0x18);
            char *base = *(char **)(engine + 0x1c);
            /* THE UNITS QUESTION, answered by the stored number. func_0204c304
               seeds every new particle with `p->v14 = self->v14`, and
               func_0204be40 renders `p14 + p8`. So the emitter's own v14 is the
               base every particle inherits. Particle::System::New should have
               put the spawn point there already divided by 8. Print it in both
               readings next to Mario's world position and the answer is
               whichever one matches. */
            {
                /* Scan the node for the spawn point instead of trusting an
                   offset. Mario stands at world (-1200, 254, 6800), so his x
                   is -4915200 in fx12 world or -614400 in scene; print any
                   word within 5% of either reading, with its offset. Whichever
                   scale turns up is the scale the engine was handed. */
                /* func_0204ae2c seats a system: position at +0x20, ZERO at
                   +0x14, a pointer at +0x18. The system is therefore fine.
                   func_0204be40 renders each PARTICLE as v14 + v8, so walk the
                   live particle list (func_0204a730 links new particles onto
                   the list at mgr+0x10) and print what a particle actually
                   carries. */
                std::printf("[fx]   sys pos +0x20 %.1f %.1f %.1f (scene)\n",
                            *(int *)(node + 0x20) / 4096.0,
                            *(int *)(node + 0x24) / 4096.0,
                            *(int *)(node + 0x28) / 4096.0);
                {
                    /* func_0204a5c8 is the render walk: ctx = system+0x34,
                       and the particle list it hands the billboard is ctx+0x8. */
                    char *ctx = *(char **)(node + 0x34);
                    char *plist = ctx ? *(char **)(ctx + 0x08) : 0;
                    int shown = 0;
                    for (char *p = plist; p && shown < 2; p = *(char **)p, ++shown)
                        std::printf("[fx]     particle v8 %.1f %.1f %.1f  "
                                    "v14 %.1f %.1f %.1f  sum %.1f %.1f %.1f\n",
                                    *(int *)(p + 0x08) / 4096.0,
                                    *(int *)(p + 0x0c) / 4096.0,
                                    *(int *)(p + 0x10) / 4096.0,
                                    *(int *)(p + 0x14) / 4096.0,
                                    *(int *)(p + 0x18) / 4096.0,
                                    *(int *)(p + 0x1c) / 4096.0,
                                    (*(int *)(p + 0x14) + *(int *)(p + 0x08)) / 4096.0,
                                    (*(int *)(p + 0x18) + *(int *)(p + 0x0c)) / 4096.0,
                                    (*(int *)(p + 0x1c) + *(int *)(p + 0x10)) / 4096.0);
                }
            }
            std::printf("[fx]   system %p interval=%u counter=%u pentry=%p "
                        "(id %d) def=%p\n",
                        (void *)node, (unsigned)*(unsigned char *)(node + 0x58),
                        (unsigned)*(unsigned short *)(node + 0x38),
                        (void *)pentry,
                        pentry ? (int)((pentry - base) / 0x20) : -1,
                        pentry ? *(void **)pentry : 0);
            node = *(char **)node;
        }
    }
    _ZN8Particle10SysTracker6UpdateEv(t);   /* Stage::Render's call */

    /* THE CENSUS. One line per live system, one per live particle, taken after
       the update so freshly emitted particles are visible and before RenderAll
       so nothing has been consumed. Everything the placement question needs is
       on these lines: where the system thinks it is, what its particles
       inherited from it (func_0204c584 copies system+0x20 into particle+0x08),
       and what the view matrix turns the sum into -- the same arithmetic
       func_0204be40 does. So if the view column is sane and the billboard
       still lands elsewhere the fault is in the render; if the view column is
       already wrong the position was wrong before the render ever saw it. */
    if (fx_trace() >= 2) {
        static int fx_frame;
        char *engine = *(char **)(t + 4);
        char *base = *(char **)(engine + 0x1c);
        char *node = *(char **)(engine + 4);
        const int *m = data_0209b3ec;
        std::printf("[fx] census f%d: %d systems\n", fx_frame++,
                    *(int *)(engine + 8));
        for (int guard = 0; node && guard < 64; ++guard) {
            char *pentry = *(char **)(node + 0x18);
            /* the four gates func_0204a730 puts in front of the emit, in the
               order it tests them, so a system that never emits names its own
               reason instead of being guessed at:
                 thresh (def+0x28): 0, or counter must be under it
                 counter % interval (self+0x38 % self+0x58): must be 0
                 self+0x1c bits 0 and 1: both must be clear
               and then func_0204c584's own rate accumulator, which emits
               floor((def->rate + self->3a) / 0x1000) particles. */
            char *sdef = pentry ? *(char **)pentry : 0;
            std::printf("[fx]   sys %p id=%d pos=(%.1f,%.1f,%.1f) prim=%d sec=%d"
                        " | ctr=%u ival=%u thresh=%u f1c=%08X rate=%d acc=%d\n",
                        (void *)node,
                        pentry ? (int)((pentry - base) / 0x20) : -1,
                        *(int *)(node + 0x20) / 4096.0,
                        *(int *)(node + 0x24) / 4096.0,
                        *(int *)(node + 0x28) / 4096.0,
                        *(int *)(node + 0x0c), *(int *)(node + 0x14),
                        (unsigned)*(unsigned short *)(node + 0x38),
                        (unsigned)*(unsigned char *)(node + 0x58),
                        sdef ? (unsigned)*(unsigned short *)(sdef + 0x28) : 0u,
                        (unsigned)*(int *)(node + 0x1c),
                        sdef ? *(int *)(sdef + 4) : -1,
                        (int)*(short *)(node + 0x3a));
            int shown = 0;
            for (char *p = *(char **)(node + 0x08); p && shown < 3;
                 p = *(char **)p, ++shown) {
                const double sx = (*(int *)(p + 0x08) + *(int *)(p + 0x14)) / 4096.0;
                const double sy = (*(int *)(p + 0x0c) + *(int *)(p + 0x18)) / 4096.0;
                const double sz = (*(int *)(p + 0x10) + *(int *)(p + 0x1c)) / 4096.0;
                const double vx = (sx * m[0] + sy * m[3] + sz * m[6]) / 4096.0 + m[9] / 4096.0;
                const double vy = (sx * m[1] + sy * m[4] + sz * m[7]) / 4096.0 + m[10] / 4096.0;
                const double vz = (sx * m[2] + sy * m[5] + sz * m[8]) / 4096.0 + m[11] / 4096.0;
                std::printf("[fx]     p base=(%.1f,%.1f,%.1f) off=(%.1f,%.1f,%.1f)"
                            " sum=(%.1f,%.1f,%.1f) view=(%.1f,%.1f,%.1f)\n",
                            *(int *)(p + 0x08) / 4096.0, *(int *)(p + 0x0c) / 4096.0,
                            *(int *)(p + 0x10) / 4096.0,
                            *(int *)(p + 0x14) / 4096.0, *(int *)(p + 0x18) / 4096.0,
                            *(int *)(p + 0x1c) / 4096.0,
                            sx, sy, sz, vx, vy, vz);
            }
            node = *(char **)node;
        }
    }
    if (fx_trace()) {
        /* one line a second at 30fps, plus every frame the count changes, so
           a dust puff that lives twelve frames still shows up */
        static int n, last = -1, peak;
        int sys = 0, par = 0;
        port_particle_counts(&sys, &par);
        if (par > peak) peak = par;
        if (par != last || (n % 30) == 0) {
            std::printf("[fx] frame %d: %d systems live, %d particles live "
                        "(peak %d)\n", n, sys, par, peak);
            last = par;
        }
        ++n;
    }
}

/* Stage::GraphCallback1's call, and it has to run WHERE GraphCallback1 runs:
   after Stage::Render has drawn the level. Particles are translucent and the
   dust sits on the ground at Mario's feet, so submitting them ahead of the
   opaque level pass loses every pixel to the ground drawn over them. Measured
   both ways on the same frame: with the level suppressed the pass paints 5006
   pixels, with it drawn afterwards it paints none.

   Nothing in here depends on the GX position matrix, which is why it is free to
   move: func_0204be40 puts the particle through data_0209b3ec itself and then
   loads the billboard absolutely (MTX_IDENTITY, then MTX_MULT_4x3), so the only
   state it reads is the scene-unit view matrix the whole frame already renders
   in. */
extern "C" void port_particle_render(void)
{
    char *t = tracker();
    if (!t || !*(char **)(t + 4))
        return;                       /* subsystem down; say nothing per frame */

    /* SM64DS_NO_FX_RENDER=1 keeps the simulation and drops the submission,
       for A/B-ing what the particles actually put on the screen */
    static int no_render = -1;
    if (no_render < 0)
        no_render = std::getenv("SM64DS_NO_FX_RENDER") ? 1 : 0;
    /* SM64DS_FX_TRACE=3: what RenderAll actually put in the triangle buffer.
       The subsystem simulates correctly and submits geometry, so anything
       invisible is being lost between submission and the raster: a degenerate
       billboard, an off-screen transform, a zero alpha, or a texture that
       decoded to nothing. Print the triangles it added so the answer is read
       off the numbers instead of guessed. */
    size_t tris_before = 0;
    if (fx_trace() >= 3) {
        ntr::gx_polygons(tris_before);
        /* The particle billboard is already in VIEW space: func_0204be40 sends
           MTX_IDENTITY then MTX_MULT_4x3, which only lands the billboard alone
           in the position matrix if the mode is a position mode. If the mode
           is projection here, identity wipes the perspective and the view
           stays in position, so every particle gets the view applied twice.
           Print the state rather than assume it. */
        static int said;
        if (!said) {
            said = 1;
            int mode = -1;
            float pos[16], proj[16];
            ntr::gx_debug_matrices(&mode, pos, proj);
            std::printf("[fx] at particle time MTX_MODE = %d (0 proj, 1 pos, "
                        "2 pos+vec, 3 tex)\n", mode);
            std::printf("[fx]   pos  row3 %.1f %.1f %.1f\n", pos[12], pos[13], pos[14]);
            std::printf("[fx]   proj row0 %.3f %.3f  row3 %.1f %.1f\n",
                        proj[0], proj[1], proj[12], proj[13]);
        }
    }
    /* SM64DS_FX_SCALE, the "the matrix is in the wrong units" experiment, is
       GONE: the units were never wrong. The billboard translation this pass
       produces matches the view matrix applied to the particle's own position
       to the decimal, and the reason it used to come out at (-577, 31, -992)
       was that the only systems with anything to emit were distant ones -- the
       particle definition blob was truncated, so Mario's dust carried a record
       of zeros and never spawned. See port/tools/romdata.py. */
    if (!no_render)
        _ZN8Particle9RenderAllEv();
    if (fx_trace() >= 3) {
        /* AFTER the draw: if the identity-then-multiply worked, the position
           matrix now holds the billboard alone, whose translation is the
           particle in VIEW space. For the dust at scene (-150, 34.9, 837.8)
           through this view matrix that is about (0, -15, -108). Anything
           else means the billboard did not land where the ROM intends. */
        size_t tn = 0;
        const ntr::GxTriangle *ta = ntr::gx_polygons(tn);
        static int said_after;
        if (!said_after && tn > tris_before) {   /* only on a frame that DREW */
            said_after = 1;
            int mode = -1;
            float pos[16], proj[16];
            ntr::gx_debug_matrices(&mode, pos, proj);
            std::printf("[fx] AFTER a real particle draw: mode %d, pos row3 "
                        "%.1f %.1f %.1f  (billboard should be about 0 -15 -108)\n",
                        mode, pos[12], pos[13], pos[14]);
        }
        static int shown;
        if (tn > tris_before && shown < 6) {
            ++shown;
            std::printf("[fx] RenderAll added %u triangles:\n",
                        (unsigned)(tn - tris_before));
            for (size_t i = tris_before; i < tn && i < tris_before + 3; ++i) {
                const ntr::GxTriangle &t = ta[i];
                std::printf("[fx]   tri a(%.1f,%.1f,%.3f w%.2f) b(%.1f,%.1f,%.3f) "
                            "c(%.1f,%.1f,%.3f) col %08X alpha %u cull %u tex %s %dx%d\n",
                            t.v[0].x, t.v[0].y, t.v[0].z, t.v[0].w,
                            t.v[1].x, t.v[1].y, t.v[1].z,
                            t.v[2].x, t.v[2].y, t.v[2].z,
                            t.v[0].color, (unsigned)t.alpha, (unsigned)t.cull,
                            t.tex ? "yes" : "NULL", t.tw, t.th);
                const float area =
                    (t.v[1].x - t.v[0].x) * (t.v[2].y - t.v[0].y) -
                    (t.v[2].x - t.v[0].x) * (t.v[1].y - t.v[0].y);
                std::printf("[fx]     signed area %.3f%s\n", area,
                            area == 0.0f ? "  <-- DEGENERATE" : "");
                /* The vertex colour is white, so whatever tint the puff ends
                   up with came out of the bound palette. The archive's own dust
                   palettes are white-to-grey (texture 2) and white-to-tan
                   (texture 3) and it contains no green at all, so printing the
                   DECODED texels says directly whether the right palette was
                   bound or a neighbour's was. */
                if (t.tex) {
                    std::printf("[fx]     texels(%08X):", t.dbg_tex);
                    int shownpx = 0;
                    for (int q = 0; q < t.tw * t.th && shownpx < 6; ++q)
                        if (t.tex[q] >> 24) {
                            std::printf(" %08X", t.tex[q]);
                            ++shownpx;
                        }
                    std::printf("%s\n", shownpx ? "" : " (all transparent)");
                }
            }
        }
    }
}

