// PORT_HOST_ABI. func_ov006_0211ba88, host-copied. Run mg9, lane BOX.
//
// dScMgSound_c (actor id 0x16f, scene 367). This body is slot 4 of
// data_ov006_02142df8, one of the ten per-entity states the 0x14-stride array
// runs, and it reaches the port FOUR ways broken. All four were found by
// disassembling 0x0211ba88 out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0 and reading it against src/func_ov006_0211ba88.c line by line.
//
// THE SRC TU IS BANNERED "Logic verified correct vs ROM" AND IT IS NOT. That
// sentence is the reason this file exists rather than a slice line, and the
// four defects are stated separately because only the first is a port
// problem; the other three are decomp problems this port must not inherit.
//
// THE FOURTH WAS FOUND BY THE run mg9 REVIEWER, not by this lane, and it is
// recorded here rather than only in the review because THIS HEADER IS THE
// ROUTING NOTE the decomp side will read when it comes to correct the banner.
// A routing note that lists three of four defects would send someone to fix
// three of them and re-bless a banner that is still false. It is section 4
// below, and this lane re-derived it from its own disassembly rather than
// taking it on report.
//
// ---- 1. THE RECEIVER IS DROPPED AT A VTABLE SLOT 35 CALL ------------------
//
// src spells the call
//
//     if ((*(int (***)(void))base)[0x8c / 4]() != 0)
//
// -- a raw function-pointer read out of the vtable, typed `int (*)(void)`, so
// MSVC compiles `call dword ptr [reg+8Ch]` with NOTHING in ecx. The ROM at
// 0x0211bab4:
//
//     ldr   r1,[r0]          the vptr, off r0 which is still `this`
//     ldr   r1,[r1,#0x8c]    slot 35   (0x8c / 4 == 35)
//     blx   r1               r0 == this, so the receiver IS passed
//
// hal/scene_mg.cpp's mb_v35 is `static int __fastcall mb_v35(void *s, void *)`
// and forwards s to func_ov004_020ad660, whose matched body is
// `return (r0[2] & 0xff) != 0`. With no receiver it reads whatever is in ecx.
// This is the SAME defect port/mg_fanout_costs.txt section 14 found in mb_v35
// itself and repaired there, arriving from the caller's side this time.
//
// THE FIX IS THE ONE src/func_ov006_021063a0.c ALREADY USES for its own two
// slot-35 reads: a shadow class of thirty-five virtuals with the slot as the
// thirty-sixth, so the compiler emits a real __thiscall and `this` rides ecx.
//
// AND THIS SHADOW CLASS IS THE SAFE KIND, which is not a formality -- run mg6
// lane MEM's shadow-class test is that a shadow over a HOST C++ vtable
// (Model, ModelAnim, BlendModelAnim, built by hal/cxxname_bridge.cpp in MSVC
// order) is wrong and a shadow over the MOUNTED ROM TABLE is right. This
// object's vptr is data_ov006_0213f844, which hal/scene_mg_boombox.cpp fills
// BY ROM WORD and therefore leaves in ROM slot order. Slot 35 is slot 35.
//
// ---- 2. THE MODULO IS APPROXIMATED BY ONE SUBTRACTION ---------------------
//
// The ROM at 0x0211bafc:
//
//     mov r3,r4              n = idx
//     cmp r4,r2 / blt        if (idx >= limit)
//   L: sub r3,r3,r2          n -= limit
//     cmp r3,r2 / bge L      WHILE n >= limit
//
// which is n = idx % limit. src does exactly one subtraction and then guards
// the `n == 0` test with `if (n < limit)`, so for any idx >= 2*limit it takes
// neither branch. THE LIMITS ARE SMALL ENOUGH FOR THAT TO FIRE: the two tables
// are data_ov006_0212ef5c = {4,4,5,5,...} and data_ov006_0212ef6c =
// {3,4,4,4,4,4,5,5}, and the loop that calls this body runs idx 0..9. At limit
// 3 the ROM sends idx 6 and idx 9 down the `n == 0` path and src sends them
// down the scan loop instead; at limit 4, idx 8. The ROM's own loop is
// transcribed below.
//
// ---- 3. A BYTE STORE IS SPELLED AS AN INT STORE, OVER A LIVE NEIGHBOUR ----
//
// src writes
//
//     *(int *)(e + 0x5000 + 0xf4) = 4;
//
// where the ROM at 0x0211bb88 writes `strb lr,[ip,#0xf4]` with lr == 4. The
// entity record is packed: +0x50f4 is the state index and +0x50f5 is THE
// IN-PLAY GATE that func_ov006_0211b954 tests before it dispatches at all. A
// four-byte store at +0x50f4 puts a zero in +0x50f5 and takes that entity out
// of the machine, which is a silent stop rather than a crash. The store below
// is a byte.
//
// ---- 4. THE `n == 0` TEST IS NESTED WHERE THE ROM'S IS NOT ---------------
//
// Found by the run mg9 reviewer. The ROM's `cmp r3,#0` at 0x0211bb14 is the
// BRANCH TARGET of the `blt` at 0x0211bb04, so it runs on BOTH paths -- the
// one that went round the subtraction loop and the one that skipped it:
//
//     0211bafc  mov r3,r4        n = idx
//     0211bb00  cmp r4,r2
//     0211bb04  blt 0x0211bb14   <-- idx < limit jumps STRAIGHT to the test
//     0211bb08  sub r3,r3,r2
//     0211bb0c  cmp r3,r2 / bge 0x0211bb08
//     0211bb14  cmp r3,#0        <-- reached from both
//     0211bb18  moveq r0,#1 / strbeq r0,[r1] / popeq / bxeq lr
//
// src puts that test INSIDE `if (idx >= limit)`, so an entity whose idx is 0
// never reaches it and never gets its flag set, where the ROM sets it. idx 0
// is the FIRST of the ten records the dispatcher walks, so this is not a
// corner: it is the one entity that is wrong on every tick.
//
// The fix is free once the loop from section 2 is written the ROM's way: the
// `if (n == 0)` below sits at the same level the ROM's `cmp r3,#0` does, so
// hoisting it out of the `if (n >= limit)` closes divergences 2 and 4 with
// one shape.
//
// ---- AND A FIFTH THING THAT IS NOT A DIVERGENCE, stated so nobody counts it
//
// The ROM sets no meaningful return value on any path. It leaves r0 = 1 after
// the flag store, r0 = -0x18 after the `popgt` early-out, and
// `base + idx*0x14 + 0x5000` on the fall-through -- three different values,
// none of them computed to be returned. The dispatcher's `blx` discards it,
// src types the function `int` and returns 0, and this file does the same. An
// earlier version of the note below called r0 "the stored 1 rather than a
// returned one", which is wrong as stated -- on ARM it is both -- and is
// corrected here. Nothing reads it, so it is a typing artefact and not a
// fifth defect.
//
// ---- WHAT IS AND IS NOT CHANGED IN src/ -----------------------------------
//
// NOTHING. THE DECOMP-SIDE FIX IS ROUTED, NOT TAKEN, which is the ruling
// port/mg_fanout_costs.txt section 10 records for the fabricated ov004
// language table: whether the corrected source still builds byte-identically
// under mwccarm is a byte-gated-tree question and this is not a byte-gated
// tree. src/func_ov006_0211ba88.c is UNTOUCHED and is excluded from
// port/slice_box.txt; this file defines the symbol.
//
// THE FULL LIST THE DECOMP SIDE OWES, so the banner can be corrected in one
// pass rather than three: (1) pass the receiver at the slot-35 call, (2) make
// the modulo a loop, (3) make the +0x50f4 store a BYTE, (4) hoist the
// `n == 0` test out of the `if (idx >= limit)`. All four, or the banner is
// still false.

