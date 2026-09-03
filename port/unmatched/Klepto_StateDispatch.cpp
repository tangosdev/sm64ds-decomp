/* HOST COPIES of KLEPTO's two pointer-to-member dispatchers (actor 239,
 * ov062 _ZTV6Klepto / _ZTV9daJango_c 0x0211dd5c) and the seat of its ten
 * state-handler template records.
 *
 * KLEPTO drives a five-state machine through five 16-byte descriptors in ov062
 * bss (data_ov062_0211e14c/15c/16c/17c/18c), each two mwcc {function, delta}
 * records: the ENTER half at +0 and the MAIN half at +8. func_ov062_0211c658
 * stores a descriptor pointer at this+0x42c and dispatches the ENTER half once;
 * Klepto::Behavior dispatches the MAIN half every frame. Both matched TUs form
 * the pointer-to-member over a FORWARD-DECLARED struct (`struct Klass;` /
 * `struct C;`), so MSVC hands them the general (worst-case-inheritance) PMF
 * representation, a multi-word value plus a this-adjust thunk, not the single
 * code pointer a complete single-inheritance class gets -- the Chuckya /
 * KnockDownPlank / Scuttlebug case exactly. Each body below is the matched
 * source's control flow line for line; only the dispatch is read as a plain
 * { fn, delta } and the fn called with `this`.
 *
 * THE SEAT (the Chuckya "seat the source before the copy" order)
 * --------------------------------------------------------------
 * The ten SOURCE records live at ov062 0x0211dcd0..0x0211dd18 (8 bytes each);
 * __sinit_ov062_0211d6fc -- which gate 32 already runs -- copies them into the
 * five bss descriptors. The ovdata mount lays the source blocks down with their
 * fn words as DS CODE ADDRESSES (the mount rebases only pointers into other
 * mounted DATA). port_klepto_states_seat() rewrites each source fn word with its
 * host body BEFORE that sinit runs, validating each mounted word against the ROM
 * address first (WRONG-BYTES abort). All ten .delta halves are 0. All ten
 * handlers are matched src (slice_klepto.txt). Called from
 * hal/actor_overlays.cpp between port_chuckya_states_seat() and
 * __sinit_ov062_0211d6fc().
 *
 * func_ov062_0211bd10 (0x508), the e17c MAIN handler (the per-frame cap-steal
 * logic: fly the path, watch the closest player, dive when he is in range with
 * a cap on), was the last of the ten to be decompiled and used to be seated as
 * an inert-return stub here. It is matched src now (2004/b56, strict relocs)
 * and seats like the other nine.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortPmf { unsigned fn; int delta; };

/* ---- Behavior's C-linkage helpers (matched src or hosted) ---------------- */
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self, void *cc);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wm,
                                              unsigned int j);
void func_ov062_0211c6a8(void *self);
void *_ZN5Actor10FindWithIDEj(unsigned int id);
void func_02012790(int a, int b);
int func_ov062_0211c658(void *c, void *rec);      /* our own host copy below */
void _ZN9ActorBase18MarkForDestructionEv(void *self);
void _ZN14BlendModelAnim7AdvanceEv(void *self);
void func_ov062_0211b51c(void *self);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN12CylinderClsn6UpdateEv(void *self);
extern unsigned char data_ov062_0211e14c[];
extern unsigned char data_ov062_0211e17c[];

/* the ten state-handler template records (mounted, {fn, delta}) */
extern PortPmf data_ov062_0211dcd0, data_ov062_0211dcd8, data_ov062_0211dce0,
    data_ov062_0211dce8, data_ov062_0211dcf0, data_ov062_0211dcf8,
    data_ov062_0211dd00, data_ov062_0211dd08, data_ov062_0211dd10,
    data_ov062_0211dd18;

/* the ten matched handlers (called by host pointer with `this` as arg 0) */
int func_ov062_0211ba84(void *c); int func_ov062_0211c594(void *c);
int func_ov062_0211bd10(void *c); int func_ov062_0211b880(void *c);
int func_ov062_0211c2f4(void *c); int func_ov062_0211bc54(void *c);
int func_ov062_0211b8d8(void *c); int func_ov062_0211b800(void *c);
int func_ov062_0211c218(void *c); int func_ov062_0211b930(void *c);

}  /* extern "C" */

/* PORT_HOST_ABI: mwcc pointer-to-member on the forward-declared struct C. The
   matched func_ov062_0211c658 stores the descriptor pointer at this+0x42c, reads
   the ENTER record (descriptor+0) and, if non-null, calls it with `this`. Read
   as a plain { fn, 0 }. */
extern "C" int func_ov062_0211c658(void *cv, void *rec)
{
    char *c = (char *)cv;
    *(void **)(c + 0x42c) = rec;                /* c->pp = rec */
    {
        PortPmf *q = *(PortPmf **)(c + 0x42c);  /* q = c->pp */
        if (q[0].fn == 0)
            return 1;
        return ((int (*)(void *))(size_t)q[0].fn)(c);
    }
}

/* PORT_HOST_ABI: mwcc pointer-to-member on the forward-declared struct Klass
   (the MAIN half, descriptor+8). The matched Behavior's control flow line for
   line. */
