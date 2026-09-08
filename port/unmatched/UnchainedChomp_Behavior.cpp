/* HOST COPIES of src/_ZN14UnchainedChomp8BehaviorEv.cpp (the per-frame body)
 * and src/func_ov100_02143b18.cpp (the InitResources-time installer both share
 * the +0x668 Holder with), plus the seat for the two {function, delta} statics
 * they dispatch. The gate-16 pointer-to-member case again -- the simplest table
 * of the four ov100 classes carry: a single Holder* at +0x668 rather than an
 * N-state array or a callback-node list.
 *
 * ---- what the ROM actually does ------------------------------------------
 *
 * The chomp keeps a Holder* at +0x668. func_ov100_02143b18 seats it at the end
 * of InitResources (`c->pp = &data_ov100_021486f4`) and immediately dispatches
 * the pair's word-0 pointer-to-member; Behavior dispatches word-1 (the `hi`
 * pair, Holder+8) every frame in one of its two branches. data_ov100_021486f4
 * is a {lo, hi} P4 that __sinit_ov100_021475a4 fills from the two source
 * statics:
 *
 *     data_ov100_021486f4.lo = data_ov100_02148000   (fn 0x02143ae0, delta 0)
 *     data_ov100_021486f4.hi = data_ov100_02147ff8   (fn 0x02143aa4, delta 0)
 *
 * -- ov100's own DS CODE ADDRESSES (port/ov100_syms.txt line 34; both sit
 * immediately before UnchainedChomp_SpawnInfo). The fn/delta words were read
 * straight off overlay_0100.bin: both deltas are 0, so the virtual-bit branch
 * of the pair encoding is dead on this level, but it is spelled below anyway
 * (the Door reading: a dead line that is right beats a live assumption).
 *
 * The two host bodies are ordinary matched src (func_ov100_02143aa4 / _02143ae0,
 * added to slice_gate21.txt), the Fish precedent: real names in the seat table.
 * 02143aa4 (`hi`) is the one Behavior actually dispatches; 02143ae0 (`lo`) is
 * the one func_ov100_02143b18 dispatches at InitResources time.
 *
 * ---- why these are host copies --------------------------------------------
 *
 * BOTH source TUs form the pointer-to-member while its class is INCOMPLETE:
 *   func_ov100_02143b18.cpp: `struct C; typedef int (C::*PMF)(); struct C {...}`
 *   _ZN14UnchainedChomp8BehaviorEv.cpp: `struct Actor;` stand-in at the typedef
 * -- the case MSVC answers with the four-word general representation, which
 * quadruples the pair stride and dispatches a neighbour's body. (The prep
 * artifact called func_ov100_02143b18 safe matched src; it is not -- C is
 * incomplete at the typedef, the exact Door func_ov100_021453d8 shape. Hosted.)
 *
 * ---- and why the statics are seated ---------------------------------------
 *
 * The seat rewrites the two SOURCE statics before the sinit runs (the LakituBro
 * reading: one less mapping to get wrong), each checked against the ROM address
 * its host body was compiled from. Both are matched src, so neither is trapped.
 *
 * The bodies below are the matched sources' control flow line for line; only the
 * pointer-to-member dispatches are spelled as plain calls through the pair's
 * resolved word (r0 = this, r1 = the argument, the ARM ride-through under
 * __cdecl the caller cleans -- the same one Door met all through this gate).
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

/* the two Behavior helpers, plain matched src (in slice_gate21.txt) */
int func_ov100_02143370(char *c);
void func_ov100_02143b68(char *c);

/* the two PMF targets the seat wires as host bodies (matched src). Declared as
   the (this, arg) shape the ROM dispatch hands them; both read only `this`. */
int func_ov100_02143aa4(void *self, void *arg);   /* hi 0x02143aa4, Behavior's */
int func_ov100_02143ae0(void *self, void *arg);   /* lo 0x02143ae0, installer's */

/* the rest of Behavior's engine calls */
unsigned short DecIfAbove0_Short(unsigned short *p);
void _Z14ApproachLinearRiii(int &x, int target, int step);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
void _ZN12CylinderClsn5ClearEv(void *thiz);
void _ZN12CylinderClsn6UpdateEv(void *thiz);
void func_02012694(int, void *);
void _ZN5Actor15HugeLandingDustEb(void *thiz, bool b);
void *_ZN5Actor13ClosestPlayerEv(void *thiz);
void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned a, unsigned b,
    const void *pos, const void *rot, int e, int f);
