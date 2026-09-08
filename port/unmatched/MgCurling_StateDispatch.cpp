// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, per-class half:
// dScMgCurling_c's five dispatching TUs and its twenty-five state addresses.
// Run link60, lane MG2.
//
// Read unmatched/MgBase_StateDispatch.cpp's header first. It carries the ROM
// disassembly of the dispatch sequence, the measurement that every adjustment
// word in this closure is zero, the finding that the constructors already copy
// the pairs correctly, and the finding that the wall is twelve TUs where the
// link names six. None of that is repeated here.
//
// THIS FILE IS THE FAN-OUT'S TEMPLATE. Everything in it is one class's: five
// host copies and one address switch over twenty-five addresses. The next
// twenty-nine lanes write one of these and reuse the framework file unchanged,
// which is the split port/mg_fanout_costs.txt section 2 calls the whole cost
// model.
//
// ---- THE TWENTY-FIVE ADDRESSES, AND WHERE THEY COME FROM ------------------
//
// Not guessed and not read out of a running table. src/__sinit_ov006_021304ac.c
// names, in source order, which .data pair fills which .bss slot, and the pairs
// were read out of extracted/overlays/overlay_0006.bin at (addr - 0x020bfec0).
// Twenty-five pairs, every one {code, 0}, and the seven tables they fill:
//
//   table              n  arity  state addresses (in slot order)
//   ---------------    -  -----  --------------------------------------------
//   data_ov006_021418b0 2    0    020e1b54  020e1854
//   data_ov006_021418c0 3    1    020e1264  020e1214  020e0d84
//   data_ov006_021418d8 3    1    020e0884  020e07b0  020e071c
//   data_ov006_021418f0 4    1    020e1100  020e0ff0  020e0edc  020e0e18
//   data_ov006_02141910 4    1    020e2c08  020e2868  020e285c  020e26f8
//   data_ov006_02141930 4    1    020e0ca0  020e0b64  020e0a24  020e091c
//   data_ov006_02141950 5    0    020e3210  020e3078  020e2f78  020e2ebc
//                                 020e2eb8
//
// 2+3+3+4+4+4+5 = 25, which is the count hal/scene_mg.cpp section 4 states.
// Twenty-four have a matched src TU in port/slice_mg1.txt. The twenty-fifth is
// the hole below.
//
// ---- func_ov006_020e1854 WAS A HARD FLOOR. AMENDED, run link60 lane CT1 ---
//
// IT IS HOSTED NOW. unmatched/MgCurling_State_020e1854.cpp transcribes the
// state from the ROM (0x300 bytes, 186 instructions, read out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0 with the base proved
// against relocs.txt:11726), and this switch calls it. Three callees, all
// already matched, so the hole did not cascade. The state is the aim-and-charge
// half of the shot: stylus down, cursor tracks it inside a fixed box, scrubbing
// builds power at +0x4ec8 and aim at +0x4ede, and releasing sets +0x4ee4 back
// to 0 and +0x4ee5 to 1 for the parent to act on.
//
// WHAT DID NOT CHANGE, and must not be read as changed: the DECOMP still has no
// body and no delink block for 0x020e1854. stategen.py still reports it under
// REFUSALS and that report is still true. The host body is therefore called
// port_mg_curling_st_020e1854, not the ROM's symbol -- see that file's banner.
// If the refusal ever disappears without a src TU landing, something lied.
//
// The paragraph below is the ORIGINAL finding, kept because it is the evidence
// that the floor was real and because hal/scene_mg_faces.cpp section 3 and
// hal/scene_mg.cpp section 6 still quote it. Those two comments are now stale
// on this one point and are left for their owning lanes rather than edited from
// here.
//
// ---- THE ORIGINAL FINDING -------------------------------------------------
//
// It is slot 1 of data_ov006_021418b0, it has no delink block in
// config/arm9/overlays/ov006/delinks.txt and no src file defines it, so
// dScMgCurling_c cannot reach one of its own states however the dispatch is
// written. hal/scene_mg_faces.cpp's section 3 recorded that it carries no trap
// there "because nothing in the slice references it by name"; THAT IS NO
// LONGER TRUE OF THE ADDRESS, and this switch is what changed it. It is still
// not given a symbol -- inventing a definition for a body with no source is
// the guess port/tools/inferred_stub_guard exists to refuse -- so the case is
// handled here, at the dispatch site, where the report says which state was
// wanted rather than only that something was missing.
//
// ---- TWO RIDE-THROUGHS, TRANSCRIBED AND NOT REPAIRED ----------------------
//
// func_ov006_020e2eb8 sits in a zero-argument slot and takes (void).
// func_ov006_020e285c sits in a ONE-argument slot and also takes (void).
// Both are the ARM ride-through the port keeps meeting: on the DS r0 and r1
// already hold self and the index and a body that ignores them is correct.
// Calling them with no arguments from here is the faithful host form, because
// MSVC's __cdecl is caller-cleaned and a callee that declares nothing reads
// nothing. They are called as declared and the switch says so at each site.
// This is NOT the defect class of hal/scene_actor_faces.cpp's veneers, where a
// (void) callee stood between a caller and a body that DID want arguments.
//
// AMENDED, run link60 lane CUR2. THE SECOND HALF OF THAT IS NO LONGER TRUE OF
// func_ov006_020e285c, and the reason is a change this lane made rather than
// an error in the paragraph above. 020e285c forwards to func_ov006_020e20bc,
// which WAS a return-0 face -- a body that ignores its arguments, exactly the
// case the paragraph describes. This lane transcribed 020e20bc from the ROM
// (port/unmatched/MgCurling_Collide_020e20bc.cpp) and it wants (self, idx), so
// the (void) veneer became precisely the scene_actor_faces defect class named
// in the last sentence.
//
// THE FIX IS THE TAIL JUMP AND NOT A HOST COPY. src/func_ov006_020e285c.c
// stays in port/slice_mg1.txt exactly as it is -- the first attempt took it
// out, hosted it, and cost the gate a linked TU (5831 -> 5830) for nothing.
// The case below now declares 020e285c as (char *, int) and passes (c, a);
// MSVC compiles the one-call forwarder as a jmp, which reuses THIS frame, so
// both arguments are where the collision body reads them. That is not left to
// luck: port/tools/tailjump_guard.py carries a CLASS C row for the pair and
// fails the build if the jmp ever becomes a call.
//
// func_ov006_020e2eb8 is UNCHANGED and still called with no arguments. Its
// target is not affected by anything here.

