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

Begin from current `origin/main`, never from an old migration branch. Protect a
dirty primary checkout by using a wired worktree:

```powershell
git fetch origin main
C:\Users\andre\.codex\skills\decomp-worktree\wt-setup.ps1 `
  -Name realcpp-<slice> `
  -Branch cpp/real-cpp-<slice> `
  -Base origin/main
```

Read `AGENTS.md`, `.claude/skills/decomp-cpp-class-form/SKILL.md`,
`notes/dtor-migration.md`, and the current class claims in `CLAIMS.md`. Claim a
class or coherent class chain before changing it. Do not overlap an active
claim.

Check the current frontier rather than trusting an old list:

```powershell
python tools/langmode_audit.py --by-class
python tools/langmode_audit.py --list c-mangled
python tools/langmode_audit.py --list cpp-handspelled
```

Use `python tools/srcpath.py <symbol>` for every existing source path. Do not
invent `src/` paths.

### Parallel agent protocol

Parallelize by class ownership, not by individual function or pipeline stage.
Each migration agent owns one non-overlapping class or dependency chain from
header evidence through final verification. Two agents must not concurrently
edit the same class layout, base class, destructor graph, shared header, delink
range, or production TU.

On a machine with the local Redis coordination service, use the installed
`agent-lock` skill before fan-out. The orchestrator, not the worker, acquires the
complete file and half-open ROM-range lock set before dispatch. Give every worker
a distinct `AGENTLOCK_HOLDER`, renew work that approaches its TTL, and release on
every success and error path. A conflict means assign different work or wait;
do not launch an agent and let it discover the overlap after editing.

Redis locks are deliberately short-lived and machine-local. They prevent two
local LLM sessions from editing the same files or address range, but do not
replace repository claims or Git history as durable coordination. Follow the
installed skill for service startup, acquire/check/list/renew/release commands,
and the read-only local lock dashboard.

A practical four-agent team is:

| Role | Owns | Must not own |
| --- | --- | --- |
| Coordinator/integrator | current-main refresh, class claims, dependency map, branch review, merge order | speculative source rewrites on worker branches |
| Migration worker A | one unclaimed class/chain and all affected consumers | any class/header assigned to another worker |
| Migration worker B | a disjoint class/chain and all affected consumers | shared bases that worker A is changing |
| TU reconstruction worker | already-genuine C++ classes, `src_tu/`, TU manifests, combined-TU evidence | production promotion before `verify`, `partial`, and `linkcheck` pass |

With a fifth agent, make it verification-only. It independently reruns strict
relocations, affected-header consumers, full ROM link, attribution, port, dead
references, language-mode, and TU-state gates against each proposed branch.

The coordinator gives every worker a written packet:

```text
claim: <class or dependency chain>
base commit: <origin/main commit>
owned headers: <paths>
owned sources/symbols: <paths and addresses>
known bases/members: <dependencies>
excluded active claims: <classes/chains>
required gates: <commands>
deliverable: <small source-only PR or shadow-TU evidence PR>
```

Every worker uses its own short-path wired worktree and branch. Workers regularly
report discovered header dependencies to the coordinator; the coordinator
serializes dependent branches and rebases/re-verifies the downstream one after
the prerequisite merges. Unrelated leaf classes can proceed together. Shared
base-class, member-lifetime, and production-TU changes must form an explicit
merge train.

Do not split one class into a "header agent" and a "method agent." That handoff
loses ABI context and lets both branches appear green against incompatible
layouts. Do not let multiple workers append speculative fixes to one large PR.
Publish small, independently green slices so a failed class does not block the
others.

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
  --func <symbol> --addr 0x<addr> --size 0x<size> --module <module>
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

## 7. Consolidate proven methods into class translation units

The destination is not permanently one source file per symbol. Once a class has
several genuine C++ methods, reconstructing their original translation unit is
the next step. One class per file is the default hypothesis, but the ROM may
show that a class was split across files or shared a file with helpers or nearby
classes. Follow compiler and link evidence, not aesthetics.

Do this in two stages:

1. migrate methods to compiler-spelled class form while each remains
   independently byte- and relocation-verifiable;
2. combine only a compatibility-proven ordered group into one production TU.

A combined TU can change more than the individual function bodies:

- function and literal-pool order;
- inline and out-of-line helper emission;
- constructor/destructor variant emission;
- vtable, RTTI, and COMDAT ownership;
- static initialization order;
- relocation addends and section layout.

The repository's production surfaces support multi-function sources, but prove
that readiness from the current checkout before each promotion:

```powershell
python tools/cpp_tu_compat.py --require-ready
python tools/cpp_tu_state.py --check-note
```

Use the shadow-TU workflow to reconstruct and test the boundary without changing
production enrollment:

```powershell
python tools/tubuild.py list
python tools/tubuild.py inspect <module>/<candidate>
python tools/tubuild.py create <module>/<candidate>
python tools/tubuild.py compile <module>/<candidate>
python tools/tubuild.py verify <module>/<candidate>
python tools/tubuild.py partial <module>/<candidate>
python tools/tubuild.py linkcheck <module>/<candidate>
```

`verify` proves licensed text and relocations. `partial` proves that one TU
compile can yield the same per-function contributions as the current production
objects. `linkcheck` is the whole-range/module proof. None of them alone enrolls
the TU, and `tubuild.py promote` is currently dry-run only; production promotion
must deliberately update the tracked source, TU manifest, and delink ownership.

When promoting, preserve the ROM-supported method order and license every
non-text section the TU owns. Re-run all per-function strict relocation checks,
the full linked ROM gate, attribution, `port_refcheck`, language-mode audit, and
`cpp_tu_state.py --write-note`/`--check-note`. Reject a consolidation that makes
the final link less exact even when every method matched in isolation.

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

## Agent handoff prompt

Copy this prompt to another AI agent:

> Continue the SM64DS real-C++ migration from the latest `origin/main`. Read
> `AGENTS.md`, `.claude/skills/decomp-cpp-class-form/SKILL.md`, and
> `notes/real-cpp-migration-runbook.md` completely. Work in a wired short-path
> worktree and claim a currently unclaimed class or coherent class chain. A real
> migration must define `Class::Method`, a constructor, a destructor, or a real
> namespace entity and let mwccarm generate the mangled symbol. Do not submit a
> hand-spelled `_ZN...` definition or an `extern "C"` wrapper as C++.
>
> Proceed in small independently verified slices. Map base/member lifetime,
> delete paths, vtable/RTTI ownership, key functions, and manual lifecycle
> consumers before destructor or header work. For every candidate require exact
> mwccarm 2004/b56 bytes, `linkcheck`/`prepush_linkcheck` `VERIFIED` relocation
> identity, regenerated enrollment, a full `rombuild.py -j16 --no-rom` result of
> zero mismatches and 106/106 exact modules, plus `port_refcheck`, dead-reference,
> attribution, affected-header, and language-mode gates as applicable. Restore
> any failure exactly and leave compiler walls unmigrated. Put documentation and
> tooling changes in a separate PR from source migrations. After a class has
> several genuine methods,
> use the shadow-TU workflow to prove their original order and ownership, then
> consolidate only compatibility-proven groups into a production class TU.
> Report concrete evidence, not optimism.

When finished with a disposable wired worktree, remove it only with the skill's
safe teardown command:

```powershell
C:\Users\andre\.codex\skills\decomp-worktree\wt-remove.ps1 `
  -Path C:\tmp\sm64ds-realcpp-<slice>
```
