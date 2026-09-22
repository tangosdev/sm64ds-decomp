/* ==========================================================================
 * RETIRED -- THIS FILE IS NOT IN ANY BUILD. Run link100 wave 15, lane SEAT15D,
 * LINK15 BATCH 3. Both deltas this file lists below are now in src/:
 * src/func_ov004_020ad8b8.c:3 declares `extern int func_ov004_020adc3c(int* c);`
 * and :24 passes `c`, and the TU's own header states the defect in the past
 * tense ("THE ARGUMENT IS THE FRAMEWORK OBJECT AND IT USED TO BE DROPPED
 * HERE"). The matched TU is taken through port/slice_l15mg.txt and the SEAT15D
 * block in port/CMakeLists.txt, and this file's source-list entry there is
 * commented out. The body is deleted so the two copies cannot both be taken;
 * the banner below is kept because the disassembly in it is the derivation.
 *
 * The rest of the file, from here down, is the note as it was written.
 * ==========================================================================
 */

/* PORT_HOST_ABI. func_ov004_020ad8b8, ov004's minigame-framework helper that
 * dScMgFlower_c::InitResources calls on its first statement. Run mg5, lane
 * FLW, under the coordinator's granted displacement ruling.
 *
 * THE THIRD INSTANCE OF THE FAMILY port/mg_fanout_costs.txt SECTION 6 NAMES,
 * and the first of the three found by RUNNING rather than by reading. Section 6
 * records (b) a dropped `this` on slot 7 and (c) a delete with no pointer on
 * slot 5, both ARM argument ride-throughs in matched read-only src, and
 * prescribes for both "a PORT_HOST_ABI host copy in port/unmatched/ that places
 * the argument, leaving src/ and the byte gate alone". This is that, for a
 * third function.
 *
 * WHAT THE ROM DOES, disassembled out of extracted/overlays/overlay_0004.bin at
 * base 0x020ad660 (the shipped image, not a dsd export):
 *
 *     020ad8b8  stmdb sp!,{lr}
 *     020ad8bc  sub   sp,sp,#4
 *     020ad8c0  ldr   r0,[pc,#0x3c]    ; = 0x020beb68, the GLOBAL'S ADDRESS
 *     020ad8c4  ldr   r0,[r0]          ; r0 = the WORD IN IT, a pointer
 *     020ad8c8  cmp   r0,#0
 *     020ad8cc  beq   0x20ad8f4        ; -> mov r0,#0, return
 *     020ad8d0  bl    0x20adc3c        ; r0 IS NEVER TOUCHED BETWEEN THE TWO
 *     020ad8d4  mov   r1,#0
 *     020ad8d8  bl    0x2013580
 *     020ad8dc  cmp   r0,#0
 *     020ad8e0  ldreq r0,[pc,#0x20]    ; = 0x0209b308
 *     020ad8e8  ldreq r0,[r0,#0x14]
 *     020ad8ec  ldm   sp!,{lr} / bx lr
 *
 * and the callee is four instructions:
 *
 *     020adc3c  ldr r0,[r0,#8] / and r0,r0,#0xff00 / lsr r0,r0,#8 / bx lr
 *
 * So func_ov004_020adc3c's argument is THE VALUE OF data_ov004_020beb68 READ AS
 * A POINTER, and the ROM delivers it by the same ldr that made the null test.
 * One register does two jobs, which is free on ARM and unspellable in C.
 *
 * WHAT src CANNOT SAY. src/func_ov004_020ad8b8.c declares
 *
 *     extern int func_ov004_020adc3c(void);
 *
 * and calls it with no argument, while src/func_ov004_020adc3c.c defines
 *
 *     int func_ov004_020adc3c(void *c) { return (*(unsigned int *)((char *)c + 8) & 0xff00) >> 8; }
 *
 * and dereferences c on its first statement. mwccarm accepts the mismatch and
 * the ROM built, so the byte gate has never had an opinion on it -- the same
 * reason section 6's (a), (b) and (c) all survived to be found by the port.
 *
 * WHAT IT COST BEFORE THIS FILE, measured rather than reasoned about. On the
 * host the callee reads [esp+4], which the caller never wrote:
 *
 *     FAULT c0000005 at +0x00049026 accessing 00000009
 *       flw_init -> _ZN13dScMgFlower_c13InitResourcesEv (slot 0) +0xa
 *                -> func_ov004_020ad8b8 +0x10
 *                -> func_ov004_020adc3c +0x6
 *
 * The stack happened to hold 1, and 1 + 8 is the address in the fault line. The
 * global itself was live and valid on that boot (0x307fa008, a real host
 * pointer the ov004 mount's rebasing had produced), so nothing was wrong with
 * the DATA -- only with the delivery.
 *
 * THE DELTA FROM src, line by line, per the unmatched/MgBase_DeclConflict.cpp
 * precedent. The body below is src/func_ov004_020ad8b8.c verbatim except:
 *
 *   1. the declaration of the callee takes the parameter its DEFINITION takes
 *          src:   extern int func_ov004_020adc3c(void);
 *          here:  extern int func_ov004_020adc3c(void *c);
 *   2. the call passes the value the ROM has live in r0 at that instruction
 *          src:   func_ov004_020adc3c()
 *          here:  func_ov004_020adc3c((void *)data_ov004_020beb68)
 *
 * Nothing else. The null guard, func_02013580's second argument of 0, the
 * `r == 0` fallback to data_0209b308[0x14/4] and both return paths are src's,
 * unchanged, and every one of them is confirmed against the disassembly above.
 *
 * THE CAST IS THE ROM'S OWN READING AND NOT A CHOICE. The global is declared
 * `int` by src and holds a pointer; the ROM's `ldr r0,[r0]` produces exactly
 * the same 32 bits this cast produces, and the very next instruction in both
 * the ROM and this file tests those bits against zero. If the global's type is
 * ever corrected on the decomp side, this line follows it and the cast goes.
 *
 * IT DEFINES func_ov004_020ad8b8, so src/func_ov004_020ad8b8.c is OUT of
 * port/slice_flw.txt -- listing both would be an LNK2005. That is the one
 * linked function this ruling costs, and it is why the ruling was the
 * coordinator's to make rather than this lane's.
 */

/* THE BODY LIVES IN src/func_ov004_020ad8b8.c, on port/slice_l15mg.txt. It
   spells DELTA 1 and DELTA 2 itself now: the callee's declaration takes the
   parameter its definition takes, and the call passes the pointer the ROM has
   live in r0 across the bl. src declares the global `int*` rather than casting
   an `int`, which is the same 32 bits and the reading this file's own
   "THE CAST IS THE ROM'S OWN READING" paragraph asked for. */
