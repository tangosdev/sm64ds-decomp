# Domain buckets under `src/`

Status: active policy, 2026-08-27. Answers one question `tools/srcpath.py` deliberately
leaves open: which top-level directory does a class's *first* promoted file go into.

## What this is not

This does not change `AGENTS.md`'s placement doctrine or touch `tools/srcpath.py`.
Placement still follows migration, not the other way round (`srcpath.py:46-67`):
`placement_for` only ever *follows* a cohort that already agrees on one directory. It has
no opinion on where a class's directory should first be created, because that decision —
"is `dFader_c` a runtime primitive or a game object" — needs the kind of judgment a scan
of the tree can't supply. That judgment call is what this doc standardizes, so it stops
being reinvented per PR.

It also doesn't touch `src/unnamed/<module>/`. That name stays as-is (not `unknown/`) —
it's hardcoded as `UNNAMED_DIR` in `srcpath.py`, and `src/unnamed/ov063/` is already
migrated and wired into AGENTS.md, the enrolment table, and every tool that calls
`placement_for`. Renaming it to match this doc's vocabulary would cost a real migration
PR for zero functional gain. It's the maturity gate for address-named symbols that
haven't been converted to real C++ yet (`func_ov006_1234abcd`), sharded by ROM module
because that's the only grouping the evidence supports at that stage — orthogonal to the
domain buckets below, which are for classes that already have a name and a real C++ form.
`notes/archive/n64-decomp-cross-reference.md:527-537`'s ruling ("grouping axis is
module/overlay, not subsystem") is scoped to that flat, unconverted case and isn't in
tension with this doc.

## The target tree

```
src/
  runtime/
    memory/
    math/
    graphics/
      fader/
    audio/
    filesystem/
  game/
    player/
    camera/
    actors/
    objects/
    stages/
  ui/
    hud/
    menus/
    messages/
  minigames/
  unnamed/
    <module>/           <- maturity gate, not a domain; see above
```

This is the agreed target shape, not something to build in one PR. Sub-buckets populate
one class at a time, at promotion, same as every other placement decision in this repo —
an empty leaf directory above is not created until a real class needs it.

## The `d`-prefix does not mean "actor"

The working assumption had been that the ROM's `d`-prefixed RTTI classes (`daTrs_c`,
`dBgCh_SphCrr`, ...) are gameplay actors, and mostly they are. `Fader` disproves it as a
*sufficient* rule: `include/Fader.h:54-58` records that `tools/rtti_extract.py` reads
`__si_class_type_info` naming the real classes `dFader_c`, `dFdBrightness_c`,
`dFdColor_c` and `dFdWipe_c` — the same prefix family — but none of them appear in
`ACTOR_SPAWN_TABLE` (the 391-entry table `notes/actor-naming.md` derives actor names
from). They're instantiated directly by scene-transition code, not spawned. So:

**A `d`-prefixed class is a gameplay actor only if it also has an entry in
`ACTOR_SPAWN_TABLE`** (`symbols/overlay_actors.md` / `tools/actor_names.py` is the
source of truth for that). A `d`-prefixed class without a spawn-table entry is a
ROM-native RTTI class of some *other* kind, classified the same way as anything else in
this doc: by what it actually does.

`Fader`, by content, is a fixed-point screen-blend interpolator (`AdvanceInterp`,
`currInterp` in 20.12) plus a wipe-pattern texture loader (`FaderWipe::LoadAndSetFile`) —
a rendering primitive, not gameplay logic and not UI text/menus. That's
`src/runtime/graphics/fader/`, not `src/game/actors/`.

## Bucket-by-bucket grounding

- `runtime/` — engine primitives independent of any one actor or scene.
  - `memory/`, `math/` — no dedicated `notes/*.md` yet; placeholders only.
  - `graphics/` — `fader/` is the one populated example (see above). `Fader` was
    previously under `src/engine/fader/`; remapping it here is migration debt, not done
    by this doc.
  - `audio/` — `_ZN5Sound6Player...` classes exist (`srcpath.py`'s `class_of` docstring
    cites `Sound::Player` as a real symbol, grouped under outer component `Sound`). No
    dedicated `notes/*.md` yet.
  - `filesystem/` — no dedicated note yet; placeholder.
- `game/` — actors and game-flow state driven by the spawn table or scene logic.
  - `player/` — `notes/player-provenance.md`.
  - `camera/` — no dedicated note yet; placeholder.
  - `actors/` — `d`-prefixed classes **with an `ACTOR_SPAWN_TABLE` entry**
    (`notes/actor-naming.md`, `notes/enemy-provenance.md`, `notes/platform-provenance.md`,
    `notes/minigame-provenance.md` for the ones that stay under `minigames/` instead —
    see below). Default landing spot for a spawn-table actor.
  - `objects/` — no rule yet to distinguish this from `actors/`; every `d`-prefixed
    spawn-table class found so far has gone to `actors/`. Leave `objects/` empty until a
    real distinguishing case shows up rather than guessing at a split.
  - `stages/` — `notes/scene-provenance.md`, `notes/dscene-c-siblings-census.md`.
- `ui/` — no gameplay logic, on-screen presentation only.
  - `hud/`, `menus/` — no dedicated note yet; placeholders.
  - `messages/` — was `src/engine/message/`; remapping here is migration debt.
- `minigames/` — top-level, not nested under `game/`, per `notes/minigame-provenance.md`.

**Adding a bucket this doc doesn't already list** needs the same bar: point at existing
evidence (a census/provenance doc, the RTTI hierarchy, or `ACTOR_SPAWN_TABLE` membership)
in the PR that adds it, not a guess about what EAD Tokyo probably had.

## Migration debt (not executed by this doc)

Three already-migrated buckets predate this tree and don't fit it as-is. Remapping any of
these is a separate rename-only PR, done with the usual attribution-safe recipe — nothing
here executes them:

| Today | Target | Why |
|---|---|---|
| `src/actors/{BigBoo,...,daTrs_c}` (10 classes) | `src/game/actors/` | same content, new parent |
| `src/engine/message/` | `src/ui/messages/` | UI content, mislabeled `engine/` |
| `src/engine/fader/` | `src/runtime/graphics/fader/` | not a spawn-table actor; see above |
| `src/unnamed/ov063/` | unchanged | name intentionally kept, see above |

## Decision procedure for a promotion PR

1. Does the class already have any files in a subdirectory? Then `placement_for` already
   answers this — do nothing, it follows the existing cohort (which may still be the old,
   unmigrated location until its migration-debt PR lands).
2. First-ever promoted file for this class (the bootstrap case `placement_for` can't
   answer): check `ACTOR_SPAWN_TABLE` membership first, then match content against the
   bucket list above, and create `src/<bucket>/.../<Class>/` in the same PR that promotes
   the class. If nothing obviously fits, default to `src/game/actors/<Class>/` — most of
   the ROM's named classes are actors, and a wrong-but-consistent default beats a
   one-off directory nobody else will reuse.
3. Never pre-create a bucket for a class that hasn't been promoted yet.

## Why not more buckets, sooner

Only 168 files (`src/actors/`, `src/engine/`, `src/unnamed/ov063/`) have moved out of the
flat root as of 2026-08-26, out of 11,302. Directory reorganization is this project's
lowest-priority goal, behind byte-match accuracy and portability — this doc exists so
that the placement decisions already happening inside conversion PRs are consistent with
each other, not to justify spending dedicated effort moving already-converted classes
into a fuller tree.
