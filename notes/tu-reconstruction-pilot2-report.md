# TU reconstruction pilot 2 — agent-driven minigame TU merges (`ov006`)

**What this is:** the at-scale rehearsal for the minigame conversion. Two ov006
minigame TUs — one single-class (`dScMgBSC_c`), one multi-class
(`dScMgMCarlo_c` + `dMgMCarloCardObj_c`, the ChillBully #1837 shape) — were
driven end-to-end by two concurrent agents through `tubuild.py create ->
reconcile -> compile -> verify -> linkcheck`, each agent holding its own
`agentlock` resources (one address range + its full file set), the coordinator
owning the serial gates, ledgers, and commits. The pilot exists to measure:
wall clock, compile/verify iterations, coordinator interventions, and
surprises — the numbers that decide whether the minigame conversion can be
fanned out at scale.

**What it is not:** a build change. Both shadow TUs are unenrolled,
`config/**/delinks.txt` is untouched, and the 42 legacy one-function sources
remain the sole enrolled owners of both ranges. No `rombuild.py`, no
`eligible.py`, no promotion.

---

## 1. Headline result

Both TUs are **TEXT-VERIFIED** — produced by the agents on an early pass, then
independently reproduced by the coordinator after the runs (byte comparison,
`objisolate` plan, relocation destinations all clean, single verify pass each):

```
TU ov006/dScMgBSC_c+dScMgBSC_c          (19 functions, 0x02124908..0x021255f8)

byte comparison   : 19/19 MATCH  (tools/match.py extract_func + compare, relocation-aware)
objisolate check  : clean  (tools/objisolate.py plan())
emission order    : all 19 function(s) in the expected ROM-ascending section order

Result: 19/19 MATCH, objisolate clean, reloc-destinations clean -> TEXT-VERIFIED
```

```
TU ov006/dMgMCarloCardObj_c+dScMgMCarlo_c+dScMgMCarlo_c   (23 functions, 0x020f7634..0x020f8e44)

byte comparison   : 23/23 MATCH  (tools/match.py extract_func + compare, relocation-aware)
objisolate check  : clean  (tools/objisolate.py plan())
emission order    : all 23 function(s) in the expected ROM-ascending section order

Result: 23/23 MATCH, objisolate clean, reloc-destinations clean -> TEXT-VERIFIED
```

The third leg (whole-TU linkcheck) did not complete for either TU — not
because of the TU contents, but because both ids hit structural pipeline gaps
that no previous TU had ever exercised (§4.2, §4.3). The banked `--partial`
diagnostics carry the link evidence instead:

- **BSC** (34-char stem, one `+`): partial reached the full link — all phases
  through checkModules true, module fidelity passed, TU range
  0x02124908..0x021255f8 **0 differing bytes**, **0 NEW symbols** vs the
  9-error baseline; contribution equivalence 17/19, the two non-equivalents
  being the D1/D0 vague-linkage pair inherent to the form-B inline destructor
  (STB_LOPROC vs production STB_GLOBAL), which mwldarm coalesces — the linked
  module is byte-exact regardless.
- **MCarlo** (54-char stem, two `+`): both linkcheck forms abort at the OS
  file-name limit before their own selector is ever reached, so the partial
  banked is the pre-link diagnostic — contribution equivalence 21/23 (the same
  D1/D0 signature, independently confirming the form-B artifact across TUs),
  131 extra inert imports all with `symbols.txt` homes, 0 without.

## 2. Measurements

| Metric | BSC (single-class) | MCarlo (multi-class) |
|---|---|---|
| Functions / classes | 19 / 1 (form B) | 23 / 2 (one zero-text) |
| ROM text range | 0xCF0 bytes | 0x1810 bytes |
| Agent wall clock | ~24m34s | ~31m26s runtime¹ |
| Compile runs | 4 (all pragma scoping) | 2 (1 fail: L1006 implicit conversion) |
| Verify runs | 2 (PASS, then sanity PASS) | 3 (22/23 → 23/23 → post-policy PASS) |
| fdiff oracle runs | 8-combination pragma probe | 3 |
| linkcheck runs | 2 (whole-TU lcf abort; partial full-link clean) | 2 (both MAX_PATH abort; partial pre-link diagnostic) |
| `compiler_only_output` rows | 13 deadstrip-data | 13 deadstrip-data |
| Tool uses | 143 | 171 |
| Locks held | addr + 21 files, renewed throughout | addr + 25 files, renewed throughout |

¹ MCarlo's session spanned a compaction (~2 days wall), so the honest number is
its runtime; its own phase log puts the active tooling window on the final day
at ~7–8 minutes plus an untimed reconcile phase before the compaction.

**Coordinator cost (the thing the pilot really measures):**

- Interventions during agent runs: **1** — a shortcut note to MCarlo after BSC
  proved the `+` lcf blocker, telling it not to re-derive the grammar analysis
  (MCarlo still hit a *different, earlier* blocker, so the note saved one
  dead-end cycle, not the diagnosis).
- Serial gates: 2 independent verify re-runs (both first-try PASS), 2 manifest
  integrity checks, 4 lock releases, 2 ledger-row rewrites, 3 commits.
- Both agents stopped at the stop rule on their structural blockers and
  reported instead of forcing — no allowlist violations, no writes outside
  their two files each, locks renewed and never released early.

## 3. What the reconciliation looked like

The manifests carry the full notes (6 and 7 reconciliation notes respectively);
the shape in brief:

- **BSC**: form-B local class — inline destructor in the body carrying the
  verified D1 body, members verbatim from `include/dScMgBSC_c.h` — because the
  ROM's function order demands the D1-then-D0 head with no D2; 13 deadstrip-data
  policy rows license the vtable/RTTI chain the form drags in, leaving
  `_ZTV10dScMgBSC_c` external exactly as the per-function objects had it.
- **MCarlo**: `dScMgMCarlo_c` the same form-B way; `dMgMCarloCardObj_c`
  deliberately gets NO local class — it contributes zero text, its only
  appearance being one vtable-address import in 7730, and card RTTI stays
  gap-owned. Recognizing that early avoided a phantom second class and
  phantom policy rows: on the compile side, multi-class was nearly free.

## 4. Findings

### 4.1 FIXED — `tubuild linkcheck` ignored compiler-only policies in the whole-tree compile

Every promoted TU carrying a `compiler_only_output` row (arm9/ActorBase_SceneNode,
ov100/daObjPathLift_c) failed `[4/8]` fail-closed at isolation, because the
linkcheck executor called `RB.compile_one` without the policy the real build
passes. Fix: compute `RB.compiler_only_policies(srcs)` once, hand the
TU-under-test's own entry to the `[4b]`/`[4c]` raw-object audits instead
(double-apply is refused), and pass the rest into the compile lambda.
Validated on the banked `ov006/MgPicturePoker` control: 11,075 enrolled
sources, 0 failed, **MODULE-LINK-VERIFIED**, 0 NEW symbols vs baseline —
the first linkcheck evidence that manifest has ever carried (banked in
c954daf2e). Commit **d110b421f**, standalone, ready to upstream on its own.

### 4.2 OPEN — mwldarm's lcf grammar cannot express `+` in an object basename

`dScMgBSC_c+dScMgBSC_c.o(.text)` parses as object `dScMgBSC_c` followed by the
section-list operator: *"File not found: dScMgBSC_c / Expecting: ("* at lcf
line 11095. Every previously link-verified TU has a `+`-free stem; class-shaped
ids (`Class+Class` from tu_map's multi-class labeling) are the first to attempt
the whole-TU linkcheck. Needs a pipeline-level naming decision, not a per-TU
workaround — the basename derives from the manifest source path and the
delinks allowlist pins it.

### 4.3 OPEN — Windows MAX_PATH caps the scratch tree for long multi-class stems

MCarlo's 54-char three-class stem deepens `build/tu/<id>/link/src/...` far
enough that a corpus object (sorted before its own selector) exceeds the OS
file-name limit: *"The file name is too long."* The multi-class cost
concentrates entirely in the link pipeline — compile and verify are unaffected.
§4.2 and §4.3 share a natural joint fix: a short, `+`-free scratch-side
basename/directory convention (hashed stems) closes both without touching any
banked file.

### 4.4 RECIPE — a header's data view can be byte-load-bearing

`decl_common.h`'s `extern int` view of `data_ov006_021424fc/02142508` is
ABI-correct, yet under it mwcc CSEs the pair's loads and the ROM's uncached
reload sequence plus its ldrgt-before-lslgt order do not survive: **8 bytes
and 5 relocations lost to a view change alone**, function body identical.
Fix: extend the macro-shield technique to data identifiers, declare the
file-scope `Node*` view, restore the legacy body verbatim; fdiff is the oracle
that decides which view the ROM needs (149/149). Nothing in the recipe warned
that a mere extern *type* could cost bytes — now it does.

### 4.5 RECIPE — `#pragma opt_strength_reduction off` is file-global, last-wins (2004/b56)

Measured with an 8-combination probe: the pragma cannot be scoped to one
member. The two legacy sources that carried it each opened with it and never
closed it, so the original merged TU's own final state was off for everything —
whole-file off is historically correct, not a compromise. Generalization
already in the generator's note: positional pragmas (`long_calls`) carry
bracketed; other global pragmas need hand adjudication.

### 4.6 RECIPE — extern "C" signature collisions are call-syntax-only

The legacy files split on `_Z14ApproachLinearRiii` (reference form vs pointer
form). Under extern "C" both spellings pass the address in r0 — pick either
and normalize the call sites; both measured inside the 149/149 match. The
recipe's union rule ("keep the most complete observation") could not resolve
this class of conflict because neither side is more complete.

### 4.7 PROCESS — the stop rule held twice

Both agents hit a structural blocker they could not fix inside their two-file
allowlist, and both stopped and reported rather than forcing (no subst drives,
no registry edits, no renames, no unforced tool edits). The pilot's most
important process result: the allowlist + stop-rule combination is sufficient
supervision for unsupervised agents on this pipeline.

## 5. Scale-up math

- **Inventory**: ov006's symbol table carries 32 `dScMg*_c` scene-class labels
  (30 minigame scene classes + the two shared 3D bases) and 5 `dMg*` helper
  classes; **1,596 per-function sources** in `src/` still own ov006 text.
  Banked so far: 6 small Tier-1 slices (#1827) + this pilot's 2 whole-class
  TUs. The high-confidence fan-out unit is the whole-class TU at the
  BSC/MCarlo grain — on the order of 25–35 more TUs for the scene classes,
  more if helper classes get their own.
- **Measured cost**: ~25–35 min agent time per whole-class TU (19–23
  functions), 2–4 compile iterations, ≤3 verify runs; ~5 min of serial
  coordinator time per TU (verify re-run, manifest check, ledger/claims,
  commit, locks).
- **Parallelism**: 2 agents ran concurrently with zero interference — disjoint
  locks, disjoint scratch trees, read-only shared caches. Nothing in the
  measured flow is mutually exclusive below the coordinator's serial gates.
- **Projection**: at the measured costs, the ov006 scene-class corpus is
  ~15–20 agent-hours — a few wall-clock hours at 4–6 concurrent agents — plus
  ~3 hours of serial coordinator gating. The blockers in §4.2/§4.3 gate only
  the third leg (linkcheck); legs 1+2 (TEXT-VERIFIED) can fan out
  immediately.

## 6. Outside-perspective review

Per the project owner's standing instruction, an independent review agent —
fresh context, no inherited assumptions — read both TUs, both manifests, the
real headers, the banked precedents (ChillBully, the six Tier-1 manifests,
promoted `ActorBase_SceneNode`), the plan, and the tool, and judged the pilot
against the goal sentence: *"period accurate idiomatic, readable, c++ code
the original EAD team would have written."* The coordinator independently
re-verified the review's three highest-stakes claims before acting on them.

### 6.1 What the review found

**The byte pipeline is strong; the goal has no gate.** Every rung of the
ladder (verify, linkcheck, promote) scores bytes. Nothing anywhere scores
readability, idiomatic shape, naming, or comment accuracy. The review's
demonstration: BSC's TU shipped green with a comment claiming
`func_ov006_02125364` was "the deleting destructor, vtable slot 17" —
contradicted by its own body two lines below (a mode-dispatched handler
touching `mHudScore` and the state field), by its own sibling comment at TU
line 219, and by the manifest (the real D0 is 0x0212497c). The coordinator
confirmed it against the cited oracle — `tools/rtti_vtables.py --own
dScMgBSC_c` puts D0 at slot 17 / 0x0212497c and `func_ov006_02125364` at
**slot 18** — so the legacy comment the reconciliation pass "corrected"
was right all along. **The comment is now fixed** (oracle-checked rewrite;
re-verified 19/19 after the edit; the correction is its own commit). The
incident is the pilot's most valuable process finding: judgment errors on
the goal axis ship green and are invisible until someone reads the code.

**Ranked gaps between the TUs and EAD shape:**

1. ~80% of the volume is still `extern "C"` free functions taking `char*`
   with raw offsets — while the TU's *own type system* asserts they are
   members (the state-dispatch table is typed `void (dScMgBSC_c::*)()`).
   [DISTANCE, the dominant cost]
2. MCarlo's card class is spelled four ways in one file (`Node`, `Obj`,
   `NodeR`, `SceneVt` shims) — three granularities of ONE EAD class, with
   its virtuals as free functions and its vtable-store as a bare extern.
   [DISTANCE]
3. The local form-B class definitions contradict the real headers, which
   still encode the per-function-era out-of-line-destructor shape; 13 RTTI
   siblings share that shape, so the fork must be settled family-wide
   before promotion. [form B: INHERENT; the header/TU fork: PIPELINE,
   promotion-blocking]
4. One field, many spellings: the state field at 0x51b8 is `unk_51b8` six
   times and raw offsets sixteen times in three notations, in the same
   file; `mHudScore` is named in two functions and `*(int *)(o + 0xb4)` in
   two others. [PIPELINE, the largest readability multiplier]
5. Redundant shadow typedefs (`typedef int s32` beside the included
   `types.h`). [PIPELINE, trivial]
6. Per-TU macro shields for `decl_common.h` views the pilot itself measured
   wrong (the 149/149 fdiff evidence) — the goal-serving move is one header
   fix verified across consumers; the shield is the byte-safe per-TU move
   and will be re-derived by every future TU touching those globals.
   [PIPELINE, multiplies]
7. Reverse source order, ordinal banners, file-global pragma: the honest
   price of byte-match; the review explicitly would not spend effort here.
   [INHERENT, acceptable]
8. Comments are about the pipeline, not the game: no state machine named
   (values 7, 9, 0xa–0xd are bare), no member says what it does for the
   minigame — plus the wrong comment above. [DISTANCE]
9. Cross-TU extern-signature splits between the two sibling TUs within one
   pilot: `_Z14ApproachLinearRiii(int&, int, int)` vs `(int*, int, int)`,
   `RandomIntInternal(int*)` vs `(void*)` — both byte-true because
   extern "C" does not mangle parameter types, which is exactly why
   nothing catches it. Confirmed by direct read of both files. [PIPELINE]

### 6.2 Scale-up risks the pilot does not reveal

- **ov006 is the plan's own forbidden ground** (plan §14: don't begin with
  under-segmented modules; MCarlo's `boundary_confidence: medium` sinit
  corroboration is module-wide, not narrowed per TU). A two-TU pilot earns
  the exception; a fan-out needs per-TU boundary corroboration first — an
  invented TU boundary is worse than per-function files because it acquires
  a filename, a manifest, and a promotion claim.
- **The boundary policy contradicts the pilot-1 precedent, silently**:
  PoleLift included its Spawn factory; both minigame TUs excluded theirs,
  each reconciler just drawing the line at the byte-match range. Nobody
  decided. Decide with evidence before fan-out or every scene TU may be
  under-bounded and promotion strands the factory.
- **The 13-sibling class-form decision will otherwise be made 13 times,
  concurrently**, in 13 private dialects, and the two pilot TUs already
  disagree about their own status comments.
- **Per-file locks cannot coordinate shared-hidden-file surgery**
  (`decl_common.h` views, `delinks.txt` at promotion, attribution trackers) —
  exactly the operations scale-up needs most.
- **Pragma-conflict merges are uncounted**: BSC was lucky (both pragma
  carriers agreed); plan §10 predicts a TU with *conflicting* file-global
  pragma needs is unmergeable. A pre-fan-out census over the candidate
  list is cheap and excludes dead ends before agent-weeks burn.
- **Linkcheck economics are unmeasured**: verify is cheap and parallel;
  linkcheck compiles the whole tree per run. At 2 TUs invisible, at 20 it
  may be the serialization point.

### 6.3 Coordinator's adjudications

Two corrections to the review, both accepted in substance: (a) the six
Tier-1 Mg manifests were banked in #1827 *before* this pilot discovered the
linkcheck blockers — the pilot banked two TUs and found the blockers; the
forward-looking point stands (stop banking class-shaped TUs until the
ladder's top rung is fixed, or bank them knowing the third leg is deferred).
(b) MCarlo's id "names a class the TU does not define" — half right: the
card class's *methods* are the TU's content (as free functions); what the id
actually gets wrong is the **duplicated `dScMgMCarlo_c` label** and the
`+`-grammar consequences. That duplication is a tu_map labeling bug worth
fixing alongside the `+`-safe basename policy.

The review's proposed goal-side gates, all mechanical and ROM-free, are
endorsed: a **shadow census** in verify (count local typedefs/structs/shields
a real header already defines or contradicts; zero-or-justified at
link-verified so it becomes a promotion prerequisite), **named-member
coverage** (flag raw-offset access to fields the headers name), a
**cross-TU extern-signature consistency check** per module (would have
caught both splits), and a **comment/claim review field** in the manifest.

## 7. Verdict

**The measured answer to the pilot's question — can agents with agent-locks
convert minigames at scale? — is yes for the byte pipeline, and not yet for
the goal.**

What scales now, on the pilot's own numbers: whole-class TUs reach
TEXT-VERIFIED in ~25–35 agent-minutes with one coordinator intervention per
run, near-zero serial cost (~5 min/TU), and proven 2-way concurrency. Legs
1+2 are production-ready.

What does not scale yet, per the review and this pilot's own incidents:

1. **The promotion ladder's top rung is broken for every class-shaped TU**
   (§4.2 `+` grammar, §4.3 MAX_PATH). Fan-out before the fix banked entries
   whose promotion path cannot be exercised and whose form-B destructor pair
   — the one novel codegen bet — has never survived a real whole-TU link.
2. **The family-wide decisions will be forked, not made** (class form across
   13 siblings; Spawn/element-ctor boundary; shared-helper ownership;
   pragma-conflict census). Per-TU agents under file locks cannot make
   these; they can only produce 13 private variants of them.
3. **The goal axis is ungated.** The wrong-destructor comment shipped green
   and was caught only because the owner's review cadence produced a reader.
   At 25–35 TUs, that class of error ships silently; and the bulk of the
   produced text (extern "C" bodies, shadow structs, per-TU shields) is a
   private dialect that family-wide unification would later invalidate,
   forcing re-verification of the whole accumulated corpus.

**Recommendation — conditional GO, in this order:**

- **Now, infrastructure (coordinator/maintainer work, no agents):** fix the
  two linkcheck blockers (`+`-safe scratch basenames + short scratch stems
  closes both; plus the tu_map duplicate-label bug); upstream the
  `compiler_only` policy fix (d110b421f) as its own PR; write the one
  family-wide decision note (form B propagation with a measured prototype
  header change on one sibling; Spawn/element boundary policy per the
  PoleLift precedent; `020c1d80`/`020c1c64` shared-helper ownership;
  pragma-conflict census over the candidate list).
- **Then a measured mini-wave (2–3 TUs, agents, goal gate ON):** add the
  shadow census and cross-TU extern checks to verify, require one
  game-semantics comment per member, and measure the *real* per-TU cost of
  goal-satisfying output — the pilot measured the byte path; the naming and
  member-conversion work adds unmeasured time on top.
- **Then full fan-out**, wave size set by the mini-wave's numbers, with the
  outside-perspective review as a standing per-wave gate (this review is
  instance #1 of the owner's requested cadence).

**Decisions for the owner:** (1) upstream d110b421f now or with the pilot
PR? (2) the `+`/MAX_PATH naming fix — scratch-side sanitization (no banked
file changes) vs a promoted-TU rename convention? (3) confirm the
conditional-GO order, or fan out legs 1+2 immediately and accept the
re-verification debt.