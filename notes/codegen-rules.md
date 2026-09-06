# mwccarm codegen rules (condensed doctrine)

**This is the load-bearing rulebook, condensed for loading whole.** It condenses
`notes/mwccarm-codegen.md`'s curated sections `## 1`-`## 6d` and `## 6az` — the parts of
that 317KB file meant to be read as rules, not as a dated discovery log. Everything from
`## 6e` onward in that file is an append-only, chronological lab notebook (one section per
investigation, dated); it is not condensed here. Where this file states a rule, the source
file's section label is cited in parentheses so you can read the fuller derivation,
evidence, and worked examples — this file keeps the mechanism and the measured numbers,
not the narration.

How **mwccarm 1.2/sp2p3** (`-O4,p -enum int -lang c99 -char signed -interworking -proc
arm946e -gccext,on -msgstyle gcc`) turns C into the exact ARM bytes in the ROM (2004/b56 is
the canonical matching pin; findings here hold on both). **This does not replace the
oracle** (`tools/match.py`) — register allocation especially is non-local and cannot be
predicted by hand, so every candidate is still compiled and byte-diffed. Treat this as the
priors that get a first draft close.

---

## 1. Ground rules of the build (mwccarm-codegen.md §1)

- **Reloc slots are wildcards.** Every `bl`/`blx` target and every pc-relative `ldr` of an
  external address is compared as a wildcard by the oracle. The *name* you call only has
  to compile, not be byte-correct — but resolve the real callee anyway, since it keeps the
  C correct and the argument count right.
- **Symbol size includes the trailing literal pool.** Pool words after a function
  disassemble as junk (`andeq`/`andseq`). `ins[-1]` is therefore **not** the `bx` — find
  the epilogue (`mov r0,r4; pop; bx` or `mov r0,#k; add sp; ldm; bx`) by scanning, never by
  negative index.
- **`-O4,p` reorders and merges aggressively.** Pool loads hoist above their use, repeated
  globals collapse to one pool word, loads batch before stores. Write straightforward C and
  let the optimizer reorder — do not hand-schedule.

## 2. The register-allocation wall (§2, the dominant blocker)

mwccarm's allocator is **non-local**: which physical register a temp lands in depends on
the whole function's pressure, not the local statement. You usually **cannot force
`ip`/r12 coloring from source** — inline temporaries, `int` index vars, element-struct
rewrites have all failed to force it. If a candidate is byte-identical except a consistent
register renaming, you are at this wall: flag it and move on, or change the *shape* of the
computation (extra/fewer temporaries) to nudge pressure. There is no reliable knob. Tell:
`match.py` shows the diff confined to register fields with the structure otherwise aligned.

**Measured (`tools/coloring.py` over the matched corpus):**
- Only **~8.5%** of matched functions use `ip`/r12, almost always as a short-lived
  value-shuffle scratch (`ldr ip`/`str ip`/`mov ip`); essentially never live across a `bl`.
- Driver is **register pressure, not calls**: ip-using functions average ~0x66 bytes vs
  ~0x3c for the rest, with near-identical call counts.
- **The "right logic, wrong color" pile is small** — `triage.py` finds only a handful of
  regperm-only functions per module. Coloring is the *last* thing to go wrong, after
  types/offsets/structure are already right, not the main blocker.
- **Matching C for an ip function is just natural, full C** — no ip-forcing trick. Example
  that matches as-is: `return p[0] | (p[1]<<8) | (p[2]<<16) | (p[3]<<24);`. Coloring emerges
  from expressing the *complete* computation with correct types; a minimized stub
  under-expresses the pressure and colors low instead.

**Lever: the access EXPRESSION changes the allocation.** When a candidate is
byte-identical except a consistent register renaming, vary *how* you write the memory
access:
- `*(T*)G |= mask` (cast-and-deref) vs `G[0] |= mask` (index) produce **different**
  register orderings — the deref form reproduced the ROM's r0/r1 ordering on the
  `global_field_bitop` family where indexing always swapped them; it is now what the
  template emits.
- Other knobs: a named temporary vs an inline subexpression; the fuller surrounding
  computation vs a minimal stub; pointer arithmetic vs array indexing. None are guaranteed
  (allocation is non-local), but trying 2-3 access forms is cheap and often flips a regperm
  miss into a match.

## 3. Leaf patterns — no calls (§3)

