# Matching C style: writing C that mwccarm reproduces byte-for-byte

Mined from 36 byte-matched functions (the corpus) and validated by hand-cracking. The
agents get the LOGIC right; the misses are codegen SHAPE -- the C compiles to a different
instruction shape (register pressure / callee-saved set, control-flow layout, store order).
These are the transferable rules for steering the shape. Get the logic right first, then
apply these. The final section lists what is NOT source-controllable -- do not fight those;
hand them to the decomp permuter (tools/permuter/).


mwccarm 2004/b56 at -O4,p (canonical matching pin; 1.2/* still useful for sweeps). Rules ordered by impact. Get LOGIC right, then steer SHAPE with these.

### 1. Control flow â€” branch direction & layout (highest impact)

**Mirror the ROM's branch polarity; the `if`-true arm is the inline/fall-through block.**
WHEN: ROM tests `cmp r0,#0; bne <else>` with the true block falling through and the else placed out-of-line below a `b` to the tail.
WRITE: Write the condition exactly as the ROM tests it; put the block you want inline as the `if`-true arm and negate the test to match the ROM's `bne`/`beq`.
```c
// ROM: cmp; bne -> RegisterEggCoinCount inline, GivePlayerCoins out-of-line
if (!IsCollectingCap(p)) RegisterEggCoinCount(...); else GivePlayerCoins(...);
```

**Put the rare/work path's `return` last so its tail lands out-of-line at the bottom.**
WHEN: function returns 1 on the work-doing path and 0 on fall-through (or vice versa).
WRITE: keep the work-doing `return 1;` inside the `if`; make the bare `return 0;` the final statement. The compiler lays the trailing return out-of-line and branches to it with the last `beq`. Common/expected path = inline fall-through.

**`while(node)` find-first traversal: entry-guard + bottom test.**
WHEN: linked-list/find-first that may run zero times; in-loop early hit.
WRITE:
```c
node = first;
while (node) { if (id == *node) return node; node = node->next; }
return 0;
```
Emits: `cmp;beq <after>` entry guard, body, `cmp;bne <top>` back-edge, in-loop hit as predicated `cmp;bxeq lr`, `mov r0,#0;bx lr` after. Keep success-return inside the loop, failure-return after.

**Put shared post-if/else work once, after the block â€” not duplicated in both arms.**
WHEN: both arms followed by the same trailing call. WRITE one straight-line statement after the if/else; compiler emits the tail once (fall-through from else, `b` from if-arm).

### 2. Register pressure â€” the push set

**One callee-saved register pushed per value live across a `bl`. This is the master lever.**
WHEN: pointer/scalar arg used after a call. WRITE: use it naturally; it becomes `mov r4,r0` + `push {r4,lr}`. Keep exactly ONE value live across calls -> `{r4,lr}`; three live (self + fetched sub-ptr + saved scalar) -> `{r4,r5,r6,lr}`. To shrink the push set, restructure so each value dies before the first call. Don't add locals that straddle a call.

**No-arg / nothing-held-across-call body -> `{lr}` + `sub sp,#4`, not `{r4,lr}`.**
WHEN: `void f(void)` that's just a list of `void()->void` calls, OR a single indirect/virtual tail call holding nothing across it.
WRITE: `void f(void){ a(); b(); c(); }` -> `stmdb sp!,{lr}; sub sp,sp,#4`. Choosing void/no-arg vs one-arg flips the prologue.

**Sequential calls on the same arg -> save once in r4, `mov r0,r4` before each.**
WHEN: straight-line `f1(c); f2(c); ...`. WRITE N statements; emits `push {r4,lr}; mov r4,r0` then `mov r0,r4` per call, `mov r0,#1` after the last for a constant return.

**Don't hoist repeated `(char*)c+OFF` or a global base into a local â€” it's recomputed per use.**
WHEN: same sub-object/global used at multiple sites. WRITE the `c+OFF` expression (or global access) inline at each call site; the compiler re-emits `add rX,r4,#OFF` / `ldr rX,[pc,#..]` each time. Hoisting perturbs allocation and misses.

**Assign-call-result-then-test fuses into `movs`.**
WHEN: `p = call(); if(p)` where p lands in a callee-saved reg. WRITE them adjacent: `p=call(); if(p!=0){...}` -> `movs r4,r0; bne` (saves AND sets flags in one insn). Return it later with `mov r0,r4`. Don't separate the assign from the test.

### 3. Predication vs branches (body size is the switch)

**Tiny, call-free if/else bodies predicate; call-containing bodies branch.**
WHEN: then/else are each a few ALU/load/store ops, no calls. WRITE plain `if/else` with short straight-line arms -> one `cmp` + cond-suffixed body ops (`ldrshgt`,`strhle`,`movle`). If a body contains a call or is multi-instruction, you get a real `beq`/`bne` skip instead. Use body size to choose the shape.

**Value-less early return -> predicated `pop/bx` inline.**
WHEN: `if(cond) return;` guard. WRITE as a guard clause (not an else-block) -> `cmp; popXX {regs}; bxXX lr`.

**Early return WITH constant -> `movXX r0,#K` then predicated epilogue.**
WHEN: `if(cond) return K;`. WRITE the early guard -> `cmp; movne r0,#0; popne; bxne`. Keep distinct early-return statements (don't collapse with `&&`) to get per-guard predicated epilogues.

**Side-effect-free constant early return predicates the WHOLE epilogue; adding a call breaks it.**
WHEN: `if(field==4) return 1;`. WRITE bare constant return -> `addeq sp,#4; moveq r0,#1; ldmeq sp!,{lr}; bxeq lr`. Adding a side-effecting call to that body forces a branch instead.

**Multi-call body guarded by `&&` -> two sequential compare-and-branch-to-end.**
WHEN: `if(A && B){ ...calls... }` with a shared return at the bottom. WRITE the cheap integer test first, the call-returning test second -> `cmp;ble <end>; cmp;beq <end>;` body inline, single return at bottom. Don't wrap the whole rest of the function in `if(cond){...}` when you want the predicated-epilogue shape â€” use guard-and-return for that.

**Ternary with cheap arms -> dual predicated, no branch.**
WHEN: `x = cond ? A : B;` where A/B are an immediate or single PC-relative load. WRITE the ternary -> `cmp; ldrne r6,[pc,#..]; moveq r6,#imm`.

**Force `moveq/movne` + redundant `cmp` by materializing a bool local.**
WHEN: ROM shows `cmp rX,#1; moveq r0,#1; movne r0,#0; cmp r0,#0; beq`. WRITE the comparison into an int local, then branch on the local:
```c
int b = (int)(g == 1); if (b != 0) { ... }   // NOT if(g==1) â€” that folds to one cmp/bne
```

### 4. Constants & immediates

**Negative encodable constant -> `mov;rsb`; pass `-1` to get `mvn`.**
WHEN: storing/passing a negative const. WRITE the plain signed literal (`-0x2000`, `-0x3c000`, `-0xf000`) -> `mov rX,#pos; rsb rX,rX,#0`. Write `-1` (not `0xffffffff` spelled out) -> `mvn rX,#0`. Don't pre-negate or use bitwise tricks.

**Textual form picks pool-load vs negate.** A clean negative of an encodable immediate -> `mov;rsb`. A value that isn't a simple negation (e.g. `0xfffff998`) -> `ldr [pc,#..]`. Spell the constant the way the ROM materializes it.

**A `0` reused across adjacent stores/args stays in one register.** Write literal `0` plainly in multiple statements; compiler keeps one `mov r2,#0` and reuses it for `strb`, `str`, and a stacked zero arg. Don't fight it.

### 5. Memory access shape

**Statement order == str/call order. The compiler does NOT reorder independent stores.**
WRITE field stores and calls in the exact ROM order, even when offsets are out of numeric order (0x9c, 0xa0, 0x98 stays scrambled). Match widths: `*(unsigned char*)`->`strb`, `*(short*)`->`strh`, `*(int*)`->`str`.

**Volatile MMIO: exact order + width preserved.**
WHEN: stores/loads to fixed absolute addresses, spin loops. WRITE `*(volatile WIDTH*)ADDR` with width matching the asm; volatile forbids reorder/merge so source order == asm order. 64-bit write -> two `str`. Spin: `while(*(volatile unsigned short*)0x4000280 & 0x8000);` -> `ldrh; ands; bne`.

**Array index: literal -> folded offset; variable -> `lsl#2` scaled load.**
`data[0]`/`data[1]` -> `ldr [base]`/`ldr [base,#4]`. `data[i]` -> `ldr [base,i,lsl#2]`. Byte table `tbl[i]` on `unsigned char[]` -> `ldrb r1,[base,r0]` (no separate add). Keep the index inline in the addressing.

**Non-power-of-2 struct stride -> `mul`/`mla`, not shift.**
WHEN: `arr[i].field`, `sizeof(elem)` not a power of two (e.g. 0xc). WRITE a real struct array type sized to match; `arr[idx].a` -> `mov r0,#0xc; mul r0,idx,r0; ldr [base,r0]`. Boolean index: default `idx=0`, set `idx=1` under an `if`, then `&arr[idx]` -> `mov r3,#0; moveq r3,#1; mov r0,#0xC; mla r2,r3,r0,r2`.

**Whole-struct (POD, multiple of 4 words) copy -> `ldm!/stm!` block copy.**
WHEN: copying a fixed-size struct field or assigning a struct-returning call's result. WRITE a struct of N ints and use struct assignment â€” NOT memcpy, NOT field-by-field:
```c
struct M48 { int w[12]; };
*(M48*)(c+0x2e8) = *res;   // -> add ip,r4,#off; repeated ldm lr!,{r0-r3}/stm ip!,{r0-r3}
```

**Newly-allocated pointer is reloaded from the field, not cached.**
WHEN: store an alloc result into a field then write through it. WRITE:
```c
*(void**)(thiz+0x148) = _Znwj(0xc);
p = *(s32**)(thiz+0x148);   // reload â€” mwcc does str then ldr back
p[0] = ...;
```

**Local struct passed by reference lands on the stack via `add rN,sp,#off`.**
WHEN: ROM reserves stack and passes `add r0,sp,#off` as a struct ptr. WRITE a plain local `struct V3 v;`, fill it field-by-field, pass `&v`. Frame `sub sp,#sz` matches struct size; the slot address is recomputed per use, not held.

### 6. Calls

**5th+ args spill to `[sp]`,`[sp,#4]` in source order; one slot reserves a frame.**
WRITE args in natural order; arg5 -> `str [sp]`, arg6 -> `str [sp,#4]`, with `sub sp,sp,#N` in the prologue (even in an otherwise-leaf function â€” the stack arg, not register pressure, creates the frame). Stores interleave with register-arg setup. Trailing `0` arg reuses a held zero reg; `-1` -> `mvn`.

**Globals/dispatch via PC-relative literal per use.** `global_array[index]` -> `ldr [pc,#imm]` base, `ldrb` the byte index, `ldr [base,r1,lsl#2]`. Virtual call: model the sub-object at the right offset; `obj.method(0)` -> `add r0,base,#suboff; ldr r2,[r0]; ldr r2,[r2,#slot*4]; blx r2`. Declare exactly enough preceding virtuals to hit the slot.

### 7. Arithmetic idioms

**`abs(a-b)` -> `subs; rsbmi`.** WRITE exactly `int d = A - B; if (d < 0) d = -d;` (not a ternary/helper) -> `subs r1,A,B; rsbmi r1,r1,#0`.

**64-bit fixed-point `>>12` -> `smull;lsr;orr`.** WRITE `(int)(((s64)t * x) >> 12)` with `x` pre-shifted (`m[i]<<12`) -> `smull rlo,rhi,t,x; lsr rlo,#0xc; orr rlo,rlo,rhi,lsl#20`.

**`short<<12` summed into a base fuses into a shifted-add.** Read source as signed short (`short* m`) so you get `ldrsh`; express the sum in one expression (`ip + (m[1]<<12)`) -> single `add r,ip,r,lsl#12` instead of separate shift+add.

**Chain bitfield extract in one expression to keep it in r0.** WRITE `((Call()>>8)&0x3f)+0xaa` as one expression -> `lsr r0,#8; and r0,#0x3f; add r1,r0,#0xaa`. Intermediate locals perturb the r0/r1 allocation.

### Not source-controllable (don't waste cycles; hand to permuter)
- **Base-address materialization** (`add r2,r5,#OFF; ldr [r2]` vs `ldr [r5,#OFF]`): pointer locals, sub-object access, volatile all still fold to `[base,#off]`. **PARTLY SUPERSEDED (2026-07-29)** -- for read-modify-write sites this IS source-controllable via the u64 no-op mask launder; see "RMW base materialization" below. The entry still holds for plain single-use loads/stores.
- **Large-offset base split**: offset beyond ARM rotated-immediate range (e.g. 0x418, or `strh` past its smaller range) forces `ldr [pc];add` or `add base; strh [#small]`. Encoding-forced, not source-driven â€” leave it.
## Hand-crack validated rules (2026-06-20, high-effort hand-cracking + diff-oracle)

These were proven by hand-cracking real functions and watching the divergence count drop:

- **NEW-CTOR NULL CHECK: put the body INSIDE `if (p) { ... }`, return p after** -- NOT `if (!p) return p;`.
  `p = New(sz); if (p) { Ctor(p); ...installs...; } return p;` -> `movs r4,r0; beq <end>` (branch
  around the body), shared `mov r0,r4; pop; bx` exit. The early-return form predicates (`popeq;bxeq`)
  and misses. Validated: func_ov003_020adc10 18 -> 7 divergences.
- **MULTI-CONDITION EARLY-OUT TO A SHARED RETURN: use a `&&` chain, not separate `if(x) return K;`**.
  `if (a==0 && b==0 && c==0 && d==0) return 1; return 0;` -> each test `cmp; bne <shared ret0>`
  (branch), with a single shared exit. Separate `if (a) return 0;` statements PREDICATE each
  (`movne r0,#0; bxne lr`) and miss. Validated: func_ov007_020aebac 10 -> 2 divergences.

## The base-address materialization wall (precise characterization)

mwccarm sometimes emits `add rX, base, #OFF; ldr/strb [rX]` (materialize) where direct
`[base,#OFF]` would do. From the corpus + hand-cracking:
- It IS reproducible when a base is reused at CONSECUTIVE NON-ZERO offsets: `int* p = (int*)(c+OFF);
  p[1]; p[2];` or `r[0x17]; r[0x18]; r[0x19];` materializes `add rX,base,#OFF` and uses [rX,#k].
  (Examples: _ZN18SolidHeapAllocator8ResetEndEv, func_020080b0.)
- It is NOT reproducible when the FIRST/only access is at offset 0 from the materialized base
  (`q[0]`, `*f`, `o->flags[0]`): C always folds the zero-offset access to direct `[base,#OFF]`.
  Tried pointer locals, sub-objects, volatile, real struct array members -- all fold. The permuter
  cannot reach it either (it has no "materialize a base" mutation). This remains the most common
  residual blocker (1-2 instructions) on otherwise-matched functions.

  **EXCEPTION -- read-modify-write sites ARE reachable (2026-07-29, PR #815).** The
  "single-address read-modify-write" case previously listed here as unmatchable is source-
  controllable; see the next section. Four functions in one batch turned on it.

### Materialization IS reproducible -- the triggers (mined from oracle-verified matches)

The materialized base (`add rX, base, #OFF`) is NOT random. Reproduced examples from the matched
corpus show exactly what makes mwccarm emit it instead of folding:
- ARGUMENT MATERIALIZATION (func_02022b04, func_0200762c): passing `&obj->field` or `(T*)(base+OFF)`
  as a POINTER ARGUMENT to a call emits `add rX, base, #OFF`. Dereferencing the same expression
  (`*(T*)(base+OFF)`) stays folded `[base,#OFF]`. So `f(&this->sub)` materializes; `this->sub.x` folds.
- LIVE-ACROSS-CALL (_ZN18SolidHeapAllocatorC1EPvj): a base pointer that is used both before and after
  a `bl` is forced into a callee-saved register and kept materialized; all post-add accesses go via it.
- CONSECUTIVE NON-ZERO indices on an OPAQUE base (func_020080b0-style `r=call(); r[0x17];r[0x18]`):
  materializes `add rX,r,#0x5c` then `[rX],[rX,#4],[rX,#8]`.
- LOAD/STORE ORDER is controlled by OPERAND and STATEMENT order (func_0201adac "reversed load order":
  `lo | (hi<<8)` loads `lo` first). Reordering the field copies flips which offset loads first AND
  whether the base materializes before the first load vs folds it.

### RMW base materialization: the u64 no-op mask launder (2026-07-29, PR #815)

**The rule.** In the ROM, **every read-modify-write materializes its address via a pool constant,
while every single-use load/store folds onto the 12-bit offset.** That split is not noise -- it is
readable straight off the disassembly before you write any C, and it tells you exactly which sites
need laundering.

ROM shape for an RMW at a non-encodable offset:
```
ldr  r2, [pc]  ; = 0x4c21
add  r3, r0, r2
ldrb r2, [r3]
add  r2, r2, #1
strb r2, [r3]
```
Naive C (`c[0x4c21]++`) instead folds onto whatever base is already live: `ldrb/strb [r2,#0xc21]`.

**The lever** -- a no-op 64-bit mask, which blocks the base/value CSE and forces full-constant
address materialization:
```c
#define A(a) (*(unsigned char*)(((long long)(int)(a)) & 0xFFFFFFFFFFFFFFFFLL))
A(c + 0x4c21)++;                       /* materializes */
x = *(unsigned char*)(c + 0x4680);     /* plain fold -- leave it alone */
```
Also spelled `u16 *p = (u16*)(((int)c + 0x4c0c) & 0xFFFFFFFFFFFFFFFFULL); *p = *p - 1;`

**Two constraints, both load-bearing:**
1. **Launder ONLY the RMW sites.** Laundering the single-use accesses too will diverge -- the ROM
   folds those. EXCEPTION (2026-08-01, codegen notes 6ax): this rule governs the
   address-materialization split. For pure ORDERING residue between two chains the mask is a
   scheduling-class demotion that can go on EITHER side -- CapEnemy::GetCapState matched by
   laundering the plain single-use read and leaving the RMW alone. Launder the chain that must
   yield, not the one that must lead.
2. **Use textually distinct macros for distinct addresses.** One shared macro CSEs the laundered
   addresses together into callee-saved registers and inflates the frame (observed: `sub sp,#0xc`
   appearing from nowhere). This is the 6ak "textual variance is a handle on value-numbering" rule
   showing up concretely. The cheapest way to make them distinct is to **vary the inner cast type**
   -- semantically identical on a 32-bit target, different value-numbering classes:
   ```c
   #define L(a) (*(int*)(((long long)(int)(a))      & 0xFFFFFFFFFFFFFFFFLL))
   #define M(a) (*(int*)(((long long)(unsigned)(a)) & 0xFFFFFFFFFFFFFFFFLL))
   #define N(a) (*(int*)(((long long)(long)(a))     & 0xFFFFFFFFFFFFFFFFLL))
   ```
   (verbatim from `src/func_ov006_02114c04.c`, which needs all three)

