/* HOST COPIES of src/func_ov100_02145550.cpp (daDoor_c::Behavior) and
 * src/func_ov100_021453d8.cpp (the callback installer it shares a table
 * with), plus the seat for the fifteen {function, delta} statics both
 * dispatch. The gate-16 pointer-to-member case for the sixth time, and the
 * first one where the table is a POINTER THE ACTOR CARRIES rather than an
 * index into a fixed array.
 *
 * ---- what the ROM actually does ------------------------------------------
 *
 * The door keeps a CALLBACK NODE pointer at +0x140. Each node is 0x10 bytes
 * and holds two mwcc pointers-to-member: one at +0, which the installer
 * dispatches the moment it seats the node, and one at +8, which Behavior
 * dispatches every frame. __sinit_ov100_02147698 builds nine of those nodes
 * out of the fifteen statics at ov100 0x021480d4..0x02148144 -- plus arm9's
 * data_02086b58, which is the NULL pair (both halves zero, which is what the
 * ROM mounts there), and both dispatch sites test word 0 against zero before
 * they will call.
 *
 * Read off the ROM at 0x02145570 and 0x02145400, the encoding is the pair
 * mwcc always emits:
 *
 *     fn    = word 0        delta = word 1
 *     this' = this + (delta >> 1)
 *     if (delta & 1)  fn is a BYTE OFFSET into this'-> vtable
 *     else            fn is the function address
 *
 * Both branches are spelled out below rather than asserted away. Every one of
 * the fifteen statics carries delta 0 in the ROM, so the virtual half is dead
 * on this level -- but it is one line, and a dead line that is right beats a
 * live assumption.
 *
 * ---- why these two are host copies ----------------------------------------
 *
 * Both source TUs write `struct C;` and form the pointer-to-member while C is
 * INCOMPLETE, which is the case MSVC answers with the four-word general
 * representation. That quadruples the node's stride and dispatches a
 * neighbour's body.
 *
 * ---- and why the statics are seated ---------------------------------------
 *
 * Those fifteen statics are the overlay image's own words -- DS CODE
 * ADDRESSES -- and the sinit copies them into the nodes. The seat rewrites
 * the STATICS before the sinit runs (the LakituBro reading: same guarantee,
 * one less mapping to get wrong), each checked against the ROM address its
 * host body was compiled from. ALL FIFTEEN ARE MATCHED SRC, so none of them
 * is trapped; the one trap at the bottom of this file is a different hole,
 * two calls further in.
 *
 * The dispatch is r0 = this, r1 = the argument, and the fifteen bodies do not
 * agree on how many arguments they admit to taking -- nine take two, six take
 * one. That is the ARM ride-through the port has met all through this gate:
 * the second argument is in r1 whether or not the callee reads it. Under
 * __cdecl the caller cleans the stack, so one call shape serves all fifteen.
 */
#include <cstdio>
#include <cstdlib>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern "C" {

void *func_ov100_02145370(void *self);

struct PortPmf { unsigned fn; int delta; };
extern PortPmf data_ov100_021480d4[], data_ov100_021480dc[],
    data_ov100_021480e4[], data_ov100_021480ec[], data_ov100_021480f4[],
    data_ov100_021480fc[], data_ov100_02148104[], data_ov100_0214810c[],
    data_ov100_02148114[], data_ov100_0214811c[], data_ov100_02148124[],
    data_ov100_0214812c[], data_ov100_02148134[], data_ov100_0214813c[],
    data_ov100_02148144[];

int func_ov100_02144730(void *, void *);
int func_ov100_02144ccc(void *, void *);
int func_ov100_02144c6c(void *, void *);
int func_ov100_02144bf4(void *, void *);
int func_ov100_02144c64(void *, void *);
int func_ov100_021449c8(void *, void *);
int func_ov100_02144a38(void *, void *);
int func_ov100_02144468(void *, void *);
int func_ov100_02144950(void *, void *);
int func_ov100_021446f8(void *, void *);
int func_ov100_02144528(void *, void *);
int func_ov100_021444e8(void *, void *);
int func_ov100_02144cf8(void *, void *);
int func_ov100_0214491c(void *, void *);

}  /* extern "C" */

typedef int (*PortDoorFn)(void *, void *);

/* RUN link100 LANE PMFB8 GATE 2: THE FIFTEEN STATICS TAKE __fastcall FACES.
   src/func_ov100_021453d8.cpp and src/func_ov100_02145550.cpp are in the link
   (port/slice_pmfb8.txt) and dispatch these records themselves. Their emitted
   sides agree word for word --
       021453d8   mov edx,[ecx] / test / mov ecx,[ecx+4] / push a2 /
                  add ecx,eax / call edx
       02145550   mov edx,[ecx+8] / test / mov ecx,[ecx+12] / push eax /
                  add ecx,esi / call edx
   -- receiver in ecx, ONE pushed word neither caller pops because __thiscall's
   stack half is callee-popped. So ONE face shape serves both halves of the
   node: __fastcall(self, dead_edx, void *arg), which pops that word itself.
   FOURTEEN faces for the fourteen distinct code words (0x02144730 is held by
   two of the fifteen statics), one face per code word.
   The bodies keep the (void *, void *) declaration this file has always used:
   nine of them take two words and six take one, the caller cleans under
   __cdecl, and r1 is set on ARM whether or not the callee reads it -- the ride
   -through this file's header already rules on. */
