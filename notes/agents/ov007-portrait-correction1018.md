# OV007 portrait source correction

This continues [issue #4112](https://github.com/tangosdev/sm64ds-decomp/issues/4112) and supersedes the source-quality acceptance of the original PR #4117 and #4119 candidates. Both PRs are drafts. The correction task is `ov007-portrait-correction1018`.

The pointer tables at `020d7f0c..020d85fc` belong to the original configured data interval. Making their pointer elements const and changing the link configuration to accommodate that guess was wrong. They now emit writable `.data`; their pointees remain const. Original historical declarations are still not known.

## Source ownership

The current one-function carrier is provisional. Its address-derived filename does not identify an original TU. A concrete consolidation lead consists of 25 consecutive functions at `020b8f50..020bbf98` using the same two controller globals, `02104ba0` and `02104b9c`. Initialization at `020b9880` allocates the 0x4c controller and 0x14 visual state; teardown at `020b9770` frees them; playback at `020bbd24` advances progress; `020bbe60` selects a portrait. The generic path engine and its allocators are a separate layer. Adjacency and shared lifecycle support this research scope, but do not prove the original file boundary.


The root global-data coordinator owns the continuation. Before expanding source, reserve all 25 controller paths and their text interval, and reconcile [PR #4106](https://github.com/tangosdev/sm64ds-decomp/pull/4106): at `f487f61ac9addec52c46d1556c1ceaac640c156e` it changes `func_ov007_020bad8c.c` and supplies a 12-byte table at `020d7b54`. Its changes must be composed and reviewed, not overwritten.

A combined file is not yet ready. The two controller globals have incompatible pointer/array/struct declarations across the 23 C and two C++ sources. Local `S` and `G` tags conflict. Callers disagree with the return types of `020baa10`, `020bad8c`, `020c6550`, `020c92d0` and `020c5188`; the `020bdeb0` veneer also has an unresolved argument contract. `020baa10` includes the separately reserved `decl_common.h`, which declares the controller global as `char*`. Use the established `Ov007MenuState.h` layout and inspect the shared-header dependencies before introducing a controller type.

The integration sequence is to derive one controller/visual-state layout from initialization, teardown and accesses; reconcile each caller contract with byte proof; then compile a readable combined C++ controller in ROM function order with the existing C ABI. Scope the strength-reduction control in `020b9880` and propagation control in `020bb4b0` so they cannot affect neighbors. Establish complete portrait-data ownership separately, then enroll the combined source and retire the absorbed files only after independent source and relocation review. A rename, a header full of numeric imports, or concatenation of the current raw-offset functions would not satisfy that work.

## Reproduction

From the repository root, with the pinned compiler and extracted ROM available:

```sh
python notes/experiments/ov007-portrait-correction1018.py
python tools/tubuild.py linkcheck --baseline --module ov007 -j16
python tools/tubuild.py linkcheck ov007/AnimationPathSets -j16
python tools/rombuild.py -j16 --data-provenance-requests build/portrait-correction/requests.json --report-json build/portrait-correction/production.json
python tools/prepush_linkcheck.py --range 8d866a3620fa342c72cfb56d5d255f484878ced9..HEAD
python tools/check_decl_agreement.py --changed 8d866a3620fa342c72cfb56d5d255f484878ced9
python tools/port_refcheck.py
```

The committed experiment compiles the actual source, checks original section kinds and writable flags, verifies every native object against ROM bytes after exact relocation, checks configured overlay destinations, and generates the requests consumed by the production provider check. It requires no private helper scripts or precomputed fixtures. Its output is object-level proof; the separate full production command establishes actual providers and module fidelity.

At this checkpoint, the native experiment passes: 26 objects, 2,636 bytes, and 468 data relocations. Production results must be bound to the tested commit; source-layout acceptance remains pending. Nine historical DSD symbol diagnostics must be reported separately from new diagnostics. All 435 unique leaf arrays and the three Luigi pointer tables remain ROM supplied. This is not a fully source-built overlay or a recovered original TU.