**Landed examples** (all byte-identical, strict relocs, linkcheck VERIFIED):
`src/func_ov006_02114c04.c` (the clearest -- RMW/single-use split across a whole loop body),
`src/func_ov006_0211fe78.c` (63 divergences -> 0 in one edit),
`src/func_ov006_020d8d84.c`, `src/func_ov006_0211e8a8.c`.

**Cost of the stale note.** `func_ov006_0211e8a8` was worked to 12 divergences, correctly diagnosed
as the "first-access-fold wall" per the old text, and abandoned as unmatchable. Pointed at the
sibling files above it closed in **16 compiles**. A wrong floor entry is worse than no entry: it
stops a capable attempt that was one lever from landing. If you hit a documented wall, check
whether anything in the recent corpus has already broken it.

### The counter-lever: stopping over-materialization

The opposite failure (mwcc materializes/CSEs an address the ROM rematerializes per region) has its
own fix -- **do not pre-fold the base**. Modelling `self + 0x4694 + (i<<6)` as a flat constant makes
mwcc pool-load it and hoist the result into callee-saved registers (observed: 7-register push).
Modelling the same thing as a **stride-0x40 struct array** indexed `[i]` yields the ROM's
`add rX,self,i,lsl#6; add rX,rX,#0x4000; ldrb [rX,#0x694]` and rematerializes per region.
(`src/func_ov006_020d69b8.c`, `src/func_ov006_020d816c.c`.)

