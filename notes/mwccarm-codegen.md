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

Two more residual classes confirmed source-invariant across many C spellings; when the diff
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

Both classes live in nearmiss/db.jsonl at div 1-2; candidates for an asm-block close-out
(sec 8) if their subsystems ever need completion.

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
- **Vary the launder spelling per block to stop cross-call CSE.** Repeating the identical
  u64-mask expression in two blocks invites mwccarm to hoist the shared computation above
  the call; spelling the mask differently per block (zero-extend vs sign-extend placement)
  keeps each materialization local (func_ov090_02131b94).
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

## 7. Workflow implications

- **Free tiers first, every cycle:** `clone.py --apply` (byte-identical retarget) then
  `paramclone.py --apply` (same skeleton, substituted immediates) - they harvest the families
  each new match unlocks for $0 (dry-run without `--apply`, like every banking tool). Then
  `sweep.py --apply` applies the templates above across all modules.
- **One representative -> a template.** When you hand-crack a function that clearly recurs,
  add a rule to `swarm.py` and `sweep.py --apply` clears the cluster for free. That is how the
  whole template tier was built and it's the cheapest way to lower difficulty in bulk.
- **The oracle stays.** Understanding shrinks the number of compile-and-check iterations; it
  never removes the check. "Matched" means the bytes are identical, full stop.

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

---

*Add to this file whenever you learn a new codegen rule. It is the project's accumulating
model of mwccarm - the cheaper, local alternative to disassembling the compiler itself.*
