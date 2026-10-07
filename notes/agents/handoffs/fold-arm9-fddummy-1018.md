# fold/arm9-fddummy-1018 — arm9 dFdDummy_c shard fold

> **Superseded.** The family-wide fader fold (`src/engine/fader/dFader_c.cpp`,
> manifest `arm9/dFader_c`) absorbed this standalone TU and its range; the
> constructor at `0x02017278` is now `_ZN10dFdDummy_cC1Ev`. Kept as the record
> of the earlier step; the file and manifest named below no longer exist.

## What

- Class: `dFdDummy_c` (`include/dFdDummy_c.h`) — the no-op FaderColor
  variant, only ever embedded (dScDSMT_c member at +0x54).
- Range: `0x020171c8..0x02017278` (arm9), contiguous and exhaustive: 5
  members, bounded below by a ModelAnim2 thunk and above by the
  embedded-construction helper `func_02017278`, which keeps its own shard.
- Staged under `src_tu/`, promoted to a standalone `dFdDummy_c.cpp` TU (since superseded, see below)
  via `tools/tu_promote.py` (5 attribution overrides, 4 CONVERTED identities).
- 5 legacy shards `git rm`'d.

## Verification

- `tools/tubuild.py verify arm9/dFdDummy_c` — **5/5 MATCH**, objisolate
  clean, reloc destinations clean, emission order ROM-ascending.
  TEXT-VERIFIED.

## The one non-obvious thing

The class's own cartridge RTTI spells `dFdDummy_c` (matching the project
name), but the BASE chain it drags in does not: the emitted
`_ZTS5Fader`/`_ZTI5Fader`/`_ZTI10FaderColor`/`_ZTI15FaderBrightness` records
are homeless next to the cartridge's `dFdColor_c`/`dFdBrightness_c` rows.
First merge attempt left RTTI on → 6 unlicensed extras, promotion refused.
Fix is the dExt* lever: `#pragma RTTI off` — no records emit, the vtable
preamble's typeinfo word deadstrips with the rest of the data sections.

## Shape

Source order is back-to-front (D1, AdvanceFade, SetBackwardTime,
SetForwardTime) under default deferred codegen — no `defer_codegen off`
needed since nothing carries unscoped pragmas. One `~dFdDummy_c()` emits the
D2,D0,D1 group + the vtable; D2 has no cartridge home (deadstrip), and the
class has no C1/C2 anywhere — it is only ever embedded.

`AdvanceFade` keeps its `decl_Fader.h` mangled call to
`_ZN5Fader13AdvanceInterpEv` — the base body is still a free function.

## Bookkeeping

- `config/arm9/delinks.txt`: 5 shard blocks → one `complete` claim
  `.text 0x020171c8..0x02017278`.
- the standalone `arm9/dFdDummy_c` manifest: `promoted`, 5/5,
  `compiler_only_output` records the D2 deadstrip + the
  `_ZTV10dFdDummy_c` deadstrip-data at `0x0208ea6c`.
- `attribution.json`: five `dFdDummy_c.cpp#symbol` overrides preserve each
  shard's author.
