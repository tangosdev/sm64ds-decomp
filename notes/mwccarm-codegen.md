# mwccarm codegen: lever catalogue

How **mwccarm** (Metrowerks CodeWarrior for ARM / NITRO; here 1.2/sp2p3 and the recovered
2004 build `2004/b56`, flags `-O4,p -enum int -lang c99 -char signed -interworking
-proc arm946e -gccext,on`) turns C/C++ into ARM bytes, and what to try when your bytes
differ from the target. It is meant to be reusable on any other Nintendo DS title built
with the same compiler (Mario Kart DS, etc.); SM64DS function names appear only as
evidence pointers.

This is a **distilled index**. The full dated session log, with probes, tables and dead
ends, is [`archive/mwccarm-codegen-log.md`](archive/mwccarm-codegen-log.md). Each lever
below ends with `Log:` followed by section ids; the id is the leading token of a `##`
heading in the log (`6k` = "## 6k. Callee-saved locals color in REVERSE declaration
order"). `6ay` and `6bs` occur twice in the log. The machine-readable short form is
`notes/levers.jsonl` (`tools/levers.py`).

## 0. How to work

- Compile every candidate and byte-diff it; nothing here predicts the allocator, it only
  tells you which axis to move. Fix the **first** structural divergence before touching
  colours, and read register *roles* (which web owns which register), not the
  divergence count, to order residues. Log: 2, 6b, 6al, 6bg, 6bo.
- Reloc slots (`bl` targets, pc-relative extern loads) are wildcards in a byte oracle, and
  the symbol size includes the trailing literal pool: find the epilogue by scanning, not
  by index. Count pool words on both sides before blaming pool growth. Log: 1, 6ca.
- Most misses are not "register allocation": categorizers mislabel structural misses as
  coloring. Re-triage a "regperm" label before parking it. Log: 7a, 7b, 7c, 6ac.
- A "floor" claim is only as good as the probe behind it. A probe seeded from the draft
  can only re-derive the draft; a floor often falls to a different model or a different
  search order. Log: 6af, 6bf, 6bg, 6dc, 6dd, 6cm (superseded).
- First check the **source semantics** (callers, sibling twins, callee *definitions*,
  dropped arguments) before any codegen lever. Log: 6ad, 6cy, 6ce (addendum 2), 6cx.
- A near-miss can be a measurement bug: truncated target bytes, a split-symbol fragment,
  or the verifier using different compiler flags than the real build. Log: 6z (tooling
  bug), 9a, 6co.

## 1. Symptom -> lever quick table

| What the diff shows | Try first | Section |
|---|---|---|
| Whole function a consistent register rename | decl order (reverse for callee-saved), statement order of first use, type of the named local | 3.1 |
| One word short, a missing `mov rX,rY` | dead-assignment elimination probe | 3.9 |
| One word long, an extra `add rX,base,#off` before `[rX]` | member RMW materialises on b56; vary one lvalue spelling | 2.2 |
| ROM has `add rX,base,#off; ldr [rX]`, you fold | u64-mask launder, or encoding-forced/pointer-as-value | 2.1 |
| Pool load where you emit `mov #imm` | usually literals-as-bare in loop body, or a compiler-build delta | 2.5, 5.2 |
| Loads batched, ROM interleaves (or reverse) | struct-copy vs field-wise, fake data dependency, volatile pairing | 2.4, 4.2 |
| Predicated `movcc` vs real branch (or reverse) | override form vs ternary, cond-opt refusal rule | 4.3 |
| Stack frame size off by a few words | frame-layout order, C++ declared-dtor locals, by-value class params | 3.6, 6.2 |
| Prologue loads vptr before homing `this` | real C++ virtual call, not a C function-pointer cast | 6.1 |
| `tst` vs `ands`, no recycled dead register, `ldm` shapes | compiler-build split; stop and check the build | 5 |
| Entire TU has a frame idiom nothing in your corpus emits | prebuilt-library TU or other compiler build | 5.3 |

## 2. Addressing, materialisation and memory shape

**2.1 Materialised base + zero-offset access.** Two natural triggers: halfword/byte
accesses cannot encode offsets >= 0x100, and a pointer used as a *value* (passed to a call,
`this` of a sub-object call). Otherwise the compiler folds the offset into the addressing
mode. To force materialisation, pass the address through 64-bit arithmetic:
`*(int *)(((int)base + OFF) & 0xFFFFFFFFFFFFFFFF) |= x`. Caveats: a laundered address is
LICM-hoisted out of loops (pair with `#pragma opt_loop_invariants off`), and it value-numbers
into one callee-saved register across the whole function (pair with
`opt_common_subs off`). Launder signedness `(int)` vs `(unsigned)` gives distinct value
numbers; vary the spelling per block to stop cross-call CSE; a comma-operator form shares one
register between two pooled pointers; laundering a pooled global address also fixes its
register *class*. Inverse form: launder or demote the plain read so the RMW chain leads an
interleave. Log: 6e, 6g, 6h, 6m, 6w, 6ac, 6ak, 6al, 6ax, 6z (items 1, 6).

**2.2 First-access-fold is build-dependent.** Older builds fold the member RMW always;
`2004/b56` materialises by default when the two sides of `p->f = p->f OP x` are spelled
*identically*, and folds when they differ textually (a cv-qualified cast is its own CSE
class). Identify which regime your target build is in before choosing launder vs un-launder.
Log: 6ag, 6ca, 6bk.

**2.3 Aliasing and declarations.** Declare a global as a plain pointer (not an array) when
the ROM reloads it after a store; an array lets the compiler prove the base immutable and
CSE the load. `volatile` on the *object* plus an inline access can beat a named pointer, and
a partially-volatile struct keeps stores alive without the volatile-object schedule.
Mixing two ways of naming one object (members and `char *c` + literal offsets) changes
whether a scaled index folds into `add r0,r0,r2,lsl #2`; name the object one way. Log: 6ak,
6au, 6cm, 6dc.

**2.4 Struct and aggregate copies.** `v = *(T *)p` is a blind word move in C but
scalarises to member types in C++. Copy through a one-member-array struct at the copy site
(`*(Words *)&dst = *(Words *)src`) to restore both width *and* order; retyping the
destination is the weaker variant. Whole-struct assignment interleaves load/store; field-wise
batches loads then stores; pick whichever the target shows. A fake dependency
(`b ? a : a`) pins load batching. Log: 6ay (first), 6e, 6g, 6m.

**2.5 Bare literals vs a hoisted constant block.** A block of `mov rN,#imm` in the prologue is
the compiler hoisting bare literals used in the loop body; remove named constants and let it
hoist. Large offsets: write raw offsets off the original base, do not pre-offset the pointer.
Log: 6bh, 6e.

**2.7 Leaf and type idioms.** Bitfield extract needs an *unsigned* type (`lsr`; signed gives
`asr`). A signed byte store of -1 emits `mvn`, an unsigned one `mov #0xff`. Plain `char` is signed
under `-char signed`. Write a shift chain as the cast it came from, not as explicit shifts. A
repeated global is one pool word. Angle-to-table indexing is a double cast then shift. Magic
divisors and byte-macro idioms (READ8/WRITE8 style printf cores) have recurring spellings.
What keeps the address of a *stack local* from being re-materialised at every use is source
shape, not `opt_propagation off`. Log: 3, 4, 6, 6l, 6n, 6s, 6ck.

**2.6 `(char *)&member` on a marker in a far-out derived part** goes through the literal
pool; real members avoid it. Log: 6bi.

## 3. Register allocation (callee-saved and scratch webs)

**3.1 Decl order and rank.** Callee-saved locals that live across calls are handed registers
in *reverse* declaration order (last-declared takes the lowest, r7; the spill victim is last).
Write the declaration list in the register order you want. Rank also follows: the **type** of
a named local (`long` vs `int`, `u32` vs byte, signed vs unsigned, array-element type), the
**declaration scope depth** (shallower = higher-numbered register), **first-assignment** order,
variable *identity* (swap which name holds which value), and the *declaration line* when
declarations carry no initialiser (an uninitialised-decl block then assignment block
separates colour order from evaluation order). A fresh counter per loop avoids a reused counter
inheriting the first loop's colour. When searching, climb by pairwise transposition, not random
shuffles. Log: 2, 6k, 6p, 6q, 6aj, 6an, 6ar, 6at, 6bf, 6cc, 6cb, 6y, 6z (item 2).

**3.2 Priority boosters.** A zero-instruction fake use (`h = w ? h : h;`) raises a web's
priority on callee-saved pairs; on caller-saved r2/r3 pairs the booster direction is inverted
and it must ride the base. `volatile` reads can pin load order and flip a binding but only for
memory-sourced webs. Log: 6y, 6aq, 6v, 6bg.

**3.3 Expression and statement shape.** `i++` vs `i = i + 1`, `ok++` vs `ok = 1`, shift form
(`(short)x << 12` vs `(x << 16) >> 4`), a named partial sum with nested ifs, the operand order of
`+` or `*`, `a - (-b)` where `a + b` re-canonicalises, a `u16` parameter (free narrowing), 64-bit
operand locals ordered by assignment, ternary orientation, and a `char *` parameter that stops a
scheduler hoist are all colouring levers because they change the IR that is coloured, not just
the text. Reordering the statements that first *demand* a temp (first-demanded gets the
highest register) is the second lever after declaration order. Log: 6c, 6e, 6aw, 6bd, 6bn,
6bs (ternary), 6cb, 6cg, 6dc, 6z.

**3.4 Dead and extra defs.** A dead assignment in a loop preamble or the entry block extends a
live range into the allocator's first block and rotates a later loop's colouring; a value
assigned to a variable the optimiser will not propagate gets its own colour (a "receiver"
local). Use these as handles when nothing else moves the web. They are bounded (three stores,
a handful of axes). Log: 6cd, 6ct, 6cu, 6cv, 6bs addenda.

**3.5 Callee declaration and call shape.** A callee's declared signature is an allocator
input: a method called with its `this` keeps the incoming r0 live through the entry block.
A vcall spelling changes homing rank. Two loops that walk different tables can share one
cursor variable. Log: 6cx, 6bc, 6by, 6cy.

**3.6 Frame layout order** (b56): named scalars in declaration order, compiler temps,
address-taken aggregates, volatiles into the trailing hole. A memory-homed local always sorts
above spilled scalars. A scalar stack parameter is register-homed in parameter order. Log:
6al, 6ap, 6bj, 6bl, 6da.

**3.8 Whole-function residues and tactics.** Rank-pin families (a pack/texture-param style
function with two attractors), pool-pointer rotations cracked by giving the pointer a *name*
(birth order), the entry write order deciding the entry register pair, a flag held across calls
being rematerialised, and measured lever weights on the largest functions. These are mostly
negative or partial results worth reading before re-attempting the same shape. Log: 6am, 6ao,
6ba, 6bb, 6bm, 6bu, 6bv, 6bw, 6cz.

**3.7 Colour-list facts for b56:** main-line call results get r8, r7, r6, r5, fp, sl, sb in
declaration order. Callee-saved rank is filled in two passes and the address-constant class
rotates. Log: 6cw, 6at.

## 4. Control flow, predication and scheduling

**4.1 Derive control flow from branch targets**, not from how a guard looks. Bottom test =
do/while; `while (p && cond)` for entry-guard + back-edge traversals; a same-variable else-if
chain that loses skip branches should become a `switch` with a `break` per case; switch case
block order follows the source *label order* and a dense jump table needs the low cases
present. A switch body schedules differently from a goto chain. Break out of nested switches
to reach the shared epilogue. Log: 6b, 6e, 6ae.

**4.2 Ordering residues.** Distinct levers: reassign-before-call blocks copy propagation;
`volatile` on both members of a load/store pair pins source order; moving `volatile` captures
before a multiply rotates the mul destination; inverse-RMW launder (2.1); statement order
rotates the mul-dest free list; a store-order rank lever. A block-scoped temp flips a virtual-call-result loop swap; array
subscript indexing defeats a LICM index*scale hoist under EBB-local CSE. Several ordering shapes are true
floors (conditional-cast shift pair vs load-hoist; store EMISSION order). Log: 6d, 6e, 6i, 6j, 6o, 6v,
6z, 6bd, 6be, 6cl, 6cs.

**4.3 Booleans and predication.** A materialised bool (`cmp; moveq 1; movne 0; cmp; beq`) means
the value was stored in a named int, then tested (`int t = (f == k); if (t != false)`), and
inside a condition chain it implies the TU is **C++** (C folds it under every spelling). A
predicated select `mov r,A; movne r,B` comes from the default-then-override statement, not a
ternary. cond-opt refusal: it will not predicate an arm whose jumped-over region contains
another conditional region (a branch or predicated pair); fold budget is ~7 instructions, not
5. Inverting an if-arm's condition defeats full if-conversion. A guard with an early
`return` is plain C (it emits predicated `ldmeq`/`bxeq`), not asm. Log: 6c, 6t, 6u, 6al, 6cn,
6z (item 5), 7b.

**4.4 Optimiser pragmas** are per *function*, so an inlined helper cannot carry its own.
Real and useful: `opt_strength_reduction off`, `opt_common_subs off` (can also regress),
`opt_loop_invariants off` (not inert on larger functions), `opt_propagation off` (a blunt
instrument: it can rotate the whole function; prefer `opt_foldconstants off` or fixing the
source shape, and remove it when a draft inherited one), `opt_dead_assignments off` (probe),
`optimize_for_size on` (cracks early-exit epilogue duplication). A pragma crutch can *rotate*
colouring, so try removing it and respelling naturally. A matched twin with the pragma removed
is the cheapest oracle. Log: 6e, 6f, 6r, 6z, 6aa, 6ab, 6cf, 6cj, 6ch, 6ci, 6dh.

## 5. Compiler-build deltas (stop grinding, change the build)

**5.1 The ROM's compiler is a specific build.** For this era: CodeWarrior for NITRO V0.5-V0.6.1,
reachable via Metrowerks' public ARM ISA Edition build 0056 (October 2004). Some residues exist
in every other build. Before a long grind, compile the probe under every installed build. Log:
6ah (also the SDK's own build flags), 6ai, 6bn (addendum 2).

**5.2 Known build splits:** `tst` vs `ands` for masked branches; the ROM's compiler does not
recycle a register that died at the previous instruction (every other build does; one residue
class); first-access-fold regime (2.2); the outgoing-arg phi coalesce; cond-opt details; a
write into the register of an operand dying at that very instruction. A few MSL runtime entries
are named by the compiler itself (size-0 alias rows stop being link definitions once carved
out). Log: 6bz, 6br, 6bs (second),
6cq, 6av, 6ag.

**5.3 Prebuilt-library TUs.** Some ROM objects (MSL printf pieces, PTMF wrappers) stage by-value
aggregates below `sp` without adjusting it; every standard build emits an fp-pinned dynamic
frame. This fingerprint means a different compiler built that TU; recognise it and use the
other build rather than looking for a lever. Log: 9, 6ak (second and third instance), 6ai.

**5.4 `ldm`/`stm` come only from equal-width block moves.** A 3-in/4-out burst is not compiler
output. Log: 6bt.

**5.5 Escape hatches.** Hand-written SDK assembly primitives match as inline `asm` functions
(and the assembler does accept `ldm{cond}ia`, contrary to an early claim); guard functions
with early returns do not need it. Log: 8, 6t, 7b.

## 6. C++ specifics

**6.1 Compile as C++ when the ROM does C++ things.** Real virtual dispatch emits the
pre-indexed `ldr rV,[r0,#OFF]!` and loads the vptr before homing `this`; never emulate a vtable
call with a C function-pointer cast. Use a dummy-slot vtable to hit the right slot. PMF
dispatch has a fixed ABI prologue; PMF argument liveness (declare the PMF with its int
parameter and forward the index) pins colouring. Log: 5, 6e, 6bc.

**6.2 By-value class parameters are homed to the stack** (`push {r0-r3}`, +0x14 bytes in the
studied case): a by-value class or template (a fixed-point wrapper) as a parameter costs the
same in both builds and no spelling fixes it, but a **scalar** parameter of the same width
stays in registers, and the *caller* pays nothing once the parameter is scalar. The "wall" is
about class-typed parameters only; compiler-numbered local labels (`@NNN`) count *named*
parameters in class declarations, so changing a declaration can renumber symbols. Log: 6az
(and its BATCH, MEASURED, CORRECTED subsections).

**6.3 Locals with a declared destructor** get a stack home where plain C gives an unused local
none (frame-size lever). Placement `new` always null-tests in b56: construct what the object
really is (member or block-scope local) instead of calling the mangled C1 symbol. A hand-written
`__register_global_object` is a function-local static the source hasn't declared; mwcc emits the
guard, stores, registration and `__sinit_` itself. Log: 6cr, 6de, 6df, 6dg.

**6.4 try/catch** compiles from `//cpp` and the 0x0207xxxx-style runtime "functions" can be
split-symbol fragments. Log: 9a.

## 7. Tooling traps

- mwccarm **silently ignores unknown pragmas and unknown `-opt` keywords**, so a typo reads as
  "inert". Use the screened pragma vocabulary (`notes/mwccarm-pragmas.txt`). `long_calls` is not
  an mwccarm pragma; `-w illpragmas` is honoured only for pragma *arguments* on b56. Log: 6as,
  6br, 6bx, 6db.
- Match the **real build flags** in the verifier (`-Cpp_exceptions off`); a verifier compiled
  C++ with exceptions on and manufactured a "7-word floor". Log: 6co.
- Permuter on a C++ near-miss: set the language, keep the unmangled name, regenerate relocs,
  check the C port's frame first; it is the wrong tool for a size-exact schedule tail. Beware
  fake matches and unmeasured scores (two scores can rank a candidate oppositely). Log: 6cp,
  6cr (addendum), 6bn (addendum 4), 6bx.
- Differential-execution audits hide bugs unless you make the control fail first. Log: 6bp
  (addendum), 6bq (addendum).

## 8. Section-id index

Every id is the leading token of a `##` heading in [the log](archive/mwccarm-codegen-log.md); `6ay` and `6bs` occur twice. Lever column = the numbered paragraph above that cites it (ids may be cited as evidence by several).

| Log id | Log heading (abridged) | Lever |
|---|---|---|
| 6ay | The C++ struct-copy lever reaches load ORDER, not just load width | 2.4 |
| 6az | Class-typed by-value parameters are homed to the stack (the Fix12 w... | 6.2 |
| 1 | Ground rules of the build | 0 |
| 2 | The register-allocation wall (the dominant blocker) | 0, 3.1 |
| 3 | Leaf patterns (no calls) | 2.7 |
| 4 | Call / reloc patterns | 2.7 |
| 5 | C++ patterns - compile as C++, not C | 6.1 |
| 6 | Idiom families worth knowing (high-count in the game) | 2.7 |
| 6b | Logic is necessary but NOT sufficient -- match the codegen SHAPE | 0, 4.1 |
| 6c | Boolean materialization & predicated-select shapes | 3.3, 4.3 |
| 6d | Floor taxonomy additions (refine batch, 2026-07-01) | 4.2 |
| 6e | Fable-discovered levers (2026-07-01) - walls that turned out to be... | 2.1, 2.4, 2.5, 3.3, 4.1, 4.2, 4.4, 6.1 |
| 6f | The pragma space, exhaustively characterized (2026-07-01) | 4.4 |
| 6g | The materialization "floor", precisely bounded (2026-07-01 corpus s... | 2.1, 2.4 |
| 6h | Laundering a pooled global's ADDRESS fixes its register CLASS, not... | 2.1 |
| 6i | Block-scoped temp flips a virtual-call-result loop coloring swap (2... | 4.2 |
| 6j | Array-subscript indexing defeats a LICM index*scale hoist under EBB... | 4.2 |
| 6k | Callee-saved locals color in REVERSE declaration order (2026-07-10) | 3.1 |
| 6l | Store-type and magic-divisor levers (2026-07-11 coddog batch 1) | 2.7 |
| 6m | u64-laundering the DST deref flips the aggregate-copy writeback-tem... | 2.1, 2.4 |
| 6n | ov092 near-miss session levers (2026-07-11) | 2.7 |
| 6o | Ordering-residual triage: five levers, four true floors (2026-07-11) | 4.2 |
| 6p | A loop counter reused across loops inherits its prior loop's regist... | 3.1 |
| 6q | Register-web priority follows VARIABLE IDENTITY, not declaration/de... | 3.1 |
| 6r | `#pragma opt_propagation off` keeps a 0/1 ternary selector as a sta... | 4.4 |
| 8 | The `asm`-block escape hatch (for hand-written-asm SDK primitives) | 5.5 |
| 6s | The arm9 printf/soft-double cluster: READ8/WRITE8 macro idioms (202... | 2.7 |
| 6t | Guard functions with early returns are PLAIN C, not asm-hatch (2026... | 4.3, 5.5 |
| 6u | Backend cond-opt: the exact predicate-vs-branch rule (2026-07-16, F... | 4.3 |
| 6v | Four levers from the 2026-07-16 Fable batch (22 matches, arm9 + ov0... | 3.2, 4.2 |
| 6w | Comma-operator shares one register between two pooled pointers (202... | 2.1 |
| 6y | Three levers for steering a callee-saved register WEB (2026-07-20,... | 3.1, 3.2 |
| 6z | Seven levers from the 2026-07-21 Opus→Fable refine cascades (25 mat... | 0, 2.1, 3.1, 3.3, 4.2, 4.3, 4.4 |
| 9 | Prebuilt-library TUs: the ROM contains objects the canonical compil... | 5.3 |
| 7a | The arm9 near-miss head: "register allocation" is a MISLABEL (2026-... | 0 |
| 7b | Second arm9 head cascade (2026-07-22, 4 matches) + an amendment to... | 0, 4.3, 5.5 |
| 7c | Third arm9 head cascade (2026-07-22, 7 matches) -- the head's deep... | 0 |
| 6aa | The pragma crutch ROTATES callee-saved coloring (func_02033464, div... | 4.4 |
| 6ab | Coloring-wall fleet levers (2026-07-25/26, 24 matches from the pure... | 4.4 |
| 6ac | Opus groups-of-5 batch (2026-07-26, 9 arm9 matches): the "s64 famil... | 0, 2.1 |
| 6ad | Opus batch 2 (2026-07-26, 7 arm9 matches incl. func_02048234): sibl... | 0 |
| 6ae | Opus batch 3 (2026-07-26, 9 arm9 matches): constructs, not registers | 4.1 |
| 6af | Opus batch 4 (2026-07-27, 4 arm9 matches, deep-pile breakthroughs) | 0 |
| 6ag | First-access-fold is a COMPILER-BUILD delta, not a source construct... | 2.2, 5.2 |
| 6ah | The build that compiled the ROM: CodeWarrior for NITRO V0.5-V0.6.1... | 5.1 |
| 6ai | RECOVERED: a 2004 mwccarm (build 0056), and the wall falls (2026-07... | 5.1, 5.3 |
| 6aj | Uninit-decl split: vreg birth order follows the DECLARATION LINE, n... | 3.1 |
| 6ak | Batch levers from the 2026-07-27 overlay batches (32+ matches, ov00... | 2.1, 2.3, 5.3 |
| 6al | arm9 endgame batch 3 (2026-07-28): the TU language, the local's TYP... | 0, 2.1, 3.6, 4.3 |
| 6am | The two biggest arm9 functions: measured lever weights (2026-07-30,... | 3.8 |
| 9a | CW try/catch EH IS reproducible from //cpp, and the 0x0207xxxx runt... | 0, 6.4 |
| 6an | Signedness is a coloring lever separable from condition codes (2026... | 3.1 |
| 6ao | Flag-across-call rematerialization, and the for(;;) dead epilogue (... | 3.8 |
| 6ap | Volatile-store/plain-read split: place a scalar in the volatile hol... | 3.6 |
| 6aq | Booster direction on CALLER-saved pairs is inverted, and it must ri... | 3.2 |
| 6ar | A LICM-hoisted address local colors by its DECLARED TYPE, not by or... | 3.1 |
| 6as | TOOLING TRAP: mwccarm silently ignores unknown pragmas, so a typo r... | 7 |
| 6at | Callee-saved rank is filled in TWO passes, and the address-constant... | 3.1, 3.7 |
| 6au | volatile-on-the-OBJECT plus an inline access beats a named pointer... | 2.3 |
| 6av | The outgoing-arg phi coalesce: a build delta no register-resident c... | 5.2 |
| 6aw | The sub-identity beats commutative canonicalization: `a - (-b)` whe... | 3.3 |
| 6ax | Inverse RMW-launder: demote the PLAIN read to let the RMW chain lea... | 2.1 |
| 6ba | The 4a8-pack rank-pin: a partial decouple, and a canonicalization s... | 3.8 |
| 6bb | 4a8-pack rank-pin, second full-angle sweep: the phase-order boundar... | 3.8 |
| 6bc | The vcall SPELLING is a callee-saved homing-rank lever (daPly_c::Ca... | 3.5, 6.1 |
| 6bd | Two store-side levers from the div 6/7 pair (cannon lid + question-... | 3.3, 4.2 |
| 6be | FLOOR: the conditional-cast shift-pair split vs load-hoist collisio... | 4.2 |
| 6bf | Pairwise-transposition climbing beats random shuffles on decl-order... | 0, 3.1 |
| 6bg | Measure the register IDENTITY, not the divergence count, before cla... | 0, 3.2 |
| 6bh | A hoisted constant block in the prologue is evidence of BARE LITERA... | 2.5 |
| 6bi | `(char *)&member` on a `u8` marker in the DERIVED part goes through... | 2.6 |
| 6bj | The pool's regions, park membership, and the DESTINATION of a free... | 3.6 |
| 6bk | Four transferable results from the same campaign | 2.2 |
| 6bl | A memory-homed local always sorts ABOVE every spilled scalar, and t... | 3.6 |
| 6bm | Named-web birth order cracks a pool-pointer rotation, and a two-sou... | 3.8 |
| 6bn | The `<<9` coalesce is a WHOLE-BLOCK flip driven by ONE component, a... | 3.3, 5.1, 7 |
| 6bo | A pragma x source-shape PRODUCT moves a web that neither factor mov... | 0 |
| 6bq | Three homing levers and a store-order rank lever (ov074 daKuriKing_... | 7 |
| 6bp | A named local holding an ADDRESS outranks the compiler's own addres... | 7 |
| 6br | `tst` is a compiler-VERSION split, `long_calls` is not an mwccarm p... | 5.2, 7 |
| 6bs | The ROM's compiler does not recycle a register that just died; ever... | 3.3, 3.4, 5.2 |
| 6bt | mwccarm's only ldm/stm path is the equal-width block move, so a 3-i... | 5.4 |
| 6bu | Eight colouring levers from the ov006 third wave (four matched, run... | 3.8 |
| 6bv | Nine levers from run m100: the sense of the test, the stride in the... | 3.8 |
| 6bw | The entry write order decides the entry register pair, and one scor... | 3.8 |
| 6bx | Two scores that rank the same candidate oppositely, three pragma na... | 7 |
| 6by | Two loops that walk different tables share ONE cursor variable, and... | 3.5 |
| 6bz | Four MSL runtime entries are named by the COMPILER, not by us, and... | 5.2 |
| 6ca | Under 2004/b56 the member RMW materialises by DEFAULT, and the fold... | 0, 2.2 |
| 6cc | `long` and `int` are the same type and colour differently: declarat... | 3.1 |
| 6cd | Dead assignments in a loop's preamble extend live ranges into the a... | 3.4 |
| 6ce | A launder on a pool address picks which of two entry-block attracto... | 0 |
| 6cb | Block DEPTH of a named web is a rank lever that declaration ORDER i... | 3.1, 3.3 |
| 6cf | A MATCHED twin with its pragma removed is the cheapest oracle for a... | 4.4 |
| 6cg | A vector built from three member reads: fold the added constant int... | 3.3 |
| 6ch | A near-miss that is SHORT by exactly one register-to-register copy... | 4.4 |
| 6ci | A `#pragma opt_common_subs off` carried by a near-miss draft can be... | 4.4 |
| 6cj | `opt_propagation off` is a BLUNT instrument: it buys one folded sta... | 4.4 |
| 6ck | What stops mwccarm re-materialising the ADDRESS OF A STACK LOCAL at... | 2.7 |
| 6cl | Three floors re-measured with the wave-9 levers, and the two compil... | 4.2 |
| 6cm | There are FOUR cells for a scaled index, not three, and 2004/b56 ca... | 0, 2.3 |
| 6cn | cond-opt's real refusal rule: a conditional region INSIDE the jumpe... | 4.3 |
| 6co | The verification chain compiled C++ with exceptions ON and the buil... | 0, 7 |
| 6cp | Running the permuter on a C++ near-miss: set the LANGUAGE in cc.txt... | 7 |
| 6cq | Both of wave 9's group-c floors are the SAME residue: 2004/b56 writ... | 5.2 |
| 6cr | A frame slot nothing writes is a C++ frame slot: a DECLARED destruc... | 6.3, 7 |
| 6cs | Copy the emit spelling out of a MATCHED sibling verbatim, let the c... | 4.2 |
| 6ct | A value assigned to a variable the optimiser will not propagate get... | 3.4 |
| 6cu | A dead initialiser in the ENTRY block pulls a loop-carried variable... | 3.4 |
| 6cv | The receiver lever on a CSE web that crosses a call: give the produ... | 3.4 |
| 6cw | The callee-saved colour list 2004/b56 hands to main-line call resul... | 3.7 |
| 6cx | A callee's DECLARED signature is a register-allocation input: a met... | 0, 3.5 |
| 6cy | A "dead mov" residue can be an argument the caller really passes: c... | 0, 3.5 |
| 6cz | Two findings and one model for the 6bs class: a colouring model tha... | 3.8 |
| 6da | A scalar stack parameter the loop uses directly is register-homed i... | 3.6 |
| 6db | Two floors re-attacked, and the `-opt` vocabulary: an unknown `-opt... | 7 |
| 6dc | func_0202ffec matched: 64-bit operand locals, with their assignment... | 0, 2.3, 3.3 |
| 6dd | func_02009e70 matched: the two distance limits are later live range... | 0 |
| 6de | Placement new always null-tests in 2004/b56; build the object as wh... | 6.3 |
| 6df | A hand-written `__register_global_object` call is a static `Vector3... | 6.3 |
| 6dg | arm9 static initializers folded into their owners (2026-10-27) | 6.3 |
| 6dh | Two per-function pragmas retired: a folded-away null guard replaces... | 4.4 |
