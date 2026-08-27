# Static-initializer ownership wave A

Research base: `7b26fa34f`; compiler: `mwccarm 2004/b56`; worktree:
`C:\tmp\sm64ds-sinit-wave-a`.  This note audits ownership only.  It does not
authorize moving any production source.

The claims service was unavailable locally (`tools/claims_key.txt` and
`CLAIMS_API_KEY` were both absent).  `CLAIMS.md` had no active span for the
three owners (the OneUpMushroom row is already `done`), so this lane made no
claim and changed no production source, delinks, symbol file, or manifest.

## Verdict summary

| initializer | ownership | regeneration readiness |
|---|---|---|
| `__sinit_ov002_02100adc` | **CONFIRMED: `ov002/OneUpMushroom`** | Exact organic proof: a natural non-const 14-entry PMF array emits a raw-identical `0x174` `.init`, the same 15 relocation word offsets/types/addends, 14 8-byte source descriptors, a `0x70` BSS destination, and one 4-byte `.ctor`. Production still needs names/signatures for the 14 anonymous state methods and TU-level link placement. |
| `__sinit_ov002_02100f84` | **CONFIRMED: `ov002/daObjKurumajiku_c`** | Partial organic proof: four separate vector objects with an inline three-component constructor and destructor emit the exact `0xe0` size, exact 13 relocation word offsets/types/addends, four 12-byte objects, four 12-byte registration nodes, one 4-byte destructor and one 4-byte `.ctor`. Sixteen non-relocation words differ because of store/register scheduling, so regeneration is **not byte-ready**. |
| `__sinit_ov063_0211e5fc` | **CONFIRMED: `ov063/MadPiano`** | Exact organic proof: three distinct 8-byte resource objects plus a 2x2 PMF table emit a raw-identical `0x100` `.init`, the same 20 relocation word offsets/types/addends, three 12-byte registration nodes, four 8-byte descriptors, a 32-byte BSS table and one 4-byte `.ctor`. Production still needs the real special-member type declarations/names and TU-level link placement. |

The ownership verdicts stay confirmed even where regeneration is incomplete:
ownership is established by exclusive consumers and TU boundaries; source
reconstruction is a separate publication gate.

## 1. OneUpMushroom (`ov002:3`)

`__sinit_ov002_02100adc` copies fourteen 8-byte PMF descriptors into
`data_ov002_0210dc00` (`0x70` bytes).  Its only ROM consumer relocation is
`0x020b01bc -> 0x0210dc00`, in `OneUpMushroom::Behavior`; the only source
consumer is `src/_ZN13OneUpMushroom8BehaviorEv.cpp`.  There are no unmapped or
external consumers.

The descriptor copy order and relocated method targets are:

| index | descriptor | target |
|---:|---:|---:|
| 0 | `0x02108350` | `0x020aff10` |
| 1 | `0x02108330` | `0x020afe4c` |
| 2 | `0x02108348` | `0x020afd10` |
| 3 | `0x02108358` | `0x020afc44` |
| 4 | `0x02108338` | `0x020afbb4` |
| 5 | `0x02108310` | `0x020afa98` |
| 6 | `0x02108328` | `0x020afa6c` |
| 7 | `0x02108320` | `0x020af950` |
| 8 | `0x02108308` | `0x020af924` |
| 9 | `0x02108340` | `0x020af838` |
| 10 | `0x02108318` | `0x020af7cc` |
| 11 | `0x02108300` | `0x020afa50` |
| 12 | `0x02108368` | `0x020af908` |
| 13 | `0x02108360` | `0x020af724` |

Every target is inside the inferred OneUpMushroom TU
`0x020aee40..0x020b05d0` (38 functions, high-confidence boundaries).  The TU
contains the class D1/D0 destructors, vtable-related methods, resource methods,
Behavior and both spawn functions; no function hole was found.

The ROM `.ctor` entry is `.p__sinit_ov002_02100adc` at `0x021080d8`, between
`dEnemyBase_c` (`0x021080d4`) and the medium-confidence Coin initializer
(`0x021080dc`).  ov002 has 26 initializers/26 ctor entries but 71 inferred TUs,
so order is only a neighbor check here, not the ownership proof.