Related: write the cast **inline** at each use. Hoisting it into a local pointer (`Ent *ents = ...`)
forces one addressing form everywhere; inline lets mwcc pick per context, which is what the ROM does.
And mixed addressing for the *same field* is real, not sloppiness -- one access may need the flat
pool-base form while others need struct-member form; unifying them costs an instruction.

Also worth knowing: the odd-looking `+0x4600`/`+0x4000` base splits in these overlays are just ARM
immediate-offset encoding limits (`ldrh` imm <= 0xff, `ldr`/`ldrb` <= 0xfff) over a record array --
predictable, not something to reverse-engineer per function.

### The hard floor: first-access-fold (tested exhaustively, NOT source-controllable)

Even when materialization fires correctly, one residual instruction often stays locked: the FIRST
access through the materialized base folds to direct `[base,#OFF]` while later accesses use the
register. Example (constructor, div=1): ROM `str r0,[r4]` vs ours `str r0,[r5,#0x24]` -- r4=fl=c+0x24,
r5=c, same address, mwccarm picks the folded form for the first store only. Confirmed unreachable by:
- every field/statement order, struct-copy, nested-member, reference, address-of form;
- all 10 compiler versions (1.2 base..sp4, 2.0 base..sp1p6) and opt flags (noprop/nocse/nopeep/
  noschedule/levels) -- bytediff never improves;
