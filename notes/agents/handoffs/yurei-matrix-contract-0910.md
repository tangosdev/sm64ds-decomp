# Yurei matrix helper contract correction — 2026-09-11

`YUREI-MATRIX-CONTRACT-01` corrects the declaration used by
`func_ov065_0211696c` in `src/actors/daYurei_Mucho_c.cpp`. The previous
`(void *dst, const void *a, const void *b)` declaration put the writable output
in the wrong position. The actual definition in `src/MulMat4x3Mat4x3.c` takes
`(const int *a, const int *b, int *dst)` and writes twelve words through its
third argument. The caller now declares that exact contract and explicitly
casts its first argument, a byte-addressed model matrix, to `const int *`.

The argument values and order were already correct. At ov065:0x02116a14,
inside the 380-byte function at 0x0211696c, the call reaches arm9:0x02052914.
The first argument is the matrix at `*(char **)(self + 0x314) + 0xc0`;
arguments two and three both point to `data_020a0e68`. The 544-byte helper
detects that output aliases its second input, computes into a temporary, then
copies back. This fixes a false source contract; it does not claim that the
previous ROM-exact call corrupted data.

Against input/main `e85cec84a6b331d5fac3382c681f060a79cfa1bb`, fresh
mwccarm `2004/b56` builds produce identical complete objects. All 23 licensed
functions (4,280 bytes) are VERIFIED with zero blind words or byte differences;
all 156 text references resolve to their configured modules. The actual shared
helper independently reproduces all 544 bytes. Existing emitted metadata remains
6 VERIFIED and 5 PARTIAL; intact TU ownership and compiler-only policies pass.
The five partial records are not counted as verified.

Producer scripts and detailed reports are under
`C:/tmp/sm64ds-yurei-matrix-r2398-0910/build/`: `prove-yurei-matrix.py`,
`prove-full-tu.py`, and `yurei-matrix-contract/{report,full-tu-proof}.json`.
No baseline, shared helper, header, manifest, enrollment or attribution entry
changes. This follow-up does not supersede the historical source findings and
partial reconstruction recorded in `daYurei_Mucho_c-ov065.md`; issue #2478
remains open for that broader work. Independent source review and the actual
main/private validation gates remain required before this correction merges.