#include <cstdio>
#include <cstdlib>   /* std::fprintf and std::abort, for the boot installer
                        run link100 lane PMFB5 added at the end of this file */

struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's one entry point; see MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);
void port_mg_call1(void *self, unsigned code, int adj, int a);

/* ---- the twenty-four matched state bodies, in address order ------------- */
void func_ov006_020e071c(char *c, int i);
void func_ov006_020e07b0(char *o, int i);
void func_ov006_020e0884(char *c, int i);
void func_ov006_020e091c(char *base, int i);
void func_ov006_020e0a24(char *base, int idx);
void func_ov006_020e0b64(char *base, int index);
void func_ov006_020e0ca0(char *o, int i);
void func_ov006_020e0d84(char *c, int i);
void func_ov006_020e0e18(char *base, int idx);
void func_ov006_020e0edc(char *c, int idx);
void func_ov006_020e0ff0(void *base, int idx);
void func_ov006_020e1100(char *c, int idx);
void func_ov006_020e1214(char *base, int idx);
void func_ov006_020e1264(char *c, int idx);
void func_ov006_020e1b54(char *c);

/* THE TWENTY-FIFTH STATE, hosted as of run link60 lane CT1. Deliberately NOT
   named func_ov006_020e1854: the decomp still has no body and no delink block
   for that address, and stategen.py's reconstruct check (run by
   port/build-port.cmd before configure) fails the build if this switch calls a
   `func_*` or `_Z*` symbol for such an address. `port_` is the repo's existing
   way to say "host body, not decomp symbol" -- the same ruling stategen already
   prints for port_player_st_climb_main at 0x020cb5bc. Derivation, provenance
   and the full ROM listing are in unmatched/MgCurling_State_020e1854.cpp. */
void port_mg_curling_st_020e1854(char *c);
void func_ov006_020e26f8(void *w, int i);
void func_ov006_020e285c(char *c, int idx);  /* one-argument slot; riders named
                                                by lane CUR2, see the header */
void func_ov006_020e2868(char *c, int idx);
void func_ov006_020e2c08(char *self, int idx);
void func_ov006_020e2eb8(void);              /* zero-argument slot, (void) body */
void func_ov006_020e2ebc(char *thiz);
void func_ov006_020e2f78(char *c);
void func_ov006_020e3078(char *c);
void func_ov006_020e3210(char *c);

