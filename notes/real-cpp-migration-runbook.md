# Real C++ migration runbook

This runbook is for converting already matched SM64DS sources into the kind of
C++ the original Nintendo EAD codebase plausibly contained.

The goal is not to make a C function compile after changing its extension. The
goal is to make the compiler own the C++ ABI:

- define `Class::Method`, `Class::Class`, or `Class::~Class` in source;
- let mwccarm spell the mangled symbol;
- model inheritance, members, vtables, and lifetime in headers where evidence
  supports them;
- preserve the exact ROM bytes and every relocation destination.

A `.cpp` file that defines `_ZN...` by hand, or wraps such a definition in
`extern "C"`, is still C-shaped source. Do not submit that as a real-C++
migration. `extern "C"` can be legitimate at an actual C ABI boundary, but it
must not be used to disguise a hand-mangled C implementation.

PR #1758 is the reference shape: it reconstructed real destructor ownership,
used headers and inline class operators where the ABI required them, and left
compiler walls unmigrated instead of forcing fake source.

## Non-negotiable acceptance criteria

Every migrated source must satisfy all of these:

1. The definition is compiler-spelled class or namespace C++.
2. The isolated function bytes match the ROM with mwccarm 2004/b56.
3. Relocations resolve to the exact ROM destinations; `MATCH` alone is not
   enough.
4. Every affected source still matches after a shared-header change.
5. The full linked build has zero mismatching functions and all 106 modules are
   exact.
6. `.c` to `.cpp` renames preserve `port/` references and contributor credit.
7. The language-mode audit improves or stays within its ratchet.

If any gate fails, restore the old source. A plausible near-match does not
belong in `src/`.

## 1. Start safely and coordinate

Read [AGENTS.md](../AGENTS.md), [the agent protocol](agents/PIPELINE.md), and the
assigned task's accepted input and handoff. Resume existing work and proof before
creating a new branch; only a new scope starts from current main. Use a separate
[wired worktree](worktree-inputs.md) to protect the primary checkout. Keep build
outputs private and dispose of linked worktrees with the setup helper's safe
removal counterpart.

Use the current [v2 queue](agents/queue-v2.md) before edits and reserve shared
headers, lifecycle owners and ROM ranges. Schedule an independent verifier for
every source candidate; independence is required even with a small agent budget.
The producer, verifier and integrator follow their protocol roles, not a second
claim/release workflow in this guide. Launch/resume prompts live in
[LAUNCH.md](agents/LAUNCH.md).

Check the current source frontier rather than trusting an old list:

```powershell
python tools/langmode_audit.py --by-class
python tools/langmode_audit.py --list c-mangled
python tools/langmode_audit.py --list cpp-handspelled
python tools/srcpath.py <symbol>
```

For a TU promotion assignment, use [the canonical promotion workflow](tu-promotion-conventions.md).
Do not add separate folder-localization or per-function staging PRs by habit.
This runbook supplies source-form techniques; it does not turn a production task
into a shadow-only research task.

## 2. Choose a coherent slice

Prefer a small class-owned group that can be proved independently:

- ordinary methods with an existing, trustworthy class header;
- destructor variants whose base and member lifetimes are already known;
- a base class before derived classes that depend on its layout or destructor;
- a member type before an owner whose destructor must destroy that member.

Do not start with a broad header rewrite. First map the ABI ownership graph:

- direct and virtual bases, including each base offset;
- non-trivial members and destruction order;
- D2 base-object, D1 complete-object, and D0 deleting destructors;
- the class's deleting operator and heap destination;
- vtable and RTTI ownership, address-point addends, key virtual function, and
  multiple-inheritance thunks;
- every manual constructor/destructor consumer that would double-run after a
  real member or base is introduced.

Run this before any header edit:

```powershell
python tools/affected_src.py include/<Class>.h
```

If the ownership graph is incomplete, migrate a smaller method-only slice or
stop. Do not guess shared layout.

## Small library and runtime workstream

This workstream improves small library and runtime sources while preserving their
production output. Actors and minigames are outside its scope. Ordinary C and
hardware assembly remain appropriate where the evidence supports them; a C++
extension or a lower audit count is not the objective.

Start with one understandable operation or interface: fade interpolation, a matrix
helper, or a cache/palette upload boundary. Prefer named members, coherent
declarations, and compiler-spelled calls over duplicate layouts, raw mangled calls,
and unnecessary casts. Do not invent a type, original name, or TU boundary to make
the source look finished.

