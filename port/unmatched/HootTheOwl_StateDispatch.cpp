/* HOOT_THE_OWL (234, ov094, gate 194): the SEAT of the owl's five state cells
 * and the one host copy left of its state machine, the per-frame tick in
 * src/_ZN10HootTheOwl8BehaviorEv.cpp.
 *
 * THE CELLS. __sinit_ov094_021367e8 copies TEN 8-byte source records
 * (data_ov094_021369c0..02136a08, five {enter, tick} pairs) into five bss
 * cells, data_ov094_02136b30/b40/b50/b60/b70, each a 2-entry array.
 * func_ov094_02136188 (matched, port/slice_pmf3.txt) stores a cell pointer at
 * self+0x3c8 (mCurrentState) and dispatches cell[0], the ENTER record;
 * Behavior reads the same pointer back every frame and dispatches cell+8, the
 * TICK record (HootTheOwl::State's mMain in include/HootTheOwl.h).
 *
 * WHAT THE ROM DOES WITH A RECORD (run linkfull lane PMF1, disassembled out of
 * extracted/overlays/overlay_0094.bin at 0x02136488..0x021364b8, the same
 * five-instruction decode Klepto's Behavior carries at 0x0211c968):
 *
 *     ldr   r1, [r4, #0x3c8]      the cell
 *     ldr   r0, [r1, #8]          word 0 of the tick record: the null test
 *     add   r3, r1, #8
 *     ldr   r1, [r3, #4]          word 1
 *     add   r0, r4, r1, asr #1    receiver = this + (word 1 >> 1)
 *     ands  r1, r1, #1            word 1 bit 0 set: a VIRTUAL member, word 0
 *     ldrne ...                   is then an offset into the receiver's vtable
 *     ldreq r1, [r3]              clear: word 0 IS the code address
 *     blx   r1
 *
 * That is the ARM C++ ABI member pointer: the virtual flag lives in bit 0 of
 * the ADJUSTMENT word and the adjustment is stored doubled. MSVC's
 * pointer-to-member under /vmg /vmm (target-wide, port/CMakeLists.txt block
 * R8) is the same eight bytes in the same order, and its call is
 * `ecx = this + word 1; call word 0`: no shift and no virtual branch. The two
 * agree exactly when word 1 is zero, and it is zero in all ten ROM records:
 * each +4 word of 0x021369c0..0x02136a0c reads 00000000 in the image and no
 * relocation is recorded from any of them (config/arm9/overlays/ov094/
 * relocs.txt; runs/linkfull/out/PMF1/rom_hoot_records.txt). The seat below
 * aborts on any nonzero word 1 as well, so a wrong mount cannot reach either
 * dispatcher.
 *
 * THE RECEIVER RIDES IN ECX. mwcc hands the receiver over in r0; MSVC's call
 * puts it in ecx and pushes NOTHING, whether the transfer is Behavior's call
 * or func_ov094_02136188's tail jump (both set ecx = this + word 1 first). The
 * ten state bodies are extern "C" cdecl and read their receiver from [esp+4],
 * so every cell word, enter and tick alike, is seated with a __fastcall face
 * that takes ecx and calls the body with it (the Pokey / Ukiki shape, and the
 * face goes on every row: port/tools/pmf_guard.py's rule, because the face is
 * right for a call and a tail jump alike).
 *
 * THE SEAT runs from hal_fill_hoot_the_owl_vtable() in
 * hal/actor_classes_ov094.cpp, after the sinit has filled the live cells and
 * before InitResources can dispatch through func_ov094_02136188. It verifies
 * each live record reads the ROM's own {address, 0} first (a wrong mount
 * aborts loudly instead of seating garbage), then overwrites word 0 with the
 * face.
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

/* The ten faces: receiver in ecx, body called with it as its one cdecl
   argument. */