/* the seven mount tables, re-typed to the ROM's eight-byte pair. The mount
   defines the storage; __sinit_ov006_021304ac fills it at scene load. */
extern MgPmf data_ov006_021418b0[];
extern MgPmf data_ov006_021418c0[];
extern MgPmf data_ov006_021418d8[];
extern MgPmf data_ov006_021418f0[];
extern MgPmf data_ov006_02141910[];
extern MgPmf data_ov006_02141930[];
extern MgPmf data_ov006_02141950[];

/* the ordinary callees the host copies below keep */
int  RandomIntInternal(int *seed);
void func_ov006_020e1680(char *c);
extern int data_0209d4b8;

/* host-copied further down this file, and called by the Behavior host copy
   above its own definition */
void func_ov006_020e12d0(char *o);

/* the boot installer at the end of this file; hal/scene_mg.cpp calls it after
   the ov006 constructors have filled the tables (run link100 lane PMFB5). */
void port_mg_curling_states_seat(void);

}  /* extern "C" */

// ---- the class's address switch --------------------------------------------

static unsigned g_curling_state_hits;

extern "C" int port_mg_try_ov006_0(void *self, unsigned code)
{
    char *c = (char *)self;
    ++g_curling_state_hits;
    switch (code) {
    case 0x020e1b54u: func_ov006_020e1b54(c); return 1;
    case 0x020e1854u: port_mg_curling_st_020e1854(c); return 1;
    case 0x020e2eb8u: func_ov006_020e2eb8();  return 1;   /* (void) body */
    case 0x020e2ebcu: func_ov006_020e2ebc(c); return 1;
    case 0x020e2f78u: func_ov006_020e2f78(c); return 1;
    case 0x020e3078u: func_ov006_020e3078(c); return 1;
    case 0x020e3210u: func_ov006_020e3210(c); return 1;
    default: --g_curling_state_hits;          return 0;
    }
}

extern "C" int port_mg_try_ov006_1(void *self, unsigned code, int a)
{
    char *c = (char *)self;
    ++g_curling_state_hits;
    switch (code) {
    case 0x020e071cu: func_ov006_020e071c(c, a); return 1;
    case 0x020e07b0u: func_ov006_020e07b0(c, a); return 1;
    case 0x020e0884u: func_ov006_020e0884(c, a); return 1;
    case 0x020e091cu: func_ov006_020e091c(c, a); return 1;
    case 0x020e0a24u: func_ov006_020e0a24(c, a); return 1;
    case 0x020e0b64u: func_ov006_020e0b64(c, a); return 1;
    case 0x020e0ca0u: func_ov006_020e0ca0(c, a); return 1;
    case 0x020e0d84u: func_ov006_020e0d84(c, a); return 1;
    case 0x020e0e18u: func_ov006_020e0e18(c, a); return 1;
    case 0x020e0edcu: func_ov006_020e0edc(c, a); return 1;
    case 0x020e0ff0u: func_ov006_020e0ff0(c, a); return 1;
    case 0x020e1100u: func_ov006_020e1100(c, a); return 1;
    case 0x020e1214u: func_ov006_020e1214(c, a); return 1;
    case 0x020e1264u: func_ov006_020e1264(c, a); return 1;
    case 0x020e26f8u: func_ov006_020e26f8(c, a); return 1;
    case 0x020e285cu: func_ov006_020e285c(c, a); return 1;  /* riders named */
    case 0x020e2868u: func_ov006_020e2868(c, a); return 1;
    case 0x020e2c08u: func_ov006_020e2c08(c, a); return 1;
    /* arity-0 body reached through a one-argument slot. data_ov006_021418b0 is
       an arity-0 table so this case should never fire, but the ROM body ignores
       r1 anyway, so calling it here is the same ride-through this file already
       documents for func_ov006_020e285c. */
    case 0x020e1854u: port_mg_curling_st_020e1854(c); return 1;
    default: --g_curling_state_hits;             return 0;
    }
}

extern "C" unsigned port_mg_curling_state_hits(void)
{
    return g_curling_state_hits;
}

// ---- the five host copies --------------------------------------------------

