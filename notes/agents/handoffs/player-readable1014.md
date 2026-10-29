# Handoff: player-readable1014

This commit improves readability in the production Player translation unit while
preserving its entire pinned-compiler object. It completes two producer passes;
it does not complete Player's class reconstruction or independent source review.

## Identity and resumption

- Issue: <https://github.com/tangosdev/sm64ds-decomp/issues/3785>.
- Task/stage: `player-readable1014` / `readability`; producer session
  `codex-player-readable1014`, Codex harness.
- Branch: `cpp/player-readable1014`.
- Source base, accepted input and installed workflow tools:
  `132347a5a8d617de18c0de31b0b598788fa08329`.
- Status: locally byte-verified candidate, awaiting independent verification.
- Next action: verifier reviews this immutable commit against its base, including
  source quality, retained limitations and production enrollment, then repeats
  the relevant compiler/link checks in a separate wired worktree.
- Local experiments, objects, per-function results and private receipts remain
  under `C:/tmp/sm64ds-player-readable1014/build/player-readable/`. They are not
  committed. The primary checkout was not used for source edits.

## Changes and evidence

The source scope is `src/game/player/Player.cpp`, ov002
`[0x020bccfc, 0x020e67a8)`: 612 enrolled functions, 257 changed bodies.
Headers, layouts, manifests, symbol names and attribution are unchanged.

Pass one replaced 676 scalar offset accesses in 125 functions with existing
members of the same width and signedness. Pass two replaced 581 mangled call
sites, used typed state objects and animation interfaces, simplified repeated
Player casts and integer pointer arithmetic where matching allowed, and removed
obsolete declarations. The local stand-in Player/fBase classes in
`func_ov002_020c8b78` are gone. The introductory comment now describes the current
scope instead of repeating superseded migration counts.

No new semantic state, animation or field names are asserted. All 71 typed
address-named state objects have the three PMF records populated by
`src/game/player/Player.cpp` (`__sinit_ov002_021019d0`). The script actor's type follows its
Spawn producer and dActor position/angle accesses; its slot stays unnamed.
The original physical Player file boundary remains an inference.

[The experiment record](../../experiments/player-readability-1014.json) contains
the changed ordinals, source blob, object hashes, measured failed simplifications
and outstanding reconstruction work. It distinguishes compiler barriers from
unresolved declarations; a remaining cast is not automatically necessary.

## Byte and production proof

- Pinned compiler: `2004/b56`, stock `rombuild.CFLAGS` with `-lang c++`.
  The base and final raw objects are identical, all 364,400 bytes, SHA-256
  `8578494b097f2690c70bd21119d794a87a1ecd73f115dba65edadb3255190132`.
  This includes all emitted code, relocation records, symbol metadata and order.
- The raw object defines 618 functions: the 612 enrolled functions plus six
  existing compiler-only empty destructors. Applying the unchanged manifest
  policy produces SHA-256
  `285238d9b2627e66076e7f73d947bb9346188968fb7d3c5d0c87e42152da23d6`,
  exactly the Player object consumed by the production build. Player emits no
  non-text allocated data.
- `config/arm9/overlays/ov002/delinks.txt` and the generated stock configuration
  both enroll this source as `complete` over the range above. This is a `src/`
  change, not a shadow translation unit.
- `python tools/rombuild.py -j16 --no-rom`: exit 0; 106/106 modules exact,
  11,334/11,334 source-built functions reproduce, zero new symbol errors.
  Cache use: 2,751 objects reused, one compiled. The final source was compiled
  again after the last readability refinements; its prepared object was checked
  against the object that this build consumed and is identical.
- All 612 enrolled functions from that final fresh raw object were checked with
  `linkcheck.linkcheck`, using each exact symbol, module, address and size:
  612 `VERIFIED`, zero differing bytes, zero blind relocations. The local JSON
  report is `build/player-readable/final-object-linkcheck.json`.
- `python tools/check_decl_agreement.py --changed 132347a5a8d617de18c0de31b0b598788fa08329`:
  exit 0, no new disagreements or local redeclarations of header interfaces.
  This is the repository ratchet, not a claim that every legacy declaration agrees.
- `python tools/port_refcheck.py`: exit 0, 267 references resolve.
- `git diff --check`: exit 0.

The ordinary pre-push gate command is
`python tools/prepush_linkcheck.py --files src/game/player/Player.cpp --json build/player-readable/linkcheck.json`.
Its result is recorded in the experiment JSON alongside the fresh-object check.

## Limits and remaining work

The most concrete layout concern found during the second pass is
`GetBodyModelID()` returning 4 for metal while `Player.h` declares
`mBodyModels[4]` and a byte plus padding at `0x0ec`. New animation rewrites retain
explicit slot loads instead of extending the existing out-of-bounds array idiom.
Resolve this shared layout with allocation/lifecycle evidence and consumer
verification before propagating it. The actor slot at `0x72c` likewise remains
inside an undeclared portion of the layout.

Scalar Fix12 animation/particle/music boundaries, opaque object and vector views,
some raw offsets, and measured copy/reload forms remain. Constructor/destructor,
vtable/RTTI and initializer ownership are still separate. Issue #3785 is open
reconstruction scope; this readability sweep does not close it.

The stock control and candidate both report the same nine pre-existing symbol
check errors. The whole-tree data inventory still reports one differing
`_ZTV10dCcAcPos_c` from two unchanged destructor sources, 1,519 partial symbols
and 21 unnamed symbols. These are not resolved by the Player byte pass.
The production run used `--no-rom`; no candidate NDS packaging, gameplay test,
private PR CI, independent verifier acceptance or current-main composition is
claimed here.
