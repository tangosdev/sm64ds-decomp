//cpp
// NONMATCHING: register allocation only. Logic verified against the ROM
// instruction for instruction; not byte-matchable from C at mwccarm 2004/b56.
// Counts as decompiled, not matched.
//
// dScMgSlot1_c, vtable slot 6, the Behavior. 0x81c bytes at 0x0210c9e0 in
// overlay 6. This is the whole state machine of Mario Slot (scene 364): a
// FOURTEEN-arm jump-table switch on the state index at self+0x46b4, with a
// per-frame tail every arm falls into and a single exit that returns 1.
// port/slice_s364.txt section 5 is the floor documentation this retires.
//
// CLOSEST APPROACH: SIZE EXACT -- 519 words against the ROM's 519, 0x81c to
// the byte. 431 of those words are byte-identical at their own offset and a
// further 42 are relocation slots which hold a placeholder in the object and
// carry the ROM's word once linked. That is 473 of 519 (91.1%). The 46 residual
// words are ALL register allocation: the same instruction, the same operands,
// a different register number. The dominant one is the prologue's r1/r2 swap --
// the ROM puts `self + 0x4000` in r2 and the state index in r1, this build the
// other way round -- and it propagates into arms 6 and 13, which read that base
// again before the first call clobbers it.
//
// THE RELOCATIONS ARE CHECKED AND EXACT, which is the part a byte count does
// not tell you. The object emits 42 relocations, config/arm9/overlays/ov006/
// relocs.txt records 42 in this range, every offset lines up and every
// destination symbol is the same one the ROM's record names. So all eighteen
// distinct callees, both indirect dispatches and all eleven pooled globals are
// wired to the addresses the ROM wires them to; none of the residue is a wrong
// callee hiding behind a wildcard.
//
// WHAT WAS TRIED, so the next lane does not repeat it. All 25 installed mwccarm
// builds, scored as aligned-identical words out of 519: 2004/b56 is the clear
// winner at 432, the 1.2 family lands at 280-284 and 514-515 words, the whole
// 2.0 family collapses to 496 words / 215, and the dsi family to 474 / 124.
// -O2,p -- the mg7 lead -- is WORSE here, not better: 527 words and 246. -O3,p
// reaches 520 / 401; -O4 and -O2 without ,p collapse to 509 / 291 and
// 516 / 269. All six declaration orders of the three locals are FLAT at 432, so
// the pret regalloc lever does not move this one. Also flat: switching on the
// memory read instead of a local, splitting 0x46b4 into 0x4000 + 0x6b4, an
// unsigned touch index, a separate declaration and assignment for the spin
// counter, and hoisting the vtable word into its own local before the indirect
// call.
//
// THE decomp-permuter DID EARN ITS RUN, which is worth recording because the
// last two heavyweight lanes reported it flat. Seeded with the hand draft at
// score 1245 it reached 1215 and then 1055 inside the first four minutes and
// then did NOT improve on 1055 over the remaining run, which reached iteration
// 8677 before it was stopped -- one continuous process, one "base score" line
// in its log. So the honest headline is "the whole gain arrived early", not
// "eight thousand iterations of hill climbing". The whole difference is one
// respelling: case 9/11 keeps its `r` local for the
// read-modify-write and then uses the EXPRESSION `(state - 9) >> 1` again for
// the three uses after it, so mwcc rematerialises the index instead of keeping
// r live across func_02012718. That is 26 words, from 432 aligned-identical to
// 458 and from 397 positional to 423. Nothing else it found survived review,
// and the one thing that did was re-derived by hand and re-scored through
// tools/match.py rather than trusted out of the permuter's own output dir.
//
// AND THE LAST EIGHT WORDS CAME FROM A HOST BUG, not from a matching idea. The
// two vtable dispatches and the betIcon tail call were first written as plain C
// function-pointer calls. That is 8 words away from the ROM, which reads `this`
// into r0 FIRST and loads the table out of r0 -- and on the host it is a
// crash, because the port's thunks are __fastcall-with-a-dummy and a __cdecl
// indirect call gives them `this` on the stack. Rewriting all three as C++
// virtual calls through shadow structs fixed the crash AND closed the eight
// words, 423 positional to 431. The lesson generalises: on this port a
// function-pointer call into a filled vtable is wrong twice over, and the
// byte gate sees only one of the two.
//
// WHAT DID MOVE IT, and these are the load-bearing parts of the source below:
//
//   * #pragma opt_strength_reduction off. With it on, mwcc turns `i * 4` into an
//     induction variable in three separate loops where the ROM recomputes
//     `self + i*4` inline with a shifted operand -- worth about 7 words and a
//     whole register's worth of allocation pressure. The ROM build DID strength
//     reduce the one loop where it pays, the `i * 0x15` strip walk in case 7, so
//     that walk is spelled here as the pointer it compiles to.
//   * The u64 no-op mask launder L() on three read-modify-write sites
//     (self+0x4705 and self+0x46b8 twice). The ROM materialises an RMW address
//     through a literal-pool constant while every single-use access folds onto
//     the 12-bit offset, and L() is what holds that split
//     (notes/matching-style.md, "RMW base materialization").
//   * ASSOCIATIVITY IS LOAD-BEARING and cost this lane the most time. Three
//     places where the two spellings are the same address and different code:
//     `*(u8 *)(c + i + 0x4702)` groups as `(c + i) + 0x4702` and reproduces the
//     ROM's two adds, where `c[0x4702 + i]` pools the constant first and does
//     not; `*(u8 *)(s + *q + 0x46c0)` folds the constant into the store's own
//     offset where `s[*q + 0x46c0]` hoists `s + 0x46c0` into a second induction
//     variable; and `(u8 *)c + i + 0x46ff` is the ONE spelling of the stop
//     pointer that stops mwcc reusing the loop's other `c + i`, which is worth
//     the last word of the size and 38 identical words on its own.
//   * The touch point is read as two indexed loads, `data_020a0de8[idx*4+2]`
//     and `[idx*4+3]`, not through a base pointer: a base pointer gets hoisted
//     above the press test and loses two words.
//
// THE ONE THING NOT REPRODUCED THAT IS NOT REGALLOC is case 6's test. The ROM
// materialises the boolean -- `cmp r0,#0; moveq r0,#1; movne r0,#0; cmp r0,#0;
// beq` -- where every C spelling tried here folds it to a single `cmp/bne`.
// `!x`, `(x == 0)` into a local, and `(x == 0) == 1` were all tried; the last
// one is three words closer in size and 72 words worse everywhere else, so the
// folded form is what ships. Three words of the residue are that.

typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;

struct Pt { int x; int y; };

extern "C" {
extern u8   DecIfAbove0_Byte(u8 *p);
extern void _ZN5Sound12PlayBank2_2DEj(u32 id);
extern int  Sound_PlayIfNotActive(int a, int b, int c, int d);
extern void func_02012718(int a, int b);
extern void func_02012790(int a);
extern void _ZN5Scene9SetFadersEP15FaderBrightness(void *f);
extern void func_0202ec9c(void *f, int a);
extern void _ZN5Scene14StartSceneFadeEjjt(u32 a, u32 b, u16 t);
extern void func_ov004_020ad79c(int a, int b);
extern void func_ov004_020adb1c(int a);
extern void func_ov004_020ae274(int a);
extern void func_ov006_0210c180(char *c);
extern void func_ov006_0210c1a8(int *o);
extern void func_ov006_0210c218(u8 *p, s16 a, s16 b);
extern void func_ov006_0210c278(u8 *o);
extern void func_ov006_0210c2c0(int *p, int v);
extern void func_ov006_0210c2d4(char *c);
extern int  func_ov006_0210c500(void *p);

extern int  data_ov006_0213e600[];
extern u8   data_ov006_0213e4d8[];
extern struct Pt data_ov006_0213e63c[];
extern s16  data_ov006_0213e654[];
extern s16  data_ov006_0213e656[];
extern s16  data_ov006_0213e4f8[];
extern s16  data_ov006_0213e4fa[];
extern u8   data_020a0e40;
extern u8   data_020a0de8[];
extern u8   data_020a0de9[];
extern int  data_0209f61c;
}  /* extern "C" */


/* THE TWO INDIRECT DISPATCHES ARE C++ VIRTUAL CALLS AND THAT IS NOT A STYLE
   CHOICE. The ROM enters both through the object's own table with `this` in
   r0 -- `mov r0, r4; ldr r2, [r0]; ldr r2, [r2, #0x48]; blx r2` for slot 18
   and `add r0, r4, r0; ldr r1, [r0]; ldr r1, [r1]; blx r1` for the betIcon --
   and a plain C function-pointer call reproduces neither. On the HOST it is
   worse than a byte difference: port/hal/scene_mg_slot1.cpp's thunks are
   __fastcall with the dummy second parameter that makes them __thiscall-
   compatible, so a __cdecl indirect call hands them `this` on the stack and
   they read whatever is in ecx. That crashed inside func_ov006_0210c354 with
   `this` reading self+0x470c -- caught by a scripted-touch run, not by any
   compile-time check. A shadow struct with virtuals is the shape
   src/func_ov006_0210d1fc.cpp already uses over this same object, and
   port/slice_s364.txt section 7's shadow-class test passes for it: the table
   under it is a MOUNTED ROM table that the seat's fill writes by ROM word, so
   the slot indices below are the ROM's own. */
