# Pkn integration, 2026-09-10

This staging composition starts from accepted source
`c7e6c4331bd20fecc2ada944a89dd48a331cbc85` and includes the accepted Sanbo tree
`ccac1f85144fe1eed327efcf3f15d5bb80f7a014`. That is a staging base while Sanbo's
private validation runs. Actual main after its merge must be incorporated and
reviewed before PR #2450 is published or merged.

Pkn remains a text-only promotion of 24 functions and 5,764 bytes. Six ordinary
method definitions plus the compiler-generated destructor pair, fifteen
address-named helpers and a manual factory remain. The shared class header,
manifest policy and both ov084 enrollment files retain their accepted content.
The TU comments are shorter; @symbol annotations and ROM ordinals are retained.

Integration review preserves PKN-01 through PKN-06 and corrects three further
provenance findings:

- PKN-07: the 23-function queue census predates promotion and omitted the factory.
  The current row counts one TU; its line count is derived from the current file.
- PKN-08: the field notes now describe actual reads and writes, including the
  Sound::PlaySub byte, selected-player data, particle and sound handles, and the
  additional state-2 condition for the position collider. The word at 0x478
  occupies all four bytes through 0x47b. Earlier scout observations remain intact,
  with a dated correction where they inferred padding from instruction starts.
- PKN-09: the declaration banner no longer asserts a single C-linkage region or
  a language prohibition on block-scope declarations.

Remaining raw interfaces and source reconstruction stay tracked in #2473.
Nothing here establishes original field names or a universal compiler limit.

The global composition is checked as an exact recursive attribution merge and
record multiset merge. All 2,716 staging-base converted identities survive,
including six PiranhaPlant paths mapped to their existing daPkn_c method symbols.
The combined converted set still has 2,716 entries. The attempt, provenance and
rename ledgers contain 2,073, 721 and 3,626 records respectively. Generated queue
and TU-state notes are refreshed from the staged authorities.

Queue evidence records fresh class/module/metadata proof, the full-ROM and
consumer reports, preserved credit, port references and static/aggregate checks
at their exact commits. Staging checks do not replace independent review and
terminal private validation against the final actual main base.
