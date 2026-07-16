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

---

*Add to this file whenever you learn a new codegen rule. It is the project's accumulating
model of mwccarm - the cheaper, local alternative to disassembling the compiler itself.*