#define DOOR_FACE(sym)                                                     \
    static int __fastcall door_f_##sym(void *self, void *dead_edx, void *arg) \
    {                                                                      \
        (void)dead_edx;                                                    \
        return sym(self, arg);                                             \
    }

DOOR_FACE(func_ov100_02144730)
DOOR_FACE(func_ov100_02144ccc)
DOOR_FACE(func_ov100_02144c6c)
DOOR_FACE(func_ov100_02144bf4)
DOOR_FACE(func_ov100_02144c64)
DOOR_FACE(func_ov100_021449c8)
DOOR_FACE(func_ov100_02144a38)
DOOR_FACE(func_ov100_02144468)
DOOR_FACE(func_ov100_02144950)
DOOR_FACE(func_ov100_021446f8)
DOOR_FACE(func_ov100_02144528)
DOOR_FACE(func_ov100_021444e8)
DOOR_FACE(func_ov100_02144cf8)
DOOR_FACE(func_ov100_0214491c)

static const struct { PortPmf *slot; unsigned rom; PortDoorFn host; }
g_door_callbacks[] = {
    {data_ov100_021480d4, 0x02144730, (PortDoorFn)door_f_func_ov100_02144730},
    {data_ov100_021480dc, 0x02144ccc, (PortDoorFn)door_f_func_ov100_02144ccc},
    {data_ov100_021480e4, 0x02144c6c, (PortDoorFn)door_f_func_ov100_02144c6c},
    {data_ov100_021480ec, 0x02144bf4, (PortDoorFn)door_f_func_ov100_02144bf4},
    {data_ov100_021480f4, 0x02144c64, (PortDoorFn)door_f_func_ov100_02144c64},
    {data_ov100_021480fc, 0x021449c8, (PortDoorFn)door_f_func_ov100_021449c8},
    {data_ov100_02148104, 0x02144730, (PortDoorFn)door_f_func_ov100_02144730},
    {data_ov100_0214810c, 0x02144a38, (PortDoorFn)door_f_func_ov100_02144a38},
    {data_ov100_02148114, 0x02144468, (PortDoorFn)door_f_func_ov100_02144468},
    {data_ov100_0214811c, 0x02144950, (PortDoorFn)door_f_func_ov100_02144950},
    {data_ov100_02148124, 0x021446f8, (PortDoorFn)door_f_func_ov100_021446f8},
    {data_ov100_0214812c, 0x02144528, (PortDoorFn)door_f_func_ov100_02144528},
    {data_ov100_02148134, 0x021444e8, (PortDoorFn)door_f_func_ov100_021444e8},
    {data_ov100_0214813c, 0x02144cf8, (PortDoorFn)door_f_func_ov100_02144cf8},
    {data_ov100_02148144, 0x0214491c, (PortDoorFn)door_f_func_ov100_0214491c},
};

extern "C" void port_door_callbacks_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0;
         i < sizeof g_door_callbacks / sizeof g_door_callbacks[0]; ++i) {
        PortPmf *p = g_door_callbacks[i].slot;
        if (p->fn != g_door_callbacks[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: Door callback %u: the mount holds "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta,
                         g_door_callbacks[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_door_callbacks[i].host;
    }
}

/* HOST COPIES RETIRED, run link100 lane PMFB8 gate 2. src/func_ov100_021453d8.cpp
   (the node installer) and src/func_ov100_02145550.cpp (daDoor_c::Behavior)
   dispatch the node themselves now, and the three helpers that only they used
   -- port_door_call, port_door_watch and port_door_fn_in_image -- went with
   them.

   WHAT WENT, AND WHY THAT IS SAFE. The .text-bounds refusal was added on
   2026-08-07 after real play dispatched an arena address through a door node,
   with the note "until that write is found". It was found in the SAME commit
   that added it: 34285a74f names gate-22's thiscall aliases as the cause --
   cdecl callers bound straight onto __thiscall bodies, so GoBehindPlayer's
   `ret 4` inside func_ov100_02144730 shifted the frame and its epilogue
   returned onto the door argument -- and replaced them with nine cdecl faces in
   hal/method_faces.cpp and hal/door_ring_faces.cpp. The refusal was kept as a
   diagnostic, not as a live shield, and the write it was watching for has not
   been seen since.

   WHAT STAYS IS STRONGER THAN WHAT WENT. port_door_callbacks_seat below still
   reads all fifteen mounted records and ABORTS unless every one holds the ROM's
   own {address, 0} before it writes a single face, so a wrong mount still stops
   the binary instead of dispatching into it; and both matched TUs keep the
   ROM's own `if (word 0 == 0) skip`, which is what makes the null pair at
   arm9's data_02086b58 safe in the nine nodes that carry it.

   The vtable-derived block sweep, the two /FAsc listings and the arity reading
   are in port/slice_pmfb8.txt and runs/link100/out/PMFB8/. */

/* The hole that used to be here (Player::CanEnterDoor trapped by name while
 * its src body was NONMATCHING) closed 2026-08-07: the div=1 residue fell to
 * the C++-virtual-dispatch respelling of the +0x48 vcall (PR #1223, notes
 * 6bc), so the matched body now compiles from src/ via slice_gate22.txt like
 * any other function, and the trap that fired the moment real play reached a
 * door is gone. */
