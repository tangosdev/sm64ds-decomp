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
```
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

```
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

```
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

```
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

```
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
out. Found on func_02009e70 (Camera::Update); it was worth ~1300 divergences on its own.

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

Second launder note, from func_020316d8: **the launder defeats mwcc's range-folder.**
`short w = <byte-valued expr>;` gets range-folded and both shifts of the `lsl #16 / asr #16`
pair vanish; `(short)(int)LAUNDER(expr)` restores the packed form and the correct size. It
did not pay off there on divergence, but the range-folder is beatable and that is worth
knowing.

### Read register ROLES, not the divergence count, to order two residues

func_020316d8 looked like two independent residue classes. Mapping which web owns which
register showed one is downstream of the other: the free-register *pools* differ only
because `rowstep` lands in `lr` for us and `r3` for the ROM, and everything in the second
cluster is a cascade of that. A divergence count cannot see this; a role map can. Same
technique found that on func_02009e70 a single assignment (`self -> r8`) accounts for 145
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
