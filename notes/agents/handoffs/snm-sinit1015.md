# Snowman Breath native static initialization

Producer: `codex-native-inits-three1015`; task `native-inits-three1015`.
Base: `e209dacfaaa95cb5490637ff053d62182689bebe`.

`src/game/actors/d_a_snm_bth.cpp` now defines its scene position as an ordinary
C++ `Vector3(0x27a52b, 0xdf2000, 0x2626bb)`. The compiler emits the 84-byte
initializer and destructor registration at their original ROM homes; the old
handwritten C initializer is retired. The imported vector no longer needs C linkage.
The 17 existing function bodies remain identical to the untouched object.

Owned ov027 ranges (half-open): .text 0x021120c4..0x02112b14;
.init 0x02112f70..0x02112fc4; .ctor 0x02112fd0..0x02112fd4;
.bss 0x02113d04..0x02113d1c (12-byte node and 12-byte vector).
The emitted registration label @826 collides with ov018, so the manifest maps
that local name to the unused @930 at the original node address. The generated
registration import resolves to the original arm9 helper at 0x020731dc.
No object bytes or relocation records are patched.

The vector ends four bytes before the independently configured BSS module end
0x02113d20. The original initializer stores precisely three words at offsets
0/4/8 from 0x02113d10, and the unchanged shared Vector3 has exactly three
32-bit coordinates. An inferred address-only label at 0x02113d1c records that
object boundary, leaving the four alignment bytes ROM-owned. It adds no storage
and does not claim a recovered original identifier.

Early proof: pinned 2004/b56 compiles the untouched and native source; the three
non-text ranges, exact symbol sizes/placement and all five relocations verify.
Full candidate gate: SCRATCH-DATA-VERIFIED; all four owned ranges and all five
nontext relocations pass, all 106 modules reproduce, and the 16 MiB ROM is
identical to the fresh strict stock control (SHA-256
`d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`).
The raw symbol audit retains the same nine existing errors and adds none; it
is not green. Per-function prepush: 17 VERIFIED, initializer BLIND-1 on the
raw compiler registration import, zero blocking findings. The stricter owned
section/full-ROM gate canonicalizes that import and verifies every destination,
so no generated-initializer relocation remains unproved.
Source style, declaration agreement, dead references and 257 port references
pass. Default production verification command:
`python tools/rombuild.py -j16 --no-rom --report-json build/snm-production.json`.
Its terminal result is included in the PR description. Full reports remain
under ignored build/snm-baseline.txt, build/snm-fullrom.txt,
build/snm-prepush.txt and build/snm-production.txt.
No shared header, method body, vtable, RTTI or particle layout is reconstructed
by this slice. Existing compiler-only policies remain checked, and the class's
other interface/layout/lifecycle work remains partial. Attribution stays by
ROM address; the retired shard was not in the CONVERTED baseline.
Independent source acceptance and private CI validation are pending.