- the permuter (div=1 seed, produced no improvement -- no source-AST mutation changes this backend
  instruction-selection choice).

Same floor blocks the forward-interleaved Vec3-copy shape (func_ov020/ov026): ROM materializes the
copy-source base and reads forward (0,4,8) interleaved with arg setup; no source order reproduces it
(forward folds the first read; reverse reads 8,4,0). NOTE: the corpus's own func_020080b0 has this
exact shape, was banked `opus-hand`, but does NOT reproduce under any config -- evidence this specific
schedule defeated prior hand-matching too. (Flag for a corpus re-verification sweep.)

Conclusion: the materialization triggers above DO crack the subset of misses that fit them (arg-pass,
live-across-call, consecutive-index, order-sensitive), **plus read-modify-write sites via the u64
mask launder (2026-07-29)**. The residual first-access-fold on plain single-use accesses and the
forward-interleaved-copy schedule still look like a backend floor -- but note that the RMW case sat
in this section as "tested exhaustively, NOT source-controllable" for months and was wrong. Treat
these entries as "nobody has cracked it yet", not "it is impossible".

## Reference-decomp ground truth + corrected understanding (2026-06-21)

Cloned and studied pret/pokediamond + pret/pokeplatinum (same mwccarm family). Key corrections
to the claims above, and the levers humans actually use. (Clones at ../\_ref_pokediamond,
../\_ref_pokeplatinum.)

