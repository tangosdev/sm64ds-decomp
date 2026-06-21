# Matching C style: writing C that mwccarm reproduces byte-for-byte

Mined from 36 byte-matched functions (the corpus) and validated by hand-cracking. The
agents get the LOGIC right; the misses are codegen SHAPE -- the C compiles to a different
instruction shape (register pressure / callee-saved set, control-flow layout, store order).
These are the transferable rules for steering the shape. Get the logic right first, then
apply these. The final section lists what is NOT source-controllable -- do not fight those;
hand them to the decomp permuter (tools/permuter/).


mwccarm 1.2/sp2p3 at -O4,p. Rules ordered by impact. Get LOGIC right, then steer SHAPE with these.

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
- **Base-address materialization** (`add r2,r5,#OFF; ldr [r2]` vs `ldr [r5,#OFF]`): pointer locals, sub-object access, volatile all still fold to `[base,#off]`. Permuter territory.
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
  (`q[0]`, `*f`, `o->flags[0]`, a single-address read-modify-write): C always folds the zero-offset
  access to direct `[base,#OFF]`. Tried pointer locals, sub-objects, volatile, real struct array
  members -- all fold. The permuter cannot reach it either (it has no "materialize a base" mutation).
  This is the single most common residual blocker (1-2 instructions) on otherwise-matched functions,
  and it is currently UNMATCHABLE from C when the access is zero-offset. Leave it for a future model
  or a permuter pass that learns this transform.

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
live-across-call, consecutive-index, order-sensitive). The residual first-access-fold / forward-
interleaved-copy schedule is a true backend floor -- the same cases human decompilers leave
NonMatching. The edge is automating everything source-controllable, not grinding this residue.