struct Obj {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17();
    virtual void m48(int a);          /* vtable offset 0x48 = slot 18 */
};

struct BetIcon {
    virtual void Update();            /* betIcon_c slot 0, the per-frame tail */
};

/* Every read-modify-write below materialises its address through a pool
   constant in the ROM while every single-use access folds onto the 12-bit
   offset; the no-op 64-bit mask is what holds that split
   (notes/matching-style.md, "RMW base materialization"). */
#define L(a) (((int)(a)) & 0xFFFFFFFFFFFFFFFFULL)

#pragma opt_strength_reduction off

/* THE FOURTEEN ARMS, and the field map they run on. Everything named here is
   read off the ROM; nothing is inferred from the game's title screen.
 *
 *   +0x46a4 + r*4   int   reel r's scroll offset, 20.12, counted DOWN
 *   +0x46ac         int   which is reel 2's, spelled out in arm 13
 *   +0x46b0         int   the spin loop's sound handle
 *   +0x46b4         int   THE STATE INDEX, 0..0xd -- this switch
 *   +0x46b8         int   the round's payout, before the per-coin multiplier
 *   +0x4660               the betIcon_c sub-object; its slot 0 is the tail
 *   +0x4684 +0x4690 +0x469c   three effect objects the tail ticks
 *   +0x46c0 + r*0x15      reel r's 21-entry symbol strip
 *   +0x46ff + r     u8    reel r's stop index into that strip
 *   +0x4702 + r     u8    reel r is still spinning
 *   +0x4705         u8    how many reels have been stopped this round
 *   +0x4708         u8    the speed step, 0..2, raised by func_ov006_0210c500
 *   +0x4709         u8    the winning symbol; indexes the {15,10,10,5,5,20}
 *                         payout table data_ov006_0213e4d8
 *   +0x470a         u8    this round was a win
 *   +0x470b         u8    how many Superstars (symbol 5, the wild) were in it
 *   +0x470c         u8    the arm timer every DecIfAbove0_Byte call counts down
 *
 *   0  arm the three reels, clear the counters, state = 1, timer 60
 *   1  wait, play sound 0x160, state = 2, timer 30
 *   2  wait, state = 3
 *   3  SPINNING. Scroll every armed reel by data_ov006_0213e600[speed + r],
 *      wrap its stop index mod 21 on underflow, keep sound 0x161 alive, and
 *      -- message 565's touch-to-stop rule -- test the touch point against
 *      each reel's 0x40 x 0x40 box around data_ov006_0213e63c[r]. When all
 *      three have stopped and coasted to zero, func_ov006_0210c500 scores the
 *      line, the payout table and the wild bonus are added, the coin counter
 *      is paid, state = 4, timer 60.
 *   4  wait, then fire the win effects and branch: paid -> state 5 timer 100,
 *      nothing -> state 7 timer 60
 *   5  wait, hand the payout to the bet icon, state = 6, timer 60
 *   6  wait for the coin drain at +0x4680, then vtable slot 18 with 4
 *   7  wait; coins left -> vtable slot 18 with 5 (next round), none left ->
 *      blank all three strips to symbol 0xd and state = 9
 *   9  11  drop reel (state - 9) >> 1 to zero, state += 1
 *   10 12  wait, state += 1
 *   13     drop reel 2 to zero, state = 8
 *   8      wait, then fade the scene out
 *
 * The emission order below -- 0..7, then 9/11, 13, 8, 10/12 -- is the ROM's
 * own: it is what makes the jump table's fourteen entries land on the same
 * offsets the ROM's do. */
