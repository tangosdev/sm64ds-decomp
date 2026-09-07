/* HOST COPIES of src/func_ov094_02136188.cpp (the state-cell installer) and
 * the inline PMF dispatch inside src/_ZN10HootTheOwl8BehaviorEv.cpp (the
 * per-frame state tick) -- HOOT_THE_OWL's (234, ov094, gate 194) own state
 * machine. The Unagi/MrBlizzard/BabyPenguin shape: a REAL C++ pointer-to-
 * member dispatched through a DELIBERATELY INCOMPLETE class.
 *
 * func_ov094_02136188 forms:
 *     struct C; typedef int (C::*PMF)();
 *     struct C { char pad[0x3c8]; PMF *pp; };
 *     c->pp = p;
 *     PMF *q = c->pp;
 *     if (*q == 0) return 1;
 *     return (c->**q)();
 * -- store the cell pointer p at self+0x3c8 (mCurrentState) and immediately
 * dispatch cell[0] (the PMF itself, an 8-byte {fn,delta} pair). Behavior
 * reads the SAME stored pointer back every frame and dispatches through it
 * a second time (`Obj *o = *(Obj**)(this+0x3c8); if (o+8 != 0) (this->*o->fn)()`,
 * Obj{char pad[8]; Fn fn;} -- fn at +8 is the same one-word PMF encoding,
 * a non-virtual complete-class dispatch since every state cell's own delta
 * word is 0, ROM-byte verified against overlay_0094.bin the same way every
 * PMF table in this port has been).
 *
 * MSVC's PMF representation for an INCOMPLETE class (no base, no virtuals
 * declared at the typedef) is the "unknown inheritance" general form, not a
 * plain function pointer, and does not reproduce the ROM's own {function,
 * delta} semantics when called through this way -- the SoundObject/Cap/
 * MrBlizzard/BabyPenguin/Unagi disease, confirmed the same way every other
 * instance was: reading the recovered source's own struct declaration, not
 * inferred from a crash. Both dispatch sites are host copies, translated
 * field for field with the PMF call replaced by a plain function-pointer
 * call through the same struct layout the seat below establishes.
 *
 * THE SEAT (the MrBlizzard/BabyPenguin/Unagi/Amilift/LavaBubble "seat the
 * source before the copy" order). __sinit_ov094_021367e8 copies TEN 8-byte
 * SOURCE records (data_ov094_021369c0..02136a08, five {enter,tick} pairs)
 * into the five bss cells (data_ov094_02136b30/b40/b50/b60/b70, each a
 * 2-entry array) InitResources/Behavior/the ten state handlers all read by
 * address. Each source record's own fn word (reloc-confirmed against
 * config/arm9/overlays/ov094/relocs.txt) is one of the ten func_ov094_*
 * state-handler bodies -- CODE, which the mount's pointer-rebase pass does
 * NOT touch (it only rebases pointers into other MOUNTED DATA), so the raw
 * mounted words are DS addresses. port_hoot_the_owl_states_seat() rewrites
 * each source record's fn word with its HOST body's address BEFORE the
 * sinit runs, validating each mounted word against the ROM's own address
 * first (a wrong mount aborts loudly instead of seating garbage) -- called
 * from hal_fill_hoot_the_owl_vtable() in hal/actor_classes_ov094.cpp.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {
struct PortHootPmf { unsigned fn; int delta; };

/* the ten matched state-handler bodies (slice_gate194.txt) each cell's
   enter/tick fn word targets, reloc-confirmed against
   config/arm9/overlays/ov094/relocs.txt. */
int func_ov094_02135c28(void *self);
int func_ov094_02136024(void *self);
int func_ov094_02135bd4(void *self);
int func_ov094_02135e64(void *self);
int func_ov094_02136150(void *self);
int func_ov094_021358b4(void *self);
int func_ov094_0213598c(void *self);
int func_ov094_02135fe0(void *self);
int func_ov094_02135ee0(void *self);
int func_ov094_021359d8(void *self);

/* the five bss cells, each a 2-entry array of {fn,delta} -- [0]=enter,
   [1]=tick, per __sinit_ov094_021367e8's own copy order. */
extern PortHootPmf data_ov094_02136b30[2], data_ov094_02136b40[2],
    data_ov094_02136b50[2], data_ov094_02136b60[2], data_ov094_02136b70[2];

typedef int (*PortHootFn)(void *);
static const struct { PortHootPmf *cell; unsigned enter_rom, tick_rom;
                       PortHootFn enter_host, tick_host; }
g_hoot_cells[5] = {
    {data_ov094_02136b40, 0x02136150, 0x02136024, func_ov094_02136150, func_ov094_02136024},
    {data_ov094_02136b50, 0x02135fe0, 0x02135ee0, func_ov094_02135fe0, func_ov094_02135ee0},
    {data_ov094_02136b60, 0x02135e64, 0x02135c28, func_ov094_02135e64, func_ov094_02135c28},
    {data_ov094_02136b70, 0x02135bd4, 0x021359d8, func_ov094_02135bd4, func_ov094_021359d8},
    {data_ov094_02136b30, 0x0213598c, 0x021358b4, func_ov094_0213598c, func_ov094_021358b4},
};

/* Seat the five cells with HOST function addresses, the MrBlizzard/
   BabyPenguin/Unagi shape: verify the sinit copied the ROM's own {address,0}
   pairs (WRONG BYTES aborts instead of silently calling into garbage), then
   overwrite enter_fn/tick_fn with the host bodies' own addresses. Called
   from hal_fill_hoot_the_owl_vtable() BEFORE InitResources can dispatch
   through func_ov094_02136188. */
