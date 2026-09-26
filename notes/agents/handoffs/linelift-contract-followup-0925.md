# LineLift contract follow-up

This document describes this commit. The queue records the exact output SHA;
checks performed afterward belong in separately published evidence.

## Identity and scope

- Task: `linelift-contract-followup-0925`, producer stage `repair`.
- Session: `codex-linelift-producer-0925` (Codex).
- Source branch: `fix/linelift-contract-0925`.
- Accepted input: `e63828444b7ad3fc20afb58de29857b618dddbcf`.
- Workflow: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Original promotion: [PR #3142](https://github.com/tangosdev/sm64ds-decomp/pull/3142),
  merged as `5be9949b1745dfa760fd40c1786fd888dc4d0fa0`.
- Scope: the local AddVec3 import/call casts in `d_a_linelift2.cpp`, one
  allocation description in `symbols/actor_renames.tsv`, and the three healed
  AddVec3 parameter keys for this TU in the declaration baseline.
- The user's scoped ledger authorization permits this precise baseline removal;
  task `pr-2471-source-review-fixes` retains its branch and reservation.

## Contract and source reasoning

`src/AddVec3.c` defines three `Vec3 *` parameters, reads the first two vectors,
and writes the third. LineLift's old declaration instead described an
output-first Vector3 operation with const second and third parameters.
The local import now uses the definition's Vec3 tag and parameter contract.
The call retains position as both its first and third arguments and the motion
delta as its second argument. Its three casts use the corrected pointer type.
An opaque local Vec3 declaration suffices; no shared header claim is introduced.

The vtable allocation description is `0x358`, matching the class size assertion
and the retail factory's allocation. No symbol address or enrollment changes.
The baseline removes only the three repaired parameter keys and adds none.

## Reconstruction and ownership

This repairs an import contract in the existing promoted TU. It changes no
method conversion, layout, raw-field access, lifecycle body, vtable/RTTI,
initializer ownership or data owner. Existing ABI bridges and shadow layout
remain outside this bounded follow-up. It adds no claim of original names or
additional class reconstruction. The 15-function production TU remains enrolled
in ov091 over `0x02131c14..0x02132404`; vtable data spans
`0x02134eb8..0x02134f40` including its preamble.
All source paths and symbol attribution remain unchanged.

## Verification and next action

The pre-commit proposal independently compiled before/after against the accepted
input: both complete 11,224-byte objects have SHA256
`f50521f2a34dea340a38e911f930338f1da5910c3fd96bfd860ad17c9e9a5710`.
Its whole-tree checker overlay removed exactly three keys with no new finding.
Those are proposal measurements, not final candidate acceptance.

The integrator must record fresh final-candidate full-ROM, relocation,
declaration-agreement, complete-object and relevant static checks in the queue
evidence. No shared header changes or file moves require extra consumer/path
expansion. Private validation and independent source acceptance are pending at
commit creation. Original proposal authors must not verify their own repair.
Before publication, compose with the landed receiver repair and refresh all
affected proof. Integration owner: `codex-linelift-integrator-0925`.