extern "C" int _ZN6Klepto8BehaviorEv(void *self)
{
    char *c = (char *)self;

    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    DecIfAbove0_Short((unsigned short *)(c + 0x444));

    {
        /* (((Klass*)c)->*(m->pmf))() -- m = descriptor at +0x42c, pmf = the
           record at m+8; read as a plain { fn, 0 } and fn called with `this`. */
        PortPmf *m = *(PortPmf **)(c + 0x42c);
        if (m[1].fn != 0)
            ((void (*)(void *))(size_t)m[1].fn)(c);
    }

    {
        int accum = *(int *)(c + 0xa8);
        int a0    = *(int *)(c + 0x9c);
        int lim   = *(int *)(c + 0xa0);
        int sum   = accum + a0;
        if (sum >= lim)
            lim = sum;
        int t = *(int *)(c + 0xac);
        *(int *)(c + 0xa8) = lim;
        *(int *)(c + 0xac) = t;
    }

    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(c, c + 0x110);
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x178, 0);

    *(short *)(c + 0x8c) = *(short *)(c + 0x92);
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(short *)(c + 0x90) = *(short *)(c + 0x96);
    func_ov062_0211c6a8(c);

    {
        unsigned int actorId = *(unsigned int *)(c + 0x44c);
        if (actorId != 0) {
            char *p = (char *)_ZN5Actor10FindWithIDEj(actorId);
            if (p != 0) {
                if (*(int *)(c + 0x468) == 1) {
                    *(int *)(p + 0x5c) = *(int *)(c + 0x450);
                    *(int *)(p + 0x60) = *(int *)(c + 0x454);
                    *(int *)(p + 0x64) = *(int *)(c + 0x458);
                    goto skip_destroy;
                } else if (*(unsigned char *)(p + 0x403) == 0) {
                    *(int *)(p + 0x5c) = *(int *)(c + 0x450);
                    *(int *)(p + 0x60) = *(int *)(c + 0x454);
                    *(int *)(p + 0x64) = *(int *)(c + 0x458);
                    goto skip_destroy;
                } else {
                    *(int *)(c + 0x44c) = 0;
                    func_02012790(0xa, 0);
                    *(unsigned short *)(c + 0x444) = 0x1e;
                    func_ov062_0211c658(c, data_ov062_0211e17c);
                    goto skip_destroy;
                }
            } else {
                *(int *)(c + 0x44c) = 0;
                func_02012790(0xa, 0);
                *(unsigned short *)(c + 0x444) = 0x1e;
                func_ov062_0211c658(c, data_ov062_0211e17c);
                goto skip_destroy;
            }
        }
    }

    if (*(int *)(c + 0x468) == 1 && *(unsigned char *)(c + 0x448) != 2) {
        int b = (*(int *)(c + 0xb0) & 8) != 0;
        if (b != 0)
            _ZN9ActorBase18MarkForDestructionEv(c);
    }
skip_destroy:
    _ZN14BlendModelAnim7AdvanceEv(c + 0x334);
    if (*(void **)(c + 0x42c) != (void *)data_ov062_0211e14c)
        func_ov062_0211b51c(c);

    _ZN12CylinderClsn5ClearEv(c + 0x110);
    _ZN12CylinderClsn6UpdateEv(c + 0x110);
    _ZN12CylinderClsn5ClearEv(c + 0x144);
    _ZN12CylinderClsn6UpdateEv(c + 0x144);

    return 1;
}

/* ---- THE SEAT -------------------------------------------------------------
   The ten source fn words (0x0211dcd0..0x0211dd18), each {slot, rom, host}
   matched against its own reloc destination in
   config/arm9/overlays/ov062/relocs.txt. Rewritten BEFORE __sinit_ov062_0211d6fc
   copies them into the five bss descriptors. */
static const struct { PortPmf *slot; unsigned rom; void *host; }
g_klepto_states[] = {
    {&data_ov062_0211dcd0, 0x0211ba84, (void *)func_ov062_0211ba84},
    {&data_ov062_0211dcd8, 0x0211c594, (void *)func_ov062_0211c594},
    {&data_ov062_0211dce0, 0x0211bd10, (void *)func_ov062_0211bd10},
    {&data_ov062_0211dce8, 0x0211b880, (void *)func_ov062_0211b880},
    {&data_ov062_0211dcf0, 0x0211c2f4, (void *)func_ov062_0211c2f4},
    {&data_ov062_0211dcf8, 0x0211bc54, (void *)func_ov062_0211bc54},
    {&data_ov062_0211dd00, 0x0211b8d8, (void *)func_ov062_0211b8d8},
    {&data_ov062_0211dd08, 0x0211b800, (void *)func_ov062_0211b800},
    {&data_ov062_0211dd10, 0x0211c218, (void *)func_ov062_0211c218},
    {&data_ov062_0211dd18, 0x0211b930, (void *)func_ov062_0211b930},
};

extern "C" void port_klepto_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_klepto_states /
                             sizeof g_klepto_states[0]; ++i) {
        PortPmf *p = g_klepto_states[i].slot;
        if (p->fn != g_klepto_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: KLEPTO state template %u: the mount "
                         "holds %08x/%d, the ROM's own table says %08x/0 -- "
                         "WRONG BYTES\n", i, p->fn, p->delta,
                         g_klepto_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_klepto_states[i].host;
    }
}
