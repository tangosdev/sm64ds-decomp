# Cup tables current-main composition

This composition continues PR #4042 and queue task `data-cup-tables1017`.
Producer session: `codex-gate4042-compose1018`; composition task:
`gate4042-compose1018`. The independent verifier must be a different session.

Original accepted source head: `c702d3f074e4dfc97425d688276cffbd753cd769`.
Merged main: `0e478c3cef361afce66be76965ed0a3a9895b10c`.
Both commits remain ancestors of this merge commit; no rebase or force push.

The merge completed automatically without conflicts. Relative to main, the
source/config contribution is still the original Cup table slice: the unchanged
Cup source and manifest, and its one 96-byte data claim at
`ov006:0x0213c094..0x0213c0f4`. The newly landed Memory2 data claim and all other
main changes are retained. Previously inherited tooling is already on main and
is absent from the resulting PR diff. No source correction is claimed here.

All prior source findings and partial-reconstruction scope remain in the original
queue evidence. The three inherited historical provenance findings
`MG0930-PROV01`, `MG0930-PROV02`, and `SCMG2877-01` were already corrected in the
original input and candidate; this composition introduces no repair for them.
The Smartball destructor comment is limited to the pinned compiler, its collision
helper is a native member, and Cup Render uses `mFrame[drawIndex]` directly.

Compiler/relocation checks and full production validation for this composition
are recorded separately in the queue handoff evidence. This document alone does
not establish new byte proof or independent source acceptance. The independent
verifier should inspect the merged main relationship and retained Cup/Memory2
claims, preserve every inherited finding, and record the exact composition and
review base. Root remains the sole merger; server validation must complete on
the new PR head before landing.
