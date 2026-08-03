# ITCM: 41 functions nothing was ever pointed at

As of 2026-08-01, `arm9/itcm` was **0 of 41 matched**, 22,904 bytes. Not floored, not
attempted. This file records why it was invisible, what is in there, and what the first
matches established, so the rest can be routed instead of rediscovered.

**Eleven landed** the day this file was written, all first- or second-try, all from reading
structure rather than grinding levers. That hit rate is the point: ITCM is not the residue
arm9 became, it is untouched ground. One larger function (`DetectClsn(RaycastLine&)`) was
attacked and did not match; its structure is recovered and written up below.

## Why it was invisible

**FIXED 2026-08-01.** `tools/modules.py::modules()` enumerated **`main` plus the overlays
and nothing else.** ITCM and DTCM are autoloads in `config/arm9/config.yaml`, and they were
absent from that list. Every tool built on `modules()` inherited the blind spot:

- `linkcheck.py` / `pr_linkcheck.py` — `_ranges()` never contained itcm, so a slot at
  `0x01ff….` resolved to no module and the file reported **`NONE`** (zero slots checked),
  which reads like "clean" and is actually "not looked at". Confirmed on the first five
  files here: the two arm9 siblings in the same run reported `ok`.
- anything downstream of those, including the relocation metric in the merge gate
  (`notes/pr-validation.md`).

`tools/worklist.py` also returns nothing for `--module arm9`, so a scheduler asking for
work never sees these either.

The fix was small and additive — ITCM's range `0x01ff8000..0x01ffdf3c` does not overlap
arm9 (`0x02004000+`) or any overlay, and DTCM's `0x023c0000..0x023c0020` overlaps nothing
either, so `read_at`'s preference order is unaffected. The one wrinkle was provenance:
`modules()` reads binaries out of `extracted/`, and there is no `extracted/itcm.bin`; the
image is `build/build/itcm.bin`, named by `config/arm9/config.yaml`
(`hash: ec91a2b334e8151e`). The registry now follows config.yaml for the autoloads and
skips them when `build/` is absent, so a checkout with no build still gets a usable
registry.

Verified by negative control, not by assertion: with a callee deliberately swapped in
`GetSurfaceInfo`, `pr_linkcheck` now reports
`WRONG  0x01ffd920 itcm  func_02036acc -> 0x02036ac4` where it previously reported `NONE`.
All five ITCM sources present at the time went `NONE` -> `ok` (eleven now do); ten arm9/overlay files re-checked `ok`, and
`pytest tools/` passes (75 passed, 2 skipped), so nothing outside the autoloads moved.

## Verifying an ITCM function today

```
python tools/match.py --c src/<sym>.cpp --func <sym> --addr 0x01ff…  --size 0x…  \
    --bin build/build/itcm.bin --base 0x01ff8000 --module itcm --strict-relocs
```

**`--module itcm`, not `--module arm9/itcm`.** The path-style spelling makes
`--strict-relocs` silently pass: `check_destinations` finds no rows, `bad` is empty, and a
deliberately wrong callee still reports MATCH. Verified with a negative control — swapping
`func_020381cc` for `func_02036acc` in `GetSurfaceInfo` reported MATCH under `arm9/itcm`
and correctly reported no match under `itcm`. Any ITCM result recorded with the path-style
name is unverified, not verified.

`config/arm9/itcm/relocs.txt` is real and populated (152 entries), so the check works once
the module string is right.

## What is in there

| group | count | bytes | notes |
|---|---|---|---|
| `MeshCollider` collision core | 8 | 10,412 | the octree; three `DetectClsn` overloads plus five named accessors |
| `MeshCollider`, unnamed | 6 | 88 | `func_01ffb07c`..`func_01ffb0c8` — all matched, see below |
| unnamed `func_01ff…`, unexamined | 19 | 10,764 | `func_01ff8000`..`func_01ffde98` |
| runtime / SDK | 8 | 1,640 | `__aeabi_idiv`, `__aeabi_uidiv`, `__aeabi_uldiv`, `__aeabi_ulmod`, `OSReadROMArea`, `DMAStartTransfer`, `DMAStartTransferFB`, `IRQ::UserInterruptHandler` |

