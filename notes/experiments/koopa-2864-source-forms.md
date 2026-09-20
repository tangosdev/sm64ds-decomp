# Koopa PR #2864: bounded shadow-TU source forms

This records the historical shadow input below. The three caller bodies now live
in `src/game/actors/d_a_nknk.cpp`; the identity and production promotion evidence
is in [the later handoff](../agents/handoffs/pr2864-identity-promotion-0920.md).
The older compiler results and limitations below remain evidence for their pinned
input, not claims about the promoted candidate.

Input: `74796d608f77ca9351bb6b2c907104edd615976c`; parent/base:
`9022c77b3834c5ee77c31b4fb4af587418137d6c`. This experiment repairs the existing
unpromoted shadow TU. It does not establish whole-TU enrollment or source acceptance.

## Compiler experiment

Compiler: `tools/mwccarm/2004/b56/mwccarm.exe`. Flags from
`build_pin.flags_for` for this `//cpp` source:

```text
-O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc -Cpp_exceptions off
```

Compile the complete shadow source named by the unchanged Koopa manifest once;
derive each of its 39 functions with `objisolate.derive`, then use `linkcheck.linkcheck`
against its ov062 address and size. Every accepted form below has 39 `VERIFIED`
results, zero blind relocations and no differences. The complete object, including
its still-unlicensed metadata, is identical to the original input:
SHA256 `24100cde460a1703e174ef4c0ec071463ec51e669d0ca8b62625f8f16d11a041`.

| Cumulative candidate | Result |
| --- | --- |
| Original input | 39/39; reference object |
| `mWalkState = 0; mInvincibleTimer = 0;` | Entire object identical |
| `mModelAnim.Render((const Vector3*)&mScaleX);` | Entire object identical |
| One void, two-argument animation helper; void transition helper with plain early returns | Entire object identical |
| SetAnim bridge agrees with the current out-of-line definition: void, typed pointers, scalar speed, u16 start frame | Entire object identical |
| Genuine SetAnim call with `Fix12<int> speed = {koopa->mAnimSpeed};` | Rejected: helper grows from 0x50 to 0x5c; other 38 functions verify |

The last probe called
`koopa->mModelAnim.SetAnim((BCA_File*)data_ov062_0211cee8[idx]->file, data_ov062_0211cf0c[idx], speed, 0)`.
Its object SHA256 is `da0dfc7a6473df67c95d3b88a7b481b2189e890b721ff27a6fdc44624e3cbf42`.
This is one measured source form, not an absolute compiler limitation. The final
source retains the scalar bridge and removes the now-unused fabricated `Mdl` class.

## Contract evidence

`ModelAnim` begins at Koopa+0x300 and has size 0x64. The old ModelAnim-relative
0xc4/0xca expressions reached Koopa+0x3c4/+0x3ca, beyond that member. `Koopa.h`
already names these u16 fields `mWalkState` and `mInvincibleTimer`. The named stores
correct object ownership in the source; the absolute ROM addresses never changed.

`func_ov062_02117994(char*, int)` selects an animation and stores its index at
+0x398. ROM instruction 0x021179a0 overwrites incoming r2 before any read, so the
namespace alias with a third argument has no callee support. At caller
0x02118100, r2=1 supplies the state store at 0x0211810c. Dropping the fabricated
third argument leaves that exact instruction sequence unchanged.

The helper has no meaningful return value. Its actual SetAnim callee is defined
as void in `src/_ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj.c`; the helper stores the
animation index after the call and returns without defining another result.
The corrected bridge uses that definition's scalar parameter contract. The ROM
reads its fifth argument with `ldrh` at 0x02016780. `ModelAnim.h` still declares a
by-value Fix12 speed and u32 start frame. Reconciliation of that shared-header
boundary is outside this task; no claim that both source declarations agree is made.

`func_ov062_02118058` changes state when the tracked actor is near and within its
facing cone. Neither the distance nor angle checks are used as a result by any
recorded caller. All four incoming ov062 relocation sites are:

| Caller and enrolled source | Call site(s) | Following instructions |
| --- | --- | --- |
| `func_ov062_021183e0`, `src/game/actors/d_a_nknk.cpp` (formerly `func_ov062_021183e0.c`) | 0x0211856c | restore registers; return |
| `func_ov062_02118cdc`, `src/game/actors/d_a_nknk.cpp` (formerly `func_ov062_02118cdc.c`) | 0x02118dd0 | restore stack/registers; return |
| `func_ov062_02118de8`, `src/game/actors/d_a_nknk.cpp` (formerly `func_ov062_02118de8.cpp`) | 0x02118e64, 0x02118ef0 | restore registers; return |

The source search and all module-qualified config relocation records agree on
these three callers/four calls. Each caller is itself void and discards the
call result. At the pinned experiment input, the production shards still declared the old int
return and retained their old definition: that stage changed only the shadow TU.
The later owned promotion retires those superseded shards and declarations.
Six unchanged production controls also strictly verify: the three callers,
`func_ov062_02118058`, `func_ov062_021180d4`, and ModelAnim::SetAnim.

## Reproduction and limits

Local ignored artifacts are under `build/koopa2864-probes/`: cumulative candidate
sources and objects, `results.json`, `final-check.json`, `contract-rom-disassembly.txt`,
and original/final scratch manifest verification logs. Driver scripts are retained
in the coordinator's ignored `build/reviewer-0919/` directory as
`koopa2864-probes.py` and `koopa2864-final-check.py`.

An independent check can compile with `match.compile_c(source, '2004/b56',
build_pin.flags_for(source))`, then iterate the unchanged manifest's `functions`:
call `objisolate.derive(object, symbol)` and
`linkcheck.linkcheck(symbol, int(address, 16), int(size, 16), 'ov062',
reloc_audit.build_name_index(), obj=derived, sym=symbol)`.
Per-function isolation is a diagnostic, not permission to discard metadata in a
promoted build.

For complete-output accounting, copy the manifest entry into a scratch JSON
document with `schema_version: 1` and an `entries` list, then run:

```text
python tools/tubuild.py --manifest build/koopa2864-probes/manifest.json verify ov062/Koopa+KoopaSmall
```

Both the original and repaired source return exit 1: 39/39 text matches, clean
relocation destinations, but 13 unlicensed section/symbol records, missing policies
for `_ZN5KoopaD2Ev` and `_ZN7Vector3D1Ev`, and destructor ordinal pair (0,1) outside
ROM order. The coined Koopa RTTI still differs from retail `daNknk_c`. No policy
was added, no metadata was silently licensed, and the tracked manifest is unchanged.
