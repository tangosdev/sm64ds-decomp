# Minigame-base readability: first pass

This handoff describes this commit; the queue records the immutable output SHA.

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/2912
- Task: `minigame-base-readable-0922`, reconstruct stage.
- Producer: `codex-minigame-base-readable-0922` (Codex).
- Branch/worktree: `cleanup/minigame-base-readable-0922`,
  `C:/tmp/sm64ds-mg-baseread0922`.
- Original source base and workflow: `0a609664d09c0377f8680d464a84fd0e6e75e0f4`.
- Composed main: `b93e467b9b7b6a75a32a547fcd1158399a3525dd`.
- Scope: production `src/minigames/d_s_mg_base.cpp`, eight functions in
  ov004 `[0x020b04e8,0x020b0a38)` (1,360 bytes), plus this handoff.
- Status: local candidate; independent exact-commit verification remains required.

## Changes and evidence

Replace the fabricated `Self`, `VSlot32` and `ObjV34` interfaces with the virtual
methods already declared by `dScMgBase_c.h`: slots 26, 27, 28, 31, 32 and 33.
Calls remain unqualified virtual dispatch so derived scenes keep their overrides.
The borrowed actor-style method names do not establish scene event meanings.

Use the existing menu, touch-options, state-controller and counter fields instead
of reconstructing addresses from `this`. The frame counter increments directly
and still wraps at 40. Unknown fields retain their existing provisional names;
this pass adds no layout assertion or shared declaration.

Call `ApproachLinear(int&, int, int)` with its actual int return contract and use
`Sound.h` for the native `StopLoadedMusic_Layer1` call. Remove obsolete declaration
and source-generation prose. Keep all eight symbol markers and definition order.

The unrelated stylus owner remains an explicitly local six-slot `SceneVCall6`
view, structurally matching the view in `dScMiniGm_c.cpp`. The historical
local type spelling is retained: renaming it created a new declaration
fingerprint without resolving the inconsistent global types across consumers. Its slot-5 predicate
is observed at input gates; its full class and inheritance are unresolved. The
previous assertion that it was another `dScene_c` was unsupported: that scene
slot has a different contract. No new original class name is claimed.

## Local proof and limits

The baseline was compiled before editing at the original source base above. Candidate
and baseline complete raw ELF objects are identical: 5,392 bytes, SHA256
`3f747958d3caa9f78b9ef7390af4631d24404410c92159710631f40610ab2011`.
This includes code, data, symbol tables and relocations. Snapshots and reports
are ignored under `build/base-readable/`, not committed.

- `python tools/tubuild.py compile ov004/dScMgBase_c`: all eight licensed
  functions, no unlicensed output.
- All eight manifest entries checked with `linkcheck.linkcheck`, after the
  tool's normal object preparation: **8 VERIFIED**, 1,360 bytes,
  every `diffs: []` and `blind: 0`; `linkcheck.json`.
- `romdata_check.py --files src/minigames/d_s_mg_base.cpp`: this TU emits no
  data records; zero DIFFERS/UNNAMED. This does not claim ownership of the
  class's separately emitted vtable or initializer.
- `port_refcheck.py`: all 418 references resolve. `git diff --check`: clean.
- Full production ROM build at the original base passed: 11,208 functions,
  106/106 exact modules, 26 initialized-data claims and three BSS claims.
  The new composition receives fresh exact-base ROM, declaration and relocation
  checks; their results belong in the published evidence before acceptance.

No header, symbol, manifest, enrollment or attribution file changed. No symbol
or path moved. Private CI, Source review acceptance and merging are not claimed.

## Remaining scope and next owner

This is an eight-function production-TU cleanup, not completed reconstruction of
all minigame-base functions. Other enrolled class slices remain in the wider
first-pass inventory. The `Ent` record, other global record layouts, unknown
fields/helper names, and the stylus owner's class remain unresolved. Their raw
accesses were not replaced with guessed shared types.

Keep the file-global `opt_strength_reduction off` setting and the manifest's
existing boundary: the neighboring real run needs a different compiler setting.
No new compiler barrier was encountered in this pass; the measured object equality
supports all adopted changes. The later second pass can investigate remaining
record accesses after the entire minigame inventory has had its first pass.

Next: independent verifier reviews the exact output and all required gate results;
the coordinator then handles a focused PR and separate composition proof. The
local aggregate worktree's unrelated four incoming promotion-order failures are
not acceptance evidence for this source branch.