**Humans DO match these -- the "humans leave it NonMatching" line above is mostly WRONG.**
pokeplatinum is ~100% matched with ZERO non-matching escape hatches. pokediamond has 6
hand-written-asm functions in the whole game. So the floor is real but TINY (our 35/121
materialization near-misses are the same residual; only a handful are truly unbeatable). The
bulk of every "near-miss" is closeable -- it just takes the right source structure.

**The levers humans use (validated in the reference source, quoted idioms):**
- LOCAL DECLARATION ORDER + C89 scoping is the #1 lever. `// must declare i first to match
  (regswap)`, `// must declare C89-style to match`, `// must maintain this declaration order`
  recur dozens of times. Reordering/ re-scoping locals shifts register allocation.
- VARIABLE TYPE/WIDTH: `u32 heapID = ...; // using HeapID enum causes regalloc fuckery`. Keep
  the plain int/u32 instead of an enum/narrower type to flip allocation.
- VARIABLE REUSE: reuse one local for two roles to force the same register (`// must reuse side
  var to match`).
- `volatile` on a plain local (not just MMIO) to pin load/store placement.
- DEAD-VALUE RETENTION: keep an unused computed value the original kept (`// unused, but must be
  kept to match`) -- it holds a register or forces an instruction.
- `void *` LAUNDERING of pointer arithmetic (`// MUST be cast to void * to match`).
- `== TRUE` to force a materialized boolean; an empty branch to force a compare.
These are UNIVERSAL mwcc levers (apply to our 1.2). Two reference levers are version-specific and
do NOT apply to us: `-ipa file` global-base materialization (text.c:251) and static-inline helper
reconstruction (driven by `-inline on,noauto`) -- both are 2.0-series; SM64DS is 1.2 (no -ipa).

**HONEST CAVEAT proven by hand this session: these levers CASCADE.** mwcc allocation is
NON-LOCAL, so applying a lever to an existing near-miss usually makes it WORSE, not better
(func_ov020 div 2->9 moving one assignment; func_ov026 div 2->8). The near-miss structure is
often already the closest EASY reach; the last 1-2 instructions need the EXACT original C
structure, which is a search, not a single edit. So:
- Use these levers to write FRESH functions right the first time (no cascade baggage).
- For an existing near-miss, the lever is a SEARCH (permuter / many manual tries), not a fix.

**Workflow the references use:** decomp.me single-function scratch + objdump diff + skilled
manual iteration. NO permuter, NO objdiff, NO m2c. Our permuter/oracle/diff-oracle stack is
AHEAD of theirs on tooling -- the gap is the manual structure-search patience, not the tools.

**The near-miss DB breakdown (tools/categorize_misses.py): of 121,** ~47 different-op/idiom,
35 base-materialization (the real floor), 18 regalloc, 7+7 missing/extra logic, 6 push-set.
So ~60-85 are addressable (not floor) -- but each needs the structure search above. Grinding
them is low-ROI vs the COVERAGE GAP: 6,850 unmatched funcs, 1,248 small (<0x40), many easy and
UNTRIED (proven: func_0206165c, a 24-byte global-pointer setter, matches at canonical with
`(G+i)[5]=v` but no template covers the shape). Harvest those first.

## Batch levers (2026-07-29, PR #815 -- 18 matched of 19)

Each was isolated against a control (the reported "no effect" results are as useful as the hits).

### Read the matched siblings FIRST (the single highest-yield habit)

Both functions in this batch that resisted brute force fell to reading already-matched code in the
same subsystem -- neither fell to search:

- `func_020316d8` was floored at **23 divergences after ~5600 compiles** of declaration-order,
  statement-order, type and pragma hill-climbing, and diagnosed as a pure allocator rotation. The
  supporting finding was even correct: decl order and `register`/type knobs have *zero* effect on
  that function. It was not an allocator floor. The matched sibling `src/func_0201b100.c` contains
  the same blit loop and carries the load-bearing spellings directly -- a `(short)(int)` double cast
  to defeat the range-folder, the slow path as an `else` block with block-scoped locals, a char-cast
  third store that breaks an mla-coalesce. ~650 compiles from that start.
- `func_ov006_0211e8a8` was abandoned at 12 divergences against a documented wall; three siblings in
  the same batch had already broken it. 16 compiles once pointed at them.

The pattern: hill-climbing explores the axes you thought of. A matched sibling shows you the axis you
did not. When a function stalls with "logic certainly correct, residual is pure regalloc", stop
sweeping and go read the nearest matched function in the same object family or subsystem.

**Two caveats on picking siblings, both learned the hard way.**

**1. Filter out `// NONMATCHING` files.** ~1% of `src/` (102 files as of 2026-07-29) are asm hatches
or non-reproducing drafts. Their codegen by definition does NOT reproduce the ROM, so spellings
copied from one are worse than useless. "Exists in `src/`" is not the same as "is a verified match"
-- grep the first few lines before you learn anything from a file.

**2. Search by RESIDUAL SIGNATURE, not by address.** Nearest-by-address is only the first guess, and
it has now failed three times in a row while signature search succeeded. Pick a distinctive,
greppable feature of the *residual* and scan every matched function for it:

