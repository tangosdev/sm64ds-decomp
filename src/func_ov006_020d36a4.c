//cpp
// NONMATCHING: one constant-materialisation idiom, size 0x504 vs 0x4fc (two
// instructions). Logic verified correct vs ROM; not byte-matchable from C at
// any of the twenty-five installed mwccarm builds. Counts as decompiled, not
// matched.
//
// THE RESIDUE IS TWO INSTRUCTIONS IN ONE PLACE AND THE CAUSE IS NAMED. In the
// shuffle's swap the ROM holds the array offset 0x4694 in a REGISTER for the
// whole loop and reaches the second element with a pre-indexed writeback:
//     add r1, sb, r8, lsl #2 / add r2, sb, r0, lsl #2 / add r0, r1, r4
//     ldr r3, [r0] / ldr r1, [r2, r4]! / str r1, [r0] / str r3, [r2]
// 0x4694 is not an ARM immediate, so it has to be either a register or a
// split, and mwccarm takes the split -- `add rN, #0x4000` plus a #0x694 load
// offset -- which costs one extra instruction on each of the two sides.
//
// WHAT MOVED THE COUNT, recorded because the two spellings are not
// interchangeable and the next reader should not "tidy" either one: the lane
// permutation A[] is addressed as an INDEXED POINTER and the goal array is
// addressed as a byte-offset expression. Swapping either spelling moves the
// emitted size by eight instructions in the wrong direction (0x4dc with both
// as pointers, 0x528 with both as offsets, 0x520 for the original pair). The
// same split shows in the ROM: it uses scaled indices for the two int arrays
// and post-indexed cursors for the two byte arrays, and
// `#pragma opt_strength_reduction off` fixes the first at the cost of the
// second (0x4c4). Also tried and rejected: opt_common_subs off (0x504, no
// change), opt_propagation off (0x588), and the swap written through explicit
// pointer locals (0x514).
//
// A PER-INSTRUCTION DIFF IS NOT AVAILABLE FOR THIS ROW and that is stated
// rather than left implied: tools/match.py refuses to align two bodies of
// different sizes, so the measurement above is a structural diff of the two
// disassemblies and not the byte gate's own table.
//
// dScMgAmida_c ROUND SETUP: shuffle the four ladder lanes, refuse a board that
// repeats the last one, then initialise the four walkers.
//
// Two callers -- vtable slot 6 (the Behavior, at 0x020d4bd4) and the init tail
// inside func_ov006_020d3ba0 (at 0x020d3da0) -- and it dispatches slot 36, the
// mode==2 predicate, THREE times: once to choose the shuffle policy and twice
// per lane in the tail.
//
// WHAT THE FOUR LANES ARE. The tail writes the walker's start X as
// A[i] * 64 + 32, which is 32, 96, 160 and 224 -- the same four rail
// coordinates src/func_ov006_020d3ba0.c draws its vertical lines at (0x20,
// 0x60, 0xa0, 0xe0). A[] at +0x4694 is therefore the LANE PERMUTATION: which
// rail each of the four contestants starts on. Its inverse is written into
// data_ov006_02141640, which is what the goal row reads.
//
// THE RETRY IS "DO NOT DEAL THE SAME BOARD TWICE". +0x5374 is zero on the
// first round, and the shuffle is accepted unconditionally; on every later
// round the new permutation is compared against the previous one saved in
// data_ov006_02141650 and the shuffle is repeated until something differs.
//
// The three random draws are all the game's standard 15-bit range idiom,
// ((RandomIntInternal(&seed) >> 16) & 0x7fff) * range >> 15, with range 4 and
// range 3 -- the two pool words are 0x00004694 (the array offset) and
// 0x00007fff (the mask), read out of the literal pool at 0x020d3b94/0x020d3b98.

typedef unsigned char u8;
typedef int s32;

struct Obj {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34(); virtual void v35();
    virtual int m_90();
};

extern "C" {
int RandomIntInternal(int *seed);
/* extern, NOT a bare `int`. Run mg9 section 19 records the defect this avoids
   and src/func_ov006_020d3ba0.c is the file it was found in: mwccarm takes a
   bare `int data_0209d4b8;` in an extern "C" block as a TENTATIVE DEFINITION
   and merges it, C++ has no tentative definitions, so MSVC makes it a STRONG
   definition of the arm9 RNG seed that hal/scene_mg_faces.cpp owns inside
   DSSTATE -- two TUs of one class advancing two different seeds. That file
   needed a host copy to escape it. This one does not: it only takes the seed's
   ADDRESS, so the honest spelling and the working one are the same word. */
extern int data_0209d4b8;
s32 data_ov006_02141640[];
s32 data_ov006_02141650[];
char data_ov006_0212e1a8[];
}

#define VCALL90(p)  (((struct Obj *)(void *)(p))->m_90())

/* The game's 15-bit range draw. UNSIGNED throughout: the ROM emits lsr for
   both shifts, and a signed >> would emit asr. */
#define RANGE(r) ((int)((((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fffu) * (unsigned)(r) >> 15))

/* A[] -- the lane permutation, four ints at +0x4694.
   SPELLED AS AN INDEXED POINTER AND GOAL BELOW IS NOT, which is a measurement
   rather than a style choice: the ROM addresses this array with a scaled index
   (`add r0, sb, i, lsl #2`) and the goal array through a walking cursor, and
   swapping either spelling moves the emitted size by eight instructions. */
