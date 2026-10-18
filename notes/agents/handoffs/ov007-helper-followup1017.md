# Handoff: ov007-helper-followup1017

This document describes this commit. The queue records its immutable output SHA.

## Identity and scope

- Task: `ov007-helper-followup1017`; producer session
  `codex-ovlang-follow-producer1017`, Codex harness.
- Branch: `cpp/ov007-helper-followup1017`.
- Source base and workflow pin: `e8dc126bcda1865cce7a4a4cb920fb2095deefb9`.
- Follow-up to the overlay language audit in PR #4019. This branch is independently
  based on main; it does not include that PR's 82 conversions.
- Stages: producer then independent verifier. Publication is authorized; merging
  is not. The queue owns the exact source paths, five ranges below, overlay
  enrollment, two historical path inventories, and this handoff.
- Scratch research was contributed by `/root/audit_007_018` and
  `/root/audit_002_006`; each reviewed the other's candidates. Final verification
  belongs to `/root/audit_019_030`, who did not edit any candidate.

## What changed and why

| Function | Overlay 007 range | Change |
| --- | --- | --- |
| `func_ov007_020c0354` | `[0x020c0354, 0x020c0384)` | Model each eight-byte clearing block as two integer words. |
| `func_ov007_020c0eb0` | `[0x020c0eb0, 0x020c1048)` | Use an integer-valued flip-membership predicate to retain the original comparison sequence in C++. |
| `func_ov007_020c1814` | `[0x020c1814, 0x020c184c)` | Call the existing no-argument reset with its actual void return and restore texture/palette offsets. |
| `func_ov007_020c2f14` | `[0x020c2f14, 0x020c3054)` | Store the three coordinates in a private array-backed record to retain whole-record copies. |
| `func_ov007_020b7594` | `[0x020b7594, 0x020b7658)` | Repair the clearing helper's declaration and its two pointer casts; caller remains C. |

The first four files use C++ mode and keep the configured exports through C
linkage. The fifth is a necessary caller repair. Production enrollment and two
historical inventories follow the new paths; those inventories' historical
classification labels are not claims of the final language. No shared headers,
symbol addresses, declaration baseline, function authors, or data ownership change.

The flip predicate implements the original equality tests for all integer inputs;
it does not reinterpret them as bit tests. Natural C++ short-circuit spelling
emitted 416 bytes and a switch emitted 412, against the ROM's 408. Integer-valued
comparisons retain 408 bytes without assembly, volatile accesses, or new pragmas.

The extrema record has three coordinates in their original order. Array storage
preserves aggregate copying that scalar members changed in C++ mode. The record
is in an unnamed namespace: this is local representation evidence, not a shared
Vector3 declaration or a claim about Nintendo's original type name. Independent
review caught and resolved its initial global-name collision.

The clearing helper's sole caller allocates 0x400 bytes and clears 0x80 eight-byte
blocks. Store order remains second word then first word. Existing caller object
layout and integer-held pointers remain partial reconstruction; this change does
not claim to recover the containing class. Texture/palette offsets follow the
existing reset implementation and downstream texture-loading consumers.

## Reconstruction boundaries

These are ordinary free-function language conversions, not class reconstruction
or translation-unit promotion. Names and filenames remain reconstructed. No new
lifecycle, vtable, RTTI, initializer, or data ownership is claimed. The unresolved
field in `func_ov007_020b797c` and separately reserved ov002 helpers remain outside
this slice. PR #4019's audit is a snapshot of its own scope; this handoff records
these four later improvements without rewriting that earlier snapshot.

## Proof

Pinned compiler: 2004/b56. Producer logs are retained in `build/ovlang/`.

- Explicit `tools/linkcheck.py` for all five functions: 5 VERIFIED, zero byte
  differences and zero blind relocations; 1,028 bytes total (`linked.json`).
- `python tools/rombuild.py -j16 --no-rom`: exit 0, 106/106 modules exact,
  11,334/11,334 source functions reproduce, 338/338 source-owned data claims
  exact, zero new symbol errors (`rombuild.log`). The untouched-base control
  retains its pre-existing symbol diagnostics, and aggregate romdata reporting
  retains one pre-existing difference; neither is a new source-owned mismatch.
- `python tools/check_decl_agreement.py --changed e8dc126bcda1865cce7a4a4cb920fb2095deefb9`:
  exit 0, no new disagreements or local header redeclarations (`declarations.log`).
  No baseline changes are needed.
- `python tools/source_style.py --base e8dc126bcda1865cce7a4a4cb920fb2095deefb9`:
  exit 0, five files, zero findings and zero formatting deferrals (`style.json`).
- `python tools/port_refcheck.py`: exit 0, 247 references resolve (`port.log`).
- `python tools/check_dead_references.py`: exit 0, no new dead references or broken
  markdown links (`deadrefs.log`).
- `python tools/langmode_audit.py --check build/ovlang/langmode-baseline.json`:
  exit 0, ratchet PASS against the fetched chaos-data baseline (`langmode.log`).
- Source-review reports and compiler experiments remain in ignored research
  directories `build/ov002-030-lang1017/barriers/` in the primary checkout and
  `build/lang-types/` in the scratch type-research worktree. Those artifacts are
  not production inputs and do not substitute for final composed verification.
- Independent verification and private CI: pending at producer handoff. Their
  exact candidate/base evidence belongs to the queue and PR, not an amended
  source commit. No shared headers changed; no header consumer expansion needed.
