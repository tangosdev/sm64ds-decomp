# DropShadow* mangled-free vs member form, and Animation::Advance (PIANO-2712-02, PR #2712)

- tested_commit: 747142e33 (cleanup-dapiano at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: the deslop bullet that used to sit at
  `src/game/actors/daPiano_c/d_a_piano.cpp:39-41` made two separate
  assertions in one sentence -- that `dActor_c::DropShadowRadHeight` and
  `dActor_c::DropShadowScaleXYZ` must stay mangled free declarations
  (wall 6az), and that `Animation::Advance` must keep the free form with an
  `(Animation *) upcast arg` because "the member goes through the thunk".
  Both were inherited as "daBmb's leftover" rather than measured here. This
  experiment measures each half separately. They do not come out the same
  way: the first is a real constraint, the second is false and is fixed in
  this candidate.

## Half 1 -- DropShadowRadHeight / DropShadowScaleXYZ: CONSTRAINT SUPPORTED

Carrier function `func_ov063_0211d5f4` (`0x0211d5f4`, size `0x234`), which
holds all three drop-shadow calls. Header member forms are
`include/dActor_c.h:276-282`; both take `Fix12<int>` by value.

- attempted_change A (scratch-applied, then reverted) -- the literal
  spelling, calling the member with the same integers the mangled free
  declaration accepts (its parameters are `Fix12i`, and `include/types.h:45`
  makes that a plain `s32`):

```diff
-    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
-        self, &self->mShadowModel3, &self->mShadowMats[2], 0xf0000, 0x50000, 0xf);
+    self->DropShadowRadHeight(self->mShadowModel3, self->mShadowMats[2], 0xf0000, 0x50000, 0xf);
```

- result A: does not compile. `Fix12<int>` is a bare aggregate
  (`include/math/Fix12.h:21-23` is `{ T val; }`), so there is no implicit
  conversion from `int`.
- log A (verbatim):

```text
  ! compile failed (2004/b56): mwccarm.exe: warning: Environment variable 'MWCIncludes' not found
src\game\actors\daPiano_c\d_a_piano.cpp:579: function call '[daPiano_c].DropShadowRadHeight({lval} ShadowModel, {lval} Matrix4x3, int, int, int)' does not match
src\game\actors\daPiano_c\d_a_piano.cpp:579: 'dActor_c::DropShadowRadHeight(ShadowModel &, Matrix4x3 &, Fix12<int>, Fix12<int>, unsigned int)' (non-static)
src\game\actors\daPiano_c\d_a_piano.cpp:580: function call '[daPiano_c].DropShadowScaleXYZ({lval} ShadowModel, {lval
```

- attempted_change B (scratch-applied, then reverted) -- the member form with
  each `Fix12<int>` argument materialized, which is what the type actually
  requires:

```diff
+        Fix12<int> rad = {0xf0000}, dep = {0x50000};
+        Fix12<int> sx1 = {0xa0000}, sy = {0x50000}, sz1 = {0x110000};
+        Fix12<int> sx2 = {0x80000}, sz2 = {0xf0000};
+        self->DropShadowRadHeight(self->mShadowModel3, self->mShadowMats[2], rad, dep, 0xf);
+        self->DropShadowScaleXYZ(self->mShadowModel1, self->mShadowMats[0], sx1, sy, sz1, 0xf);
+        self->DropShadowScaleXYZ(self->mShadowModel2, self->mShadowMats[1], sx2, sy, sz2, 0xf);
```

- command:

```text
python tools/match.py --c src/game/actors/daPiano_c/d_a_piano.cpp \
  --func func_ov063_0211d5f4 --addr 0x0211d5f4 --size 0x234 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
```

- result B: baseline MATCH; variant size `0x234` vs `0x29c` (+0x68), no
  match. Same shape as the `SetAnim` measurement in
  `notes/experiments/piano-2712-setanim-6az.md`, larger because three calls
  home seven values instead of one.
- log B (variant, verbatim):

```text
TARGET func_ov063_0211d5f4 @ 0x0211d5f4 size 0x234  bytes: 10402de928d04de20020a0e30118a0e30137a0e3001061e20040a0e10c308de510208de518208de51c208de520208de514108de5fe18d4e1e8019fe5cd79fceb... (564 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x234 vs candidate 0x29c

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED. The mangled free declarations stay, and the
  source comment now states the measured cost instead of citing a sibling.

## Half 2 -- Animation::Advance: CLAIM REFUTED, FIXED IN THIS CANDIDATE

Carrier function `func_ov063_0211d8cc` (`0x0211d8cc`, size `0x2d8`). The
member form is `include/Animation.h:69`, and `ModelAnim` derives from
`Animation` as a non-leading base (`include/ModelAnim.h:69`), which is the
upcast the old spelling performed by hand.

- attempted_change (KEPT -- this is the change this candidate now ships):

```diff
-    extern void _ZN9Animation7AdvanceEv(Animation* a);
...
-    _ZN9Animation7AdvanceEv((Animation*)&self->mModelAnim);
+    self->mModelAnim.Advance();
```

- command (per-function, link, emission):

```text
python tools/match.py --c src/game/actors/daPiano_c/d_a_piano.cpp \
  --func func_ov063_0211d8cc --addr 0x0211d8cc --size 0x2d8 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
python tools/linkcheck.py --name func_ov063_0211d8cc \
  --addr 0x0211d8cc --size 0x2d8 --module ov063
```

  Emission by the same method as `notes/experiments/piano-2712-factory.md`:
  compile the TU with the production flags via `tools/match.py` (`compile_c`),
  list defined `STT_FUNC` symbols, and run `tools/objisolate.py`
  (`plan_many`) with the 15 ROM-order symbols.

- result: MATCH, link VERIFIED with no blind slots, and the object is
  unchanged in shape -- exactly the 15 `.text` functions, no data sections,
  no thunk and no vague `Animation` copy. The compiler performs the
  base-subobject adjustment itself, so the hand-written upcast bought
  nothing. There is no wall here.
- log (verbatim):

```text
TARGET func_ov063_0211d8cc @ 0x0211d8cc size 0x2d8  bytes: f04f2de934d04de200a0a0e16e0e8ae23c75fceb000050e334d08d12f04fbd181eff2f11050aa0e30090a0e30980a0e198008ae509b0a0e10960a0e10170a0e3... (728 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

```text
{
 "name": "func_ov063_0211d8cc",
 "module": "ov063",
 "addr": "0x0211d8cc",
 "verdict": "VERIFIED",
 "diffs": [],
 "blind": 0
}
```

```text
compiled: True 9400
---defined STT_FUNC---
func_ov063_0211d5f4 size=0x234 sec=.text[5]
func_ov063_0211d828 size=0x64 sec=.text[7]
func_ov063_0211d88c size=0x40 sec=.text[9]
func_ov063_0211d8cc size=0x2d8 sec=.text[11]
func_ov063_0211ddac size=0x48 sec=.text[18]
func_ov063_0211dba4 size=0x14 sec=.text[13]
func_ov063_0211dbb8 size=0x1c0 sec=.text[14]
func_ov063_0211dd84 size=0x28 sec=.text[17]
func_ov063_0211dd78 size=0xc sec=.text[16]
func_ov063_0211ddf4 size=0x48 sec=.text[20]
_ZN9daPiano_c16CleanupResourcesEv size=0x50 sec=.text[22]
_ZN9daPiano_c6RenderEv size=0x2c sec=.text[24]
_ZN9daPiano_c8BehaviorEv size=0xf4 sec=.text[25]
_ZN9daPiano_c13InitResourcesEv size=0x17c sec=.text[27]
daPiano_c_classInit size=0x98 sec=.text[29]
---defined data/object---
---plan_many(15)---
error: None
```

- verdict: CLAIM REFUTED -- the free declaration and the `(Animation *)`
  upcast are removed, and the TU calls the real member. One fewer manually
  spelled method symbol in this file.

## Note on inherited claims

Both halves of the old bullet were carried over from a sibling actor rather
than measured against this TU. One survived measurement and one did not. A
leftover that cites another file's result is not evidence about this
candidate, which is the whole point of PIANO-2712-02.
