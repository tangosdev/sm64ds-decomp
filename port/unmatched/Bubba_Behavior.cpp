/* HOST COPY of src/func_ov032_021121b4.cpp -- BUBBA's Behavior, the TICK half
 * of its five-cell pointer-to-member state machine (id 228, ov032, level 24).
 * Run rel0215, lane cast-sweep2.
 *
 * WHY A HOST COPY. The matched src runs the tick dispatch INLINE, so the whole
 * TU has to be the host body rather than a one-line forwarder:
 *
 *     struct Klass; typedef void (Klass::*PMF)();
 *     struct M { char pad[8]; PMF pmf; };
 *     M *m = *(M **)(c + 0x3b0);
 *     if (m->pmf != 0) (((Klass *)c)->*(m->pmf))();
 *
 * `Klass` is never defined, so mwcc's PMF is the ROM's 8-byte
 * {function, delta} pair while MSVC's is the "unknown inheritance" form. The
 * `char pad[8]` steps over the cell's LO (enter) half to reach the HI (tick)
 * half, which is the Ukiki `c->pp + 1` step written the other way round.
 * The full reasoning is in port/unmatched/Bubba_StateEnter.cpp's header and is
 * not repeated; only the offset changes (cell + 8 rather than cell + 0).
 *
 * ROM 0x021121f0, the dispatch:
 *     ldr  r1, [r4, #0x3b0]      the cell
 *     ldr  r0, [r1, #8]          the HI half's function word
 *     cmp  r0, #0 -> skip
 *     add  r3, r1, #8
 *     ldr  r1, [r3, #4]          the HI half's delta word
 *     add  r0, r4, r1, asr #1    the receiver
 *     ands r1, r1, #1            bit 0 selects a VIRTUAL call
 *     ldrne r2, [r0] / ldrne r1, [r3] / ldrne r1, [r2, r1]
 *     ldreq r1, [r3]
 *     blx  r1
 *
 * Everything else below is the matched source statement for statement.
 * The matched src TU stays in src/ as the byte proof and is dropped from
 * port/slice_sweep2_ov032.txt.
 *
 * NOTE ON THE STATE COMPARE. The `!= data_ov032_02113aac` test picks the
 * animation rate: 0x2000 while BUBBA is in the cell at 0x02113aac (the chase),
 * 0x1000 otherwise. It compares the CELL POINTER, not a function address, so it
 * is unaffected by the seat rewriting the cells' function words -- the mounted
 * cell addresses are what both sides read.
 *
 * PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately incomplete
 * class; MSVC's PMF representation there does not reproduce the ROM's
 * {function, delta} pair.
 */
extern "C" {

struct CylinderClsn;
struct WithMeshClsn;
struct ModelAnim;

int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(
    void *self, WithMeshClsn *wm, ModelAnim *ma, unsigned int j);
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, CylinderClsn *cc);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, WithMeshClsn *wm,
                                              unsigned int j);
void func_ov032_02112044(char *c);
void _ZN9Animation7AdvanceEv(void *self);
void func_ov032_021113fc(void *self);
void _ZN12CylinderClsn5ClearEv(CylinderClsn *self);
void *_ZN5Actor13ClosestPlayerEv(void *self);
void _ZN12CylinderClsn6UpdateEv(CylinderClsn *self);
extern char data_ov032_02113aac[];

/* HOST COPY RETIRED, run link100 lane PMFB8 gate 2. src/func_ov032_021121b4.cpp
   dispatches BUBBA's tick half itself now: with block R8's /vmg /vmm the
   emitted member pointer IS the ROM's eight-byte {function, delta} pair, read
   at offset 8 of the cell exactly where `struct M { char pad[8]; PMF pmf; }`
   puts it, and the whole banner above about MSVC's representation expired with
   R8.

   WHAT CHANGED WITH IT. The FIVE TICK words of the five cells now hold zero-arg
   __fastcall faces rather than the raw cdecl bodies, because the matched TU's
   `call eax` puts the receiver in ecx and pushes nothing. The faces live beside
   the seat in hal/actor_classes_ov032.cpp. The five ENTER words are UNCHANGED:
   their dispatcher, src/func_ov032_02111ff4.cpp, is already in the link and
   emits a TAIL JUMP, so the cdecl body it reaches reads the caller's own first
   stack word, which is self.

   The cell-pointer comparison this file's banner discusses is untouched by any
   of it -- it compares the mounted cell ADDRESS, not a function word.

   The vtable-derived block sweep, the /FAsc listing and the tail-jump control
   are in port/slice_pmfb8.txt and runs/link100/out/PMFB8/. */
}
