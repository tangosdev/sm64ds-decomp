/* RETIRED HOST COPY of src/func_ov007_020add3c.c -- SCENE 1'S IMPLICIT r0 ARGUMENT,
 * with the one thing the ROM leaves implicit spelled out: the forward.
 * ov007 0x020add3c, 0xd8 bytes (216, 54 ARM instructions).
 *
 * ============================ WHAT THIS IS ================================
 *
 * A DISPLACEMENT, NOT AN INTERIM. src/func_ov007_020add3c.c is MATCHED and
 * stays matched; it is simply not in the port's ov007 slice any more, and this
 * body links in its place. That costs the port one linked TU and it is the
 * price of the seat, not a regression. The shape was ruled before it was taken
 * -- derived by run link60 lane RT1 in port/ov007_seat.txt section 5c,
 * classified and confirmed by lane SC1N in section 5d, and confirmed again by
 * both of their reviews -- because displacing a matched TU picks which of two
 * matched bodies stops being the decomp, and this file's precedent is that a
 * review rules that and a lane executes it.
 *
 * THE SECOND DISPLACEMENT IN THIS OVERLAY AND THE OPPOSITE FRAME FROM THE
 * FIRST. port/unmatched/Ov007_RideThrough_020be980.cpp answers a MIDDLE frame
 * that was handed two values in r1/r2 and named neither. This one answers a
 * frame that owns its value outright and does not pass it. Same defect class,
 * different frame, and the difference is the whole reason this is a fresh
 * derivation rather than the same fix again.
 *
 * ============================ THE SEAM ====================================
 *
 * TWO matched TUs, not three, and both are right:
 *
 *   src/func_ov007_020add3c.c   extern int func_ov007_020ae558(void);
 *                               func_ov007_020ae558();       <- passes nothing
 *   src/func_ov007_020ae558.c   int func_ov007_020ae558(char *self)
 *
 * On ARM there is nothing to pass. The ROM's caller saves its OWN incoming
 * argument into r4 and never writes r0, so r0 still holds it at the branch:
 *
 *   020ADD3C  push {r4, lr}
 *   020ADD40  mov  r4, r0     <- saves its own argument, does not MOVE it
 *   020ADD44  bl   #0x20ae558 <- r0 still holds add3c's incoming argument
 *   020AE564  ldr  ip, [r0]   <- and the callee reads it
 *
 * mwccarm emits no setup because none is needed, and the matched caller must
 * declare no argument to reproduce that. On MSVC every argument is a stack
 * slot: the caller pushes zero and the callee reads [ebp+8], which is the
 * caller's own saved esi.
 *
 * WHY THE CALLER AND NOT THE CALLEE, which is the only real choice here.
 * THE FRAME WHERE INFORMATION IS DESTROYED IS THIS ONE. The value is add3c's
 * own declared parameter; it exists here, it is wanted one frame down, and
 * this is where it stops being passed. func_ov007_020ae558 declares the
 * parameter it needs and is blameless, and a host copy of the callee could not
 * invent the value -- not as a matter of taste but as a matter of arithmetic.
 * Its one caller runs inside a nine-iteration loop at the tail of
 * src/func_ov007_020aed98.cpp:
 *
 *   for (i = 0; i < 9; i++)
 *       func_ov007_020add3c(*(int*)(gg + i * 4 + 0xa4));
 *
 * so the forwarded value is nine different pointers out of nine different
 * slots in one run. No constant a callee-side host body could supply would be
 * right more than once.
 *
 * AND THE CALLER ALREADY HAS IT ON THE HOST, which is what makes this copy a
 * forward and not a reconstruction. Read off this tree's own walk_window.exe
 * at the map address of func_ov007_020aed98, the loop passes it correctly:
 *
 *   0042AEEB  mov  esi, 0xa4          <- the running byte offset
 *   0042AEF0  mov  eax, [0xc6fda8]       data_ov007_0210342c
 *   0042AEF5  push [esi + eax]        <- THE ARGUMENT, pushed
 *   0042AEF8  call 0x42a310              func_ov007_020add3c
 *   0042AEFD  add  esi, 4
 *   0042AF03  cmp  esi, 0xc8          <- 0xa4 .. 0xc4, nine iterations
 *
 * This body only forwards what its frame is already handed.
 *
 * ============================ THE SIGNATURE ===============================
 *
 * UNCHANGED FROM THE MATCHED SOURCE. `(char *c)` is what src/ declares and
 * what the one call site passes; nothing about this displacement widens it.
 * The only edit to the body is that the call to func_ov007_020ae558 forwards
 * `c`, and the callee's declaration here names the parameter the callee's own
 * matched definition names.
 *
 * THE CALL SITES, RE-DERIVED IN THIS LANE RATHER THAN TAKEN ON THE REVIEWS'
 * WORD, out of config/arm9/overlays/ov007/relocs.txt:
 *
 *   func_ov007_020ae558   ONE reference in the whole config, and it is a call:
 *                         from:0x020add44 kind:arm_call. There is no second
 *                         site at which a different shape could be right.
 *   func_ov007_020add3c   ONE arm_call, from:0x020aef34, which is inside
 *                         func_ov007_020aed98's delink span
 *                         (start:0x020aed98 end:0x020aef78) -- the loop above.
 *
 * TWO ROWS THAT LOOK LIKE EXTRA CALLERS AND ARE NOT. ov002's and ov003's
 * relocation tables each carry a row written FROM 0x020add3c, to 0x02022d80
 * and to 0x0205a448. Those are those overlays' own unrelated calls at an
 * address that overlaps in memory because the overlays share the slot. They
 * are not references to this function. The same trap is named in
 * Ov007_RideThrough_020be980.cpp's header for the other seam, and it is worth
 * naming twice because a grep for the bare address finds them both times.
 *
 * ============================ PROVENANCE ==================================
 *
 * The body below is the ROM's, read off extracted/overlays/overlay_0007.bin
 * with capstone at the config-aligned base 0x020ad660 (the minimum section
 * start in config/arm9/overlays/ov007/delinks.txt), NOT the dsd export. Its
 * delink block runs start:0x020add3c end:0x020ade14 and symbols.txt gives
 * size=0xd8, so the 54 instructions disassembled are the whole function, and
 * every one of them was checked against the matched source in this lane: the
 * strh of the return value through P0+4, the ae834 call on P0, the
 * `cmp r0, #7 / addls pc, pc, r0, lsl #2` eight-arm jump table whose only two
 * non-default arms are case 0 (func_ov007_020ae2d0) and case 3
 * (func_ov007_020ae070), the ae454 call, the three word copies out of P0 at
 * +8, +0xc and +0x20 into P1 at +0, +4 and +8, and the trailing
 * `mov r0, #0x64000 / rsb r0, r0, #0` compare that zeroes P1+8. Nothing in the
 * matched source is unaccounted for and nothing in the ROM is unrepresented.
 *
 * MEASURED ON THIS TREE'S BINARY, NOT ARGUED FROM WHAT MSVC OUGHT TO EMIT, and
 * disassembled FROM THE MAP ADDRESS because a linear walk desynchronises on
 * MSVC's data islands (section 5d's tooling correction). func_ov007_020add3c
 * at 0042A310 shows BOTH shapes of `push` inside one frame, which settles the
 * save-versus-argument question without leaving the function:
 *
 *   0042A310  push ebp
 *   0042A311  mov  ebp, esp
 *   0042A313  push esi           <- a SAVE. Never cleaned.
 *   0042A314  call 0x42a8d0         func_ov007_020ae558, ZERO arguments
 *   0042A319  mov  esi, [ebp+8]  <- reads its OWN argument, AFTER the callee
 *   0042A31C  mov  ecx, [esi]       already read a slot nobody filled
 *   ...
 *   0042A325  push [esi]         <- an ARGUMENT
 *   0042A327  call 0x425d20         func_ov007_020ae834
 *   0042A32E  add  esp, 4        <- and it IS cleaned
 *
 * A save is never cleaned and is followed by the function reading its own
 * frame; an argument is cleaned by the caller, because this is cdecl. The call
 * this file fixes has only the first kind before it.
 *
 * AND THE CALLEE'S HALF, at 0042A8D0:
 *
 *   0042A8DC  mov  ebx, [ebp+8]  <- the parameter nobody pushed
 *   0042A8E6  mov  ecx, [ebx]    <- the fault
 *
 * The fault report at the pre-seat baseline of this lane reads `FAULT code
 * c0000005 at +0x0002a8e6 accessing 000000a4` with `ebx=000000a4
 * esi=000000a4`, and its stack shows the return address into the caller at
 * +0x0002a319. The faulting instruction is a bare `[ebx]` with no
 * displacement, so the accessed address IS the garbage slot's contents and
 * there is no arithmetic to close: 0xa4 is the loop's byte offset on iteration
 * ZERO, so this caller had never completed once. Those offsets are this
 * binary's alone; the map addresses and the git_sha are the currency.
 *
 * The behaviour below is the matched source's, unchanged in every other
 * respect.
 */
