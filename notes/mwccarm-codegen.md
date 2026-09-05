# mwccarm codegen notes

How **mwccarm 1.2/sp2p3** (`-O4,p -enum int -lang c99 -char signed -interworking
-proc arm946e -gccext,on -msgstyle gcc`) turns C into the exact ARM bytes in the ROM.

The point of this file: **write the matching C with the compiler's habits in mind, so the
first draft is close and you iterate less.** It does *not* replace the oracle
(`tools/match.py` / `swarm.oracle_ok`) - register allocation in particular is non-local and
cannot be predicted by hand, so every candidate is still compiled and byte-diffed. Think of
this as the priors that get you from "plausible C" to "usually one or two tweaks away."

These rules were learned empirically from the 4,400+ matched (C -> exact-bytes) pairs in
`src/`, and most are encoded as template rules in `tools/swarm.py`. When you discover a new
one, add it here AND, if it recurs, as a rule in `swarm.py` so the free tier can sweep it.

---

## 6ay. The C++ struct-copy lever reaches load ORDER, not just load width

The known divergence is that `v = *(T *)p` is a blind byte move in C and scalarises
to the members' own types in C++. The documented fix -- copy through a struct whose
only member is an array -- is usually described as restoring the `ldm`/`stm` pair.
It does more than that, and `RollingRock::Behavior` (ov021 0x02112854, 0x2d8) is the
case that shows it.

Copying a `Vector3_16` out of an actor at +0x92 as a method scalarised to three
`ldrsh` where the ROM has `ldrh` -- four words out. Retyping the destination to an
unsigned-member struct fixed the width and left THREE words: mwcc then reused one
register and interleaved `strh r1,[sp,#8]` between the two loads, where the ROM
batches `ldrh r2,[r4,#0x92]` and `ldrh r1,[r4,#0x94]` and stores after. The file's
note ruled out ten spellings -- named temps, pointer derefs, const references,
laundering the address, splitting the copy, reordering it -- and concluded
"spelling alone does not reach it".

    struct AngleWords { u16 w[3]; };
    *(AngleWords *)&v16 = *(AngleWords *)(c + 0x92);

matches exactly. The address is not 4-aligned so no `ldm` is involved: what the
array member buys here is that the copy stops being three member assignments at
all, and with it goes the register reuse that forced the interleave. Note the
lever is applied AT THE COPY, through a cast, not by retyping the destination --
retyping to an unsigned struct is the variant that leaves three words.

**How to apply.** When a C-to-C++ conversion misses at a struct copy, try the
array-member cast before any of the ten spellings above; it is one line and it
addresses both the width and the order. Reading the copy into a local of the
array-struct type first is NOT the same thing and is much worse (188 instructions
against 182).

## 6az. Class-typed by-value parameters are homed to the stack (the Fix12 wall)

Declaring a parameter as a by-value class - including the real `Fix12<int>`
template reconstructed in include/math/Fix12.h - makes mwccarm home the incoming
register arguments to the stack (`push {r0-r3}`) and reload each use, +0x14 bytes
on ShadowModel::InitModel. Scalar parameters of identical width stay in registers.
Identical on 1.2/sp2p3 and 2004/b56; `register`, `const`, and an inline conversion
operator instead of direct member access change nothing. Nor does declaring the
aggregate a `union` rather than a `struct`, or copying each parameter to a local
before use -- re-measured on Actor::SetRanges (0x02010e08, target 0x24), where every
aggregate form costs the same +0x14 and only a scalar parameter reproduces.

### CORRECTED 2026-08-27: the cost is per USE, and the wall was partly self-inflicted

The paragraph above used to end "The cost is in the parameter passing, not in how
the member is reached." That is wrong, and the correction matters because it turns
an unexplained wall into an ordinary one.

**A by-value class parameter that a body never reads costs nothing.**
`dEnemyBase_c::KillByInvincibleChar` takes `(const Vector3_16 &, Player &,
Fix12<int>)`, reads only the first two, and reproduces ov002 0x020ada40 exactly as
a real C++ method -- no homing, no prologue change. Homing is emitted in response
to a use, not by the calling convention. On `IsGoingOffCliff`, which has two such
parameters, reading one costs +0x8 and reading both +0x10: **two words per USED
class parameter.** So the lever, if one existed, would have to remove the read.

Which leaves the question the old note asked -- "the retail ROM's own `5Fix12IiE`
functions read their arguments straight from registers, so the original build had
a lever we have not found" -- resting on a premise worth checking. It does not
survive:

**The image contains zero `_Z...` strings and zero occurrences of `5Fix12IiE`.**
Both censused over arm9 and all 105 overlays. A stripped ROM has no function
symbol table; its RTTI carries length-prefixed CLASS names (`7daKrb_c`) and
nothing else. So no function's mangled name in this tree is ROM-derived --
every one is reconstructed, `include/types.h`'s "the ROM's own mangled symbols
spell it out" included -- and a `5Fix12IiE` in a name is a decision somebody
made, not evidence.

When that decision contradicts the bytes, the bytes win. `dEnemyBase_c::
IsGoingOffCliff` was the standing example of the wall: 17 spellings swept, best
+0x8, "cannot be a real method". Renamed to
`_ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi` -- `Fix12i`, a plain
`s32` typedef that mangles as `i`, keeping the fixed-point intent -- it matches
0x020ae2b8/0x19c as a real method. The wall was the name.

Two practical notes from that migration. The parameters must be USED DIRECTLY:
aliasing them into locals at entry (`int fix2 = down_;`) keeps both the parameter
and the copy live and cost two extra callee-saved registers, r7 and r8, plus a
spill reload. And `self` becomes `this`, not a `dEnemyBase_c *self = this;` local,
for the same reason.

**How to apply.** Before calling a `5Fix12IiE` symbol a wall, ask what anchors the
class spelling. If the answer is "the convention", and the ROM reads the argument
straight from a register, the honest signature is scalar. Where the spelling IS
anchored -- a shared-library entry point whose type is established elsewhere --
keep the extern-C definition with scalar args and declare the true signature in
the class header for callers; a call emits the same substituted symbol either way,
which the byte gate confirms.

## 1. Ground rules of the build

- **Reloc slots are wildcards.** Every `bl`/`blx` target and every pc-relative `ldr` of an
  external address is a relocation the oracle compares as a wildcard. So the *name* you call
  only has to compile - it does not have to be the byte-correct address. Still resolve the
  real callee name (the worklist does this for you) because it makes the C correct and keeps
  arg counts right.
- **Symbol size includes the trailing literal pool.** The pool words after the function
  disassemble as junk (`andeq`/`andseq`). `ins[-1]` is therefore **not** the `bx` - always
  find the epilogue (`mov r0,r4; pop; bx` or `mov r0,#k; add sp; ldm; bx`) by scanning, never
  by negative index.
- **`-O4,p` reorders and merges aggressively.** Pool loads get hoisted above their use;
  repeated globals collapse to a single pool word; loads are batched before stores. Write the
  straightforward C and let the optimizer reorder - do not hand-schedule.

## 2. The register-allocation wall (the dominant blocker)

This is *the* reason hard functions miss, and the one thing understanding-the-compiler can't
fully solve.

- mwccarm's allocator is **non-local**: which physical register a temp lands in depends on
  the whole function's pressure, not just the local statement. An isolated minimal function
  often colors a temp into `r1`; the ROM colored the same temp into `ip`/`r12`. The bytes
  differ only by that coloring (`triage.py` calls this **regperm**).
- You usually **cannot force `ip`/r12 coloring from C source**. Tried and failed: inline
  temporaries, `int` index vars, element-struct rewrites. If a candidate is byte-identical
  except for a consistent register renaming, it's regperm-blocked - flag it and move on, or
  hand-tune by changing the *shape* of the computation (extra/fewer temporaries) to nudge
  pressure. There is no reliable knob.
- Practical tell: if `match.py` shows the diff is only in register fields and the structure
  lines up, you're at the wall. Don't burn iterations re-expressing the same logic.

**Empirical findings (from `tools/coloring.py` over the matched corpus):**
- Only **~8.5%** of matched functions use `ip`/r12 at all, and it is almost always a
  **short-lived value-shuffle scratch**: the dominant contexts are `ldr ip` / `str ip` /
  `mov ip` (load a value, hold it, store it). It is essentially never live across a `bl`
  (calls clobber ip).
- The driver is **register pressure, not calls**: ip-using functions average ~0x66 bytes
  vs ~0x3c for the rest, with nearly identical call counts. Bigger function -> low regs
  fill -> the allocator spills a temp to ip.
- **The "right logic, wrong color" pile is small.** `triage.py` finds only a handful of
  regperm-only functions per module (e.g. 5 in arm9 <=0x50); the rest are "no template" =
  real logic. So coloring is the *last* thing that goes wrong, after types/offsets/structure
  are already correct - not the main blocker. Most misses fail earlier than the color.
- **Matching C for an ip function is just natural, full C** - no ip-forcing trick. Example
  that matches as-is: `return p[0] | (p[1]<<8) | (p[2]<<16) | (p[3]<<24);`. The coloring
  emerges from expressing the *complete* computation with correct types; a minimized stub
  under-expresses the pressure and colors into a low reg instead.

**Lever: the access EXPRESSION changes the allocation.** When a candidate is byte-identical
except a consistent register renaming, vary *how* you write the memory access before giving
up - the logic is the same but the codegen differs:
- `*(T*)G |= mask` (cast the array/base to a pointer and deref) vs `G[0] |= mask` (index)
  produce **different** register orderings. The deref form reproduced the ROM's r0/r1
  ordering on the `global_field_bitop` family where indexing always missed (it swapped r0
  and r1). This is now what the template emits.
- Other knobs that shift coloring: a named temporary vs an inline subexpression; writing the
  fuller surrounding computation vs a minimal stub; pointer arithmetic vs array indexing.
  None are guaranteed (allocation is non-local), but trying 2-3 access forms is cheap and
  often flips a regperm miss into a strict match.

## 3. Leaf patterns (no calls)

- **Bitfield extract needs an *unsigned* pointer type.** `*(unsigned*)p >> sh & mask` emits
  `lsr`; a signed type emits `asr` and won't match. Same for `unsigned char`/`unsigned short`
  loads when you want zero-extension.
- **Two adjacent words load-load-store-store is a struct copy**, not two scalar assignments:
  `*(P2*)dst = *(P2*)src;` (or `= G;` for a global). mwccarm batches both loads then both
  stores; writing two separate assignments produces different bytes/ordering.
- **`add` folds into the addressing mode.** For `*(T*)(p+off) = 0`, don't model an explicit
  `add` - emit the store and let CW pick add-vs-direct by whether `off` fits the immediate.
  Tracking an effective offset through an `add rT,rB,#big` is enough; the compiler chooses.
