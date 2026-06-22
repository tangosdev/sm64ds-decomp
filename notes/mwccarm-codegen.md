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

## 7. Workflow implications

- **Free tiers first, every cycle:** `clone.py` (byte-identical retarget) then `paramclone.py`
  (same skeleton, substituted immediates) - they harvest the families each new match unlocks
  for $0. Then `sweep.py` applies the templates above across all modules.
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