extern "C" {
typedef unsigned char u8;
extern int data_ov006_0212ef5c[];
extern int data_ov006_0212ef6c[];
}

/* The mounted ROM table, in ROM slot order. Thirty-five virtuals so that the
   thirty-sixth lands on slot 35, which is func_ov004_020ad660 --
   dScMgBase_c's `return (this[2] & 0xff) != 0`, the packed spawn param's low
   byte. src/func_ov006_021063a0.c is the precedent for this spelling. */
struct MgSoundVt {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34();
    virtual int  m8c();      /* slot 35, at byte offset 0x8c */
};

/* ROM 0x0211ba88, 0x158 bytes, transcribed branch by branch.

   r5 = base, r4 = idx, r6 = idx * 0x14, r7 = &base[0x50e8 + idx*0x14].
   The entity record is 0x14 bytes: +0x50e8 a 20.12 position that this body
   walks down by 0x10000 (== 16.0) per tick, +0x50f4 the state index, +0x50f5
   the in-play gate, +0x50f6 a second flag cleared with it, +0x50f9 the
   "already handled" flag this body sets.
   THE RETURN VALUE IS 0 ON EVERY PATH HERE AND IS NOT THE ROM'S, and the
   paragraph above explains why that is a typing artefact rather than a fifth
   divergence: the ROM leaves three different uncomputed values in r0 and the
   dispatcher's blx discards all of them. src types the function int, so it is
   kept int here.
   PORT_HOST_ABI: four measured defects in the matched TU (sections 1-4
   above): a dropped vtable-slot-35 receiver, an approximated modulo, a byte
   store spelled as an int store over a live neighbour, and a differently
   nested zero test. Host-copied and corrected; not an ABI wall. */
extern "C" int func_ov006_0211ba88(char *base, int idx)
{
    int *cur  = (int *)(base + 0x50e8 + idx * 0x14);
    u8  *flag = (u8 *)(base + 0x50f9 + idx * 0x14);
    int limit;
    int n;
    int i;

    *cur = *cur - 0x10000;

    /* THE SLOT-35 CALL, WITH ITS RECEIVER. See section 1 above. */
    if (((MgSoundVt *)base)->m8c() != 0)
        limit = data_ov006_0212ef5c[*(u8 *)(base + 0x5000 + 0x627)];
    else
        limit = data_ov006_0212ef6c[*(u8 *)(base + 0x5000 + 0x627)];

    if (*flag == 0) {
        /* n = idx % limit, the ROM's repeated-subtraction loop. Section 2. */
        n = idx;
        if (n >= limit) {
            do {
                n -= limit;
            } while (n >= limit);
        }
        if (n == 0) {
            *flag = 1;
            return 0;
        }
        if (n > 0) {
            for (i = 0; i < n; i++) {
                const int j = idx - i - 1;
                char *e = base + j * 0x14;
                if (*(u8 *)(e + 0x5000 + 0xf5) != 0) {
                    if ((*cur - *(int *)(e + 0x5000 + 0xe8)) >> 0xc <= 4) {
                        *flag = 1;
                        /* A BYTE. See section 3 above. */
                        *(u8 *)(e + 0x5000 + 0xf4) = 4;
                    }
                }
            }
        }
    }

    if (*cur >> 0xc > -0x18)
        return 0;

    {
        char *e = base + idx * 0x14 + 0x5000;
        *(u8 *)(e + 0xf5) = 0;
        *(u8 *)(e + 0xf6) = 0;
    }
    return 0;
}
