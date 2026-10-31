# Domain buckets under `src/`

Status: active policy, 2026-08-27; the class-TU directories below were populated on 2026-10-14. Answers one question `tools/srcpath.py` deliberately
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

```text
src/
  runtime/
    memory/
    math/
    actor/              <- populated: ActorBase, dActor_c, dEnemyBase_c, ...
    graphics/
      fader/
    audio/
    filesystem/
  game/
    player/
    camera/
    actors/             <- d_a_<name>.cpp, flat (no per-class subdirectory)
    objects/
    stages/             <- scene classes
  ui/
    hud/                <- populated: dMeter_c, dMap_c, cap icons
    menus/
    messages/
  minigames/            <- populated: dScMg*_c, dMg*_c, d_s_mg_*, ov004 units
  lib/
    msl/                <- populated: ansi_fp.c (Metrowerks Standard Library)
    runtime/            <- populated: NMWException.cpp, __cxa_vec_* (MWCC runtime)
  unnamed/
    <module>/           <- maturity gate, not a domain; see above
```

This is the agreed target shape, not something to build in one PR. Sub-buckets populate
one class at a time, at promotion, same as every other placement decision in this repo —
an empty leaf directory above is not created until a real class needs it.

## The `d`-prefix does not mean "actor"

The working assumption had been that the ROM's `d`-prefixed RTTI classes (`daTrs_c`,
`dBgCh_SphCrr`, ...) are gameplay actors, and mostly they are. `dFader_c` disproves it as a
*sufficient* rule: `include/dFader_c.h:54-58` records that `tools/rtti_extract.py` reads
`__si_class_type_info` naming the real classes `dFader_c`, `dFdBrightness_c`,
`dFdColor_c` and `dFdWipe_c` — the same prefix family — but none of them appear in
`ACTOR_SPAWN_TABLE` (the 391-entry table `notes/actor-naming.md` derives actor names
from). They're instantiated directly by scene-transition code, not spawned. So:

**A `d`-prefixed class is a gameplay actor only if it also has an entry in
`ACTOR_SPAWN_TABLE`** (`symbols/overlay_actors.md` / `tools/actor_names.py` is the
source of truth for that). A `d`-prefixed class without a spawn-table entry is a
ROM-native RTTI class of some *other* kind, classified the same way as anything else in
this doc: by what it actually does.

`dFader_c`, by content, is a fixed-point screen-blend interpolator (`AdvanceInterp`,
`currInterp` in 20.12) plus a wipe-pattern texture loader (`dFdWipe_c::LoadAndSetFile`) —
a rendering primitive, not gameplay logic and not UI text/menus. That's
`src/runtime/graphics/fader/`, not `src/game/actors/`.

## Bucket-by-bucket grounding

- `runtime/` — engine primitives independent of any one actor or scene.
  - `memory/`, `math/` — no dedicated `notes/*.md` yet; placeholders only.
  - `graphics/` — `fader/` is the one populated example (see above). `dFader_c` was
    moved here from the engine layer.
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
  - `messages/` — moved here from the engine layer.
- `minigames/` — top-level, not nested under `game/`, per `notes/minigame-provenance.md`.
- `lib/` — code the toolchain shipped, not EAD's: it would be in any mwccarm-built
  game. The evidence is each file's own banner, which names the library source the
  body matches.
  - `msl/` — the Metrowerks Standard Library (C library). `ansi_fp.c` is MSL's
    `ansi_fp.c` in reverse source order (the Petari and Spirit Tracks decomps).
  - `runtime/` — the MWCC C++ runtime: exception handling (`NMWException.cpp`, after
    the Wind Waker decomp's `NMWException.cp`) and the `__cxa_vec_*` array helpers
    mwccarm calls by name for `new[]`/`delete[]`.
  - NitroSDK is not here: it stays in `engine/<module>/` (`os/`, `mi/`, `fs/`, `snd/`,
    `wm/`, ...), one directory per SDK module.
  - A one-function source stays in `src/` root, where `srcpath` puts any named symbol
    outside a TU, until it folds into a TU. The coined `cstd*.cpp` TUs stay in root
    until their MSL file names are recovered, so they move once.

**Adding a bucket this doc doesn't already list** needs the same bar: point at existing
evidence (a census/provenance doc, the RTTI hierarchy, or `ACTOR_SPAWN_TABLE` membership)
in the PR that adds it, not a guess about what EAD Tokyo probably had.

## Migration debt (not executed by this doc)

The engine layer predates this tree. Remapping any of
these is a separate rename-only PR, done with the usual attribution-safe recipe — nothing
here executes them:

| Today | Target | Why |
|---|---|---|
| `src/engine/<subsystem>/` | stays | the engine layer (collision, model, sound, heap, particle, view, ...) is a coherent bucket in its own right; only `message/` and `fader/` were misfiled and have moved |
| `src/unnamed/ov063/` | unchanged | name intentionally kept, see above |

## Decision procedure for a promotion PR

1. Does the class already have any files in a subdirectory? Then `placement_for` already
   answers this — do nothing, it follows the existing cohort (which may still be the old,
   unmigrated location until its migration-debt PR lands).
2. First-ever promoted file for this class (the bootstrap case `placement_for` can't
   answer): check `ACTOR_SPAWN_TABLE` membership first, then match content against the
   bucket list above, and create `src/<bucket>/` in the same PR that promotes the class.
   If nothing obviously fits, default to `d_a_<name>.cpp` under `src/game/actors/` — most of
   the ROM's named classes are actors, and a wrong-but-consistent default beats a
   one-off directory nobody else will reuse.
3. Never pre-create a bucket for a class that hasn't been promoted yet.

## Where the class TUs went

The promoted class TUs that used to sit in the top-level actors directory (223 files) and the first
`src/game/actors/` batch were filed into the buckets above in one rename-only change:
actors to `src/game/actors/` as `d_a_*.cpp`, scenes to `src/game/stages/`, minigame
scenes and helpers to `src/minigames/`, actor base classes to `src/runtime/actor/`, HUD
classes to `src/ui/hud/`, `Player` to `src/game/player/`. The old top-level actors directory no longer exists.
Directory reorganization is still the project's lowest-priority goal, behind byte-match
accuracy and portability.