- **Bitfield extract needs an *unsigned* pointer type.** `*(unsigned*)p >> sh & mask` emits
  `lsr`; signed emits `asr` and misses. Same for `unsigned char`/`unsigned short` loads
  where you want zero-extension.
- **Two adjacent words load-load-store-store is a struct copy**, not two scalar
  assignments: `*(P2*)dst = *(P2*)src;` (or `= G;` for a global). Two separate assignments
  produce different bytes/ordering.
- **`add` folds into the addressing mode.** For `*(T*)(p+off) = 0`, emit the store and let
  CW pick add-vs-direct by whether `off` fits the immediate — don't model an explicit `add`.
- **WALL: the *materialized*-`add` flag read-modify-write is NOT C-reproducible.** A
  cluster of single-flag setters appears in the ROM as a frameless 5-instruction form that
  computes the address once and shares it for load and store: `add rA,rB,#off ;
  ldr/ldrb rV,[rA] ; orr/bic rV,rV,#bit ; str/strb rV,[rA] ; bx lr` (e.g. the whole
  `BgCh::Start/StopDetecting*` family, the int-flag family at `obj+0x154`). The
  corresponding C (`*(T*)(base+off) |= bit`) ALWAYS folds into the tighter 4-instruction
  `ldrb [rB,#off]; orr; strb [rB,#off]; bx` — the ROM's form is *less* optimized than
  `-O4,p` ever produces. Confirmed unreachable: 6 source idioms x 11 mwccarm versions x 7
  opt levels, no `-opt nopeephole` knob changes it (instruction selection, not peephole).
  Not asm-hatch material either (clean compiled-code shape). Treat the whole cluster as
  shape-blocked.
- **Repeated global → one pool word.** CW loads a repeated global once; distinct extern
  names per use inflate the pool. Dedup globals by reloc-target identity.
- **Return-register discipline.** A real return loads `r0` then `bx lr`. A tail-call
  veneer is `... ; bx ip` (a thunk, not C-expressible — `is_thunk()` filters these).
- **`-char signed`.** Plain `char` is signed; use `unsigned char` explicitly for byte flags
  and indices — almost always what the game means.

## 4. Call / reloc patterns (§4)

- **Wrapper:** save arg, call, return arg → `X(a); return a;`.
- **ctor+vtable:** install vtable into `*this`, call `X(this)`, return this.
- **chain dtor:** a chain of single-pointer calls on `this(+off)`, returning this.
- **guard call:** `r = C1(this); if (!r) return r; return C2(this);` — the `beq` after the
  first call splits guarded-vs-after calls; parse the branch target offset (calls whose
  `bl` offset is below the `beq` target are inside the guard).
- **frame call with pooled args:** an `stmdb` frame calling several functions whose args
  are pool-loaded globals. Reloc'd pool slot → extern global; non-reloc slot → literal
  const. Dedup globals by reloc identity.

## 5. C++ patterns — compile as C++, not C (§5, the single biggest unlock)

A source whose first line is a literal `//cpp` compiles with `-lang c++` (`extern "C"`
keeps the symbol unmangled). Undefined virtual methods are fine — only `-c` compile, never
link; only vtable layout is read.

- **Virtual dispatch produces the pre-indexed writeback `ldr rV,[r0,#OFF]!`** that *no* C
  and no compiler flag emits. Real C++ virtual dispatch through a pointer emits it exactly
  (`Base* b = &this->base; b->m(ARG); return RET;`, with dummy virtuals before `m` to land
  the right slot). CW also has a split form (`add r0,r0,#OFF; ldr rV,[r0]`) it picks per
  offset; both come from the same C++. **Never emulate a vtable call in C**: the C form
  (`(*(fn**)(*(int*)obj + 0x48))(obj, 3)`) always HOISTS the vtable load off the object
  register; only a real `//cpp` call (`((Obj*)c)->m48(3)`) produces the `mov r0,r4; ldr
  r2,[r0]` this-temp shape (`func_ov006_0210d1fc`). If a diff shows the vtable load reading
  from the this-copy instead of the object's home register, switch to real C++.
- **Pointer-to-member-function (PMF) call** has a fixed ARM ABI prologue: `ldr
  r3,[r0,#OFF]; ldr r1,[r3,#4]; add r0,r0,r1,asr#1; ands r1,r1,#1; ldrne...; blx`. Source:
  `PMF* p = c->pp [+ N/8]; (c->**p)();` with `typedef void (C::*PMF)();`. Guarded and
  result-returning variants exist.