/* src/func_ov006_020e12d0.cpp. data_0209d4b8 keeps its C++ spelling so the
   generated alias for ?data_0209d4b8@@3HA still has its reference. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgCurling_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020e12d0(char *o)
{
    int i;
    char *e = o;
    for (i = 0; i < 0x32; i++) {
        if (*(unsigned char *)(e + 0x47a8) != 0) {
            unsigned char idx = *(unsigned char *)(e + 0x47a9);
            const MgPmf *p = &data_ov006_021418c0[idx];
            port_mg_call1(o, p->code, p->adj, i);
            if ((*(int *)(e + 0x4790) >> 0xc) >= 0xc8) {
                *(int *)(e + 0x478c) = (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf << 0xf;
                *(int *)(e + 0x4790) = -0x8000;
                *(unsigned char *)(e + 0x47aa) = 0;
                *(unsigned char *)(e + 0x47a9) = 0;
                *(unsigned char *)(e + 0x47ab) = 0;
            }
        }
        e += 0x24;
    }
}

/* src/func_ov006_020e0d84.cpp. Two dispatches, two tables, both one-argument. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgCurling_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020e0d84(char *c, int i)
{
    int idx = i * 0x24;
    unsigned char k0 = *(unsigned char *)(c + idx + 0x4000 + 0x7aa);
    const MgPmf *p0 = &data_ov006_02141930[k0];
    port_mg_call1(c, p0->code, p0->adj, i);
    unsigned char k1 = *(unsigned char *)(c + idx + 0x4000 + 0x7ab);
    const MgPmf *p1 = &data_ov006_021418d8[k1];
    port_mg_call1(c, p1->code, p1->adj, i);
}

/* src/func_ov006_020e1214.cpp. SILENT: its table was declared inside
   extern "C", so the link never named it. */
#define F1E(b, i) (*(unsigned char *)((char *)(b) + 0x47aa + (i) * 0x24))

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgCurling_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020e1214(char *base, int idx)
{
    unsigned char state = F1E(base, idx);
    const MgPmf *p = &data_ov006_021418f0[state];
    port_mg_call1(base, p->code, p->adj, idx);
}

