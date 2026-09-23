# Native-destructor declaration checker repair

This commit applies the explicitly approved patch `a44b2a93f02d5e472aee9498fdbdbdc22c689782e345bc8330c3b3c1343219c0` to the checker and its tests, on main `27ca0e1f322b9cdc538c6954c36ddba44066c683`. The scoped exception preserves the existing `pr-2471-source-review-fixes` branch and reservation. The producer is `codex-native-destructor-producer-0922`; independent verification follows this commit. No baseline, source, header, compiler or ROM input is changed.

The checker recognizes native qualified destructors only when their owner exactly matches the marked direct D0/D1/D2 identity. It keeps source return syntax distinct from the direct Arm ABI result and retains the normal implicit-receiver policy. Constructors, thunks, wrappers and ambiguous identities are not inferred. A missing or scalar receiver still produces a real finding.

The actual standalone suite passes **164 tests, zero skips**. The earlier 173-test result was the separate local composition with overload PR #2907; that patch is not included here. The exact current-base diagnostic population is 8,626 files, 38,195 declarations, 10,635 definitions and two unparsed declarators. Rows change from 16,547 to 16,461: 100 keys removed, 15 introduced missing-receiver keys, and 128 reference rows revised. Every changed symbol is a direct destructor identity. All other rows remain unchanged.

## Publication and remaining source debt

Publish as a draft. The whole-tree gate changes from zero to **15 unbanked findings** because it now recognizes the real implicit destructor receiver. These are existing source declarations requiring separate byte-proven repairs, not new baseline exceptions. The PR's tool-only changed scope does not exercise them; a green scoped check is not evidence that the whole-tree gate passes. Next owner: `codex-integrator-0920`. This PR must not be treated as ready to merge until the source dependencies are accounted for and the resulting whole-tree gate passes.

| Destructor | Existing declaration requiring a receiver |
| --- | --- |
| `_ZN10FaderColorD1Ev` | `src/__sinit_02073e6c.c` |
| `_ZN10FaderColorD1Ev` | `src/__sinit_02074edc.c` |
| `_ZN10dScEntry_c6icon_cD1Ev` | `src/d_s_entry_entry.cpp` |
| `_ZN10dScEntry_c6icon_cD1Ev` | `src/d_s_entry_result.cpp` |
| `_ZN11ShadowModelD1Ev` | `src/d_a_wanwan2.cpp` |
| `_ZN11ShadowModelD1Ev` | `src/game/actors/d_a_wanwan.cpp` |
| `_ZN12OamAnimationD1Ev` | `src/d_s_entry_entry.cpp` |
| `_ZN12OamAnimationD1Ev` | `src/d_s_entry_result.cpp` |
| `_ZN15FaderBrightnessD1Ev` | `src/__sinit_02074edc.c` |
| `_ZN15UnknownVsPlayerD1Ev` | `src/d_ent_obj.cpp` |
| `_ZN16dMgJump3DMario_cD1Ev` | `src/d_s_mg_jump.cpp` |
| `_ZN16dMgJump3DMario_cD1Ev` | `src/minigames/d_s_mg_jump2.cpp` |
| `_ZN5ModelD1Ev` | `src/d_a_wanwan2.cpp` |
| `_ZN5ModelD1Ev` | `src/game/actors/d_a_wanwan.cpp` |
| `_ZN7ClipperD1Ev` | `src/__sinit_02074e84.cpp` |

The local Cup follow-up already measures the two Jump declarations and Model/ShadowModel in `d_a_wanwan.cpp`; those source repairs are not part of this tooling PR and have not landed. No complete class reconstruction, source acceptance, or ROM proof is asserted by this tooling-only result.

## Resolution (2026-09-22): the two halves cannot be sequenced

The fifteen source declarations above are repaired in this branch. Each one
now takes the receiver the Arm C++ ABI passes a D1/D2 entry, spelled the way
its own file already spells the sibling constructor in the same `extern "C"`
block.

**The plan above -- land the source repairs as a separate PR, then this tool
change -- is not viable, and neither is the reverse.** The two halves are
mutually dependent:

| | main's parser | this branch's parser |
| --- | --- | --- |
| main's source | exits 0 | exits 1, fifteen `arity` findings |
| repaired source | exits 1, fifteen `arity` findings | **exits 0** |

Main's parser reads the *definition* as arity 0, so a correct declaration
contradicts it in the mirror direction (`got 1, expected 0`) on exactly the
same fifteen keys. Whichever half lands first leaves main red until the other
follows. They belong in one commit range, which is why the repairs are on this
branch rather than a separate one.

### Why the arity change is byte-neutral

None of the fifteen symbols has a call site. Each is referenced exactly once,
as an address passed to `__cxa_vec_ctor` or to the static-destructor registrar
`func_020731dc`, and thirteen of those references were already cast to
`(void *)`. An address-of produces the same relocation whatever the declared
parameter list, so there is no argument setup to re-select.

`src/__sinit_02074e84.cpp` is the one exception: it declares `func_020731dc`
locally with a typed `void (*dtor)(void)` parameter, which a receiver-taking
destructor no longer converts to. That registrar is *defined* in-tree
(`src/func_020731dc.c`) as `(int, int, void **)`, so the local declaration is
already a banked disagreement -- re-spelling it would re-key a banked finding
into a new one and fail the ratchet. The call site takes a cast and the
registrar declaration is left exactly as it was.

### Evidence

- `rombuild.py -j16 --no-rom`: 7,812 enrolled sources compiled, 11,209
  functions reproducing, **0 mismatching**, 106/106 modules exact,
  100.000000% of compared bytes, ROM-build analysis PASS.
- `prepush_linkcheck.py --range origin/main..HEAD`: 66 checked, 66 verified,
  0 warnings, **0 blocking** -- no WRONG-DEST.
- `check_decl_agreement.py`: exits **0**, no new disagreements, against exit 1
  with fifteen before. No baseline entry added; 103 banked rows now resolve on
  their own and remain banked.
- `test_check_decl_agreement.py`: 173 tests, 0 failures, 0 skips.

No follow-up owner is required for the fifteen. The unpublished local Cup work
that measured four of them is not on any remote branch and is not needed.
