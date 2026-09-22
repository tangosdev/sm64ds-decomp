# PR #2445 composition

This commit composes independently reviewed source
`c462261a1a6924a3d90e127c055dbfc2a4d5df97` with main
`315c46d4b180193d9f607f0a62f7d681ca3f76ac`, preserving both histories.
The accepted class source, header and manifest are unchanged.

The rename-ledger conflict contained ten main records and 29 ov072 records.
Both disjoint additions are retained; a full-record multiset comparison against
both parents and their merge base found no missing or extra records. The two
SetState naming/type disclosures remain. Attribution was checked by a recursive
three-way comparison, and the converted baseline by complete identity sets and
its count, rather than relying on an automatic textual merge.

Fresh RTTI, vtable and TU-map inputs were used to regenerate the promotion queue.
Only this class's measured line count changed, from 642 to 630. The generated
C++ TU summary was refreshed from the composed tree. All earlier handoffs remain
historical evidence at their named commits.

The current source remains partial reconstruction under issue #2476: the accepted
review fixes SNM-01 through SNM-05 and retains the explicit SNM-06 follow-up for
raw state bodies, ABI calls and the matrix adapter. Promotion is not a claim of
recovered original identifiers or complete class reconstruction.

Composition byte, data, attribution and consumer evidence must be recorded for
this commit before publication. Independent review must name this composition
and the actual target base. A previous PR check does not authorize this merge.