#define HOOT_FACE(a) static int __fastcall hoot_st_##a(void *s) \
    { return func_ov094_##a(s); }
HOOT_FACE(02136150) HOOT_FACE(02136024) HOOT_FACE(02135fe0) HOOT_FACE(02135ee0)
HOOT_FACE(02135e64) HOOT_FACE(02135c28) HOOT_FACE(02135bd4) HOOT_FACE(021359d8)
HOOT_FACE(0213598c) HOOT_FACE(021358b4)
#undef HOOT_FACE

static const struct { PortHootPmf *cell; unsigned enter_rom, tick_rom;
                       void *enter_host, *tick_host; }
g_hoot_cells[5] = {
    {data_ov094_02136b40, 0x02136150, 0x02136024, (void *)hoot_st_02136150, (void *)hoot_st_02136024},
    {data_ov094_02136b50, 0x02135fe0, 0x02135ee0, (void *)hoot_st_02135fe0, (void *)hoot_st_02135ee0},
    {data_ov094_02136b60, 0x02135e64, 0x02135c28, (void *)hoot_st_02135e64, (void *)hoot_st_02135c28},
    {data_ov094_02136b70, 0x02135bd4, 0x021359d8, (void *)hoot_st_02135bd4, (void *)hoot_st_021359d8},
    {data_ov094_02136b30, 0x0213598c, 0x021358b4, (void *)hoot_st_0213598c, (void *)hoot_st_021358b4},
};

/* Seat the five cells: verify the sinit copied the ROM's own {address, 0}
   pairs (WRONG BYTES aborts instead of silently calling into garbage), then
   overwrite each word 0 with its face. Called from
   hal_fill_hoot_the_owl_vtable() BEFORE InitResources can dispatch through
   func_ov094_02136188. */
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
   PMF3), and its tail jump sets ecx = this + word 1 before it jumps, so the
   faces above receive the owl there too. */

/* PORT_HOST_ABI: HootTheOwl::Behavior, host copy. The dispatch below is the
   matched TU's own: MSVC's 8-byte member pointer over the tick record,
   `mov ecx,[rec+0Ch] / add ecx,this / call [rec+8]` with the null test on
   word 0 alone, which is what src/_ZN10HootTheOwl8BehaviorEv.cpp compiles to
   under this build's flags (runs/linkfull/out/PMF1/msvc_hoot_behavior.asm).
   The rest is transcribed line for line off that TU with raw offsets.

   WHAT STILL KEEPS THE MATCHED TU OUT is its NAME, not its code. The TU
   defines ?Behavior@HootTheOwl@@UAEHXZ, and port/faces_sync.txt carries that
   member as a FORWARD face (the generated file defines it and calls the flat
   name this body defines), so linking the TU as it stands is a duplicate
   definition. Retiring this body is three steps: that ledger row flips from F
   to R (the face then defines _ZN10HootTheOwl8BehaviorEv and calls the
   member), this body goes, and the TU is enrolled. The faces above stay as
   they are. */
extern void DecIfAbove0_Short(void *);
extern void _ZN9Animation7AdvanceEv(void *);
extern void func_02012694(int, void *);
extern void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *, void *, unsigned int);
extern void _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(void *, void *);
extern void _ZN5dCc_c5ClearEv(void *);
extern void _ZN5dCc_c6UpdateEv(void *);
extern void func_ov094_021361d8(void *);
extern void func_ov094_021362e0(void *);
extern void func_ov094_021357a4(void *);

/* The matched TU's view of a state: eight bytes nothing reads, then the tick
   record as a real member pointer. The class is left incomplete on purpose;
   under /vmg /vmm its member pointer is the same eight bytes either way. */
struct PortHootOwl;
struct PortHootState {
    unsigned char pad_00[8];
    void (PortHootOwl::*mMain)();   /* 0x08 */
};

int _ZN10HootTheOwl8BehaviorEv(void *selfv)
{
    char *c = (char *)selfv;

    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    {
        PortHootState *o = *(PortHootState **)(c + 0x3c8);
        if (*(int *)((char *)o + 8) != 0)
            (((PortHootOwl *)c)->*(o->mMain))();
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
        _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(c, c + 0x150, 0);
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
    _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(c, c + 0x110);
    *(short *)(c + 0x8c) = *(short *)(c + 0x92);
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(short *)(c + 0x90) = *(short *)(c + 0x96);
    func_ov094_021361d8(c);
    if (*(char **)(c + 0x3c8) == (char *)data_ov094_02136b60 && *(unsigned char *)(c + 0x3d4) == 2) {
        func_ov094_021357a4(c);
    }
    _ZN5dCc_c5ClearEv(c + 0x110);
    _ZN5dCc_c6UpdateEv(c + 0x110);
    return 1;
}
}  /* extern "C" */
