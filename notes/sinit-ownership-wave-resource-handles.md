# Static-initializer ownership wave: resource-handle lane

This lane independently audits three rows ranked `high` by
`tools/sinit_owners.py`. It is an ownership experiment only. No production
`src/`, linker configuration, symbols, or TU manifest is changed.

## Summary

| Initializer | Candidate owner | Ownership verdict | Regeneration readiness |
| --- | --- | --- | --- |
| `__sinit_ov009_02112458` | `ov009/Bird` | **CONFIRMED** | Not promotion-ready: resource-family symbol names, four original PMF member names, and unlicensed vtable/RTTI output remain |
| `__sinit_ov015_02112f9c` | `ov015/PoleBillboard` | **CONFIRMED** | Structurally straightforward, but no shadow manifest exists and vtable/RTTI/data ranges are unlicensed |
| `__sinit_ov002_02101064` | `ov002/daObjMarioCap_c` | **CONFIRMED** | Not promotion-ready: `InitResources` is not a matching source, the PMF member names remain unknown, and ov002 cannot be ordinal-partitioned |

All three committed initializer transcriptions pass strict matching and linked
verification with `blind: 0`. More importantly, ordinary static C++ objects and
non-const PMF arrays in the guarded probes reproduce the complete initializer
instruction stream, size, relocation offsets, relocation types, and addends.
The probes contain no hand-written `__sinit` function.

The probe class names are deliberately neutral stand-ins. Their C1/D1 symbol
spellings therefore differ from the ROM's still-partly-unnamed resource veneers.
The one-to-one semantic mapping is:

- probe model constructor/destructor -> `func_02017acc` / `func_02017ab4`;
- probe animation constructor/destructor ->
  `_ZN13SharedFilePtr9ConstructEj` / `SharedFilePtr_Destruct_Anim`;
- compiler `__register_global_object` -> `func_020731dc`;
- probe object, registration-node, and PMF symbols -> the same-address ROM
  globals listed below.

Thus the organic objects reproduce the exact relocation topology and raw object
bytes. The already-matched C transcriptions, using the ROM spellings, separately
prove every final relocation destination by linked comparison.

## Claims and isolation

The audit ran in the wired worktree
`C:\tmp\sm64ds-sinit-wave-resources`, branch
`research/sinit-wave-resources`, based on `7b26fa34f`. The compiler canary passed
under `mwccarm 2004/b56`.

`CLAIMS.md` has only historical `done`/`released` entries relevant to Bird,
PoleBillboard, and daObjMarioCap_c; no active span conflicts with this read-only
audit. A claims key is absent, so this experiment could not be announced through
the claims service. It did not claim or alter any production span.

## 1. `ov009/Bird`

### Ownership edges

The candidate text TU is the high-boundary-confidence span
`0x021111a0..0x02111a70` with 13 functions. Both constructed globals have no
unmapped or external consumer:

| Ownership global | Size/type | Non-initializer consumers |
| --- | --- | --- |
| `data_ov009_02113c20` | 8-byte model resource handle | `Bird::InitResources`, `Bird::CleanupResources` |
| `data_ov009_02113c28` | 8-byte animation resource handle | `Bird::InitResources`, `Bird::CleanupResources` |

The initializer constructs asset 1080 with `func_02017acc`, registers
`func_02017ab4` and node `data_ov009_02113c30`, then constructs asset 1081 with
`SharedFilePtr::Construct`, registers `SharedFilePtr_Destruct_Anim` and node
`data_ov009_02113c3c`.

It also initializes the 32-byte PMF table `data_ov009_02113c48`. Every source
descriptor is 8 bytes with one zero-addend code relocation, and every target lies
inside the Bird span:

| Descriptor | Target |
| --- | --- |
| `0x02113914` | `func_ov009_021116ec` (`0x021116ec`) |
| `0x0211390c` | `func_ov009_021115d8` (`0x021115d8`) |
| `0x021138fc` | `func_ov009_0211145c` (`0x0211145c`) |
| `0x02113904` | `func_ov009_02111234` (`0x02111234`) |

The ROM BSS contribution `0x02113c20..0x02113c68` is exactly two 8-byte
handles, two 12-byte registration nodes, and one 32-byte PMF table. The four PMF
descriptors occupy exactly `0x021138fc..0x0211391c` (32 data bytes).

### Order and organic generation

This is initializer ordinal 0 of 4. `.p__sinit_ov009_02112458` at
`0x02112b34` points to it; the next word points to
`__sinit_ov009_02112524`, independently owned by CastleWater. The parallel
TU-shaped config attributes Bird's exact `.text`, `.init`, `.ctor`, data, and
BSS intervals and keeps them ordered before CastleWater.

With `SINIT_OWNERSHIP_PROBE`, `notes/sinit-probes/bird.cpp` organically emits:

- `.init`: `0xcc` bytes, raw-byte identical to the matched transcription;
- `.rela.init`: all 15 offsets identical, with four type-1/addend-`-8` calls
  and eleven type-2/addend-0 words;
