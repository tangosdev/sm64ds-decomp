# Mip Key current-main composition, PR #2853

This integrates the reviewed producer candidate
`a43d7f5aac4702dca4e4c9b17faa88fed5b1db67` with main
`9022c77b3834c5ee77c31b4fb4af587418137d6c`, preserving the original PR ancestry.
The original source base is `4407c8471ed3299ea16d9cc899020e3232ac6ee9`.
Task `pr2853-composition-0920` is produced by `codex-integrator-0920` and requires
a different session for its verify stage under policy
`18af52b49b9fb5663d417bdfa86109b7b157aaa3`.

## Scoped ledger reconciliation

The user explicitly authorized the PR-specific edits to `attribution.json` and
`config/decl-agreement-baseline.json`. The existing owners keep their reservations;
this is a documented exception for these changes, not a takeover or receipt reuse.
No independently edited change is made to `include/decl_common.h` or `AGENTS.md`.
Changes already present in the main parent are inherited without alteration.

The JSON ledgers are reconciled by key using the original base and both parents.
Conflicting values for the same identity stop reconciliation. All unrelated main
entries and their ordering are preserved. Fifteen Mip Key symbol credit overrides
are added, preserving the original 12 tangosdev, 2 ruspecial and 1 andrewboudreau
credits. Five existing converted identities move from deleted shards to `path#symbol`
identities in the promoted TU; the baseline remains 2,973, with no new gain banked.

Declaration entries follow the PR's retired shards into its live TU. Four reviewed
global identities are corrected to the source's actual interfaces: `data_0209f318`
uses `Camera *`, `data_ov085_021305d8` uses `SharedFilePtr`, and the two state objects
at ov085:0x0213071c/0x0213072c use `MipKeyState`. This records the known disagreements
with generic placeholders elsewhere; it does not assert that every repository
declaration has been reconstructed. The typed interfaces and 16-byte state data
were independently inspected and byte checked before the baseline edits.

Fresh RTTI, vtable and TU-map inputs drive `queue_audit.py` into an ignored build
copy. Only its generated Mip Key row is selected for the tracked queue. That row
now records one shard, 360 source lines and production promotion; its compiler-only
floor remains 11. No other queue row or comment is changed. The full queue audit
still has unrelated stale rows and is not claimed to pass globally.

## Source review and validation boundary

The [producer handoff](pr2853-repair-0920.md) and
[measured source forms](../../experiments/mip-key-2853-source-forms.md) preserve
the source work and its limits. MIPKEY-2853-01/02/03 fix callee contracts, fake
layouts/dispatch and volatile vector shadows. Findings 07/08 remove avoidable angle
and PMF temporaries. Finding 04's unsupported provenance wording is corrected;
seven address-named helpers still need further genuine-method reconstruction.
Finding 05 retains measured matrix-copy and fixed-point-call constraints within
partial reconstruction scope. Finding 06's four ledger identities and composition
are addressed here; independent verification remains the acceptance decision.

Before composition, the independent reviewer reproduced the source-form probes;
all 15 functions passed strict relocation checks with no differing or blind words.
The whole producer object SHA256 is
`0fab978517bf4cdb1fdd83035d0a5fae7fdcdd0f0ef4b7196b8b2e0b54485f8d`.
The integrator independently reproduced the complete 106-module ROM build on the
producer input and verified the final source pair retained that whole object.
Those results are prior-candidate evidence. The composed candidate requires its
own current-main build and an independent exact-commit/base source verdict.

Remaining reconstruction is recorded in [issue #2865](https://github.com/tangosdev/sm64ds-decomp/issues/2865),
posted with explicit user approval and naming `codex-integrator-0920` as next owner.
Independent exact-head/base Source review remains required; this producer handoff
does not assert a verifier pass.

## Independent composition corrections and current-base evidence

Independent reviewer `codex-mip-independent-source-0920` requested removal of
fourteen obsolete declaration fingerprints inherited from the original folded
source. Those bridges have since been corrected or removed. The final baseline
retains exactly seven live Mip TU fingerprints: three existing nominal math-type
seams and the four reviewed globals. No obsolete contract is newly banked under
the promoted path. Unrelated main records retain their original insertion order
and empty maps; each unchanged symbol record is compared verbatim to the main
parent before writing the ledger.

Composition `072db16d67abe7178303f72fff834f6af0873692` passes the complete current-main
ROM build: 106/106 exact modules, 11,208 reproducing functions, zero mismatches,
26 reproducing source-data claims and three BSS claims. TU validation, all fifteen
strict relocated functions, emitted-data policy, header layout, attribution
(15 consolidated, zero changed/lost), references, port references, converted
ratchet and the committed scoped declaration check pass. The ledger-only
successor changes no compiler, header, source, manifest, symbol or enrollment input.

The earlier uncommitted-merge declaration check selected the old ancestry scope
and reported ten Player caller discrepancies. The actual committed composition
selects three files and passes; none of those ten entries is added to the ledger.
The narrower CI promotion-flag audit is checked separately from the known-stale
full queue audit. Independent Source acceptance and its follow-up issue remain
required before merging.

## Reconciliation with the author update

The author rebased the original PR to `b4847306287a03bf9f4bc96bd5801a08f62037e4`
while independent review was in progress. This merge preserves that history and
the previously reviewed `f16e7bfc13dd0b6c14df57badff39b8d9549aebb`. The author
removed the old lineage list, already absent from the repaired source, and linked
the filename pilot to the live TU. The pilot now includes both the live TU and
the immutable historical factory citation, with the producer owner
`codex-repair-mip2853-0920` explicitly acknowledging this narrow integration edit.
All source, header, manifest, enrollment, credit and baseline bytes remain equal
to f16; only this handoff and the pilot citation change. The measured source row
therefore remains one file / 360 lines and the seven live declaration identities
remain valid. No author source contribution is overwritten by a force push.