**Do not assume the SDK-looking ones are policy-exempt.** Applying the objective test in
`notes/asm-policy.md` — does the body contain instructions C cannot express (`mcr`/`mrc`,
`swi`, `msr`/`mrs`, `ldm`/`stm ^`, `swp`) — to all 41 gives exactly **one** qualifier:
`func_01ffd9d4` (340 B, `mrs`/`msr`). **CORRECTED 2026-08-03: the instruction test is
necessary but not sufficient, and reading it as the whole story was my error.**
`IRQ::UserInterruptHandler` (0x01ffd97c) contains no privileged instruction yet cannot be
compiler output: it ends `ldr lr,[pc,#4]` / `bx r0`, writing LR from a link-time literal, and
never unwinds its own `stmdb sp!,{lr}` — `func_01ffd9d4` pops that frame (`popeq {pc}` at
+0x54 and `pop {pc}` at +0x94, both *before* its own first push at +0xb0). `func_01ffd9d4`
has exactly one reference in all of `config/`: the pool word inside the handler. It is a
continuation label, not a callable function, and the two share one stack frame across a
symbol boundary. No C construct expresses that. `notes/asm-policy.md`'s objective test has no
structural clause, so this class slips through it. The other 40, `__aeabi_idiv` and friends included,
are ordinary ARM only, so by this project's own rule they are unsolved matching problems
rather than assembly. An earlier draft of this file claimed the `__aeabi_*` routines were
exempt; that was wrong and is corrected here.

The prize is `MeshCollider::DetectClsn(SphereClsn&)` at 7,112 bytes — the single largest
unmatched function in the game.

## The eleven that landed

| function | addr | size |
|---|---|---|
| `func_01ffb07c` — copies a Vector3 into the 0x38 member | 0x01ffb07c | 0x1c |
| `func_01ffb098` — clears the 0x35 flag | 0x01ffb098 | 0xc |
| `func_01ffb0a4` — sets the 0x35 flag | 0x01ffb0a4 | 0xc |
| `func_01ffb0b0` — clears the 0x34 flag | 0x01ffb0b0 | 0xc |
| `func_01ffb0bc` — sets the 0x34 flag | 0x01ffb0bc | 0xc |
| `func_01ffb0c8` — `GetFile()`, returns the 0x20 member | 0x01ffb0c8 | 0x8 |
| `MeshCollider::GetUnkOctreeY() const` | 0x01ffb0d0 | 0x1c |
| `MeshCollider::GetOctreeOriginY() const` | 0x01ffb0ec | 0x10 |
| `MeshCollider::GetTriangleOrigin(s16, Vector3&)` | 0x01ffd890 | 0x48 |
| `MeshCollider::GetNormal(s16, Vector3&)` | 0x01ffd8d8 | 0x48 |
| `MeshCollider::GetSurfaceInfo(s16, SurfaceInfo&)` | 0x01ffd920 | 0x5c |

All `1.2/sp2p3`, all byte-identical with reloc destinations checked.

The six `func_01ff…` entries are MeshCollider methods whose symbols were never
recovered, so they keep their `func_` names — `symbols.txt` is the authority and the
filename is the symbol. They sit immediately before the named accessors, which is how
they were found: disassembling the gap between the last unnamed function and
`GetUnkOctreeY` turned up six one-liners. `func_01ffb07c` is what pins 0x38..0x40 as a
single `Vector3` (three word loads copied straight in), corroborated by `SetFile` seeding
it to (0x1000, 0, 0), the unit X vector. #989 declares those three as scalars
(`unk_38`/`unk_3c`/`unk_40`) and they are left that way here -- the bytes are identical
either way, and changing a struct other files build against is not this change's job.