// ---- THREE TABLES SEATED AS ONE PIECE, AND BOTH HOST COPIES ARE GONE -------
//
// Run link100 lane PMFB5. dScMgCurling_c's two remaining dispatchers move
// TOGETHER or not at all, and lane PMFB4 recorded why when it left them:
//
//   func_ov006_020e3078  READS TWO TABLES, not the one the census names.
//        data_ov006_021418b0 at its first dispatch site and
//        data_ov006_02141910 at its second, one call each. Seating only the
//        table the census named would have left the second dispatch reading a
//        cartridge code word and calling it.
//   func_ov006_020e3528  reads data_ov006_02141950, and SLOT 1 OF THAT TABLE
//        IS func_ov006_020e3078 ITSELF. Seating 02141950 without seating
//        021418b0 and 02141910 would install a face pointing at a host copy
//        that is about to be retired; retiring 020e3078 without seating
//        02141950 would leave 020e3528's host copy handing the switch an
//        address whose body had moved.
//
// So all three tables are installed by ONE installer and both host copies come
// off in the same gate.
//
//   func_ov006_020e3078   data_ov006_021418b0   2 slots   arity 0
//                         data_ov006_02141910   4 slots   arity 1
//   func_ov006_020e3528   data_ov006_02141950   5 slots   arity 0
//
// THE STRIDE, ROM SIDE, read at each body's OWN address out of
// extracted/overlays/overlay_0006.bin at ov006 base 0x020bfec0
// (runs/link100/out/PMFB5/rom_gate2.txt and dis_020e3078.txt, which
// disassembles PAST the first indirect call because the first reader stopped
// there and could not see the second site):
//   020e3078  add r3, r1, r0, lsl #3 at 020e30fc  pool 020e3204 = 021418b0
//             add r3, r5, r0, lsl #3 at 020e3178  pool 020e3208 = 02141910
//   020e3528  add r3, r1, r0, lsl #3 at 020e353c  pool 020e3574 = 02141950
// -> EIGHT at all three sites. The `add r6, r6, #0x2c` in 020e3078 walks the
// FIVE-SHELL array off the cursor, not a table.
//
// THE STRIDE, EMITTED SIDE, off each matched TU's own /FAsc listing under the
// port's own flags (runs/link100/out/PMFB5/emit_gate2_out.txt): [eax*8] and
// [eax*8+4] at all three sites. ROM 8 == emitted 8. /Zp4 is a MEASURED NO-OP on
// both TUs -- compiled both ways, the listings differ only in the TITLE line
// naming the .obj.
//
// THE ELEVEN SOURCE PAIRS ALL READ {code, 0} in overlay_0006.bin at the
// addresses src/__sinit_ov006_021304ac.c copies each slot from -- one
// constructor for all three tables -- all whole-pair copies, no field-form
// fill:
//   021418b0[0] <- 0213c23c  020e1b54/0     02141950[0] <- 0213c21c  020e3210/0
//   021418b0[1] <- 0213c2bc  020e1854/0     02141950[1] <- 0213c20c  020e3078/0
//   02141910[0] <- 0213c204  020e2c08/0     02141950[2] <- 0213c2b4  020e2f78/0
//   02141910[1] <- 0213c244  020e2868/0     02141950[3] <- 0213c1fc  020e2ebc/0
//   02141910[2] <- 0213c22c  020e285c/0     02141950[4] <- 0213c254  020e2eb8/0
//   02141910[3] <- 0213c25c  020e26f8/0
//
// THE ARITIES ARE PER SITE AND THEY DIFFER INSIDE ONE TU. At 020e3078's first
// site the ROM sets r0 to `this + adj>>1` and NOTHING else before `blx r1`, and
// the listing pushes nothing before its `call eax` -- the `push ebx` and
// `push esi` at offsets 0x52 and 0x53 are DEFERRED REGISTER SAVES, matched by
// the `pop esi` / `pop ebx` at 0xbf and 0xc0 after the loop. At its second site
// the ROM emits `mov r1, r7` (the shell counter) before `blx r2` and the
// listing pushes ebx, the same counter, with no caller cleanup. 020e3528 sets
// no r1 and pushes nothing. So 021418b0 and 02141950 take ZERO-argument faces
// and 02141910 takes ONE-argument faces, ELEVEN faces in all.
//
// ONE /alternatename. src/func_ov006_020e3528.cpp wraps its member pointer in
// `struct Entry { PMF pmf[1]; }` at C++ linkage, so MSVC spells the reference
// ?data_ov006_02141950@@3PAUEntry@@A -- read off the object with
// dumpbin /symbols, not guessed. src/func_ov006_020e3078.cpp declares BOTH of
// its tables inside its own extern "C" block and both come in as the plain C
// names the ov006 mount defines, which the UNDEF sweep of that object confirms
// (_data_ov006_021418b0, _data_ov006_02141910, _func_ov006_020e1680 and nothing
// decorated).
//
// SLOT 1 OF 021418b0 IS THE HAND-TRANSCRIBED BODY AND ITS FACE SAYS SO. The
// decomp still has no body and no delink block for 0x020e1854, so the face
// names port_mg_curling_st_020e1854 -- unmatched/MgCurling_State_020e1854.cpp's
// host symbol -- and not the ROM's `func_` name. That is the same ruling the
// switch above already carries, for the same reason: stategen.py's reconstruct
// check fails the build if a `func_*` symbol is invented for a refused address.
// Seating the table does not change the refusal and does not pretend to.
//
// SLOT 2 OF 02141910 IS THE COLLISION RIDE-THROUGH, and its face calls it
// exactly the way the switch did: `func_ov006_020e285c(c, a)` at C linkage,
// with the declaration this file already carries. src/func_ov006_020e285c.c
// declares itself (void) and tail-jumps to func_ov006_020e20bc, so r0 and r1 --
// the two words the face pushes -- ride the veneer's reused frame into the
// transcribed collision body. port/tools/tailjump_guard.py's CLASS C row for
// that pair is unaffected: it asserts that 020e285c's OWN frame is a jmp, which
// is a property of src/func_ov006_020e285c.c and of nothing here. Its note says
// "unmatched/MgCurling_StateDispatch.cpp dispatches this address from a
// one-argument slot and passes (c, a)", which is still true of this file --
// through a cell rather than through a case. That file is not this lane's to
// edit and the wording is left alone.
//
// SLOT 4 OF 02141950 IS THE OTHER RIDE-THROUGH. func_ov006_020e2eb8 sits in a
// ZERO-argument cell and its src takes (void), so its face calls it with
// nothing, which is what the switch did and what the ROM does.
//
// THE WITNESS MOVES WITH THE STATES. g_curling_state_hits is what
// hal/scene_mg.cpp prints through port_mg_curling_state_hits, and it was
// incremented by the two switches. Eleven of this class's twenty-five addresses
// no longer reach a switch, so each face takes the same increment and the
// census keeps counting exactly the dispatches that happen.
//
// THE SWITCHES STAY. port_mg_try_ov006_0 and port_mg_try_ov006_1 are the
// framework's chain entry points -- unmatched/MgBase_StateDispatch.cpp's
// port_mg_call1 chains mg_try_ov004 -> port_mg_try_ov006_1 -> report -- and
// this class's other four tables (021418c0, 021418d8, 021418f0, 02141930) are
// still routed through them by the three host copies that remain in this file.
#pragma comment(linker, "/alternatename:?data_ov006_02141950@@3PAUEntry@@A=_data_ov006_02141950")