void _ZN7PathPtrC1Ev(void *thiz);
void _ZN7PathPtr6FromIDEj(void *thiz, unsigned id);
void _ZNK7PathPtr7GetNodeER7Vector3j(void *thiz, void *out, unsigned idx);
void Vec3_Sub(void *out, void *a, void *b);
int LenVec3(void *v);
short Vec3_HorzAngle(const void *a, const void *b);
void ApproachAngle(short *cur, short target, int step, int a, int b);
int __aeabi_idiv(int, int);

extern unsigned char data_0209f2d8[];

struct PortPmf { unsigned fn; int delta; };
/* the P4{lo,hi} the sinit fills; the two SOURCE statics it copies from */
extern PortPmf data_ov100_021486f4[2];   /* [0]=lo [1]=hi (Holder+8 is hi) */
extern PortPmf data_ov100_02148000[];    /* -> lo */
extern PortPmf data_ov100_02147ff8[];    /* -> hi, the one Behavior reads */

}  /* extern "C" */

/* ---- Spawn host copy ------------------------------------------------------
 * src/UnchainedChomp_Spawn.c calls Enemy's constructor as `func_020aed98()`
 * with no argument -- correct ARM (operator new returns `this` in r0 and the
 * ctor wants it in r0, so mwcc emits the branch with nothing between), but under
 * MSVC cdecl a no-arg call passes no argument and the ctor reads `this` off the
 * caller's frame -> a c0000005 in ActorBase's ctor. The ChainChomp_Spawn case
 * exactly. func_020aed98 is ov002 0x020aed98 = _ZN5EnemyC2Ev. Everything else is
 * the matched source line for line. Also carries the PathPtr method aliases
 * InitResources needs (it declares PathPtr as a real class, whose MSVC-mangled
 * method names nothing defines; the C-linkage _ZN7PathPtr* bodies are in
 * slice_gate10). */
extern "C" {
void *_ZN9ActorBasenwEj(unsigned size);
int _ZN5EnemyC2Ev(void *self);
int _ZN25MovingCylinderClsnWithPosC1Ev(void *self);
int _ZN12WithMeshClsnC1Ev(void *self);
int _ZN9ModelAnimC1Ev(void *self);
int _ZN11ShadowModelC1Ev(void *self);
int func_020733a8(void *arr, int n, int stride, void *ctor, void *dtor);
void _ZN5ModelC1Ev(void);
void _ZN5ModelD1Ev(void);
void _ZN11ShadowModelD1Ev(void);
void func_020072c0(void);
void func_0203d384(void);
void func_02011508(void);
void func_0203d73c(void);
extern int _ZTV14UnchainedChomp;

/* PORT_HOST_ABI: ARM no-arg ctor call ride-through (operator new's `this`
   already sits in r0); cdecl needs it passed explicitly. ChainChomp_Spawn
   shape. */
void *UnchainedChomp_Spawn(void)
{
    char *p = (char *)_ZN9ActorBasenwEj(0x7a4);
    if (p) {
        _ZN5EnemyC2Ev(p);
        *(int *)p = (int)&_ZTV14UnchainedChomp;
        _ZN25MovingCylinderClsnWithPosC1Ev(p + 0x110);
        _ZN12WithMeshClsnC1Ev(p + 0x150);
        _ZN9ModelAnimC1Ev(p + 0x30c);
        func_020733a8(p + 0x370, 6, 0x50, (void *)_ZN5ModelC1Ev, (void *)_ZN5ModelD1Ev);
        func_020733a8(p + 0x550, 6, 0x28, (void *)_ZN11ShadowModelC1Ev, (void *)_ZN11ShadowModelD1Ev);
        _ZN11ShadowModelC1Ev(p + 0x640);
        func_020733a8(p + 0x6d8, 6, 0xc, (void *)func_0203d384, (void *)func_020072c0);
        func_020733a8(p + 0x720, 6, 0xc, (void *)func_0203d384, (void *)func_020072c0);
        func_020733a8(p + 0x768, 6, 6, (void *)func_0203d73c, (void *)func_02011508);
    }
    return p;
}
/* ---- InitResources host copy ---------------------------------------------
 * src/_ZN14UnchainedChomp13InitResourcesEv.cpp declares PathPtr as a real C++
 * class and calls pp.FromID/NumNodes/GetNode as THISCALL members -- MSVC-mangled
 * names nothing defines. The C-linkage _ZN7PathPtr* bodies (slice_gate10) are
 * cdecl, so aliasing the thiscall names onto them mismatches the calling
 * convention (measured: a c0000005 in FromID). Host copy spelling the PathPtr
 * calls the C-linkage way level_boot.cpp does (int path[2]={0,0}; FromID(path,
 * id); ...). Everything else is the matched source line for line. */
void* _ZN5Model8LoadFileER13SharedFilePtr(void *p);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
void _ZN9Animation8LoadFileER13SharedFilePtr(void *p);
void LoadSilverStarAndNumber(void);
void _ZN11ShadowModel12InitCylinderEv(void *self);
void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
    void *self, void *a, const void *v, int fix12, int c, unsigned d, unsigned e);
