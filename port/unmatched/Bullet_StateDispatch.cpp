/* HOST COPIES of BULLET's one pointer-to-member state machine (actor 233,
 * ov002 _ZTV6Bullet / _ZTV24daPropeller_Heyho_Fire_c 0x0210d654), the seat of
 * its two state-handler records, and nothing stubbed: both handlers are matched
 * src.
 *
 * BULLET is the projectile Snufit (func_ov065_02116364) fires. It runs a SINGLE
 * state: a 16-byte descriptor (data_ov002_02111190, ov002 bss) holding two mwcc
 * {function, delta} records, the ENTER half at +0 and the MAIN half at +8.
 * func_ov002_020fed2c (the state setter) stores the descriptor pointer at
 * this+0x350 and dispatches the ENTER half once; Bullet::Behavior dispatches the
 * MAIN half every frame. Both matched TUs form the pointer-to-member -- the
 * setter over a FORWARD-DECLARED struct (`struct C;`), Behavior over a complete
 * one -- so MSVC's pointer-to-member representation and __thiscall dispatch do
 * NOT reproduce the ROM's plain {function,delta} pair called cdecl with `this`.
 * The Klepto / PushBlock case: each body below is the matched source's control
 * flow line for line, only the dispatch read as a plain { fn, delta } and the fn
 * called with `this`.
 *
 * THE SEAT (the ov002 house order)
 * --------------------------------
 * data_ov002_02111190 is bss; __sinit_ov002_02108094 -- already in
 * slice_gate10.txt, already linked, run from walk_window / smoke_player -- copies
 * two SOURCE records (ov002 data 0x0210d600 -> func_ov002_020fed18 and
 * 0x0210d5f8 -> func_ov002_020fec94, each a real {fn,0}) into the descriptor:
 * cell[0].fn = 0x020fed18 (enter), cell[1].fn = 0x020fec94 (main). The ov002
 * mount lays those words down as DS CODE ADDRESSES, so after the sinit copy the
 * descriptor holds DS addresses. port_bullet_states_seat() rewrites the two DEST
 * fn words with the host bodies at vtable-fill time (the PushBlock/SignPost
 * gate-51 order: sinit first, seat the dest), validating each mounted word
 * against its ROM address first (WRONG-BYTES abort). Both deltas are 0.
 *
 * NO HOLE. Both handlers are matched src (slice_bullet.txt): func_ov002_020fed18
 * (enter, sets the lifetime timer) and func_ov002_020fec94 (main, the per-frame
 * fly/hit/expire logic a live fired Bullet runs). A Bullet reaches the main half
 * every frame after Snufit fires it; there is no stub in that path.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortPmf { unsigned fn; int delta; };

/* Behavior's C-linkage helpers (matched src or hosted) */
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self, void *cc);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wm,
                                              unsigned int j);
void func_ov002_020fed7c(void *self);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN12CylinderClsn6UpdateEv(void *self);

/* the descriptor the sinit fills: [0] = enter half, [1] = main half */
extern PortPmf data_ov002_02111190[2];

/* the two matched handlers (called by host pointer with `this` as arg 0) */
int func_ov002_020fed18(void *c);   /* enter: sets the lifetime timer to 200 */
int func_ov002_020fec94(void *c);   /* main : per-frame fly/collide/expire     */

}  /* extern "C" */

/* PORT_HOST_ABI: mwcc pointer-to-member on the forward-declared struct C. The
   matched func_ov002_020fed2c stores the descriptor pointer at this+0x350, reads
   the ENTER record (descriptor+0) and, if non-null, calls it with `this`. Read
   as a plain { fn, 0 }. */
extern "C" int func_ov002_020fed2c(void *cv, void *rec)
{
    char *c = (char *)cv;
    *(void **)(c + 0x350) = rec;                /* c->pp = rec */
    {
        PortPmf *q = *(PortPmf **)(c + 0x350);  /* q = c->pp */
        if (q[0].fn == 0)
            return 1;
        return ((int (*)(void *))(size_t)q[0].fn)(c);
    }
}

/* PORT_HOST_ABI: mwcc pointer-to-member (the MAIN half, descriptor+8). The
   matched Bullet::Behavior's control flow line for line. */
extern "C" int _ZN6Bullet8BehaviorEv(void *self)
{
    char *c = (char *)self;

    DecIfAbove0_Short((unsigned short *)(c + 0x100));

    {
        /* (((Actor*)c)->*(h->pmf))() -- h = descriptor at +0x350, pmf = the
           record at h+8; read as a plain { fn, 0 } and fn called with `this`. */
        PortPmf *m = *(PortPmf **)(c + 0x350);
        if (m[1].fn != 0)
            ((void (*)(void *))(size_t)m[1].fn)(c);
    }

    {
        int spd = *(int *)(c + 0xa8);
        int pos = *(int *)(c + 0x9c);
        int lim = *(int *)(c + 0xa0);
        int ac  = *(int *)(c + 0xac);
        int np  = spd + pos;
        if (np >= lim)
            lim = np;
        *(int *)(c + 0xa8) = lim;
        *(int *)(c + 0xac) = ac;
        _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(c, c + 0x110);
    }

    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x144, 0);

    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    func_ov002_020fed7c(c);

    _ZN12CylinderClsn5ClearEv(c + 0x110);
    _ZN12CylinderClsn6UpdateEv(c + 0x110);

    return 1;
}

/* ---- THE SEAT -------------------------------------------------------------
   The two dest fn words __sinit_ov002_02108094 filled with DS addresses, each
   matched against its own ROM address before the overwrite. Called from
   hal_fill_bullet_vtable (registration runs after the ov002 sinit chain). */
extern "C" void port_bullet_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    static const struct { unsigned half; unsigned rom; void *host; }
    seats[] = {
        {0, 0x020fed18, (void *)func_ov002_020fed18},   /* enter */
        {1, 0x020fec94, (void *)func_ov002_020fec94},   /* main  */
    };
    for (unsigned i = 0; i < sizeof seats / sizeof seats[0]; ++i) {
        PortPmf *p = &data_ov002_02111190[seats[i].half];
        if (p->fn != seats[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: BULLET state %u: the sinit left "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", seats[i].half, p->fn, p->delta,
                         seats[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)seats[i].host;
    }
}
