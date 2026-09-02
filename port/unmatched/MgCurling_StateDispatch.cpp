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

/* src/func_ov006_020e3528.cpp -- dScMgCurling_c::Behavior, vtable slot 6.
   THE ONE THE COST FILE OPENS WITH. Its src reads
       struct Entry { PMF pmf[1]; };  extern Entry data_ov006_02141950[];
       int j = c->idx;
       (c->*data_ov006_02141950[j].pmf[0])();
   and the struct wrapper is what hid the member pointer from facegen's WALL
   test; see hal/scene_mg_faces.cpp section 2b. `c->idx` is at 0x4eac, which is
   what func_ov006_020e3078 also writes (*(int*)(c + 0x4eac) = 2). */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgCurling_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" int func_ov006_020e3528(void *self)
{
    char *c = (char *)self;
    int j = *(int *)(c + 0x4eac);
    const MgPmf *e = &data_ov006_02141950[j];
    port_mg_call0(c, e->code, e->adj);
    func_ov006_020e12d0(c);
    return 1;
}

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

/* src/func_ov006_020e3078.cpp. SILENT as well, and the only TU in the set with
   BOTH arities. Its src includes types.h for u8/u16; the host copy spells the
   widths directly so this file keeps facegen's "includes nothing but cstdio"
   property. The two `(long long)(int)` casts in the src are the decomp's
   rendering of a sign-extended address computation and are dropped here: they
   are a no-op on a 32-bit host pointer and a truncation hazard on a 64-bit
   one, and this file is compiled for the host, not matched. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgCurling_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020e3078(char *c)
{
    if (*(unsigned short *)(c + 0x4ee2) != 0) {
        unsigned short *q = (unsigned short *)(c + 0x4ee2);
        *q = (unsigned short)(*q - 1);
        return;
    }
    if (*(unsigned char *)(c + 0xc4) == 0) {
        *(unsigned char *)(c + 0xc3) = 1;
        *(unsigned char *)(c + 0xc4) = 1;
        *(unsigned short *)(c + 0xc0) = 0;
    }
    if (*(unsigned char *)(c + 0x4ee9) != 0) {
        unsigned char *q = (unsigned char *)(c + 0x4ee9);
        *q = (unsigned char)(*q - 1);
    }
    {
        const MgPmf *p = &data_ov006_021418b0[*(unsigned char *)(c + 0x4ee4)];
        port_mg_call0(c, p->code, p->adj);
    }

    {
        int count = 0;
        int i = 0;
        char *p = c;
        for (; i < 5; i++, p += 0x2c) {
            if (*(unsigned char *)(p + 0x4689) != 0) {
                *(int *)(p + 0x466c) = *(int *)(p + 0x4660);
                *(int *)(p + 0x4670) = *(int *)(p + 0x4664);
                if (*(unsigned short *)(p + 0x4680) != 0) {
                    unsigned short *q = (unsigned short *)(p + 0x4680);
                    *q = (unsigned short)(*q - 1);
                }
                {
                    const MgPmf *e =
                        &data_ov006_02141910[*(unsigned char *)(p + 0x4688)];
                    port_mg_call1(c, e->code, e->adj, i);
                }
                if (*(unsigned char *)(p + 0x4688) != 2)
                    count++;
            }
        }
        if (count != 0)
            return;
    }
    *(int *)(c + 0x4eac) = 2;
    func_ov006_020e1680(c);
    *(unsigned short *)(c + 0x4ee2) = 0xc0;
}
