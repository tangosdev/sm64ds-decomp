// PORT_HOST_ABI. func_ov006_0211ba88, host-copied. Run mg9, lane BOX.
//
// dScMgSound_c (actor id 0x16f, scene 367). This body is slot 4 of
// data_ov006_02142df8, one of the ten per-entity states the 0x14-stride array
// runs, and it reaches the port THREE ways broken. All three were found by
// disassembling 0x0211ba88 out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0 and reading it against src/func_ov006_0211ba88.c line by line.
//
// THE SRC TU IS BANNERED "Logic verified correct vs ROM" AND IT IS NOT. That
// sentence is the reason this file exists rather than a slice line, and the
// three defects are stated separately because only the first is a port
// problem; the other two are decomp problems this port must not inherit.
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
// ---- WHAT IS AND IS NOT CHANGED IN src/ -----------------------------------
//
// NOTHING. THE DECOMP-SIDE FIX IS ROUTED, NOT TAKEN, which is the ruling
// port/mg_fanout_costs.txt section 10 records for the fabricated ov004
// language table: whether the corrected source still builds byte-identically
// under mwccarm is a byte-gated-tree question and this is not a byte-gated
// tree. src/func_ov006_0211ba88.c is UNTOUCHED and is excluded from
// port/slice_box.txt; this file defines the symbol.

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
   The return value is 0 on every path (`b` to the shared tail or an early
   `pop / bx lr` after `moveq r0,#1 / strbeq r0,[r1]`, where r0 is the stored
   1 rather than a returned one); nothing reads it -- the dispatcher's blx
   discards it -- and src types it int, so it is kept int here. */
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
