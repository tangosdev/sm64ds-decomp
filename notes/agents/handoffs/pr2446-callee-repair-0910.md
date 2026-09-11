# PR #2446 callee identity repair

Task `pr2446-callee-repair-0910` continues `pr-2446-source-review-fixes`
from PR head `d979cf2f9ad042d536ee8de1d7ee4691df01e6c3`. The preserved source
base is `71f7adf39168b5a4a296203012cbb21d948bd122`. Producer session
`review-2446-0910-bkg07` owns this repair; a different session must independently
review and verify this commit. Queue operations used the current coordinator
checkout, observed at `d00b5e8497a9224b597ee59b12fa9c0c4759b6d8`.

## Correction

**BKG-07 (provenance):** two BombKing state handlers named the unrelated
`_ZN13MontyMoleRockD0Ev` destructor as their callee. That symbol's only configured
home is **ov080:0x02123804**, size 0x54. Both ROM call records instead identify
**ov078:0x02123804**, size 0x60, the local `func_ov078_02123804` helper:

| Caller | Call address | ROM target | Corrected emitted symbol |
| --- | --- | --- | --- |
| `func_ov078_02123aa0` | `0x02123ac0` | `overlay(78):0x02123804` | `func_ov078_02123804` |
| `func_ov078_02123c20` | `0x02123c28` | `overlay(78):0x02123804` | `func_ov078_02123804` |

The evidence is in `config/arm9/overlays/ov078/relocs.txt` and the ov078/ov080
symbol tables. Both calls now use the helper already defined earlier in this TU,
and the unused `decl_MontyMoleRock.h` include is removed. No symbol or relocation
configuration is changed. The helper keeps its existing neutral name.

The previous object emitted the foreign destructor symbol at caller offsets
+0x20 and +0x8. The corrected object emits the local helper at the same offsets.
Both names resolve numerically to 0x02123804, so the prior byte and relocation
checks passed: the gates deliberately accept same-address aliases. This repair
corrects the source's callee identity; it does not claim a changed runtime branch
destination or a newly discovered byte mismatch.

## Earlier findings retained

The complete inherited review and measurements remain in
`notes/agents/handoffs/pr-2446-source-review-fixes.md` and its queue record.
The current source was inspected in full before this repair:

- **BKG-01:** the shadow helper and common declaration consistently return void.
- **BKG-02:** production owns text only: 52 functions, seven native definitions
  emitting eight method symbols, and 44 free functions including the factory.
- **BKG-03:** the previous correction distinguishes reconstructed names and
  current explanations from historical source/compiler experiments.
- **BKG-04:** class facts distinguish the 0x7c vtable payload from 0x84 storage;
  partial metadata coverage remains partial.
- **BKG-05:** Behavior calls the real `BlendModelAnim::UpdateVerts` and `Advance`;
  Render preserves the direct qualified `Model::Render` call.
- **BKG-06:** the state dispatcher uses the actual class as its member-pointer
  receiver. Its current int result does not prove every original handler
  signature. Raw actor/player fields, matrix views, ABI helpers and manual
  construction remain accepted partial reconstruction under open issue #2479,
  with humanizer follow-through owned by @andrewboudreau. This repair does not
  complete that larger reconstruction.
- **BKG-07:** the two false foreign-destructor identities are corrected here;
  the independent reviewer must confirm the source and emitted local targets.

## Local proof

Pinned compiler: **2004/b56**, verified by the wired-worktree canary. No header,
manifest, enrollment, metadata-ownership or attribution entries were edited.

- `python tools/tubuild.py --manifest build/review-manifest.d verify
  ov078/daBombking_c`: **52/52 MATCH**, clean object isolation and relocation
  destinations, all 52 functions in ROM-ascending emission order. The manifest
  was copied to ignored build storage before verification so historical records
  remain unchanged.
- The newly compiled TU object was isolated per manifest function and passed to
  `tools/linkcheck.py`'s `linkcheck` API: **52 VERIFIED**, **12,064 bytes**,
  **zero blind words and zero differences**. Both corrected relocation symbols
  were additionally checked against the module-qualified ROM call records above.
- Before/after raw-object comparison: all 52 function byte arrays are unchanged.
  The only function-relocation changes are the two callee symbol spellings above;
  offsets, relocation types, addends and numeric targets are unchanged.
- `python tools/romdata_check.py --files src/actors/daBombking_c.cpp`: five
  VERIFIED records (56 bytes), six PARTIAL records (172 equal bytes), zero
  differing or unnamed records. Production still uses ROM-backed class metadata.
- `python tools/port_refcheck.py`: **423 references checked, zero stale**.
- `python tools/rombuild.py -j 8 --no-rom`: **PASS, 106/106 modules exact**,
  11,193 source functions reproducing and zero mismatching. All 25 source-owned
  data claims reproduce; three BSS claims cover 172 NOBITS bytes. The broader
  metadata census retains 705 verified records, 224 partial, four differing and
  387 unnamed records; these inherited limits are not upgraded to passes.
  The packed cartridge was not rebuilt because this run used `--no-rom`.

Private build logs are `build/bkg-after-tu.txt`, `build/bkg-after-linkcheck.json`,
`build/bkg-romdata.json`, `build/bkg-port.txt` and `build/bkg-rombuild.txt` in
`C:/tmp/sm64ds-r2446-0910`. These are local artifacts; the committed findings and
queue evidence carry the durable summary. The independent verifier should rerun
required checks rather than treating the producer's report as source acceptance.

The integrator must compose this repair with current main, reconcile the global
rename ledger in its owned lane, obtain independent review of that exact
composition/base, and wait for terminal private validation before merging.