### The prologue wall, which blocks four of their neighbours

`func_01ffafd4` (0x34), `func_01ffb008` (0x28), `func_01ffb030` (0x4c) and `func_01ffa3e0`
(0x5c) contain no privileged instruction, so the objective test does not exempt them.
**CORRECTED 2026-08-03: that reading was too literal — see "wall #2 is a tell, not a wall" below.** They are blocked on the
same thing, and it is worth naming because it is cheap to mistake for "this was assembly".

`func_01ffafd4` is the clean specimen: thirteen instructions, and the obvious C reproduces
**all thirteen**, in order, with the right registers. The only difference is that mwccarm
brackets the body with `sub sp,sp,#4` / `add sp,sp,#4` and the ROM does not — 0x3c against
0x34. That is 8-byte stack alignment for a prologue that pushes an odd number of registers
and then calls. Inert against it: four source shapes (`register`, struct-typed target,
reusing one pointer local, no named local), all twelve compiler versions, and `-O4,s`,
`-O3,p`, `-O2`, `-opt speed`, `-opt noframepointer`.

How common is the unpadded form? Scanning arm9 for a `stmdb sp!,{lr}` prologue:
**462 functions pad, 2 do not.** So the padding is what this compiler does, and the ROM's
ITCM code does not do it.

The other three add a second shape on top: they wrap their call in a push of the whole
caller-saved set — `{r0,r1,r2,ip,lr}`, `{r0,r1,r3,ip,lr}` and `{r0,r2,r3,ip,lr}`; the exact
set varies with which register holds the live value — so the arguments survive at two
instructions' cost where mwccarm would use callee-saved registers.

### Wall #2 is a TELL, not a wall (2026-08-03)

Measured across the whole ROM, `stmdb sp!` with r12 (`ip`) in the register list splits into
two shapes, and **both are ITCM-exclusive**:

| shape | sites | elsewhere in arm9 | across 103 overlays |
|---|---|---|---|
| `ip` + at least one of r0-r3 (the caller-saved tell, wrapped around a call) | 25 | 0 | 0 |
| `ip` inside an otherwise ordinary callee-saved prologue | 14 | 0 | 0 |

Careful with the measurement: a naive scan of `extracted/arm9_dec.bin` finds all 39 too,
because the ITCM image is embedded in it at file offset 0x97000. Excluding that window leaves
zero of either shape in 640 KB of arm9 and zero across every overlay.

An idiom that appears nowhere in compiler-generated code and only inside one region is
evidence about origin, not a codegen wall to grind at. The MeshCollider block at 0x01ffb07c+,
where all 11 C matches landed, has none of either shape.

Consequence for routing: `__aeabi_uldiv` and `__aeabi_ulmod` open `push {r4,r5,r6,r7,fp,ip,lr}`
— the second shape — so they are likely hand-written too.

**RETRACTED 2026-08-03.** I originally wrote that `__aeabi_idiv` (0x20c) and `__aeabi_uidiv`
(0x1e4) "carry neither shape and are the only clean targets left in that neighbourhood". That
inference is invalid, and two independent attempts falsified it. They carry neither shape
because they contain **no `stmdb`/`push` at all** — they are frameless, relocation-free leaf
routines. Absence of a stack-shape tell in a function with no stack is not evidence of compiler
origin. Do not treat "lacks the tell" as "is C"; the tell only discriminates among functions
that have a frame.

Both are in fact **CodeWarrior's own runtime library**, not Nintendo game source, which was
measured rather than assumed: compiling `a/b`, `a%b` and `ua/ub` at 1.2/sp2p3 with the repo
flags leaves undefined references to `_s32_div_f` and `_u32_div_f`, and `%` lowers to
`bl _s32_div_f; mov r0,r1` — confirming the dual `r0`=quotient / `r1`=remainder return. So
0x01ffabe4 is `_s32_div_f` and 0x01ffadf0 is `_u32_div_f`. There is no original C to recover.