### Working method

1. Read the current [v2 queue](agents/queue-v2.md), accepted handoff, and immutable
   base. Reserve the exact sources, shared headers, and ranges through the existing
   protocol before editing. The candidate list below grants no ownership.
2. Trace the chosen operation through its callers, declarations, and definition.
   Include an existing canonical header. If none exists, a narrow native
   declaration matching the definition is a useful first experiment; a new shared
   header must serve the selected consumers and contain no speculative API or types.
3. Separate name evidence: [RTTI proves stored class names and base relationships;
   method names and parameter spellings need their own
   evidence](symbol-name-provenance.md). A hand-spelled mangled call can indicate
   declaration debt or a compiler limitation; it does not establish either one.
4. Form a small source hypothesis and compare baseline/candidate objects under the
   same pinned compiler and flags. Inspect all relevant sections, symbols, and
   relocations, including generated data and lifecycle output. Identical whole
   objects are strong evidence for a local cleanup; a text-only match is narrower.
5. Apply [candidate proof](#5-prove-each-candidate-before-growing-the-batch) and
   [consumer checks](#6-verify-migration-side-effects), including declaration
   agreement and actual `complete` delinks ownership. Obtain independent source
   review and final production validation through the existing workflow. Report
   pre-existing failures separately from failures introduced by the candidate.
6. Update the log below with the immutable candidate, measured result, remaining
   limitation, review/proof state, and next scope. Refresh evidence when the base,
   compiler inputs, shared headers, or candidate changes. Reference private artifact paths in
   the handoff; do not turn this guide into a second queue or validation database.

Treat recipes and old "required for codegen" comments as hypotheses to retest.
Remove a stale claim when the experiment disproves it, and retain the precise
boundary of any limitation that remains. A successful cast cleanup does not prove
that every cast is removable or that the reconstructed type is historically exact.

### Candidate order

These are review leads as of 2026-10-18. Recheck current source and live ownership
before adopting any row; do not expand a small slice into a whole-tree header pass.

| Order | Scope | Question to settle |
| --- | --- | --- |
| Current | `src/runtime/graphics/fader/d_fader.cpp` | Remove avoidable field-access casts, use native helper declarations where proved, and correct stale leftover comments. |
| Next | `Matrix4x3_FromTranslation.c`, `Matrix4x3_ApplyInPlaceToScale.c`, `Matrix4x3_ApplyInPlaceToRotationX.c` under `src/` | Can the tiny helpers share an evidenced trivial matrix view and agree with their callers? `common.h` and `math/Matrix.h` have different source shapes; the latter contains a nontrivial `Vector3`, so changing the include can change lifetime output. |
| Next | `src/cstd.cpp` / `src/ApproachAngle.cpp` caller boundary | Establish narrowly shared declarations if useful. The current integer `ApproachLinear` definition returns `int`; do not preserve a caller's `void` declaration merely because its result is ignored. |
| Next | CP15 and GX/GXS cache/palette upload boundary | Replace avoidable manually mangled calls using the existing native definitions; add only declarations supported by this consumer scope. Preserve hardware-required assembly. |
| Later | `src/runtime/graphics/fader/d_wipe.cpp` | Review the separate IRQ/scanline-table implementation as its own slice, after the smaller fader work. |

### Running evidence log

**2026-10-18 - fader cleanup, locally proved.** Baseline:
`39e85ae247da338885b6063c69bf7fef40e99454`. Candidate: this commit; its immutable
SHA and independent review are recorded by queue task `fader-clean1018`.
See the [handoff](agents/handoffs/fader-clean1018.md) and
[experiment record](experiments/fader-clean1018.json) for commands and limitations.

- All seven cast-through-`char*` field accesses now use direct members. Five helper
  interfaces use native declarations/calls, `ApproachLinear` agrees with its
  definition's `int` return and reference parameter, and three matrix declarations
  use `Matrix4x3*`. The entire final 12,920-byte object is identical to baseline:
  SHA-256 `93cfa557e878e4ef846d30012690703b06c50d919cbfc3793f19f8887e270f49`.
  Those particular alias casts were unnecessary. A host divider forwarder keeps
  native fader calls working in MSVC; the Win32 Release smoke test passes.
- Using `G2x.h`'s existing `short` brightness parameter at the currently `int`-typed
  boundary grew `dFdWipe_c::AdvanceFade` from 320 to 328 bytes and
  `dFdColor_c::AdvanceFade` from 148 to 156 bytes. Fifteen plausible native-call
  variants did not match. These bounded failures do not establish impossibility
  or the original parameter width. The explicit bridge remains pending a
  callee/caller audit; no shared signature was changed to hide the disagreement.
- Strict relocation proof passed all 30 enrolled functions without blind spots.
  The production run reproduced all 106 ARM9 modules, 11,334 functions, and 348
  data claims, with no new errors. Nine inherited symbol errors and one differing
  broad data advisory remain; this is not a clean whole-tree source audit.
  `--no-rom` skips candidate container packaging. Declaration disagreements fell
  by two without a baseline edit. Independent acceptance must be read from the
  queue record for the exact candidate; producer results are not that acceptance.
- Next: review the three small matrix helper interfaces, then narrowly selected
  math and hardware helper declarations. Do not expand into actors or minigames.
  The base-header allocator bridge and scene/frame manual dispatch remain outside
  this candidate. This is a proved partial cleanup, not a completed fader family.

For each later entry, record **date; base/candidate SHA; reserved scope; source
change; A/B evidence; remaining limitation; independent review; production result;
next step**. Keep a result pending until the corresponding gate has actually run.

## 3. Write real class-form source

For measured destructor, vtable, constructor and language-mode behavior, consult
[class-form compiler observations](cpp-class-form.md). These are candidate
techniques to verify, not another launch workflow.

The first line of a C++ source must be exactly:

```cpp
//cpp
```

Include the real header and define the real entity:

```cpp
//cpp
#include "Example.h"

void Example::Update()
{
    // body
}
```

For a destructor, write a destructor:

```cpp
//cpp
#include "Example.h"

Example::~Example()
{
}
```

Never use either of these as the migrated implementation:

```cpp
extern "C" void _ZN7Example6UpdateEv(Example* self) { /* ... */ }
void _ZN7Example6UpdateEv(Example* self) { /* ... */ }
```

Ask the compiler for names when needed:

```powershell
python tools/mangle.py
```

Do not contort source to preserve a guessed parameter type. Mangled parameter
types are not automatically ROM-proven; validate the declaration against calls,
relocations, and compiler output.

## 4. Destructor-specific rules

Destructors are whole-object ABI work, not isolated syntax changes.

- D2 destroys the base-object view.
- D1 destroys the complete object.
- D0 destroys and then deallocates through the class's actual delete path.
- Members are destroyed in reverse declaration order, then bases in reverse
  construction order.
- A real member or base destructor makes manual calls at old consumers wrong;
  migrate or remove those calls in the same coherent slice.

Use the fail-closed probes to discover candidates, not to prove them:

```powershell
python tools/d0_migrate.py --list
python tools/d0_migrate.py <ClassName>
python tools/dtor_probe.py --list
python tools/dtor_probe.py <ClassName>
```

mwccarm emission is source-form sensitive. Proven useful patterns include:

- an out-of-line `Class::~Class()` definition for the owned variant;
- an explicit destructor call in an unretained helper to force a complete
  destructor emission;
- defining the key virtual function in the D0 unit when that is what causes the
  compiler to emit the class vtable and destructor variants;
- a class-specific inline `operator delete` when the ROM D0 uses a non-default
  heap path.

These are evidence-guided compiler controls, not permission to keep unrelated
output. Isolate the intended symbol and audit the resulting ELF relocations and
COMDAT/vtable ownership.

Known compiler walls should stay in their existing matched form. Examples seen
in current work include a destructor whose empty C++ body necessarily destroys
a member at `+0x64`, a class without the key function needed to emit the right
variant, and candidates that match in isolation but differ in the final link.

## 5. Prove each candidate before growing the batch

First run the exact per-function compiler gate. Supply explicit target metadata
when the progress index is absent or stale:

```powershell
python tools/match.py --cpp-check --c src/<symbol>.cpp `
  --func <symbol> --addr 0x<addr> --size 0x<size> `
  --module <module> --version 2004/b56
```

Then prove relocation identity:

```powershell
python tools/linkcheck.py --c src/<symbol>.cpp `
  --name <symbol> --addr 0x<addr> --size 0x<size> --module <module>
```

For a group, use:

```powershell
python tools/prepush_linkcheck.py --files `
  src/<symbol-a>.cpp src/<symbol-b>.cpp
```

The desired result is `VERIFIED` with no blind relocation words. Treat these as
failures until explained and eliminated:

- `WRONG`: a relocation reaches the wrong symbol or addend;
- `BLIND`: the tool could not prove the destination;
- `BENIGN`: not publication proof by itself; the full link may still differ;
- a raw `MATCH`: relocated words may have been wildcarded.

After a `.c` to `.cpp` rename, regenerate the delink manifests:

```powershell
python tools/enroll.py
```

Then run the decisive whole-repository gate:

```powershell
python tools/rombuild.py -j16 --no-rom
```

Accept only output with:

```text
reproducing: <all source-built functions>
mismatching: 0
module fidelity: 106/106 exact, 100.000000%
ROM-build analysis: PASS
```

This full link is mandatory. Historical destructor candidates have passed the
isolated byte comparison and still changed the final image by one byte.

## 6. Verify migration side effects

For any changed shared header, re-check every consumer and the full branch diff:

```powershell
python tools/affected_src.py include/<Class>.h
python tools/prepush_linkcheck.py --range origin/main..HEAD
```

Commit-range checks inspect the committed candidate. Commit reviewed changes
before using `--range ...HEAD`, and refresh the relevant proof if the candidate
changes again. Use explicit `--files` checks for local iteration.

For every rename or move:

```powershell
python tools/check_dead_references.py
```

Who matched a function stays the row in `function-authors.json`. A rename does
not change it.

Check the language-mode ratchet using the current generated baseline:

```powershell
git show origin/chaos-data:langmode-baseline.json > $env:TEMP\sm64ds-langmode-base.json
python tools/langmode_audit.py --check $env:TEMP\sm64ds-langmode-base.json
```

Do not commit a lower baseline. The post-merge workflow banks improvements on
the `chaos-data` branch.

Finally run the repository's normal reference and test gates required by
`AGENTS.md` and any changed subsystem.

## 7. Consolidate into a production translation unit

Follow [TU promotion: workflow and review conventions](tu-promotion-conventions.md).
It defines the production result, the actual `tu_promote.py` command, retirement of
absorbed sources, required proof and the handoff through integration.

A separate per-function conversion phase is not mandatory. Reconstruct directly
in the combined candidate when that is the shortest verifiable path. Smaller
production method slices remain useful when explicitly assigned or when a measured
whole-TU blocker prevents promotion. Preserve the evidenced boundary and report
which functions remain C-shaped; grouping them into one file does not change that.

Combined compilation can change function order, literal pools, helper and lifecycle
emission, vtable/RTTI ownership, static initialization and relocation addends.
Use a temporary shadow for those experiments when needed, then continue through
production enrollment. Read the [partitioned reference](agent-partitioned-tu-workflow.md)
only for the relevant output-ownership or partitioning problem.

## 8. Failure and restoration discipline

Work one candidate at a time and keep the old source recoverable. If a candidate
fails:

1. restore its exact old `.c` or hand-spelled `.cpp` source;
2. remove the failed new file;
3. rerun `tools/enroll.py`;
4. rerun the full linked build;
5. record the wall or useful near-miss in the repository's supported evidence
   store, not in `src/`.

Do not weaken a shared header, add an ABI lie, add `extern "C"`, suppress a gate,
or rely on a ROM-byte fallback to make the batch green.

## 9. Keep PRs reviewable

Source migrations and documentation/tooling changes are separate PRs.

A source-migration PR should contain only the coherent migration and required
companions such as delink paths, header consumers, `port/` references,
attribution metadata, and the completed class claim. Its body should list:

- the real classes/methods/destructors migrated;
- why the compiler now owns their ABI;
- strict relocation results;
- affected-header consumer count;
- full source and module fidelity;
- port, dead-reference, attribution, and language-mode results;
- candidates rejected by the full link, when that evidence prevents repetition.

Do not describe a probe success as a verified migration.

## Agent handoff

Use [the shared launch/resume prompts](agents/LAUNCH.md) and
[handoff template](agents/templates/handoff.md). Preserve the exact candidate,
base, commands, findings and continuation owner. A local source commit is a role
handoff; report the production PR and landing state separately.
