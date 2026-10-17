# Sound native static initialization (2026-10-15)

Producer: `codex-sound-producer1015`. Queue reservation: `sound-sinit1015-pr3886` (continuation of `sound-sinit1015-checkpoint`).
Original input: PR #3868 head `38a444c1bed4838b38d5be53979e185c10ee2602`.
Publication base: main `a8983e9409027873b0461eaf2bab3e191f179864`.
Branch: `cpp/sound-native-sinit-1015`. Worktree: `C:/tmp/sm64ds-sound-sinit1015`.
Supporting tooling: `050484be1a`, PR [#3885](https://github.com/tangosdev/sm64ds-decomp/pull/3885).
The source PR is stacked on that tooling branch. Publication is authorized;
independent source acceptance and private CI validation remain review gates.
No merge is performed or claimed.

## Result

The fifteen PMF dispatch tables are ordinary C++ definitions in
`src/minigames/d_s_mg_sound.cpp`. The compiler generates the original static
initializer from them; the handwritten C initializer `__sinit_ov006_02132970.c` is retired.
Five note handlers, thirteen pop-update handlers, and thirteen three-phase pop
arrays account for all 57 copied records. The owned tables and all 57 descriptor
imports use ordinary C++ linkage. Fifteen duplicate table imports are gone.
The descriptor constants remain ROM-owned ordinary `extern` imports; direct
member-pointer descriptor emission/data ownership remains a separate continuation.

| Section | Half-open ov006 range | Ownership |
| --- | --- | --- |
| .text | 0x02119904..0x0211cbd0 | Existing 82 functions |
| .init | 0x02132970..0x02132f68 | Generated __sinit_d_s_mg_sound.cpp, 1528 bytes |
| .ctor | 0x021335d4..0x021335d8 | Pointer to that initializer |
| .bss | 0x02142cc0..0x02142e88 | Fifteen dispatch tables, 456 bytes |

Enrollment uses the intact object, exact compiler-only RTTI/vtable policy, and
manifest-licensed BSS ordering. No .data ownership is added. The two initializer
symbols are renamed at their original addresses. `function-authors.json` stays
unchanged: original credit is still recorded by address. The conversion baseline
retires only the C initializer identity, with an explicit generated-code reason;
every unrelated banked identity and its existing order are preserved.

## Live interior destination

`data_ov006_02142e02` is ten bytes inside the note table at 0x02142df8. It has a
LIVE relocation from OV007:0x020f7cf0, despite having no source/header reference.
The initial fully owned object passed the OV006 bytes and all owned-section gates,
but lost that label and changed OV007 by four bytes. Deleting the label is invalid:
`dsd delink` then refuses the cross-overlay relocation. It has been retained.

The supporting linker change defines `data_ov006_02142e02 = 0x02142e02;`
inside the generated `.ov006` output section. It first proves that the label is
inside actual emitted NOBITS storage. Both names require unique
configured homes in the same module, exactly one enclosing BSS definition, and a
complete source range covering its whole emitted extent. Ambiguity, missing
storage, out-of-range offsets, and existing definitions are not overridden.
Object bytes, relocation records, compiler instructions and allocated storage are
unchanged. `tools/test_lcf_interior.py`: 18 tests pass, including negative bounds,
ownership, ambiguity, duplicate-definition, output-module and byte-preservation cases.
The real linker experiment is essential: an expression `table + 0xa` retains
only the offset (10), while a top-level constant gets the right value in the
wrong overlay section (OV060). A canonical constant inside `.ov006` retains both
the exact address and OV006 section identity. The direct relink reproduced all
106 modules with zero differing bytes and no new symbol error; the full gate
repeats the proof without manually patched scratch inputs.

## Original PR-head producer evidence

The following measurements describe the original PR #3868 composition. Fresh
publication-base measurements are recorded separately below.

- Compiler actually exercised: pinned `2004/b56` (the worktree canary path is retired;
  the untouched Sound TU itself was compiled to establish executable readiness).
- All 82 existing function bodies and relocation tuples remain identical to the
  exact PR baseline. Moving the 57 imports out of C linkage preserves all 83
  functions and relocations, including the generated initializer.
- Focused strict ROM checks: 83/83 VERIFIED, zero blind relocations, zero differences,
  after the existing production vtable address-point normalization. Initializer
  alone also directly VERIFIED at 0x02132970, size 0x5f8, 72 relocations.
- Exact owned-section audit: .init/.ctor bytes, all 456 BSS bytes, symbol placements,
  and all 73 nontext relocations pass with zero errors.
- ROM-data check: 7 VERIFIED (212 bytes), 21 PARTIAL (68 matching prefix bytes),
  zero DIFFERS. Partial inherited metadata coverage is not whole-class acceptance.
- Declaration agreement, rename ledger, 259 port references, and conversion ratchet
  pass. Only the retired initializer was removed from the conversion baseline.
- Strict demoted baseline control: 106/106 modules exact; nine pre-existing missing
  symbols remain in `dsd check symbols`. Candidate verification compares against
  that explicit baseline and must add no new symbol errors.

Full candidate gate: `SCRATCH-DATA-VERIFIED`. All four licensed ranges
reproduce from one object; 106/106 modules are exact; all 73 owned nontext
relocations pass; nine baseline symbol-audit failures remain with ZERO new errors.
The 16 MiB packaged ROM is identical to the source-independent strict control:
SHA-256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
The raw `dsd check symbols --fail` check is still red on those nine pre-existing
errors and must not be described as green.

Default `rombuild.py -j16 --no-rom` PASS: 106/106 modules exact, 11,334/11,334
source-built functions reproduce, 333/333 owned data claims reproduce, and the
intact-TU module/symbol/storage-alias gates pass. It actually consumes the
consolidated Sound .text/.init/.ctor/.bss entry. The global ROM-data inventory
reports one differing symbol elsewhere: `_ZTV10dCcAcPos_c`, emitted by the
unchanged D0/D1 shards. The untouched PR-head control reproduces the same four
differing bytes for that symbol. Sound's focused inventory has zero DIFFERS and
no metadata definition was changed by this slice.
The exact default-build report is `build/sound-production-final.json`.
Relevant ignored artifacts: `build/sound-fullrom.txt`,
`build/tu/ov006-dScMgSound_c/link/linkcheck.json`, `build/sound-strict-all.json`,
`build/sound-owned-audit.json`, `build/sound-prepush.json`, and baseline reports under
`build/tu/_baseline/link/`. The pristine PR control is preserved separately at
`C:/tmp/sm64ds-sound-control1015`.

## Descriptor continuation

Local experiment snapshots are in ignored `build/sound-baseline.cpp/.o`,
`build/sound-probe1.cpp/.o`, `build/sound-probe2.cpp/.o`, and probe3 diagnostics.

- Direct `&dScMgSound_c::Method` table entries preserve initializer instruction
  bytes/size and all original function bodies, but emit 57 anonymous eight-byte
  descriptors (`@1307`..`@1416`, not dense). Generic linkcheck resolves several
  names to other TUs and leaves 47 unresolved. Completing their canonical symbol
  mapping and adjacent .data ownership remains unverified; this is not a claim
  that native member pointers cannot match. Three no-argument Finish handlers
  currently require an explicit PMF signature conversion in this probe.
- Named `extern const Pmf descriptor = &Class::Method` definitions add runtime
  descriptor initialization: __sinit becomes 0xe14 (3604 bytes), 129 relocations.
  They are not substituted into production.
- Externalizing ordinary BSS table references via compiler-only data policy is
  deliberately refused by objisolate. That guard has not been weakened; the final
  candidate owns the actual tables and retains the live interior label.

The whole class still has pre-existing raw field/lifecycle/interface work. This
slice improves initializer/table ownership and linkage; it is not a declaration
that Sound reconstruction or independent source review is complete.

Publication C++ source SHA-256: `41ede8e5ff1a660c64604cd1aeba2e8209da14e14f76c42b6a6f11c4ed547a3f`.
The current-main pinned formatter changed only spacing/line wrapping in two table
initializers; their element order and initializer expressions remain unchanged.

## Coordination checkpoint

The original reservation `sound-sinit1015` incorrectly listed the retired C path
as a produced file. Its history and work are preserved; its coordinator replaces
that registration with `sound-sinit1015-checkpoint`, whose outputs name only the
surviving source/config/handoff/tooling files. The deletion remains an explicit
resource/change rather than an impossible output-file requirement. The new task
records this exact local source commit and the retained worktree for adoption;
it must not restart reconstruction from scratch.

The original tested candidate remains preserved on `cpp/sound-sinit1015`. The
publication branch composes the two scoped commits onto current main, with fresh
ROM/relocation proof below. Offer this exact publication commit for independent
review; neither the producer checks nor the PR itself claim independent acceptance.
Private receipts remain ignored under `build/`.

## Current-main publication proof

Tested base: `a8983e9409027873b0461eaf2bab3e191f179864`, plus tooling
`050484be1a5a622c72bd06474bd56b45cdf68849`. This commit contains the final
formatted source and enrollment.

- Linker unit tests: 18/18 pass.
- Changed-line source style: zero findings, zero formatting deferrals.
- Declaration agreement, conversion ratchet, rename ledger and 257 port references pass.
- Relocation-aware prepush: 83/83 VERIFIED, zero warnings or blocking findings.
- Fresh strict stock control and candidate: all 106 modules exact. Full 16 MiB
  ROM identical, SHA-256
  `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
- Candidate `SCRATCH-DATA-VERIFIED`: all four owned section ranges and all 73
  nontext relocations pass. The same nine baseline symbol-audit errors remain,
  with zero new errors; the raw symbol check is not green.
- Default `rombuild.py -j16 --no-rom` passes: 106/106 modules exact, 11,334/11,334 source functions reproduce, 333/333 owned data claims reproduce, and 240 BSS claims/30,620 NOBITS bytes pass object/symbol gates.

Reports retained in ignored `build/sound-publication-baseline.txt`,
`build/sound-publication-fullrom.txt`, `build/sound-publication-prepush.txt`,
`build/sound-publication-production.txt` and
`build/sound-publication-production.json`. Independent source acceptance and
private CI validation remain pending on the publication commit.

Publication: source PR [#3886](https://github.com/tangosdev/sm64ds-decomp/pull/3886),
stacked on tooling PR [#3885](https://github.com/tangosdev/sm64ds-decomp/pull/3885).
The documentation-only successor corrects the historical retired-file reference
flagged by the dead-reference gate. Source/config/tooling blobs remain identical
to tested commit `0343ee5e92bfd66a5c7f3fbe6de0103228c4b2b1`; the continuation
retains that offered checkpoint and its predecessor history for independent review.
