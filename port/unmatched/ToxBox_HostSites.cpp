/* HOST COPY of the ONE ov092 TOX_BOX (actor 309, 8daOnms_c) site MSVC cannot
 * deliver from the matched source, run rel0215 wave 2 lane cast-lvl16pair.
 * The matched TU stays byte-locked in src/ and is simply not referenced from
 * port/slice_ov092.txt, which carries the refusal in full.
 *
 * THE SHAPE, read off the recovered source's own declarations rather than
 * inferred from a crash. src/_ZN6ToxBox8BehaviorEv.cpp opens:
 *
 *     struct ToxBox;                        <- deliberately incomplete HERE
 *     typedef void (ToxBox::*PMF)();
 *     struct TableEnt { PMF pmf; };
 *     extern TableEnt data_ov092_02132568[];
 *     struct ToxBox { char pad[0x568]; int state; };   <- completed AFTER
 *
 * and dispatches `(self->*data_ov092_02132568[idx].pmf)()`. mwccarm forms the
 * pointer-to-member on the incomplete class as the ROM's 8-byte
 * {function, delta} record. MSVC picks its "unknown inheritance" GENERAL form,
 * which is 16 bytes, so BOTH halves of that one expression break at once:
 *   * the call goes through a representation the ROM never wrote, and
 *   * `data_ov092_02132568[idx]` strides by 16 where the ROM strides by 8,
 *     so index 1 already reads the middle of record 0.
 * The eighth instance of the PMF disease in this port (SoundObject, Cap,
 * MrBlizzard, BabyPenguin, Unagi, HootTheOwl, the six ov077 sites, this).
 *
 * WHAT THE RECORDS HOLD AND WHO PUTS THEM THERE. ov092's NINE PMF source
 * records live at 0x02132240..0x02132288, each {fn, 0}, and
 * __sinit_ov092_021320cc copies them into the bss array data_ov092_02132568
 * in a PERMUTED order that is the state table:
 *
 *   state  source record        handler                ROM address
 *     0    data_ov092_02132268  func_ov092_021315ac    0x021315ac
 *     1    data_ov092_02132280  func_ov092_02131578    0x02131578
 *     2    data_ov092_02132258  func_ov092_021316b0    0x021316b0
 *     3    data_ov092_02132240  func_ov092_02131680    0x02131680
 *     4    data_ov092_02132250  func_ov092_02131650    0x02131650
 *     5    data_ov092_02132260  func_ov092_02131620    0x02131620
 *     6    data_ov092_02132248  func_ov092_021311b0    0x021311b0
 *     7    data_ov092_02132278  func_ov092_02131010    0x02131010
 *     8    data_ov092_02132270  func_ov092_02130fcc    0x02130fcc
 *
 * The permutation is the sinit's, transcribed from src/__sinit_ov092_021320cc.c
 * and cross-checked against the ROM's own relocations at 0x021321e8..0x0213220c;
 * the handler addresses are the relocation targets of the nine source records
 * (config/arm9/overlays/ov092/relocs.txt, from:0x02132240 through
 * from:0x02132280). Two independent bodies confirm the numbering:
 * func_ov092_021313b0 writes states 1..5 into +0x568 off the path walk, and
 * func_ov092_021311b0 (state 6, the fall) writes 7 and 8 for the two landing
 * outcomes -- which is exactly where those two states sit in the table above.
 *
 * THE SEAT: port_ov092_states_seat() in hal/actor_classes_ov092.cpp rewrites
 * each mounted source record's fn word with its HOST body's address, VALIDATED
 * against the ROM's own address first so a wrong mount aborts loudly instead of
 * seating garbage, and it runs BEFORE __sinit_ov092_021320cc copies the nine
 * records into bss. This file therefore reads a HOST function pointer out of
 * record[idx].fn and calls it directly. The MrBlizzard/BabyPenguin/Unagi/
 * HootTheOwl/ov077 order.
 *
 * THE RETURN-TYPE NOTE, because it differs from ov077's seat. The ROM's own
 * typedef is `void (ToxBox::*PMF)()`, and the nine handlers do not agree with
 * each other: func_ov092_02131010 returns int, the other eight return void.
 * On the DS that is r0, which this dispatch discards. The host table is typed
 * void-returning to match the ROM's typedef, and the one int-returning body is
 * reached through it on plain cdecl, where an ignored eax is exactly the same
 * discard. ov077's seat could type its table int-returning because all thirty
 * of its handlers agreed; this one cannot, and the ROM's typedef settles it.
 *
 * EVERY OTHER STATEMENT below is transcribed line for line from the matched
 * body with its own offsets. Only the pointer-to-member call is respelled.
 */
extern "C" {

struct PortOv092Pmf { unsigned int fn; int delta; };
typedef void (*PortOv092StateFn)(void *);
extern PortOv092Pmf data_ov092_02132568[];

struct PortOv092Vec3 { int x, y, z; };
struct PortOv092Mtx43 { int m[12]; };

void func_ov092_02131aec(void *c);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void func_ov092_02131a88(char *self);
void _ZN12CylinderClsn5ClearEv(void *m);
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void *m,
                                                                     void *v);
void _ZN12CylinderClsn6UpdateEv(void *m);

/* PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class, and an
   array strided by the ROM's 8-byte record rather than MSVC's 16. */
int _ZN6ToxBox8BehaviorEv(void *selfv)
{
    char *c = (char *)selfv;
    PortOv092Vec3 v;

    *(unsigned char *)(c + 0x576) = 1;
    *(unsigned char *)(c + 0x577) = 0;

    {   /* int idx = self->state;
           (self->*data_ov092_02132568[idx].pmf)();
           (*(unsigned short *)(c + 0x564))++;
           if (idx != self->state) *(unsigned short *)(c + 0x564) = 0; */
        int idx = *(int *)(c + 0x568);
        ((PortOv092StateFn)(size_t)data_ov092_02132568[idx].fn)(c);
        {
            unsigned short *cnt = (unsigned short *)(c + 0x564);
            (*cnt)++;
        }
        if (idx != *(int *)(c + 0x568))
            *(unsigned short *)(c + 0x564) = 0;
    }

    func_ov092_02131aec(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
        func_ov092_02131a88(c);

    if (*(unsigned char *)(c + 0x577) == 1) {
        if (*(short *)(c + 0x8c) != 0 || *(short *)(c + 0x8e) != 0 ||
            *(short *)(c + 0x90) != 0) {
            *(PortOv092Mtx43 *)(c + 0x528) = *(PortOv092Mtx43 *)(c + 0xf0);
            *(short *)(c + 0x8c) = 0;
            *(short *)(c + 0x8e) = 0;
            *(short *)(c + 0x90) = 0;
        }
    }

    _ZN12CylinderClsn5ClearEv(c + 0x4e8);
    v.x = 0;
    v.y = -0xfa000;
    v.z = 0;
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x4e8,
                                                                    &v);
    _ZN12CylinderClsn6UpdateEv(c + 0x4e8);
    return 1;
}

}  /* extern "C" */
