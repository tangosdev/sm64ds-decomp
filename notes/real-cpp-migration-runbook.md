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
python tools/port_refcheck.py
python tools/check_dead_references.py
```

After committing the source rename, verify contributor lineage:

```powershell
python tools/prepush_attribution.py --base origin/main
```

A commit may rewrite a file or move it, but should not do both when the path stem
also changes. Split such work so Git records an unambiguous rename and original
credit survives.

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
