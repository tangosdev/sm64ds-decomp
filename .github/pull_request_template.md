<!--
Matched functions? Add a Provenance line to the PR description or to any commit
message, and CI records HOW the match was made alongside WHO made it.

  Provenance: ai model=grok-4.5 reasoning=high harness=grok-build
  Provenance: human
  Provenance: human hand-traced the regalloc by eye

Without it the function still merges and you still get the credit -- it just lands
with no method recorded, which is the gap that left the provenance ledger empty for
weeks. Agent batches that already name their model in the commit subject are picked
up automatically and need nothing here.
-->

## What this changes

<!-- One line is fine. For matched functions, say which and at what size. -->

## Verification

<!--
For matched functions, the link gate runs automatically. If you checked locally too,
paste the result:

  python tools/match.py --c src/NAME.c --func NAME --addr 0xADDR --size 0xSIZE --trio --brief
  python tools/linkcheck.py --name NAME --addr 0xADDR --size 0xSIZE --module MODULE
-->