#define CUR_FACE0(tag, slot, sym)                                         \
    static void __fastcall cur_##tag##_s##slot(void *self, void *dead_edx)\
    {                                                                     \
        (void)dead_edx;                                                   \
        ++g_curling_state_hits;                                           \
        sym((char *)self);                                                \
    }

#define CUR_FACE0_VOID(tag, slot, sym)                                    \
    static void __fastcall cur_##tag##_s##slot(void *self, void *dead_edx)\
    {                                                                     \
        (void)self; (void)dead_edx;                                       \
        ++g_curling_state_hits;                                           \
        sym();                                                            \
    }

#define CUR_FACE1(tag, slot, sym)                                         \
    static void __fastcall cur_##tag##_s##slot(void *self, void *dead_edx,\
                                               int a)                     \
    {                                                                     \
        (void)dead_edx;                                                   \
        ++g_curling_state_hits;                                           \
        sym((char *)self, a);                                             \
    }

/* data_ov006_021418b0, arity 0 */
CUR_FACE0(b0, 0, func_ov006_020e1b54)
CUR_FACE0(b0, 1, port_mg_curling_st_020e1854)

/* data_ov006_02141910, arity 1 */
CUR_FACE1(t10, 0, func_ov006_020e2c08)
CUR_FACE1(t10, 1, func_ov006_020e2868)
CUR_FACE1(t10, 2, func_ov006_020e285c)
CUR_FACE1(t10, 3, func_ov006_020e26f8)

/* data_ov006_02141950, arity 0. Slot 1 is func_ov006_020e3078, which is the
   matched src TU now and not the host copy this file used to define. */
CUR_FACE0(t50, 0, func_ov006_020e3210)
CUR_FACE0(t50, 1, func_ov006_020e3078)
CUR_FACE0(t50, 2, func_ov006_020e2f78)
CUR_FACE0(t50, 3, func_ov006_020e2ebc)
CUR_FACE0_VOID(t50, 4, func_ov006_020e2eb8)

extern "C" void port_mg_curling_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;

    static const struct {
        MgPmf      *table;
        const char *name;
        unsigned    slot;
        unsigned    rom;
        void       *face;
    } seats[] = {
        {data_ov006_021418b0, "021418b0", 0, 0x020e1b54u, (void *)cur_b0_s0},
        {data_ov006_021418b0, "021418b0", 1, 0x020e1854u, (void *)cur_b0_s1},

        {data_ov006_02141910, "02141910", 0, 0x020e2c08u, (void *)cur_t10_s0},
        {data_ov006_02141910, "02141910", 1, 0x020e2868u, (void *)cur_t10_s1},
        {data_ov006_02141910, "02141910", 2, 0x020e285cu, (void *)cur_t10_s2},
        {data_ov006_02141910, "02141910", 3, 0x020e26f8u, (void *)cur_t10_s3},

        {data_ov006_02141950, "02141950", 0, 0x020e3210u, (void *)cur_t50_s0},
        {data_ov006_02141950, "02141950", 1, 0x020e3078u, (void *)cur_t50_s1},
        {data_ov006_02141950, "02141950", 2, 0x020e2f78u, (void *)cur_t50_s2},
        {data_ov006_02141950, "02141950", 3, 0x020e2ebcu, (void *)cur_t50_s3},
        {data_ov006_02141950, "02141950", 4, 0x020e2eb8u, (void *)cur_t50_s4},
    };

    for (unsigned i = 0; i < sizeof seats / sizeof seats[0]; ++i) {
        MgPmf *p = &seats[i].table[seats[i].slot];
        if (p->code != seats[i].rom || p->adj != 0) {
            std::fprintf(stderr, "FATAL: dScMgCurling_c state table %s slot %u: "
                         "the sinit left %08x/%d, the ROM's own pairs say "
                         "%08x/0 -- WRONG BYTES\n", seats[i].name,
                         seats[i].slot, p->code, p->adj, seats[i].rom);
            std::abort();
        }
        p->code = (unsigned)(size_t)seats[i].face;
    }
}