int func_ov100_02143b18(void *c, void *p);   /* defined below (the installer) */

void _ZN7PathPtr6FromIDEj(void *self, unsigned id);
unsigned _ZNK7PathPtr8NumNodesEv(const void *self);
/* _ZNK7PathPtr7GetNodeER7Vector3j and data_ov100_021486f4 are declared in the
   Behavior extern block above; reused here. */

extern void *data_ov002_0211092c;
extern void *data_ov100_021486bc;
extern void *data_ov100_021486a4;
extern void *data_ov100_021486ac;
extern void *data_ov100_021486b4;
extern int data_ov100_02148008[3];

/* PORT_HOST_ABI: the matched TU calls PathPtr members as thiscall while
 * the C-linkage ROM bodies are cdecl (measured c0000005). See the note
 * above. */
int _ZN14UnchainedChomp13InitResourcesEv(unsigned char *thiz)
{
    char *c = (char *)thiz;
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0211092c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x30c,
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov100_021486bc), 1, -1);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov100_021486a4);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov100_021486ac);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov100_021486b4);
    LoadSilverStarAndNumber();

    {
        int i = 0;
        char *p = c + 0x370;
        do {
            _ZN9ModelBase7SetFileEP8BMD_Fileii(p,
                *(void **)((char *)&data_ov100_021486a4 + 4), 1, -1);
            i++;
            p += 0x50;
        } while (i < 6);
    }

    _ZN11ShadowModel12InitCylinderEv(c + 0x640);
    {
        int i = 0;
        char *p = c + 0x550;
        do {
            _ZN11ShadowModel12InitCylinderEv(p);
            i++;
            p += 0x28;
        } while (i < 6);
    }

    *(int *)(c + 0x6ac) = *(int *)(c + 8) & 0xff;
    *(int *)(c + 0x6b8) = (*(unsigned int *)(c + 8) >> 8) & 0xf;
    if (*(int *)(c + 0x6ac) == 0xff) *(int *)(c + 0x6ac) = 0;

    {
        int path[2] = {0, 0};
        _ZN7PathPtr6FromIDEj(path, *(unsigned int *)(c + 0x6ac));
        *(int *)(c + 0x6b0) = (int)_ZNK7PathPtr8NumNodesEv(path);
    }

    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa0) = -0x3c000;

    {
        struct { int x, y, z; } v;
        v.x = data_ov100_02148008[0];
        v.y = data_ov100_02148008[1];
        v.z = data_ov100_02148008[2];
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
            c + 0x110, thiz, &v, 0xaa000, 0x140000, 0x200004, 0x6010);
    }

    *(unsigned char *)(c + 0x6c9) = 0x1f;
    *(int *)(c + 0x6cc) = 3;

    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(short *)(c + 0x600 + 0xa4) = *(short *)(c + 0x8e);

    *(int *)(c + 0x6d0) = 0;
    *(int *)(c + 0x6d4) = 0;

    {
        int i = 0;
        char *dst = c;
        do {
            *(int *)(dst + 0x6d8) = *(int *)(c + 0x5c);
            i++;
            *(int *)(dst + 0x6dc) = *(int *)(c + 0x60);
            *(int *)(dst + 0x6e0) = *(int *)(c + 0x64);
            dst += 0xc;
        } while (i < 6);
    }

    *(short *)(c + 0x600 + 0xca) = 0xc8;

    {
        int path[2] = {0, 0};
        _ZN7PathPtr6FromIDEj(path, *(unsigned int *)(c + 0x6ac));
        *(int *)(c + 0x6b4) = 1;
        _ZNK7PathPtr7GetNodeER7Vector3j(path, (int *)(c + 0x5c),
            *(unsigned int *)(c + 0x6b4));
    }

    *(int *)(c + 0x60) += 0x64000;
    *(int *)(c + 0x80) = 0x1000;
    *(int *)(c + 0x84) = 0x1000;
    *(int *)(c + 0x88) = 0x1000;

    func_ov100_02143b18(thiz, &data_ov100_021486f4);
    return 1;
}
}  /* extern "C" */

