# Credits

This decompilation builds on reverse-engineering knowledge from the SM64DS community.

## Symbol names & struct layouts
- **SplattyDS / DynamicAllocationDecomp** (https://github.com/SplattyDS/DynamicAllocationDecomp)
 , source of verified function/data symbol names (`symbols.x`) and struct/actor-system
  header knowledge for the EU build. We import their **names and field offsets** (facts /
  interface) into our symbol database and notes; we do **not** copy their source code -
  all matching C in `src/` is written from scratch against our own ROM. Their addresses
  were independently verified against our `dsd` analysis (≈72% land exactly on functions
  we found, confirming the same EU build).
- **SplattyDS / SM64DS-ASM-Reference** and **Arisotura / SM64DSe**, upstream symbol /
  level-format documentation the above builds on.

If you contribute, keep this separation: import knowledge, write code.

## Tooling contributions
- **webhead2oo9** (https://github.com/webhead2oo9) - made the relocation symbol resolver
  module-aware: it now resolves by `(module, address)` instead of address alone, so a
  reloc is no longer mis-resolved to a symbol from a different overlay where overlay
  address ranges overlap. Also added ITCM/DTCM symbol support.