extern "C" void port_hoot_the_owl_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (int i = 0; i < 5; ++i) {
        PortHootPmf *cell = g_hoot_cells[i].cell;
        if (cell[0].fn != g_hoot_cells[i].enter_rom || cell[0].delta != 0 ||
            cell[1].fn != g_hoot_cells[i].tick_rom || cell[1].delta != 0) {
            std::fprintf(stderr, "FATAL: HootTheOwl state cell %d: the sinit "
                         "left %08x/%u %08x/%u, the ROM's own records say "
                         "%08x/0 %08x/0 -- WRONG BYTES\n", i,
                         cell[0].fn, cell[0].delta, cell[1].fn, cell[1].delta,
                         g_hoot_cells[i].enter_rom, g_hoot_cells[i].tick_rom);
            std::abort();
        }
        cell[0].fn = (unsigned)(size_t)g_hoot_cells[i].enter_host;
        cell[1].fn = (unsigned)(size_t)g_hoot_cells[i].tick_host;
    }
}

/* func_ov094_02136188, the state-cell installer, IS NO LONGER HOST-COPIED.
   src/func_ov094_02136188.cpp is on port/slice_pmf3.txt (run link100 lane
   PMF3): with /vmg /vmm target-wide, MSVC's pointer-to-member IS the ROM's
   8-byte {function, delta} pair, and the emitted body is a TAIL JUMP -- the
   caller's own cdecl frame survives, so the seated body still reads its
   receiver from [esp+4] and the extra `p` argument sits unread at [esp+8].
   The one thing the flag cannot decide is the adjustment, and it is decided
   twice here: port_hoot_the_owl_states_seat above aborts the binary on any
   nonzero delta before an owl can dispatch, and the ten source pairs at ov094
   0x021369c0..0x02136a08 were re-read out of overlay_0094.bin with their
   relocations and every adjustment word is ROM zero. The seat and the
   Behavior host copy below both stay: Behavior is a separate ruling. */

/* PORT_HOST_ABI: HootTheOwl::Behavior, host copy -- ONLY the inline PMF
   dispatch (the state cell's own "tick" word, read back from self+0x3c8)
   differs from the matched src; the rest is transcribed line for line off
   _ZN10HootTheOwl8BehaviorEv.cpp with raw offsets, including the
   func_ov094_021357a4 call -- the matched src spells this call
   "func_ov096_021357a4" (a dsd cross-overlay alias typo, port/ov094_syms.txt's
   own header has the full derivation), but since Behavior is a host copy
   here, the real ov094 name is called directly and no /alternatename
   bridge is needed. */
extern void DecIfAbove0_Short(void *);
extern void _ZN9Animation7AdvanceEv(void *);
extern void func_02012694(int, void *);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *, void *, unsigned int);
extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *, void *);
extern void _ZN12CylinderClsn5ClearEv(void *);
extern void _ZN12CylinderClsn6UpdateEv(void *);
extern void func_ov094_021361d8(void *);
extern void func_ov094_021362e0(void *);
extern void func_ov094_021357a4(void *);

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch; MSVC's PMF over an
 * incomplete class is the wider general representation. See the header. */
int _ZN10HootTheOwl8BehaviorEv(void *selfv)
{
    char *c = (char *)selfv;

    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    {
        /* the matched src's `struct Obj { char pad[8]; Fn fn; }` where Fn
           is itself the 8-byte {fn,delta} PMF pair -- o points at the SAME
           cell func_ov094_02136188 stored at c+0x3c8, and the src's
           `*(int*)(o+8)` reads the fn WORD at cell+8 (the pad[8] skips the
           cell's own leading 8 bytes, whatever those hold for this state)
           and tests it non-null before dispatching. */
        char *o = *(char **)(c + 0x3c8);
        unsigned fn = *(unsigned *)(o + 8);
        if (fn != 0)
            ((void (*)(void *))(size_t)fn)(c);
    }
    if (*(char **)(c + 0x3c8) == (char *)data_ov094_02136b40)
        return 1;
    *(int *)(c + 0x368) = *(int *)(c + 0x3f0);
    _ZN9Animation7AdvanceEv(c + 0x35c);
    {
        char *m = *(char **)(c + 0x3c8);
        if ((m == (char *)data_ov094_02136b50 || m == (char *)data_ov094_02136b60 ||
             m == (char *)data_ov094_02136b30) &&
            (unsigned short)(*(int *)(c + 0x364) >> 0xc) == 0) {
            func_02012694(0x139, c + 0x74);
        }
    }
    if (*(char **)(c + 0x3c8) == (char *)data_ov094_02136b70) {
        func_ov094_021362e0(c);
        *(short *)(c + 0x8c) = *(short *)(c + 0x92);
        *(short *)(c + 0x8e) = *(short *)(c + 0x94);
        *(short *)(c + 0x90) = *(short *)(c + 0x96);
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x150, 0);
        return 1;
    }
    {
        int s = *(int *)(c + 0xa8) + *(int *)(c + 0x9c);
        int m2 = *(int *)(c + 0xa0);
        int ac = *(int *)(c + 0xac);
        if (s >= m2) m2 = s;
        *(int *)(c + 0xa8) = m2;
        *(int *)(c + 0xac) = ac;
    }
    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(c, c + 0x110);
    *(short *)(c + 0x8c) = *(short *)(c + 0x92);
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(short *)(c + 0x90) = *(short *)(c + 0x96);
    func_ov094_021361d8(c);
    if (*(char **)(c + 0x3c8) == (char *)data_ov094_02136b60 && *(unsigned char *)(c + 0x3d4) == 2) {
        func_ov094_021357a4(c);
    }
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    _ZN12CylinderClsn6UpdateEv(c + 0x110);
    return 1;
}
}  /* extern "C" */