| function | address-nearest siblings | what signature search found |
|---|---|---|
| `func_ov063_02119074` | nothing on frame padding | scan for prologue `push {r4,lr}; sub sp,#0x10` + near-zero sp traffic (dead frame space) -> the `volatile int dummy[N]` idiom |
| `func_ov006_02109aac` | no useful idiom | scan matched ov006 ROM bytes for `add rD, rN, rD, lsl #3` -> `src/func_ov006_02108f2c.c`, same table/compares/callee; 7 words closed in one edit |
| `func_ov006_021082fc` | adjacent twin 0x38 away, useless | `grep -l Matrix4x3_ApplyInPlaceToTranslation src/*.c` filtered to callers passing a stack Vec3 -> `src/func_ov006_02107ea8.c`; 11 -> 0 |

Practical recipes: grep `src/` for a **callee name** your function also calls (then filter by
argument shape); scan matched ROM bytes for a distinctive **instruction pattern** from your residual;
or scan for a **prologue/frame signature** matching yours.

Related, from the same batch: reading ONE well-chosen sibling beat a **10,000-iteration permuter
run** on `func_ov006_02107ea8` (the permuter floored at 10 words and never got below; the sibling
idiom took it to 0 in one edit). Do the reading before you reach for search.

`func_ov002_020d869c` is the same lesson from the other direction: it was matched by resurrecting a
banked 11-divergence near-miss whose central insight (the `~PVec(){}` trick below) had been right all
along and was written off because of an unrelated callee-arity error. **Read `nearmiss/db.jsonl` for
your target before starting.**

**Defeating dead-store elimination -- when the ROM has stores that "shouldn't exist".**

> **Four mechanisms, and they are NOT interchangeable -- pick by what else you need.**
> This symptom (the ROM stores to a stack slot that is never reloaded, while the values
> also go to the callee in registers) has come up four times with four different correct
> answers. Ranked by what to try first as of 2026-07-30:
>
> | # | spelling | keeps stores | leaves scheduler/coloring free | use when |
> |---|---|---|---|---|
> | 1 | plain struct + address-laundering casts `((int*)&t)[0] = x;` | yes | **yes** | default -- try first |
> | 2 | address **escapes** into a call (`f(&v)`) | yes | **yes** | the aggregate is already passed by address |
> | 3 | one enclosing struct whose address is taken | yes | partly | several unrelated locals need it at once |
> | 4 | `volatile` aggregate + named scalar locals | yes | **no** -- freezes store order | last resort |
>
> `volatile` is the one to reach for last: it preserves the stores but pins their order and
> introduces extra *named webs*, which cost an 11-word rotation on `func_ov006_021082fc`
> that no declaration-order permutation could fix. Swapping it for form #1 (taken verbatim
> from the twin `src/func_ov006_02107ea8.c`) closed that function 11 -> 0 in one edit.
> Form #2 beat `volatile` on `func_ov060_02117db8` for the same reason: dropping `volatile`
> alone deleted the stores as dead, but merging two stack Vec3s into one `int v[6]` whose
> address escapes into a call kept them *and* let the scheduler hoist the call-arg setup the
> way the ROM does.

- A local struct with a **user-declared destructor** makes it non-trivially-destructible, so mwccarm
  cannot dead-store-eliminate the stores, while field reads still forward into register args. This
  reproduces the ROM's "store x/y/z to `[sp],[sp+4],[sp+8]` **and** pass them in r1-r3":
  ```c
  struct PVec { s32 x, y, z; ~PVec() {} };
  ```
  Probed and rejected as the cause first: 4-byte structs, non-POD classes, `Vector3`-by-value,
  varargs, 7-arg models. It is not a struct-ABI effect. (`src/func_ov002_020d869c.cpp`)
- Where the ROM keeps several apparently-dead stores to locals, wrap **all the locals in one
  enclosing struct whose address is taken**. That blocks SROA/register promotion for the whole
  aggregate so the dead stores survive, while store-to-load forwarding still supplies the registers.
  Nothing else worked: `opt_dead_assignments`/`opt_lifetimes`/`opt_propagation` off, `(void)&d`, and
  an inlined helper taking `&d` all failed. (`src/func_ov006_02110a20.c`)

**`inline`, not `static`.** mwccarm 1.2 does **not** auto-inline a `static` 4-line helper at `-O4,p`
-- it emits a real `bl`. Marking it `inline` produced the ROM's separate expansions, each with its
own hoisted zero temp. Easy to misread as "the ROM had no helper". (`src/func_ov006_02110a20.c`)

**Stack layout is a derivation tool, not a guess.** Established with dedicated probe functions:
slots follow **strictly lexical declaration order**; function-scope locals always get the lowest
offsets; block depth and scope-close order are irrelevant; and **compiler temps are allocated after
all declared locals**. Therefore a ROM slot at the *top* of the frame that is used *first* cannot be
a declared local -- it must be a temp, i.e. a C99 compound literal (`obj->v = (Vec3){0,0,0};`).
Deriving the construct from a frame-layout contradiction beats permuting spellings.
(`src/func_ov007_020ca010.c`)

**All-zero aggregate init triggers materialized-base stack zeroing.** `Vec3 v = {0,0,0};` gives
`add rN,sp,#off; str r,[rN]; str r,[rN,#4]; str r,[rN,#8]`. Field assignments, a pointer local, an
inlined `VecSet(&v,0,0,0)` and a `void*` helper all fold to `[sp,#off]`. A **non-zero** aggregate
init instead emits a pool `ldm`/`stm` copy. (`src/func_ov007_020ca010.c`)

