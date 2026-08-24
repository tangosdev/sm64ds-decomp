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
- `rombuild.py --partitioned-tu <manifest-id>` — the M2 production path. It retains
  the manifest TU's existing ROM-ordered text selectors, compiles the merged source
  once, substitutes the exact derived text objects, and adds one reduced object for
  its licensed non-text claims. The option is repeatable for disjoint verified TUs.

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

## M2 result (2026-08-23): production partitioning is available

The normal ROM builder now consumes the strict manifest-backed partition recipe:

1. Require a current content-bound stock `tubuild linkcheck --baseline` control and a
   manifest entry whose `partitioned_link.state` is `partitioned-link-verified`.
2. Revalidate that the legacy entries are complete, exactly tile the text span, and
   that every non-text claim comes from a pure ROM gap.
3. Compile the merged source once, derive every function object, and require its full
   linker contribution to equal the current production compile+isolate object.
4. Re-run exact compiler-only deadstrip and inherited `_ZTI`/`_ZTS` canonical-import
   verification, then retain only licensed non-text sections and symbols.
5. Re-run owned byte, symbol, relocation, vtable address-point, and storage-alias
   checks before the normal `rombuild` linker consumes any prepared object.
6. Require `dsd check modules`, zero new symbol errors relative to the stock control,
   exact storage aliases, 106/106 module fidelity, and a packaged ROM whose SHA-256 is
   identical to the strict stock control.

The first production control is `ov002/daObjAbuku_c`:

```powershell
python tools/tubuild.py linkcheck --baseline --module ov002 -j 16 --clean
python tools/rombuild.py --partitioned-tu ov002/daObjAbuku_c -j 16
```

That run prepares eight objects from one merged compile (seven exact text
contributions plus one reduced `.data` object), reproduces all 106 modules, introduces
zero symbol errors beyond the seven recorded by the untouched baseline, preserves the
vtable storage alias, and produces the stock ROM hash
`d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
`--no-rom` remains useful for iteration but does not exercise the final packaged-ROM
identity gate.

`--partitioned-tu` and `--tu-module` are deliberately mutually exclusive. The first is
the evidence-backed incremental production path; the second remains the whole-module
Track B experiment described below.

## The reduction M2 implements

`rombuild._isolate` supports the legacy single-function reduction and an intact,
text-only multi-function object. A merged TU with data needs a different partition:
all text functions must come from its one compile while only the licensed non-text
ranges survive, and definitions it does not own must be externalized. `objisolate.py`
already has the primitives
`tubuild`'s partitioned link uses:

- `deadstrip_plan(raw, symbol_names)` / `derive_deadstrip` — keep a *set* of symbols.
- `externalize_plan(raw, symbol_names)` — externalize exact `_ZTI/_ZTS` RTTI objects,
  rebinding them to their canonical ROM address (only `_ZTI/_ZTS` are allowed).
- single-symbol `plan`/`derive` rebind a kept function's refs to ROM addresses, incl. the
  `_ZTV` +8 storage-vs-address-point bias (notes/objisolate.md; getting the addend wrong
  links clean and corrupts 34 modules).

M2 keeps the claimed `.text` functions plus owned `.data/.bss/.init/.ctor` bands,
externalizes inherited base-class RTTI to its exact canonical addresses, and drops
only output carrying an explicit verified disposition. It preserves the partitioned
proof's N text selectors plus one non-text selector because that is the already-proven
linker surface; it does not guess that a whole raw object is layout-equivalent.

## The catch for a chosen whole module

The externalization recipe (which RTTI to externalize, and to what address) must exist.
For ov010 the manifest entries are only `text-verified` with empty `data`, so the recipe
isn't recorded yet. Two ways forward:
1. Establish it first with `tubuild.py linkcheck ov010/<TU> --partitioned` (the scratch
   ladder), record the `data`/`externalized_output` policy in `config/tu_manifest.d/`,
   then have rombuild consume it. (Reuses the proven ladder; scratch-only today.)
2. Compute it in rombuild: detect emitted `_ZTI/_ZTS` whose canonical home is another
   module/TU and externalize to that address automatically. (No manifest dependency, but
   re-derives what the ladder already knows how to prove.)

M2 therefore refuses `--partitioned-tu` for an entry without a complete, previously
partitioned-link-verified recipe. The first usable control is Abuku. ov010 still needs
data/RTTI ownership established for each of Trap, LightBeam, and PeachPainting before
`--tu-module ov010` can become a verified whole-module build; the production mechanism
is ready, but those three source/manifest inputs are not.

## Invariants (from decomp-tu-slicing / decomp-tu-build)

- Module-level exclusivity: a module is old-shape or ph-shape, never both. `--tu-module`
  enforces this by replacing the whole module's delinks.
- A delinks file is a partition: no overlap, no gap, nothing the old config claimed left
  unclaimed. ov010's `.text/.init/.ctor` are full source partitions; `.data/.bss` are
  TU-owned bands + ROM gap (verified).
- Without `complete`, dsd fills from ROM bytes and the source is never compiled — check
  `source-built`, not `eligible`.
- `dsd delink` before `dsd objdiff`.