/* ======================== RETIRED, AND WHY =================================
 *
 * run link100 wave 15, lane SEAT15E (LINK15 BATCH 4). THIS FILE CARRIES NO
 * BODY ANY MORE. The matched TU is back in the port's ov007 slice as itself:
 * the `continue()` that cut it out of the loop in port/CMakeLists.txt is gone,
 * and so is the branch that appended this file in its place.
 *
 * WHAT WAS RE-READ, against src/ at 8ddff3187, before the cut was removed.
 * The header says the defect is an implicit r0 argument: the matched source
 * declared its callee `(void)` and called it with nothing, because on ARM the
 * caller own incoming argument is still in r0 at the branch.
 * src/func_ov007_020add3c.c today opens
 *
 *     extern int func_ov007_020ae558(char *self);
 *
 * and calls it `int r = func_ov007_020ae558(c);` on its own incoming argument,
 * and src/func_ov007_020ae558.c defines `int func_ov007_020ae558(char *self)`
 * with that one parameter and reads it as `*(int**)self`. Nothing is left in
 * r0 for a stack ABI to miss. Every other statement in the body this file used
 * to carry was the same source line for line -- the strh through P0+4, the
 * ae834 call, the eight-arm switch with its two live arms, the ae454 call, the
 * three word copies into P1 and the -0x64000 compare -- so retiring the copy
 * changes the emitted behaviour in exactly one place, and that place is the
 * argument the source now passes itself.
 *
 * THE MEASURED FAULT THE HEADER RECORDS IS THE SAME FACT FROM THE OTHER SIDE.
 * `FAULT code c0000005 at +0x0002a8e6 accessing 000000a4` was the callee
 * reading [ebp+8] on iteration zero, a slot nobody had written. With the
 * source passing the argument, the push at the call site is what fills it.
 * port/tools/tailjump_guard.py's Class A row 1 asserts the other half in every
 * build from here on: this frame must emit a REAL CALL and never a tail jump,
 * and its `tu` now names the source the build actually compiles, so the row
 * comes back off RETIRED by itself.
 *
 * THE HEADER ABOVE IS KEPT ON PURPOSE: the 54-instruction capstone reading at
 * the config-aligned base 0x020ad660, the eight-arm jump table with its two
 * non-default arms, and the measured save-versus-argument listing at 0042A310
 * that settled the class are readings of the cartridge and of a real binary,
 * and they stay true whichever side of the seam runs.
 */