**Pointer difference carries no conversion node.** To get a base-0 array stride offset into a
register as an int, `(int)&((char(*)[0x24])0)[idx][0]` emits a surplus `add rX,rY,#0` that mwcc will
not fold away (~12 cast spellings all did). Spell it as a **pointer difference** instead:
```c
off = ((char (*)[0x24])0)[idx] - (char*)0;
```
Operand order then matters: the pointer base must be the left operand of the following add, or you
get the operands swapped. (`src/func_ov006_0211e8a8.c`)

**`if/else` vs ternary has NON-LOCAL regalloc effects.** The existing "preserve ternaries" guidance
is not universal. A case written as a ternary poisoned the whole function's r1/r2 assignment for two
unrelated locals -- *despite* the ROM emitting the classic `moveq/movne` ternary shape. Rewriting it
as `if/else` fixed a 20-instruction coloring residual far from the ternary itself. When a predicated
pair is the only construct adjacent to a coloring residual, **try both forms**.
(`src/func_ov007_020b5f64.c`)

**Parallel loops want their own locals.** Six structurally identical zero-init loops sharing one
`i`/`p` pair gave correct instruction shape with permuted registers; giving each loop its own counter
and pointer fixed the allocation exactly. A declaration-order swap did not.
(`src/func_ov006_020feba8.c`)

**Assignment order, not declaration order.** Where a 24-permutation *declaration*-order sweep is
inert, reordering the **assignment / first use** can still flip the entire callee-saved assignment
(observed: 33 divergences -> 0 from moving one assignment). Source position of the first *use* sets
web rank. Corollary: an inert decl-order sweep means the lever is **gated**, not dead -- try
`#pragma opt_lifetimes off` and re-sweep. (`src/func_ov006_020fe750.c`)

**Prefer a respelling to a pragma.** `#pragma opt_strength_reduction off` and the address-tree
respelling `((int*)c + k)[0x1510]` both killed a spurious induction variable; the respelling was
chosen because the pragma is a global crutch that perturbs the allocator elsewhere in the function.
(`src/func_ov006_020e0068.c`)

### Verification discipline

- **A byte match is not proof.** A candidate here was byte-identical but relocated four pool words to
  `data_020a0de8+addend` instead of four distinct symbols -- a WRONG-DEST false match. Cause: one
  `struct {u8 a,b,c,d;}`. Fix: four separate single-byte structs, which keeps the
  `ldrb [base,i,lsl#2]` addressing *and* lands each reloc on its own symbol. Always run
  `tools/linkcheck.py` as well as `match.py`, and never pass `--no-strict-relocs`.
- **Check "wall" and negative claims hardest.** In this batch a claimed context-file disasm bug did
  not exist (two adjacent instructions conflated), and a claimed hard wall had already been broken by
  three sibling functions in the same batch.
- **Read the matched siblings and `nearmiss/db.jsonl` before starting** -- see the section at the top
  of this batch; it is the highest-yield habit here and it beat brute force on both hard functions.

## Coloring residuals: what actually works (2026-07-29 retry sweep, 9 near-misses closed)

A second sweep re-attacked banked near-misses from `nearmiss/db.jsonl`. Results below are each
backed by a landed byte-identical match, and the negative results were established with controls.

### FALSIFIED: "`#pragma opt_lifetimes off` gates declaration order"

A plausible-sounding rule -- *decl order is inert without the pragma and load-bearing with it, so an
inert decl sweep means the lever is gated rather than dead* -- was tested on three functions and is
**false**:

- `func_ov006_020fbcb8`: pragma toggled on/off/absent gave byte-identical output. Positive control
  `#pragma optimization_level 0` changed the size massively, proving the harness works.
- `func_ov007_020beb80`: full **720-permutation** decl sweep run with AND without the pragma; only
  6 distinct outputs total, best was the natural order.
- `func_ov006_020f13cc`: full 4! decl sweep x {pragma on, off} = 48 cells; every cell byte-identical
  to its no-pragma twin. Web-span rank was also inert here.

`notes/mwccarm-codegen.md` ~line 448 already listed `opt_lifetimes` as inert. **Treat every pragma as
dead until proven live FOR THAT FUNCTION by a byte delta** -- mwccarm silently no-ops illegal pragmas
and `2004/b56` ignores even `warn_illpragma`, so a byte change is the only proof. The pragmas most
often live here are `opt_common_subs off` and `opt_loop_invariants off`. An inert decl sweep usually
means the controlling axis is something else entirely, not that a pragma is gating it.

### The lever that actually closed these: DON'T NAME THE BASE POINTER

This is `notes/mwccarm-codegen.md` §6ab ("late shared-base via non-encodable offsets"), and it was
the fix in three separate functions this sweep:

A named `T *base = ...;` local colors into the early named-local band (r3/r2/r1/r0 descending) or
strands in `ip`. The ROM frequently has **no such variable** -- it lets the addressing optimizer build
the shared base itself as a **late temp**, which is what reaches the leftover-`r0` / callee-saved-`r6`
colors. Delete the named pointer and write the full offset at each use:

```c
/* misses: named base colors early */          /* matches: late temp */
int *pos = (int*)(c + 0x47f8);                 *(int *)(c + i*4 + 0x4000 + 0x7f8)
... pos[i] ...                                 *(u16 *)(c + i*2 + 0x4f00 + 0x7c)
```

`func_ov006_020f13cc`: deleting two named bases went 28 -> 15 -> **0** words. Tell-tale sign that this
is your bug: other blocks in the *same function* that already match are written in the full-offset
form. `func_ov006_020d48dc` is the same lever (28 -> 5 -> 0), and it holds even when the offsets are
out of 12-bit range and mwcc still has to split them into `add rX,base,#0x4000` + `[rX,#0x77c]`.

Note this is the **inverse** of the RMW launder above: one *forces* materialization at RMW sites, the
other *forbids naming* so the base stays a late temp. Both are the same underlying question -- where
mwcc decides to CSE an address -- and you often need both in one function.

### Other coloring levers proven this sweep