Four structural facts, each verified on the image:

* **Unguarded computed dispatch.** `add r2,r2,r2,lsl #1` then `add pc,pc,r2,lsl #2` — a 12-byte
  stride landing directly on the *n*th of 32 unrolled 3-instruction bodies, with no bounds
  check. mwccarm's only computed dispatch is a C `switch`, which emits a bounds-checked
  `cmp`/`addls pc,pc,rX,lsl #2` **plus a table of `b` words** at 4-byte stride; computed `goto`
  is a syntax error even with `-gccext,on`.
* **A statically dead instruction**: `mov r0,r0` at 0x01ffac54, unreachable (the computed
  jump's minimum landing site is pc+8 = 0x01ffac58) — pipeline padding.
* **The carry flag is a bidirectional data path.** Inside each 3-instruction step the bit
  shifted out of the numerator becomes the carry-in of the remainder update, and that update's
  borrow becomes the quotient bit. C has no carry object; measured floor for a C step is 5
  instructions against the ROM's 3.
* **456 bytes are byte-identical between the two routines** (0x01ffac14..0x01ffaddc vs
  0x01ffae08..0x01ffafd0) — one macro expanded twice with different pre/postambles.

And `__aeabi_uidiv` has a **second entry point**: `config/arm9/itcm/relocs.txt` records
`from:0x01ffaa0c kind:arm_call to:0x01ffadf8`, entering +8 to skip the divisor guard. The caller
is the shared `__aeabi_uldiv`/`__aeabi_ulmod` body, *not* `func_01ffaa34` (whose only interior
call is `bl 0x01ffabe4`). Census: 141 calls to 0x01ffabe4, 16 to 0x01ffadf0, 1 to 0x01ffadf8. A
C function cannot have two entry points, so even a byte-exact C body would be a false recovery.

Net: there are **no** clean C targets left in the maths block. It is vendor runtime end to end.
Their body is a masked read-modify-write of the word at `func_0207322c()` — a function that
just returns the constant 0x020aa3f4 (the FP status word) — returning the old value. `func_01ffb030` additionally
packs two 5-bit fields from bits [4:0] and [20:16] into [4:0] and [12:8] on the way in,
unpacks the reverse on the way out, and sets bit 30.

**A finding about the existing tree, not about ITCM.** The two arm9 functions that skip the
padding are `func_02059468` and `func_02059824`, and both are currently committed as
whole-function `asm` blocks carrying the `HAND-ASM PRIMITIVE` banner. Their bodies are
`stmdb`/`bl`/`ldmia`/`bx` and nothing else, which does not meet the objective test in
`notes/asm-policy.md` — so under the project's own rule they are misclassified and should
be `// NONMATCHING` drafts. They appear to have survived the 2026-07-24 reclassification
that caught 116 others. Flagged rather than changed: reclassifying them lowers the matched
count, which is a maintainer's call.

## Batch 2 (2026-08-03): 13 attempted, 6 matched, and the symbol table is wrong in four places

Six landed and are in `src/`, all re-verified from the shipping path with `--strict-relocs`:
`DMAStartTransfer` (0x48), `DMAStartTransferFB` (0x50, **2004/b56 only**), `func_01ffdd98`
(0x68), `func_01ffdd08` (0x90), `func_01ffde98` (0xa4), `OSReadROMArea` (0x130). Together they
are the card/DMA/reboot cluster at the top of ITCM. `DMAStartTransferFB` needs no
`rombuild-versions.txt` entry: `tools/rombuild.py` already defaults to `2004/b56`.

`FB` means **barrier**, decided from callers, not guessed: it is instruction-identical to
`DMAStartTransfer` plus two dummy reads of DMA0SAD (the DS DMA start delay), and its only three
callers (0x0205a144/98/fc) are the *Sync* helpers that busy-poll the enable bit on the next
instruction, while the plain twin serves the async/callback paths.

### Three more that are byte-exact but are NOT C -- maintainer call

Each reproduces the ROM exactly as an `asm` block, and each fails asm-policy's *instruction*
test while failing to be compiler output for structural reasons. Filed as NONMATCHING, not
landed as matched:

* `_ZN3IRQ20UserInterruptHandlerEv` (0x58) -- see the corrected policy section above.
* `func_01ffaa34` (0x1b0), the signed 64-bit divide (`__aeabi_ldiv` by its neighbours' naming):
  four entry points into one body, statically unreachable code at +0x170, a provably dead
  conditional tested twice, and it saves `ip`. Decisive: `long long a / long long b` at
  1.2/sp2p3 compiles to a 12-byte veneer *to this routine*.
* `func_01ffdb28` (0xb0), the last stage of the card reboot -- CRT0 glue written by the same
  hand as `Entry` (identical `mov ip,#0x4000000; str ip,[ip,#0x208]` opening, identical
  DTCM+0x3ffc addressing, identical hand-set-lr `bx` handoff). Measured tell: of the 8,063
  functions in the ROM containing a reloc-confirmed `bl`, exactly **5** never preserve lr --
  this one plus `Entry`, `func_020732e8`, `func_0207335c`, `func_02073584`, and all four of
  those are already committed as `asm void` or `NONMATCHING (NOT-C-EXPRESSIBLE)`.

If "makes calls and never preserves lr" joins the objective test, the last one flips cleanly.

### Four symbol-table defects, each verified independently

These make their functions unmatchable *by construction*, which is why nothing here ever moved:

| symbol | declared | evidence | should be |
|---|---|---|---|
| `func_01ff8708` | `size=0x2dc` | 18 non-`bl` branches leave the declared body (up to +0x3ec); the 0x42c "gap" after it holds 15 `add sp,#0x10` + 16 `pop {r4-r7,lr}` -- its own teardown | ~`0x6f0` |
| `func_01ff97d8` | `size=0x9e4` | 56 non-`bl` branches leave the declared body | extends into the 0x188 gap |
| `func_01ffa344` + `func_01ffa3e0` | two symbols | `a3e0` has **zero** incoming branches or calls anywhere; its only entry is fallthrough from `a344` | one symbol, `size=0xfc` |
| `func_01ffa440` | `size=0x148` | `0x01ffa4bc` has **4 external callers** (ov002 x2, ov074, arm9, all `module:none`) and no symbol | `0x78` + a new symbol at 0x01ffa4bc |

So ITCM has **42** functions, not 41. Fixing these is a prerequisite for anyone working the
soft-float block, not an optional tidy-up.

### Routing for whoever goes next

`func_01ff859c` is `double -> unsigned long long`; `func_01ff8708` is the soft-double multiply;
`func_01ff8000` is the add; `func_01ff8e10` the subtract; `func_01ffa344` is `__aeabi_f2iz`;
`func_01ffa440` is the int-to-float pair. All of them carry the caller-saved tell and several
have no prologue at all, so the whole 0x01ff8000..0x01ffa9dc block -- about 9.2 KB -- is one
hand-written soft-float library. Do not route cold C matching at it.

## The structure they established

None of this was guessed; each line names what pins it.

**`KCL_File`** (extended in `include/MeshCollider.h`, where #989 already defines it). The four pointers at 0x00..0x0c are pinned by the
already-matched `MeshCollider::UpdateFileOffsets`, which relocates exactly those four words
and nothing else. Strides are pinned by the accessors: `mul #0xc` into [0x00] (12-byte
`Vector3`), `mul #6` into [0x04] (6-byte `Vector3_16`), `lsl #4` into [0x08] (16-byte
`KCL_Prism`). `origin.y` at 0x18 and `maskY` at 0x24 are pinned by the two octree accessors;
their X/Z siblings were inferred until `DetectClsn` read all three.

Scales: vertices and the octree origin are stored at **1/64 of a Fix12i unit** (`lsl #6` on
read); face normals at **1.0 == 0x400** (`lsl #2`). The raw fields are therefore not typed
`Fix12i`.

**`KCL_Tri`** is the 16-byte record. #989 already had `posIdx` at 0x04 and `normalIdx` at
0x06; the octree walk fills in the rest of what was `pad_08`: three edge-normal indices at
0x08/0x0a/0x0c and `attribute` at 0x0e. `attribute` is handed to the CLPS lookup as a **raw index with no masking**,
so in this game the KCL attribute word *is* the CLPS index.

**The vtable.** `_ZTV12MeshCollider` at 0x020993dc, 13 slots, every one resolving to a named
function — the full map is in the `include/MeshCollider.h` header comment. The important
structural read: slots 3/4/5 are **NULL in `_ZTV16MeshColliderBase`** (0x02099388), so the
base declares `GetSurfaceInfo` / `GetNormal` / `GetTriangleOrigin` pure virtual, and slots
9-12 of `MeshCollider` still point at `MeshColliderBase`'s implementations. That is the
derivation, read straight out of the ROM, and it is handed to whoever converts the collision
chain (claimed in CLAIMS.md) rather than done here.

Type names are the ROM's own: `KCL_File` and `CLPS_Block` appear verbatim inside
`_ZN12MeshCollider7SetFileEP8KCL_FileR10CLPS_Block`, and `Vector3_16` inside
`_ZN5Actor13SpawnFireballERK7Vector3PK10Vector3_16…`. Nothing here came from a vendor SDK
header.

**Neighbours worth knowing.** `func_020381cc` is the CLPS lookup (`entry = block->data + 8 +
idx*8`, with a shared default when the block is absent or the wrong version).
`func_02037eb0` writes a `SurfaceInfo` (`{ CLPS entry at 0x0, Vector3 normal at 0x8 }`,
layout pinned by `SurfaceInfo::CopyNormalTo` at 0x02037dcc). Both are still `func_`-named.

## The one lever that was needed

`GetSurfaceInfo` sat at exactly one divergent word through five source variants and a full
version sweep:

```
ROM:  ldr r3,[r0]     <- vtable read from the incoming argument register
ours: ldr r3,[r6]     <- read from the callee-saved copy
```

It calls `GetNormal` through the vtable. A hand-rolled `(*(fn**)this)[4](...)` reads `this`
from the saved copy; **mwccarm's own virtual-dispatch sequence reads it from r0.** Writing
it as a genuine C++ virtual call — which needed the class and the correct slot index, hence
the header — matched byte-for-byte immediately.

Declaration order, hoisting the prism pointer, caching the attribute in a local, `KCL_File`
temporaries and the whole version sweep were all inert against that word. Recorded in
`notes/pret-idioms.md` as idiom 11.

## The octree walks (updated 2026-08-03)

**`DetectClsn(RaycastGround&)` (0x01ffd3f8, 0x498) MATCHED** on 2004/b56 -- the first overload to
fall, and the largest ITCM match so far. It is RaycastLine's algorithm simplified: a vertical
probe down one (x,z) column, so X and Z early-out both ways but Y only clamps at the top (a probe
starting above the octree falls into it). That is why its frame is 0x4c and not 0xfc -- no AABB,
just a column. The march snaps to the bottom of the leaf just tested and drops one cell, so a tall
empty node costs one iteration.

Its four load-bearing levers, all found by bisection:

1. **Declaration order IS the frame.** mwccarm hands out spill slots in declaration order; the
   ROM's is `x, z, y, found, bestY, leaf, normal, rawX, rawZ, rawY`, with `leaf` and `normal`
   between `bestY` and `rawX`, so those must be function-scope C89 declarations. This alone moved
   the aligner 0.794 -> 0.944.
2. The root index needs **two statements** (`idx = zpart | ypart; idx |= (u32)x >> shift;`) --
   folded into one, mwccarm hoists the octree base load and burns a register on it.
3. Both index expressions must run **z, y, x**; written x-first the shifts fold into two ORRs
   where the ROM materialises `zbit << 2` on its own.
4. `rawY - vtx[1]` must be a **named local** or two temp slots swap.

Notably there was **no frame wall at all** here -- the 0x4c frame, including the two slots holding
only the constants 0 and 1, came out right on the first draft.

### And the RaycastLine frame wall is broken

The sibling's floor said its 0xc4-vs-0xfc frame gap was fourteen spilled scalars. **That was
wrong.** Slots 0x04-0x48 are the same scalars the draft already had; the missing fourteen words
are the **nine non-address-taken Vector3 locals held as un-SROA'd stack aggregates** (`delta` and
`scaled` are write-only -- kept dead stores, the SROA-block signature). Accounting: +27 words of
aggregates minus the 13 temp slots scalarization was using = +14 = 0x38.

**The lever: a local vector type with a user-declared destructor** (`struct DVec { s32 x,y,z;
~DVec(){} };`) blocks SROA. Dead `&x` statements, references and launders do not.

`sub sp,#0xfc` now matches the ROM under both 1.2/sp2p3 and 2004/b56, and the divergence halved
476 -> 238. What remains is one register rank 3-cycle -- see the banked floor in
`nearmiss/db.jsonl` for the full inert-lever list and the three suggested routes. Cracking it
should transfer to the 7,112-byte `SphereClsn` overload, which shares this traversal.

**Tooling gotcha found here:** `tools/fdiff.py` compiles with `M.CANONICAL`, now `2004/b56`. This
cluster verifies at 1.2/sp2p3 and the two builds emit different sizes for this function (0x738 vs
0x740), so fdiff alone can measure the wrong build, and it has no `--version` flag.

## DetectClsn(RaycastLine&) -- original structural recovery

0x01ffb0fc, 0x734, 461 instructions. Attacked next because it is the smallest of the three
octree walks and shares its traversal with the other two. **It did not match.** Best tip is
banked in `nearmiss/db.jsonl` at **div=476** (the DB metric) and the attempt is in
`config/match_attempts.jsonl`. What follows is the algorithm, which is the part worth
keeping -- the residual is register allocation, not understanding.

Progress across four drafts, by size and by the fdiff aligner:

| draft | frame | size | aligner | DB div |
|---|---|---|---|---|
| naive, inner scopes | 0xc4 | 0x718 | 0.201 | — |
| C89 decls in ROM slot order, `u32` loop vars | 0xc4 | 0x728 | 0.233 | 596 |
| + `KCL_File*` cached in a local | 0xc4 | 0x70c | 0.248 | 541 |
| + endpoint pointers, no `file` caching (**banked**) | 0xc4 | 0x720 | 0.262 | **476** |

**The wall is the frame.** The ROM's is `0xfc`; every draft compiles to `0xc4`. That 0x38
is fourteen words, and it is exactly the set of scalars the ROM spills and mwccarm keeps in
registers for us -- including two slots (sp+0x44, sp+0x48) holding nothing but the constants
0 and 1, which is what maximum register pressure looks like. Until the source produces that
pressure, every offset downstream of it is shifted and the aligner sees noise. Chasing
individual divergences before the frame matches is wasted budget.

Note the metrics disagree in direction: caching `this->file` in a local improves the DB
divergence and *hurts* the size. Trust the DB metric (`nearmiss_db.evaluate`), per the same
warning in `notes/arm9-endgame.md`.

### The algorithm

Everything below is confirmed against the disassembly.

**Setup.** Convert both segment endpoints from Fix12i to KCL raw units (`>> 6`), take the
componentwise min/max, then pad the box by `0x40` raw units on each side. Convert each axis
to a cell range: `lo = (min - origin) >> 6` clamped to `>= 0`, `hi = (max + pad - origin) >> 6`
clamped to `<= ~mask`. If any axis has `lo >= hi`, return 0. The cell is therefore 0x40 raw
units = one whole world unit, which is what makes `GetUnkOctreeY`'s `~mask << 12` line up
with `origin << 6`.

**The march** is three nested loops over cells, but the step is not 1: each iteration sets
`stepX` to the distance from x to the far edge of the octree node it landed in, and takes a
running *minimum* into `stepY`/`stepZ` (both reset to 1,000,000 at the top of their body).
So a large empty node is crossed in one step on all three axes.

**Node lookup.** Root index is
`(z>>coordShift)<<zShift | (y>>coordShift)<<yShift | (x>>coordShift)`. While the word is
non-negative it is a byte offset to a child block: add it to the running base, decrement the
shift, and index the child by one bit per axis, `((z>>shift)&1)<<2 | ((y>>shift)&1)<<1 |
((x>>shift)&1)`. A negative word is a leaf; clearing bit 31 gives the byte offset of a
zero-terminated `u16` triangle list, biased by -2 so the walk is `while (*++leaf)`.

**Two skips that are easy to miss.** A leaf equal to `prevLeaf` is skipped whole. Within a
row, the leaf with the largest y-extent that has a non-empty list is remembered, and becomes
`prevLeaf` for the next row -- so a node spanning several rows is only tested once.

**Per prism.** Signed distance of both endpoints to the prism plane via the face normal; the
start must be strictly positive and the end strictly negative or the segment misses. The
parameter is `t = cstd::fdiv(dotStart >> 4, denom) << 4` with `denom = (dotStart - dotEnd) >> 4`,
guarded by `denom > 0` and by `func_020397dc` rejecting `|denom| <= 8` -- a
near-parallel guard, which is why a grazing ray cannot divide by a tiny denominator. The hit
point is `d0 + ((d1 - d0) * t >> 16)`, relative to vertex 0. Then the three edge normals:
`enrm1` and `enrm2` must both dot to `<= 0x20000`, and `enrm3` must land in
`[-0x20000, prism->length + 0x20000]`. That is the only read of `KCL_Prism::length`, and it
confirms the field is the prism's extent along its third edge normal.

**Acceptance.** Distance from the segment start, `Vec3_Dist(&hit, &s) >> 6`, must beat the
running best (seeded from `ray.clsnDist >> 6`). Then the surface is fetched --
`func_020396dc(this, prism)` is `(prism - file->prisms) / 16`, i.e. the triangle index, and
the fetch is a **virtual** call to slot 3, `GetSurfaceInfo` -- and offered to
`BgCh::ShouldPassThroughImpl`, whose fourth argument is `func_020397b8(normal.y)`, true when
the normal's Y is in `(-0xccc, 0x600)`, i.e. a steep-ish surface. If it does not pass
through, the hit is recorded into `ray.result` and `found` is set.

**Result.** On a hit, `ray.clsnDist` gets the best distance back in Fix12i, the hit position
is written over `ray.lineEnd` (via `func_020375ec`, which writes ray+0x54 -- the ray is
shortened to the hit), and `ray.hasClsn` is set.

### Where to go next

Two options, and the first is cheaper than it looks.

1. **Fix `tools/modules.py` first.** Without it nothing in the merge gate can verify an ITCM
   function, so every match here has to be hand-argued. Six lines.
2. **Get the frame to 0xfc** before touching anything else in DetectClsn. The banked tip is
   the right structure; it needs the register pressure, not different logic.

The 25 unnamed `func_01ff…` in the 0x01ff8000..0x01ffa9dc block have never been looked at
by anyone, and at 8-24 functions of ordinary size they are likely cheaper per match than
anything left in arm9.
