# Building the ROM from merged TUs (Track B)

Goal: make `tools/rombuild.py` link a module from its `config_tu/` merged translation
units (`src_tu/**/*.cpp`) instead of the per-function `src/` files, byte-identical to
retail. This is the production-enrollment step the TU-reconstruction ladder stops short
of (`tubuild.py promote --dry-run` refuses; `cpp_tu_compat` marks `enroll`/`rombuild`/
`port_refcheck` "not ready").

## What exists

- `config_tu/arm9/**` — TU-granular delinks roots for 8 overlays (ov009, ov010, ov014,
  ov019, ov020, ov029, ov070, ov077), from the "TU seed" PRs (#1646–1651). Each maps a
  TU to a merged `src_tu/actors/*.cpp` with per-section (`.text/.init/.ctor/.data/.bss`)
  ranges, but **no `complete` markers** — they are objdiff *target* roots, not builds.
- `rombuild.py --tu-module <id>` (this branch) — swaps a module's generated delinks for
  its `config_tu` version + injects `complete`, and widens `enrolled()`'s source-root
  allowlist to `src_tu/`. `rombuild_profile.available_tu_modules()` lists the 8.

## M1 result (2026-08-23): compile+enroll works, link hits the RTTI wall

`python tools/rombuild.py --no-rom --tu-module ov010` compiles all 3 merged TUs
(Trap/LightBeam/PeachPainting) as `//cpp` and enrolls them, then **mwldarm fails**:

    Multiply-defined: "typeinfo structure for fBase_c" in Trap.o
      Previously defined in _dsd_gap@main_44.o
    ... typeinfo for dBase_c / dActor_c / dBgActor_c likewise ...

A merged **derived-class** TU emits vague copies of its whole base chain's RTTI
(`_ZTI`/`_ZTS` for fBase_c/dBase_c/dActor_c/dBgActor_c), which collide with the canonical
copies the gap objects carry (those live in arm9/ov002). The per-function `src/` build
never hit this because single-function objects emit no RTTI/vtable.

The 6 currently `link-verified` manifest TUs are all *base* classes (arm9/Actor,
ActorBase, ActorDerived, ActorBase_SceneNode, ov004/dScMgBase_c, ov002/LevelObjects) —
they canonically own their RTTI, so no collision, no externalization needed. Derived TUs
are the unsolved case.

## The reduction that's needed

`rombuild._isolate` only does single-function reduction (`objisolate.isolate(obj, ONE)`).
A merged TU object must instead be reduced to **all** the ranges its delinks entry claims
while externalizing what it doesn't own. `objisolate.py` already has the primitives
`tubuild`'s partitioned link uses:

- `deadstrip_plan(raw, symbol_names)` / `derive_deadstrip` — keep a *set* of symbols.
- `externalize_plan(raw, symbol_names)` — externalize exact `_ZTI/_ZTS` RTTI objects,
  rebinding them to their canonical ROM address (only `_ZTI/_ZTS` are allowed).
- single-symbol `plan`/`derive` rebind a kept function's refs to ROM addresses, incl. the
  `_ZTV` +8 storage-vs-address-point bias (notes/objisolate.md; getting the addend wrong
  links clean and corrupts 34 modules).

So M2 = in rombuild's isolate step, for a TU-module object: keep the claimed `.text`
functions + owned `.data/.bss/.init/.ctor` bands, externalize the inherited base-class
RTTI to the gap's canonical addresses, drop the rest.

## The catch for a chosen target

The externalization recipe (which RTTI to externalize, and to what address) must exist.
For ov010 the manifest entries are only `text-verified` with empty `data`, so the recipe
isn't recorded yet. Two ways forward:
1. Establish it first with `tubuild.py linkcheck ov010/<TU> --partitioned` (the scratch
   ladder), record the `data`/`externalized_output` policy in `config/tu_manifest.json`,
   then have rombuild consume it. (Reuses the proven ladder; scratch-only today.)
2. Compute it in rombuild: detect emitted `_ZTI/_ZTS` whose canonical home is another
   module/TU and externalize to that address automatically. (No manifest dependency, but
   re-derives what the ladder already knows how to prove.)

Recommended: (1) for the first module (drive from the manifest the ladder already
produces), then generalize.

## Invariants (from decomp-tu-slicing / decomp-tu-build)

- Module-level exclusivity: a module is old-shape or ph-shape, never both. `--tu-module`
  enforces this by replacing the whole module's delinks.
- A delinks file is a partition: no overlap, no gap, nothing the old config claimed left
  unclaimed. ov010's `.text/.init/.ctor` are full source partitions; `.data/.bss` are
  TU-owned bands + ROM gap (verified).
- Without `complete`, dsd fills from ROM bytes and the source is never compiled — check
  `source-built`, not `eligible`.
- `dsd delink` before `dsd objdiff`.