- **Loop-depth-of-definition** (`notes/mwccarm-codegen.md` ~line 2154): with two scaled loop
  invariants, defining one in the OUTER loop and the other INSIDE the inner loop is what decides which
  web gets r6 and which gets r4. No declaration permutation reaches this axis.
  (`func_ov006_020fbcb8`, via its twin.)
- **Add a web to re-rank.** Function-scope locals fill r5-r7 in *reverse* declaration order while the
  parameter takes r4. So when two webs are swapped and nothing you do to *either* helps, adding a
  third function-scope local re-ranks the whole list and moves them both.
  (`func_ov007_020beb80`, 34 -> 15 words.)
- **One struct copy instead of N field stores.** Three separate `u16` stores let the scheduler hoist a
  later load above an `umull`; `*(Vec3s *)(e + 0x3c) = dd;` emits the ROM's ld,ld,st,st,ld,st group.
  (`func_ov007_020beb80`, 15 -> 0.) Statement reordering does NOT substitute for this.
- **Address-escape beats `volatile` for keeping "dead" stores.** `volatile` preserves the stores but
  freezes store order and blocks the scheduler; passing the aggregate's address to a call preserves
  them *and* leaves the scheduler free to hoist call-arg setup as the ROM does.
  (`func_ov060_02117db8`.)
- **Frame padding**: when the only residual is `sub sp` short by N bytes with no extra memory traffic
  in the ROM, `volatile int dummy[N/4]; (void)&dummy;` reserves stack and emits no code. Plain unused
  locals, `volatile` arrays *without* `(void)&`, dead arrays and `long long` locals are all eliminated.
  (`func_ov063_02119074`; the idiom already appears in 8 other `src/` files.)
- **Don't hand-expand what the compiler expands idiomatically.** A seed spelling a signed halving as
  `(s + ((unsigned)s >> 31)) >> 1` cost a match; plain `(a + b) / 2` emits the identical
  `add rX,rX,rX,lsr#31; asr rX,#1` but without the extra pseudo-register.
  (`func_ov063_021177b0` -- ten decl/order permutations stayed stuck at 6 words; all seven `/2`
  variants matched immediately.)

### Picking which banked near-miss to retry: rank by RESIDUAL TYPE, not word count

Sorting candidates by divergence count and taking the lowest is the wrong strategy -- it is actively
*anti*-correlated with tractability. A very low word count means everything except register coloring
already matches, which selects precisely for the residual class with no source-level handle.

Measured 2026-07-30: the two lowest-word-count candidates in the whole DB (`func_0202ffec` at 2 words,
`func_ov075_0211a948` at 5) were both pure register-encoding/coloring swaps and both resisted every
axis available (see the floors below). The same day, candidates that were *size-mismatched* closed
readily -- a wrong instruction count means something in the source is still wrong, therefore still
changeable.

Rank like this:

1. **Size mismatch, small delta (1-3 instructions).** Best. Missing/extra instructions have
   source-level causes: an un-laundered RMW site, a dead store that got scalar-replaced away, a base
   pointer named that should be anonymous. `func_ov006_021082fc` (3 short) and `func_ov006_020dd880`
   (1 long) both landed.
2. **Has a matched structural twin** (same callee + argument shape, same loop nest, same prologue) --
   worth more than any lever; see the signature-search table above.
3. **Word-only residual that is NOT a pure register permutation** -- a wrong shift, a fused-vs-split
   compare, an idiom hand-expanded that the compiler emits itself.
4. **Pure register rotation with no twin.** Deprioritise; these eat thousands of compiles and usually
   floor.

The ranking pass is cheap: re-compile each candidate, record `SIZE(delta)` vs `WORDS(n)`, sort on the
above. Do not rank on the stored `divergences` field -- see below.

### Two documented floors (2026-07-30) -- do not re-grind these

Both are **size-exact** with a pure register-naming residual and no reachable source handle. Recorded
with their negative evidence so nobody repeats the search. Per the caveat above, read these as
"nobody has cracked it yet", not "provably impossible".

- **`func_0202ffec`** (arm9, 0x1d8) -- **2 words**. Two `smull` instructions with Rm/Rs transposed
  (ROM `smull r7,r8,ip,r3` vs ours `r3,ip`); the values are already in the right registers. Ruled
  out: per-site operand swap at all 18 multiply sites (every swap made it *worse*, 3 vs 2); P/Q cast-
  macro swap at every site both operand orders (~36 variants, all neutral); rewriting the two
  offending terms m-first crossed with 8 declaration orders (jumps to 51 words, and **declaration
  order is completely inert here**); 5 reassociation variants. mwccarm canonicalises `smull` operand
  order, so the source multiply order is not the lever.
- **`func_ov075_0211a948`** (ov075, 0xb8) -- **5 words**. A clean two-web r2/r3 swap (ROM puts the
  `[r0,#0x88]` load in r2 and the `lsr #0x1a` value in r3; ours is reversed), cascading into the
  `and`/`lsr` order and the final `orr`. Ruled out: **all 120 permutations** of the 5-term or-chain
  (baseline was best), plus 6 definition-order/inlining variants (swap the two definitions, define
  the load last, inline either value, two declaration-list orders) -- every one returned exactly 5.

### Re-measure banked near-misses before trusting them

The `divergences` field in `nearmiss/db.jsonl` is **not comparable across rows**. `match.py` bails
before comparing words on a size mismatch, so a size-mismatched row reports a small or zero count that
looks better than an honest size-exact row. Re-compile every candidate before ranking: of 8 top-ranked
entries in this sweep, 3 turned out to be size mismatches masquerading as low divergence counts.
**Size-exact-with-N-words is strictly better than any size mismatch** -- do not trade size exactness
for a lower word count, and do not report "0 mismatching words" from a size-mismatched compile.