- **WALL: the *materialized*-`add` flag read-modify-write is NOT C-reproducible.** A large
  cluster of single-flag setters appears in the ROM as a frameless 5-instruction form that
  computes the address into its own register first and shares it for the load and store:
  `add rA,rB,#off ; ldr/ldrb rV,[rA] ; orr/bic rV,rV,#bit ; str/strb rV,[rA] ; bx lr`
  (e.g. the whole `BgCh::Start/StopDetecting*` family at `this+4`; the int-flag family at
  `obj+0x154`: `func_02009d30`, `func_0200ca14`, `func_020050dc`). The corresponding C
  (`*(T*)(base+off) |= bit`) ALWAYS folds the address into the offset and emits the tighter
  4-instruction `ldrb [rB,#off]; orr; strb [rB,#off]; bx` instead - the ROM's form is the
  *less*-optimized one, which `-O4,p` never produces. Confirmed unreachable: swept 6 source
  idioms (deref/ptr-var/index/ref/volatile/split-RMW) x all 11 mwccarm versions x 7 opt
  levels - every optimized build folds, `-O0` adds a stack frame, nothing lands the 5-instr
  shape. No `-opt nopeephole` knob changes it (it's instruction selection, not a peephole).
  These are not asm-hatch material either (clean compiled-code shape, no hand-asm tell).
  Treat the whole flag-RMW-materialized cluster as shape-blocked; flag and move on
  (2026-06-27).
- **Repeated global -> one pool word.** If the same global is referenced several times, CW
  loads it once. Emitting distinct extern names per use inflates the pool and breaks the size.
  Dedup globals by reloc-target identity.
- **Return-register discipline.** A real return loads into `r0` then `bx lr`. A tail-call
  veneer is `... ; bx ip` (a thunk) - `is_thunk()` filters these; they are not C-expressible.
- **`-char signed`.** Plain `char` is signed. Use `unsigned char` explicitly for byte flags
  and indices, which is almost always what the game means.

## 4. Call / reloc patterns

- **Wrapper:** save arg, call, return arg -> `X(a); return a;`.
- **ctor+vtable:** install vtable into `*this`, call `X(this)`, return this.
- **chain dtor:** a chain of single-pointer calls on `this(+off)`, returning this.
- **guard call:** `r = C1(this); if (!r) return r; return C2(this);` - the `beq` after the
  first call splits guarded-vs-after calls. Parse the branch target offset: calls whose `bl`
  offset is below the `beq` target are inside the guard (often just the first one).
- **frame call with pooled args:** a `stmdb` frame calling several functions whose args are
  pool-loaded globals. Reloc'd pool slot -> extern global; non-reloc slot -> literal const.
  Dedup globals by reloc identity (CW merges them).

## 5. C++ patterns - compile as C++, not C

The single biggest unlock. A source whose first line is a literal `//cpp` compiles with
`-lang c++` (and `extern "C"` keeps the symbol unmangled so the oracle finds it). Undefined
virtual methods are fine - we only `-c` compile, never link; only the vtable layout is read.

- **Virtual dispatch produces the pre-indexed writeback `ldr rV,[r0,#OFF]!`** that *no* C and
  no compiler flag will emit. Real C++ virtual dispatch through a pointer emits it exactly:
  ```cpp
  struct Base { virtual void v0(); ... virtual void m(); };  // VOFF/4 dummies before m
  Base* b = &this->base; b->m(ARG); return RET;
  ```
  CW also has a split form `add r0,r0,#OFF; ldr rV,[r0]` - it picks per offset; both come from
  the same C++.
  **Never emulate a vtable call in C (2026-07-02, func_ov006_0210d1fc, Fable):** the C
  form (`(*(fn**)(*(int*)obj + 0x48))(obj, 3)`) always HOISTS the vtable load off the
  object register; only a real `//cpp` virtual call (`((Obj*)c)->m48(3)`) produces the
  `mov r0,r4; ldr r2,[r0]` this-temp shape. If a diff shows the vtable load reading
  from r0 (the this-copy) instead of the object's home register, switch to real C++.
- **Pointer-to-member-function (PMF) call** has a fixed ARM ABI prologue:
  `ldr r3,[r0,#OFF]; ldr r1,[r3,#4]; add r0,r0,r1,asr#1; ands r1,r1,#1; ldrne...; blx`.
  Source: `PMF* p = c->pp [+ N/8]; (c->**p)();` with `typedef void (C::*PMF)();`. The
  `add r3,rA,#N` selects the (N/8)th PMF. Guarded/result-returning variants exist (null-check
  -> return a parameterized const; or return the call's int result).
- **PMF-table dispatch:** `(c->*TABLE[c->idx].pmf[k])()` - the global TABLE is reloc-wildcarded
  so it stays a leaf; element size comes from the `lsl#SH`.
- **Deleting destructor:** vtable install (compiler **hoists** the vtable pool-load above
  `mov r4,r0`, so don't pin it to a fixed position), then `D1(this)`, then a 2-arg
  `Deallocate(this, HEAP)` (HEAP is a global loaded via `ldr;ldr`), return this.
- **`new T()`:** `p = NEW(sz); if (p) { CTOR(p+off)...; p[voff]=&VT... } return p;`. Size too
  big for `mov` comes from a pool `ldr` (read the literal from the bytes; bail if that slot is
  a reloc).

## 6. Idiom families worth knowing (high-count in the game)

- **Matrix/transform setup:** `Matrix4x3_FromRotationY(m, ang); ...ApplyInPlaceToRotationX(m,
  ang); MulVec3Mat4x3(in, m, out);` then add a base position and `ApproachLinear` toward it.
  Extremely common in actor `_Main` state handlers.
- **Fixed-point scale:** `(int)(((long long)a * b + 0x800) >> 12)` is the Fix12 multiply;
  `cstd::div`/`mod`/`fdiv` route through the DS hardware divider registers at `0x4000280+`.
- **Actor state machines** (`St_*_Init` / `St_*_Main`): set a run of `this+off` byte flags,
  `SetAnim(...)`, set velocity fields (`0x98`/`0xa8`...), call `ChangeState`/sound, return 1.
  These cluster hard - one cracked representative teaches the whole family.
- **Destructor chains** (`func_ovNN_...` returning this): a vtable install, several
  `SubObjectD1(this+off)` calls in reverse construction order, then `Deallocate`.
- **Angle -> sin/cos table index** is a double-cast then a shift:
  `(unsigned short)(short)(angle + 0x8000) >> 4` reproduces the lsl16/asr16 then lsl16/lsr16
  sequence, indexing an `s16[]` table as `tbl[i*2]` (x) / `tbl[i*2+1]` (z) for a paired lookup
  (`func_ov002_020d8d10`, 2026-06-27).
- **Compiler-generated `__sinit_*` static initializers are matchable.** They are a straight
  sequence of registration calls; the constant-pool layout is reproduced by emitting the
  integer constants in source order - an ARM-encodable immediate (e.g. `0x21c` = `mov #0x21c`)
  stays inline while a non-encodable one (`0x21a`/`0x21b`) takes a pool word, so the order
  matters. Keep the trailing pool words in the target size (`__sinit_ov006_0212fc7c`).
- **Parallel-array "twin" getters.** Many overlay predicate/getter functions are byte-twins
  differing only in a gating global and one callee. Grep `src/` for the matched twin first and
  copy its struct model: e.g. three `struct { unsigned char b; unsigned char pad[3]; }` arrays
  read in lockstep at offset 0, with a fourth field as `unsigned char[i*4]`
  (`func_ov006_020f8154` twin of `func_ov006_020f9bec`, 2026-06-27).

## 6b. Logic is necessary but NOT sufficient -- match the codegen SHAPE

Analysis of LLM near-misses (drafts that compile but don't byte-match) found the dominant
failure is NOT wrong logic -- the logic is usually right. It is correct logic expressed in
C that compiles to a different instruction SHAPE. The recurring structural misses:
- **Control flow.** A mid-function `cmp; ...eq; pop..eq; bx..eq` is an early return -> write
  `if (cond) return X;` with the same test direction, not a wrapping loop/if-else. A loop
  whose conditional branch sits at the BOTTOM of the disasm is a do/while (test at bottom),
  not a `while` (test at top). A retry call at the very end belongs at the bottom of the loop.
  Conversely, a short forward branch past a guard chain is often NOT an early return -- it can
  skip a single call/block and rejoin the main body. Always derive control flow from the
  branch TARGET ADDRESS in the disasm, not from what the guard "looks like" (two 2026-07-09
  drafts mis-modeled skip-blocks as returns and diverged until re-derived).
- **Call-arg spilling.** When the ROM does `str rN,[sp,#K]` then reloads `ldr rN,[sp,#K]`
  before each of several calls, those are LOCALS the compiler spilled (calls clobber r0-r3).
  Use plain locals and pass them to each call; do NOT pack them into a struct (a struct
  changes the spill shape) unless the disasm shows a struct copy.
- **Statement order/count.** One store per assignment, in the disasm's order. No redundant or
  reordered assignments.
- **Arithmetic idiom.** `mul` then `sub` is `x*K - C`, not `(x - C/K)*K`. Reproduce the exact
  op sequence, not an algebraically-equal rewrite.
- **Switch case-block layout follows source case-label ORDER, not value order.** When a
  dense jump-table switch lays its case BLOCKS out in a non-numeric memory order, reorder the
  `case` labels in the source to match the block order in the disasm (e.g. write `case 1: ...
  case 3: ... case 2:` to get the 1,3,2 block layout). For a dense 0..N table mwccarm needs the
  low cases PRESENT even if empty -- add `case 0: case 1:` stubs to force the table instead of a
  compare chain (`func_0205fab4`, `func_02040724`, 2026-06-22).
- **Same-variable else-if chains can lose their skip branches -- force branch-per-case with
  `switch`.** When an `else if` chain compares ONE variable against different constants,
  mwccarm can prove the arms mutually exclusive and drop the "skip remaining checks"
  branches, emitting a shorter compare chain than the ROM. Rewriting the chain as a
  `switch` with an explicit `break` per case restores the ROM's branch-per-case shape even
  though the two forms are logically identical (one-shot crack of `func_0204a5c8` after
  else-if stalled, 2026-07-09).
- **Loop entry-guard shape = `while (p && cond)`.** A traversal that tests at the TOP and
  advances before the body (entry-guard `b`/`bls`/`blo` then a bottom back-edge) comes from a
  compound `while (p && cond)`, not a `for` or `do/while` -- those reallocate the iterator
  register and add/move the guard (`func_020584d0`, `func_020719ec`, 2026-06-22).
On a FALSE, diff your candidate's shape against the disasm and fix the FIRST divergence (a
missing reload, a flipped branch, an extra/absent instruction) -- don't just reshuffle.

## 6c. Boolean materialization & predicated-select shapes

How a boolean is *spelled* in C decides whether the compiler folds it into a compare or
materializes a 0/1 into a register. Getting this wrong is the most common near-miss on
guard-heavy overlay code (off by ~4 instructions), and there is a reliable knob for each.

- **Materialized bool (the verbose form).** ROM sequence
  `cmp rX,#k; moveq r0,#1; movne r0,#0; cmp r0,#0; beq` means the source stored the comparison
  in a NAMED integer, then tested it:
  ```c
  int t = (field == k);
  if (t != false) { ... }
  ```
  A plain `if (field == k)`, `t != 0`, `t == 1`, `?1:0`, an inline helper, and a real `bool`
  all FOLD to a direct `cmp/bne` (~4 instrs shorter). The suffix matters: `t != false` emits
  the needed `cmp r0,#0; beq`; `t == true` instead emits `cmp r0,#1`
  (cracked `func_ov002_020ba568`, 2026-06-27).

- **Predicated select -- use the override form, not the ternary.** ROM
  `mov r,DEFAULT; movne r,OTHER` for a `cond ? A : B` select comes from an explicit
  default-then-override statement, NOT a ternary:
  ```c
  int x = A;             /* default  -> mov r,A   */
  if (!cond) x = B;      /* override -> movne r,B */
  ```
  A real ternary constant-folds when A/B are 0/1, and a nested if-else range-merges two
  consecutive `==` into `movls`. Only the override statement reproduces the default +
  single predicated-move pair (cracked the `id2 != 0x16 && id2 != 0x17` select in
  `func_ov007_020b45b0`, 2026-06-27).

- **The access EXPRESSION shifts coloring (extends the section-2 lever).** Re-dereferencing
  the original argument each use (`**(unsigned short**)t`) instead of caching it in a local
  (`*p`) moved the long-lived pointer into `ip`/r12 and reproduced the ROM coloring. Worth a
  try when a select/guard is byte-identical except a consistent register rename.

- **Arg-builder temp ORDER sets the argument registers.** When a function builds a small
  struct of fields and passes its components to a call, the DECLARATION ORDER of the named
  temps decides which arg register each lands in. Declaring `pos` temps in `z, y, x` order
  produced x=r0/y=r1/z=r2 for the call; the natural x,y,z order mis-assigned them. Permute the
  temp declarations to match the disasm's arg-register assignment (the `Actor::Spawn` builder
  loop in `LoadDoorObjects`, 2026-06-27).

## 6d. Floor taxonomy additions (refine batch, 2026-07-01)

Three residual classes confirmed source-invariant across many C spellings; when the diff
shows one of these, stop early and report the near-miss:

- **Pool-load of an immediate-encodable constant.** The ROM emits `ldr rX,[pc,#pool]` for a
  small constant (0x4b; 0x64/0x66) passed as a call argument, where mwccarm always folds it to
  `mov rX,#imm`. Literal/decimal/enum-cast/local-var forms all produce the mov (func_0201a694,
  func_ov007_020cc4c0). Instruction-selection choice, no C99 lever found.
- **Early-exit epilogue duplication.** `-O4,p` duplicates a short conditional epilogue
  (`popeq/bxeq`) at an early exit where the ROM has a plain `beq` to the shared tail
  (func_0206ce20: identical across 10 phrasings incl. guard clause, goto, do/while-break).
  Related: an extra `add sp,sp,#imm` vs a fully if-converted merged epilogue
  (func_ov021_02111434).
- **Un-predicated branch around a pool-materialized byte-field RMW.** For `c[OFF]++` where
  OFF is not a data-processing immediate (e.g. 0x36e) but fits LDRB/STRB's 12-bit offset, the
  ROM guards the increment with a REAL `beq` and pool-loads the offset to build the address
  (`beq skip; ldr r0,[pc]; add r1,r4,r0; ldrb/add/strb; skip:`). mwccarm at -O4,p instead
  emits either the tight predicated 4-instr direct-offset form (no pool -- confirmed by an
  isolated single-statement test, so the pool is context/pressure-dependent, not
  encoding-forced) or, with a pointer var hoisted before the branch, a PREDICATED pool form.
  Swept ~8 phrasings (bare postfix, ptr-var scope/position, void* arith, inverted condition):
  each lands at most two of {pool, real branch, correct outer regs}, never all three
  (inverting the condition gets the branch but breaks outer control-flow regalloc). Example:
  func_ov072_021214dc (near-miss banked in _abwork, 48 div / 89 words, 324 vs 356 bytes).

The first two classes live in nearmiss/db.jsonl at div 1-2; candidates for an asm-block
close-out (sec 8) if their subsystems ever need completion.

## 6e. Fable-discovered levers (2026-07-01) - walls that turned out to be reachable

A Fable 5 refine batch cracked 5 of 12 drafts that Sonnet 5 (and in one case the
permuter's ~8k iterations) had exhausted. The working levers, in order of generality:

- **Fake data-dependency forces load/store batching.** The two-word struct-copy floor
  (target: ld a, ld b, st a, st b; mwccarm: ld/st interleaved) flips with
  `int a = pair.a; int b = pair.b; dst_a = b ? a : a; dst_b = b;` - the `b ? a : a`
  read pins b's load before a's store without changing the value, and the coloring
  follows (func_ov004_020b72d4, previously documented permuter-proof). Store EMISSION
  order (same regs, stores swapped) is still floor (func_ov004_020b7cd0).
- **Compile as C++ with a dummy-slot vtable to control the prologue.** Real virtual
  dispatch (`obj->m(...)` through a struct with N dummy virtuals to hit the right slot)
  loads the vptr BEFORE homing `this` to a callee-saved register; C function-pointer
  casts always home first. Cracked func_ov006_020ddeb0 and func_ov004_020b7a18 - use
  //cpp even for C-looking functions when the prologue order is the wall.
- **Reassign-before-call blocks copy-propagation.** `dh = (s16)(h - dh); f(&x, dh, ...)`
  emits the in-place lsl/asr narrowing where casting inside the argument list lets the
  compiler propagate and reorder (func_ov006_020c201c).
- **Break out of nested switches to reach the shared epilogue.** Cases that must branch
  to the function's final epilogue (not a duplicated one): `break` out of BOTH switches
  and put the tail in an `else` (func_0203fec4, the tail-merge wall from 6d).
- **`#pragma opt_strength_reduction off` WORKS** (unlike scheduling/peephole, which are
  ignored): keeps a pointer-induction loop un-reduced so the loop re-computes
  `add rX,base,rI,lsl #6` each iteration like the ROM (func_ov034_02112020's loop; its
  remaining gap is first-access-fold). Source-level, committable.
- **`volatile` array retains stack slots** that plain locals/structs lose to scalar
  replacement or rematerialization (func_ov102_0214953c).
- **A named partial-sum local + nested ifs flips bounds-check coloring.** Rewriting
  `if (cur + size > end) Crash();` as nested ifs with the sum in its own named local
  moved an r1/r2 coloring swap into place (FS_LoadOverlay, mid-band batch).
- **PMF argument liveness pins the coloring in member-fn-pointer dispatch** (found by
  andrewboudreau, PR #66, proven 7x): when the ROM's PMF-table dispatch uses r2/r3/ip
  for the address temps, declare the PMF WITH its int parameter and forward the index -
  `void (C::*)(int)` and `(c->*table[idx].pmf)(i)` - keeping `i` live in r1 through the
  blx forces the temps up. Zero-arg PMF variants of the same family (nothing to pin r1)
  remain floor.
- **STATEMENT order of first demand, not just declaration order, colors temps.**
  Reordering the statements that first USE each temp (store-before-load in an else,
  direct-global accesses instead of cached) let the scheduler re-emit ROM order with
  the right colors: first-demanded gets the highest-numbered register, descending to
  r0 (func_02040bb0). When declaration-order permutation stalls, permute statement
  order of first use.

Still floor after this pass (7/12): zero-offset first-access-fold materialization,
pre-indexed writeback from plain C (but see 6g 2026-07-02: escaped-array alias +
laundering can fuse into writeback), pure register-coloring swaps (~150 variants tried
on func_ov075_0211a948), and store-emission order.

Additions from the 2026-07-04/05 overnight runs (credit: Fable refine agents):

- **Call-bearing loops: struct-member access defeats address hoisting.** With cast-arith
  field reads (`*(int*)((char*)c + K)`) inside a loop that makes calls, mwccarm hoists
  the loop-invariant address into a spare callee-saved register in the preheader
  (`add rX, base, #K`); real struct-member access folds into the addressing mode at each
  use instead. Pair with `#pragma opt_common_subs off` when cross-call slot-address CSE
  persists (func_ov078_02125350).
- **`#pragma opt_propagation off` + a live constant local flips whole-function regalloc.**
  The pragma is honored (unlike scheduling/peephole); introducing `int base = 2` under it
  kept the constant live in r8 across a loop AND rotated the entire allocation to the ROM
  shape - single-shot fix from a 28-div draft (func_ov039_021112a0). Also the only known
  breaker for pure-constant folds the u64-mask launder cannot touch (func_ov015_021114f0).
- **Shift-form is a coloring lever (2026-07-10).** `(unsigned short)((unsigned int)x >> 12)`
  vs `((unsigned int)(x << 4)) >> 16` extract the same field but color differently; the
  cast-then-shift form flipped a 10-div residual that survived all 24 decl-order
  permutations (_ZN6Player11St_Owl_MainEv). Same family as the (short)x<<12 lever below.
- **Big-offset placement: let the compiler split, don't pre-offset (2026-07-10).** For
  cluster offsets like base+0x52c4, a pre-offset pointer (`p = c + 0x5000`) hoists the add
  to block entry; writing raw large offsets off the ORIGINAL base makes mwccarm emit its
  own `add #0x5000` lazily at the FIRST access, matching ROMs where the add appears
  mid-block (func_ov006_020e83bc).
- **Launder signedness is a value-number lever (2026-07-10).** `(int)` vs `(unsigned int)`
  inside the u64-mask give DISTINCT 64-bit value numbers, so two materialized bases around
  a call both emit instead of CSE keeping the address live across it - a finer-grained tool
  than respelling the whole mask (found on _ZN6Camera14GoBehindPlayerEj, 9->4 div).
- **Bit-field extract coloring: `(short)x << 12` vs `(x << 16) >> 4` (2026-07-10).** Both
  are the same extract, but the cast form forces a non-inplace lsl into a FRESH register
  plus an in-place asr, reproducing a target's r0/r2/r1/r0 coloring that the double-shift
  form never hits (load-bearing in func_ov007_020be0dc). Also from that crack: a global
  `#pragma opt_propagation off` can HURT - it rotates coloring function-wide, so try
  removing it when a draft inherited one.
- **Vary the launder spelling per block to stop cross-call CSE.** Repeating the identical
  u64-mask expression in two blocks invites mwccarm to hoist the shared computation above
  the call; spelling the mask differently per block (zero-extend vs sign-extend placement)
  keeps each materialization local (func_ov090_02131b94). Also holds per OCCURRENCE in
  straight-line code (2026-07-09): repeating ONE spelling for the same non-encodable
  offset let the allocator cache the materialized address in a single persistent register
  across the whole function, pulling an extra callee-saved reg into the push set where the
  ROM re-materializes at each site; alternating sign- vs zero-extend cast placement per
  occurrence made each site re-materialize independently and byte-matched
  (func_ov002_020e9840, offsets self+0x4a2 / self+0x128, similarity batch).
- **Reloc-split trap (link gate, not codegen):** a byte-perfect single-array draft can
  still be WRONG-DEST if the ROM pools TWO nearby symbol addresses (parallel halfword
  arrays 2 bytes apart) where the draft uses one base + folded offsets. Restructuring to
  two externs changes codegen (worse); class is parked for hand-modeling
  (func_ov007_020b8ec0, needs 0x0208eeec AND 0x0208eeee pooled).
- **By-value small struct arg pins a frame pointer the ROM lacks (2026-07-06).** In C
  mode, passing a small struct (e.g. a V2 vec2) BY VALUE to a callee makes mwccarm split
  it across r3/[sp] with a dynamic sp adjustment at the call site, which forces a
  `mov fp, sp` frame pointer and pulls fp into the push set. The ROM has no fp. Fix: pass
  a POINTER to a caller-local copy instead - `V2 tmp = pos[j]; f(p,c,j,&tmp);`. Killing
  the fp collapsed ~1649 divergences / +140 bytes to a full match on a 1583-insn function
  (func_ov006_02115b0c, Fable, from PR #104 draft). Diagnostic tell: target `sub sp,#N`
  with NO `mov fp, sp` while your build emits both a smaller `sub sp` and `mov fp, sp`,
  and the whole function is register-skewed downstream - that is ONE systemic fp defect,
  not N independent coloring problems. Companion levers that landed the same function:
  u64-mask laundering on the pooled spawn-slot stores, volatile vtable stores to defeat
  function-wide CSE of a shared base pointer, inline volatile-RMW counter increments, and
  block-scoping every loop counter (`for (int i=...)`) to stabilize the per-loop reg set.
- **`#pragma opt_common_subs off` = EBB-local CSE = per-block rematerialization (2026-07-07,
  MASTER lever on LARGE multi-EBB functions).** On a big switch/state-machine function the
  ROM computes a shared index like `slot*4` ONCE per extended basic block and rematerializes
  it fresh after each call clobbers the reg, folding array accesses into `[base, idx, lsl #2]`
  with NO long-lived lsl. mwccarm's default GLOBAL CSE instead hoists one `lsl`/index into a
  callee-saved reg and keeps it live function-wide - the classic "lsl hoist / long-lived CSE"
  wall. `#pragma opt_common_subs off` switches the backend to EBB-local CSE, which IS the ROM
  shape. Under the pragma the control inverts and becomes a precise tool: a **named local**
  (`int idxa = slot*4;`) acts as MANUAL long-lived CSE for values the ROM genuinely shares
  across blocks, while an **inline expression** (`arr[slot*4]`) rematerializes per EBB. Two
  companion idioms are needed because the pragma also re-enables jump-threading of two-valued
  bools: (a) declare `int t = 0;` BEFORE the load that feeds it (early-init `mov`+`movne`, no
  threading); (b) a SINGLE reused `int` scratch temp reassigned per range check (not separate
  `rel0`/`rel1` as m2c renders them) reproduces the ROM's stale-temp reads - a later block that
  reads `(u8)tmpv` gets the LAST assignment, matching the ROM's register reuse. On
  Stage::PS_Update (0x0202635c, 3115 insns) this took case 8 from 213 div to byte-exact incl.
  the language arms, placed cases 1 & 0xa's mid-case literal pools exactly, and cut global div
  916->756 / region 690->180 (16->17 cases byte-identical). Pair with per-site u64-mask
  laundering (sec 6e/6g) for the struct-style `add base,idx,lsl#2`+`[rX,#k]` accesses.
  **This corrects 6f's "inert" verdict - see the scope note there.**
- **`bics rX,rX,#0` — force BIC by hiding the mask behind a non-const local (2026-07-07).**
  Target emits `bics rX,rX,#0` (a `x & ~m` with `m` materialized to 0), but every *literal*
  spelling (`x & ~0`, `x & 0xFFFFFFFF`, `x & ~0u`) folds to identity / `cmp` at parse time
  because mwccarm constant-propagates before instruction selection. Feed the mask through a
  non-const local or a tiny inline helper (`static inline int clr(int x,int m){return x&~m;}`
  called with a runtime-looking `m` that is provably 0) so selection runs on `x & ~m` and
  picks BIC *before* the value-numbering pass proves `m==0`. Cracked _ZN5Stage8BehaviorEv
  (Fable, div 49→0) and the same residual on the Stage::PS_Update near-miss. Pairs with
  `#pragma opt_common_subs off` + a named manual-CSE local (the EBB-CSE master lever) which
  carried the rest of that 444-insn C++ dispatcher.

## 6f. The pragma space, exhaustively characterized (2026-07-01)

Swept 20 CodeWarrior pragmas x all 96 div<=4 near-misses (1,920 compiles). Verdict:

- **`#pragma optimize_for_size on` cracks early-exit epilogue DUPLICATION** - the 6d
  class where -O4,p duplicates a short conditional epilogue (popeq/bxeq) instead of
  branching to the shared tail. It byte-matched func_0206ce20 after 10 source phrasings
  had failed. When the diff shows a duplicated vs shared epilogue, put this pragma above
  the function.
- `#pragma opt_strength_reduction off` keeps pointer-induction loops un-reduced (sec 6e).
- EVERYTHING else is inert on this backlog: opt_common_subs / opt_dead_assignments /
  opt_dead_code / opt_lifetimes / opt_loop_invariants / opt_propagation /
  opt_unroll_loops off, optimization_level 1/2/3, optimize_for_size off, pool_data
  on/off, global_optimizer off, auto_inline off, inline_depth(0), ipa off, common on -
  zero divergence changes anywhere. scheduling/peephole are silently ignored (6d).
  **SCOPE CORRECTION (2026-07-07): this "inert" verdict is SIZE-SCOPED to the div<=4
  small-function backlog, which has no cross-block common subexpression to eliminate.**
  On LARGE multi-EBB functions with a genuine function-wide shared CSE (e.g. a slot-address
  reused across many switch cases), `opt_common_subs off` is a MASTER lever, not inert - it
  flips global CSE to EBB-local rematerialization, the ROM's shape. See 6e (2026-07-07,
  Stage::PS_Update). Do NOT skip this pragma on a big function just because the small-batch
  sweep found it inert. `opt_propagation off` is likewise live on large functions (6e 07-05).

## 6g. The materialization "floor", precisely bounded (2026-07-01 corpus search)

Searched all matched functions' ROM bytes for `add rX, base, #imm` + zero-offset `[rX]`
first access: 61 verified instances exist, and ALL decode to two reproducible triggers:
(1) **encoding-forced** - halfword/byte accesses cannot encode offsets >= 0x100, so the
compiler must materialize (`*(short*)(p+0x100) = k` does it from plain C); (2)
**pointer-as-value** - the address is passed to a call (incl. `this` for virtual
dispatch on a sub-object: `Sub *b = &c->sub; b->m(0);`). The TRUE unreachable class is
only: word-width access, ldr/str-encodable offset, pointer never used as a value.
Re-check a "materialization floor" diagnosis against these two triggers before giving
up - some parked entries may be misclassified encoding/arg-pass cases. Tried and dead:
C++ references, reference-to-array, inline member fns via this (all fold identically).

**RECHECK COMPLETE (2026-07-01, 16-agent batch armed with the triggers): 0/16.** Every
"base materialization / addressing" draft in the recheck confirmed all three floor
conditions case-by-case; the categorizer was right. The class is RETIRED from paid
refine attempts - remaining paths are the asm escape hatch (policy pending) or
reversing mwccarm's fold heuristic directly.

**Laundering CAVEAT (2026-07-02 big-band batch): inside a loop, the laundered address
gets hoisted as loop-invariant** - mwccarm materializes it ONCE before the loop where
the ROM re-derives it per iteration, breaking more than it fixes (func_ov073_0211f61c's
do-while counter). Launder only straight-line/per-branch sites; for in-loop RMW walls
report the near-miss.

**Struct copies: match the ROM's INTERLEAVE, both directions (2026-07-02).** Whole-struct
assignment (dst[i] = src;) emits interleaved load/store pipelining (or ldm/stm); field-by-
field emits load-batch-then-store-batch. Pick by what the target shows: Ent{int,int}
arrays needed whole-struct (__sinit_ov006_02132970), Vector3 copies usually need
field-wise (multiple matches). Neither is universally right.

**Two more coloring levers (2026-07-02, 16/16 Fable batch):** adding a THIRD named temp
for an arg build (w = y + 0x78000) rotates the r0/r1/r2 assignment; and placing a
copy-through-direct-expression BEFORE a saved= assignment makes CSE spill its temp
pre-writeback (func_ov073_021215cc, func_ov102_0214ae1c).

**RETIREMENT LIFTED (2026-07-02): the u64-mask laundering idiom IS the lever.**
`*(int *)(((int)base + 0xOFF) & 0xFFFFFFFFFFFFFFFF) |= x` forces the materialized
`add rX, base, #OFF` + `[rX]` form - the identity AND routes the address through
64-bit arithmetic, which the front-end cannot fold into an addressing mode. It was
sitting in three VERIFIED corpus files all along (func_0200d064, func_ov006_020ded00,
and tonight's func_ov007_020c9f10, whose agent mined it from the corpus). Confirmed
on two freshly-retired floor drafts: both byte-MATCH on first application
(func_ov072_0211f9c4, Player::St_Squish_Init). A mechanical sweep applies it across
the whole backlog - see the crack-loop runbook. The floor sections above remain
correct about PLAIN C forms; the laundered form escapes them.

**Laundering MASK PLACEMENT matters (2026-07-02, Opus→Fable 0x140-0x280 batch,
func_ov002_020b9c00):** when `(((int)base + OFF) & 0xFFFFFFFFFFFFFFFF)` splits into
`mov + adds` instead of the single `add rX, base, #OFF`, wrap the COMPLETED 32-bit
address instead: `((long long)(int)(base + OFF)) & 0xFFFFFFFFFFFFFFFFLL`. The
pointer arithmetic folds to one add before the 64-bit detour, yielding
`add r2,r4,#0x8e` + `ldrsh/strh [r2]`. Try both placements before calling floor.

**Pre-indexed writeback from (laundered) C - the plain-C floor has a second exit
(2026-07-02, same batch, func_ov081_02124b98):** a write-only local struct the ROM
emits member-wise was only retained by mwcc as an aggregate copy (ldm/stm) - UNTIL
the copy was rephrased as an address-escaped array `v[2]` aliasing the struct with
its delta (stores survive DSE, values stay in regs) PLUS 6g laundering of the base;
the peephole then fuses the materialized add into `ldr r1,[r0,#0x5c]!` - a writeback
form previously seen only from C++ virtual dispatch (sec 5). "Pre-indexed writeback
from plain C" in the 6e floor list is now conditional, not absolute.

**volatile-in-condition defeats CSE per branch arm (2026-07-02, same batch,
func_ov006_020c68f4):** when the target reloads a value in each arm that mwcc wants
to CSE from the `if` compare, a volatile read in the condition forces every arm to
re-load (`[dst+0x20]` reloaded per arm, no reuse of the cmp value).

**Mutable-variable subtraction beats the rsb const-fold (2026-07-02, same batch,
func_ov004_020b3cb8, Opus):** `v = K - x` const-folds to `rsb`; writing
`int v = K; v -= x;` forces the ROM's `mov rX,#K; sub` pair.

**Two Opus-declared floors cracked by Fable retry (2026-07-02, 0x400-0x800 batch):**

- **Loop-tail store-emission order** (func_ov004_020b2cb8, div 4, "volatile had zero
  effect" per Opus): the working combo is volatile store/load + routing the address
  through the REUSED plain temp + `#pragma opt_strength_reduction off` above the
  function (the pragma stops the address-as-value form from strength-reducing).
  Named laundered pointers always mis-colored - only plain temps reach r7.
- **Register-coloring swap that survives decl-order permutation** (func_ov060_02111f08,
  div 7): the lever was STORE-ORDER statement permutation inside the block (compute
  z,y,x; store sp.x,sp.y,sp.z), found by a scripted sweep of store-order perms. When
  decl-order and compute-order permutation stall on a coloring residual, sweep the
  STORE order before calling floor - extends the "statement order of first demand"
  rule (6e) to emission sites.
- Also (func_ov085_0212c230): passing a constant arg EXPLICITLY (`&data_...` instead
  of relying on CSE) colors the compare chain; a volatile read on one call arg orders
  it relative to other volatile reads across a call boundary.

Both cracks took the agent 60+ attempts - on promoted div<=8 drafts the long grind
can pay (Fable, effort high, retry tier only); do not extend this to fresh fan-out.

**The "pool-load of an immediate-encodable constant" class (6d) was a MISDIAGNOSIS**
(2026-07-02, func_0201a614): the pool slot is not a constant - it is a SYMBOL ADDRESS
(a reloc; `(int)&overlay_75`), whose unrelocated low byte read as 0x4b. When a diff
shows a pool load where you wrote a small constant, check whether the slot is a reloc
in the target and pass an address instead.

## 6h. Laundering a pooled global's ADDRESS fixes its register CLASS, not just materialization (2026-07-08)

The 6g u64-mask launder was documented as a lever for address *materialization*
(forcing `add rX, base, #off` + `[rX]`). It ALSO fixes a distinct problem: a pooled
global sitting in the WRONG register on an otherwise byte-exact function.

**Symptom (pure-coloring near-miss):** falign shows the instruction stream is identical
(0 insert / 0 delete, correct size) but a pooled global's base is colored to the leftover
register (typically r3), and that one swap ripples into ~25 differing words. On
func_ov024_02111350 the ROM held the sincos table base (`data_02082214`) in r0 and the
`c+0x300` temp in r2; every plain-C spelling put the table in r3 and the temp in r0.

**Cause:** a *plain* pool-address constant - whether inline (`data_sym[i]`) or a plain
named pointer (`T* p = data_sym;`) - is in mwcc's *rematerializable-constant* class,
which the allocator colors LAST, so it always takes whatever register is left over. A
plain named pointer mis-colors just as badly (often callee-saved or the leftover reg);
this is the same "named base pointers mis-color, only plain temps reach the intended
reg" effect seen in 6g's laundering caveat.

**Fix:** launder the global's address so it becomes a "computed" value allocated in
NORMAL birth-order:

```c
#define M(p) ((long long)(int)(p) & 0xffffffffffffffffLL)
s16 *tbl = (s16 *)(int)M(data_02082214);   /* claims r0, not the leftover r3 */
int  a1  = ...;                            /* next-born named local -> r1 */
```

The laundered-named pointer declared first claims r0; subsequent named locals / temps
descend r1, r2, r3 in birth order to match the ROM. Pair with `#pragma opt_common_subs
off` (EBB-local CSE, so the `c+0x300`-style base rematerializes per EBB like the ROM
instead of being hoisted into one long-lived register) and control long-lived vs
per-EBB values by named-local vs inline (6h's companion, the EBB-CSE master lever in 6e).

Cracked func_ov024_02111350 (div 25 -> 0, byte-exact; the file had been parked as a
`//cpp` NONMATCHING "not byte-matchable from C" draft - wrong). Found by a Fable refine
agent seeded from the 25-div near-miss. **Inert on this shape (~90 variants):** decl
orders, `register` keyword, volatile, struct-member spelling, C++ mode,
`opt_propagation off`, and - critically - a plain (un-laundered) named `tbl` pointer.

## 6i. Block-scoped temp flips a virtual-call-result loop coloring swap (2026-07-07)

**Symptom:** a loop miss that is a pure cyclic r4/r5 SWAP between a reused pointer and a
reused mask/accumulator, where the value that gets swapped comes from a virtual (or other
indirect) call result inside the loop body. Everything else in the loop is byte-exact;
only the live-range coloring of the call result vs. the carried pointer/mask is inverted.

**Fix:** route the call result through a FRESH nested scope instead of assigning it
directly to the carried variable:

```c
{ int t = o->vN(this); mask = t; }   /* correct: extra scope boundary */
/* NOT: */
mask = o->vN(this);                  /* wrong: same coloring as before */
```

The block `{ }` around the temp adds a live-range boundary that changes which register
the allocator hands the call result, which flips the rotation to match the ROM. This is
distinct from named-manual-CSE (6e/6h) - the point here is the SCOPE boundary, not
caching a value for reuse; the temp `t` is dead immediately after the assignment.

Cracked `_ZN10SphereClsn10DetectClsnEv` (SphereClsn::DetectClsn, div 16 -> 0, Fable,
~21 attempts). Try this BEFORE declaring a virtual-call-result loop swap a coloring
floor - the plain floor (register-coloring swap with NO virtual call feeding it) is
still unreachable and should still be parked.

**Confirmed NOT applicable (2026-07-11), but see 6m:** a symmetric `sb`/`sl`
(dst-temp/src-temp) coloring swap in an inlined `ldm!/stm!` aggregate-copy loop with
NO call in the loop body - mwccarm invariantly assigns dst-temp to `sb`, src-temp to
`sl` regardless of decl order, block-scoped temps, array-subscript form, reference
binding, or a fake-dependency ternary; applying the 6i call-result scoping trick here
made it WORSE (div 8 -> 20). Do not retry 6i on a writeback-temp swap with no
intervening call - the lever that DOES flip it is u64-laundering the DST deref (6m).
(`func_ov055_02111358` / `_ZN11MirrorLuigi6RenderEv`, Opus reached div 13 -> 8 via
decl reorder (i first, dst before src) fixing the OUTER counter/pointer coloring;
Fable exhausted 8 attempts on the residual inner sb/sl swap; later cracked to a full
MATCH via 6m.)

**Follow-up (2026-07-11, func_ov090_02130f94 MATCHED, div 48 -> 0):** the only other
ROM instance of the `mov sl,rSRC / mov lr,rDST / ldm sl!/stm sb!` writeback-temp shape
needed NO copy-temp intervention at all - a plain `global = *src;` aggregate assign
colored sb/sl/lr correctly on the first try. The real walls were elsewhere, and the fix
generalizes: when the target loop has BOTH a surviving multiplicative induction counter
(`add r4,r4,#3` feeding an mla) AND a scaled access that is NOT reduced (`add
r0,r7,r6,lsl #2` recomputed on both sides of a call instead of a `+=4` induction var +
cross-call CSE), turn BOTH `#pragma opt_strength_reduction off` and `#pragma
opt_common_subs off` on and hand-write the surviving counter as an explicit variable
(`idx = 6; ... idx += 3;` in the loop) so it outlives the pragma. Either pragma alone
made it worse (53/14 div); together with the explicit counter: div 6. The last 6 words
were the 3-word `v = r;` struct copy: struct assign emits ldm/stm (wrong), int temps
extend live ranges and reorder the loads (wrong); plain per-field `v.x = r.x; v.y =
r.y; v.z = r.z;` under `opt_common_subs off` emits the batched 3-load/3-store shape
byte-exact, including reusing the r.z load (r3) for a later stack arg.

## 6j. Array-subscript indexing defeats a LICM index*scale hoist under EBB-local CSE (2026-07-10)

Companion to 6e's `#pragma opt_common_subs off` master lever. Once the pragma flips CSE
to EBB-local, a manually-written pointer-arithmetic index expression like
`*(int*)(p + cur * 4)` inside a loop can still get its `cur * 4` scale hoisted out of the
loop as loop-invariant-code-motion candidate treatment, keeping `cur` itself pinned/spilled
across the loop body. Rewriting the SAME access as an array subscript,
`((int*)p)[cur]`, stops that hoist and lets `cur` un-spill into a normal per-iteration
register - closing a divergence gap that pointer-arithmetic rewrites alone did not (65->19
div on func_ov007_020ca308, paired with `#pragma opt_strength_reduction off` and the
u64-mask laundering for a separate `+0x18` RMW site, Fable, div 79->0 to full MATCH).
Try the subscript form before parking an index-variable spill as a coloring floor.

## 6k. Callee-saved locals color in REVERSE declaration order (2026-07-10)

The section-2 rule "register allocation follows declaration order" has a precise
DIRECTION for the callee-saved band (r7-r10/sb/sl), learned cracking the biggest
unmatched function in the game one-shot on Opus (_ZN5Stage9PS_RenderEv, arm9 0xb50 /
724 insns, div 21 -> 0). The long-lived locals that survive across calls are handed the
callee-saved registers in REVERSE of their C declaration order: the LAST-declared
long-lived local takes r7, the next-to-last r8, then r9, r10, sb, sl ascending. The
local declared LAST also tends to be the one SPILLED to a stack slot when pressure
exceeds the callee-saved set.

So to reproduce a target that colors `r7<-A, r8<-B, sb<-C, sl<-D` (and spills E), declare
them top-to-bottom as `D, C, B, A, E` - i.e. write the decl list in the register order
you want, LOWEST-numbered register's variable LAST, the spill victim dead last. On
PS_Render the winning decl block was `int var_sl; unsigned char var_sb; unsigned char
var_r8; int var_r7; int sp18;` (var_r7 last of the register group -> r7; sp18 truly last
-> spilled). Every natural/forward decl order mis-colored the render loop. This is a
cheap, deterministic permutation to try FIRST on any big-function loop that is
byte-identical except a consistent callee-saved renaming, before reaching for the
statement-order (6e) or store-order (6g) levers. Now in the sched_run.js prompt.

## 6l. Store-type and magic-divisor levers (2026-07-11 coddog batch 1)

Three levers surfaced cracking the 0x100-0x280 spread band (21/30 landed):

- **Byte-fill of -1: signed store emits `mvn`, unsigned emits `mov #0xff`.** A `-1`
  written through a *signed* byte pointer (`s8`/`signed char`) compiles to `mvn r3,#0`;
  the same value through a `u8` pointer compiles to `mov r3,#0xff`. When a reset/fill
  loop diverges by exactly that one instruction, flip the store type rather than the
  literal. (func_02068ae8, 1-div -> match.)

- **Reverse-derive the divisor, write the plain `/N`.** When the ROM shows a
  magic-multiply-plus-shift sequence, do NOT hand-emit the shifts: recover the original
  constant N from the magic/shift pair (Hacker's-Delight back-computation) and write the
  literal `x / N`. mwccarm regenerates the identical magic sequence, and small errors in
  a hand-rolled shift chain are avoided. (func_ov002_020b5c4c, `/2200` and `/1500`.)

- **Named `u16` local forces the `and/lsl/lsrs` narrowing triple over a single `ands`.**
  Assigning a masked value into a declared `u16` local (then using it) produces the
  and-then-shift-pair narrowing idiom; doing the mask inline collapses to `ands`. Use the
  named-local spelling when the target shows the three-instruction narrowing.
  (_ZN6Player17St_SlideKick_MainEv.)

New floor shapes confirmed (route to permuter/hand-fix, stop grinding from C): the
**SMULL RdLo/RdHi register swap** and the **mul-destination-register choice** inside
fixed-point sin/cos matrix-build blocks (func_0204be40, func_0204bbd8), and an
**address-of hoisting ORDER** floor where N independent `&field` computations are emitted
in a different but semantically-free order (func_02071d3c) - a sibling of the 6e/6g
ordering floors, unmoved by decl-order permutation.

## 6m. u64-laundering the DST deref flips the aggregate-copy writeback-temp swap (2026-07-11)

The "genuine plain floor" 6i warned about - the symmetric `sb`/`sl` src-temp/dst-temp
coloring swap in an inlined `ldm!/stm!` aggregate-copy loop with no call in the body -
is NOT a floor. Laundering the destination pointer through the u64 round-trip flips it:

```c
/* compiler emits mov sl,<src> / mov sb,<dst> (src-temp gets the HIGHER reg): */
*dst = *src;
/* compiler emits mov sb,<src> / mov sl,<dst> (src-temp gets the LOWER reg): */
*(Mtx*)(int)(((long long)(int)dst) & 0xFFFFFFFFFFFFFFFFLL) = *src;
```

The laundered dst expression delays the LHS address-temp's vreg creation past the RHS,
inverting which temp the allocator hands the lower callee-saved register. Everything
else (loop structure, running-pointer strength reduction, increment scheduling) is
untouched. Simpler complexity-adders do NOT work - `(int)`, `(char*)`, `(unsigned int)`
casts and `+ 0` all fold away before allocation; only the full u64 mask round-trip
(the same laundering idiom as 6h/6j) survives long enough. Laundering the SRC side
instead does nothing.

Cracked `_ZN11MirrorLuigi6RenderEv` (ov055, div 8 -> MATCH, one shot after a 9-variant
battery; also the only other instance of this shape in the ROM, `func_ov090_02130f94`,
is parked on the same swap - retry it with this lever). Try 6m FIRST when the residual
divergence is exactly the `mov/mov/ldm!/stm!` register pair of a struct-copy loop.

- **Free tiers first, every cycle:** `clone.py --apply` (byte-identical retarget) then
  `paramclone.py --apply` (same skeleton, substituted immediates) - they harvest the families
  each new match unlocks for $0 (dry-run without `--apply`, like every banking tool). Then
  `sweep.py --apply` applies the templates above across all modules.
- **One representative -> a template.** When you hand-crack a function that clearly recurs,
  add a rule to `swarm.py` and `sweep.py --apply` clears the cluster for free. That is how the
  whole template tier was built and it's the cheapest way to lower difficulty in bulk.
- **The oracle stays.** Understanding shrinks the number of compile-and-check iterations; it
  never removes the check. "Matched" means the bytes are identical, full stop.

## 6n. ov092 near-miss session levers (2026-07-11)

Three parked "not reachable from C" regalloc near-misses cracked byte-exact
(func_ov092_021316d8, func_ov092_021311b0, func_ov092_02131010). New rules:

- **Spell shift-chains as the CAST they came from.** A `lsl #28 / asr #16 / lsl #16 /
  lsr #16 / asr #4 / lsl #2` index chain written as five explicit shift statements
  mis-colors the whole downstream expression (table pointer / pool const / sum swap
  registers, killing an r5 spill the ROM has). Written as the source-level casts -
  `s16 t = (s16)((raw + 1) << 12); idx = ((u16)t >> 4) * 2;` with an element index -
  the same instructions come out AND the register story matches (fixed the push set
  and frame offset for free). Manual shift-spelling is semantically equal but
  allocates differently (func_ov092_021316d8).
- **When the ROM predicates one if/else arm, make THAT arm the `else`.** mwccarm
  if-converts the ELSE arm (predicated body + `bge` over it) and branches the THEN
  arm. `if (k >= 0) {A} else {B}` emitted blt/branch shape across every spelling of
  A; `if (k < 0) {B} else {A}` predicated A (5 insns: lslge/ldrshge/ldrshge/addge/
  strhge) on first try. Pair with a named temp for the RMW load (`s16 t = ang[k];
  ang[k] = (s16)(t + a4);`) to order the table load before the stack-arg reload
  (func_ov092_021316d8).
- **Compound `+=` vs explicit `=` on laundered-pointer RMWs is a coloring lever.**
  Three 6g-laundered halfword pointers + a named base: `*p = (s16)(*p + base[i])`
  colors the pointers r3/lr/ip with temp r2; `*p += base[i]` colors them ip/r2/lr
  with temp r3 (the ROM's). The compound form also flips which `add` is emitted
  first (RHS base add before LHS pointer add); if the ROM wants the LHS add first,
  re-derive the base INLINE per statement (`((s16 *)(c + 0x400))[i]` each time, no
  named base local) - CSE still merges to one add but generation order follows the
  LHS (found by the permuter on func_ov092_021311b0, reproduced by hand on
  func_ov092_02131010's zero-block: inline re-deref fixed a 3-way rotation there too).
- **Write-only shadow struct: `volatile` + named scalar locals.** When the ROM keeps
  a stack object whose stores are all dead (a "tmp" vector built, adjusted, then
  copied on to a second vector from REGISTERS), every plain aggregate spelling
  (struct assign, int[3], field-wise) gets fully scalar-replaced and the frame
  shrinks. The shape that matches: `volatile Vector3 tmp;` + plain named ints
  (`x = load; tmp.x = x; ... y = y - K; tmp.y = y; dust.x = x; ...`) - volatile
  retains every store in source order, the named locals keep the values in
  registers for the second copy, and no volatile READ is ever emitted because the
  source never reads tmp back (func_ov092_02131010, the "LandingDust double-store").
  **REFINEMENT (2026-07-30, func_ov006_021082fc): `volatile` fixes the SIZE but can
  cost you the COLORING.** On that function `volatile Vector3 tmp;` + named ints gave
  the exact frame and the 3 dead stores, but left an 11-word register rotation in the
  block feeding the call, and no declaration-order permutation (12 tried) moved it.
  The spelling that gives dead stores AND the ROM's coloring is a **plain (non-volatile)
  struct written and read through address-laundering casts**:
  ```c
  struct Vec3 t;                       /* NOT volatile */
  ((int *)&t)[0] = x;  ((int *)&t)[1] = y;  ((int *)&t)[2] = z;
  Callee(a, ((int *)&t)[0], ((int *)&t)[1], ((int *)&t)[2]);
  ```
  Taking the address keeps the stores (no SROA), non-volatile store-to-load forwarding
  still passes the values in registers, and demoting the fields to memory class removes
  the extra *named webs* that `volatile` + named ints introduce - which is what the
  rotation was. Prefer this form; fall back to `volatile` only if the coloring already
  matches. (Mined from the twin `src/func_ov006_02107ea8.c`, which uses the same idiom.)
- **Stack layout is declaration order, low to high** (volatile arrays and structs
  included): `saved[3]` then `v1` then `v2` lands sp+0 / sp+0xc / sp+0x18
  (func_ov092_021311b0; confirmed again on func_ov092_02131010's tmp/eq/dust).
- **Friendly extern aliases read BLIND in linkcheck.** `Particle_System_NewSimple`
  compiled fine but linkcheck could not resolve it; the config symbol is
  `_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_` (0x02022e98). Use the mangled
  name from config/arm9/symbols.txt in new sources (same lesson as the division
  `_u32_div_f` aliases).
- **2-instruction scheduler-order residuals ARE permuter-crackable.** The "ordering
  floor" note (cond-move polarity, store batching) does not extend to adjacent
  independent-instruction swaps: the permuter flipped one instantly on
  func_ov092_021311b0 (its mutation: inline the base re-deref). Route 2-div
  add/add or mov/str swaps to the permuter before calling floor.

## 6o. Ordering-residual triage: five levers, four true floors (2026-07-11)

Adversarial triage of every pure instruction-reorder near-miss in the DB (identical
instruction multiset, order-only diff, div<=12: 16 candidates after dropping 5 ghost
entries; one prober + one skeptic per function, ~40 spellings each): 6 cracked
byte-exact, 4 verified floor. The crack levers:

- **goto-pinned early-return flips cond-move PAIR order.** `return r == 2;` and every
  inversion spelling (`!(r!=2)`, ternaries both ways, if/else both polarities, named
  temps, xor/sub forms) canonicalize to moveq-first; the ROM wanted movne-first.
  `if (r == 2) goto yes; return 0; yes: return 1;` pins block layout so if-conversion
  predicates in layout order - byte-exact (func_0203faa8, previously survived ~8k
  permuter iterations). The goto is load-bearing: `if (r != 2) return 0; return 1;`
  canonicalizes back.
- **0/1 selects emit the TRUE arm first, and {0,1} INT arms canonicalize to
  bool(!=0)** (extends 6c), so movne-first is immovable in int spellings. Pointer-typed
  arms dodge the canonicalization while still folding to immediate movs:
  `(int)((x == 0) ? (char *)0 : (char *)1)` pins cmp/moveq#0/movne#1
  (func_ov007_020b91b4).
- **Equal-arm ternary on a call argument flips arg emission order.**
  `f(0x78, id ? c + 0x74 : c + 0x74)` (id provably nonzero, arms identical - folds to
  one add, no cmp) makes the const arg emit FIRST (mov before add = ROM order) with
  zero other byte changes (func_ov079_02126a84). Wrap the argument whose setup must
  move LATER; wrapping the other argument is inert. The same identical-arm select on a
  table pointer flips ldr-over-mov promotion on _ZN3HUD13InitResourcesEv but couples
  there (a strh stops sinking) - lever real, that function still open.
- **Loads-before-stores batching fires only in the ELSE arm.** Inverting a guard so
  the store+call arm becomes else (`==0` -> `!=0`, arms swapped) stopped a pool ldr
  from hoisting over a strb; the predicated short-arm bytes are identical either way
  (func_ov079_02126164). Same family: a default-arm guard spelled
  `if (x == 0) { ...; break; } return;` instead of `if (x != 0) return; ...`
  suppressed a pool-ldr hoist over a strh (func_ov004_020b3278).
- **volatile on a PAIR of adjacent stores un-freezes the pool-ldr batch order.**
  mwccarm otherwise batches all tail-block pool loads up front in a fixed internal
  order; volatile-qualifying the two stores dropped strh/orr into their ROM slots
  cleanly, 8 -> 6 divergences (func_0205fb58, still open).
- **Don't hand-expand what the compiler synthesizes.** A hand-expanded signed `/2`
  (add/lsr#31/asr#1 written out) pinned a byte store where written; the natural `/ 2`
  spelling regenerates the identical sequence and self-schedules the store into the
  ROM slot (func_ov095_021365d8).

What survived BOTH prober and skeptic - marked `floor(ordering)` in nearmiss/db.jsonl
(nearmiss_db.py mark-floor / unmark-floor; excluded from export-close and refine_wl):

- speculative stall-slot fill across a bne: OUR compile hoists a loop-counter init
  into a load-use gap, the ROM does not, and no spelling suppresses the hoist
  (func_ov006_020dac34);
- one-slot load-delay-slot fill (add-vs-str swap after a pool ldr): C++ frontend
  compiles the whole function byte-identical, comma-sequencing the store into the
  RMW statement is invisible to the scheduler (func_ov060_02113740);
- fixed call-arg pool-ldr hoist distance: always 3 slots above the bl in every intact
  spelling, ROM shows 1 (func_ov006_0211dad0);
- position-dependent final-block scheduler state: the byte-identical C spelling
  MATCHES in the function's first arm and diverges in the last block before the
  literal pool (func_ov006_020fb230);
- Stage::PS_Update's case-1 preheader independent-ldr pair (see that file's header).

Caveats from this run: two initial floor verdicts fell to the skeptic's fresh lever
families, and 6n's permuter result cracked a third ordering shape - treat
floor(ordering) as "no known lever", not "impossible", and unmark when a new family
lands. Mechanics: match.py diff columns are LEFT=ROM / RIGHT=candidate (two probers
misread this and inverted their narrative); the DB's div is sequence edit-distance,
not the raw MISMATCH line count (a 1-slot displacement can print 9 lines).

## 6p. A loop counter reused across loops inherits its prior loop's register color (2026-07-13, Fable 0x400-0x800)

Reusing ONE counter variable across several sequential loops (`for (i=...) {..} ...
for (i=...) {..}`) makes its register web span the whole function, so the allocator
keeps it in whatever callee-saved reg the FIRST loop colored it into - and that color
sits behind any constants the later loops hoist. Declaring a **fresh counter per loop**
(`i`, then `j`, then `k`, or block-scoped `for (int i...)` each time) lets each one
color first (r4 ahead of the hoisted pool constants), which is what the ROM does.
Matched func_ov006_0210e4f4 exactly: a 46-div residual was one coloring rotation from a
single counter shared across all 3 loops; three fresh counters cleared it in one edit.
Complements 6i (block-scoping a call-result temp) and 6k (reverse-decl-order coloring):
6i/6k control WHICH reg within a scope, 6p controls whether the web crosses scopes at
all. Cheap first thing to try on any multi-loop function with a lone counter and a
coloring-only residual. (Same batch reconfirmed the star levers on large logic:
u64-address laundering of RMW lvalues, `#pragma opt_strength_reduction off` for
mla+spilled-base index math, `#pragma opt_common_subs off` to stop cross-EBB base+off
CSE, and pointer-deref virtual calls to block devirtualization.)

## 6q. Register-web priority follows VARIABLE IDENTITY, not declaration/definition order (2026-07-13, Fable 0x400-0x800)

When two callee-saved locals (e.g. one in r8, one in `sb`/r9) sit in a swapped
coloring that survives BOTH declaration-order AND definition-order permutation, the
lever is neither: swap **which variable name holds which value**. The allocator's web
priority is keyed on the variable's identity across its whole live range, so moving the
*value* into the other-named local (rather than reordering decls/defs) is what rotates
the two colors. Cracked the r8/sb residual on func_ov006_020d6e8c after decl- and
def-order permutation both failed. Try this once the cheap ordering knobs (6k/6p) are
exhausted on a two-callee-saved swap.

## 6r. `#pragma opt_propagation off` keeps a 0/1 ternary selector as a stack scalar (2026-07-13, Fable 0x400-0x800)

A `sel = cond ? 1 : 0` (or `? a : b`) selector that the ROM keeps stack-resident and
reads back with predicated `ldrge`/`ldrlt` gets copy-propagated away by default (mwccarm
folds the constant into the consumer). `#pragma opt_propagation off` leaves the selector
as a plain scalar in the spill area so the predicated reload survives. Pair with a
`u32`-cast on the following loads to block CSE **without** volatile's slot-reload penalty.
Cracked a stranded 40-div near-miss on `_ZN7Clipper13Func_020150E8ER7Vector35Fix12IiEPh`.

## 8. The `asm`-block escape hatch (for hand-written-asm SDK primitives)

Some functions -- especially arm9 NITRO-SDK primitives -- are HAND-WRITTEN ASSEMBLY that NO C
under our flags reproduces, but they DO match as an mwccarm inline-`asm` function. The symbol
stays unmangled and any `bl SYM` inside is a wildcarded reloc, so the oracle accepts it. This
converts a whole class of "unmatchable floor" into matched (found 2026-06-22 cracking arm9):
```c
asm void func_02059824(void) { push {lr}; bl callee; pop {lr}; bx lr }
```
WHEN to reach for it (these read as HARD floor from C but are really hand-asm):
- A `push {lr}`-only frame with NO `sub sp,#4` around a single/double `bl` (C always either
  tail-call-veneers `ldr ip,[pc]; bx ip` or inserts `sub sp,#4; add sp,#4`).
- A single-register predicated `stm`/`ldm` writeback loop or unrolled 3-register `ldm/stm {r2,r3,ip}`
  block copy keeping both pointers live (`Copy32Bytes`, `MultiStore_Int`, memcpy primitives) --
  mwccarm's struct/loop copy always relocates pointers to lr/ip and uses 4-reg `{r0-r3}` batches
  with a frame, never the 3-reg frameless schedule.
- A leaf with NO prologue/epilogue that uses `r4`-`r7` (or any callee-saved) WITHOUT saving them
  and exits by FALLING THROUGH to the next function (no `bx lr`). No compiled C can use a
  callee-saved register without a save/restore frame, so this is hand-asm by definition (segment/
  autoload initializers that chain to the next routine: `func_0200497c`, found 2026-06-22). Write
  the trailing fall-through as `b <next_function_symbol>` -- the branch becomes a wildcarded reloc
  AND it forces mwccarm's auto literal-pool to land BEFORE the branch target, exactly as the ROM
  has it. `b done`/`bx lr`/no-terminator all mis-size the pool.
- SVC/syscall wrappers (`svc #N; mov pc,lr`).
- `mov`-materialized-constant stores coalesced into `stm rN!,{...}` pairs (Matrix*_LoadIdentity)
  -- though this one is sometimes still not reachable even via asm; try it.
mwccarm inline-asm SYNTAX GOTCHAS (cost real iterations): use `swi 0x123456` NOT `svc` and NOT a
`#` prefix; a conditional STM needs the explicit suffix `stmltia` NOT `stmlt`; an unconditional
multi-store needs the addressing suffix too: write `stmia r0,{...}` NOT bare `stm r0,{...}` (bare
`stm` = "unknown mnemonic"); branches use a BARE label name and the label is defined with a
trailing colon on its own line: `beq L2c` ... `L2c:` (NOT `beq L2c:` -- a colon in the branch is
"end of line expected"); a conditional half-word store is `strneh r1,[r0],#2` (width AFTER the
cond: str+ne+h), NOT `strhne`; for a byte-exact `pop {pc}` epilogue write `ldr pc, [sp], #4` NOT
`ldmia sp!, {pc}` (different encoding). `ip`/`r12` both accept in operands, but a `mov ip,#0` line
sometimes parses while the surrounding bare-`stm` does not -- fix the `stm` first.
A genuine-asm tell confirmed 2026-06-22: a leaf that does `mov`-zero then the SAME no-writeback
`stm rN,{...}` repeated dozens of times (func_020553c0, 32x identical `stm r0,{r1,r2,r3,ip}`) -- no
C emits repeated identical no-writeback stores; asm-hatch it. Same for SDK memset/fill with
alignment prologue + multiple mid-function `bx lr` exits and predicated `strneh`/`bics` (func_0205a588).

## 6s. The arm9 printf/soft-double cluster: READ8/WRITE8 macro idioms (2026-07-16, Fable largest-first)

The debug-printf formatter core `func_0206a928` (0x1360, biggest unmatched in arm9) and its
soft-double helpers (`func_0206bc8c` digit-extract, `func_0206bdb4` /10, `func_0206c244` x10,
`func_0206c51c` add, plus matched `func_0206c8b4` exp-extract / `func_0206c90c` sign / 
`func_0206c93c` div / `func_0206c9f4` pad / `func_0206ca44` strlen) are ONE original TU with
a shared byte-access idiom: every byte read/write goes through an unaligned-safe u16 RMW pair
(DS VRAM cannot take 8-bit writes). What reproduces the ROM shape:

- Reads are a TERNARY macro (value in a scratch reg, both arms if-converted ldrhne/ldrheq):
  `#define READ8(a) ((((int)(a)) & 1) ? ((*(u16*)((a)-1) & 0xff00) >> 8) : (*(u16*)(a) & 0xff))`
  Writing the read as an if/else statement into a named `b` instead makes `b` a callee-saved
  web and mis-colors the whole function.
- Writes are an if/else statement macro with the ODD arm as the then-branch; mwccarm
  if-converts the even (else) arm and emits it first with a beq over the plain odd arm:
  `#define WRITE8(a,v) if (((int)(a)) & 1) *(u16*)((a)-1) = (*(u16*)((a)-1) & 0xff) | (((v)&0xff) << 8); else *(u16*)(a) = (*(u16*)(a) & 0xff00) | ((v)&0xff)`
- Read-modify-write sites recompute the pointer BETWEEN the read and the write
  (`q = base + k; b = READ8(q) | X; q = base + k; WRITE8(q, b);`) - each macro use re-tests
  `(q & 1)` with its own `ands`.
- Doubles pass as `u64` by value; `u64 f(u64 d, int flag)` reproduces the `push {r0-r3}`
  arg-homing prologue exactly (reads via `(char*)&d`). Helpers redeclare each other with
  whatever prototype the call site needs (the ROM TU did too - see func_0206c90c's S16).
- Mantissa building: `mant = 0; mant |= 0x10; mant |= READ8(q) & 0xf;` as SEPARATE |=
  statements, then `for (i=5;i>=0;i--) { p = q + i; mant <<= 8; mant |= READ8(p); }` with the
  shift BEFORE the read (the read's conditional pair schedules between the two shift halves).
- `#pragma opt_strength_reduction off` + `#pragma optimize_for_size on` cluster-wide (as in
  the matched siblings).
- Variable u64 shifts are SOURCE-level 1-bit loops (`for (j=0;j<cnt;j++) t >>= 1;`), not
  `>> cnt` (no runtime-helper calls anywhere in the cluster).

Result: size-exact candidates for all four helpers (bc8c 48/74, c244 67/182, bdb4 153/292,
c51c 182/230 aligned divergences, all in nearmiss/db.jsonl). The residue is ONE systematic
callee-saved assignment rotation: the ROM gives the byte-pointer temps (q/p) and loop
counter (i) r4-r7 and leaves the u64 accumulator in caller-saved, while our compiles put the
accumulator low and the pointers in scratch. NOT governed by 6k reverse-decl order, decl
initializers, `register`, block-scoping, or in-TU asm callees (all probed and dead). A
two-step pointer build (`q = (char*)&d; q += 6;`) DID pull q into r4 on bc8c (53->48) -
statement-shape changes web priority where decl order does not (consistent with 6q).
Permuter is grinding the remainder; whoever cracks ONE of these cracks all five, then the
0x1360 monster (a DFA-table printf: state tables data_0209a0a0/data_0209a130, 0x40-byte
chunked stream writes through the callback at +0x10, %f via these soft-double helpers).

## 6t. Guard functions with early returns are PLAIN C, not asm-hatch (2026-07-16, natbree PR #387 fallout)

A conditional/early return that the ROM emits as a PREDICATED return -- `addeq sp,sp,#4;
moveq r0,#0; ldmeq sp!,{lr}; bxeq lr` -- reads like it needs hand asm (per section 8, the
inline assembler cannot even spell `ldm{cond}`/`bx{cond}`: a `pop {pc}` epilogue has to be
written `ldr pc,[sp],#4`). It does NOT. Write the guard as a plain C `return;` / `return 0;`
at `-O4,p` and mwccarm emits the whole predicated conditional return itself. The early-return
BODY (4 instrs: sp adjust + retval + ldm + bx) is too big for the optimizer to fold, so it
stays a predicated block that byte-matches the ROM. Asm-hatching these is strictly WORSE --
the assembler rejects the mnemonics, so you never match.

Proof: `func_02069918` (arm9) matched on main via #388 as plain C --
```c
void func_02069918(char *arg0) {
    u16 v;
    if (*(u16 *)(arg0 + 2) != 0) return;   // -> cmp; addne sp; ldmne sp!,{lr}; bxne lr
    v = *(u16 *)(arg0 + 4);
    if (v == 7) return;
    if (v == 9) return;
    if (v != 0x15) return;
    ...
}
```
natbree submitted `func_02069918` AND `func_02068398` in the (now-closed) PR #387 as
`asm`-hatched near-misses believing the `ldm{cond}` was unreachable from C. It is reachable;
`func_02069918` was already solved this way. So: a guard/dispatch function whose only "asm
tell" is conditional-return encoding is a section-6 C problem, not a section-8 asm one. Reach
for asm only when the tell is a frame/callee-saved/hand-schedule shape (section 8), not merely
a predicated return.

RESIDUAL FLOOR (why `func_02068398` still floors in plain C): two sequential guards that both
branch to a SHARED tail with a one-instruction reassignment before it. The ROM branches BOTH
guards --
```arm
    beq done          ; if p == 0
    ldr r0,[r0,#0x4b4]
    cmp r0,#0
    beq done          ; if y == 0
    mov r1,r0         ; sel = y   (unconditional)
done:
```
-- but mwccarm predicates the SECOND guard's `mov` into `movne r1,r0` and merges the branch,
regardless of phrasing: nested `if`, short-circuit `&&`, goto-pin (6o), reusing `p` for `y`,
and not-reloading the top-checked value ALL compile to `movne` (div=6, five variants tried).
The first guard branches (its skip-body is 4 instrs, too big to predicate); only the trailing
1-instr guard collapses. This is a predication-vs-branch floor in the same family as 6c: hand
asm branches the middle correctly (div=2) but cannot do the `ldm{cond}` returns, plain C does
the returns but predicates the middle -- a genuine double-bind. Best banked at div=2 in
nearmiss/db.jsonl (`func_02068398`, source "natbree (double-guard predication floor)").

The exact backend rule that causes this -- and the one construct that beats it -- is now
characterized in 6u. Read that before spending any more time on `func_02068398`.

## 6u. Backend cond-opt: the exact predicate-vs-branch rule (2026-07-16, Fable on func_02068398)

6t left "mwccarm predicates the trailing 1-instr guard regardless of phrasing" as an empirical
observation. The rule behind it, and why it is a true floor for a SINGLE-condition guard:

**mwccarm's backend cond-opt refuses to predicate a block that has >= 2 CFG predecessors.**

Everything else follows from that. Corollaries, each verified with minimal toys:
- **Arm-size threshold**: an arm of 5 instructions predicates; 6 branches. (This is why 6t's
  FIRST guard branches -- its 4-instr skip-body is over the fold budget -- while the trailing
  1-instr `mov` collapses.)
- **Other proven refusals**: a flag-setter in the arm, a call in the arm, a live loop structure.
- **cond-opt runs on post-RA physical code.** Copy-chains, u64-launders and other IR-level
  tricks have already collapsed by then, which is why every 6e/6h-family lever is inert here.
- **No pragma reaches it.** The full pragma table was dumped from mwccarm.exe:
  `opt_generateconditionalassignments` is IRO-level (`on` ICEs, `off` is inert),
  `opt_optimizecontrolflow` / `opt_repositioncode` / `opt_rotateloops` / `defer_codegen` /
  `late_fixes` and ~6 more are all inert on this. `-O1` DOES emit the unconverted branch, but
  loses the predicated early-return and O2+ tail scheduling -- and no flag state mixes the two.
  (CI flags are fixed anyway; only `//cpp` is a legal per-file directive.)
- **asm barriers do not work**: `asm("")`, `asm volatile`, gcc-operand forms and MW `asm {}`
  are lowered to IR, and cond-opt predicates them anyway (output byte-identical to plain C).

**The one matched precedent, and why it is not available here.** A corpus scan of all of arm9
for the ROM shape `cmp rX,#0; beq #8; <single mov>` finds exactly TWO sites: `func_02068398`
and `func_0205c048` -- and c048 is MATCHED. Its source (src/func_0205c048.c) buys the branch
with a genuinely 2-condition guard, giving the assignment block two predecessors:
```c
if (r8 != 0)      goto Lsep1;   /* pred 1: the goto      */
if (spflag == 0)  goto Lskip1;
Lsep1:  r8 = 1;                 /* pred 2: the fallthrough -> 2 preds -> BRANCH, not movne */
Lskip1:
```
`func_02068398` has only ONE condition. Every way to manufacture a second predecessor costs an
instruction mwccarm has no late pass to remove: it has **no cross-jumping** (duplicated tails
stay duplicated), no branch-to-next elimination, and redundant `cmp`s survive peephole. Measured
costs: c048-style double-goto = +8 bytes; redundant-inner-if = +4 (0x7c). Real `while`/`do-while`
loops also block predication (a toy emits the exact `beq; mov r1,r0`), but any surviving loop
needs a reachable, unfoldable backedge (+8 and a backward branch the ROM lacks); foldable ones
(`while(0)`, unconditional break) dissolve in IRO pass 2
(`IRO_CopyAndConstantPropagation -> EvaluateConditionals`) before cond-opt ever runs.

**Verdict**: for a single-condition guard, the ROM's linear 1-predecessor `beq + mov` is
UNREACHABLE from C under the canonical flags. `func_02068398` floors at div=1 (w-join draft).
Do not re-attempt it at 1.2/sp2p3; it is permuter/asm territory, and per 6t the asm route cannot
spell its `ldm{cond}` returns either. **When you see this shape, first ask whether the function
has a second condition** -- if it does, the c048 construct matches it for free.

## 6v. Four levers from the 2026-07-16 Fable batch (22 matches, arm9 + ov002/004/006/007)

**`volatile` as a register-pair SELECTOR -- with a hard precondition.** A statement-level
volatile init on ONE member of a closed scratch-register pair makes that temp take the HIGHER
register of the pair, changing NO instruction:
```c
u16 tail = *(volatile u16 *)&r->tail;   /* ip<->lr or r1<->r2 pair: this web now takes the higher reg */
```
Took `func_0206ca7c` from div=4 to MATCH -- a function that otherwise read exactly like a 6k/6q
coloring floor. **Precondition (this is the whole trick):** the web must have a MEMORY HOME.
It works on a struct member; it does NOT work on a register-param copy or a purely computed
temp, because there is no address to qualify. A refine pass applying it to 6 parked near-misses
scored 0/6 for exactly this reason -- `func_0205d688` and `func_ov060_021184bc` are param-copy
and computed-temp webs. Two further constraints, both cost real iterations:
- **Expression-level volatile does not fire it.** Only a statement-level *init* does.
- **Volatile loads schedule ASAP**, so this can introduce a load-ORDER swap while fixing the
  register swap. That is a real trade, not a bug: it left `func_ov007_020c6550` deadlocked at
  div=2 (right registers, wrong load order; every phrasing restoring load order flips the pair
  back). Try both-volatile, or pin the other load earlier with a data dependency.
It also defeats mwccarm's aggressive copy-elision when you need a dead store materialized
(`*(volatile s32*)&v.y = yv;` -- load-bearing on `func_ov002_020ea90c`).

**Launder the FULL address, not just the base, to control CSE.** 6h covers laundering a pooled
global's address. Refinement: laundering `base + off` as one expression CSE-MERGES two uses of
that address; laundering only the base keeps them DISTINCT. Pick deliberately:
```c
*(int *)((char *)(((int)o + 0x46b0) & 0xFFFFFFFFFFFFFFFF) + off)   /* merged -> ROM's [r0,r5] form */
```
Cracked `func_ov006_0210446c`. Caveat found while failing `func_ov006_020dbe9c`: a laundered
named global always outranks a plain local in allocation order, so the launder can re-color
correctly-but-wrongly (pool jumps AHEAD of the index). That was the wall there (div=21).

**Naming an invariant fixes its COLOR; leaving it unnamed fixes its LICM ORDER.** Two separate
knobs on the same loop:
- named loop-invariants -> you control which register each lands in;
- unnamed -> LICM hoists them in first-use order, matching the ROM's preheader interleave.
`func_ov006_0212a2e0` matched by naming only `mask` and leaving table/seed/base unnamed.
`func_ov006_020fc8c0` is the counter-example and a clean statement of a floor: named gets the
colors but the wrong preheader order, unnamed gets the order but the wrong colors, and ~40
variants could not get both (div=39).

**Equal-arm ternary moves an argument's setup later.** `f(..., guard ? e : e, ...)` -- the guard
folds away and emits nothing, but the arg's setup cluster moves to the ROM's slot. Moved an
emission cluster 5 slots (div 24 -> 14) and was load-bearing on `func_ov006_02115a5c`. Related:
an **explicit reload of a named count after a call** reproduces mwccarm's entry-throwaway-base
+ LICM'd-reload-base split.

## 6w. Comma-operator shares one register between two pooled pointers (2026-07-18, Random batch)

When the ROM materializes two pooled base pointers but keeps them in the SAME callee-saved
register (they are never simultaneously live), laundering both as separate statements (the 6e/6g
u64-mask) makes mwccarm treat them as both-live and hand each its OWN register -- a coloring
regression that pulls an extra reg into the push set. Fold the second pointer's materialization
into a comma-operator inside the clause that first uses it, so its virtual register does not go
live until that clause: `(p2 = LAUNDER(base2), p2->field)`. The allocator then reuses the
register the first pointer vacated and reproduces the ROM's shared-register shape. Landed
`func_ov006_020ce674` (two coloring rotations at once) paired with a per-pointer u64-mask launder
on all three data pointers -- the launder alone regressed to both-live; the comma-operator is what
serialized the two live ranges onto one register.

Companion lever, same session (`func_ov006_020ec2bc`): a loop counter written as a DERIVED
expression at the use site -- `(i*2+0x10)<<12` -- instead of an explicit `int k` counter avoids a
redundant strength-reduced induction variable. The explicit counter made mwccarm keep BOTH
`mov r7,#0x14` and `mov r6,#0x14000` live, raising pressure and spilling the loop-invariant `0` to
the stack; the derived form drops the second induction var. This is the OPPOSITE direction from the
earlier "explicit reload of a named count" / explicit-counter finding above -- so when a loop has a
multiplicative induction plus a spill, probe BOTH counter shapes (explicit scalar vs derived
expression); one of them frees the spilled register.

## 6y. Three levers for steering a callee-saved register WEB (2026-07-20, arm9 tail, Fable refine)

From closing `_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii` (67 -> 2) and
`_ZN12WithMeshClsn20UpdateExtraContinousEv` (246 -> 5). The recurring shape: a source form
that fixes the SCHEDULE simultaneously rotates two callee-saved registers, so the fix and the
regression are welded together (every "fixes +0x11c, costs 41" attempt). These three levers
break that weld by moving one value's web rank without touching the schedule.

**1. Zero-instruction priority booster: a fake self-select on a call result.**
`h = GetObjHeight(...); h = w ? h : h;` emits NO instructions but adds a USE to h's web,
raising its allocator priority above a competitor's. This is what let `h` retake r4 so `y`
could keep r5 while the schedule stayed correct. Variants `h = h ? h : h;` and doubled forms
behave identically. Distinct from the 6e fake-dependency ternary, which is used to create an
ordering dependence -- here the ternary is used *purely as a web-priority boost* and its value
is discarded by the optimizer. Reach for this when two values are fighting over one
callee-saved register and use-count (not order) is the discriminator.

**2. Callee-saved web rank follows the DECLARATION SCOPE DEPTH of the value-carrying temp.**
Measured on one value, everything else fixed: function-top decl -> sb (87 div), while-top ->
r6 (63), nested block -> r5 (41). Shallower scope = higher-numbered register. Independently
this reached the same fix as lever 1 above.

**3. A coalescing copy `y = u;` ranks by WHICH SIDE carries the live range across the calls.**
Placed BEFORE the calls, the web ranks as `y` (function-scope, bottom rank); placed AFTER the
competing def, it ranks as `u` (the block-scoped temp). This refines 6q: web *identity* is not
fixed by the variable, it is steerable by where the copy sits relative to the calls the range
crosses.

**4. `volatile` as a COLORING lever, not an aliasing one** (from the WithMeshClsn `str`-pair
cluster). Default mwccarm behaviour with two plain scalar temps: r4 goes to the temp of the
LAST store, and loads are emitted in REVERSE store order. The ROM did the opposite (r4 = the
first-stored value, loads in natural order) and NO permutation of assignment/store/decl order
reached it (7 forms). Reading each value through a volatile lvalue --
`c0 = *(volatile s32 *)&src->si.w0;` -- pins load order AND flips the r4 binding to
first-stored, byte-identical, with the offset-form ldr/str otherwise unchanged. Caveat found
the same session: volatile pinning only flips MEMORY-sourced webs; on two register-computed
values it does nothing (and forcing a memory round-trip adds instructions).

### Floors confirmed the same session (do not re-grind; both are marked in nearmiss/db.jsonl)

- **Prologue ARG-HOMING order** (`OAM::Render`, 2 words at +0x38/+0x3c): ~16 distinct spellings
  across 4 agents. Several give BYTE-IDENTICAL output; decl order reaches the region but
  rotates registers instead of reordering the store; direct `.val` use and struct-caching both
  regress to 52. Compiler-internal.
- **Allocator priority between two register-computed values** (`WithMeshClsn`, 5 words): the
  hoist itself is reachable, but the resulting vo2/cp r4<->r6 identity swap survived retyping
  as pointer, both launder shapes, dead-web coalescing (via two different dead webs), and
  use-count inflation (copy-prop folds it) -- the last four produced BITWISE-IDENTICAL output.

**Method note.** Both functions were closed by running 3 agents on the SAME target with
DIFFERENT angles and cross-pollinating: on `OAM::Render` two agents reached div 2 independently
via lever 1 and lever 2 above. A single agent had already declared that residual a floor after
10 probes -- it was not. Conversely, a residual that survives ~4 agents with disjoint angles
(as both floors above did) is worth believing.

## 6z. Seven levers from the 2026-07-21 Opus→Fable refine cascades (25 matches, PR #527/530/531/532)

All discovered by Fable escalating residuals Opus had mapped as coloring/scheduling floors,
seeded from Opus's narrowed draft. The meta-lesson (again): a residual one model calls a floor
routinely falls to a *different* model's register rotation — always Fable-escalate div≤20 misses.

1. **`#pragma opt_loop_invariants off` is NOT inert on larger functions.** The small-function
   verdict (6f) does not generalize. When a u64-laundered in-loop RMW base (6h) keeps getting
   hoisted out of the loop by LICM (producing a lower divergence but the wrong schedule), this
   pragma pins it in-loop. Cracked `func_0205e6e4` (in-loop word-RMW that laundering alone
   hoisted to 46 div).

2. **Short-type an array temp to the array's element type.** A temp holding `data_02082214[i]`
   (an `s16[]`) typed as `int` vs `short` changes the multiply grouping AND the register
   coloring of the product. Typing it `short` fixed both. Cracked `func_0202048c`.

3. **A `char*` (pointer-arithmetic) parameter stops the scheduler hoisting a laundered `add`
   into a load-use slot.** Where an `int` param let mwcc speculatively hoist `add r2,r4,#OFF`
   across a `ble` into the `ldr`→`cmp` load-use slot, retyping the param as `char*` (so the
   offset is pointer arithmetic, not an int add) blocked the hoist. Cracked `func_ov075_021190a4`.

4. **`volatile` on BOTH globals of a load/store pair pins them to source order** and, as a side
   effect, relieves live-range pressure enough to move a variable off callee-saved r5 into a
   scratch reg. Use when the scheduler batches two loads ahead of two stores but the ROM
   interleaves them. Cracked `func_ov006_020ed8a4`.

5. **Invert an if-arm's condition to defeat mwcc if-conversion.** When the ROM keeps a partial
   predicated+branch form (`strble` / `ble` / RMW) but mwcc fully if-converts your version
   (predicated `strble` + predicated RMW, one insn short), rewrite that arm so its THEN/ELSE are
   swapped (e.g. laundered `-=K` as THEN, plain store as ELSE) — the asymmetry stops full
   if-conversion. Cracked `func_ov063_0211aa34`. (An apparent `bl` tail-merge there was an
   alignment artifact, not a real divergence.)

6. **Split a `count++` used as an array index into `old=count; <laundered RMW>; buf[old]=v`.**
   A plain `c->count++` copy-propagates away the fresh address; the ROM routes the increment
   through a materialized register BEFORE the store. Capturing `old` and laundering the RMW
   forces that shape. Cracked `func_0205e280`.

7. **Statement order, not expression shape, rotates the mul-dest free list.** Moving two
   `volatile` stack captures to BEFORE a lerp/multiply expression changed which scratch reg the
   `mul` destination coalesced into (letting it take the dying operand's register). Any *naming*
   of the product subterm instead restructured mul/mla and regressed. When a mul-chain coloring
   residual resists expression rewrites, permute the surrounding statements. Cracked
   `func_ov006_02124088`. Also: `bad=1;` before `reject=0;` (declaration/assignment order)
   reproduced the ROM's `mov r3,#1` / `mov r1,#0` emission order — pair with `opt_propagation
   off` (6r) to stop that order from jump-threading a `==0` test into a `blo`. Cracked
   `func_02060b64`.

### Band economics (measured, 2026-07-21)
`crackloop.py refine --max-div 25` structural drafts ≈ **70% Opus @71–75K/landed**. Widening to
`--max-div 40` drops Opus to **~17% @288K** (bigger gaps bottom out on coloring/scheduling
floors) — but Fable then recovers **~75% of the closest misses** from the narrowed drafts
(@12–46K/landed). Lead with Opus at div≤25; when that vein thins (it is draining —
`refine_attempted.txt` consumes the pool each prep), widen and lean harder on Fable escalation.

### A truncated-target tooling bug that hides a TRUE match (not a codegen floor)
`func_ov064_0211a4c4` compiles reloc-wildcarded **byte-identical** to the ROM, but abverify
reports 28 div because the worklist `target_hex` (528 B) is **truncated**: `dsd` split the
function's dead exit block into a phantom symbol `func_ov064_0211a6d4` (0xc bytes —
`add sp,#0xc` / `pop {r4-fp,lr}` / `bx lr`, i.e. this frame's own epilogue after a
function-ending infinite loop). The compiler emits that dead block from every loop form; the ROM
has it too. **Fix:** merge `func_ov064_0211a6d4` into `func_ov064_0211a4c4` (size → 0x21c) in the
symbol config and regenerate `target_hex`; the source then MATCHes. Worth a sweep for other 0xc
pure-epilogue phantom-tail symbols immediately after `noreturn`/infinite-loop functions.

## 9. Prebuilt-library TUs: the ROM contains objects the canonical compiler never built

Distinct from every floor above. These are not "C we cannot spell" -- they are translation units
that were **not compiled by mwccarm 1.2 at all**, so no source and no lever can reach them. The
tell is that a whole TU's frame/staging idiom is absent from the entire 1.2-matched corpus while
being exactly what a DIFFERENT mwccarm version emits. Recognize and skip; do not grind, and do
not asm-hatch (these are compiled code, not hand-asm, so section 8 does not apply).

**Confirmed instances (2026-07-16):**
- **MSL `__pformatter`** (`func_0206e4a4`, arm9, 0x83c). Passes a 16-byte `print_format` struct
  by value staged BELOW `sp` with no sp adjustment and no frame pointer
  (`add r0,sp,#0x34; sub r5,sp,#8; ldm r0,{r0-r3}; stm r5,{r0-r3}`). Every mwccarm 1.2
  (base/sp2/sp2p3/sp3/sp4 x ~75 flag combos, C and C++) instead emits
  `sub ip,sp,#8; mov sp,ip; ... add sp,ip,#8`, which pins `mov fp,sp`, steals fp (the ROM keeps
  `' '` there) and grows the frame 0x244 -> 0x24c, shifting every stack offset -- a 999-word
  cascade from one lowering at 4 call sites. mwccarm **2.0/dsi** emit the ROM's exact no-bump
  form. Decisive: the matched siblings in the same TU (`func_0206fb08`/`fd6c`/`f820`) byte-match
  ONLY under 1.2/sp2p3. Cross-check: pret/pokediamond's Nintendo-built `__pformatter`
  (arm9/asm/MSL_Common_printf.s) shows the identical no-bump pattern and is still unmatched .s
  there after years.
- **The 4 `ActorBase::Process` PTMF wrappers** (`func_0204322c`, `func_02043288`, `func_0204335c`,
  `func_020432e4`). Pass three 8-byte pointer-to-member constants by value; arg 2 spans r3/[sp].
  mwccarm 1.2 emits `push {fp,lr}; mov fp,sp; mov sp,ip; add sp,ip,#4` (fp frame + dynamic sp)
  from EVERY source form (C structs, C++ PTMF constants, real member call, union, nested struct,
  char/short arrays, u64-struct, K&R, variadic, local copies, all opt pragmas). The ROM's
  fixed-frame staging (`stmdb {lr}; sub sp,#0xc; ... sub ip,sp,#4; str; str; ldm ip,{r3}`, no sp
  moves) is what **2.0** emits -- 2.0 + `-proc arm7tdmi` reaches 1 word (`ldr r3,[ip]` vs
  `ldm ip,{r3}`). These 4 are the ONLY occurrences of this staging idiom in the whole ROM
  (arm9 + all overlays scanned); nothing in the 1.2-matched corpus produces it.
  **SOLVED 2026-07-27 (6ai):** all four match under the recovered 2004 build 0056, which
  emits the ROM's fixed-frame staging directly. The near-miss C already in the DB matches
  unchanged; only the compiler was wrong. The "2.0 emits it" observation was the right
  scent from the wrong direction, since the real build predates 1.2 rather than following
  it.

**Rule**: before a long grind on a big library-looking function, check whether its TU-mates
match at 1.2 and whether the divergent idiom appears ANYWHERE in the matched corpus. If the
idiom is unique to that TU and a different mwccarm version reproduces it, stop -- it is a
prebuilt object, and the div count is not a measure of how close you are.

## 7a. The arm9 near-miss head: "register allocation" is a MISLABEL (2026-07-22, Opus->Fable cascade, 6 matches)

The heuristic categorizer in `refine_wl.py` labelled ALL 14 arm9 near-miss drafts
"register allocation" -- the category the router sends to the permuter and skips by
default. That head had therefore NEVER been refined. It was wrong on 6 of 8: the
residuals were structural, and 6 landed. **Run `refine_wl.py --include-all-cats` on a
module whose head is uniformly one category** -- a uniform label is evidence of a
categorizer failure, not of a real floor. (The same head is likely mislabelled in other
modules; check before trusting the routing.)

Levers that landed (all steer coloring/scheduling through SOURCE STRUCTURE, not pragmas):

- **Reverse-order independent global stores** (`func_020338b0`, div 4->0). mwcc
  re-reverses two adjacent independent stores to globals, so write the pair in the
  REVERSE of the order you want emitted. Combined here with hoisting one store above an
  `if` guard, which fixed an r1/r2-vs-r2/r3 scratch split.
- **Block-scope the loop pointer, declare the counter last** (`func_0203128c`, div 6->0).
  Scoping a pointer inside the if-body and declaring a distinct counter local AFTER it
  flipped a pure r5/r6 swap. This is 6i applied to a loop induction pair.
- **Stage a store-triple through a local struct temp, in emission order**
  (`func_0200c394`, div 7->0). Three shifted s16->fx32 values assigned to `t.x/t.z/t.y`
  in that order, then stored out, rotated the load/shift registers to r0/r3/r1/r2
  exactly. A struct temp is a stronger ordering constraint than three scalars.
- **Short-typed callee params hoist the narrowing** (`func_0205f77c`, div 8->0).
  Declaring the callee with `unsigned short` params instead of keeping explicit u16
  locals lets the compiler hoist the narrowing; explicit u16 temps rotated ALL
  callee-saved coloring (a 720-permutation decl sweep bottomed at div 22). Cf. 6y.
- **Arg pass-through keeps r0-r2 live to the `bl`** (`func_02068dc8`, div 4->0, Fable).
  A callee declared `void`/no-args in the DB was really taking the caller's args:
  passing `a,b,c` through costs zero instructions and holds r0-r2 live to the call,
  rotating an early scratch temp onto r3. **This is the 6z `this`-liveness lever
  generalized from C++ methods to plain args** -- suspect it whenever an early flag-load
  temp lands one register too low.
- **Drop an index temp for pure array subscripts** (`func_0203bc7c`, div 7->0, Fable).
  After `#pragma opt_loop_invariants off` fixed the pooled base coloring, removing an
  `ofs = i*2` temp in favour of `arr[i]` renumbered the vregs and slotted the induction
  `add` into a load-use gap. An index temp is an extra vreg; subscripts are free.

**Also confirmed here**: a low-div residual can be a COMPREHENSION bug. `func_0205f77c`'s
last 3 divergences were not codegen at all -- the "discarded" tail IME read is the
function's RETURN VALUE (`dummy = *IME; ... return dummy;`). Read the tail before
declaring a floor.

**Real floors from this batch** (both survived Opus 8 attempts AND Fable 8):
`_ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2` div 2 (coupled schedule+coalescing: the target
needs the AND after the dependent `lsr` with dst on the pool-const register; any order
that moves the AND later rotates t0 out of sl and blows div to 29-30) and `func_020729f4`
div 3 (r0/r1 rotation on a loop-head tag byte, invariant under 12+ spellings).

## 7b. Second arm9 head cascade (2026-07-22, 4 matches) + an amendment to 6t/6u

Follow-on to 7a, on the arm9 drafts carrying *structural* categories (predicate vs branch,
missing logic, constant/value). Opus 4/12, Fable 4/9 on the misses.

**AMENDMENT to sec 6t/6u**: the claim that an `asm` block "cannot spell `ldm{cond}` /
`bx{cond}`" is **WRONG**. mwccarm assembles `ldmeqia sp!, {lr}` (the explicit `ia` suffix is
what it wants) and `bxeq lr` directly. It also accepts `DCD 0x<word>` for hand-encoded
instructions where `dc.l` / `dc.w` / `opword` / `.long` all fail to compile -- useful to know
for the leaf-library asm files. The *C-side* predication floor in 6u is real and was
re-confirmed twice: plain C predicates the middle guard into `movne` (div 6) and no source
form reaches the ROM's conditional-return pair. **This does not make asm the answer for game
logic** -- `func_02068398` matches byte-exact as a whole-function `asm` block and was
deliberately NOT banked; it stays a near-miss. Whole-function asm for game logic is an asm
transcription, not a decompilation.

Levers that landed:

- **Fake-dependency store reordering** (`Particle::CheckLavaCallback::SpawnParticles`, div 2->0).
  To make a Vector3 field be *computed* first but *stored* second, hoist it into a temp and
  reference the temp in a degenerate ternary on the other store:
  `vy = <y expr>; v.x = vy ? sx<<3 : sx<<3;`. Generalizes 6y's fake-dependency trick to
  store-emission order.
- **Mixed temp/RMW interleave beats both pure families** (`Camera::Render`, div 4->0, Fable).
  On a 6-element `(x+4)>>3` writeback, *def order* pins register/slot assignment while
  *store-statement order* pins emission. Pure-temp and pure-RMW forms both floor (an
  exhaustive 720-permutation sweep bottomed at div 4); temps for a SUBSET (0,1,2,4) with
  def order (2,1,0,5,4,3) let the stores run ascending while holding the coloring. When a
  store block resists, mix the two idioms rather than permuting within one.
- **Volatile must cover BOTH sides of a str/ldr pair** (`func_0206dac4`, Fable). Volatile on
  the counter global alone was ignored by the scheduler for the non-volatile array load;
  volatile on the counter *and* the fn-ptr array, plus a temp index
  (`i = cnt-1; cnt = i; arr[i]();`), pinned str-before-ldr. Extends the 6-family volatile
  lever: the scheduler only serializes a pair when both ends are volatile.
  (NOTE: this function was matched concurrently upstream in #542 and was not banked here.)
- **Read the callee's real width and arity** (`func_02059c18`, div 6->0). The callee returned
  a full `long long` (the `sbc` subtracts BOTH words) and took a third arg the ROM never
  materializes -- `mov r2,#0` is absent because r2 already held a CSE'd zero from an earlier
  `*(vu16*)0x4000106 = 0` store, which is exactly what kept r2 live and pushed another load
  onto ip. A "missing" argument set-up instruction is evidence of a live CSE'd constant, not
  of a missing argument.
- **Control flow again, not codegen** (`OAM::Reset`, div 4->0). The if-arm's branch skipped
  past two `MultiCopy32Bytes` calls AND a zero-store pair, so five statements belonged inside
  the `else`; only two shared stores are the tail. This also explained an apparently
  "duplicated" `mov r2,#0` (separate basic blocks). **Third such case in two batches** -- when
  a residual includes a duplicated constant materialization, suspect basic-block structure
  before reaching for a coloring lever.

**Gate catch worth remembering**: `func_0205fb58` reached div 0 by merging a "phantom" symbol
(`data_020a813c`) into one struct array at `data_020a8138` so mwcc folds the +4 field offset
into a pool literal. The wildcarded byte oracle accepts it; the link gate rejected it
(`WRONG-DEST reloc: data_020a8138 != config 0x020a813c`). If a merge like this is genuinely
right, it is a CONFIG symbol-merge change, not a source spelling -- see the dsd symbol-split
note in the session log.

## 7c. Third arm9 head cascade (2026-07-22, 7 matches) -- the head's deep tail still pays

Opus 6/11 at 40.5K tok/landed on a div 2-17 pool (BETTER than the div 2-6 pools of 7a/7b),
then Fable 1/5 at 231K on the misses. Read that pair carefully: **on this backlog, draft
divergence count did NOT predict winnability** -- div 10/11 drafts landed while div 2-5 ones
floored. What predicted it was whether the residual was structural. The Fable escalation, which
paid 2/4 and 4/9 on shallower pools, dropped to 1/5 here; escalation is worth one pass, not two.

New levers:

- **Spill slots are grouped by TYPE, not declaration order** (`func_020319fc`, div 5->0).
  A stack-slot ordering residual was fixed by declaring a local `int` instead of `unsigned int`
  -- signed/unsigned was pinning the slot grouping. Try the type before permuting decl order.
- **DELETE a named local and let EBB-local CSE rediscover it** (`func_02046bbc`, div 10->0).
  A named `struct Node0 *node = ctx->node;` forced scratch r0 coloring; using inline
  `ctx->node->m18` / `ctx->node->m28` at both uses let CSE make the temp, which colors to
  callee-saved r7 like the ROM. The inverse of the usual "add a temp" instinct: **a named local
  and a CSE temp get different coloring priority**.
- **Mixed named/inline spelling of the SAME invariant reorders the preheader hoist batch**
  (`func_0206071c`, div 11->0, Fable). mwcc emits loop-invariant ADDRESS adds before CONSTANT
  movs; naming the invariant at only ONE of its three uses (inline at the other two) flipped
  the group order to the ROM's constants-first. Pairs with `opt_loop_invariants off`.
- **`opt_loop_invariants off` fixes preheader EMISSION ORDER, not just hoisting**
  (`func_02060f60`, div 2->0) -- it stopped LICM lifting a laundered base above the constant movs.
- **`volatile` on a ring-buffer array pins an indexed store ahead of a wrap-check**
  (`func_0205b070`, div 11->0), after fixing tail store order with inline RMW instead of
  hoisted temps.
- **A backward `bne` with imm=-4 is a spin-wait loop, not an `if`** (`func_0205583c`, div 11->0).
  The draft had modelled a GXSTAT busy-wait as an `if`; the back-edge block boundary is what
  re-materializes a `mov #0` the draft appeared to be "missing". **FOURTH control-flow-not-codegen
  case in three batches** -- the recurring tell is a missing or duplicated constant
  materialization. Check the branch offsets before believing a coloring diagnosis.

**Confirmed floors** (survived Opus AND Fable, both exhaustive): `Stage::LoadFog` (r5/r8 swap,
the CSE'd zero web colors last under every lever), `func_020341a8` (r1/r2 swap rooted in one
in-place-vs-fresh allocator choice), `GX::LoadTex` (r4/r5 web-identity swap; 9 disjoint probes
compiled BITWISE IDENTICAL -- a 6y-class allocator-priority floor).

## 6aa. The pragma crutch ROTATES callee-saved coloring (func_02033464, div 24->0, 2026-07-25)

The biggest single lever found for the "pure register permutation" wall. func_02033464 was
226/226 words schedule-identical with a consistent 4-register rename (args + one pool
pointer cycled through r4/r5/r6/r7). Every spelling variant of the same IR kept the same
rotation, because spellings that fold to the same IR give the same coloring. The draft's
`#pragma opt_strength_reduction off` + `opt_common_subs off` pair bought the right SCHEDULE
but perturbed the allocator's ranking; the ROM's coloring was mwccarm's NATURAL output for
pragma-free source. Probes confirm the natural rule: args live across a call home to
callee-saved regs in REVERSE arg order (c=r4, b=r5, a=r6), robust to use order, early
stores, and volatiles. If your draft's homing differs from that pattern, the draft (or its
pragmas) is perturbing the allocator; the original source did not force an unnatural
coloring, it simply never needed your crutch.

Recipe that took it 24->0, all source-level, no pragmas:
1. Delete the pragmas, diff again, and read WHERE the bytes break without them. Each break
   site names the natural construct the original used to get that schedule effect.
2. SR-bloat on an indexed VRAM store loop: type the pointer `volatile u16 *` (real hardware
   semantics; certainly the original's type). Volatile-qualified indexed stores are left
   un-strength-reduced, un-CSE'd, and emission-pinned for free.
3. CSE merging a byte-global read across a loop into one cached callee-saved reg (draft)
   where the ROM reloads at the tail: declare the global `volatile u8` and feed multi-use
   statements through an explicit temp (`u32 ch = g; x = ch + A; y = ch + B;`). One load
   per source mention, fresh reload at the tail, and the cache register frees up.
4. Pool-load ordered above a volatile stack store in the ROM but below it in the draft:
   hoist the read into a named temp BEFORE the volatile store in source
   (`int *dst = (int*)(cp + (g << 5)); li = 0; MultiStore_Int(li, dst, n);`).
5. The address-expression TREE picks which optimizer eats a second store to base+i+K:
   `*(u16*)((char*)&base[i] + 0x40)` gets SR pointer-walked; `base[i + 0x20]` spawns a
   second induction counter; `(base + i)[0x20]` is left alone (matches the ROM's
   `add r0, base, i, lsl #1` + `strh [r0, #0x40]`). Sweep all three forms; they are NOT
   equivalent to the allocator even when they emit the same bytes.
6. Last in-loop pair swap (base vs x fighting over r4): declaration order arbitrates it
   once the IR is right (declare the winner earlier). The decl-order lever only works
   AFTER the IR-level differences are gone; sweeping it on wrong IR is wasted motion.

Diagnostic discipline: a homing-only oracle (which regs the entry movs pick) can look
"fixed" while the schedule silently breaks downstream; always score with the full
word-diff count plus a positional map (entry vs loop vs tail regions). Mid-function
early-return guards demote the guarded value to color LAST under the pragma'd IR (the
demotion follows the guarded variable and needs surrounding pressure to trigger); that
demotion vanished with the pragmas, so treat guard-demotion as a symptom, not the disease.

63 nearmiss DB entries carry these schedule pragmas (several at div 1-10); they are the
immediate re-crack queue for this recipe. NOT applicable as-is to the s64/smull family
(func_02048234-style drafts diverge in schedule, not pure coloring; different wall).

## 6ab. Coloring-wall fleet levers (2026-07-25/26, 24 matches from the pure-regperm cohort)

The 6aa recipe fleet ran the full pure-register-permutation near-miss class (42 targets).
23 fleet matches plus 4 landed concurrently; all three functions previously recorded as
confirmed allocator floors (Stage::LoadFog, func_020341a8, GX::LoadTex) FELL. New levers,
each verified by a div->0 crack:

- **A stubborn scratch-web permutation near a call can be a DROPPED ARGUMENT.** Twice in
  one fleet: func_ov006_02108f2c (callee takes (self, idx); passing the value that already
  sat in r1 emits zero extra bytes but extends the value's web with r1 arg affinity,
  flipping the whole block) and func_ov006_020f7ee4 (a pass-through second argument keeps
  r1 live through case 0, rotating every scratch web up one register). Diagnostic tell:
  ROM temps ascend from rN+1 while yours ascend from rN, or a single reg reaches the call
  site unclobbered on every path. Cross-check callee signatures against already-matched
  callers BEFORE grinding coloring levers.
- **Equal-bytes shift-extract respell is a coloring lever at call-arg sites.**
  (unsigned short)(x >> 12) vs ((unsigned int)(x << 4)) >> 16 emit the identical
  lsl/lsr pair but rank webs differently; restored natural right-to-left arg coloring
  where every temp-hoist folded back (func_ov002_020d4748, div 8->0 in one step).
- **Branch-order inversion renumbers webs; decl order arbitrates only after.** Writing
  `if (x != k) cheap else rare` instead of the positive test rotated the loop webs, then
  reverse-decl-order (6k) landed the exact ROM colors (Stage::LoadFog, div 10->0 after a
  6x6 decl grid had floored at 10 under the original branch order).
- **De-volatile via char-cast reads.** A volatile spill-array crutch stalls at a pure
  color swap; plain array + *(int*)((char*)v + K) call-arg reads keep the memory pin
  without perturbing the allocator (func_ov002_020ae4cc). Pair with single-def folded
  temps (int y = load + n*0x50;) to keep the mla coalesced with its load.
- **Late shared-base via non-encodable offsets.** Deleting the named/laundered base and
  writing full-offset derefs (*(s16*)(c+0x334), *(s16*)(c+0x336)) makes the addressing
  optimizer build the shared base itself as a LATE temp that colors into the scratch the
  ROM wants; named/laundered address webs color EARLY in def order (func_ov098_0213a36c).
  Chain RMWs through a named s16 temp loaded first to demote address CSEs to lr/ip.
- **__sinit copy-group statement order** (struct copies before scalar stores) flips the
  src/dst pointer pair in the final group (__sinit_ov075_0211bb00; the pragma there is
  load-bearing, size drifts without it).

Verified rank-pinning floor (the one residue class that survived): mwccarm colors
named-local webs descending from r3 in FIRST-DEFINITION order; a web whose single
definition must precede the load that consumes it is rank-pinned, and CSE re-merges every
splitting attempt (func_ov006_020cb72c, 100+ variants; same signature as the previously
asm-hatched func_02058568/func_ov007_020bfd70). Six more no-progress hypotheses from the
fleet are in the nearmiss DB notes for their functions; the remaining unrun backlog is
listed in CLAIMS.md.

## 6ac. Opus groups-of-5 batch (2026-07-26, 9 arm9 matches): the "s64 family" was a mirage, and eight new rules

Taxonomy correction first: the "s64/smull schedule family" label was a CLASSIFIER ARTIFACT.
Three independent agents proved their targets contained no smull/s64 math at all - the
classifier keyed on u64-LAUNDER CASTS in drafts, not on actual 64-bit code. The real
anatomies were: IRQ RMW materialization, stack-escape aliasing, a bitmask blitter, and
plain rank problems. Classify by ROM anatomy, not draft text. Corollary of the batch:
every crack came from READING the ROM (pool words, register liveness at boundaries,
instruction multisets per cluster) before spelling variants.

Rules, each verified by a div->0 crack (full write-ups in the opus batch journal):

- **Whole-expression launder vs base-launder decides RMW base materialization**
  (func_0205c5e4). `(u32*)(((s64)(int)(r + 0x10)) & ~0ULL)` emits ONE `add rX, r7, #0x10`
  and un-folds the addressing mode; laundering just the base then adding the offset
  re-folds. Where you put the launder in the address TREE is itself the lever.
- **Taking &arr[i] escapes the WHOLE stack array** (RaycastGround::DetectClsn). Every
  unrelated scalar living in that array joins the alias set, its stores become ordered
  memory ops, and LICM/outgoing-arg scheduling pin around them. Model separate concepts
  as separate locals unless the ROM shows the pinning.
- **Callee-saved rank classes** (ExpandingHeapAllocator::Reallocate): locals whose
  defining RHS is param-only address arithmetic, taken as a maximal DECL PREFIX, promote
  above param webs. Also: a rank-extraction oracle (read the register role out of fixed
  offsets) beats div as a search signal - it stays informative when the value is wrong.
- **`smull <magic>` + `asr #1` fixup = pointer subtraction of a 12-byte struct**
  (func_02072dac). Read the literal pool BEFORE guessing a divisor: 0x2aaaaaab with the
  extra asr is /12, i.e. `Ep - Ep` for a 3-word struct, not `x / 3`.
- **`#pragma opt_loop_invariants off` STACKS on `opt_propagation off`** (func_020424c0):
  the pair killed every SCHED word where 18 single pragmas did nothing. Plus: a loop
  counter zeroed as the LAST statement before its do-while pins the bl/mov order.
- **Constant-carrying launder picks the AND coalesce partner** (OAM::Render Matrix2x2
  overload). `mask = t0 & 0xc000c000` reuses the word's register; `mask = 0xc000c000;
  mask &= t0` reuses the constant's - the ROM's choice. AND-operand analogue of 6ab's
  shift-extract respell.
- **Stack-resident constants are a local ARRAY, not volatile locals** (func_020490b0).
  volatile stores are high-priority for the list scheduler and sink to the block front;
  a plain `s32 tmp[4]` whose address escapes via the call argument keeps the stores in
  natural order and frees the scheduler to hoist pool loads above them.
- **C89 all-locals-at-top re-ranks the whole callee-saved list** (func_02063718).
  Block-scoped locals form their own LATE rank class; hoisting `int b; int i; char *ptr;`
  out of their case blocks to the function top rewrote [.., r6, r7, r8] into the ROM's
  [r4..r8] in one edit (div 23->0).
- **ROM burning a scratch on a constant while r0 sits free means r0 is LIVE: the
  function returns the callee's result** (func_02063e08, 20->5). Semantics tell, not a
  codegen lever: `r = f(...); e[0] = 1; return r;`.

New verified floor: func_0202ffec (quaternion product, 2 words). mwccarm puts the
SECOND-EMITTED operand load into smull Rn unconditionally; commuting the source product
renames the LOAD DESTINATIONS instead of swapping the fields (verified directly). The ROM
needs first-emitted Rn at 2 of 16 sites = mixed per-term evaluation order, with NO KNOWN
construct that flips load emission order at those sites without moving the smull
semantics. Same rank-pin class as func_ov006_020cb72c. Calibration note: this is a
verified-mechanism claim bounded by known construct classes, not an impossibility proof.
Three "confirmed floors" fell in the week of 2026-07-25 when a new lever family (6aa)
appeared; floor marks mean "do not spend model time by default", and the recorded open
angle (load emission order) is where a genuinely new construct class would attack. ~800 compiles incl. exhaustive
operand-spelling space (which collapses to 4 points per slot, not 12: Q(a,b)==R(b,a),
U==Q), pair-flip and annealing sweeps. Do not re-attempt.

## 6ad. Opus batch 2 (2026-07-26, 7 arm9 matches incl. func_02048234): siblings beat grinding

- **Cross-reference the callers BEFORE any coloring work** (func_02048234, 34->0, the
  longest-standing holdout). grep config/arm9/relocs.txt for "to:<addr>", map the callers
  to their enclosing functions, and if a matched sibling exists, TRANSCRIBE its idiom.
  The draft's semantics were subtly wrong in ways no codegen lever could reach.
- **Same-family scaffold port** (func_0204bbd8, 52->0): func_0204be40, matched earlier in
  the SAME run, shared the whole actor-render scaffold; copying it verbatim fixed the
  entire first half in one edit, and the mat[] store order came from the sibling too.
  Schedule batches so families land together.
- **Pin the dead scratch UP instead of pulling the live value down** (func_020729f4,
  3->0). When two scratch webs are swapped vs ROM and precoloring the live one would
  extend its range, give the DEAD one an affinity for the ROM's register - here by
  passing the already-in-place value as a second call argument (zero bytes emitted).
  Extends the 6ab dropped-argument family.
- **Param webs outrank locals unless you break the copy coalesce** (func_020503a4,
  60->4): natural rank is [homed params, reverse-arg][locals, reverse-decl]; a perfect
  7-register mirror band means the ROM wanted the block order flipped (write-up in the
  entry's DB note).
- **Result-variable initialization order re-ranks outgoing-arg emission**
  (func_02063e08, 5->0): `r = 0;` as a statement BEFORE the argument computations (not a
  declaration initializer) flips pre-schedule arg emission from ascending to descending
  register order.
- **The u64-launder carries a PHANTOM (address,value) pair** (func_020676e0, 4->2,
  measured): even when the laundered address folds back into [rN,#k] and emits nothing,
  its web still reserves a color and pushes the value one rank up. Measured grid in the
  DB note; two named open angles (8-byte aggregate store word order, phantom coalescing
  with the base). Do not re-grind the swept axes.

## 6ae. Opus batch 3 (2026-07-26, 9 arm9 matches): constructs, not registers

- **switch-case blocks schedule differently than goto-chain blocks** (func_020676e0,
  2->0, killing its recorded "phantom-pair" floor hypothesis). A switch case body will
  not hoist a pc-relative pool load above a preceding store; the same statements behind
  `if (m == k) goto ck;` dispatch DO hoist, and the dispatch bytes are identical. The
  prior campaign's launder was a fix-construct for the switch's schedule that introduced
  its own coloring artifact. When a case body's residual is a schedule perturbation or
  an artifact of something you added to fix one, respell the switch as a goto chain
  BEFORE any allocator lever. Diagnostic that found it: extract the case body into a
  standalone micro-function, compile, compare - if the micro matches ROM, the
  surrounding CONSTRUCT is the disease.
- **An enum-typed conditional defeats the {0,1} boolean-set canonicalizer at zero cost**
  (func_020503a4, 4->0). The canonicalizer that reorders `x = cond ? 0 : 1` pairs keys
  on the TYPE of the conditional expression: `int x = (int)(cond ? FMT_A : FMT_B)` with
  an enum never fires it, frame layout unchanged. Declaring the DESTINATION as enum also
  suppresses it but rotates the frame slot rank (non-int locals form their own frame
  class) - that trap is what produced the old "mutually exclusive conditions" floor
  claim. Original source almost certainly used a real enum.
- **Offsets in the literal pool are a shape tell** (func_02009aa8, rebuilt 999->0 in ~15
  compiles): pool words like 0x186/0x17e that are too small to be addresses mean
  halfword accesses past +0xff materializing their offsets - the draft's shape was
  wrong, not its colors. Classify by SIZE DRIFT first; the whole regperm lever family is
  inapplicable when the byte count disagrees (also func_02058308, stale DB div, same
  lesson).
- **Recognize library code and transcribe the reference** (func_0205e3d4, 59->0): it is
  RFC 3174 SHA1ProcessMessageBlock; porting the reference declaration block verbatim did
  more than any codegen lever. Check readable constants (K values, magic tables) against
  standard algorithms before treating a big function as game logic.
- **Read the hardware semantics off the constants** (func_02046e28, 101->0): POLYGON_ATTR
  masks, RGB555 shift ladders and DIFF_AMB bits identified the function as the material
  animation applier and fixed the struct layout; the sibling in the function-pointer
  table at 0x020157a8 supplied the idiom.

Batch meta: 9/15 matched, and the two lowest-div "nearly done" holdovers (020676e0 at 2,
020503a4 at 4) both turned out to be CONSTRUCT problems whose prior floor hypotheses
were artifacts of the fix-construct, not the compiler. Calibration keeps winning:
mechanism claims bounded by known constructs, never impossibility.

## 6af. Opus batch 4 (2026-07-27, 4 arm9 matches, deep-pile breakthroughs)

- **A probe seeded with the draft's own construct can only re-derive the draft**
  (func_02032f9c, 10->0, killing its recorded floor). The prior floor claim rested on a
  synthetic probe that faithfully reproduced the draft's behaviour - because it was
  built FROM the draft's q-address spelling. The ROM's register really was free; the
  fix was the sibling's pointer-INDEX idiom (`(u16*)(base + 0x40) + (u16)(w/8 >> 1)`)
  vs the draft's folded byte offset - equal bytes, different webs. When writing a floor
  hypothesis, ask what the probe inherited from the draft.
- **Deep-pile divergence mostly means "draft never got attention"**: func_020345b0
  (120->0), func_0206ece0 (112->0), func_0206655c (179->0) all fell in one session
  each with ROM-first reading, and Stage::InitResources went 193->4. High div is not
  a difficulty signal; it is an attention signal.
- **Read-order of adjacent globals is a real lever** (func_02032f9c lever 2): naming
  one global's read into a local FIRST and leaving the other inline flips two pool
  pointers and a dependent pair; five c-first spellings all reach 0, the n-first order
  never does.
- func_020316d8 survived ~4300 compiles across 17 lever families with a byte-identical
  div-28 residual and now carries the strongest exclusion list in the DB; treat as
  priced-out pending a genuinely new construct class.

**`func_02072fcc` is a ONE-INSTRUCTION miss and the best hand-fix candidate in the backlog**:
mwcc PRE-hoists a loop-invariant `b+1` whose only use is on a cold retry path into the
preheader (`add r1,r1,#1` + in-loop `mov r3,r1`) where the ROM recomputes `add r3,r1,#1`
in-loop. All 17 "divergences" are that one insn plus the +4 branch-offset ripple. Both models
exhausted the pragma space (opt_loop_invariants / common_subs / propagation / lifetimes /
strength_reduction / dead_assignments, optimization_level 2/3, optimize_for_size), u64
laundering, `&b[1]`, temps, goto/nested loop forms. Do not spend more model time on it.

## 6ag. First-access-fold is a COMPILER-BUILD delta, not a source construct (SetGroundFlag challenge, 2026-07-27)

Ran the WithMeshClsn::SetGroundFlag (0x02035708, size 0x14) challenge to ground: find a
source form that materializes `this+0x10` for the RMW without the u64 launder. Negative
result, now with the mechanism identified:

- ~20 new formulation classes beyond the challenge's original 10, ALL emit the folded
  0x10 form: bitfield set (bit 4), MI second-base member, volatile member, volatile
  OBJECT pointer, inlined arg-pass helpers (C and C++), method-on-embedded-member,
  accessor returning ref/ptr, self-helper with mask arg, register-pinned pointer,
  one-trip loop, statement-expr assignment, late-const index, char* two-step,
  struct-pointer +1 scaling, int-typed address.
- All 24 local compiler builds (1.2 base..sp4, 2.0 base..sp2p4, dsi 1.1..1.6sp2) fold,
  C and C++ front ends both (mwccarm emits Itanium mangling natively, so .cpp method
  candidates extract under the same _ZN name).
- Opt levels -O1..-O4 with ,p/,s all fold; -O0 emits a frame (0x20). The fold lives in
  the L1 core (selection/regalloc), NOT in a toggleable pass: -opt noprop / nocse /
  nolifetimes / nodeadstore / nostrength / noloop / nodeadcode all still fold. The
  opt_* pragmas ARE recognized by mwccarm 1.2 (verified: -w illpragmas warns on a fake
  pragma, stays silent on opt_propagation) and are still irrelevant to this fold.

The positive result is a ROM census (adjacent-word scan of arm9_dec.bin for
ldr/ALU/str with [rB,#imm] folded twice, vs add rB,base,#imm / ldr [rB] / ALU /
str [rB] quads):

- member-base RMW: 95 materialized vs 1 folded, and the single folded site
  (0x02070450) is a pool-loaded GLOBAL counter with a flag-setting subs feeding a
  branch (`if (--n)`), not a this-relative RMW.
- stack-slot RMW ([sp,#imm]): 17 folded, 0 materialized -- both compilers agree there.
- single reads always fold: the whole 0xc getter family (IsOnGround = ldr [r0,#0x10];
  and #0x10; bx lr) beside every 0x14 setter/clearer of the same word. Getters return
  the raw mask (no shift), so the original source was int + mask constants, not
  bitfields.

So the target compiler NEVER folds a member address that feeds both a load and a store:
it keeps the CSE'd address temp. Every mwccarm we have re-folds that temp
unconditionally at O1+. That is the entire first-access-fold "floor": not a missing
source construct, a codegen delta in whichever CW-for-DS build Nintendo actually ran
(a pre-1.2 2004-era release; tools/mwccarm starts at 1.2). That build is now identified
by name and its behaviour confirmed from primary artifacts; see 6ah. Consequences:

- The u64 whole-expression launder (6ac: launder the WHOLE address expression, not the
  base) is the CORRECT canonical compensation; 1,612 src files / 3,142 uses carry it
  today. Do not spend model time hunting formulations for materialized-RMW residues.
- If an earlier CW-for-DS build is ever sourced, expect a mechanical mass de-launder
  across those files, and re-test the base-materialization family (6g) against it.
- Calibration, 6ae-style: bounded mechanism claim, not an impossibility proof. But the
  bound is hard: 95:1 in ROM, 0 reachable across 24 builds x 10 opt configs x 7 -opt
  toggles x ~30 formulations.

## 6ah. The build that compiled the ROM: CodeWarrior for NITRO V0.5-V0.6.1 (2026-07-27)

6ag bounded first-access-fold to a compiler-build delta but could not name the build.
It now has a name and a build-number range, and the fold behaviour is confirmed from
period artifacts rather than inferred. Nintendo's 2003-2004 DS compiler ran a `V0.x`
product numbering under two names, IRIS (the pre-NITRO codename) and NITRO. It is the
SAME product line as everything in tools/mwccarm, just far older.

Read the two version strings carefully, because they are different fields and confusing
them sends you hunting the wrong thing. The CLI banner of the Dec-2003 compiler, from
`docs/_private/CodeWarrior/mwccarm.help.txt` in `irisSDK-20031203.tar.gz` (verified
directly, not secondhand):

```text
Metrowerks C/C++ for Embedded ARM.
Copyright c 2003, Metrowerks Corporation
Version 2.0 build 36 (build 0036)
Runtime Built: Nov 22 2003 11:16:18
```

with the linker at `Version 2.0 build 47`. So the banner says `Embedded ARM` in 2003
exactly as it does in 2005, and the core version numbering is continuous: build 36
(Nov 2003) through build 72 (CW-for-NITRO 1.2, 2005-06-14, our earliest) and on to 87.
Separately, `Metrowerks C/C++ for ARM v1.0a1` is the ELF *producer stamp* written into
`.o`/`.a` files by that era's compiler, not its banner. Both strings appear in the same
object. Do not treat the producer stamp as a product-line tell.

The install paths and producer stamps in each SDK's prebuilt `.a` libraries date the
product line:

| NitroSDK | date | producer stamp | product path |
|---|---|---|---|
| 1.0 | 2004-04-16 | `Metrowerks C/C++ for ARM v1.0a1` | `CodeWarrior for IRIS V0.2`, `CodeWarrior for NITRO V0.3` |
| 2.0rc3 | 2004-12-10 | `Metrowerks C/C++ for ARM v1.0a1` | `CodeWarrior for NITRO V0.6.1` |
| 2.2a | 2005-08-26 | `Metrowerks C/C++ for ARM 2.0.0.73` | (none recorded) |

`2.0.0.73` in the Aug-2005 SDK is core build 73, one past our earliest, so by then the
SDK is already being built by our compiler family. NitroSDK 1.0's release notes name
"CodeWarrior Versions 0.2, 0.3, or 0.4.1"; ReleaseNotes-1.2 adds "NITRO-SDK 1.2 was
created to be used with this V0.5". The era's build system keys off `CWFOLDER_IRIS` /
`CWFolder_NITRO`, and installers are named `CW_NINTENDO_DS_R<ver>.exe`.

The `SDK_CW_BUILD_NUMBER_LD` ladder in the SDK buildtools maps LINKER builds to
products: 0047 = Dec-2003 IRIS base, 0050 = IRIS V0.2 hotfix 1, 0056 = NITRO V0.3,
0057 = V0.4.1, 0058 = V0.5, 0061 = V0.5 hotfix 1, 0062 = V0.6. Dated product history:
V0.1 Nov 2003, V0.2 Dec 2003, V0.3 Mar 2004, V0.5 May 2004, V0.6 Jul 2004, V0.6.1 Aug
2004; by Mar 2005 the install path becomes `CW for NINTENDO DS V1.0.2`, then 1.1
(build 70 era, ~Mar 2005) and 1.2 in June 2005.

The compiler's own changelog settles the dating. `ARM_Compiler_Notes.txt` ships inside
the 1.2sp2p3 zip and carries an unbroken build history from the beginning:

| core | builds | dates |
|---|---|---|
| 1.0a | 0001-0006 | 0001 = 2002-10-02, "first build of ARM compiler" |
| 1.0 | 0007-0021 | 2003-02-19 to 2003-06-19 |
| 2.0 | 0033-0050 | 2003-09-28 to 2004-06-15 |
| 2.0 | **0053-0062** | **2004-09-09 to 2004-12-16** |
| 2.0 | 0063-0066 | Jan 2005, internal builds |
| 2.0 | 0067-0071 | 2005-01-21 to 2005-03-29 |
| 2.0 | 0072 | 2005-05-12, ships in product 1.2 |

**So SM64DS (NA, gamecode ASME, shipped 2004-11-21) was compiled by CodeWarrior for
NITRO V0.5, V0.6 or V0.6.1, whose mwccarm core is 2.0 build 0053-0062.**

One structural fact makes the hunt wider than it looks: NITRO was not a fork. That
single changelog covers one mwccarm lineage which also feeds Metrowerks' public,
non-Nintendo "CodeWarrior for ARM" / "ARM ISA Edition" products. So a public 2004-dated
update from that line should carry a build in the 0053-0062 range, and public products
survive in places Nintendo-confidential ones do not.

Two measurements confirm this is the ROM's compiler:

1. Controlled fold census. Same SDK codebase, three compiler eras, counting RMW sites
   as materialized (`add rA,rB,#imm` then `[rA]`) vs folded (`[rB,#imm]`):
   Apr 2004 / IRIS V0.2 = 64.1% materialized (175 vs 98); Dec 2004 / NITRO V0.6.1 =
   61.8% (754 vs 466); Aug 2005 / build 73 = 26.2% (192 vs 542). The behaviour collapses
   by 2.4x exactly at the 2004-to-2005 changeover, on near-identical source. For
   reference the ROM itself is 226 materialized vs 11 folded, and all 24 local builds
   fold the equivalent C 100% of the time.
2. Link fingerprint. Reloc-free `.text` sections from each SDK's `.a` libraries,
   searched byte-identically inside the ROM images: Apr 2004 hits 46 sections /
   4,472 bytes, Dec 2004 hits 96 / 10,076, Aug 2005 hits 72 / 6,976. The Dec-2004 SDK
   wins on both absolute bytes and hit rate (16.9% of candidates vs 13.1% and 11.0%),
   which puts the game's linked SDK at the V0.6.1 era.

One negative result that saves future effort: there is no lost 2003-era flag to chase.
Diffing build 36's `-help` surface against build 82's, zero options were removed and
essentially none added (`-generic_symbol_names`, plus `-ARM`). The 2003 option set is
the 2005 option set. Whatever changed about address folding changed inside codegen, so
it cannot be recovered by flags on the compilers we have. Obtaining the binary is the
only route. (The Dec-2003 SDK flag set, for reference: `-lang c -proc arm946e -nothumb
-nopic -nopid -interworking -O4 -opt speed -inline on,noauto -msgstyle std -w all -enc
SJIS -char unsigned -stdinc -enum int -stdkeywords off -avoid_strb all,err`. Nintendo's
`-avoid_strb` was renamed `-avoid_byte` in V0.4 and is hidden from `-help` in both.)

Availability, as of 2026-07-27: **no public archive holds any pre-2005 mwccarm.**
Verified independently across archive.org `cw_consoles` and `ninty_curated_sdks`, the
`twlsdk.randommeaninglesscharacters.com` mirror, the Paladin leak, the Pokemon Platinum
leak, and `mid-kid/metroskrew` (oldest patch targets build 72). Every one floors at
CW-DS 1.2 / build 72. What the Paladin leak's IRIS SDK does preserve is the `-help`
dumps quoted above, which is how we know build 36 exists at all. Two known-missing
in-family patches would also be new if found: `cw_ds_1_2_sp2_patch_20050915` (build 80)
and `cw_ds_1_2_sp2_patch2_20050929` (build 81), plus 1.2 SP1, which no collection has.

Two realistic paths to a build in range remain open.

The first is the public product line, which is promising precisely because it was sold
without an NDA. Metrowerks' own public FTP is preserved at archive.org
(`ftp_metrowerks_updates.7z`, 5.5 GB) and its `Metrowerks/CWARM/` folder holds
`CW_ARM_2.1.1_Update.exe`, 24,337,350 bytes, dated 2004-10-20, whose internal file
table lists `mwccarm.exe`. That date lands inside the 0053-0062 window. The file can be
pulled without fetching the whole 5.5 GB: read the 7z start header and end header by
HTTP range, then fetch only the solid block holding it. The blocker is that its payload
is an appended InstallShield cabinet starting at offset 102912 (the PE's own resources
are only 30 KB), which 7-Zip cannot open and which needs `unshield`. Nothing here is
verified until someone extracts it and reads the banner, so treat it as a lead, not a
result. If it does yield a 0053-0062 mwccarm, the first thing to run is the fold probe:
compile `*(u32*)((char*)p+0x154) |= 0x40` and check whether the address materializes.

The second is a leaked or mirrored 2004-2005 DS game source tree, because those commit
the whole CodeWarrior install under `sdk/cw/ARM_Tools/Command_Line_Tools/`. Three such
trees are public today (two copies of `retsam_00jupc` carrying CW-DS 2.0 SP2, and
`yin846/pokemon_dp` carrying 1.2 SP2), which proves the pattern; none is old enough yet.
Useful search hooks: `CWFOLDER_NITRO`, `CWFOLDER_IRIS`, `"CodeWarrior for NITRO V0"`,
`"CW for NINTENDO DS V1.0.2"`, `CW_NINTENDO_DS_R`.

Metrowerks' own roadmap talk from the Nintendo DS Developer Conference, dated
2004-09-30 (`7_Metrowerks.pdf`, "CodeWarrior for NINTENDO DS", Rafael Campana), fills
in the rest of the 2004 line and dates it precisely. As of that talk the shipping IDE
was 5.6, with two releases pending: "Release 0.9 (~Late September 2004)" adding overlay
debugging, a command line debugger interface and batch pre/post linker plugins, then
"Release 1.0 (~Early October 2004)" adding long veneer branch support, watchpoints, a
simple profiler and a cache viewer. So a 1.0 did exist, in October 2004, and the jump
straight to the surviving 1.2 of June 2005 is a gap in the archives rather than in the
version numbering. SM64DS (NA) shipped 2004-11-21, which places its toolchain in the
V0.6.1-to-1.0 window and before 1.2 by a wide margin.

Worth noting for the asm-hatch work: long veneer branch support is a Release 1.0
(Oct 2004) feature, landing right inside the game's build window. That is consistent
with the veneer class being reachable from `#pragma long_calls on` rather than being
hand-written assembly.

Practical effect for matching: nothing changes today. V0.6.1 is not in any public
archive (the surviving mirrors floor at the June-2005 1.2 installer), so 1.2/sp2p3
remains the best available proxy and the u64 whole-expression launder remains the
correct compensation. What changes is the search: anyone hunting the real build should
look for `CodeWarrior for IRIS` / `CodeWarrior for NITRO V0.x` / the banner
`Metrowerks C/C++ for ARM v1.0a1`, not for a "1.0" or "1.1". The 2003-2004 IRIS-era
material is the only place it can still be hiding.

A build-discrimination check worth knowing when reasoning about any of this: on a
26-function sample of matched code, 1.2/base (72), 1.2/sp2 (79) and 1.2/sp2p3 (82) are
completely indistinguishable, all three matching every function. sp3 (84) and sp4 (87)
start to diverge, and the 2.0 line never matches. So "sp2p3" in this repo means "the
72-82 family", and the true compiler sits at or below build 72.

### The SDK's own build flags, and what they rule in or out

NitroSDK ships the makefile fragment it built itself with,
`build/buildtools/commondefs.cctype.CW`. It is worth reading in full because it is the
only surviving record of how Nintendo actually drove this compiler. Highlights:

- `CWFOLDER_IRIS ?= C:/Program Files/Metrowerks/CodeWarrior for NITRO V0.6.1`, with
  `CWFOLDER_NITRO` aliased to it and tools under `ARM_Tools/Command_Line_Tools`.
- It branches on `SDK_CW_BUILD_NUMBER_LD` with the literal values `0050 0056 0057 0058
  0061 0062`. That is the 2004 linker build series, and it sits well below our earliest
  build 72, independently corroborating where the V0.x line falls.
- Per-build workaround switches exist for that era: `SDK_CW_WA_OPT4`,
  `SDK_CW_WA_CONSTPOOLS`, `SDK_CW_WA_OPT_BLX`, plus `CW_AVOID_STRB`, which is
  `-avoid_strb all,noerr` on builds 0050/0056 and `-avoid_byte strb -warn_byte none`
  after. Our 1.2/sp2p3 rejects `-avoid_strb` outright and accepts `-avoid_byte`, another
  small confirmation that `-avoid_strb` belongs to the older builds only.
- The library flag set is `-lang c -proc arm946e -nothumb -nopic -nopid -interworking
  -O4 -inline on,noauto -opt speed -msgstyle std -w all -enc ascii -char signed -stdinc
  -enum int -stdkeywords off -Cpp_exceptions off`.

Two results from testing those against our compiler:

- `-avoid_byte strb` is NOT what the game used, and this is now settled rather than
  assumed. The flag replaces byte stores with `swpb`, and because `swpb` has no offset
  addressing mode it forces exactly the materialized `add rA,rB,#imm` we have been
  chasing: `ldrb r2,[r0,#4] / add r1,r0,#4 / bic r0,r2,#1 / swpb r0,r0,[r1]` instead of
  the folded `ldrb/bic/strb`. Tempting, but the ROM contains zero `swp` or `swpb`
  instructions across arm9, arm7 and every overlay, so the game was not built this way.
  Worth recording that the materialization IS reachable under some flag mode, i.e. it is
  a codegen mode rather than something the compiler structurally cannot express.
- `-inline on,noauto`, `-inline off` and `-stdkeywords off` are all neutral on matched
  code (verified on three arm9 functions of 224/404/744 bytes, all still MATCH). Since
  `-inline on,noauto` is what the SDK actually used and it costs nothing on known-good
  functions, it is a free axis to add to the near-miss sweep, particularly for residues
  suspected of an inlining-decision difference.

## 6ai. RECOVERED: a 2004 mwccarm (build 0056), and the wall falls (2026-07-27)

6ah concluded that obtaining a pre-2005 binary was the only way past first-access-fold,
and that no archive held one. The second half was wrong, in a useful way. NITRO was not
a fork: the same mwccarm core shipped in Metrowerks' public, non-NDA "CodeWarrior for
ARM ISA Edition", and Metrowerks' own FTP is preserved on archive.org. Its October 2004
update carries the compiler.

```text
Metrowerks C/C++ for Embedded ARM.
Copyright (c) 2004, Metrowerks Corporation
Version 2.0 build 56 (build 0056)
Runtime Built: Sep 16 2004 13:20:23
```

`tools/recover_cw2004.py` reproduces it end to end and verifies the hash, downloading
about 24 MB rather than the archive's 5.5 GB (7z keeps its header at the end, so three
range requests locate and pull only the solid block that holds the one file). It then
splits the self-extractor payload and inflates the InstallShield volume in-process, so
nothing third-party is installed and nothing is executed. Artifact: 2,248,704 bytes,
sha1 `8eb0b9653ea1c9a589c3a4399e37e2780059a818`. Install it as `2004/b56`.

**The wall falls.** On the exact probe 6ag called shape-blocked, plain C, no launder:

```arm
add r1, r0, #0x154 / ldr r0, [r1] / orr r0, r0, #0x40 / str r0, [r1] / bx lr
```

That is the ROM's 5-instruction materialized form. Every one of the 24 builds we had
emits the folded 4-instruction version instead, at every opt level, under every source
idiom tried. The difference is the compiler, exactly as 6ag predicted.

Sweeping the 372-row near-miss DB through build 0056 landed **9 matches for free**,
verified with strict reloc checking:

| function | module | size | was |
|---|---|---|---|
| func_ov006_020ce46c | ov006 | 520 | div 6 |
| func_ov006_020c4fa4 | ov006 | 972 | div 8 |
| func_ov075_02116f40 | ov075 | 284 | div 13 |
| func_ov006_020f46ec | ov006 | 236 | div 20 |
| func_ov003_020b0894 | ov003 | 672 | div 55 |
| func_0204322c | arm9 | 92 | size mismatch |
| func_02043288 | arm9 | 92 | size mismatch |
| func_020432e4 | arm9 | 120 | size mismatch |
| func_0204335c | arm9 | 92 | size mismatch |

Spot-checked against all 24 older builds: none of them match any of these. The four
arm9 entries were not close misses, they were the wrong size entirely.

This also breaks a second documented wall. Those four arm9 entries are the
`ActorBase::Process` PTMF wrappers written up further up this file: mwccarm 1.2 forces a
`push {fp,lr}` dynamic frame from every source form tried, giving 0x68 against the ROM's
0x5c, and the note concluded the ROM's fixed-frame staging "is what 2.0 emits". Build
0056 emits that staging directly and all four match on the near-miss C unchanged. The
2.0 reading was the right scent from the wrong direction, since the behaviour belongs to
the era before 1.2 rather than after it. Two unrelated walls falling to one binary is
worth recording because it raises the prior on compiler era over source spelling for any
future floor diagnosis.

**Build 0056 is an ADDITION to the sweep, not a replacement for 1.2/sp2p3.** On an
80-function sample of already-matched code, 78 matched under both, 0 under b56 alone,
and 1 regressed: `func_ov004_020adc1c` (ov004, 32 bytes) where b56 emits 0x24 against
the ROM's 0x20. So b56 is not the game's exact build either. The true build sits
between 56 and 72 and, per 6ah's product ladder and the Sept-Dec 2004 window, is most
likely 0058-0062, i.e. CodeWarrior for NITRO V0.5/V0.6/V0.6.1. Switching wholesale
would break roughly 1% of the corpus; adding b56 to the version sweep costs nothing and
wins the materialized family. The hunt is now much narrower: builds 0057-0062.

Note for CI: the self-hosted validate box needs `2004/b56` installed before any
b56-only match can be committed to src/, otherwise it cannot reproduce them and the
files would land red. Run `tools/recover_cw2004.py` there first.

### 6ai addendum: the 0057-0071 public hunt is EXHAUSTED (2026-08-01)

A systematic enumeration closed the public-recovery route for the builds between 0056
and 0072. Findings, so nobody re-spends this search:

- The Metrowerks FTP `CWARM` folder is capped at the Oct 20 2004 update (build 56)
  across TWO independent archive.org crawl snapshots; the folder's own index.html
  confirms nothing was ever posted after it.
- The `ARM_R1.2.x` updates (Dec 2002) are IDE patch cabs with no compiler, ~2 years
  too old regardless.
- CodeWarrior for Symbian (3.1 full installer AND the 3.1.1 update) ships NO mwccarm:
  both cab volumes were carved 100% -- it is an IDE/debugger front end driving ARM GCC
  via XML adapters, same as the S60 SDK (arm-epoc-pe-gcc). The era's Symbian products
  are not mwccarm carriers at all.
- The NitroSDK 3.0 leak's cw.zip is Jan/Apr 2006, i.e. build 72+, redundant with
  1.2/sp2p3.
- The dsi/ TWL builds (1.1-1.6sp2, nine of them) were swept against the floors the
  same day: all structurally wrong-era (smaller code, 999 everywhere).

Remaining routes are all non-public: a CodeWarrior for NITRO V0.5-V0.6.1 SDK leak
surfacing, or NDA-era Nintendo/Metrowerks archives. Until one appears, the five arm9
floors (InitResources 4, OAM::Render 2, 0202ffec 2, LoadTex 5, `func_02009e70` 96) are
at their true resting state: every source-side axis is closed with evidence in the
near-miss DB.

## 6aj. Uninit-decl split: vreg birth order follows the DECLARATION LINE, not first assignment (2026-07-27, Opus→Fable on func_ov007_020c3fe4)

The decl-order rules (2, 6e, 6k, 6q) all assume declarations carry initializers, which
locks two levers together: the declaration line IS the first definition, so coloring
order and evaluation order move as one. They are separable. Declare the locals
*uninitialized* in one block, then assign in a second block, and the allocator ranks
the webs by the DECLARATION line while the code still evaluates in assignment order.

The case that proved it: func_ov007_020c3fe4 (ov007 0xac), a div-8 near-miss where the
ROM kept a sub-object pointer `p` in scratch r2 and a shifted value `y` in callee-saved
r5, and every initialized-decl spelling had them swapped. All 120 permutations of the
initialized decls were swept — div 8 was the unique floor, and 9,107 permuter
iterations never improved it, because with `int y = p->y >> 12;` the y-web cannot be
born before the p-web that feeds it. Splitting:

    s32 y; s32 m; s32 x; s32 vis; s32 onX; s32 negM; struct Inner *p; s32 f;
    p = self->inner;
    ...original statement order...

hands short-lived `y` (declared first) the r5 slot and late-declared long-lived `p` the
scratch r2, without perturbing any of the 35 already-matching instructions → div 0 on
1.2/sp2p3. Three of six tested orders matched (y-first with p late in all of them), so
the constraint is the y-before-p rank relation, not one magic permutation. ~24 compiles
total, after u64-laundering (6h), pragmas (6f/6aa), escape-aliasing and shift respells
(6ab/6ac) all stalled at div 8.

Rule of thumb: when a residual is a pure 2-register swap between a value and the
pointer/base it derives from, initialized decls CANNOT reach the ROM's coloring if the
ROM births the derived value's web first. Try the uninit-decl split before calling it a
wall — it reaches decl-order × assignment-order combinations the initialized form
structurally cannot express. (C89 style keeps this legal in plain C.)

Two same-day floors for contrast, both banked in nearmiss/db.jsonl with evidence:

- **func_ov075_0211a948 (div 5): a two-attractor rank-pin.** Every spelling of
  `tex>>3 | fmt<<26 | ...` renormalizes to attractor A (ROM's instruction selection,
  swapped coloring, div 5) or attractor B (`fmt`-term-first: ROM's coloring, wrong
  selection — `tex,lsr #3` folds into the orr and `lsl #26` materializes, div 7). The
  ROM is A's selection with B's coloring; fold-side and color-rank ride the same
  canonical tree and ~46 compiles across 6aa/6ab/6ac/6h found no construct that
  decouples them. The uninit-decl split does NOT help here — the swap is inside one
  expression tree, not between two statements. Open angle: anything that changes
  pre-RA operand evaluation order of `|` without reordering the tree.
- **func_ov006_020dbe9c (div 21): pooled-base web with exactly two reachable colors.**
  The pool base of `data_02082214` colors `ip` un-laundered and `r6` laundered (6h
  moves the class, as documented) — but the ROM wants `r4`, and all 21 divergences are
  a forced cascade of that one choice. Laundering rotating the color without reaching
  the target is the tell that the web's rank class is pinned (6ab), not that more
  laundering variants are worth grinding.

## 6ak. Batch levers from the 2026-07-27 overlay batches (32+ matches, ov002/ov006/ov007/ov079)

Rules proven byte-exact during the July 27 overlay campaign (PRs #768/#770/#771 and the
follow-on batch). Each was load-bearing in at least one match; function addresses cited.

### Expression spelling is not CSE-neutral

- **`i++` vs `i = i + 1` are different to the optimizer.** Swapping one for the other
  adds/removes a temp and can change coloring downstream. If a loop tail has one
  spurious `mov`, try the other spelling before anything exotic.
- **`ok++` vs `ok = 1` change spill behavior** for a flag that is only ever tested
  non-zero: the increment keeps the variable live through the loop (it reads it) and can
  force a spill the constant store avoids - and vice versa when the ROM wants the reload.
- **`+` association order selects the addressing mode.** `base + OFF + i*N` makes the
  compiler fold `base+OFF` first (pooled base, then reg+reg scaled access:
  `add r0, rB, rOff; ldr [r0, r1, lsl #N]`), while `base + i*N + OFF` folds the scaled
  index first and emits an immediate-offset access (`add r0, rB, r1, lsl #N;
  ldr [r0, #OFF]`). Pick per the ROM's form, field by field.
- **Switch case SOURCE order is a block-layout lever.** mwccarm emits case bodies in
  source order (the jump table stays sorted). When block layout is off but the table is
  right, permute the case bodies.
- **A `j = 0;` init can be moved across an if-converted clamp** to change which EBB it
  schedules into - one-slot hoist/sink residuals around a `cmp/movlt/movge` cluster are
  often just this.

### Laundering refinements (extends 6h)

- The u64-mask launder works **inside a predicated block**: the pool materialization it
  forces merges with the if-conversion, giving `ldrne rX, [pc, #..]`-style sequences a
  plain spelling can't reach.
- The launder **blocks store-address refolding where `volatile` does not**: `volatile`
  only pins the access itself; the compiler can still refold the address computation
  onto a CSE'd base. The launder pins the address expression.
- **Timer/counter fields read with mixed signedness match mixed loads**: in
  func_ov006_02103d78 the same u16 field is tested `!= 0` and decremented as `u16`
  (ldrh) but compared `<= 0` as `s16` (ldrsh). Spell each access with the exact type
  the ROM's load implies; the "one declared type per field" instinct is wrong here.
- **Two textually different launder macros create two CSE base classes**
  (func_ov006_0212a654): the ROM kept two copies of the same array base, each serving a
  different subset of written fields. A single launder macro CSEs every laundered
  address onto one base and is permanently one instruction short of the ROM. Two
  spellings that differ only textually - e.g. `(int)M(i) * 32` vs `(unsigned)i * 32` -
  are NOT CSE'd together, and each group gets its own base copy. General rule: the
  optimizer's value-numbering is syntactic enough that textual variance is a handle for
  controlling CSE class membership.

### Aliasing and ABI shape

- **Declare a global as a plain pointer, not an array, when the ROM reloads it after a
  store** (func_ov007_020b6d40): `extern char *g;` can alias the pointed-to storage, so
  a following store forces a reload of `[rG]`; `extern char *g[];` lets the compiler
  prove the base immutable and CSE the load away.
- **Route a call result through a named temp to reorder pool loads across the `bl`**
  (func_ov007_020b6d40): `*(int *)(g + 0x74) = f(...)` may materialize the pool base
  before the call; `tmp = f(...); *(int *)(g + 0x74) = tmp;` moves it after.
- **A cast on a store base breaks pointer-CSE for the tail** (func_ov006_020fd17c):
  spelling the last store's address off `(unsigned int)c` instead of `c` stops it
  sharing the function-wide `c + off` base, so the compiler recomputes the address at
  the tail - freeing a callee-saved register and removing a spill.
- **The return type is a coloring lever** (func_ov007_020b7138): a tail that is a pure
  r0-vs-rN swap - and survives every decl-order/pragma/permuter attack - can be the ABI
  pinning the returned value to r0. The worklist's inferred `void` was wrong; making the
  function `int` and returning the constant it stores flipped the whole tail coloring.
  Rule: when the last value materialized is a constant that also gets stored, try
  returning it. Callee signatures from the worklist are hints, not truth (same lesson
  as the `_ZThn80_` thunks).
- **In-loop redefinition from a const-qualified load defeats preheader pair-promotion**
  (func_ov006_02119c74): mwccarm promotes a loop-invariant multiply operand pair into
  the preheader unconditionally, replacing an in-loop `smull` with a hoisted one.
  Reloading the operand each iteration from a `const`-qualified source makes it
  formally loop-variant (the const qualifier keeps the value provably unchanged, so
  semantics hold) and the `smull` stays inside the loop.

### Late-batch additions (same campaign, batch 4)

- **Partially-volatile struct beats the volatile object** (func_ov006_02126948, Fable):
  `struct V3 { int x, y; volatile int z; };` on a PLAIN local keeps all three member
  stores alive (the volatile member anchors the object) while the non-volatile members
  escape the volatile-object canonical schedule - giving the ROM's naive uncoalesced
  store group that ~30 volatile-object/launder/pragma variants could not reach. Paired
  with `#pragma opt_lifetimes off` to collapse the frame. Rule: volatility per MEMBER is
  a scheduling dial, not just an access pin.
- **Callee-saved coloring follows the LOOP DEPTH of a definition, not decl order**
  (func_ov006_021027e4): all 720 decl-order permutations were inert while moving a
  definition between nesting depths rotated {r4,r5,r6} cyclically. Companion trick: a
  parameter self-assignment inside the loop (`a1 = yb + j`) with the invariant sourced
  from an untouched copy forces the entry `mov` to survive copy propagation.
- **`a *= -1` is not respellable** (func_ov079_02124008): only the compound-assignment
  form emits the ROM's `mvnmi` + `smulbb` + `lsl/asr` truncation quartet; every
  arithmetically equal spelling folds to `rsb`.
- **Constant-def hoisting out of loops is GLOBAL REGALLOC, not LICM**
  (func_ov006_020ee994, wall): pool-address `ldr`s and `mov #imm` defs are placed at the
  dominating point by the allocator at every opt level >= 1 in all 25 builds, immune to
  every pragma/flag. Only `#pragma optimization_level 0` (function-granular, kills
  CSE/sched too) or an irreducible CFG (a live second entry into the loop -> local
  allocation, costs exactly the guard's 2 instructions) suppress it. A ROM loop with
  full CSE/scheduling but zero constant hoisting and free callee-saved regs is a
  fingerprint of a compiler build we do not have (or hand-scheduling) - stop grinding
  and bank it.

### Second b56-only function confirmed (extends 6ai / section 9)

func_ov002_020dec70 stages a by-value `Vector3_16f` argument **below sp with no sp
adjustment** (`sub r3, sp, #8` ... `ldm r3, {r1,r2}`, never touching sp). Every 1.2 and
2.0 build instead emits the `sub ip,sp,#8; mov sp,ip; ... add sp,ip,#8` dance and pins
fp, cascading the whole function (verified across 11 builds x ~6 flag combos x 9 source
spellings). `2004/b56` emits the ROM form verbatim. This is the same fixed-frame-staging
fingerprint as 6ai's `func_ov006_020f0bf0`, now in a second overlay - by-value small-
struct args below sp are shaping up to be THE b56-era detector. Two other levers were
needed under b56 and are portable: a single `||` chain so all gates share one `return 0`
block, and the biased field value held in a temp across calls.

### Third instance, and how to READ the fingerprint off the ROM (func_0206de14, #785)

Confirms the detector in arm9. `func_0206de14` forwards its own arguments to a dispatch
handler and stages a by-value pair below sp exactly as above; b56 emits it verbatim,
every 1.2/2.0 build adds `mov sp, r2` / `add sp, r2, #4` and pins fp, costing 12 bytes.

What this one adds is the diagnostic, because here the by-value struct was **not known in
advance** - the banked draft modelled the arguments as five ints forwarded through `&a0`,
and read as a "stale draft" that merely needed re-deriving. The tell is in the call setup:

```arm
sub r2, sp, #4      str r1, [r2]      str r0, [r2, #4]      ldm r2, {r3}
add r1, sp, #0x1c   ldm r1, {r1, r2}
```

An `ldm` **at a call site** is an aggregate argument copy, not an ordinary load. Two
consecutive incoming slots read with one `ldm` into consecutive argument registers is an
8-byte struct passed by value. If one is written to `[sp-4]` and `[sp]` and then partly
read back, that struct straddles the r3-to-stack boundary and the compiler is materialising
it contiguously across that edge. Counting the argument words as separate ints reproduces
the same *values* but emits plain per-slot loads and comes out short. So:

**A near-miss that is short by a few instructions around a call, where the ROM uses `ldm`
or `stm`, is an argument-SHAPE bug, not a divergence to grind.** Recover the signature
first. Size misses in general deserve a shape review before a lever sweep.

Process corollary worth more than the lever: this function had already been swept against
b56 and recorded as a negative. The sweep fed it the wrong-shape draft, so it never put the
real function in front of the compiler. **A version sweep is only evidence about the source
you feed it.** Re-derive any draft that does not reproduce before believing a build sweep,
and treat "compiler X does not help here" as scoped to the draft that was tested.

---

*Add to this file whenever you learn a new codegen rule. It is the project's accumulating
model of mwccarm - the cheaper, local alternative to disassembling the compiler itself.*

## 6al. arm9 endgame batch 3 (2026-07-28): the TU language, the local's TYPE, and the stack-slot order

Four levers from the fan-out over the last arm9 residue. Two are new tells for reading a
disassembly, one is a measured layout rule, one refines the launder.

### A materialized bool inside a condition chain means the TU is C++

The ROM materializes `data_0209f2d8 == 1` as `cmp / moveq / movne / cmp / beq` - it builds
the boolean into a register and *then* tests it, instead of branching on the first compare.
In **C mode mwccarm folds that away under every spelling**: an `int` temp, `!!x`, a ternary,
the comma operator, `(g == 1) == 1`, a `static` helper function. In **C++ mode a
`static inline` predicate is inlined and its result is materialized exactly as the ROM has
it**, while plain comparisons in the same translation unit still fold normally.

So a materialized bool sitting inside an `&&` chain is a tell about two things at once:
the file is C++, and the condition came from an inlined accessor rather than being written
out. Found on `func_02009e70` `(Camera::Update)`; it was worth ~1300 divergences on its own.

### The TYPE of a named local selects its callee-saved rank

On func_0204a730 a byte-valued web was stuck in r4 across ~35 variants and read as
rank-pinned. It is not. It is pinned only while it is a *compiler temp* or a *non-`u32`
named local*. Hoisting the value manually into `u32 bb;` at **function scope** drops it to
the bottom of the callee-saved order (byte -> r7, matching the ROM) and rotates three other
webs up into r4/r5/r6.

`u8`, `u16`, `s32` and `int` for the same variable all leave it at r4. The type is
load-bearing on its own, independently of declaration position. This is a different knob
from 6aj (declaration line controls birth order) - here birth order is fixed and the
*rank class* moves.

Companion move on the same function: relocating `s32 k` from function scope into the loop
block **after** an existing pointer local promoted the counter over the node pointer. The
two edits only work together; each alone regresses.

### Measured stack-slot layout order

Useful whenever a residual is a `[sp,#N]` that will not move. mwccarm 1.2/\* and 2004/b56
lay the frame out in this order:

1. named scalars, in declaration order
2. compiler temps
3. address-taken aggregates
4. `volatile`s last, dropped into the alignment hole

Corollary that costs time if you do not know it: **single-member structs, unions and
1-element arrays are scalarized** back into group 1, so none of them are a way to reach a
late slot. On func_0204a730 the ROM's word sits in the hole at `[sp,#0x2c]`; a
non-volatile, non-address-taken scalar always lands at `0x1c` no matter where it is
declared, and every construct that does reach `0x2c` re-folds the shift pair the ROM
keeps. Conjecture: the fold is blocked exactly when the load can be value-numbered, which
requires the non-address-taken non-volatile class, which structurally cannot reach the
hole.

### The launder alone loses to CSE across a whole function; pair it with `opt_common_subs off`

When the ROM re-materializes the same address (`add rX, self, #0x154`) at each of many RMW
sites, the u64 launder is **not** sufficient: mwcc value-numbers the laundered address into
one callee-saved register for the entire function. All four launder spellings (`&`, `|`,
`^`, `(long long)`) collapse to the same value number. `#pragma opt_common_subs off` is
what keeps the sites distinct.

The pragma then costs three CSEs the ROM *does* have - restore those with named temps.
Pragma off plus selective named temps is the general shape, not pragma off alone.

Second launder note, from `func_020316d8`: **the launder defeats mwcc's range-folder.**
`short w = <byte-valued expr>;` gets range-folded and both shifts of the `lsl #16 / asr #16`
pair vanish; `(short)(int)LAUNDER(expr)` restores the packed form and the correct size. It
did not pay off there on divergence, but the range-folder is beatable and that is worth
knowing.

### Read register ROLES, not the divergence count, to order two residues

`func_020316d8` looked like two independent residue classes. Mapping which web owns which
register showed one is downstream of the other: the free-register *pools* differ only
because `rowstep` lands in `lr` for us and `r3` for the ROM, and everything in the second
cluster is a cascade of that. A divergence count cannot see this; a role map can. Same
technique found that on `func_02009e70` a single assignment (`self -> r8`) accounts for 145
of 282 differing words, which turns "302 divergences" into "one 4-cycle on
{r6,r8,sb,sl}".


## 6am. The two biggest arm9 functions: measured lever weights (2026-07-30, Opus)

Neither matched, but both moved a long way and every lever below was isolated by
building the variant WITHOUT it and re-verifying, so the numbers are measured
rather than attributed.

### func_02009e70 (Camera::Update, 0x109c): 302 -> 183

Two levers carry the whole distance, and one of them is worth more than everything
else in this file combined.

| variant | divergences |
|---|---|
| seed as banked | 302 |
| booster removed, pragma kept | 302 |
| pragma removed | 816 |
| both | 183 |

**`#pragma opt_common_subs off` is load-bearing here: -632.** Without it the frame
is also wrong (0x1a4). On a function this size, CSE across the whole body is the
dominant effect; check this BEFORE grinding registers.

**The web-priority booster is worth -118 on its own.** Writing `sb = sb ? sb : sb;`
immediately after the `Math_Function_0203b14c(self+0x130)` call makes `sb` claim r9,
which pushes `self` down into r8 and matches the ROM's `mov r8, r0` entry. This is
the 6y lever-1 shape; what is new is the placement rule - the boost has to sit at
the point where the competing web is still live, not at the top of the function.

A 2700-candidate booster sweep found no SECOND booster that helps, and decl-order
permutation (52 tried), scope depth, per-web splits, 98 adjacent-statement swaps and
volatile pinning all failed to move 184->183. The residue is a three-way
r8/sb/sl rotation, which is the rank-pinned web shape from 6ab.

### func_02072168 (VM interpreter, 0x880): 446 -> 195

**A dense switch was NOT the problem** - worth recording as a negative result,
because it looks like the obvious first move on a 20-case interpreter. mwccarm had
already emitted `addls pc, pc, r0, lsl #2` plus 20 branch entries from the seed's
switch, including the two default entries. Do not spend a pass on this.

The real causes, in order of weight:

- **A semantic bug in the seed.** Case 0x12 stored `q+4` after already doing `q += 4`,
  a double advance the ROM does not do. It also forced a register spill into `sp+4`,
  which shifted EVERY later local slot by 4 and cost roughly 90 divergences on its own.
  Fixing one wrong store was worth far more than any codegen lever here.
- **Memory-homing a local with `volatile`: -79.** Case 0x10's `k` is memory-homed in the
  ROM (`str r2,[sp,#0x84]` / `ldr r3,[sp,#0x84]` around the third parse call).
  Declaring it `volatile int k;` reproduces that, and because slot 0x84 is then
  occupied, every later slot falls back into ROM alignment. This is the frame-layout
  lever from 6al seen from the other side: you can place a slot by forcing a home, not
  only by moving a declaration.
- **A named advance variable stops mwcc sinking the increment.** Replacing `q += 4` with
  a `unsigned char *adv;` declared in the case block and assigned right after the
  function-pointer assembly keeps `q` in r0 for the `ldrb` chain and lands
  `add rX, r0, #4` early, exactly like the ROM.

Rejected with evidence: `#pragma opt_propagation off` DOES produce the ROM's early
`and rX,r5,#K` + `cmp rX,#0` flag shape, but it recolours globally (op r5->r4,
neg1 r4->r7) and lands at 376. Decl-order permutation does not recover the colouring.

## 9a. CW try/catch EH IS reproducible from //cpp, and the 0x0207xxxx runtime "functions" are split-symbol fragments (2026-07-26, Fable)

Two structural discoveries from the asm-hatched MSL C++ runtime cluster at arm9
0x02071644-0x0207359c. Both mean symbol-map fixes, not codegen levers.

**(1) `//cpp` + `try`/`catch` compiles with exceptions ON under project flags** (no
`-Cpp_exceptions` flag is passed, and mwccarm's C++ default is on). It reproduces the whole
CW EH codegen vocabulary byte-exactly at 1.2/sp2p3: the EH frame (`sub sp,#0x18*records;
mov fp,sp`), the try-entry sp spill (`str sp,[fp,#0x14]` = record0+0x14), catch islands
placed after the function epilogue, the implicit catch-exit unregister call
(`add r0,fp,#0; bl __unregister`, reloc-resolving to func_02071ba0), nested try records at
fp+0x18 (sp spill at fp+0x2c), and rethrow via a plain call that lands on func_020717c0.
Section 5's "compile as C++" rule extends to EH: never try to fake these frames from C.

**(2) The catch islands were carved into separate symbols** (func_020732e8, func_0207335c,
func_02073584) even though they are the SAME compiled function as their parents
(func_0207328c, func_02073300, func_02073534 - MSL array-destroy helpers,
`__destroy_arr`-family). Matching the COMBINED region (parent size + island size) from
natural MSL-style C++ gives:

- func_0207328c+020732e8 (0x74): 29/30, byte-exact except ONE extra dead
  `ldr sp,[fp,#0x14]` before the outer epilogue.
- func_02073300+0207335c (0xa8): 42/43, same single dead reload - the nested try, the
  destroy-remaining loop, the mid-island `ldr sp,[fp,#0x2c]` and the rethrow call ALL match.
- func_02073534+02073584 (0x68): 26/27, same single dead reload.

Winning source shape (0207328c; 02073300 adds the nested
`try { if (--n) do {...} while (--n); } catch (...) { func_020731fc(); } func_020717c0();`
inside the catch; 02073534 is `while (current > base) { current -= size; dtor(current); }`
in the try, no dtor null-check):

```cpp
//cpp
extern "C" void func_020731fc(void);   /* terminate()-style wrapper; source-level call */
typedef void (*dtor_t)(void *);
extern "C" void func_0207328c(void *block, unsigned int n, unsigned int size, dtor_t dtor)
{
    if (dtor) {
        char *p = (char *)block + n * size;
        try {
            if (n) { do { p -= size; dtor(p); } while (--n); }
        } catch (...) {
            func_020731fc();   /* emits the island's leading bl */
        }
    }
}
```

The single residual is systematic: every 1.2-family compiler (base/sp2/sp2p3) emits a dead
`ldr sp,[fp,#0x14]` when an outer catch falls into the function epilogue; the ROM build
omits it (2.0-family does omit it but diverges everywhere else: pop-pc epilogues, no
if-converted early returns). `__attribute__((noreturn))` on the catch call deletes TOO MUCH
(the unregister call as well); optimize_for_size restructures the whole function. Section-9
verdict: prebuilt MSL library object, one peephole newer than 1.2/sp2p3. The committed asm
hatches stay; if the symbol map ever merges the islands into their parents, these //cpp
sources are 1 instruction away and the drafts are the place to start.

**(3) Two more split-symbol proofs.** func_020729e8 (0xc, bare `add sp,#0xac; pop; bx`) is
the severed epilogue of func_02072168 (0x880, ends exactly at 0x020729e8) - unmatchable
standalone by construction. And func_02071644's asm-hatch "regperm floor" claim is WRONG:
the symbol map truncated it. Its dead trailing `bx lr` (mwccarm always appends one after a
`for(;;)` whose exits are all early returns) was carved off as "func_02071694", currently
"matched" as an empty function. Real C oracle-MATCHES the true 0x54 extent:

```c
void func_02071644(void *obj, int len) {   /* decimal digit-string increment w/ carry */
    unsigned char *first = (unsigned char *)obj + 5;
    unsigned char *p = first + len - 1;
    unsigned char d;
    for (;; *p-- = 0) {
        d = *p;
        if (d < 9) { *p = (unsigned char)(d + 1); return; }
        if (p == first) {
            *p = 1;
            *(s16 *)(((long long)(int)((char *)obj + 2)) & 0xffffffffffffffffLL) += 1;
            return;
        }
    }
}
```

(6g launder forces the ROM's `add r1,r0,#2` materialized halfword RMW; the trailing dead
`bx lr` is byte 0x50-0x53.) Rule: when a tiny symbol is a bare epilogue, a lone `bx lr`, or
a catch-island shape (starts mid-frame, uses fp/r4-r7 it never set), check whether the
PREVIOUS symbol's compiled form simply extends over it before believing any floor label.

## 6an. Signedness is a coloring lever separable from condition codes (2026-07-30, GX::LoadTex)

GX::LoadTex sat at div=16 as a pure r4/r5 web-identity swap (`base`->r4/`top`->r5 vs the
ROM's r5/r4) with both locals typed `unsigned int`. Retyping BOTH as plain `int` flips the
allocator's rank assignment and lands the ROM's coloring (16->7). Signed vs unsigned is a
RANK input like the local's width (6al): int and unsigned int locals do not rank equally.

The catch and the second half of the lever: the retype drags the COMPARISONS signed
(`lt/ge` where the ROM has `lo/hs`, 7 residual divs). Do NOT fix that with `(int)` casts
on the operands - fix it by leaving the comparisons in mixed signed/unsigned form so C's
usual arithmetic conversions promote them back to unsigned (`int top` vs `unsigned offset`
compares unsigned, emitting lo/hs) while the LOCALS keep their signed rank. Coloring reads
the declared type; the condition code reads the promoted comparison type. The two are
independently steerable (7->0, byte-identical, 1.2/sp2p3).

Scope check (same night): a mechanical signedness sweep (singles, pairs, all-flip; 581
compiles) across the seven other stuck arm9 residues (0202ffec, CapEnemy::GetCapState,
Stage::InitResources, 0204a730, 020412f0, Stage::PS_UpdateOkAndBackButtons, 02038824)
moved NOTHING. The lever fires when the residue is a two-local callee-saved web-identity
swap; it does not perturb scheduling knots or wider coloring webs. Sweep script pattern:
flip (unsigned int|int|u32|s32) decls in the bank draft, abverify each.

## 6ao. Flag-across-call rematerialization, and the for(;;) dead epilogue (2026-07-30, func_02072168)

Two finds from the deep pass on the WM bytecode dispatcher (both currently OPEN, no
counter-lever known):

1. **mwccarm refuses to hold a cheap flag live across calls.** The ROM computes
`and rX, op, #mask` into a callee-saved register BEFORE two `bl`s and does a bare
`cmp rX, #0` after. Every mwccarm spelling instead REMATERIALIZES (`ands r1, op, #mask`)
at the use site - the inverse of the usual too-much-CSE problem. Inert or worse:
`volatile int` (stack spill, wrong shape), `unsigned char` (spurious narrowing AND),
`(op & mask) != 0` bool cast (much worse), statement reordering,
`#pragma opt_common_subs off` (regressed), `#pragma opt_propagation off` (regressed).
The heuristic looks like remat-if-cheaper-than-spill with no source knob found yet. It
recurs in ~10 of the dispatcher's 20 cases and dominates its div=195. Any future crack
of this pattern unlocks func_02072168 nearly whole.

2. **`for(;;)` containing a predicated early `return` emits a dead unreachable trailing
epilogue** (20-byte minimal repro). `while(1)` and goto-loop forms behave identically;
`#pragma optimize_for_size on` converts the predicated return to a branch-to-shared-tail
(reachable, but not the ROM's predicated shape). If a target ROM function has NO dead
epilogue but ours does, the ROM's early-exit idiom differs from a plain in-loop return -
treat the dead-epilogue delta as a diagnostic fingerprint, not noise. (Related: 6al's
trailing-bx-lr note - mwccarm appends one after a for(;;) whose exits are all early
returns; that one the ROM DOES keep.)


## 6ap. Volatile-store/plain-read split: place a scalar in the volatile hole WITHOUT losing value numbering (2026-07-30, func_0204a730 MATCHED)

The 6al stack-layout rule (named scalars in decl order -> temps -> address-taken
aggregates -> volatiles into the trailing hole) had a corollary conjecture: a scalar
that must sit in the hole must be declared volatile, and volatile ACCESS kills value
numbering, re-folding shifted-use chains. WRONG CONCLUSION - placement and access
volatility are separable, same shape as 6an's coloring-vs-condition-code split:

    u32 flags;                                  /* plain object */
    *(volatile u32 *)&flags = *(u32 *)player;   /* the ONE write, through a volatile cast */
    ... ((u32)(flags << K) >> 0x1f) ...          /* all reads plain */

The volatile-cast WRITE marks the object volatile-touched for frame layout (it lands
in the hole at [sp,#0x2c], after the aggregates), while every read stays a plain
value-numberable load, so the ROM's lsl,lsl,lsr,str,lsr,str three-live-values shape
survives. The mirror split (volatile decl, cast-away-volatile reads) does NOT work -
mwcc tracks the object's declared volatility for VN, so the fold still dies; and
(void)&flags address-taking moves the slot but breaks 34 words elsewhere. Landed
func_0204a730 byte-identical together with the banked group-B recipe (u32 byte-hoist
at function scope + s32 k demoted into the loop block after var_r5).
## 6aq. Booster direction on CALLER-saved pairs is inverted, and it must ride the BASE (2026-07-31, func_020412f0 MATCHED)

Closing `func_020412f0` (5 -> 0 on 2004/b56) refines 6y lever 1. That lever was measured on
callee-saved webs, where boosting a value pulls it toward the LOWER-numbered register. On a
caller-saved r2/r3 pair the booster pushes the boosted web toward the HIGHER-numbered
register instead. Reading 6y's direction across to scratch registers costs a whole sweep:
a 20-variant hand pass and a first agent pass both boosted the wrong web and stalled at 5.

**Shape.** A byte-triple store into a pooled global, `data_020a2409[r6] = b9; d9[1] = ba;
d9[2] = bb;`. ROM holds the pool base in r3 and the derived `base + r6` in r2, and gives the
LOWER register to the LONGER-lived derived pointer. Every plain spelling inverts both.

**Fix.** Three conditions must hold simultaneously; any one alone is inert:

```c
u8 *b9b = data_020a2409;      /* 1. base is a NAMED local, declared FIRST */
u8 *d9  = b9b + r6;           /* 2. derived comes from the LOCAL, not the symbol again */
u8 b9v = ((volatile Msg *)m)->b9;
u8 bav = ((volatile Msg *)m)->ba;
b9b = b9b ? b9b : b9b;        /* 3. booster on the BASE (the SHORTER-lived web) */
b9b[r6] = b9v;
d9[1] = bav;
d9[2] = m->bb;
```

Boost the shorter-lived web to push it UP; the longer-lived one then falls into the lower
register. Also improves 1.2/base|sp2|sp2p3 from 12 to 7, so it is a better source shape, not
a b56 artifact.

**Inert on this shape (stayed at exactly 5):** booster on the DERIVED pointer at three
placements (after its def, after the first store, doubled); the named base hoisted to the
function-scope decl list; block-scope decl order in six orderings; a named base declared
after the derived; `&arr[i]` vs `arr + i`; dropping the volatile temps; the 6h u64 address
launder on the base, on both pooled bases, and on the derived; `opt_lifetimes off`;
`scheduling off`.

**Actively harmful here (record as regressions, not neutrals):** derived declared in a
deeper nested block than the base store (14 div, rotates r0-r3 across the whole window); the
6w comma-operator serializing the derived's def into its first use (14 div, same rotation);
6q identity reuse of an existing function-scope pointer as the derived (50 div, leaks out of
the case and rotates r4/r5/r6 across an unrelated tail).

**Process note.** The near-miss DB stores ONE divergence number per row with no record of
which compiler produced it. This function sat at "12" for weeks because that was its 1.2
score; it was 5 on b56 the whole time. Sweep `--all` before believing any row's number, and
before calling anything a floor.

## 6ar. A LICM-hoisted address local colors by its DECLARED TYPE, not by ordering (2026-07-31, func_02038824 MATCHED)

Closing `func_02038824` (20 -> 0 on 2004/b56, and 25 -> 5 on the whole 1.2 line). The
residual was one r4/r5 two-web swap plus a 9-word prologue schedule cluster that rides on
it: ROM colors the hoisted `pos` (`self + 0x3c`) r5 and the shared distance-temp/flags web
r4, and every plain spelling mirrors both.

**The lever is the local's declared TYPE:**

```c
int pos = (int)self + 0x3c;              /* matches */
...
if (Vec3_Dist((void*)pos, &v) > r5 + limit) continue;
```

versus the natural `char *pos = self + 0x3c;`. This is NOT a cast-on-the-expression
effect. Keeping a pointer local and moving the arithmetic inside it,
`char *pos = (char *)((int)self + 0x3c);`, only reaches 12. The int-typed local is what
flips the pair.

**Why every ordering lever was inert.** `pos` is LICM-hoisted out of the loop, and a
hoisted value's web is pinned to the rank of its ORIGINAL scope. It cannot be pushed below
an in-loop temp by declaration order or scope depth. Callee-saved rank on this function
otherwise tracks source order of first definition, descending (`limit` r8 > `i` r7 >
`o` r6 > `pos` r5 > temp r4). When a two-web swap involves a loop-invariant that mwcc
hoists, skip the ordering family entirely and go to the type of the carrying local.

**Inert here (output bitwise identical at 20):** the entire 6y/6aq booster family on `pos`,
on the temp, and on `flags`, at after-def and before-use placements; deeper scope for
`pos`; the temp at loop scope after `sl`, as decl-with-initializer, and merged with `flags`
(3 decl positions); `pos` declared after the temp's first assignment (the front end
canonicalizes it away); `pos` retyped `struct Vec3 *`, `const char *`, `unsigned char *`;
`&self[0x3c]` vs `self + 0x3c`.

**Regressive:** `pos` at loop top or via a laundered loop-top copy (30); `pos` uninit at
function scope assigned deep (38); `pos` at function top (39); temp at loop top before `o`
(27); `opt_propagation off` (98). **Size-breaking (0x210 vs 0x214):** inlining
`self + 0x3c` into the call, or a tight nested block around def and use, both of which lose
the hoist.

## 6as. TOOLING TRAP: mwccarm silently ignores unknown pragmas, so a typo reads as "inert"

**Verified directly 2026-07-31.** Adding `#pragma zzz_not_a_real_pragma off` to a MATCHING
source compiles clean, emits no diagnostic, and produces BYTE-IDENTICAL output -- the
function still matches. A real pragma on the same source (`opt_propagation off`) moves 105
words. mwccarm accepts any `#pragma <identifier>` and drops the ones it does not know.

**Consequence:** every "pragma X is inert" line in this file is only evidence if X is a
real mwccarm pragma. A misspelled or invented pragma name is indistinguishable from a
genuinely inert one in a divergence sweep, and it will be recorded as a tried-and-failed
lever that nobody re-tries.

On `func_02038824` this bit a whole sweep: `scheduling`, `register_coloring`,
`global_optimizer` and `peephole` all returned the baseline number and were logged as
inert, but they were never actually tested. Only `optimize_for_size`, `optimization_level`,
`opt_dead_code` and `opt_propagation` demonstrably moved the output on that shape.

**How to apply -- there is an exact screen, use it instead of guessing.** `-w illpragmas`
makes mwccarm print `<file>:<line>: warning: illegal #pragma` and NAME THE LINE for every
pragma it does not know, while staying silent on real ones. This is now in
`match.py`'s `DEFAULT_FLAGS` (and therefore in `swarm.CPP_FLAGS`, which derives from it), and
`compile_c` surfaces the warning even on a SUCCESSFUL compile, since a clean compile is
exactly the case where a typo'd pragma would otherwise pass as "inert". It is a warning-only
flag with no codegen effect: all banked matches were re-verified with it on.

**Caveat that matters: `2004/b56` does NOT support `-w illpragmas`** -- it stays silent on a
bogus pragma. The whole 1.2 line (base, sp2, sp2p3) does warn. So if you are working a
function whose best version is b56, a single `--version 2004/b56` run will NOT screen your
pragma names. Run `--all` (which compiles the 1.2 versions too) at least once, or screen the
file against 1.2/sp2p3 explicitly, before recording any pragma result.

If for some reason you cannot use the flag, the fallback is the old advice: prove the
compiler HONOURS the pragma by finding one source shape where it changes output at all.
`optimize_for_size on` is a reliable positive control -- it demonstrably moves output.

## 6at. Callee-saved rank is filled in TWO passes, and the address-constant class is a rotator (2026-07-31, PS_UpdateOkAndBackButtons 18 -> 9)

A three-web rotation over r5-r7 that survived decl order, scope depth, the 6y/6aq booster
family at every placement, the 6h launder, C-vs-C++ TU, and a type sweep. Measuring the
classes rather than permuting the source produced a model that predicts the coloring.

**Model.** mwccarm assigns the callee-saved band in TWO passes, not one:

1. **NORMAL-class** webs, in first-definition order, ascending from r5.
2. **ADDRESS-CONSTANT-class** webs (rematerializable pointer constants, i.e. a pooled
   `&global` or a pointer-typed literal), in first-definition order, after all normal webs.

So a pooled base pointer is always pushed BELOW every ordinary local regardless of where it
is defined, which is why ordering levers cannot move it. Confirmed three ways on this
function: baseline normal `{0x1000, idx}` -> r5,r6 with addr `{ptr}` -> r7; retyping the
`0x1000` local to `(int*)0x1000` moves it into the address class and it lands on r6, the
ROM's slot; and typing a RUNTIME-valued local as `int*` changes nothing, because the class
needs a pointer-typed CONSTANT, not merely pointer type.

**New lever.** Casting a named integer local's constant initializer to a pointer type
(`int *k = (int *)0x1000;`) moves that web into the address-constant class and pushes it one
register UP. First time this rotator has been isolated; it is a deterministic way to reorder
two callee-saved webs when decl order is inert.

**The ROM's shape here is all three webs in ONE class ranked by definition order**
(ptr 0x38 -> r5, 0x1000 0x44 -> r6, idx 0xa0 -> r7). Since `idx` is runtime-computed it can
only be normal-class, so closing the last 9 words means getting the pooled `ptr` OUT of the
address-constant class. Nothing tried reaches that.

**Limitation of 6h worth recording there:** the u64-mask address launder moves a pooled
global into normal birth-order allocation for SCRATCH registers (that is what 6h and 6aq
measured), but it is a no-op on a CALLEE-SAVED web. Here it left the coloring untouched,
made the booster inert, and re-hoisted the pool load to +0x30 (14 div). Do not assume 6h
generalizes across the two register bands.

**Separately steerable, worth 4 of the original 13:** an `x = (x & mask) | sl;` RMW emits
`orr r3,r3,sl` when `mask` is a NAMED LOCAL and `orr r3,sl,r3` when it is an inline literal.
Flipping the source operand order does nothing (mwcc canonicalizes commutative `|` before
codegen); the discriminator is whether the AND's right operand is a named local. Splitting
the RMW through a named temp reaches the same shape with the name kept.

## 6au. volatile-on-the-OBJECT plus an inline access beats a named pointer (2026-07-31, PS_UpdateOkAndBackButtons MATCHED)

Closing `_ZN5Stage25PS_UpdateOkAndBackButtonsEb` (18 -> 9 -> 0, byte-identical on 1.2/base,
1.2/sp2, 1.2/sp2p3 AND 2004/b56). 6at correctly diagnosed the last 9 words as a coloring
rotation needing the pooled base out of the address-constant class, but the fix is not
"de-rematerialize the pointer". It is two coupled changes that only work together:

```c
extern volatile int data_02075610[];        /* 1. volatile on the OBJECT */
...
for (j = 0; j < data_02075610[idx]; j++)    /* 2. NO named pointer, index inline */
```

Three measured states, which is what makes the mechanism clear:

- **Named pointer local** (`int *ptr = data_02075610`): the base enters the
  address-constant class, is colored after every normal web, and lands in r7. This is the
  entire 9-word miss, and it DOMINATES -- a named `volatile int *` pointer to the volatile
  array still sits at 9. The address-class assignment wins over the volatility.
- **Plain inline access, no volatile:** the base rematerializes in normal birth order but
  into a SCRATCH register, and the loop bound is cached, so the tail reload disappears and
  the function comes out three words SHORT (0x148).
- **volatile + inline:** the bound is re-read every iteration (the ROM reloads at +0xc0 and
  +0x114), which keeps the base live across the inner loop so it must take a callee-saved
  register -- but it gets there via the normal-birth path, not the address-constant class.
  Result: ptr r5 / 0x1000 r6 / idx r7, exactly the ROM. The volatile restores the three
  missing words and rotates the coloring in one move.

**Generalisation.** When 6at says a pooled base is stuck in the address-constant class, the
lever is to DELETE the named pointer rather than to launder it, and then to restore whatever
reloads the inlining optimised away. `volatile` on the object is the cheapest way to force
those reloads, and unlike `volatile` on a local it costs no stack slot.

**Inert / regressive on this shape (6as-screened):** union int-to-pointer pun (not folded --
forces a real stack round-trip and SPILLS the base, +2 words); a struct member holding the
pointer (folds back to the pool load, 14); runtime-cancel identities
`&arr[base] - base` and `&arr[i] - i` (folded back to the pooled constant, 14); an `int`
holding the address with an inline `(int*)` cast at the index site (stays address-class, 14);
`opt_common_subs off` and `opt_strength_reduction off` (honoured but byte-identical here --
these are established-real pragmas, verified by a zero-byte disassembly diff, not 6as typos).

## 6av. The outgoing-arg phi coalesce: a build delta no register-resident construct breaks (2026-07-31, Stage::InitResources)

A negative worth recording precisely, because it is cheap to re-grind and about 40 constructs
have now died on it.

**Shape.** A variable assigned on both arms of a branch and then passed as an argument:

```c
int bank = 0x36;
if (soundGroup == 0) { ...; bank = table[level2 * 3]; }
Sound::LoadGroupAndSetBank(soundGroup, bank);
```

ROM colors `bank`'s phi web to a NON-argument register and emits one shared merge copy
`mov r1,r2` at the join, which both paths flow through. Every mwccarm build we own -- the
whole 1.2 line and 2004/b56 -- coalesces the phi DIRECTLY into the outgoing argument register,
so the merge copy never exists and the branch jumps 4 bytes further, straight to the call.

**Why it is a trap.** On this function a `u8` variable's truncation (`and rX,rX,#0xff`)
accidentally makes the SIZE correct while being the wrong instruction. So the function reads
as "4 words differ, size fine" and looks like an ordinary coloring miss, when in fact the two
defects are coupled: removing the truncation (int variable, or a direct one-step index) always
lets the coalesce happen and the function drops 4 bytes SHORT. A match needs no truncation AND
the phi off the argument register AND the merge copy, all at once.

**The one construct that does reproduce the copy costs too much.** A memory-resident
volatile or address-taken temp (`volatile int arg1 = bank;`) buys the copy but pays a stack
slot, overshooting by 4. Note this is the exact opposite of 6au, where `volatile` on an OBJECT
was free: volatile on a LOCAL is memory-resident and always costs a slot. Do not confuse the
two levers.

**What would be needed:** the phi's web must be live simultaneously with whatever holds the
base, to create interference that blocks the coalesce. Every free way to force that (no fold,
no extra instruction) has been tried.

**Inert (~40 constructs, none pragmas so no 6as risk):** int vs u8 variable; one-step vs
two-step indexing; a named int index; the 6h u64 launder on the table base, on the passed
VALUE, and as a laundered named arg temp; the 6w comma-operator argument form; an explicit
if/else phi; a ternary phi; a named read before the assignment; the 6y/6aq booster on the
variable, on the other argument, and on the table base at several placements; a 6ar int-typed
base local dereferenced through a cast; the operand swap `(i)[table]`; a 6at `u8 * const`
named base with a named int index; a volatile LOAD via `*(volatile u8*)`; relocating the
initialized declaration to the branch; the uninit-decl split in six orders; naming the guard
read in five declaration positions.

**Class:** register-coalescer build delta, same family as 6ag's first-access-fold. Route it to
the permuter, not to a construct hunt.

**6av addendum (2026-08-01):** the permuter route is structurally BLOCKED here: the function
is cpp-only (the boolean-materialization idiom at insn 9 needs the C++ frontend; c99 collapses
it to compare-and-branch), and the permuter's mutation engine is C-only, so sweeps silently
no-op on it. Same caveat applies to any //cpp near-miss routed to crunch.py/batch.py. Pragma
space and sibling census are closed (see the DB row); the only remaining route is a recovered
pre-1.2/V0.6.1 compiler build.

### 6as addendum: the recognised-pragma vocabulary, and three corrections it produced

`notes/mwccarm-pragmas.txt` now holds the **246 pragma names mwccarm actually parses**,
derived by pulling every lowercase identifier out of `mwccarm.exe` and screening each one
INDIVIDUALLY with `-w illpragmas`. Individual screening matters: `#pragma warning off`
suppresses the diagnostic for every line after it, so a batched scan inflates the accepted
set (725 vs the true 246).

Three corrections fell straight out of it, all of which had been recorded as evidence:

- **`generateconditionalassignments` is not a pragma. The real name is
  `opt_generateconditionalassignments`.** The 150-variant `func_02068398` campaign recorded
  this lever as inert; it was never applied. (Re-run with the correct name: still 1 word, so
  the floor survives -- but the earlier evidence was void.)
- **`scheduling` is not a pragma.** Scheduling is controlled by the `-opt schedule` /
  `-opt noschedule` COMMAND-LINE flag. Any "`#pragma scheduling off` is inert" line is
  meaningless, including ones asserted this session on the grounds that the token appears in
  the binary -- appearing in the binary is not the same as being pragma-exposed.
- **`long_calls` is not a pragma.** The source now at
  `src/_ZN16daObjCtMecha04_c17AfterClsnCallbackEP4dBgWP8dActor_cS3_.cpp` carried
  `#pragma long_calls on` plus a comment crediting it for the pooled
  `ldr ip,[pc,#8]; bx ip` tail-call. It was silently ignored; the file matches on all 12
  builds with the line removed. Comment corrected.

**Practical rule:** a pragma result is evidence only if the name is in that file. Proving a
real pragma inert is a finding; proving a fake one inert is noise that then gets cited as a
closed avenue.

## 6aw. The sub-identity beats commutative canonicalization: `a - (-b)` where `a + b` re-canonicalizes (2026-08-01, func_02072168 MATCHED)

The last real divergence on func_02072168 (12 -> 0) was a 9-word scratch-register rotation
in one two-armed statement (case 9's `b = <mem> + v1`). The mechanism, decoded from the
assignment pattern: mwccarm canonicalizes the commutative add to +(simple, complex) --
`a + v1` and `v1 + a` compile IDENTICALLY (both re-canonicalize to +(v1, a)) -- and then
walks operands right-to-left assigning scratch registers. The ROM's tree is +(a, v1), so
its walk hits v1 last (r0) where ours hit it first. No cast, unary +, `(0, v1)` comma,
`| 0`, `+ 0`, u64 mask on either operand, or arm-local temp moved a byte: everything that
compiles to the same value re-canonicalizes with it, and temps rotate the whole web (12 ->
262).

The lever: spell the add as a SUBTRACTION, `b = <mem> - (-v1)`. Subtraction is not
commutative, so canonicalization leaves the operand order alone; late strength reduction
then folds `- (-x)` back to the plain add rN,rN,rM with +(a, v1) order intact. One edit,
9 words, byte-exact. Semantics are exact on ARM (two's-complement wraparound; the negate
of INT_MIN folds through the same add).

Scope note: this is the first known construct that steers CANONICAL OPERAND ORDER of a
commutative op, which 0202ffec's smull floor and several ordering floors were declared
unreachable over. When a diff shows a pure scratch-register rotation around one
commutative op, try the sub-identity before calling it a floor.

Landing note (split-symbol carriers, extends 9a(3)): func_02072168 is banked and
re-verified at the COMBINED 0x88c extent (0x02072168..0x020729f4) because its compiled
object also emits func_020729e8, the severed 12-byte epilogue. RESOLVED 2026-08-01: the
symbol map now merges the pair (config/arm9/symbols.txt lists func_02072168 at size
0x88c, the func_020729e8 row and its stub src file are gone) -- the first symbol-map
merge of a severed fragment into its parent. Precedent for the func_02071644/
func_02071694 pair (9a(3)'s other proven case) when someone lands that one.

## 6ax. Inverse RMW-launder: demote the PLAIN read to let the RMW chain lead an interleave (2026-08-01, CapEnemy::GetCapState MATCHED)

GetCapState's "leader-of-interleave" floor (two equal-height chains, registers identical,
ROM leads A,B,A,B with the RMW-pool chain, mwcc leads B,A,B,A with the plain-load chain)
fell to the exact INVERSE of the matching-style.md RMW rule: launder the plain single-use
field_f4 READ with the u64 no-op mask and leave the RMW site alone.

```c
    field_b0 = *(int *)(((long long)(int)&field_f4) & 0xFFFFFFFFFFFFFFFFLL);
```

Instruction selection is unchanged (the load still folds to `ldr r1,[r4,#0xf4])` -- the mask
only demotes the load out of its default value-numbering/scheduling class, and that demotion
alone flips which chain leads. Every documented spelling of the RMW side (naming, casting,
block-scoping, splitting, comma-fusing, 22 probes re-verified 2026-08-01) is inert or
regressive.

Rule amendment: "launder ONLY the RMW sites" holds for the ADDRESS-MATERIALIZATION split
(that is what the ROM's RMW/single-use anatomy dictates). But for ORDERING residue between
two chains, the launder is a scheduling-class demotion you can apply to EITHER side: launder
the chain that must YIELD, not the one that must lead.

## 6ay. Four new axes tested on the arm9 floors, all closed (2026-08-01, post-#960 theory sweep)

After #960 landed the last two matchable arm9 functions, four genuinely untried axes were
run against the five remaining floors (`InitResources` 4, `OAM::Render` 2, `0202ffec` 2,
`LoadTex` 5, `func_02009e70` 96). All four came back negative with real controls. Recorded so
none of them is ever re-spent.

**(1) TU composition is DEAD.** Nintendo compiled multi-function TUs, we compile
single-function ones -- tested whether any allocator tie-break rides on TU-level state.
Non-static DCE-safe control functions before AND after the target, then REAL ROM-adjacent
neighbor merges (``CleanupResources`` + ``InitResources`` in ROM order, six conflicting extern
types reconciled, the neighbor re-verified byte-matching its own extent from inside the
merged TU): zero bytes moved, byte- and reloc-level identical, on both floor classes
tested. TRAP FOUND on the way: mwccarm does NOT finalize a function's codegen at its
closing brace -- a `#pragma opt_*` toggle AFTER the function retroactively changes its
bytes (`OAM::Render` shrank 8 bytes when a restore line alone was appended). Consequence
for the readable-tree migration: a matched draft that relies on unscoped
`#pragma opt_* off` cannot be folded into a multi-function TU as-is; the required restore
changes its own output. Audit pragma-carrying drafts before any TU consolidation.

**(2) The version x flags grid is DEAD.** 16 versions x 8 -O variants x 4 floors = 512
cells: the 12 wrong-size versions (1.2/sp3, sp4, all 2.0/*) are wrong-size at EVERY -O
level, and no cell anywhere beats canonical. On the two control-flow-heavy floors the
`,p` bias is load-bearing for size itself. Version axis now closed across the whole
optimizer space, not just at canonical flags.

**(3) Hidden flags are DEAD, and here is the fast enumeration method.**
`-help all,secret,obsolete,ignored,deprecated,meaningless,compatible` -- the `secret`
keyword is itself unlisted -- prints the compiler's full flag table (546 lines vs 513),
exposing all 8 flags invisible to `-help all`. Newly screened inert: -lines, -browse,
-[no]force_compile, -linkername, -inline always, -allow_byte, -warn_byte. The only one
with codegen effect is -avoid_byte, already documented in 6ah (ROM does not use it).
Use the -help secret line instead of strings-mining next time.

**(4) Goto-pin CFG restructure and the REAL permuter are DEAD on these floors.**
Goto/label surgery on InitResources' bank phi: inert or regressive (goto cascades into a
full basic-block reschedule, 4 -> 20). OAM::Render's prologue transposition is immune to
ALL body-level restructuring (6 variants byte-identical) -- it is a pre-body,
parameter-count-driven filler-zip decision, a true build discriminator. The real
decomp-permuter (first actual run on this residue, 6,512 candidates, 8 min, -j8) never
improved func_0202ffec's smull-operand score: the smull field and its operand's physical
register are one joint allocation decision. `func_02009e70`'s RC4 cluster narrowed:
pragma-toggle/named-CSE/temp-split/block-boundary all byte-inert, reorder regresses;
same coloring-priority class as 0202ffec.

Permuter operational note (Windows): --quiet or --stop-on-zero stall permuter.py with
stdout-flush OSErrors; run plain with output redirected (~14 cand/sec at -j8).

## 6ba. The 4a8-pack rank-pin: a partial decouple, and a canonicalization split from its *ov075* twin (2026-08-02, Fable on `func_ov080_02125460`, ~330 compiles, still div 5)


`func_ov080_02125460` (TEXIMAGE_PARAM pack, div 5) is the same two-attractor rank-pin
as `func_ov075_0211a948` (6ab/2114): ROM wants A's selection (b>>3 materialized, lsl#26
folded into the first orr) with B's coloring (b=r2 coalescing the dying pointer,
t26=r3). Three findings extend the class record:

- **The 120-perm result does NOT transfer between twins.** On *ov075* every or-chain
  permutation returned exactly 5; here the baseline order is the UNIQUE minimum at 5
  and all 119 others are 7-10. Same residue class, different canonicalization
  landscape - re-run the perm sweep per function before importing a twin's negative.
  B-order here = 7: coloring flips to the ROM's (b=r2, t26=r3) but fold-side follows
  term order strictly - across ~20 spellings (b/8, u64-shift launder, static-inline
  sh3, mask-first) the orr folder ONLY takes the second operand's shift; no commute
  was ever observed. Late-lowering spellings either lower before selection (b/8,
  inline) or materialize extra code (u64 launder +8, mask-first +4).
- **First known partial decouple of the class: a named extract + ANY equal-arm
  var-cond ternary in the load's index.** `t = (a>>0x1a)&7; b = q[t ? 0 : 0];` (cond
  identity irrelevant - `q ? 0 : 0` behaves identically; the folded diamond acts as a
  forward-substitution barrier) reaches b=r2-coalesced (`ldr r2,[r2]`) WITH A's
  selection - the two load-bearing words the plain A-tree can never produce. Cost: the
  a/t-web coloring rotates (a=lr instead of r4, t26=r4, t20=r3, t23=ip) and t29's lsr
  sinks below the first orr = stable div 14. The rotation is immune to all 6 decl
  orders, a-laundering (+8), cast-arith a-loads, and all 120 dep-side term
  arrangements (phase-2 sweep min 7). Adding a second cond-use of `a` worsens (16).
  Constant-arm ternaries (`c ? 3 : 3`) parse-fold and do nothing anywhere.
- **Context does not leak into the pack block.** Inlining the head's single-use
  pointer, equal-arm ternaries in other blocks, struct-typing, volatile HW stores,
  extern-C C++ mode, BB barriers (label / do-while(0) / switch(0)), two-def webs
  (t&=7, b>>=3), and a 5-statement `v |=` accumulator chain (survives as structure
  but rotates q/a/v) all leave the b/t26 pair untouched. One head trap worth banking:
  the 0x1b6 halfword must stay ANONYMOUS inside the short-circuit `||` - naming it
  hoists the ldrh above the branch and breaks the matching head.

Verdict unchanged (still parked at 5, `// NONMATCHING`): the b/t26 rank rides the
canonical A-tree, and the only rerank construct drags the a-family with it. The open
angle narrows to: a substitution barrier that does NOT perturb sibling-web coloring,
or any construct that makes the orr folder commute.

## 6bb. 4a8-pack rank-pin, second full-angle sweep: the phase-order boundaries (2026-08-05, Fable on `func_ov080_02125460`, +55 compiles, still div 5)

A fresh session re-attacked 6ba's wall with the post-6ba lever families (6y boosters,
6aq caller-saved inversion, dead-use priority, fold-commute probes). Nothing beat 5;
what the sweep bought is a precise map of WHICH PHASE eats each construct:

- **Dead-expression elimination runs BEFORE web construction.** Every `(void)`-use
  spelling of the extract or of `b >> 3`, at every placement, with and without
  `#pragma opt_dead_code off`, compiles to the exact baseline bytes. A booster
  statement (`x = x ? x : x;`) whose variable has NO later real read is likewise
  dead-eliminated whole (tried on the head's dying `r3` web to shape the free list,
  and on `a` after the pack store). 6y-1 boosters exist only for values with a
  downstream real use - dead-use priority boosting is impossible in b56.
- **The div-14 rotation rides the named web's EXISTENCE, not multi-def promotion.**
  `#pragma opt_propagation off` + a SINGLE-def named `t` rotates identically to the
  self-select and index-ternary forms; so does every booster placement/cond
  permutation (cond self/c/q, doubled, before/after b's decl, anonymous-b variant,
  and an a+t double-boost attempting r4-class occupancy). The 6aq recipe does not
  transplant: here the boosted scratch web always promotes INTO the r4 class and
  drags the a-family, regardless of arrangement.
- **The orr folder DOES commute - but only under multi-use, which always
  materializes the rejected shift.** A-order + duplicated t26-term: coloring flips
  to ROM's (b=r2 coalescing dying q, t26=r3) and the folder folds b's lsr instead -
  but the t26 lsl materializes (7). B-order + `b >>= 3` statement: same commute
  geometry from the other side, propagation folds the shift back (7). Fold slot and
  use count are coupled at selection time; there is no free commute.
- **Adjacent `x|x` parse-folds and RE-CANONICALIZES the whole or-chain.** B-order
  with the b-term duplicated adjacently compiles to the A-CANONICAL baseline bytes -
  the idempotence fold normalizes term order before selection. Separated duplicates
  survive as a real extra orr (+4). Expression-arm equal ternaries (`V ? X : X`)
  materialize a diamond (+8) in both value and index positions; only const-arm forms
  on an already-named var fold free (the 6ba finding).
- **Renormalizing-neutral on this pair** (exact baseline bytes): `register`,
  int-retype of b + `(u32)` cast at use, volatile b-load, volatile q (extends 6y-4:
  b IS memory-sourced and still does not flip), `qb = *(u32*)qb` identity reuse (the
  web splits at the reassignment - 6q does not apply to straight-line reassignment),
  and all recognized opt pragmas incl. `optimization_level 2/3`. Size-breaking:
  `optimization_level 1`, bitfield-struct reads of q[4] (extract shape changes),
  static-helper-call inlining, u64 value-launder of the extract.

Verdict: parked at 5, unchanged. The open angle is now a construct that gives t26's
web a use which (a) exists at allocation, (b) vanishes before selection, and (c) is
not a named web - no known b56 construct occupies that phase window. Treat the
b/t26 pair as a true allocator rank pin absent a genuinely new construct class.
Full per-family nodes: config/match_attempts.jsonl, parents under 6339f0f5....

### 6bb addendum: gccext statement-exprs open a FOURTH attractor - and close it (same session, +17 compiles)

`-gccext,on` means `({ ... })` statement expressions compile, and they were absent
from all 385 prior compiles on this function. They are NOT neutral:

- **An inline SE whose body is anchored by a LOAD survives as a web** and does two
  things at once: it SEALS its shift from copy-propagation (the `b >> 3` def stays
  materialized - the orr folder then COMMUTES and folds t26's lsl with Rn=b, giving
  the first-ever exact `and r3,r3,#7` + A-selection fold since 6ba opened), and it
  DEMOTES the web to the BOTTOM of the scratch hand-out (b lands in ip, below every
  anonymous temp; position in the chain is irrelevant). Stable new residue at 11.
- **Register-pure SE bodies always collapse** back to anonymous temps: plain
  extracts, `s &= 7` two-statement bodies, and internal self-select boosters
  (the booster additionally leaks the div-14/15 rotation out of the SE scope).
- **Decl-init SEs dissolve wholly**: `u32 bb = ({ q[0] >> 3; });` propagates SE and
  all into the single use - it behaves exactly like the plain B-order spelling
  (right coloring, wrong fold side, 7), not like the inline SE.
- Scope-depth nesting of the store statement, `%`/division respellings of the
  extracts, and every splice of {SE-seal, B-order rank, opt_propagation off,
  nesting} land in the now-five known attractors {5, 7, 11, 14, 15}.

Net: the fold-seal (inline load-anchored SE) and the r2 rank (named local, bare in
a B-order chain) each work alone and live in PROVABLY disjoint construct classes;
every bridging construct either dissolves (decl-SE), demotes (inline SE), or
rotates (pragma / booster / multi-def). The 6bb open-angle spec tightens once
more: the missing construct must seal a shift-def from propagation while keeping
named-block-scope rank - nothing in c99+gccext reaches that intersection.

## 6bc. The vcall SPELLING is a callee-saved homing-rank lever (Player::CanEnterDoor, div 13->0, 2026-08-07)

The pret-idiom note (6e area) already records that C++ virtual dispatch loads the
vptr before homing `this` where a C function-pointer cast homes first - a PROLOGUE
ORDER effect. It is also an ALLOCATOR RANK effect: on _ZN6Player12CanEnterDoorEh
(0x020ca5cc, the door-crash NONMATCHING), every source form spelling the +0x48
vcall as a C fn-ptr cast (`(*(T**)(c+0x360))->vtbl[0x48/4](...)`) homed the args
this=r4/door=r5, a global 13-word rename against the ROM; respelling ONLY the
vcall as real C++ virtual dispatch (18 dummy virtuals + `o->GetType()`, no other
change) restored the natural reverse-arg homing (door=r4, this=r5) and landed
div=0 with a plain `return 0;`. The old draft's `{int r = door; r -= door;
return r;}` RMW was a byte-costing pressure hack compensating for the C-cast
perturbation; with the virtual spelling it is unnecessary.

Ruled out on the way (all div=13 inert on the swap): param type widening,
local copies of either arg, decl order, DeMorgan/merged/split guard respellings,
if/else fallthrough restructure (that one goes 12 bytes short - the occupied
path's mov #0 epilogue is a distinct source-level `return 0`, not tail
duplication), 12 single pragmas, 4 pragma pairs, bool-return method form, and
every algebraic-zero return (they fold in IRO and erase the liveness they were
meant to add). The permuter cannot reach it either (14k iters, best 200): the
lever is a C-vs-C++ dispatch respelling, outside its mutation space.

Rule: when a pure-permutation residue sits in a function that dispatches through
a C-cast fn-ptr table, respell the dispatch as a dummy-virtuals C++ call BEFORE
grinding coloring levers - check it against 6aa's natural-homing probe first.
## 6bd. Two store-side levers from the div 6/7 pair (cannon lid + question-block bounce, 2026-08-08)

Both landed in PR #1227, cracking a div=6 and a div=7 that had sat as
"pure residue" near-misses.

**The store-RMW respell.** `*p = (s16)(*p + 0x1000)` and `*p += 0x1000` are
the same operation on an `s16 *`, but they are NOT the same to the address
allocator: the explicit-cast form materializes the pointer into the slot the
ROM did not use, and the compound-assignment form flips it back. On
func_ov102_021498e0 (question-block bounce, 0x398) this single respell closed
the address-materialization half of a div=7 (the other half was a real seed
bug: case 0 loaded data_ov102_0214e870 where the relocs prove the ROM loads
data_ov102_0214e8c0 -- re-read the relocs before trusting an inherited seed).

**First-consumed-web order.** On func_ov098_0213ade8 (cannon lid, 0x2bc,
div 6->0): compute x before y AND store x before y, so the x web is the first
CONSUMED scratch web and takes r0. The 6q first-consumed rule, applied at the
web level rather than the expression level: the consumer order of whole webs
decides the scratch coloring, and source statement order is the lever that
sets it.

## 6be. FLOOR: the conditional-cast shift-pair split vs load-hoist collision (func_ov002_020bb614, div 7, 2026-08-08)

A new ordering-floor SHAPE, distinct from the 6o/6av families. The ROM
schedules two independent loads (`ldr r1,[r6,#0x60]`, `ldr r5,[r6,#0x598]`)
BETWEEN the `lslne`/`lsrne` halves of a conditional `(u16)` zero-extend.
Keeping r0 busy through the gap is what colors the downstream my/mx pair
r1/r0 and orders the msgPos stores y,z,x. From C, mwccarm will not split the
shift pair that way: any form that lifts the first load into the gap also
co-hoists the SECOND load too early AND sinks the flag-init `mov r4,#0` --
three constraints that are pairwise satisfiable and mutually exclusive as a
triple.

Axes swept to establish it: msgPos store-order permutations (36), local
declaration order (7 forms), the msgId cast form (ternary, mask,
manual-shift split), volatile-on-object (4 forms), load/if reordering (11
forms), plus the prior permuter run (floor score 80 at 650 iterations). The
body from +0x44 to +0x3dc is byte-identical throughout; all seven divergences
are the one prologue permutation. Evidence banked on the near-miss row.

Rule: when the residual is a shift-pair with foreign loads interleaved, check
whether entering the gap forces a second hoist before spending on ordering
levers -- this shape reads as crackable (no floor note, "just scheduling")
and is not.

## 6bf. Pairwise-transposition climbing beats random shuffles on decl-order floors (func_ov007_020c9688, div 33 -> 10, 2026-08-15)

A "FLOOR(regperm)" call on a 12-local declaration list survived 100+ variants,
two permuter runs and two sessions, and then fell to swapping two declaration
lines. The mechanism was never exotic; the SEARCH was wrong.

func_ov007_020c9688 (ov007, 0x300) was banked at div=33 with
`floor {class: regperm, evidence: "... cannot force ip ..."}`. Splitting the
residual by address region showed two independent clusters: 14 words in the
first loop (a 3-cycle over {r1, r3, ip} for the webs `hi`/`bA`/`bE`) and 19 in
the second (a 5-cycle over the callee-saved chain). The first cluster is now
ZERO from one change: move `hi` from declaration slot 7 to slot 3 and `bA` from
3 to 7. `hi` lands in ip, exactly the coloring the floor note said could not be
forced.

Rank rule this pins down, complementing 6ab: among named-local webs competing
for the SCRATCH file, earlier declaration takes the HIGHER register. Measured
here as slot 3 -> ip, slot 4 -> r3, slot 7 -> r1. 6ab's "descending from r3 in
first-definition order" is the same rule seen from the other end, and ip is in
the sequence rather than an unreachable spill-of-last-resort.

Why two sessions missed it. The prior sweeps, and my own first pass, sampled
declaration order RANDOMLY -- 400 random shuffles of a 12-element list. A
specific adjacent transposition has probability ~1/66 of appearing in a random
shuffle at the needed pair of slots, and nothing in the sample is a small step
away from the seed, so the search never sees the gradient. Greedy climbing over
pairwise transpositions found it in one pass. Both moves matter: swap any two
DECLARATION lines and any two ASSIGNMENT lines, score, keep improvements,
repeat to a local optimum, restart a few times.

Cost: an oracle call is ~0.19s (`wallcrack.Target.div`), so a full
transposition neighbourhood of a 12-decl / 13-assignment body is 144 compiles,
about 4s on 8 threads. There is no reason to sample this space randomly.

Two more things worth carrying:

- **Score by ADDRESS REGION, not just the total.** Splitting 33 into 14/19 made
  two independent problems out of one, and made each one's local optimum
  legible. A single global count hides which lever moved what.
- **The permuter's stock scorer will happily reward semantically WRONG code.**
  Here it "improved" the second loop by 2 words by moving a read from `*q` to
  after `q += 0x18` -- reading the NEXT record -- emitting `ldrh r3,[rN,#0x18]!`
  where the ROM has post-indexed `ldrh r3,[rN],#0x18`. The honest form of the
  same lever (read the saved cursor `q0` at the use site instead of a named
  temp) scores identically and is correct. Read permuter diffs for semantics
  before banking them; a score is not a proof.

Remaining residual is 10 words, all in the second loop: a pure 3-cycle over `q`
(ROM sb), the argument zero (ROM r8) and the m-reset zero (ROM r7). That one
survived 720 declaration permutations, 7 hill-climb restarts from random
starts, named-zero webs at every declaration slot, 6 outer-loop shapes, 10
inner-loop forms, the verified pragma vocabulary and all 25 installed
compilers, so it is banked as the live near-miss rather than a floor claim --
the last two claims on this function were both wrong.

## 6bg. Measure the register IDENTITY, not the divergence count, before claiming a decl-order lever is exhausted (func_ov007_020c9688, still div 10, 2026-08-16)

Follow-up to 6bf on the same function. 6bf closed the first-loop cluster and left
10 words in the second loop: a 3-cycle where the ROM colors the cursor `q` into
`sb`, the call's argument zero into `r8` and the m-reset zero into `r7`, while
every compile of ours colors them `r7` / `sb` / `r8`.

Roughly 900 further hypotheses moved the count by exactly nothing. All of these
return div=10, not "about 10" -- the identical number, with loop 1 still at zero:

- all 120 block declaration permutations, and 735 more from the 6bf greedy
  pairwise climb over a 15-element *function-top* declaration list (seed plus 6
  random restarts, every one a local optimum at 10 with no gradient anywhere)
- 6y lever 2 (scope depth): `q` at five function-top slots
- 6y lever 1 (fake self-select use-count boost) in five placements
- 6y lever 4 (`volatile` on a memory-sourced web), plus `register`
- the type-rank lever: `q` as `char*`, `u32`, `s16*`, `u16*`, `void*`, `Entry*`
- nine zero-plumbing shapes, including one shared named zero, two named zeros in
  both definition orders, and reusing the function-top `heap = 0` (already an
  argument to the loop-1 allocator) as the loop-2 call arguments
- the pragma vocabulary, singles and pairs

The useful move was to stop scoring the count and start reading the allocated
register out of the object. Disassembling word +0x250 (`q`) and +0x264 / +0x268
(the two zeros) across 520 variants gives four outcomes and only four:

    (q, zero_a, zero_b) = (r7, sb, r8)   137
                          (r5, sb, r8)   134
                          (r4, sb, r8)   127
                          (r6, sb, r8)   122

The zero pair is **invariant**. Declaration order moves `q` freely around the low
callee-saved band r4-r7 -- which is why the count looks alive at 10/13 -- and
never once places it above r7. mwccarm ranks the two loop-invariant constant webs
that live across the call above a cursor pointer web, and hands them the two
highest free callee-saved registers; the ROM does the opposite.

Two things to carry:

- **A flat divergence count across a large sweep is not evidence the space is
  searched; it can mean the lever never touched the web you care about.** Reading
  the register identity turns "900 variants, no progress" into a specific,
  falsifiable statement about which register a web can and cannot reach. Cheap:
  the object is already in hand, it is four lines of capstone.
- **Not marked as a floor**, deliberately. Two prior floor claims on this function
  were both wrong, and a `floor` entry makes the refine tooling skip the target.
  What would break this one is a lever that outranks a cross-call constant web
  against a pointer web, which is a rank rule none of 6k / 6q / 6y / 6ab / 6bf
  currently spells. The near-miss stays live at div 10.

## 6bh. A hoisted constant block in the prologue is evidence of BARE LITERALS in the loop body (func_ov007_020b2998 MATCHED, 2026-08-16)

func_ov007_020b2998 (ov007, 0x23c, the title screen's widget setup) opens with a
block that reads like deliberate setup: `mov r8,#0x1000`, `mov r0,#0x32000` plus
`rsb r0,r0,#0`, seven more `mov rN,#imm` / `str rN,[sp,#..]` pairs, then
`mov r5,sl`, `mov r7,#1`, `mov r6,#0x80000`, `mov fp,#0x60000`. Twelve constant
webs materialized before a three-iteration loop, five kept in callee-saved
registers and seven spilled to the frame.

Nothing in the source expresses any of it. Eleven of the twelve are BARE LITERALS
written inline in the arms of a `switch (i)` inside the loop (`w->f_4e = 0x14;`,
`w->f_70 = 0x20;`, `w->f_0 |= 2;` and so on); the twelfth, `0x1000`, is CSE'd with
the pre-loop `P->f_f8 = 0x1000` store. mwccarm treats each distinct loop-invariant
literal as a web, hoists the whole set into the preheader, colors what fits and
spills the rest. The block reproduced for free from the naive transcription on the
first compile. The matched file has no named constants, no const table and no
hand-hoisted temps.

**Read the block as evidence, not as structure.** A prologue full of
`mov rN,#imm` / `str rN,[sp,#..]` for values only consumed inside a loop is a
statement about the LOOP BODY: it spelled those numbers inline. Drafting them as
named locals or as a table declared above the loop is the wrong shape, and an
expensive one, because named locals then join the declaration-order rank game of
6ab / 6bf and a table changes the addressing as well.

**The hoist/no-hoist split is by materialization cost, which makes the literal pool
a diagnostic for what the source spelled.** Three negative Q12 constants appear in
this function and only one is hoisted. `0x32000` is a rotated 8 bit immediate, so
`-0x32000` costs two instructions (`mov` plus `rsb`), becomes a web, and lands in
the spilled set at `[sp,#0xc]`. `-0x320ff` and `-0x31f01` are reachable by neither
`mov` nor `mvn`, so each stays a pc relative load (`ldr r0,[pc,#0xc8]`) inside its
own switch arm and is never hoisted, despite being exactly as loop-invariant as the
one that was. `0x7fff`, stored to a halfword field, behaves the same way.

So the pool tells you which constants the source spelled in a form mwccarm could not
synthesize. A value sitting in the pool that your draft hoisted, or a value hoisted
into the prologue that your draft left as a pool load, is a shape error in the draft
and not a coloring problem; no register lever will close it.

**The zero web coalesces with the induction variable's initializer.** The zero used
for `w->f_46 = w->f_4c = 0;` inside the loop is not a fresh `mov r5,#0`. The ROM
emits `mov sl,#0` for `i = 0` and then `mov r5,sl`. Constant webs are ranked and
coalesced against each other, the same machinery 6bf and 6bg measure from the other
end, and the zero is cheap enough here to reach a register while `0x100` and `0x14`
are not.

Rule: do not spend a cycle reverse engineering a prologue constant block before
checking whether it falls out of the naive body. Here it did, and the whole 0x23c
matched on the second compile once the per index dispatch was written as a `switch`
(the ROM lays out the full compare chain before the case bodies, which an if/else
chain cannot produce and which is worth exactly the one word of size difference) and
`int i` was declared ahead of the object pointer. div=0 at 2004/b56, strict relocs,
linkcheck VERIFIED.

## 6bi. `(char *)&member` on a `u8` marker in the DERIVED part goes through the literal pool (SignPost::InitResources 0x198 -> 0x188, 2026-08-16)

A half-migrated class carries `u8 mFoo;` markers where a real member belongs, and the
call sites cast: `SomeClass_Init((char *)&mFoo, ...)`. That is byte-transparent while
the member sits in the base part of the object — but not once it is far enough out.

`SignPost` derives from `Platform` (0x320) and puts a `MovingCylinderClsn` at 0x320
and a `ShadowModel` at 0x358. With both spelled as `u8` markers, the offsets
`this + 0x320` and `this + 0x358` came out as **`ldr` from the literal pool plus
`add`**, where the ROM has a single `add rN, rM, #0x320`. Four extra words —
`InitResources` compiled to 0x198 against the ROM's 0x188 — and the *whole* of the
difference was those two address computations.

Typing the members and calling the methods on them (`mMovingCylinderClsn.Init(...)`,
`mShadowModel.SetFile(...)`) matched on the first compile.

The threshold is not the offset. `mWithMeshClsn` at **0x3c8** in the same class,
reached the same way, never had the problem: its call site passes the address to a
function that takes it as its first argument in r0, and mwcc emits the plain `add`
there. Do not derive a rule about "offsets above N" from this — the shape that
breaks is `(char *)&marker` used as a *non-leading* argument, where mwcc has already
committed a register-allocation decision by the time it needs the address.

The practical form: **if a migrated class's function is a small multiple of 4 bytes
too long and the surplus is address arithmetic, the remaining `u8` markers are the
cause.** Type them and the length falls out; no coloring lever is involved.

Same family as 6ar (a hoisted address local colors by its declared type) seen from
the layout side rather than the register side.

## 6bj. The pool's regions, park membership, and the DESTINATION of a free second definition (DetectClsn(SphCrr) MATCHED, 2026-08-21)

The frame under 2004/b56 `-O4,p` is ordered regions:

```text
low  [outgoing args][CHAIN][pool: webs / singles / coalesced band][aggregates]  high
```

The CHAIN holds declared locals kept in memory, in declaration order, and it
CLOSES at the first aggregate declaration -- a scalar declared among the
aggregates is exiled to the pool. The pool always begins at chain end + 8 (one
structural pad word -- on this function it is the dead high-word leg of the
`(s64)x * x` widening, present in every build), so ANY chained placement of a
variable, an apparently-free 4-byte alignment gap included, shifts every pool
slot by four.

Coalesced-band membership (named locals that live among the temps) has
measured triggers: an all-remat web (a no-op re-bind of `&object.field`
qualifies -- it rematerialises from a register), a propagated compile-time
constant (including `k = zeroVar` copies), an iteration-contained in-loop
coalesce (an in-loop bare-load local whose uses stay inside the iteration),
a volatile-source bare read -- and, the one that closed this function, **a
zero-code volatile round-trip second definition**:

```c
rsc = sphere.radius << 4;
rsc = (s32)(volatile s32)rsc;   /* emits NOTHING; demotes rsc into the band */
```

The FLAVOUR of a free second definition picks the destination: `x = x + k0`
folds and demotes nothing; `x = x * k1` demotes only to the annex right after
the chain (shifting the pool); the volatile round-trip demotes into the band
proper, landing on the source-position-ordered slot. A null result on a lever
spelled one way is not a null result on the lever. (The round-trip is a
matching hack -- no 2004 author wrote it; it proves the allocator state and
the mechanism, not the original spelling.)

Bands order by source position of the entity's defining/first-materialization
point (first-def for values, first-use for constants); a park's position does
not move when the demoting statement moves. Storage webs (multi-def non-remat
locals like `dotN >> 31` highs) allocate as 8-byte pairs and can never take a
4-byte band slot; every arithmetic demotion of a load-derived scalar fuses it
with the widening pad word into such a pair, which is why only the round-trip
reaches the band. Full data: notes/collision-system.md Phase 3b.

## 6bk. Four transferable results from the same campaign

* **A per-site cv-qualifier CAST is its own CSE class.** ~350 statement-level
  rewrites of a prologue were byte-neutral because mwcc canonicalises them to
  one IR before allocation -- but `((const T *)p)->x` against a
  `const volatile T *` declaration never joins the bare `p->x` web: it stops
  the just-computed address register being consumed directly and lets a read
  subset share one reload. Statement reordering cannot reach scheduling
  effects that a per-site qualifier cast reaches for free.
* **A two-leaf sum emits right-load-first, and the add is always
  `add rd, LEFT, RIGHT`.** The ROM's operand order on a commutative add over
  two loaded terms is unreachable from any two-leaf spelling; name the term
  whose load the ROM does not re-issue at the site so it becomes a register
  leaf and the add commutes.
* **Score with the flags that build the ROM.** fdiff's `-w illpragmas` regime
  read a fully matched file as 4/1778: a literal-pool word and the ORDER of
  four hoisted zero-init stores are `-Cpp_exceptions off` codegen. Two
  phantom "defects" and one inflated bucket count came from the wrong regime.
* **The compiler's own pragma table is extractable.** The full list of names
  `#pragma <name>` accepts is embedded as plain strings in mwccarm.exe -- 81
  legal names under 2004/b56, including undocumented internals
  (`opt_marknonregtemps`, `opt_scalarizeliveranges`,
  `opt_optimizenonregaccess`, `opt_serializeassignments`,
  `opt_decomposeaggregates`). Probe legality via the `illegal #pragma`
  warning match.compile_c surfaces; `opt_generateconditionalassignments on`
  ICEs this build. On this function all 81 x on/off were inert or
  regressions -- the census exists so nobody re-guesses pragma names.

## 6bl. A memory-homed local always sorts ABOVE every spilled scalar, and that is a frame-layout floor (func_ov075_0211621c, div 44 -> 40, 2026-08-29)

`func_ov075_0211621c` (ov075, 0x394) is the VS/course-entry results row: for each
player slot it draws the star icon, patches the star-count digit into the returned
OAM entry from `data_0209f310[pid]`, then lays out a three-digit time from
`data_0209f358[pid]`. Two independent near-misses (grok-4.6, div 49 then 44) stalled
on the same residual.

**The lever that moved it (44 -> 40).** Hoisting the pooled table read above the
stride arithmetic fixes a two-register coloring swap at the top of the function.
The ROM holds the `data_ov075_0211c6e8` base in `r2` and `d * 16` in `r1`; every
spelling that computes the stride first gets them the other way round. The source
order of two *independent* pooled reads in the prologue picks which one takes the
lower scratch register, even though neither feeds the other:

```c
xbase  = data_ov075_0211c6e8[d];     /* must come first */
stride = (d * 16) + 0x38;
```

That is the general form: when a prologue has two unrelated pool loads and one is
mis-colored, permute their *statement* order before reaching for anything else.
It cost four words here and is free to try anywhere.

**The floor (40 words, one paired swap).** The ROM frame is
`0x18 count, 0x1c stride, 0x20 hundreds, 0x24 i`, and `hundreds` (the `n / 100`
digit) is memory-homed: mwcc writes the division back to its slot after every
sub-step, three `str [sp, #0x20]` in a row. Reproducing that homing requires an
aggregate spelling (`int hv[1]`); a plain scalar keeps the value in `fp` and the
function comes out 0x390, one instruction short. But **mwccarm places a
memory-homed aggregate above every spilled scalar in the frame, unconditionally**,
so the array can only land at `0x24` and `i` takes `0x20`. The ROM wants the
opposite order, which no aggregate spelling can produce.

**Measured, per 6bg, by identity rather than by count.** Reading the allocated
register out of word `+0x48` and the slot out of word `+0x180` across ~900 compiles:

- all 720 function-top declaration permutations: `xoff` is `sb` in **720/720** and
  `hundreds` is at `0x24` in **720/720**. What declaration order *does* move is
  `i`'s slot, freely and uselessly, 240 each at `0x18` / `0x1c` / `0x20`. The count
  reads 40 throughout, which is exactly the 6bg trap: the lever is alive, it just
  never touches the web that is wrong.
- 12 further positions for the `xoff` declaration inside the loop block: `sb`, 12/12.
- the full pragma census from 6bk, all 69 `opt_*` names extracted from
  `mwccarm.exe` x on/off on both the aggregate and the scalar base: 124 inert,
  6 size regressions, 7 illegal-token (those take an argument), and **zero** that
  moved either web.
- seven memory-homing spellings (1-element array, plain scalar, `volatile` scalar,
  address-taken through a pointer, innermost-scope array, a 2-element array holding
  both values, two separate 1-element arrays), the access-expression levers from
  section 2, and associativity/commutation on all four `(xbase + k) + xoff` sites.

**Not marked as a floor** in the DB, per the 6bg convention. What would break it is
a spelling that memory-homes a value *without* making it an aggregate, so it sorts
with the spilled scalars. `opt_decomposeaggregates` / `opt_scalarize` /
`opt_marknonregtemps` were the obvious candidates and all three are inert here. The
near-miss stays live at div 40.
## 6bm. Named-web birth order cracks a pool-pointer rotation, and a two-source-shape floor (ov075 VS-entry pair, 2026-08-29)

Two ov075 functions matched and one floor confirmed while closing the last
unsourced bodies in the VS/wireless entry overlay.

* **A pool pointer that colors one register off is a MISSING NAME, not a
  coloring wall.** `func_ov075_02119dc4` sat at div 5 with the whole function
  otherwise exact: the ROM held the pooled global's address in r2 while every
  spelling of the draft put it in r0, and the two loaded values rotated with
  it. The rule from 6ab ("named-local webs color descending from r3 in
  FIRST-DEFINITION order") reads forwards as well as backwards: the ROM's
  r3/r2/r1/r0 descent means BOTH the struct pointer and the pool pointer were
  named locals, defined in that order. Giving the pool address a name defined
  immediately after the struct pointer -- inside the same comma group so it
  still lands after the short-circuit -- took div 5 to 0 in one step. Its type
  (`int *`, `const int *`, `u32 *`, `struct P2 *`) was irrelevant; only the
  existence and the position of the name mattered.

* **A comma-group assignment sinks a named pointer into a `||` right
  operand.** Declaring the pointer at the top of the enclosing block hoists
  its `add rD, base, #imm` above the guard calls, which is four instructions
  early. `if (A || ((p = expr), B(p)))` computes it exactly where the ROM does
  without changing a byte anywhere else. This is the placement knob that
  block scoping cannot express in C.

* **A `symbol + nonzero addend` data reloc false-flags in the strict-reloc
  gate.** `reloc_audit.object_reloc_dests` is addend-aware only for `_ZTV`
  names; every other reloc resolves by symbol name alone. A loop that
  strength-reduces to a running pointer one byte into a global therefore
  reports `cand data_0209fc5c (0x0209fc5c) != config 0x0209fc5d` on bytes that
  are exact -- `linkcheck.py` (which does apply addends, and says so in its
  own docstring) returned VERIFIED on the same object. If the interior address
  has its own `data_<addr>` symbol, walking an explicit pointer from THAT
  symbol keeps the bytes and clears the gate; here `data_0209fc5d` was already
  in symbols.txt. Note the spelling is load-bearing: `q[0]; ...; q++;` as
  separate statements reproduces the ROM's `ldrb`/`add #1` pair, while
  `*q++` costs a word and `arr[i - 1]` kills the strength reduction outright.

* **FLOOR (func_ov075_0211afb0, div 4): a two-instruction coalescing
  preference in the SECOND of two unrolled vertex emissions.** The ROM routes
  `x << 9` through the dying index register (`lsl sb, r0, #9`) and `z << 9`
  through the freed raw-x register (`lsl r0, r5, #9`); mwccarm coalesces both
  shifts in place (`lsl r0, r0, #9` / `lsl r5, r5, #9`). The first vertex
  block, from the identical source, is exact. Everything else in the function
  -- schedule, all other registers, the whole prologue and both loops -- is
  byte-exact. ~50 shapes were probed across two independent authors: locals vs
  inline member reads, `s16`-then-`(u16)`-at-use vs `(u16)` at assignment,
  named intermediate shifts, the three address-expression trees from 6aa #5,
  `&arr[k]` / `arr + k` / byte-cast arithmetic, C and C++ TU form, a `static
  inline` emit helper, equal-arm ternaries, volatile statement-level inits,
  reversed decl order, and `opt_propagation` / `opt_strength_reduction`. The
  near-miss DB's pre-existing row (do-while loops, `char *` bases, explicit
  byte offsets) and this campaign's struct-array shape produce the SAME four
  wrong words at the SAME offsets. Two unrelated source shapes converging on
  one residual is the signature of a real allocator preference, not of an
  unexplored spelling: hand it to the permuter, not to another rewrite.

* **`(u16)((v << 9) >> 16)` is a four-instruction unit the scheduler splits.**
  For DS vertex packing the ROM emits `lsl #9 / asr #16 / lsl #16 / lsr #16`
  and sinks the final `lsr` of the z component below the x|y store. Writing
  the z value into a named `int` (holding only `(v << 9) >> 16`) and casting
  to `u16` at the store reproduces that split; casting at the assignment keeps
  all four together, and an `s16` intermediate costs two extra words per
  component because mwccarm will not prove `(x << 9) >> 16` fits in 16 bits.

## 6bn. The `<<9` coalesce is a WHOLE-BLOCK flip driven by ONE component, and its trigger lives in a phase window that excludes the split it needs (func_ov075_0211afb0, div 4 -> 2 measured, ~9000 compiles, 2026-08-29)

6bm parked `func_ov075_0211afb0` at div 4 with the verdict "two unrelated source
shapes converging on one residual is the signature of a real allocator
preference". That verdict was right about the class and wrong about the space:
the residue is reachable. This session moved the measured floor to **div 2** and,
more usefully, identified the exact construct that steers it.

**The residue is ONE allocator decision, not four.** Under our colouring `z<<9`
coalesces into its own source register (`r5`) and `x<<9` into its own (`r0`); the
ROM gives `z<<9` the freed array-base register `r0`, and `x<<9` is then squeezed
out to `sb`. Fix `W_z9`'s colour and the other three words follow. Live ranges are
IDENTICAL under both colourings -- this is pure colour choice, and `wallcrack`
tags all four words `regperm`.

**It is NOT register availability, and an earlier draft of this note was wrong to
say so.** Block A coalesces both shifts in place, but not because it is starved:
`r8` holds `i + 1`, is defined at +0xc8 and last read at +0xd8, so it is free for
the whole of block A -- and the ROM still coalesces there. Both blocks have spare
registers; the ROM coalesces in the first and not in the second. Availability is
not the discriminator, so do not reason from it.

* **Only the Z component's spelling moves the block's colouring.** With the x
  and y components held fixed, sweeping every component independently over
  {named-int + cast at the store, 16-bit-typed intermediate, fully inlined}
  shows x and y are inert in all nine combinations, and z alone flips all four
  words. The flip is a block-level event: changing z re-colours the X chain too.

* **The trigger is a NARROWING CONVERSION anywhere in the z chain -- nothing
  else.** Verified inert (byte-identical output, our colouring): `unsigned`,
  `long`, `unsigned long`, `signed`, `u32`, pointer round trips (`char *` /
  `void *` + `(int)`), an `enum` round trip, a `long long` round trip, and every
  cast that does not change width. Verified to flip: `short`/`s16` on the raw z,
  `u16`/`unsigned short`/`short` on the shift result, and `u16 t = vz << 9`.
  Width-preserving conversions create no node; only a conversion that emits
  shifts does.

* **`short vzb` reaches the ROM's four registers EXACTLY, at div 2.** Split the
  shared `vz` into per-block locals, leave vertex A's `int`, and declare vertex
  B's `short`: block A stays byte-exact, block B's window becomes
  `lsl sb,r0,#9 / lsl r0,r5,#0x10 / asr r5,r0,#0x17 / asr r0,sb,#0x10` -- ROM's
  registers in all four slots, only the two z shift AMOUNTS wrong (0x168,
  0x16c), because narrowing to k bits folds the pair to `lsl #(32-k)` /
  `asr #(32-k+7)`. **This shape is NOT a candidate: it computes a different
  value** (`sign_extend16(z) >> 7`, not `(z<<9)>>16`) and is recorded as a
  codegen probe only. Solving `32-k = 9` wants a 23-bit type, which C reaches
  only through a bitfield -- and every bitfield route is closed below.

* **THE FLOOR, stated precisely: the flip construct and the split it needs live
  in disjoint phase windows.** The ROM splits z's `(u16)` truncation across the
  x|y store (`lsl #16` early, `lsr #16` after the store); that split needs the
  cast at the USE (6bm). But every construct that flips the colouring FUSES the
  four truncation ops at the definition. Placing the 16-bit conversion before
  the packed store flips and fuses (div 14); placing it after the packed store
  splits correctly and does not flip (div 4). One construct, two mutually
  exclusive positions -- the same disjoint-construct-class signature as 6bb's
  fold-seal vs r2-rank pair. The open angle is a construct that presents a
  narrowing conversion to the colourer and dissolves before the truncation is
  lowered; nothing in c99+gccext reaches that intersection.

Closed this session, each a real negative:

* **b56 does not unroll.** The "two emissions came from one 2-iteration loop"
  hypothesis is dead: `for (k=0;k<2;k++)` and its do-while form stay rolled
  (540 -> 460 bytes) under every unroll pragma (`opt_unroll_loops`,
  `opt_unroll_count`, `opt_full_unroll_limit`, `opt_unroll_instr_count`,
  `opt_findoptimalunrollfactor`, `opt_addgotounrolledlooptest`,
  `opt_unrollpostloop`). Loop unrolling is not a lever on this compiler.
* **Inlining is fully flattened before allocation.** A `static inline` emit
  helper reproduces the straight-line bytes exactly, and so do TWO textually
  distinct helpers (`emitA`/`emitB`) called once each -- the honest form of
  "distinct-macro laundering" for a repeated block. Distinct textual origin does
  NOT buy distinct web ancestry here. Three signatures `(base,off)`,
  `(p,base,off)`, `(p)` x six inline pragmas: all div 4.
* **Statement order is not the lever.** All **840** linear extensions of block
  B's dependence poset compile to 52 distinct windows, none the ROM's, best
  still 4. Setup-order permutations (including deferring `off1`/`p1` until after
  block A) and hoisting block B's loads up into block A are all destructive.
* **Pragmas do not reach it.** The full verified 246-name vocabulary at on/off
  (492 compiles) and 7000+ PAIRS of the 60 `opt_*` names leave the window
  untouched; only `opt_dead_assignments off`, `opt_common_subs off` and
  `opt_propagation off` move it, all far worse.
* **Also inert (byte-identical):** `volatile` on any or all of the MMIO stores
  and loads, BB barriers (`do{}while(0)`, `switch(0)`, `if(1)`, a `goto` label,
  a bare scope) around either block, C++ TU form (`extern "C"`, wrapped block,
  C++ inline helper), and asymmetric pointer spellings between the two blocks.
* **NEW EQUIVALENT SPELLING, banked for reuse:** a signed 16-bit bitfield at bit
  offset 7 -- `struct BF { unsigned lo : 7; int hi : 16; unsigned hz : 9; }`,
  read as `((BF *)(p + 8))->hi` -- compiles BYTE-IDENTICALLY to
  `(*(int *)(p + 8) << 9) >> 16`. It is exact, not a launder, and it is a
  conversion at the source level that the compiler canonicalises into the shift
  pair before colouring (hence no flip). Useful anywhere a DS fixed-point
  narrowing reads better as a field than as a shift pair. A 23-bit bitfield
  (`sign_extend23(z) >> 7`, which IS `(z<<9)>>16` exactly) does not fold its
  `>> 7` into the extract, and a 23-bit bitfield LOCAL homes to memory (+44
  bytes), so neither route reaches the amounts the flip needs.
* **The permuter cannot see it either.** Three seeds (the struct-array shape,
  the char*/offset do-while shape, and the bitfield shape) plateau at score 20
  -- exactly the four regalloc words -- over 6000+ iterations each.

Verdict: still parked at div 4 for a SEMANTICALLY VALID source (`// NONMATCHING`),
with the mechanism now named and the open angle narrowed from "some unexplored
spelling" to one specific phase-window intersection. Do not re-run component
naming, decl order, statement order, inline helpers, barriers, volatile, C++
form, or the pragma vocabulary on this function; they are measured and closed.

### 6bn addendum: a `u16` PARAMETER is a free narrowing -- and it pins the conflict to the value that crosses the store

The matched sibling `func_ov075_0211a948` recovers this family as NitroSDK G3
inlines over `volatile u32 *` register macros, so `func_ov075_0211afb0`'s vertex
loop was re-attacked in that shape. Two results worth keeping:

* **`static inline void G3Vtx(u16 x, u16 y, u16 z)` costs NOTHING.** Passing
  `(v << 9) >> 16` to a `u16` parameter compiles to the same 540 bytes as the
  straight-line `(u16)` cast: the argument narrowing IS the `lsl #16 / lsr #16`
  the ROM already emits, so it is free. This is a genuinely size-neutral
  narrowing conversion and therefore a usable colouring lever elsewhere -- unlike
  an `fx16`/`s16` parameter, which costs the +2 words per component that 6bm
  measured (540 -> 564 for one block, 588 for both).
* **The flip follows the value that CROSSES the store, and only that value.**
  With `G3Vtx(u16, u16, int)` -- x and y narrowed at the call, z left wide --
  the colouring does not move (div 4). With `G3Vtx(u16, u16, u16)` it flips
  (ROM's first two words) and fuses (div 14). Splitting the helper into
  `G3VtxXY(u16, u16)` + `G3VtxZ(u16)` so z's narrowing happens AFTER the x|y
  store loses the flip and returns to div 4.

That is the same conflict from a third independent construct family (after the
local's type and the placement of a `u16` local assignment): the narrowing must
sit before the packed store to re-colour the block, and the ROM's z truncation
must be lowered after it. z is the only value in the block whose live range
crosses that store, which is exactly why z is the only component that moves the
colouring -- and exactly why the lever cannot be applied to it for free.

Measured, not argued: holding the flip construct fixed and enumerating all 840
linear extensions of block B's dependence poset yields 56 windows, best div 14,
and the truncation never re-splits.

### 6bn addendum 2: the residue is NOT a compiler-build artifact

6ah puts the ROM's compiler at mwccarm 2.0 build 0053-0062, and `2004/b56` (build
56) sits inside that window, so a build gap is not automatically available as an
excuse here. Measured anyway: the div-4 candidate was compiled with all 25
archived mwccarm builds. Four of them produce a size-correct object --
`2004/b56`, `1.2/base`, `1.2/sp2`, `1.2/sp2p3` -- and **all four emit the same
four wrong words with the same registers** (`lsl r0,r0,#9 / lsl r5,r5,#9 /
asr r5,r5,#0x10 / asr r0,r0,#0x10`). The rest drift in size (`1.2/sp3`,
`1.2/sp4` at 536; `2.0/*` at 396; `dsi/*` at 376). A coalescing preference that
is stable across three years of compiler releases is a property of the SOURCE
SHAPE, not of the build -- so "wrong build" is not an available explanation for
this residue, and neither is it for the class.

### 6bn addendum 3: correction to the mechanism claim, and the last closed axes

**Correction, and it matters for whoever picks this up.** Addendum 1 reads as if the
narrowing conversion IS the ROM's mechanism. It cannot be: the ROM's z truncation
is lowered at the USE (`lsl #16` before the x|y store, `lsr #16` after it), which
is exactly the spelling that does NOT flip. So a narrowing is a SUFFICIENT trigger
for the flipped colouring, not the route the original source took. The ROM reaches
the same allocator state some other way, and that way is still unknown. Read the
floor as "the only trigger found so far conflicts with the required lowering",
not as "only a narrowing can trigger it".

Closed since addendum 2, all inert (byte-identical, div 4, our colouring):

* **Declaration order.** 6bf pairwise-transposition climbing, six restarts
  (canonical + five shuffles), 10 rounds each: every restart converges to div 4
  and ZERO of the four window words ever matches. Decl order does not touch this.
* **Frame layout of the 52-byte matrix local**: `int[13]`, `int[12] + int`,
  `int[4][3] + int`, `char[52]` all identical.
* **Pointer qualifiers and load types**: `restrict` on self/base/p0/p1, `const`
  loads, `s32` vs `int`, `unsigned char` vs `u8`, `&base[off]` vs `base + off`.
* **An inline function's PARAMETER is not a distinct object.** `static inline int
  cvt(int v) { return (v << 9) >> 16; }` and its split forms (`shl9`/`sar16`)
  compile byte-identically to the inline expression -- parameters are flattened
  before colouring, so "pass it through a parameter" is not a web-splitting lever.
* **TU context**: neighbour functions before/after, static data plus a user,
  extra extern declarations -- no effect (this function carries no pragma, so the
  6ay unscoped-pragma trap does not apply).
* **Function signature**: 2, 3 and 4 parameters (the 6ab dropped-argument lever)
  leave it unchanged; giving a callee an extra argument only adds code.
* **Flags (diagnostic only, not admissible)**: `-opt noschedule`, `-opt schedule`
  and plain `-O4` all reproduce the same four words, so the residue is purely
  allocation, not scheduling. `-O4,s` breaks size.
* **6y-1 self-select boosters** (`v = v ? v : v;` as a statement) on the x, z and
  shift webs at every position: best div 10, and the flip never appears.
* **Bitfield type variations**: `int : 16` and `long : 16` at bit offset 7 are
  byte-identical to the shift pair (the useful new spelling); `short : 16`,
  `signed short : 16` and `unsigned short : 16` change the access entirely and
  shrink the function to 524 bytes.

### 6bn addendum 4: the closing numbers, and a fake-match hazard in the permuter

Final tallies for this campaign (2026-08-29/30), so the next attempt can start from
the frontier instead of re-measuring it. Roughly 250,000 compiles.

* **Four permuter seeds, four plateaus.** Structurally different bases, all run to
  a flat line with `--stop-on-zero`:

  | seed | shape | runtime | iterations | best score |
  |---|---|---|---|---|
  | `perm_cp0` | struct-array, `for` loops | 4h00m | 23,468 | 20 (= div 4) |
  | `perm_db` | `char *` + byte offsets, `do/while` | 4h00m | 21,151 | 10 (= div 2) |
  | `perm_bf` | 16-bit bitfield extract | 3h06m | 7,821 | 20 |
  | `perm_d2` | the div-2 narrowing basin | 2h28m | 6,121 | 10 |

* **FAKE-MATCH HAZARD, worth generalising.** `perm_db` reached score 10 on its own
  by inserting `vz = (short)(*(int *)(p1 + 8));` -- independently rediscovering the
  hand-found div-2 point, which confirms the finding but also shows the permuter's
  mutations are NOT semantics-preserving: it will happily add a truncating cast that
  changes the computed value in exchange for a better score. A score-0 result is
  still safe (identical bytes implies identical semantics), but any NON-zero
  permuter output is a byte-distance artefact and must never be banked as a
  near-miss draft or read as progress without checking what it computes. That is
  why this campaign banked no improved DB row: the only thing better than div 4 is
  arithmetically wrong.

* **Wide searches, all flat.** 166,000+ randomized whole-function structural samples
  (loop forms, base/offset/pointer spellings, decl order, `j++` placement,
  per-block per-component spellings, store-address forms); 87,600 evaluations of a
  simulated annealer over (statement order x decl order); 55,296 G3-parameter-space
  combinations; 5,040 (statement order x six equivalent z spellings); 840 block-A
  orders; 7,000+ pragma pairs. Across all of them, **278 distinct register windows
  were observed in that four-word slot, and the maximum agreement with the ROM any
  of them reaches is 2 of 4** -- reached only by the narrowing family. No
  semantically valid shape has ever produced even one of the four ROM words.

* **Annealing inside the flipped basin does not help either**: 47 restarts holding
  the `u16` z intermediate fixed while shuffling orders, decls and the x/y
  spellings never improves on the basin's own floor and never gets past 2 of 4.

Read together with addendums 1-3: the flip is reachable, the only known triggers
are narrowing conversions, they all fuse the truncation the ROM splits, and
availability/order/pragmas/flags/inlining/TU-context/compiler-build are measured
dead ends. The next attempt should look for a construct that is a conversion to
the colourer and dissolves before lowering -- or accept this as the hand-fix
backlog it now is.
## 6bo. A pragma x source-shape PRODUCT moves a web that neither factor moves alone (func_ov075_02116128, div 31 -> 20, 2026-08-30)

`func_ov075_02116128` (ov075, 0xf4) recolours the 4-bit palette field of a 24x4 BG-map
box border: four corner-column entries, then a 24-iteration loop over the top and bottom
rows. Callees are `G2::GetBG1ScrPtr` and `func_02030958`; the only pool words are
`data_0209fc50` and `0x00000fff`.

Its residue is a pure register rotation with the ROM's SCHEDULE reproduced exactly -
instruction for instruction, including where the loop-invariant palette materialises.
The ROM colours `p`=r2, the shifted value=r3, the palette/store temp=r0, `i`=r1; the
natural source colours them r2 / r0 / r1 / r3.

**The stuck web is a birth coalesce, and single-axis sweeps cannot break it.** The value
is born as the call's return in r0 (`add r0,r0,#0xa` / `lsl r0,r0,#0x10`) and the
truncating `lsr` writes in place. It stayed in r0 across ~8,000 compiles: 1,501
declaration permutations, 820 statement interleavings, a 600-cell value x pointer x loop
x order cross, the complete 246-name pragma vocabulary (on and off, two bases), the
expression respellings of 6ab, volatile / struct-member / union / array-element webs,
extra and dropped parameters, a separate loop cursor, the three 6aa address-expression
trees, and an inline helper. Two colourings, no movement.

**What moved it was a PRODUCT of two levers, neither of which does anything alone:**

    #pragma opt_lifetimes off        (or opt_common_subs off)
    ...
    w   = angle << 0x1c;            /* two NAMED variables in the shift chain */
    pal = w >> 16;                  /* and `pal` used at all six store sites  */

Ablated one axis at a time, exactly those two are necessary; the value's type, the
pointer spelling, the loop form, block scoping and statement order are all free. With
them the whole front half of the function is byte-exact through +0x5c, including the
ROM's write-back truncation `lsr r3,r0,#0x10 / subne r0,r3,#4 / lslne / lsrne r3,r0`,
and `p` and `i` both land on the ROM's registers.

**Method rule this pins down:** a web that is invariant across a large single-axis sweep
is not evidence of a floor - it can mean every axis was tested alone. Cross the pragma
vocabulary with the source-shape axes before writing the floor note. The 986-job pragma
sweep here reported "all inert", and the same pragma in the presence of one extra named
local is what broke the wall.

**The residual is a genuine two-attractor pin.** The named palette local is REQUIRED to
break the coalesce, and it is also what takes r3 away from the shifted value:

* palette named and used at all six sites -> shift value r0, palette r3, and the
  palette materialises four words early (div 20)
* palette left implicit (a CSE/LICM temp) -> the ROM's exact schedule, but the shift
  value coalesces back into r0 (div 24)

Every attempt to have both collapses to one attractor or the other: `pal` declared in an
inner block, assigned after each of the five store sites in turn, spelled as `hi / 0x10000`
or as a second copy, or split so the singles use the shifter-operand form - CSE folds it
back every time (div 31).

**Corpus check.** The ROM contains exactly one other instance of this 4-bit field insert
(`lsl #0x1c` then `orr ..., lsr #16`): `func_ov007_020c0308`, which is matched. Its source
spells the value as one named `u32` local, `hi = ((u32)(c << 28)) >> 16;`, under
`#pragma opt_strength_reduction off`. Transplanted here that spelling reaches div 23 with
the same colouring, so the sibling settles the SPELLING but not the allocation.

**Two hypotheses closed with evidence while getting here:**

* **Not a bitfield.** A `u16 tile:12 / pal:4` struct makes mwccarm emit `bic` +
  `orr ..., lsl #12`; the ROM has `and #0xfff` + `orr ..., lsr #16`. The source masks
  explicitly.
* **TU shape is codegen-neutral here, tested in its strong form.** `tu_map.py` does put
  this function and `func_ov075_0211621c` in one TU - but that TU is 82 functions
  (0x02115ab8-0x0211a854), so a two-function unit is not the ROM's unit either. Both
  forms were measured. Weak form: filler neighbours and file-scope statics before and
  after the body, six configurations, no change. Strong form: the two ROM-adjacent
  functions compiled as ONE translation unit from their real bodies, both size-exact (244
  and 916), in both orderings. `func_ov075_02116128` comes out div 20 / div 24 with a
  byte-identical six-register assignment to compiling it alone, and `func_ov075_0211621c`
  comes out div 40 in the combined unit - exactly its standalone value, so the shared TU
  does not disturb its frame layout either, which is what the 6bl floor would have needed.
  Sharing a TU is true here and buys nothing in either direction.

* **The extra web has to be in the value's OWN chain.** A second named local anywhere
  else - in the index chain (`s = t*0x20+0xa0`), in the pointer chain (`q = bg+m; p = q`),
  a copy of the call result, of `bg`, of `t`, of `b` - leaves the palette web on r0, with
  and without `opt_lifetimes off`, `opt_common_subs off` and `opt_dead_assignments off`
  (28 combinations). The lever is not "add register pressure" or "add a web"; it is
  specifically a second name on the dataflow the value passes through.

### 6bo addendum: score the ROLES, not the count, and sample before you climb

Two method points from the same session, both of which changed what the search could see.

**Divergence count has no gradient on a cascading residual.** One wrong web costs a
dozen words here, so div is nearly constant across the whole neighbourhood and a greedy
climb on it wanders. Scoring instead by *how many of the ROM's register roles the
candidate reproduces* - read straight out of the object, six of them: the box pointer,
the shifted value, the palette, the loop counter, the 0xfff mask and the second store's
temp - turns one flat number into six independent bits and makes the local optima
legible. That is 6bf's "score by address region" and 6bg's "measure the register
identity" applied together, and it is what surfaced the third family below.

**A grammar plus uniform sampling finds families hill climbing cannot reach.** 40,000
random draws from a parameterised source grammar (declaration order x chain aliasing x
statement interleaving x pointer form x loop form x type x pragma pairs) turned up a
shape twice that ~30,000 hill-climb compiles never reached from any seed.

Three attractors, and only three, over ~60,000 compiles. Roles are
(p, shiftedValue, palette, i, mask, secondStoreTemp):
```text
    ROM                                   r2  r3  r0  r1  r4  r5
    A  opt_lifetimes off + a two-name
       shift chain, palette named and
       used at all six sites              r2  r0  r3  r1  r4  r5   div 20
    B  plain shape, palette implicit      r2  r0  r1  r3  r4  r5   div 24
    C  opt_common_subs off + a copy
       through a second name              r2  r5  r0  r1  r3  r4   div 34
```
Each is a different rotation and each reproduces a different three of the six roles, and
**the shifted value never once reaches r3** - it is r0, r1, r2 or r5 in every compile,
while r3 is always taken by something else (the palette in A, the loop counter in B, the
mask in C). C is the interesting one: it emits the ROM's in-place `lsl rV, rV, #0x1c` and
puts the palette in r0 and the counter in r1, and its three wrong registers are a clean
r5 -> r3 -> r4 -> r5 rotation in the callee-saved band, a different problem from A's and
B's caller-saved pin.

C's band is pinned as hard as A's and B's, and reading the identity says so precisely.
9,648 compiles against C's exact shape - 201 declaration orders x six pragma sets x two
integer types x `register` on each of the three value names - produce **exactly two**
colourings, `(r1,r5,r0,r2,r3,r4)` and `(r2,r5,r0,r1,r3,r4)`. Declaration order moves the
box pointer between r1 and r2 and nothing else; the shift value never leaves r5, the mask
never leaves r3, the second store's temp never leaves r4. So the callee-saved levers
(6k reverse decl order, 6y `register` and type rank, 6at two-pass fill) are all exercised
and all inert on this shape, which is what makes it a rank pin rather than an unexplored
spelling.

Two more product cells covered after the first pass, both negative and both worth not
re-running: chain aliasing x pragmas x declaration order (37,800 compiles, 19 colourings,
no r3 - the original chain sweep had run with no pragma at all, which is why it was worth
redoing), and the palette's definition point moved to each of the five positions among the
store sites while staying widely used (`opt_common_subs off` decouples placement from
colouring entirely - all five positions give the same div 20).

**A div-6 permuter output that must NOT be banked, and what it proves anyway.** The
permuter arm seeded on family C produced a candidate six words from the ROM - everything
exact except the first store's block. It got there by *sinking the `<< 0x1c` below the
four single stores*, so those four write `angle >> 16`, which is zero, instead of the
palette. Semantically wrong, exactly the failure 6bf warns about; the honest form (shift
before the stores) re-scores at div 31. It is still the most informative object of the
session, because it isolates the rule: **r3 goes to the web that the four single stores
read as a shifter operand, and that web is the one born at +0x4c.** In the ROM those are
the same web because the `lsl` happens IN PLACE inside it; in every honest source the
`lsl` starts a new web that is born later, so the singles read the later web and r3 goes
to whatever was born at +0x4c instead. Every in-place spelling was then tried in the
permuter's own context - `pal = pal << 0x1c`, `pal <<= 0x1c`, a copy then `<<=`, the fully
inline `(pal << 0x1c) >> 16`, and `pal * 0x10000000` - and all six land in family C's
rotation rather than merging the webs.

Banked as a live near-miss, not marked as a floor: what would break it is a lever that
keeps a variable's pre-shift and post-shift values in ONE web across an in-place shift,
or one that outranks a call-return-coalesced web against a loop-invariant one. No rule in
6k / 6q / 6y / 6ab / 6bf spells either yet.

## 6bq. Three homing levers and a store-order rank lever (ov074 daKuriKing_c, two MATCHED, one at div 11, 2026-08-30)

The Goomboss overlay's three unsourced bodies. Two fell on the first day; the third is
one lever short. What they share is that every single blocker was a question about
which locals mwccarm keeps IN MEMORY, and in what order the source touches them.

**1. A struct local assigned FIELD BY FIELD is scalarized; assigned as a COPY it stays
homed.** `func_ov074_021201f0` reads the boss's `Vector3_16` facing, bumps the y by
0x7fff and writes it to the player. Written as

```c
ang.x = *(unsigned short *)(c + 0x8c);   /* ... y, z */
```

mwccarm removes `ang` entirely and the body comes out eight instructions (0x20) short of
the ROM. Written as one struct copy

```c
ang = *(struct Vector3_16 *)(c + 0x8c);
```

the same three `ldrh`/`strh` pairs appear, the local keeps its frame slot at sp+8, and
every later read of a member comes back as `ldrsh` from memory. That difference is the
whole match. The tell in the ROM is mixed load signedness on the SAME logical value:
`ldrh` out of the object (the copy, which does not care about signedness) and `ldrsh` out
of the frame slot (a member read promoted to int). One value read two ways means a
memory-homed struct sits between them.

**2. A homed struct whose address never escapes needs the address taken AT THE READ.**
`func_ov074_021204c0` builds a particle position in a `Vector3` and passes its three
members as separate scalars. There is no call taking `&v`, so the field form scalarizes
again (four instructions short). Neither `&v` in the increment, a `pv = &v` pointer, a
struct copy (which turns into `ldm`/`stm`), a by-value wrapper, nor an inline helper
reproduces the ROM: the copy changes the loads, and every form that keeps a live pointer
costs one extra `add rN, sp, #imm`. What works is taking the address only in the reads
that feed the call:

```c
NewSimple(0xb1, ((int *)&v)[0], ((int *)&v)[1], ((int *)&v)[2]);
```

The address is folded to sp offsets, so no pointer survives, but the local is homed and
the three dead stores the ROM emits come back. This is the missing half of 6bi/6bj: what
homes a struct is any address-taking use, and a use that constant-folds still counts.

**3. Statement order, not declaration order, settled a 10-word register identity.** The
tail of the same function computes an index `k` and an interpolation delta `d`, then uses
both. All six declaration orders of `d` and `k`, and every naming of the twice-read
`f_5e4`, returned div=10 with the identical register assignment - the 6bg signature. The
lever was writing `k = ...;` BEFORE `d = ...;`. With `d` first the index web takes r2 and
the object-field web takes r3; with `k` first they swap, which is the ROM, and the count
goes to zero. Worth carrying because 6ab / 6bf / 6bg all frame the rank rule around
DECLARATION order; here the declaration list was inert across every permutation and the
assignment order was the whole lever.

**4. The rank of hoisted array base pointers follows the source order of the STORES that
consume them.** `func_ov074_02121380` rebuilds three collision cylinders in a loop over
three bones, reading three parallel 3-element arrays. The ROM colours the four hoisted
base pointers strictly descending by frame address - `yoff`@0x28 -> sl, `rad`@0x34 -> sb,
`hgt`@0x40 -> r8, the output vector@0x64 -> r7. Every honest first draft produces the
same set rotated by one: rad -> sl, hgt -> sb, out -> r8, yoff -> r7.

Measuring the identity rather than the count (6bg) showed the map is a function of the
ROLE, not the frame address: permuting the three arrays' DECLARATION order moves them in
memory but leaves each array on the same register. It is also flatly invariant under all
24 declaration permutations, 125 element-type combinations, eight loop-body shapes, six
prologue write orders, both `Vector3 v[3]` / `int buf[9]` / nested-struct spellings of the
frame region, `while` / `do-while` / `for` / pointer-indexing loop forms, and 14 pragmas
plus 91 pragma pairs.

The one lever that moves it is the order the five record stores are WRITTEN IN THE
SOURCE. The stores go to five distinct offsets off one base, so all 120 orders are
semantically identical and the scheduler is free to re-emit them; but the order the
source presents them in decides the rank of the base webs that feed them. Only source
order `0x114, 0x144, 0x118, 0x148, 0x14c` (paired with computing the position temps
z, y, x) reaches the ROM's map, and it takes the residue from 25 words to 11. The natural
order `0x114, 0x118, 0x144, 0x148, 0x14c` NEVER reaches it, under any of the levers above.

That leaves the two-source-shape conflict this function is currently stuck on, in the
shape of 6bn: the store order that gives the ROM's registers emits the 0x144 store three
slots early, and the store order that emits the schedule correctly gives the rotated
registers. What would break it is a lever that reorders emitted stores without reordering
them in the source, or one that ranks a base web by something other than its consumer's
source position.

**One thing this function is still modelled, not proven, on.** Its frame carries 24 bytes
between the last array and the output vector that no instruction ever touches, and a
further word above the output vector. mwccarm drops a local that is never referenced --
verified with unused scalars, unused structs, `volatile` unused locals, arrays whose every
store is dead, and address-taken-but-folded forms, none of which reserve a byte. The only
shapes that reproduce the frame are ones where the untouched bytes belong to the SAME
object as the output vector, which is used: `Vector3 out[3]` with `&out[2]` passed, a
three-member struct with its last member passed, or `int buf[9]` with the vector at
`buf[6]`. All three give the identical object, so the bytes are right and the original
declaration is a guess. Anyone resuming this should treat the array-index-2 spelling as a
placeholder, not as recovered source.

**The near-miss DB will print a smaller number than the gate, and neither is wrong.**
This body reads div=9 in `nearmiss/db.jsonl` and 11 under `tools/match.py`. It is not a
difference in what gets wildcarded -- both sides wildcard the same two reloc slots
(offsets 0x1c8 and 0x370), checked by comparing the candidate object's own relocations
against the ones `config/arm9/overlays/ov074/relocs.txt` implies. The gate counts
differing words PER OFFSET; `nearmiss_db.evaluate` disassembles both sides to mnemonic
strings and scores a `difflib.SequenceMatcher` alignment over that sequence, so a block
that is merely REORDERED realigns and is charged once instead of at every offset it
shifted. Expect the DB to under-report exactly on a residue like this one, whose whole
remainder is a reordering. Score merges by the gate.

**Cheap diagnostic worth reusing.** For a residue in a loop over several arrays, print the
`add rN, sp, #imm` instructions from the candidate object and read off the frame-offset ->
register map, then compare with the ROM's. Four lines of capstone, and it turns "25 words
diverge" into "these four webs are rotated by one", which is a claim a sweep can falsify.

### 6bq addendum: the siege on func_ov074_02121380, and three bugs its audit controls found

The cylinder rebuild came back as the highest-value crack on the board once the port
seating showed the boss's Behavior calls it unconditionally. It did not fall. What the
second pass is worth recording is which of the new levers reached it and which did not,
and one correction to the first pass's own evidence.

**Correction to 6bq as first written.** Its pragma line claimed 14 pragmas and 91 pairs
measured inert. Two of those names (`scheduling`, `register_coloring`) are not pragmas at
all -- exactly the 6as trap -- and the sweep was single-axis on ONE source shape, which
6bp then showed is not a valid way to measure a pragma. Re-run properly: the full
246-name verified vocabulary at on and off, on BOTH attractor shapes, then crossed with
all 120 record-store orders and 6 position-temp orders (5040 cells). No cell beats the
baseline on either attractor. Only one pragma is even size-neutral and live here,
`opt_dead_assignments off`, and it moves the body the wrong way (25 -> 62, 11 -> 48).
The original conclusion survives; the evidence behind it did not, and would not have
been worth citing.

**6bp's named-address lever is real here but points the other way.** This residue IS a
base-pointer rank problem, so the lever should have been the one. Measured: deleting the
name of the record cursor `p` is completely free -- identical size AND identical
divergence on both attractors, so mwccarm canonicalises that one; deleting the name of
the rolling bone cursor `q` costs 8 bytes; and giving the three array bases names
(`int *py = yoff;`) costs more still and drifts every frame offset. The rank of the four
hoisted bases is set by the STORE SOURCE ORDER (6bq above), not by naming, and 6bp's
lever cannot reach it. Both levers are about address-web rank; they are not the same
lever, and a body can be immune to one and controlled by the other.

**Statement RELOCATION (6bp) also does not contain the move.** Climbed to a local
optimum from both attractors over the relocation neighbourhood of all three ordered
statement sequences (127 neighbours a round). The register-correct attractor is already
a local optimum at 11; the schedule-correct one climbs 25 -> 12 and stops.

**6bn's spellings do not apply and were not faked.** Its narrowing-conversion triggers,
the size-neutral `u16` inline parameter and the signed-16-bitfield-at-offset form all
need a value that is narrowed on the way to a store. Every value this body moves is a
32-bit Fix12 that is stored whole, and the one small integer it passes (the bone index)
is a word load from a word array. There is no semantically valid narrowing to introduce,
so the family is recorded as not-applicable rather than as measured-inert.

**Two new axes swept, both inert.** The order of the three bone-position stores into the
cursor (6 orders x 6 temp orders x all 120 record-store orders, 4320 cells), and a
one-variable reuse where a single local holds the y component and is then reassigned to
the record base -- which is what the cartridge's `ldr fp,[r4,#0x3b0]` into a
callee-saved register looks like. The reuse is interesting and still lost: it INVERTS
which store order colours correctly (RZHYX instead of RXHYZ) without lowering the floor,
which is more evidence that the rank is a function of consumer order rather than of the
webs themselves.

**The permuter hazard from 6bn addendum 4, reproduced exactly.** ~16000 iterations over
four runs on two structurally different seeds. On the div-11 seed every candidate the
permuter scored BETTER than its base (445 and 575 against 665) is size-drifted to 0x378
or 0x37c -- worse by the byte oracle, better by a scorer that charges 60 per reorder and
5 per regalloc. With an ordering residue the two metrics point in opposite directions;
score every permuter output through the byte oracle before believing it.

**Three audit-harness bugs the controls caught, none of which the PASS side could show.**
Adapting the ov034 harness (6bp addendum) to this function needed three fixes, and every
one was invisible until a deliberately broken control failed to be caught:

1. **`ldm`/`stm` were unimplemented.** The ov034 body had none; this one copies its
   3-word bone table with them. This one at least announces itself -- the interpreter
   raised on the first instruction rather than quietly agreeing.
2. **Each side must map its OWN initialised data.** The bone table is a local array
   initialiser whose image lives in the object's `.data` and is reached through a pooled
   address that the harness patches with the ROM's address. Both sides therefore read the
   ROM's table, and a control that changed the candidate's table to {1, 8, 9} passed.
   Any pooled data the candidate defines itself has to be mapped per side.
3. **Seeding that never reaches an arm silently narrows what the audit covers.** The
   ground tests compare a bone y plus a scaled bias against the boss's own y; with the
   bias always negative and the bone heights bracketing the threshold too tightly, the
   `>=` arm never ran, so the boundary control (`<` -> `<=`) and the RMW-mask control
   both passed. Fixed by deriving the stub's reported bone height FROM the bias, so one
   selector lands exactly on the threshold and one lands above it.

Worth carrying beyond this function: give every discriminating input its own digit in a
mixed-radix trial index. That makes `--trials N` visit the state lattice exactly once
each instead of sampling it, so "exhaustive over the discriminating states" becomes a
claim with a number behind it (2700 here), and report the per-arm coverage next to the
PASS -- a branch never taken is a branch the audit cannot speak for.

## 6bp. A named local holding an ADDRESS outranks the compiler's own address temp, and that rotates every register below it (`_ZN7Wiggler8BehaviorEv`, div 122 -> 20, 2026-08-30)

The Wiggler's `Behavior` (ov034, 0x02112b5c, 0x6e0, 440 words) sat banked at div 122
with a floor claim about "a global r6/r7 vs cylinder r5/r4/r6 allocation tradeoff". The
tradeoff was real but it was a symptom. One source change moved 85 of the 102 words that
eventually fell.

**The lever.** `ldrh` carries only an 8-bit immediate offset. The angle field this
function reads lives at `c + 0x446`, which does not fit, so mwccarm *must* materialise a
base register and then load at `[base, #0x46]`. The cartridge's base is a
**compiler-generated address temp**. The draft spelled the same value as a named local,
`char *angbase = c + 0x400`, and a named local does not take the same rank in the scratch
file as the temp the compiler would have invented. Measured on this body: as a named
local the web landed in **r4, the TOP of the contended group**; the cartridge has it in
**r0, the bottom**. Every other web in the group shifted one slot to compensate, so both
blocks that used it came out as a clean uniform rotation:
```text
    ROM      angbase=r0  round=r1  zero=r2  hx=r3  hz=r4  scale=r5  tbl=r8
    named    round=r0    zero=r1   hx=r2    hz=r3  angbase=r4  scale=r5  tbl=r8
```
Deleting the local and writing `*(u16 *)(c + 0x446)` inline at both use sites took the
head block 23 -> 2 and the particle block 48 -> 2, total **105 -> 37**. The same lever
applied to the row cursor `char *p2 = c + (i << 1)` (spelled inline at its two uses) took
**27 -> 20**. Both are the identical shape: *an address the compiler was going to
materialise anyway must not be given a name*.

This generalises past this function. Any field past an addressing-mode limit forces
materialisation - 8 bits for `ldrh`/`ldrsh`/`strh`, 12 for `ldr`/`str`/`ldrb` - so any
struct member above 0xFF (halfword) or 0xFFF (word) is a candidate. When a residue is a
uniform rotation of the low registers, look for a named local holding a base address
before reaching for declaration order.

**Declaration order was not the lever, and 6bf's neighbourhood does not contain the one
that was.** The full pairwise-transposition climb over all 14 declarations (74 compiles)
found nothing, twice, at two different divergence levels. What worked was a different
operator: **relocating a single statement a long distance**. Moving
`angbase = c + 0x400` out of its block to before an earlier call was worth 131 -> 107 on
its own, and no transposition of adjacent declarations contains that move. A relocation
neighbourhood is cheap to build (only `local = expr;` with no memory or call on the
right-hand side, at conditional depth 0, moved between the last writer of what it reads
and the first reader of what it writes) and it is worth having beside 6bf's transposition
climb. Watch one trap while building it: a **declaration line is not a reader**, and
counting it as one gives every candidate zero legal positions and a silently empty search
(the first version of this tool reported "local optimum" after 118 candidates because of
exactly that).

**A pragma's value is SHAPE-DEPENDENT, and measuring it on the wrong shape is how a
sweep lies to you.** A 1024-point product sweep (every subset of the eight verified
pragmas x four source shapes, scored per address region per 6bf) found
`opt_strength_reduction off` load-bearing and `opt_common_subs off` apparently inert, and
that second reading was carried forward as "the draft has been hauling a dead crutch". It
was wrong, and the way it was wrong is the transferable part. Every shape in that sweep
was seeded at or near the div-122 draft - i.e. BEFORE the named-address locals came out.
Re-measured through the gate on both bodies:
```text
    div-122 draft   with `opt_common_subs off` 122   without 122   -> genuinely inert
    shipped body    with `opt_common_subs off`  20   without  27   -> worth 7 words
```
So the pragma did not start out dead and stay dead: it **became load-bearing once the
named-address locals were deleted**. Removing `angbase` and `p2` is exactly what stops the
source from pre-sharing those addresses by hand, which is what leaves a common
subexpression for the pragma to have an opinion about in the first place. The two levers
are coupled, and a single-axis pragma sweep run before the source-shape lever cannot see
it.

Two rules out of this. A pragma sweep is only valid for the source shape it was run on -
re-run it after any structural lever, never carry the verdict across. And never write
"inert" unqualified: say inert *on which body, measured through which gate*.
Measured that way on both bodies: `scheduling off` is genuinely inert (122/122 and
20/20), and `opt_lifetimes off` is catastrophic on both (159 on the draft, 122 on the
shipped body). Only `opt_common_subs off` flipped -- and it flipped because a source
lever, not a pragma, changed what it had to work on.

**What the residue is, and why the permuter is the wrong tool for it.** 16 of the final
20 words are pure REORDER: our exact instruction, with the registers already correct,
appears in the cartridge a few words away. decomp-permuter's stock scorer charges 60 per
reorder against 5 per regalloc, and on this body it duly found a "better" candidate -
545 against a 755 base - whose actual byte divergence is **36, worse than the 20 it
started from**. That is the upstream author's own caveat (quoted in
notes/research-matching-levers.md F4.1) reproduced as a measurement: when the residue is
ordering rather than allocation, the permuter's score and the byte oracle point in
different directions, and following the score walks away from the match.

### 6bp addendum: three bugs a differential-execution harness hides unless you make it fail first

The NONMATCHING body ships with a differential-execution audit (an ARM interpreter over
capstone's detailed decode - no unicorn on the build box - running the cartridge bytes
and the candidate's compiled bytes over the same randomised state). The five
deliberately-broken control candidates were not a formality: they caught three bugs, and
every one of them was invisible from the PASS side because it hit both sides equally.

1. **capstone's condition codes are 1=EQ, 2=NE.** Having those two swapped inverted every
   conditional in both runs at once, so a correct candidate still compared equal while
   the branches actually executed were the wrong ones.
2. **capstone renders `lsl rd, rn, #k` with TWO operands**, carrying the amount in
   `ops[1].shift` (it is MOV-with-shift underneath). Reading `ops[1]` as the amount makes
   a *register* the shift count, so the two schedules shifted by different amounts.
3. **The function's own bytes must be mapped as DATA.** Every `ldr rN, [pc, #k]` reads the
   literal pool at the tail of the function; without the code image in memory those loads
   return noise, so every pooled constant and every global address is garbage and the
   pool-dependent branches are unreachable.

Two design points worth carrying: compare the **final value of every written byte**, not
the write sequence, because two correct schedules legitimately reorder independent stores
(call *order* is semantic and should still be compared); and truncate each recorded call's
arguments to that callee's **arity**, because registers above it hold caller garbage that
differs harmlessly. Finally, uniform random memory is not enough - a rare-path control
(`state == 2`, one in 256) went undetected until the discriminating fields were driven
through their interesting values deliberately, and a call stub that always returns
non-zero leaves every `if (f() != 0)` else-arm unreachable. Coverage was 21 of 22 callees
before the harness was trusted (a reviewer's independent re-run read 22 of 22).

State the harness's guarantee precisely, per control. Four of the five controls here are
size-neutral and are caught by EXECUTION; control 2 (dropping the `+ 1` on the second
sincos index) changes the instruction count, so it is caught by SIZE alone and never
exercises the comparator. A control that the byte gate would have rejected anyway proves
nothing about the interpreter, so count only the size-neutral ones when claiming what the
audit can detect.

### 6bs TERNARY ORIENTATION is a colouring lever: two rigid regimes, and the bridge between them

Measured on `_ZN14TTC_MovingBeam8BehaviorEv` (ov065, 0x0211bd8c, 0x178), lane TTC of run
vsdec, ~700 compiled variants across 11 product sweeps. The whole body reproduced on the
first try except one trailing clamp block, which cost the entire session and turned out to
be a single lever nobody had written down.

THE SHAPE. A range test followed by a clamp:
```c
    int y = *(s32 *)(c + 0x60);
    int lo = *(s32 *)(c + 0x320);
    int hi = *(s32 *)(c + 0x324);
    int in = 0;
    if (y >= lo)
        in = (y <= hi);
    if (in == 0) {
        *(s32 *)(c + 0x60) = <clamp>;
        ...
    }
```
TWO RIGID REGIMES. How you spell `<clamp>` decides the colouring of ALL FOUR values, and
there are exactly two outcomes. Writing the clamp as open-coded `if`s:
```text
    y = r0   in = r3   hi = r2   lo = r1
```
Writing it as a single ternary expression:
```text
    y = r2   in = r1   hi = r0   lo = r3
```
These are not tendencies, they are absolute over every spelling measured. The regime is
chosen by the clamp, and then nothing else moves it.

WHAT IS INERT (measure once, never pay for it again). None of these changed the colouring
OR the score, in any combination:
  - declaration order of the flag: 4 slots x 6 orders of the loads
  - flag type: `int` / `u8` / `s32` / `bool` / `s16`
  - `if (!in)` vs `if (in == 0)`
  - wrapping the block in an extra nested scope
  - hoisting all four locals to FUNCTION scope, all 24 permutations
  - `static inline` / `inline` / `static __inline` helper for the range test: the inlined
    helper is BYTE-IDENTICAL to the open-coded form, so "extract it into an inline
    function" is not a distinct lever here, only a different way to write the same IR
  - a dead extra load: mwcc eliminates it, so it cannot be used as a colouring shim

Within a regime, declaration order does one thing and one thing only: the memory-loaded
locals take their registers in REVERSE order of declaration. A constant-initialised flag is
excluded from that chain and appended at the end. That rule held on all six orders.

THE ROM SAT BETWEEN THE REGIMES: `y=r0 in=r1 hi=r2 lo=r3` -- the ternary regime's flag and
low bound, with the open-coded regime's other two. 0 of 198 spellings in the widest cross
reached it, and the permuter could not either (two structurally different seeds, 4000+ and
2500+ iterations, both flat at their base score).

THE LEVER. The bridge is the ARM ORIENTATION of the inner ternary. These are semantically
identical and compile differently:
```text
    (y <= hi) ? y : hi      ->  movgt <y's reg>, <hi's reg>     select lands in y
    (y >  hi) ? hi : y      ->  movle <hi's reg>, <y's reg>     select lands in hi
```
mwcc materialises the FIRST arm into the result register and conditionally moves the second
into it. So the arm you write first decides which operand's register the result occupies,
and therefore which register is free for everything downstream. Swapping the arms of an
inner ternary is a free, semantics-preserving colouring move that no other axis reproduces.

The match was:
```c
    *(s32 *)(c + 0x60) = (y < lo) ? lo : ((y > hi) ? hi : y);
```
with the plain block-scope declarations in order y, lo, hi. Changing `(y > hi) ? hi : y`
back to `(y <= hi) ? y : hi` and nothing else moves it from a byte match to seven words off (four in the clamp itself plus three range-test loads that recolour with it; reviewer-measured).

HOW TO USE IT. On any near-miss whose residue is conditional moves plus the registers
around them, sweep BOTH arm orders of every ternary and every `if/else` that selects a
value, as a product with the open-coded-vs-expression choice. Two axes, four cells, and
they are cheap. Do not sweep declaration order first -- on this family it is nearly inert
and it burned most of the session.

METHOD NOTE worth keeping. What actually solved this was instrumenting the sweep to report
the COLOURING each spelling produced (which register each load and the flag landed in), not
just the divergence count. The score alone said 7, 8, 9, 13 with no gradient to climb; the
colouring table showed two attractors and made the missing cell obvious. Score-only sweeps
hide the structure. Also: the permuter needs a C-shaped base (pycparser cannot parse
`extern "C"` or `class`), and for a body like this one a C file with the mangled name as a
plain identifier and `this` as an explicit first parameter reproduces the C++ output
word-for-word, so the residue can be worked in C and ported back.

## 6br. `tst` is a compiler-VERSION split, `long_calls` is not an mwccarm pragma at all, and `2004/b56` only half-screens pragmas (2026-09-05, arm9 handback run m100)

Three tooling-level results from the 0x0205xxxx handback pile. None of them is a source
shape -- they change which functions are even reachable, and they retire two levers that
have been re-tried by hand at least twice.

### 1. `tst Rn,#imm` vs `ands Rd,Rn,#imm` is decided by the compiler build, not by the source

The 1.2 line and `2004/b56` never emit `tst` for a masked branch; they emit `ands` into a
scratch register. The 2.0 line and the whole DSi line always emit `tst`. Same source, same
flags, twenty-five builds:

```c
typedef unsigned int u32;
void tst_probe(u32 *p, u32 x) { if (x & 2) *p = 1; }
```

```
1.2/base 1.2/sp2 1.2/sp2p3 1.2/sp3 1.2/sp4 2004/b56   ->  021011e2  ands r1, r1, #2
2.0/{base,sp1,sp1p2,sp1p5,sp1p6,sp1p7,sp2,sp2p2,sp2p3,sp2p4}
dsi/{1.1,1.1p1,1.2,1.2p1,1.2p2,1.3,1.3p1,1.6sp1,1.6sp2}
                                                      ->  020011e3  tst  r1, #2
```
(the following three words -- `movne r1,#1 / strne r1,[r0] / bx lr` -- are byte-identical
across all twenty-five, so this is the whole delta.)

Nothing on the source side moves it. It is not a peephole you can provoke and it is not an
`optimize_for_size` effect; the split is clean at the 1.2/2004 vs 2.0/dsi line.

**How to apply.** A `tst` in the target is a VERSION ORACLE, and a cheap one -- it is
visible in the disassembly before you write any C. If the ROM bytes for your function
contain `tst`, `2004/b56` (the canonical build, 6ai) can never reproduce them, and every
divergence count you collect under `--version 2004/b56` is measuring the wrong compiler.
Re-run under `--version 2.0/<build>` (or a dsi build) from the start, and if it lands, pin
it by appending `<name> 2.0/<build>` to `config/rombuild-versions.txt`. Conversely an
`ands Rd,Rn,#imm` whose result is dead rules the 2.0 and dsi lines OUT. The ROM mixes
both, which is consistent with 6ah: the shipping toolchain was not one binary.

**The oracle classifies COMPILER OUTPUT only.** Hand-written asm uses `tst` freely and
carries no version signal at all, so the rule above says nothing about a hand-asm body.
`func_0205a588` -- the subject of 6bs, banked as a HAND-ASM PRIMITIVE -- has four `tst`
instructions (+0x08, +0x38, +0x6c, +0x74) and that does not disqualify `2004/b56` for
anything, because no compiler wrote those words. Apply the oracle to a body you have reason
to believe a compiler produced, and re-check the banner before you re-pin a version on it.

### 2. `#pragma long_calls` is not an mwccarm pragma -- and the veneer it was supposed to
produce is what a plain tail call already emits

`#pragma long_calls on` draws `warning: illegal #pragma` from every build in the tree that
supports the screen (all of 1.2/*, all of 2.0/*, all of dsi/*), exactly like the invented
`#pragma zzz_totally_bogus_name`. It is an ARM/RVCT pragma that was borrowed into decomp
folklore; mwccarm does not have it. Output with it is byte-identical to output without it
on every build. Every "tried long_calls, inert" line anywhere is a 6as false negative, and
the lever should not be re-tried.

**51 committed sources carry it right now** (`grep -rl '#pragma long_calls' src/`) -- 50 of
them overlay files (19 in ov007, the rest spread over
ov002/006/014/015/016/022/029/030/036/063/064/065/066/073/079/080/091/095/098) and exactly
ONE arm9 file, `src/func_0205d4a0.c`. Most sit under a header comment that says the pragma
is what emits the pooled veneer. They all still byte-match, because the pragma is inert, but
the comment is wrong and every one of those compiles now prints `warning: illegal #pragma`
under the `-w illpragmas` in `DEFAULT_FLAGS`. Deleting the line from those files is a no-op
on the bytes; only the comment needs rewriting.

The reason it looked plausible is that the veneer shape it was supposed to force is already
the default. With `-interworking` (which is in `DEFAULT_FLAGS`), a function whose entire
body is one call in TAIL position lowers to a pc-relative indirect long branch, no pragma:

```c
extern void far_target(void);
void veneer_probe(void) { far_target(); }
```
```
-interworking on : ldr ip, [pc] ; bx ip ; <reloc word at +8>     (12 bytes, no frame)
-interworking off: b far_target                                   (4 bytes)
```
The return type is irrelevant (`return far_target();` on an `int` function is the same three
words). Add a SECOND call and it is no longer a tail call, so you get the ordinary
`stmdb sp!,{lr} / sub sp,sp,#4 / bl / bl / add / ldm / bx lr` frame instead. That is the
whole mechanism: tail position plus interworking, nothing else.

**Corpus check that settles it.** Scanning arm9 for every 12-byte ARM function in
`config/arm9/symbols.txt`, the pc-relative-load-then-`bx` shape appears 63 times in three
register forms:

```
ldr r12,[pc,#0] ; bx r12   58 sites   <- compiler output; 56 have a src file, 0 NONMATCHING
ldr r0, [pc,#0] ; bx lr     4 sites   <- not a veneer: returns a constant address
ldr r1, [pc,#0] ; bx r1     1 site    <- func_02057014, the only one
```
So the ip form is a solved shape (a one-line tail call in a plain `.c` file reproduces it;
of the 56 banked sources, 33 are plain `.c` files that do exactly that and the other 23 are
`.cpp`), and `func_02057014` routing the address through **r1** is a 1-in-63 outlier that no
register-allocation lever reaches -- ip is where the compiler's tail-call thunk always puts
it, and there is no C construct that names r1 for a branch target. That function is a FLOOR
(hand-written), not a missing source spelling.

### 3. `2004/b56` DOES honour `-w illpragmas`, but only for pragma ARGUMENTS, not for pragma NAMES

6as says b56 "does NOT support `-w illpragmas`". That is half right, and the half that is
wrong is the dangerous half. Measured on the same three files:

```
                       #pragma long_calls on   #pragma zzz_bogus_name off   #pragma opt_propagation bogusarg
1.2/{base,sp2p3,sp4}   illegal #pragma         illegal #pragma              illegal #pragma
2.0/{base,sp2p3}       illegal #pragma         illegal #pragma              illegal #pragma
dsi/{1.2p2,1.6sp2}     illegal #pragma         illegal #pragma              illegal #pragma
2004/b56               (silent)                (silent)                     illegal #pragma
```

b56's diagnostic fires on a real pragma given a bad argument and stays silent on a name it
has never heard of. So under b56 a clean compile proves your pragma's ARGUMENT parsed, and
proves nothing whatsoever about whether the pragma exists. A typo'd or invented name still
reads as "inert" there.

**How to apply.** Unchanged from 6as in practice, but for a sharper reason: screen every
pragma name against a 1.2 build once (`--version 1.2/sp2p3`, or any `--all` run) before
banking a pragma result, even when your function's pinned version is b56 and even when the
b56 run was silent. Silence from b56 is not evidence.

## 6bs. The ROM's compiler does not recycle a register that just died; every build we own does, and that single delta is the whole residue on four separate arm9 floors (2026-09-05)

**The rule, in one probe.** Give a function a parameter that dies at the first store and a
temporary born immediately after it:

```c
typedef int s32;
void reuse_probe(s32 *m, s32 a, s32 b)
{
    m[0] = b;          /* b (r2) dies here */
    m[1] = 0;          /* which register does the 0 get? */
    m[2] = 0;
    m[3] = 0;
    m[4] = a;
}
```

Every installed build recycles `b`'s register:

```
1.2/base 1.2/sp4 2004/b56 2.0/base 2.0/sp2p4  ->  str r2,[r0] ; mov r2,#0 ; str r2,[r0,#4] ...
```
(the dsi line merges the pair into `stm r0,{r2,r3}` and does not answer the question.)
The allocator takes the LOWEST free scratch register, and "free" includes one that died on
the previous instruction. The ROM's compiler skips it and takes the next register that has
not been used yet.

**Four sites, same delta -- the entire residue on `func_0205256c`, and three of the fourteen
divergent words on `func_0205a588`.**

| site | dead register at that point | ROM picks | every build picks |
|---|---|---|---|
| `func_0205256c` +0x04, the shared `0` temp | r2 (`c`, stored at +0x00 and +0x02) | `r3` | `r2` |
| `func_0205a588` +0x10, the loaded halfword | nothing dead here -- r3 and ip are both fresh | `ip` | `r3` |
| `func_0205a588` +0x5c, the loop end pointer | r3 (`n & ~3`, dies at this instruction) | `ip` | `r3` |
| `func_0205a588` +0x7c, the tail halfword load | r2 (`n`, dies at the `bxeq` above) | `r3` | `r2` |

Row two is NOT the recycle rule. Nothing has died at +0x10, so what it shows is the same
preference one step earlier: handed two untouched scratch registers, the ROM's compiler
reaches for `ip` where every build we own takes the lowest one, `r3`. It may well be the same
underlying bias, but only rows one, three and four are the dead-register case proper.

On `func_0205256c` this is provably the ONLY residue: a source shape exists whose fourteen
Thumb instructions match the ROM one-for-one in mnemonic, operand form and order, and the
diff is a whole-function `r2 <-> r3` swap. The shape is one reused local plus one short-lived
local:

```c
#pragma thumb on
#pragma opt_propagation off
void func_0205256c(s32 *m, s32 s, s32 c) {
    s32 t, ns;
    m[0] = c;  m[8] = c;
    t = 0;   m[1] = t; m[3] = t; m[5] = t; m[7] = t;
    ns = -s;
    t = 1;   t = t << 12;
    m[6] = s; m[2] = ns; m[4] = t;
}
```

**What does NOT move it** (measured on that shape, ~200 compiles): all six declaration
orders of the temps and in-place vs split declaration (once the declaration is split from the
assignment, RANK STOPS MATTERING ENTIRELY -- a useful negative against 6aj, which holds for
initialised declarations, not for `s32 a, b;` followed by assignments); a twelve-statement
pairwise-transposition hill-climb over the whole body (6bf method, four restarts); all
fourteen real `opt_*`/`optimize_for_size`/`global_optimizer`/`peephole` pragmas in both
directions; `register`, `const`, `unsigned`, `short`, `long`; and all twenty-five installed
builds (1.2 and dsi both give the same count, 10 of 14, though dsi hoists the `rsbs` so its
ten are not literally 1.2's ten; 2.0 gives 11). On `func_0205a588` the same conclusion falls
out of a 240-cell product sweep of head/mid/loop/tail spellings -- all 240 compile to the
SAME 37 words, so block-local spelling is fully canonicalised there -- and a 58-cell pragma
sweep on top of it, every cell 14/37.

**The one construct that flips it is not usable.** Sourcing the temp from a `volatile` local
(`volatile s32 zz = 0; s32 z = zz;`) does make the allocator skip the dead register and take
r3 -- but it also materialises the stack slot, so the function grows by eight bytes, 0x1c to
0x24. The spelling has to be exact: feeding the volatile into the EXISTING reused temp does
not flip the register at all (r2 comes back, at the same 0x24), and only a SEPARATE named
local fed from the volatile reaches r3. An extern-const source (`s32 z = g_zero;`) skips it
too and costs a literal-pool load. There is no register-resident construct that skips a dead
register, because the choice is made after every source-level distinction has been erased.

**How to apply.** When a near-miss is a pure register permutation AND the permutation is
"the ROM used a fresh register where we recycled a just-dead one", stop sweeping source
shapes: it is a build delta, in the same class as 6av's outgoing-arg phi coalesce, and the
ROM's own compiler (CW NITRO V0.6.1, 6ah, unarchived) is the missing piece. Bank the
shape-exact draft as a seed and move on. Conversely, if a near-miss has the ROM recycling a
dead register and your draft using a fresh one, the defect is yours and is worth chasing --
that direction is the compiler's default and is always reachable.

## 6bt. mwccarm's only ldm/stm path is the equal-width block move, so a 3-in / 4-out burst is not compiler output (func_02052514, 2026-09-05)

`func_02052514` widens a 3x3 fx32 matrix to a 4x4 in three bursts:

```
ldm r0!, {r2, r3, r4}          <- 12 bytes in
stm r1!, {r2, r3, r4, ip}      <- 16 bytes out, ip pre-loaded with 0 at the top
```

**mwccarm cannot emit this, and the reason is structural.** There is exactly one construct
that produces `ldm`/`stm` at all: the inline block move for a whole-struct assignment, and it
always loads and stores the SAME width -- `d->v = *s` on a 12-byte struct gives
`ldm rX, {a,b,c}` / `stm rY, {a,b,c}` (no writeback, and it burns two `add` instructions per
row recomputing the addresses). Nothing merges free-standing `ldr`/`str` into a multiple:
writing the row as three loads into named locals followed by four stores gives nine separate
`ldr` and twelve separate `str`, in any order, with or without pointer post-increment
(measured, six unrolled shapes, 0x50-0x78 bytes against the ROM's 0x3c; a rolled
post-increment loop lands at 0x44, still with no data `ldm`/`stm`).

So the ROM's fabricated fourth word -- a zero that rides along in the store multiple but was
never in the load multiple -- has no C spelling. Combined with the leaf frame
`stmdb sp!,{r4}` / `ldm sp!,{r4}` (mwccarm spends `push {r4,lr}` here because its block move
needs `lr` as a scratch), this is a hand-written primitive and the existing HAND-ASM header on
`src/func_02052514.c` is correct. Do not re-open it.

The transferable test: **an `ldm`/`stm` pair with different register counts, or an `stm` whose
register list contains a value the matching `ldm` did not load, is hand-asm.** Equal widths
with two `add`s in front of them is compiler output.