typedef int (*PortChompFn)(void *, void *);

/* ---- RUN link100 LANE PMFB7 GATE 2: THE PER-FRAME RECORDS ARE FACES -------
 * UnchainedChomp's matched TU dispatches its state cell's +8 half as a real
 * pointer to member -- mov eax,[cell+8] / test eax,eax / je /
 * mov ecx,[cell+12] / add ecx,this / call eax, the ROM's own offsets, receiver
 * in ecx, NOTHING pushed, ARITY ZERO, /Zp4 diff 0 lines
 * (runs/link100/out/PMFB7/emit_all21_out.txt). The seat used to install plain
 * cdecl bodies that take their self off the stack, so each PER-FRAME record now
 * holds a zero-argument __fastcall face that forwards the receiver as the one
 * cdecl argument the ROM's own state body takes -- the same call the cell held
 * before, made through ecx instead of the stack.
 *
 * THE ENTER RECORDS DO NOT CHANGE: they are reached by the class's state-change
 * helper, which MSVC compiles as a one-call forwarder ending in `jmp`, so the
 * caller's own frame is reused and a plain cdecl body is right there.
 *
 * WHICH RECORD IS WHICH is read out of the class's own __sinit
 * (runs/link100/out/PMFB7/slots_gate2.txt), never assumed.
 */
static void __fastcall pmfb7_ov100_02143aa4(void *self, void *dead_edx)
{
    (void)dead_edx;   /* record 02147ff8, the per-frame half */
    ((void (*)(void *))(void *)func_ov100_02143aa4)(self);
}

static const struct { PortPmf *slot; unsigned rom; PortChompFn host; }
g_unchained_chomp_states[] = {
    {data_ov100_02148000, 0x02143ae0, func_ov100_02143ae0},   /* lo */
    {data_ov100_02147ff8, 0x02143aa4, (PortChompFn)(void *)pmfb7_ov100_02143aa4},   /* hi */
};

extern "C" void port_unchained_chomp_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0;
         i < sizeof g_unchained_chomp_states / sizeof g_unchained_chomp_states[0];
         ++i) {
        PortPmf *p = g_unchained_chomp_states[i].slot;
        if (p->fn != g_unchained_chomp_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: UnchainedChomp state %u: the mount "
                         "holds %08x/%d, the ROM's own table says %08x/0 -- "
                         "WRONG BYTES\n", i, p->fn, p->delta,
                         g_unchained_chomp_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_unchained_chomp_states[i].host;
    }
}

/* the pair encoding, shared by the installer and Behavior. delta 0 on this
   level, but the virtual half is spelled anyway (the Door reading). */
static int port_chomp_call(const PortPmf *p, void *self)
{
    char *adj = (char *)self + (p->delta >> 1);
    PortChompFn f;
    if (p->delta & 1)
        f = *(PortChompFn *)(*(char **)adj + p->fn);   /* virtual: fn is offset */
    else
        f = (PortChompFn)(size_t)p->fn;
    return f(adj, 0);
}

/* HOST COPY of src/func_ov100_02143b18.cpp: seat the Holder at +0x668 and run
   its word-0 (lo) pointer-to-member immediately, the InitResources-time call. */

/* func_ov100_02143b18 IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov100_02143b18.cpp back on port/slice_pmf2.txt: with /vmg /vmm global (the
   R8 block in port/CMakeLists.txt) MSVC's pointer-to-member IS the ROM's
   8-byte {function, delta} pair, and the matched TU compiles to the same
   tail jump this body was -- measured, listing in that slice's header.
   The reading above is kept because it is the derivation. */
/* HOST COPY RETIRED, run link100 lane PMFB7 gate 2. src/_ZN14UnchainedChomp8BehaviorEv.cpp
   dispatches its own field now: with /vmg /vmm (block R8) MSVC's pointer to
   member IS the ROM's eight-byte {code, adjust} pair, so the widening this
   banner was written for does not happen. The per-frame half of every state
   cell holds a zero-argument __fastcall face; the enter half does not change,
   because the helper that dispatches it tail-jumps. Measurements in
   port/slice_pmfb7.txt and runs/link100/out/PMFB7/. */