Natural probe result:

```
probe .init size 0x174; hand source size 0x174; raw_equal=True
relocation words (both):
  138 13c 140 144 148 14c 150 154 158 15c 160 164 168 16c 170
all are type 2 (R_ARM_ABS32), addend 0
```

The compiler emits the descriptors in individual 8-byte `.data` sections; their
method relocations are type 2/addend 0.  The generated initializer loads them in
source order (`@2`, destination, `@4` ... `@28`), exactly corresponding to the
ROM descriptor order above.

## 2. daObjKurumajiku_c (`ov002:28`)

The initializer constructs four contiguous 12-byte vector objects and registers
the empty `Vector3::~Vector3` (`0x020072c0`) separately for each:

| object | components `(x,y,z)` | registration node |
|---|---|---|
| `0x0210ddd0` | `(0, +0x190000, 0x12c000)` | `0x0210ddac` |
| `0x0210dddc` | `(0, -0x190000, 0x12c000)` | `0x0210ddb8` |
| `0x0210dde8` | `(+0x190000, 0, 0x12c000)` | `0x0210ddc4` |
| `0x0210ddf4` | `(-0x190000, 0, 0x12c000)` | `0x0210dda0` |

The four registration calls are at `0x02100fb0`, `0x02100fdc`, `0x02101004`
and `0x02101030`, all targeting `func_020731dc`.  The pool relocation order is
`ddd0, Vector3D1, ddac, dddc, ddb8, dde8, ddc4, ddf4, dda0` at initializer
offsets `0xbc..0xdc`.  Four direct destructor registrations prove four separate
objects; an array would require an array-destruction path.

The only consumer relocation is `0x020b6c4c -> 0x0210ddd0`, in
`daObjKurumajiku_c::Behavior`, whose source indexes the contiguous storage as
four `Vector3` values.  No other source consumes any of the eight object/node
symbols.  The inferred TU is `0x020b69e4..0x020b6c54` (six functions, medium
boundaries), containing D0/D1, Behavior, Render and two anonymous functions;
no function hole was found.

The `.ctor` word is at `0x021080ec`, between high-confidence QuestionSwitch
(`0x021080e8`) and daObjMarioCap_c (`0x021080f0`) entries.

An aggregate `Vector3 v = {...}` was rejected as a generator: it places the
components directly in `.data` and emits only a `0x78` registration initializer.
Four separate objects of a 12-byte probe type with an inline three-argument
constructor instead emit the correct runtime stores and inventory:

```
probe .init size 0xe0; hand source size 0xe0; raw_equal=False
relocation words (both): 2c 58 80 ac bc c0 c4 c8 cc d0 d4 d8 dc
calls: type 1 (R_ARM_CALL), addend -8 at 2c/58/80/ac
pool: type 2 (R_ARM_ABS32), addend 0 at bc..dc
```

Sixteen non-relocation words differ (`38 3c 40 4c 50 60 64 68 74 78 8c 90
94 9c a0 a4`).  They are register choice/store scheduling in objects 1-3; object
0, all calls, all literal positions, size and section inventory agree.  This is
a concrete compiler/source-spelling blocker, not evidence against ownership.
The current shared `Vector3` type has the destructor but no recovered
three-component constructor, so production regeneration must first recover that
constructor spelling without changing unrelated consumers.

## 3. MadPiano (`ov063:3`)

The first half constructs and registers three consecutive 8-byte resource
handles:

| object | asset | constructor target | destructor target | node |
|---|---:|---:|---:|---:|
| model `0x0211ef80` | `0x40a` | `0x02017acc` | `0x02017ab4` | `0x0211ef98` |
| collision `0x0211ef88` | `0x40b` | `0x02017b4c` | `0x02017b34` | `0x0211efa4` |
| attack animation `0x0211ef90` | `0x40c` | `0x0201799c` | `0x02017984` | `0x0211efb0` |

Constructor calls occur at initializer offsets `0x10/0x2c/0x48`; registration
calls at `0x20/0x3c/0x58`.  Calls are type 1/addend -8.  The pool entries at
`0xbc..0xe8` are object/destructor/node triples in that same order and are type
2/addend 0.  The current parser does not classify the three destructor veneers
as C++ destructors, but their placement as the second argument to three
`func_020731dc` calls is direct registration evidence.

