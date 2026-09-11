# Clock source continuation — 2026-09-11

This continues PR #2490 at `2f166831be8a87760023eb45851fbd36f5b96560`,
source base `224e660ea0219d0999c58483eb2dd9b38991f5ef`, and the accepted
`issue-2481-clock-source-review` task. The historical handoff remains unchanged.
Task `pr2490-source-review-0910` reserves only the class TU, its header and
manifest, this handoff, and the existing ov013 text/data ranges. The producer
is `codex-r2445-0910`; independent acceptance and final main composition remain
separate requirements. The queue records this commit's immutable identity.

Both factories now return `(int *)new daObjClock_c`. The compiler supplies the
existing base and Model construction and class vptr installation. The class-local
`operator new(unsigned long)` forwards to the existing fBase allocator's unsigned
int ABI. Plain new without that bridge retains a 56-byte factory but refers to
unresolved global `_Znwm` (BLIND-1); declaring the operator with unsigned int is
rejected by mwccarm. With the bridge, both factories and the entire object are
identical to the accepted input. The obsolete explicit constructor declarations
and manual stores are removed.

The profile priority fields now use u16, matching the actual fBase constructor
and its unsigned setters. Fresh proof verifies the 352-byte constructor at
arm9:0x02043dec. Its instructions at 0x02043eac and 0x02043ebc use `ldrh` for
profile offsets +4 and +6. This models the observed consumer contract without
claiming an original typedef. Field order, storage widths and initializer values
are unchanged.

Source comments now state the current behavior and relevant matching constraint
concisely. The header no longer asserts that unobserved byte ranges are padding,
that no class can derive from Clock, or that an inline destructor must be declared
first to choose the key function. The retained inline form emits D1 then D0.
The measured out-of-line probe reproduces all nine individual bodies but emits
an additional 48-byte D2 and orders D2/D0/D1, violating the current TU's licensing
and ROM order. This is a bounded experiment, not a proof about every possible
original source form. All symbol markers and ROM ordinals are retained.

## Findings carried forward

- **PR2444-02 — fixed:** the transform helper retains the accepted typed receiver,
  owned Model matrix and inherited angle/position fields. Its C linkage, external
  char-pointer boundary and address-derived symbol remain unchanged.
- **PR2444-04 — fixed:** the two factory/profile names remain reconstructed project
  spellings. RTTI supplies the class identity; debug strings supply CLOCK_LONG
  and CLOCK_SHORT. Those observations do not recover complete original names.
- **PR2444-05 — fixed:** isLongHand still tests actorID 0x125. Hand index zero is
  long and one is short; condition, local type and assignments are unchanged.
- **PR2444-06 — fixed:** the four ordinary overrides remain virtual through their
  inherited signatures. InitResources is the first declared non-inline virtual;
  the class TU supplies its definition and the vtable/RTTI group.
- **CLOCK-PROFILE-01 — fixed:** +4/+6 still name behavior/render priority roles.
  The earlier retained-s16 observation is historical; CLOCK-PROFILE-02 records
  this continuation's measured unsigned consumer correction.
- **CLOCK-PARTIAL-01 — deferred, narrowed:** both manual factories are now repaired.
  The external transform boundary, file-table casts, larger external interfaces,
  and original identifiers/types remain partial reconstruction under
  [issue #2481](https://github.com/tangosdev/sm64ds-decomp/issues/2481).
  Humanizer/integrator follow-through through @andrewboudreau owns that work.
- **CLOCK-FACTORY-02 — fixed:** both native new expressions preserve the complete
  object and correct actor allocator target; no constructor/vptr bridge remains.
- **CLOCK-PROFILE-02 — fixed:** unsigned priority halfwords agree with the actual
  zero-extending consumer and unsigned setters, preserving all owned data.
- **CLOCK-PROVENANCE-02 — fixed:** unknown bytes and destructor/key-function
  behavior are described with bounded evidence; duplicate recovered annotations,
  emphatic historical banners and unsupported universal claims are removed.

## Exact local proof and limits

Fresh immutable-input and candidate builds use mwccarm `2004/b56`. Both complete
objects have SHA-256
`85704d1f9f3915f3c35d894d38f32b91e1d2079ae9572c5e4a4e64be453e0129`.
All nine functions reproduce 752 bytes with zero blind words/differences and
28 text references resolving to the expected modules. All 216 owned data bytes
at ov013:0x021121a4..0x0211227c and all 37 data relocation destinations are exact.
Intact object licensing, six inherited compiler-only metadata dispositions,
nine isolation plans and ROM-ascending text order pass. Raw metadata retains
seven VERIFIED and four PARTIAL records; the partial records are not promoted
to verified by the complete owned-data result.

Only `src/actors/daObjClock_c.cpp` consumes the changed class header. The landed
header parser checks four annotated fields through size 0x128 with no mismatch
or unparsed fields. All 423 port references resolve. Source/enrollment ranges,
profile registry, attribution, shared headers and older handoffs are unchanged.

Reproducible local scripts and detailed evidence are in the producer worktree's
ignored build directory: `prove-clock.py`, `probe-clock.py`,
`prove-clock-boundaries.py`, `baseline/`, `candidate/`, and `clock-probes/`.
The valid header alternatives pass their scratch directory explicitly through
`compile_c(..., include_dirs=[probe_dir])`. An earlier probe accidentally selected
the repository header and is preserved separately as
`preliminary-wrong-header-report.json`; it is not evidence for a header change.
Full production-ROM, credit and prescribed pre-push outcomes are attached to the
exact queue candidate. Historical promotion reports in the manifest retain their
original identities; no new result is written over that history. Final actual-main
source acceptance and terminal private validation are still required to merge.
