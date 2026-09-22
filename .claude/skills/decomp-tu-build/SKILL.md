---
name: decomp-tu-build
description: Consolidate an evidenced SM64DS translation unit, prove its compiler output, and promote it into production src/ with absorbed sources retired. Use for TU promotions and combined-compilation, relocation or output-ownership failures. Temporary src_tu/ experiments are intermediate work; use decomp-tu-slicing for boundary evidence.
---

# Build and promote a translation unit

Read [the canonical promotion workflow](../../../notes/tu-promotion-conventions.md)
for the production result, command sequence, justified staging exceptions and
review gates. Coordination and launch prompts live in
[PIPELINE](../../../notes/agents/PIPELINE.md) and
[LAUNCH](../../../notes/agents/LAUNCH.md). This skill supplies compiler techniques;
it is not a second workflow that ends at a shadow commit.

Resume an existing candidate or production owner first. An already promoted TU is
edited in `src/` in the assigned worktree. For a new consolidation, the helper may
stage a candidate in `src_tu/`; continue with `tu_promote.py` after proof, in the
same task. Do not add a folder move, a separate backport PR, or a shadow-only PR by
habit. A successful dry-run plans ownership changes; it does not establish bytes,
source quality, default-build coverage or merge acceptance.

## Boundary inputs

When generating or refreshing a map, run the prerequisite chain:

```sh
python tools/rtti_extract.py
python tools/rtti_vtables.py
python tools/tu_map.py
```

Current `tu_map.py` refuses missing or stale vtable input. An existing map can
still outlive the inputs it describes; verify provenance when adopting it.
`--blind` is a negative control, not a normal way to select a TU. See
[decomp-tu-slicing](../decomp-tu-slicing/SKILL.md) for boundary uncertainty.

## Build the candidate without changing its meaning

Use `tubuild.py inspect`, `create` only for a new candidate, then `compile` and
`verify` while iterating. Follow the canonical workflow for link mode, production
promotion and final gates. `tubuild.py promote` is an older dry-run planner;
**`tu_promote.py` is the production mutation tool**.

If `create` refuses an `extern "C" { ... }` legacy body because its splitter cannot
find the definition, curate the candidate and use `tubuild.build_manifest_entry`
for the manifest schema. A generator limitation is not a reason to change the
function's linkage or invent a second source owner.

Inspect the manifest's complete declared scope and every emitted function/data
section. A diagnostic subset can locate a mismatch; it cannot be presented as
promotion of the full TU or used to reduce existing production coverage.

## Function order and compiler hazards

- The pinned compiler commonly emits distinct function sections in reverse source
  order. Measure the candidate; destructor variant groups have their own order.
  Read order from ELF section indices, not `st_value`, which is normally zero in
  these per-function sections.
- `#pragma opt_propagation` and `optimize_for_size` can apply file-wide. A pragma
  that repairs one member may change the others. Compare the whole declared TU
  after each change; do not discard a matched member to improve the count.
- Combining previously separate declarations exposes real type conflicts. Preserve
  necessary declaration order, remove duplicates, and reconcile against definitions,
  call sites and ROM evidence. The longest or most detailed declaration need not
  be correct. Shared-header changes require all affected consumers to be proved.
- In a disposable diagnostic probe, C linkage can expose declarations that C++
  interpreted as overloads. Do not retain blanket `extern "C"` or hand-mangled
  definitions as a substitute for genuine C++ methods. Actual C ABI helpers remain
  valid; [class-form observations](../decomp-cpp-class-form/SKILL.md) explain the
  source forms to test.
- `999 word(s) differ` can indicate a size difference, including destructor variant
  emission. Inspect the actual object before treating it as a type mismatch.
- An inherited `decl_common.h` declaration can conflict with the TU's own symbol.
  Repair the declaration/ownership using evidence and prove affected consumers;
  do not assume a shared declaration is correct merely because it already exists.

## Inspect the whole verification result

A masked `match.py` comparison alone can accept a call to the wrong destination.
`tubuild.py verify` must establish all three dimensions:

```text
byte comparison   : 7/7 MATCH
objisolate check  : clean
Result: 7/7 MATCH, objisolate clean, reloc-destinations clean -> TEXT-VERIFIED
```

A destination failure appears on the symbol's row and degrades the verdict.
Missing proof is not a pass. Even TEXT-VERIFIED does not prove non-text ownership,
whole-link behavior, or production enrollment; continue the canonical gates.

For data/lifecycle output, use the relevant
[partitioned experiment reference](../../../notes/agent-partitioned-tu-workflow.md).
Do not infer data ownership solely because code loads an address: preserve ordered,
disjoint contributions and prove vtable/RTTI/type-name relationships. Inspect the
unisolated object so discarded metadata does not escape review.

## Protect the worktree and the handoff

Use a separate [wired worktree](../../../notes/worktree-inputs.md), with private
`build/` state; only the content-keyed object cache may be shared. Use the safe
worktree removal helper for linked inputs.

If production is blocked, report the specific symbol/range, failed gate or tool
policy, pinned experiment and next action/owner. Otherwise hand off the verified
production candidate to the independent verifier and integrator. A shadow status
or a folder of per-function files does not complete this task.
