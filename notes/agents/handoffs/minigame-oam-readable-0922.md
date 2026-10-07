# Minigame OAM render wrappers readability: first pass

This handoff describes this commit; the queue records the immutable output.

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/2998
- Task: `minigame-oam-readable-0922`, produce stage.
- Producer: `devin-ov004-mgoam` (Devin).
- Branch/worktree: `deslop/ov004-mgoam`,
  `C:/Users/issus/Documents/SGH/decomps/worktrees/deslop-ov004-mgoam`.
- Claim input commit: `4189f3c19b0f00e793d9f722a69cfe7791aa7f35`.
- Composed main: `4ff9f02533abb84ff1269aa6fe503a64c036f9bc`.
- Scope: production `src/minigames/d_s_mg_base.cpp`, the three OAM render
  wrappers in [ov004](../../../config/arm9/overlays/ov004/symbols.txt):
  `Hud_RenderSprite` `[0x020af68c,0x020af770)` (0xe4),
  `RenderOamMainScreen` `[0x020afa20,0x020afb20)` (0x100) and
  `RenderOamBothScreens` `[0x020b0104,0x020b023c)` (0x138), plus this handoff.
- Status: local candidate; independent exact-commit verification remains
  required.

## Changes

Each wrapper gated on the same three reads: the scene pointer at
`data_ov004_020beb68`, `+0x4628` and a `vtable[0x1a]` call compared to 2.
All three are already modeled: `data_ov004_020beb68` is `dScMgBase_c *`,
`+0x4628` is `dScMgBase_c::mMenuOpen`, and vtable slot 26 (+0x68) is
`dScMgBase_c::OnHitByCannonBlastedChar`, the same spelling the file's own
class methods already use. The `char *`/`void **`/`GetFn` scaffold is gone;
each body loads the global once as `dScMgBase_c *scene`.

The `+0x4664` halfword stays an explicit `u16` byte-offset access: the modeled
storage ends at `mSceneKind` (0x4660), so 0x4664 is beyond the modeled base.
The read also stays a fresh global load after the virtual call, as the ROM
emits it, rather than a reuse of the cached `scene` pointer.

Calls now use the callee's native spelling:

- `OAM::RenderSub` and `OAM::Render(bool, OamAttr *, …, Matrix2x2 *)` are
  real C++ members of the `OAM` namespace (definitions in
  `src/engine/oam/OAM.cpp`), declared once at file scope next to the existing
  forward declarations, which moved up so `Hud_RenderSprite` can see them.
- The ten-argument `Render` keeps its literal mangled spelling
  `_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii` -- its definition documents
  that as the by-value `Fix12<int>` wall -- but the local declaration now
  matches the definition (`void` return, `OamAttr *`, `s32` coords).

Parameter names follow the callee contracts (`attr`, `x`, `y`, `palette`,
`priority`, `mtx`); the extern "C" signatures keep their `void *`/`int`
types so the caller declarations elsewhere in the tree still agree. All
three `@symbol` markers and definition order are unchanged.

## Local proof and limits

The TU compiled byte-identical before and after the edit.

- `python tools/tubuild.py verify ov004/dScMgBase_c --no-write`:
  **125/125 MATCH**, objisolate clean, relocation destinations clean,
  ROM-ascending emission order; the three edited functions each MATCH.
- `python tools/prepush_linkcheck.py --files src/minigames/d_s_mg_base.cpp`:
  **125 verified, 0 warnings, 0 blocking**.
- `python tools/romdata_check.py --files src/minigames/d_s_mg_base.cpp`:
  0 DIFFERS / 0 UNNAMED.
- `python tools/check_decl_agreement.py --changed origin/main`: no new
  declaration disagreements, no new local redeclarations of
  header-declared symbols. The RenderSub/Render namespace declarations
  spell the definitions' own contracts.
- `python tools/port_refcheck.py`: 337 references resolve.
  `python tools/check_dead_references.py`: no new dead references.
  `python tools/queue_audit.py --check-promoted`: agrees.
  `git diff --check`: clean.

No header, manifest, symbol, enrollment or baseline file changed. The
sibling wrappers (`func_ov004_020af770` family, `DrawOamSprite`,
`func_ov004_020afdd0`/`020aff38`/`020b023c`/`020b0380`) keep the same
gate shape with their own local views; unifying them is the wider second
pass, not this commit.

## Queue spec note

The task's `produces` list names three standalone per-symbol sources
(`Hud_RenderSprite.cpp`, `RenderOamMainScreen.cpp` and
`RenderOamBothScreens.cpp` under `src/`) that do not exist: these
functions live inside the promoted `ov004/dScMgBase_c` TU, and extracting
them would undo the fold and need manifest/delinks churn this task
explicitly avoids. The spec's paths are function-name boilerplate; the
real deliverable is the in-place deslop recorded above, so `v2 publish`
(which requires the produces paths in the output commit) cannot accept
the spec. The lease was released with this reason for the coordinator to
amend `produces` to `src/minigames/d_s_mg_base.cpp` or cancel; the
produced commit and PR carry the actual work.
