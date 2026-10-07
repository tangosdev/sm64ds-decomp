# Monte Carlo composition after touch-input naming

This composition preserves the accepted readability candidate
`56a138fc5570d022929fe475b4211f4f86fd176d` and incorporates main
`7145b30ce76a4b6b32087cabc496e4d13dfd3ed8`, which landed the shared
`PlayerInput.h` names in PR #3640. The original producer handoff records the
historical candidate and its original baseline; the evidence below describes
the updated composition.

The two overlapping HitTest hunks retain both improvements: the shared
`gActivePlayerSlot` and `gTouch*` declarations, and the descriptive local names,
stride-four indexing, branch order and touch bounds from our readability pass.
No other source repair is introduced. The final PR remains limited to Monte
Carlo and its handoff notes relative to current main.

The prepared touch-input base `c69a45550075eb926ff0853c2d8f40016e491329`
has exactly the same Git tree as the actual main commit above. Its independently
reviewed preview `d37e728a70ac5473f6203a23a96184a2171702e3` supplies the
resolved source unchanged. Fresh base and preview ELF objects are identical:
21,808 bytes, SHA256
`f57d00be900d1b204c8b0cfa9a59c29df5687b89e47913a4540e64ea414a44f5`.
The new symbol names change the ELF string table, so the original handoff's
21,824-byte object hash is historical and is not the current baseline.

Independent preview checks verified all 25 functions with zero differences or
blind relocations. The preview full-ROM build passed 106/106 modules and
11,273 source-built functions, with zero mismatches. Source-owned claims were
51 data and 11 BSS entries; the global metadata census retained 388 partial
records and one differing record. These are preview facts, not a substitute
for the exact final-commit review and private validation recorded separately
in queue task `mgread-mcarlo-composition-1007` and PR #3642.

Prior findings MCARLO-PROV01 (fixed comments) and MCARLO-RECON01 (deferred
factory/helper/interface work) carry forward. Completion remains partial,
with the minigame reconstruction lane continuing under issue #3171.