#define LANE(sb, i)   (((s32 *)((sb) + 0x4694))[i])
/* the goal row each lane maps to, four ints at +0x46a4 */
#define GOAL(sb, i)   (*(s32 *)((sb) + 0x46a4 + (i) * 4))

extern "C" void func_ov006_020d36a4(char *sb)
{
    int i;
    int j;
    int t;
    int again;
    int ok;
    u8 was[4];
    u8 now[4];

    *(u8 *)(sb + 0x53dd) = 0;

    if (VCALL90(sb)) {
        /* MODE 2, "Connect the Characters": a full four-lane shuffle. */
        again = 1;
        for (i = 0; i < 4; i++)
            LANE(sb, i) = i;

        do {
            for (i = 0; i < 4; i++) {
                j = RANGE(4);
                t = LANE(sb, i);
                LANE(sb, i) = LANE(sb, j);
                LANE(sb, j) = t;
            }

            if (*(s32 *)(sb + 0x5374) == 0) {
                /* first round: take whatever came out, and pick the goal row
                   set off the state index */
                again = 0;
                switch (*(s32 *)(sb + 0x53d4)) {
                case 0:
                    GOAL(sb, 0) = 0; GOAL(sb, 1) = 0;
                    GOAL(sb, 2) = 3; GOAL(sb, 3) = 3;
                    break;
                case 1:
                case 3:
                case 5:
                    GOAL(sb, 0) = 0; GOAL(sb, 1) = 0;
                    GOAL(sb, 2) = 1; GOAL(sb, 3) = 3;
                    break;
                default:
                    for (i = 0; i < 4; i++)
                        GOAL(sb, i) = i;
                    break;
                }
                for (i = 0; i < 4; i++)
                    data_ov006_02141640[LANE(sb, i)] = GOAL(sb, i);
            } else {
                /* later rounds: remember the last board, build the new one and
                   keep shuffling until it differs somewhere */
                for (i = 0; i < 4; i++)
                    data_ov006_02141650[i] = data_ov006_02141640[i];
                for (i = 0; i < 4; i++)
                    data_ov006_02141640[LANE(sb, i)] = GOAL(sb, i);
                for (i = 0; i < 4; i++) {
                    if (data_ov006_02141650[i] != data_ov006_02141640[i]) {
                        again = 0;
                        break;
                    }
                }
            }
        } while (again == 1);
    } else if (*(s32 *)(sb + 0x46c8) == 1) {
        /* MODE 0/1, "Mario's Slides", one walker: swap its lane with one
           other. On the first round any of the four will do; afterwards the
           draw excludes lane 0 so the board is guaranteed to move. */
        if (*(s32 *)(sb + 0x5374) == 0) {
            j = RANGE(4);
        } else {
            j = RANGE(3) + 1;
        }
        t = LANE(sb, 0);
        LANE(sb, 0) = LANE(sb, j);
        LANE(sb, j) = t;
    } else {
        /* MODE 0/1 with more than one walker: shuffle every lane, then refuse
           a board whose OCCUPANCY SET matches the previous round's. The two
           four-byte marks below are the occupancy, not the permutation. */
        for (i = 0; i < 4; i++)
            was[i] = 0;
        for (i = 0; i < *(s32 *)(sb + 0x46c8); i++)
            was[LANE(sb, i)] = 1;

        do {
            for (i = 0; i < 4; i++)
                now[i] = 0;
            for (i = 0; i < 4; i++)
                LANE(sb, i) = i;
            for (i = 0; i < 4; i++) {
                j = RANGE(4);
                t = LANE(sb, i);
                LANE(sb, i) = LANE(sb, j);
                LANE(sb, j) = t;
            }
            for (i = 0; i < *(s32 *)(sb + 0x46c8); i++)
                now[LANE(sb, i)] = 1;

            if (*(s32 *)(sb + 0x5374) == 0) {
                ok = 0;
            } else {
                ok = 1;
                for (i = 0; i < 4; i++) {
                    if (now[i] != was[i]) {
                        ok = 0;
                        break;
                    }
                }
            }
        } while (ok == 1);
    }

    /* THE FOUR WALKERS. Start X is the lane's rail, start Y is above the top
       of the board, and the per-lane release delay is zero in mode 2 and
       staggered by the state row's +0x14 field times 60 otherwise. */
    for (i = 0; i < 4; i++) {
        if (VCALL90(sb)) {
            *(s32 *)(sb + 0x46b8 + i * 4) = 0;
        } else {
            *(s32 *)(sb + 0x46b8 + i * 4) =
                i * *(s32 *)(data_ov006_0212e1a8
                             + *(s32 *)(sb + 0x53d4) * 0x1c + 0x14) * 0x3c;
        }

        *(s32 *)(sb + 0x4660 + i * 8) = LANE(sb, i) * 64 + 32;

        if (VCALL90(sb))
            *(s32 *)(sb + 0x4664 + i * 8) = -0xcc;
        else
            *(s32 *)(sb + 0x4664 + i * 8) = -0xd4;

        *(s32 *)(sb + 0x4684 + i * 4) = -1;
        *(u8 *)(sb + 0x4680 + i) = 0;
        *(u8 *)(sb + 0x46b4 + i) = 0;
    }

    *(s32 *)(sb + 0x46cc) = 0;
    *(s32 *)(sb + 0x53e0) = 0;
}