- **PMF-table dispatch:** `(c->*TABLE[c->idx].pmf[k])()` — the global `TABLE` is
  reloc-wildcarded so it stays a leaf; element size comes from the `lsl#SH`.
- **Deleting destructor:** vtable install (compiler **hoists** the pool-load above `mov
  r4,r0` — don't pin its position), then `D1(this)`, then a 2-arg `Deallocate(this, HEAP)`
  (HEAP loaded via `ldr;ldr`), return this.
- **`new T()`:** `p = NEW(sz); if (p) { CTOR(p+off)...; p[voff]=&VT... } return p;`. A size
  too big for `mov` comes from a pool `ldr` — read the literal from the bytes; bail if that
  slot is a reloc.

## 6. Idiom families worth knowing (§6, high-count in the game)

- **Matrix/transform setup:** `Matrix4x3_FromRotationY(m, ang); ...ApplyInPlaceToRotationX(
  m, ang); MulVec3Mat4x3(in, m, out);` then add a base position and `ApproachLinear` toward
  it. Extremely common in actor `_Main` state handlers.
- **Fixed-point scale:** `(int)(((long long)a * b + 0x800) >> 12)` is the Fix12 multiply;
  `cstd::div`/`mod`/`fdiv` route through the DS hardware divider at `0x4000280+`.
- **Actor state machines** (`St_*_Init`/`St_*_Main`): set a run of `this+off` byte flags,
  `SetAnim(...)`, set velocity fields, call `ChangeState`/sound, return 1. One cracked
  representative teaches the whole family.
- **Destructor chains:** a vtable install, several `SubObjectD1(this+off)` calls in reverse
  construction order, then `Deallocate`.
- **Angle → sin/cos table index:** `(unsigned short)(short)(angle + 0x8000) >> 4`,
  indexing an `s16[]` table as `tbl[i*2]`/`tbl[i*2+1]` for a paired lookup.
- **`__sinit_*` static initializers are matchable.** A straight sequence of registration
  calls; reproduce the constant-pool layout by emitting integer constants in source order —
  an ARM-encodable immediate stays inline, a non-encodable one takes a pool word, so order
  matters.
- **Parallel-array "twin" getters.** Many overlay predicate/getter functions are byte-twins
  differing only in a gating global and one callee — grep `src/` for the matched twin first
  and copy its struct model.

## 6b. Logic is necessary but NOT sufficient — match the codegen SHAPE (§6b)

LLM near-misses are dominated not by wrong logic but by correct logic in the wrong
instruction SHAPE:

- **Control flow.** A mid-function `cmp; ...eq; pop..eq; bx..eq` is an early return → `if
  (cond) return X;` with the same test direction. A conditional branch at the BOTTOM of the
  disasm is a do/while (test at bottom), not a `while`. A short forward branch past a guard
  chain can instead skip a single call/block and rejoin — always derive control flow from
  the branch TARGET ADDRESS, never from what the guard "looks like".
- **Call-arg spilling.** `str rN,[sp,#K]` then reload before each of several calls are
  spilled LOCALS (calls clobber r0-r3) — use plain locals per call, not a struct (a struct
  changes the spill shape) unless the disasm shows an actual struct copy.
- **Statement order/count.** One store per assignment, in the disasm's order — no
  redundant or reordered assignments.
- **Arithmetic idiom.** `mul` then `sub` is `x*K - C`, not the algebraically-equal `(x -
  C/K)*K`. Reproduce the exact op sequence.
- **Switch case-block layout follows source case-label ORDER, not value order.** When a
  dense jump-table's case blocks lay out non-numerically, reorder the `case` labels to
  match; for a dense 0..N table, low cases must be PRESENT (even empty, `case 0: case 1:`)
  to force the table instead of a compare chain.
- **Same-variable else-if chains can lose their skip branches.** mwccarm can prove the arms
  mutually exclusive and drop the "skip remaining" branches; rewriting as a `switch` with
  an explicit `break` per case restores the branch-per-case shape even though logically
  identical.
- **Loop entry-guard shape = `while (p && cond)`.** A top-tested, advance-before-body
  traversal comes from a compound `while (p && cond)`, not a `for`/`do-while` — those
  reallocate the iterator register and add/move the guard.

On a FALSE, diff your candidate's shape against the disasm and fix the FIRST divergence —
don't just reshuffle.

## 6c. Boolean materialization & predicated-select shapes (§6c)

How a boolean is *spelled* decides whether it folds into a compare or materializes a 0/1.
Getting this wrong is the most common near-miss on guard-heavy overlay code (~4
instructions off), with a reliable knob for each:

- **Materialized bool (verbose form).** ROM `cmp rX,#k; moveq r0,#1; movne r0,#0; cmp
  r0,#0; beq` means the source stored the comparison in a NAMED integer, then tested it:
  `int t = (field == k); if (t != false) { ... }`. A plain `if (field==k)`, `t != 0`, `t ==
  1`, `?1:0`, an inline helper, or a real `bool` all FOLD to a direct `cmp/bne` (~4 instrs
  shorter). The suffix matters: `t != false` emits `cmp r0,#0; beq`; `t == true` instead
  emits `cmp r0,#1`.
- **Predicated select — use the override form, not the ternary.** ROM `mov r,DEFAULT;
  movne r,OTHER` for `cond ? A : B` comes from an explicit default-then-override statement:
  `int x = A; if (!cond) x = B;` — NOT a ternary. A real ternary constant-folds when A/B are
  0/1, and a nested if-else range-merges consecutive `==` into `movls`; only the override
  statement reproduces the default + single predicated-move pair.
- **The access EXPRESSION shifts coloring** (extends §2's lever). Re-dereferencing the
  original argument each use (`**(unsigned short**)t`) instead of caching it in a local
  moved a long-lived pointer into `ip`/r12 and reproduced the ROM coloring — worth trying
  when a select/guard is byte-identical except a register rename.
- **Arg-builder temp ORDER sets the argument registers.** When a function builds a small
  struct of fields and passes its components to a call, the DECLARATION ORDER of the named
  temps decides which arg register each lands in — permute the temp declarations to match
  the disasm's arg-register assignment.

## 6d. Floor taxonomy additions (§6d, refine batch, 2026-07-01)

Three residual classes confirmed source-invariant across many C spellings; on these, stop
early and report the near-miss:

- **Pool-load of an immediate-encodable constant.** The ROM emits `ldr rX,[pc,#pool]` for a
  small constant passed as a call argument where mwccarm always folds it to `mov rX,#imm`.
  Literal/decimal/enum-cast/local-var forms all produce the `mov`. Instruction-selection
  choice, no C99 lever found. **(Refuted for one specific case, §6ai — see below.)**
- **Early-exit epilogue duplication.** `-O4,p` duplicates a short conditional epilogue
  (`popeq/bxeq`) at an early exit where the ROM has a plain `beq` to the shared tail —
  identical across guard clause, goto, and do/while-break phrasings. Related: an extra `add
  sp,sp,#imm` vs a fully if-converted merged epilogue.
- **Un-predicated branch around a pool-materialized byte-field RMW.** For `c[OFF]++` where
  `OFF` is not a data-processing immediate but fits LDRB/STRB's 12-bit offset, the ROM
  guards the increment with a REAL `beq` and pool-loads the offset (`beq skip; ldr
  r0,[pc]; add r1,r4,r0; ldrb/add/strb; skip:`). `-O4,p` instead emits either the tight
  predicated 4-instruction direct-offset form (pool is context/pressure-dependent, not
  encoding-forced) or, with a pointer var hoisted before the branch, a PREDICATED pool
  form. ~8 phrasings swept, each lands at most two of {pool, real branch, correct outer
  regs}, never all three.

The first two classes live in `nearmiss/db.jsonl` at div 1-2; candidates for an asm-block
close-out (see §8 in mwccarm-codegen.md) if their subsystems ever need completion.

**Caveat on "floor" claims in this section: at least one has since been refuted.** The
pool-load class above was found to be a misdiagnosis in one case (mwccarm-codegen.md §6ai,
2026-07-02): the pool slot was not a constant but a **symbol address** (a reloc) whose
unrelocated low byte happened to read as a small immediate. When a diff shows a pool load
where you wrote a small constant, check whether the slot is a reloc before calling it a
floor. Treat every entry in this section as "nobody has cracked it yet", not "impossible" —
consistent with the general caution in §6az below.

## 6az. Class-typed by-value parameters are homed to the stack (the Fix12 wall)
(mwccarm-codegen.md §6az — full derivation, corrections, and the renamed-symbol fix there;
this is the single most-cited passage in all of `notes/`, ~170 citers in `src/`)

Declaring a parameter as a by-value class — including the real `Fix12<int>` template
(`include/math/Fix12.h`) — makes mwccarm home the incoming register arguments to the stack
(`push {r0-r3}`) and reload each use, **+0x14 bytes** on `ShadowModel::InitModel`. Scalar
parameters of identical width stay in registers. Identical on 1.2/sp2p3 and 2004/b56;
`register`, `const`, an inline conversion operator, a `union` instead of `struct`, and
copying each parameter to a local before use all change nothing (re-measured on
`Actor::SetRanges`, target `0x24`, every aggregate form costs the same `+0x14`).

**The cost is per USE, not per parameter, and the "wall" was partly self-inflicted.** A
by-value class parameter a body never reads costs **nothing**:
`dEnemyBase_c::KillByInvincibleChar(const Vector3_16&, Player&, Fix12<int>)` reads only the
first two and reproduces exactly as a real C++ method, no homing. On `IsGoingOffCliff`,
which has two such parameters, reading one costs `+0x8` and reading both `+0x10` — **two
words per USED class parameter**. Homing is emitted in response to a *use*, not the
calling convention.

**There is no compiler lever here — the wall was a naming decision.** The image contains
**zero** `_Z...` strings and zero `5Fix12IiE` occurrences anywhere (censused over arm9 and
all 105 overlays) — a stripped ROM has no function symbol table, so every mangled name in
this tree is reconstructed, never ROM-derived. `dEnemyBase_c::IsGoingOffCliff` was the
standing example of an unbeatable wall (17 spellings swept, best `+0x8`); renamed to
`_ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actrisbbi` (`Fix12i`, a plain `s32` typedef
mangling as `i`, keeping the fixed-point intent) it matches exactly. **When the ROM reads
an argument straight from a register but the class spelling says otherwise, the class
spelling is wrong, not the compiler.**

**How to apply.** Before calling a `5Fix12IiE` symbol a wall, ask what anchors the class
spelling. If the answer is only "the convention," and the ROM reads the argument straight
from a register, the honest signature is scalar. Two practical notes from the migration:
parameters must be used DIRECTLY — aliasing into a local at entry (`int fix2 = down_;`)
keeps both live and costs extra callee-saved registers plus a spill reload; and `self`
becomes `this`, never a `dEnemyBase_c *self = this;` local, for the same reason. Where the
spelling IS anchored (a shared-library entry point established elsewhere), keep the
`extern "C"` definition with scalar args and declare the true class signature separately
for callers — a call emits the same substituted symbol either way.

---

## Destructor emission order (the D0/D1/D2 hard gate)

Not in the dated log at all — this rule lives only in `notes/agents/roles/writer.md`
("Rules that decide whether the bytes come back"), included here because it is exactly the
kind of mwccarm-emission rule this file exists to hold, and because it was previously
unreachable from any front door. The lever set is closed, measured, not guessed:

- **Inline in class ⇒ D1 then D0, always.**
- **Out-of-line ⇒ D2, D0, D1** — plus a homeless D2.

A `delete p` scaffold, a `p->~X()` scaffold, and moving the declaration below the overrides
all change **nothing**. If the cartridge puts D0 *below* D1, no admissible source form
reproduces the order, and the pair cannot live in that TU's licensed `.text` run — see
`notes/agents/roles/writer.md` for the promotion mechanics and
`notes/tu-reconstruction-pilot-report.md` §3 for the original discovery.

---

## See also

- `notes/mwccarm-codegen.md` — the full 317KB reference: this file's own uncondensed
  source (`## 1`-`## 6d`, `## 6az`) plus the dated discovery log (`## 6e` onward) that
  this file does not condense. Search it by section label for the fuller treatment of
  anything above.
- `notes/matching-style.md` — the sibling rules card for *style* (how to spell C so the
  shape matches), mined from the matched corpus rather than from compiler mechanics.
- `notes/agents/roles/writer.md` — the D0/D1/D2 rule above, plus the TU-promotion
  mechanics that depend on it.