- `.ctor`: one 4-byte, zero-addend pointer to the generated initializer;
- `.data`: four independent 8-byte PMF descriptors;
- `.bss`: 8 + 8 + 12 + 12 + 32 bytes;
- no `.exceptix` under `-Cpp_exceptions off`.

The existing Bird shadow TU remains 13/13 text-matched, objisolate-clean,
relocation-destination-clean, and 13/13 contribution-equivalent. Promotion is
still refused for its existing compiler-only D2/helper outputs and vtable/RTTI
side effects; one legacy entry also lacks `complete` despite its carried asm hatch
matching in the shadow build.

## 2. `ov015/PoleBillboard`

### Ownership edges

The candidate text TU is the high-boundary-confidence span
`0x021111a0..0x02111314` with 7 functions. Its sole directional global has no
unmapped or external consumer:

| Ownership global | Size/type | Non-initializer consumers |
| --- | --- | --- |
| `data_ov015_02114960` | 8-byte model resource handle | `PoleBillboard::InitResources`, `PoleBillboard::CleanupResources` |

The initializer constructs asset 1416 with `func_02017acc` and registers
`func_02017ab4` through node `data_ov015_02114968`. The exact ROM BSS interval
`0x02114960..0x02114974` is one 8-byte handle plus one 12-byte node.

### Order and organic generation

This is initializer ordinal 0 of 7. `.p__sinit_ov015_02112f9c` at
`0x02113410` points to it; the next word points to
`__sinit_ov015_02112fdc`, independently owned by KnockDownPlank. The parallel
TU config attributes PoleBillboard's `.init` (`0x02112f9c..0x02112fdc`),
`.ctor` word, and exact BSS interval to its first text TU.

With `SINIT_OWNERSHIP_PROBE`, `notes/sinit-probes/pole_billboard.cpp`
organically emits:

- `.init`: `0x40` bytes, all 64 bytes identical to the matched transcription;
- `.rela.init`: the identical five offsets/types/addends (two
  type-1/addend-`-8` calls, three type-2/addend-0 words);
- `.ctor`: one 4-byte pointer;
- `.bss`: one 8-byte object and one 12-byte registration node;
- no data or exception output.

All seven existing text functions verify under the compiler pin. Regeneration is
not yet a production claim because no shadow TU/manifest exists and the expected
class vtable/RTTI/data output has not been licensed.

## 3. `ov002/daObjMarioCap_c`

### Ownership edges

The candidate text span is `0x020b6f18..0x020b8bf0` with 31 functions and high
boundaries on both sides. All 22 directional globals resolve only to consumers
inside that span; there are no unmapped or external consumers.

The thirteen 8-byte animation handles are:

| Global | Asset ID in source order | Consumers beyond `InitResources` / `CleanupResources` |
| --- | ---: | --- |
| `data_ov002_0210de30` | `0x8012` | `OnTurnIntoEgg`, `func_020b7330`, `func_020b74d0` |
| `data_ov002_0210de38` | `0x8013` | `func_020b74d0` |
| `data_ov002_0210de50` | `0x480` | none |
| `data_ov002_0210de60` | `0x47e` | none |
| `data_ov002_0210de48` | `0x47f` | none |
| `data_ov002_0210de28` | `0x478` | none |
| `data_ov002_0210de08` | `0x476` | none |
| `data_ov002_0210de20` | `0x47c` | none |
| `data_ov002_0210de40` | `0x479` | none |
| `data_ov002_0210de10` | `0x477` | none |
| `data_ov002_0210de00` | `0x47d` | none |
| `data_ov002_0210de58` | `0x47b` | `func_020b7330` |
| `data_ov002_0210de18` | `0x47a` | `func_020b7330` |

Each is constructed through `_ZN13SharedFilePtr9ConstructEj`, registered through
`func_020731dc`, and paired with `SharedFilePtr_Destruct_Anim`. Its matching
12-byte registration node is, in the same source order:
`data_ov002_0210dee0`, `...deec`, `...def8`, `...de68`, `...de74`,
`...de80`, `...de8c`, `...de98`, `...dea4`, `...deb0`, `...debc`,
`...dec8`, and `...ded4`.

The remaining nine ownership globals are 16-byte, two-entry PMF arrays:

| Global | Consumers beyond `InitResources` |
| --- | --- |
| `data_ov002_0210df04` | none |
| `data_ov002_0210df14` | none |
| `data_ov002_0210df24` | none |
| `data_ov002_0210df34` | none |
| `data_ov002_0210df44` | `func_020b7f7c` |
| `data_ov002_0210df54` | `OnTurnIntoEgg`, `Render`, `func_020b6fcc`, `func_020b7f7c` |
| `data_ov002_0210df64` | none |
| `data_ov002_0210df74` | `func_020b7f7c` |
| `data_ov002_0210df84` | none |

All eighteen source PMF descriptors at `0x0210951c..0x021095ac` relocate with
zero addends to functions inside the MarioCap span:

```text
0951c->020b7cdc  09524->020b7e08  0952c->020b7e1c
09534->020b7f24  0953c->020b71e8  09544->020b7d6c
0954c->020b74d0  09554->020b7200  0955c->020b7d58
09564->020b7cec  0956c->020b7b70  09574->020b781c
0957c->020b76ec  09584->020b71f0  0958c->020b7d9c
09594->020b7330  0959c->020b7d94  095a4->020b7c30
```