extern "C" int func_ov006_0210c9e0(char *c)
{
    int i;
    int state;
    u32 n;

    state = *(int *)(c + 0x46b4);
    switch (state) {
    case 0:
        for (i = 0; i < 3; i++) {
            *(u8 *)(c + i + 0x4702) = 1;
        }
        *(u8 *)(c + 0x4705) = 0;
        *(u8 *)(c + 0x470a) = 0;
        *(u8 *)(c + 0x470b) = 0;
        *(int *)(c + 0x46b4) = 1;
        func_ov006_0210c2d4(c + 0x4660);
        if (*(u8 *)(c + 0xc4) == 0) {
            *(u8 *)(c + 0xc3) = 1;
            *(u8 *)(c + 0xc4) = 1;
            *(u16 *)(c + 0xc0) = 0;
        }
        *(u8 *)(c + 0x470c) = 0x3c;
        break;

    case 1:
        if (DecIfAbove0_Byte((u8 *)(c + 0x470c)) == 0) {
            _ZN5Sound12PlayBank2_2DEj(0x160);
            *(u8 *)(c + 0x470c) = 0x1e;
            *(int *)(c + 0x46b4) = 2;
        }
        break;

    case 2:
        if (DecIfAbove0_Byte((u8 *)(c + 0x470c)) == 0) {
            *(int *)(c + 0x46b4) = 3;
        }
        break;

    case 3: {
        int nspin = 0;
        for (i = 0; i < 3; i++) {
            if (*(u8 *)(c + i + 0x4702) == 1 ||
                *(int *)(c + i * 4 + 0x46a4) != 0) {
                int *p = (int *)(c + i * 4 + 0x46a4);
                nspin++;
                *p = *p - data_ov006_0213e600[*(u8 *)(c + 0x4708) + i];
                if (*p < 0) {
                    if (*(u8 *)(c + i + 0x4702) == 1) {
                        u8 *q = (u8 *)c + i + 0x46ff;
                        *q = *q + 1;
                        *q = *q % 0x15;
                        *p = *p + 0x40000;
                    } else {
                        *p = 0;
                    }
                }
            }
        }
        if (nspin != 0) {
            *(int *)(c + 0x46b0) =
                Sound_PlayIfNotActive(*(int *)(c + 0x46b0), 2, 0x161, 0);
        }
        if ((u32)*(u8 *)(c + 0x4705) < 3) {
            int idx = data_020a0e40;
            int hit = 0;
            if (data_020a0de8[idx * 4] != 0) {
                if (data_020a0de9[idx * 4] != 0) {
                    hit = 1;
                }
            }
            if (hit != 0) {
                int tx = data_020a0de8[idx * 4 + 2];
                int ty = data_020a0de8[idx * 4 + 3];
                for (i = 0; i < 3; i++) {
                    if (*(u8 *)(c + i + 0x4702) == 1) {
                        int dx = tx - data_ov006_0213e63c[i].x;
                        int dy = ty - data_ov006_0213e63c[i].y;
                        if (dx <= 0x20 && dx >= -0x20 &&
                            dy <= 0x20 && dy >= -0x20) {
                            u8 *r = (u8 *)L(c + 0x4705);
                            *(u8 *)(c + i + 0x4702) = 0;
                            *r = *r + 1;
                            if (*(u8 *)(c + 0x4671) != 0) {
                                *(u8 *)(c + 0x4671) = 0;
                                func_ov004_020ad79c(*(int *)(c + 0xa8),
                                                    *(int *)(c + 0xb4));
                            }
                            func_02012718(0x162,
                                          data_ov006_0213e63c[i].x << 12);
                            break;
                        }
                    }
                }
            }
        } else {
            int all = 1;
            for (i = 0; i < 3; i++) {
                if (*(int *)(c + i * 4 + 0x46a4) != 0) {
                    all = 0;
                }
            }
            if (all == 1) {
                *(u8 *)(c + 0x470a) = (u8)func_ov006_0210c500(c);
                *(int *)(c + 0x46b8) = 0;
                if (*(u8 *)(c + 0x470a) != 0) {
                    int *w = (int *)L(c + 0x46b8);
                    *w = *w + data_ov006_0213e4d8[*(u8 *)(c + 0x4709)];
                }
                n = *(u8 *)(c + 0x470b);
                if (n != 0) {
                    if (n < 3) {
                        int *w = (int *)L(c + 0x46b8);
                        *w = *w + n * 2;
                    }
                }
                func_ov004_020ad79c(*(int *)(c + 0x46b8) *
                                        *(int *)(c + 0x467c) +
                                        *(int *)(c + 0xa8),
                                    *(int *)(c + 0xb4));
                *(int *)(c + 0x46b4) = 4;
                *(u8 *)(c + 0x470c) = 0x3c;
            }
        }
        break;
    }

    case 4:
        if (DecIfAbove0_Byte((u8 *)(c + 0x470c)) == 0) {
            if (*(u8 *)(c + 0x470a) != 0) {
                int s = *(u8 *)(c + 0x4709);
                func_ov006_0210c218((u8 *)(c + 0x4684),
                                    data_ov006_0213e654[s * 2],
                                    data_ov006_0213e656[s * 2]);
            }
            n = *(u8 *)(c + 0x470b);
            if (n != 0 && n < 3) {
                func_ov006_0210c218((u8 *)(c + 0x4690),
                                    data_ov006_0213e4f8[(n - 1) * 2],
                                    data_ov006_0213e4fa[(n - 1) * 2]);
            }
            if (*(int *)(c + 0x46b8) != 0) {
                *(int *)(c + 0x46b4) = 5;
                *(u8 *)(c + 0x470c) = 0x64;
                func_ov006_0210c180(c + 0x469c);
                func_02012790(0x26);
            } else {
                *(int *)(c + 0x46b4) = 7;
                *(u8 *)(c + 0x470c) = 0x3c;
                func_02012790(0xe);
            }
            *(u8 *)(c + 0xc3) = 0;
        }
        break;

    case 5:
        if (DecIfAbove0_Byte((u8 *)(c + 0x470c)) == 0) {
            func_ov006_0210c2c0((int *)(c + 0x4660), *(int *)(c + 0x46b8));
            *(u8 *)(c + 0x470c) = 0x3c;
            *(int *)(c + 0x46b4) = 6;
        }
        break;

    case 6: {
        int idle = !*(int *)(c + 0x4680);
        if (idle != 0) {
            func_ov004_020adb1c(*(int *)(c + 0xa8));
            if (DecIfAbove0_Byte((u8 *)(c + 0x470c)) == 0) {
                ((Obj *)(void *)c)->m48(4);
            }
        }
        break;
    }

    case 7:
        if (DecIfAbove0_Byte((u8 *)(c + 0x470c)) == 0) {
            if (*(int *)(c + 0xa8) > 0) {
                ((Obj *)(void *)c)->m48(5);
            } else {
                u8 *s = (u8 *)c;
                for (i = 0; i < 3; i++) {
                    int *p = (int *)(c + i * 4 + 0x46a4);
                    u8 *q = (u8 *)c + i + 0x46ff;
                    *p = *p + 0x40000;
                    *q = *q + 1;
                    *q = *q % 0x15;
                    *(u8 *)(s + *q + 0x46c0) = 0xd;
                    s += 0x15;
                }
                *(int *)(c + 0x46b4) = 9;
            }
        }
        break;

    case 9:
    case 11: {
        int r = (state - 9) >> 1;
        int *s = (int *)(c + 0x46a4);
        s[r] = s[r] - 0x2000;
        /* r IS DELIBERATELY NOT REUSED PAST THIS TEST, and that is the
           decomp-permuter's finding rather than a style choice: respelling the
           three uses below as the expression itself, so mwcc rematerialises
           `(state - 9) >> 1` instead of keeping r live across the call, is
           worth 26 words. */
        if (*(int *)(c + r * 4 + 0x46a4) < 0) {
            *(int *)(c + ((state - 9) >> 1) * 4 + 0x46a4) = 0;
            *(u8 *)(c + 0x470c) = 0xa;
            *(int *)(c + 0x46b4) = *(int *)(c + 0x46b4) + 1;
            func_02012718(0x164,
                          data_ov006_0213e63c[(state - 9) >> 1].x << 12);
            if (((state - 9) >> 1) == 0) {
                func_ov004_020ae274(2);
            }
        }
        break;
    }

    case 13: {
        int *p = (int *)(c + 0x46ac);
        *p = *p - 0x2000;
        if (*(int *)(c + 0x46ac) < 0) {
            *(int *)(c + 0x46ac) = 0;
            *(u8 *)(c + 0x470c) = 0x3c;
            *(int *)(c + 0x46b4) = 8;
            func_02012718(0x164, data_ov006_0213e63c[2].x << 12);
        }
        break;
    }

    case 8:
        if (DecIfAbove0_Byte((u8 *)(c + 0x470c)) == 0) {
            _ZN5Scene9SetFadersEP15FaderBrightness(&data_0209f61c);
            func_0202ec9c(&data_0209f61c, 2);
            _ZN5Scene14StartSceneFadeEjjt(5, 0, 0);
        }
        break;

    case 10:
    case 12:
        if (DecIfAbove0_Byte((u8 *)(c + 0x470c)) == 0) {
            *(int *)(c + 0x46b4) = *(int *)(c + 0x46b4) + 1;
        }
        break;
    }

    ((BetIcon *)(void *)(c + 0x4660))->Update();
    func_ov006_0210c278((u8 *)(c + 0x4684));
    func_ov006_0210c278((u8 *)(c + 0x4690));
    func_ov006_0210c1a8((int *)(c + 0x469c));
    return 1;
}