The second half copies four PMFs into `data_ov063_0211efbc` in this order:

| destination slot | descriptor | target |
|---:|---:|---:|
| 0 | `0x0211ecd8` | `0x0211dd78` |
| 1 | `0x0211ece8` | `0x0211dbb8` |
| 2 | `0x0211ece0` | `0x0211dba4` |
| 3 | `0x0211ecf0` | `0x0211d8cc` |

All four targets are in MadPiano's TU.  The two table consumers are
`0x0211ddf0` and `0x0211de38`, in
`src/unnamed/ov063/func_ov063_0211ddac.cpp` and `...0211ddf4.cpp`; they expose
the destination as two entries of two PMFs.  All resource consumers also remain
inside MadPiano: CleanupResources consumes all three handles and InitResources
loads the model/animation/collision handles.  No external or unmapped consumer
was found.

The inferred TU is `0x0211d4b8..0x0211e1c0` (17 functions, high boundaries),
containing D0/D1, all four PMF targets, both dispatchers, Behavior, Render,
resource methods and spawn; no function hole was found.  ov063 has exactly four
inferred TUs, four initializers and four ctor entries, providing independent
order corroboration.  MadPiano's final `.ctor` word is `0x0211e708`, after
BigBooIcon+BooCage+daTrs_c (`e6fc`), MansionSteps (`e700`) and FallBlockBbh
(`e704`).

Natural probe result:

```
probe .init size 0x100; hand source size 0x100; raw_equal=True
relocation words (both):
  10 20 2c 3c 48 58 bc c4 c8 cc d4 d8 dc e4 e8 ec f0 f4 f8 fc
```

The probe emits three 8-byte BSS resource objects, three 12-byte BSS registration
nodes, one 32-byte BSS PMF table, four 8-byte `.data` descriptors and one 4-byte
`.ctor`.  This exactly explains the ROM's contiguous `ef80..efdc` ownership
region.  Real production declarations remain blocked because `SharedFilePtr.h`
does not yet claim the 8-byte layout or constructor/destructor special members,
and the three ROM constructor/destructor veneers have distinct generic names.

## Reproduction commands and gates

The probe is `src_tu/research/sinit_wave_a_probes.cpp`; it is compile-disabled
unless exactly one `SINIT_PROBE_*` macro is supplied and is not listed by any
manifest/delinks file.  Representative compile command (repeat with
`SINIT_PROBE_KURUMAJIKU` and `SINIT_PROBE_MADPIANO`):

```powershell
$env:LM_LICENSE_FILE='1700@localhost'
tools\mwccarm\2004\b56\mwccarm.exe '-O4,p' '-enum' int '-lang' 'c++' `
  '-char' signed '-interworking' '-proc' arm946e '-gccext,on' '-msgstyle' gcc `
  '-Cpp_exceptions' off '-Iinclude' '-DSINIT_PROBE_ONEUP' -c `
  src_tu\research\sinit_wave_a_probes.cpp -o build\sinit-wave-a\oneup.o
```

Existing hand sources were compiled with the same flags except `-lang c99`.
`match.extract_func` was used to compare raw function bytes and relocation word
sets; `tubuild.elf_inventory` plus `pyelftools` recorded emitted sections,
relocation types, symbols and addends.

All three existing initializer sources passed strict match and linked-ROM gates:

```text
python tools/match.py --c <source> --func <symbol> --addr <addr> --size <size> \
  --version 2004/b56 --module <module> --brief
  -> MATCH (all three)

python tools/linkcheck.py --c <source> --name <symbol> --addr <addr> \
  --size <size> --module <module>
  -> VERIFIED, blind: 0 (all three)
```

The ownership inputs were regenerated with:

```text
python tools/rtti_extract.py --check
python tools/rtti_vtables.py
python tools/tu_map.py
python tools/sinit_owners.py --show all
```

Results: 429 RTTI records, 413 edges, zero unresolved; 454 inferred TUs; 305
initializers, including 174 high-confidence rows.  The checked-in probe and this
note do not change ROM inputs.