The target data/BSS shape is exact and contiguous:

- 18 PMF descriptors: `0x0210951c..0x021095ac`, 144 bytes;
- 13 handles: `0x0210de00..0x0210de68`, 104 bytes;
- 13 registration nodes: `0x0210de68..0x0210df04`, 156 bytes;
- 9 PMF arrays: `0x0210df04..0x0210df94`, 144 bytes.

### Order and organic generation

This is initializer ordinal 8 of 26. Its `.ctor` word at `0x021080f0` sits
between the words for `__sinit_ov002_02100f84` (daObjKurumajiku_c) and
`__sinit_ov002_02101478` (PushBlock). This ordering is corroborating only:
ov002 has 26 initializers but 71 candidate text TUs, so the ordinal partition
correctly refuses to attribute `.init`, `.ctor`, data, or BSS. Several TUs that
need no static initializer occur between those named owners.

With `SINIT_OWNERSHIP_PROBE`, `notes/sinit-probes/mario_cap.cpp` organically
emits:

- `.init`: `0x414` bytes, all 1,044 raw bytes identical;
- `.rela.init`: all 80 offsets/types/addends identical (26
  type-1/addend-`-8` calls and 54 type-2/addend-0 words);
- `.ctor`: one 4-byte initializer pointer;
- `.data`: eighteen 8-byte PMF descriptors (144 bytes);
- `.bss`: thirteen 8-byte handles, thirteen 12-byte nodes, and nine 16-byte
  PMF arrays (404 bytes total);
- no exception output.

This is strong ownership confirmation despite the module-wide ordinal gate
remaining unavailable. It is not promotion readiness: the current
`daObjMarioCap_c::InitResources` source fails the pin check with 999 differing
words and lacks `complete`; no merged shadow manifest exists; the eighteen
original PMF member names remain address-derived; and the TU would emit
unlicensed vtable/RTTI data.

## Verification record

The committed initializer sources were checked with explicit module/address/size
parameters:

```powershell
python tools/match.py --c src/__sinit_ov009_02112458.c --func __sinit_ov009_02112458 --addr 0x02112458 --size 0xcc --version 2004/b56 --module ov009 --strict-relocs --brief
python tools/linkcheck.py --c src/__sinit_ov009_02112458.c --name __sinit_ov009_02112458 --addr 0x02112458 --size 0xcc --module ov009

python tools/match.py --c src/__sinit_ov015_02112f9c.c --func __sinit_ov015_02112f9c --addr 0x02112f9c --size 0x40 --version 2004/b56 --module ov015 --strict-relocs --brief
python tools/linkcheck.py --c src/__sinit_ov015_02112f9c.c --name __sinit_ov015_02112f9c --addr 0x02112f9c --size 0x40 --module ov015

python tools/match.py --c src/__sinit_ov002_02101064.c --func __sinit_ov002_02101064 --addr 0x02101064 --size 0x414 --version 2004/b56 --module ov002 --strict-relocs --brief
python tools/linkcheck.py --c src/__sinit_ov002_02101064.c --name __sinit_ov002_02101064 --addr 0x02101064 --size 0x414 --module ov002
```

Observed: all three `MATCH`; all three `VERIFIED`, `blind: 0`.

The probes were compiled with pinned production C++ flags plus
`-DSINIT_OWNERSHIP_PROBE`. Comparing the generated initializer against each
compiled transcription with `tools.match.extract_func` produced:

```text
PoleBillboard  size 64/64     raw_equal=True  reloc_topology_equal=True  5 relocs
Bird           size 204/204   raw_equal=True  reloc_topology_equal=True 15 relocs
MarioCap       size 1044/1044 raw_equal=True  reloc_topology_equal=True 80 relocs
```

Additional scoped gates:

- `python tools/tu_config.py --module ov009 --module ov015 --module ov002 ... --report`:
  Bird and PoleBillboard receive ordered full-section intervals; ov002 correctly
  refuses the invalid 26-to-71 ordinal partition.
- `python tools/tubuild.py verify ov009/Bird`: 13/13 MATCH, objisolate clean,
  relocation destinations clean; promotion refused for pre-existing unlicensed
  output.
- `python tools/tubuild.py partial ov009/Bird`: 13/13 contribution-equivalent.
- `python tools/tu_map.py --check`: all invariants pass.
- `python tools/port_refcheck.py`: 407 references checked, all resolve.
- `python -m unittest tools.test_sinit_owners tools.test_tu_config`: 17 tests pass.
- compiling all three probes without `SINIT_OWNERSHIP_PROBE`: no `.text`,
  `.init`, `.ctor`, `.data`, or `.bss` content is emitted.
- `python tools/eligible.py` followed by `python tools/check_references.py`:
  11,069/11,189 eligible and no new unresolved references.
- `python tools/langmode_audit.py --check langmode-baseline.json`: ratchet pass.
- `git diff --check`: pass.

The ignored build products are not evidence intended for commit; the guarded
probe sources are the reproducible recipe.
