# Unused data declaration repair, 2026-09-10

Task `data-interface-repair-0910` continues from main
`df70231d3cf547597b2cb36bed8b1494e0753dbd`. It removes two unused declarations
from `include/decl_common.h` whose types disagree with their existing definitions.
No caller needs these exports: a complete tracked reference search finds uses
only in the two owning translation units, neither of which includes that header.
The definitions and their current type locations remain unchanged.

- `data_ov045_02112fdc`: the local `GuraResourceDescriptor` in
  `src/game/actors/d_a_obj_km2_gura.cpp` contains three pointers. Its 12 ROM bytes
  resolve to ov045 addresses `0x02113220`, `0x02113228`, and `0x021124f0`.
  All three data relocations explicitly name overlay 45. The initialization and
  cleanup helpers consume the first two entries as file handles and the third
  as collision configuration. The obsolete common declaration was `int[]`.
- `data_ov100_02148390`: the table in `src/game/actors/d_a_star_gate.cpp` holds
  four `daStarGateInfo` records, each six bytes: two signed bytes followed by two
  signed halfwords. The ROM rows are `(12,5,28,37)`, `(30,6,29,38)`,
  `(50,7,30,39)`, and `(80,8,31,36)`. The existing type and size assertion in
  `include/daStarGate_c.h` agree with the indexing and field accesses.
  The obsolete common declaration was `signed char[]`.

These measurements support the existing layouts and uses; they do not recover
unique original type or field spellings. No shared type was introduced.

Both complete data records independently verify against their own overlays,
with zero blind words. The two owners' 25 functions also verify: 2,640 text bytes
and 109 permitted-module references. All 891 transitive consumers of
`decl_common.h` compile before and after the deletion under their configured
compiler versions and build flags; every complete object is byte-for-byte equal.

For a stable diagnostic comparison, the repaired declaration checker is loaded
from immutable commit `302437407099df4d95872c492b83f8505fd19246` without changing
the source branch's tools or baseline. Its complete finding set drops from
22,964 to 22,962: exactly these two disagreements disappear, with no other change.
This is a scoped diagnostic result, not a claim that the repository has no
remaining declaration disagreements. The checker is undergoing separate review.

Exact object hashes, reference lists, data and function reports, and final
consumer link-check results are retained in the queue producer evidence under
`C:/tmp/sm64ds-decldata-r2445-0910/build`. Independent source acceptance and
terminal PR validation remain required before landing.
