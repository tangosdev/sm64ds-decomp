# Minigame family decisions (ov006) — DRAFT for coordinator review

Status: ADJUDICATED 2026-08-30, AMENDED 2026-08-31 — the coordinator's rulings
are the ADJUDICATION section at the end; the four decisions are in force for
the mini-wave and the fan-out. The AMENDMENT section (2026-08-31) records how
the decisions re-scope now that the upstream wave has promoted five class
TUs and six factory manifests to main; the REVIEW #2 FOLD-IN section (same
day, after it) folds the second outside-perspective review in — the extended
D3 drift list, the re-priced coverage, and the re-adjudicated wave order.
Prepared on branch
`pilot2-ov006-tu-merge`, 2026-08-30, against the two banked
pilot TUs (`src_tu/actors/dScMgBSC_c+dScMgBSC_c.cpp`, 19/19 text-verified;
`src_tu/actors/dMgMCarloCardObj_c+dScMgMCarlo_c+dScMgMCarlo_c.cpp` — see
notes/tu-reconstruction-pilot2-report.md), in service of the owner's goal,
verbatim: "convert the minigame code to period accurate idiomatic, readable,
c++ code the original EAD team would have written."

Four family-wide decisions, one cross-cutting tooling gap, and a mini-wave
roster. Each DECISION ends with a RECOMMENDATION line for adjudication.
Evidence is inline (addresses, relocs, file:line). Scratch tooling that
produced it lives under `build/` and is regenerable: `roster_and_siblings.py`,
`roster_details.py`, `extern_census.py`, `minigame_extern_inventory.py`,
`pragma_census.py`, `pragma_carriers.py`, `extern_probe/` (probe.cpp,
probe2.cpp and their mwccarm objects). Unit indices and spans cite
`build/tu_map.json` as regenerated this session (`python tools/tu_map.py`);
regenerate before relying on an index.

## DECISION 1 — form B destructor emission, family-wide

Form B (the banked BSC shape) writes the destructor inline in the class body.
mwccarm 2004/b56 then emits the complete (D1) and deleting (D0) destructors as
the first two `.text` functions of a class's run in the TU that defines its
key function, and emits no base-object destructor (D2) at all.

Three independent measurements say this is the family idiom, not a BSC quirk:

1. **Every sibling unit's run opens D1-then-D0.** The census below lists all
   25 dScMg* class units in ov006. Every uncontaminated one opens its run with
   `<class>D1Ev`, `<class>D0Ev`; in the contaminated ones (see the
   cross-cutting section) the D1/D0 pair opens the class-run portion
   immediately after the distant absorbed block. `grep -c D2Ev
   config/arm9/overlays/ov006/symbols.txt` is **0** — no D2 exists anywhere in
   the module.
2. **The base class already requires it.** include/dScMgSingle3DBase_c.h:89-95
   declares `virtual ~dScMgSingle3DBase_c() {}` inline with the warning:
   "MUST STAY DEFINED INLINE — all 13 children inline this body, and
   `_ZN19dScMgSingle3DBase_cD2Ev` exists nowhere in the ROM, so an
   out-of-line definition leaves every child with an undefined external.
   MEASURED on dScMgMemory_c; do not move the body out." The banked BSC TU
   follows suit and text-verified 19/19 (config/tu_manifest.d/ov006/
   dScMgBSC_c+dScMgBSC_c.json, run 0x02124908..0x021255f8, 19 functions).
3. **The 0x4f38 shared member is the base itself, and every child owns it by
   inheritance.** `sizeof(dScMgSingle3DBase_c) == 0x4f38` is a static assert
   in the header (include/dScMgSingle3DBase_c.h:134); the tail is one minigame
   camera at 0x4660..0x471c plus an embedded-by-value `Particle::SysTracker`
   at 0x471c..0x4f38. The factories prove the offset from the allocation side:
   src/MgLuckyStars_Spawn.cpp:12 allocates 0x51d0 for dScMgBSC_c (0x4f38 base
   + 0x298 own) and line 18 passes `p + 0x4f38` to func_ov006_020c1d80.
   dScMgFlower_c's first own member, `mArray`, sits exactly at 0x4f38
   (notes/minigame-provenance.md:501); dScMgSlot3_c's first named fields start
   at 0x4fe4 (same note, lines 482-493). So "which sibling owns the
   0x4f38-style shared member" resolves to: all of them, as subobject — what
   varies is each class's own block after 0x4f38.

The header documents 13 direct RTTI children of dScMgSingle3DBase_c (card,
cup, memory x2, mahjong-carlo x2, roulette, slot3, sound, BSC, snowball,
flower, 3DEsp). The census adds dScMgHanachan_c as a fourteenth sibling,
inside the over-merged unit[34] mega-cluster. The dScMgD3DBase_c-derived side
of the family (Jump, Jump2, Trampoline, Trampoline2, Curling x2, Hanachan,
Pachinko x2, Bomroom, Luigi, Teresa, Coin, Panel, Smartball, Amida...)
shows the same D1-then-D0 opening in its units, so the emission idiom is not
base-specific. What this pass did NOT itemize: the D1/D0 placement of each of
unit[34]'s seven classes, and `sizeof(dScMgD3DBase_c)` — both honest gaps, the
first only matters if unit[34] is ever un-merged.

### dScMg* class-unit census (build/tu_map.json, this session)

| unit | run | n | class(es) | distant members |
| --- | --- | --- | --- | --- |
| 23 | 0x020d1018..0x020d5974 | 26 | dScMgAmida_c | 0 |
| 25 | 0x020d5a54..0x020d95a4 | 81 | dScMgBomroom_c | 0 |
| 26 | 0x020d95a4..0x020dbd54 | 35 | dMgCardObj_c, dMgDilarCardObj_c, dScMgCard_c | 1 |
| 28 | 0x020dbe40..0x020de988 | 63 | dScMgCoin_c | 0 |
| 29 | 0x020de988..0x020e0574 | 30 | dScMgCup_c | 0 |
| 31 | 0x020e0638..0x020e3854 | 47 | dScMgCurling_c | 0 |
| 32 | 0x020e3854..0x020e6bf4 | 52 | dScMgCurling2_c | 0 |
| 34 | 0x020e6c28..0x020eebe8 | 180 | MgPsycheOut, dMg3DEspAnimSet_c, dMg3DEspModel_c, dScMg3DEsp_c, dScMgD3DBase_c, dScMgHanachan_c, dScMgJump_c | 3 |
| 35 | 0x020eebe8..0x020efc08 | 30 | dScMgJump2_c (factory in-unit) | 1 |
| 37 | 0x020efc0c..0x020f3834 | 58 | dScMgLuigi_c | 0 |
| 38 | 0x020f3834..0x020f5504 | 46 | dScMgMemory_c | 0 |
| 40 | 0x020f5564..0x020f75d4 | 51 | dScMgMemory2_c | 0 |
| 42 | 0x020f7634..0x020f8e44 | 23 | dMgMCarloCardObj_c, dScMgMCarlo_c | 0 |
| 44 | 0x020f8ef4..0x020fa6ac | 23 | dMgMCarlo2CardObj_c, dScMgMCarlo2_c | 0 |
| 46 | 0x020fa75c..0x020ff3ec | 75 | dScMgPachinko_c | 0 |
| 48 | 0x020ff420..0x0210428c | 74 | dScMgPachinko2_c | 0 |
| 49 | 0x0210428c..0x0210788c | 72 | dScMgPanel_c | 0 |
| 50 | 0x0210788c..0x0210a400 | 42 | dScMgRoulette_c | 2 |
| 52 | 0x0210a4b0..0x0210a708 | 8 | dScMgSingle3DBase_c (the base itself) | 0 |
| 54 | 0x0210a8c0..0x0210d6b8 | 49 | dScMgSlot1_c, dScMgSlot3_c | 8 |
| 56 | 0x0210d740..0x02119900 | 132 | dScMgSmartball_c + 12 cMgSmartball_* element classes | 0 |
| 57 | 0x02119904..0x0211cb70 | 83 | dScMgSound_c | 2 |
| 59 | 0x0211cbd0..0x021207dc | 82 | dScMgTeresa_c | 0 |
| 60 | 0x021207dc..0x0212231c | 43 | dScMgTrampoline_c | 1 |
| 63 | 0x021225ac..0x021245a8 | 47 | dScMgTrampoline2_c | 7 |
| 65 | 0x02124908..0x021255f8 | 19 | dScMgBSC_c (banked) | 0 |
| 67 | 0x0212568c..0x021295ac | 26 | dScMgSnowball_c | 3 |
| 71 | 0x0212a554..0x0212b7f8 | 19 | dScMgFlower_c | 10 |

(Rows 60/63: the recorded end is the end of the last *labeled* function; a
trailing absorbed member extends each run to its factory — the two 0x174-byte
helpers detailed under DECISION 2. Distant-member counts are the verified
ones from the contiguity analysis, not tu_map's own bookkeeping.)

Per-function sources: named members live in `src/_ZN<len><Class>_<method>Ev.cpp`
(e.g. src/_ZN18dScMgTrampoline2_c13InitResourcesEv.cpp), unnamed ones in
`src/func_ov006_<addr>.c`; each unit's exact list is `unit["functions"]` in
build/tu_map.json.

RECOMMENDATION (D1): adopt form B family-wide — inline destructor in the class
body for every dScMg* scene class and every dMg* element class in ov006, no
out-of-line destructor anywhere in the module. No per-class exception exists in
the ROM evidence; if a future unit's run does not open D1-then-D0, treat it as
a mis-split unit, not a new form.

## DECISION 2 — Spawn/element-ctor boundary policy

**What the PoleLift precedent actually settled** (config/tu_manifest.d/ov045/
PoleLift.json): its run 0x0211150c..0x02111840 contains seven functions
*including PoleLift_Spawn* — the factory is inside the class TU's run, with
the boundary record "seven functions with no foreign symbol inside the span",
and `__sinit_ov045_02112214` in the same .o owns the .bss statics
0x021131a8/0x021131b0. Settled: one .o per element = class functions + factory
+ file-scope SpawnInfo + sinit'd statics; the factory is not a separate TU.

The minigame census agrees, with three independent proofs.

**(a) Every factory is contiguous with the end of its class run.**

| minigame | class run | factory |
| --- | --- | --- |
| Card | unit[26] 0x020d95a4..0x020dbd54 | MgPicturePoker_Spawn 0x020dbd54 (banked separately: config/tu_manifest.d/ov006/MgPicturePoker.json, 3 fns 0x020dbd54..0x020dbe40, text- and module-link-verified) |
| MCarlo | unit[42] 0x020f7634..0x020f8e44 | unnamed factory 0x020f8e44 |
| MCarlo2 | unit[44] 0x020f8ef4..0x020fa6ac | MgPairAGoneAndOn_Spawn 0x020fa6ac |
| Memory | unit[38] 0x020f3834..0x020f5504 | MgMemoryMatch_Spawn 0x020f5504 (tu_map candidate unit[39]; no banked manifest entry) |
| BSC | unit[65] 0x02124908..0x021255f8 | MgLuckyStars_Spawn 0x021255f8 (the pilot manifest deliberately stops here) |
| Trampoline | unit[60] 0x021207dc..(see below) | MgTrampolineTime_Spawn 0x02122490 |
| Trampoline2 | unit[63] 0x021225ac..(see below) | MgTrampolineTerror_Spawn 0x0212471c |

**(b) .data interleave proves the SpawnInfo shares the class .o.** The linker
concatenates per-object .data, so an 8-byte SpawnInfo sandwiched inside a
family's typeinfo records is same-.o proof. config/arm9/overlays/ov006/
relocs.txt: `0x0213bc50 -> 0x020dbd54` (PicturePoker's SpawnInfo) sits inside
the Card element cluster, next to the 12-byte derived `_ZTI17dMgDilarCardObj_c`
whose base word points at 0x0213bc48 (`_ZTI12dMgCardObj_c`);
`0x0213d580 -> 0x020f8e44` sits inside MCarlo's cluster;
`0x0213d70c -> 0x020fa6ac` sits inside MCarlo2's, four bytes before the
element vtable header `0x0213d724 -> _ZTI19dMgMCarlo2CardObj_c` whose slots
`0x0213d728 -> 0x020f98dc` and `0x0213d72c -> 0x020f9994` land inside
unit[44]'s run — the key-function rule emitting the element vtable in the
same .o that defines the element's out-of-line virtuals.

**(c) The Trampoline "anomaly" is resolved — and it is the same artifact twice.**
unit[63]'s recorded end 0x021245a8 is not adjacent to MgTrampolineTerror_Spawn;
the gap is one function, `func_ov006_021245a8` (size 0x174), which IS a member
of the unit, called only from
src/_ZN18dScMgTrampoline2_c13InitResourcesEv.cpp:48. Its body ends exactly at
0x0212471c, the factory's first byte. The Time side mirrors it to the byte:
`func_ov006_0212231c` (size 0x174, member of unit[60], called only from
src/_ZN17dScMgTrampoline_c13InitResourcesEv.cpp:52) bridges unit[60]'s
recorded end 0x0212231c to MgTrampolineTime_Spawn at 0x02122490. Terror_Spawn's
own callee func_ov006_021248a8 (declared at src/MgTrampolineTerror_Spawn.cpp:48,
defined in src/func_ov006_021248a8.cpp) follows at 0x021248a8, and BSC's D1
opens at 0x02124908. So 0x021207dc..0x02124908 is one contiguous family
stretch (a 4-byte unit[62] filler at 0x021225a8 aside), and the .data agrees:
MgTrampolineTime_SpawnInfo 0x0213fab8 adjacent to `_ZTI17dScMgTrampoline_c`
0x0213fad0, MgTrampolineTerror_SpawnInfo 0x0213fbc8 adjacent to
`_ZTI18dScMgTrampoline2_c` 0x0213fc04. Honest caveat: unlike Card, these two
SpawnInfos sit *between* the two RTTI clusters rather than inside one, so the
.data alone cannot distinguish one .o from two adjacent ones — here the .text
contiguity through the absorbed helpers carries the one-.o claim.

**(d) The tu_map factory cuts are label-driven artifacts.** Every *labeled*
factory became its own unit (MgPicturePoker, MgPairAGoneAndOn, MgMemoryMatch,
MgTrampolineTime, MgTrampolineTerror...), while `dScMgJump2_c_Spawn` stayed
inside unit[35] — verified this session — the one exception that shows the
split tracks label bookkeeping, not .o structure.

RECOMMENDATION (D2): adopt the PoleLift boundary family-wide — one .o per
minigame containing the scene class(es), the element classes, the factory plus
any factory-only callees, the file-scope SpawnInfo, and the sinit'd statics.
Concretely: merge each banked factory TU into its class TU before conversion
(including the factory the BSC pilot deliberately excluded), and when a
unit's recorded end is not adjacent to its factory, look for a trailing
absorbed member (the two 0x174-byte InitResources helpers are the measured
examples) before concluding a separate .o.

## DECISION 3 — shared-helper ownership and cross-TU signature consistency

True signatures — the mangled names and the surviving definitions agree
(homes in config/arm9/symbols.txt lines ~1509-1537; definitions in src/):

| ROM symbol | address | true signature | definition |
| --- | --- | --- | --- |
| `_Z14ApproachLinearRsss` | 0x0203adec (0x6c) | `int ApproachLinear(s16&, s16, s16)` | src/_Z14ApproachLinearRsss.cpp |
| `_Z14ApproachLinearRiii` | 0x0203ae58 (0x80) | `int ApproachLinear(int&, int, int)` | src/_Z14ApproachLinearRiii.cpp |
| `_Z15ApproachLinear2Rsss` | 0x0203aed8 | `int ApproachLinear2(s16&, s16, s16)` | src/_Z15ApproachLinear2Rsss.cpp |
| `_Z15ApproachLinear2Riii` | 0x0203af34 | `int ApproachLinear2(int&, int, int)` | src/_Z15ApproachLinear2Riii.cpp |
| `_Z14ApproachLinearR7Vector3RKS_5Fix12IiE` | 0x0203b89c | `int ApproachLinear(Vector3&, const Vector3&, Fix12<int>)` | as mangled |
| `RandomIntInternal` | 0x0203b990 (0x24) | `extern "C" int RandomIntInternal(int* seed)` | src/RandomIntInternal.c |

The `R` in each mangled name proves the first parameter is a reference (the
current spellings use pointers); the definitions return int (1 on reach, 0
otherwise), not void. RandomIntInternal's ROM name is unmangled, so it is
extern "C" — `int RandomIntInternal(int* seed) { *seed = *seed * 0x19660d +
0x3c6ef35f; return *seed; }` is the period-accurate spelling.

**Why the banked TUs get away with wrong spellings.** BSC declares
`extern "C" int _Z14ApproachLinearRiii(int& v, int a, int b);` and
`extern int RandomIntInternal(int* seed);`
(src_tu/actors/dScMgBSC_c+dScMgBSC_c.cpp:173,179); MCarlo declares
`extern void _Z14ApproachLinearRiii(int *x, int t, int s);`, `extern int
RandomIntInternal(void* seed);` and `extern int _Z15ApproachLinear2Rsss(s16*
r, s16 t, s16 s);` (src_tu/actors/dMgMCarloCardObj_c+dScMgMCarlo_c+
dScMgMCarlo_c.cpp:254,256,408). All of these sit inside `extern "C" { }`
blocks, and C linkage discards parameter and return types — so the
pointer-for-reference, void-return and void*-seed variances are invisible to
compiler and linker; only call syntax and return-value use matter. They are
link-safe but not what the EAD team wrote.

**The hazard that forces a real choice (MEASURED).** mwccarm 2004/b56
`-O4,p -lang c++` (build/extern_probe/): a *bare C++* declaration spelled with
the mangled name emits a phantom double-mangled reference — probe.cpp produced
`_Z19func_ov006_020c0c80Pv`, `_Z22_Z14ApproachLinearRiiiPiii` and
`_Z17RandomIntInternalPi`. These are invisible to text-verify (match.py
compares relocated words as wildcards) and fatal at the ROM link.
include/decl_common.h documents exactly this for its own declarations, and its
`ApproachLinearI` resolves nowhere. Conversely probe2.cpp proves the
idiomatic spellings emit the exact ROM symbols: `int ApproachLinear(int&, int,
int);` -> `_Z14ApproachLinearRiii`; `int ApproachLinear(short&, short,
short);` -> `_Z14ApproachLinearRsss`; `int ApproachLinear2(int&, int, int);`
-> `_Z15ApproachLinear2Riii`; `extern "C" int RandomIntInternal(int*);` ->
`RandomIntInternal`.

One reconcile lesson already in the tree: MgWhichWiggler's guessed `(void*)`
for func_ov006_020c1d80 was dropped when the definition proved
`void* func_ov006_020c1d80(char*)` (src/func_ov006_020c1d80.cpp) — the extern
must follow the definition, not the guess.

RECOMMENDATION (D3): per-TU idiomatic declarations — not a shared header, not
mangled-name identifiers. In each new TU declare exactly what it calls: the
ApproachLinear family as plain C++ true-name declarations (which mangle to the
exact ROM symbols), `RandomIntInternal` and the `func_ov006_*` helpers as
extern "C" spelled with the definition's signature. Do not extend
include/decl_common.h (it is auto-generated by tools/decl_headers.py and
already carries the bare-declaration hazard). Never write a mangled-name
identifier in C++ linkage.

## DECISION 4 — pragma-conflict census and policy

Census (build/pragma_census.py over the ov006 per-function sources): 221
legacy sources carry `#pragma` directives. Zero units merge two members with
conflicting states of the same directive — every `opt_*` occurrence in the
tree is "off" and `optimize_for_size`, where present, is "on". So a conflict
policy is needed for *mechanism*, not because any conflict exists today.

The mechanism, decoded on BSC with an 8-combination probe: `opt_*` directives
are file-global last-wins in mwccarm, and they only matter to *carriers* —
functions whose codegen is byte-sensitive to the directive. Every other member
byte-matches under either state. BSC has 2 carriers
(`opt_strength_reduction off`) and text-verified 19/19 under the file-wide off
state. Positional pragmas (`#pragma push` / an O-level / `pop`, `long_calls`)
are bracket-order-sensitive and must stay wrapped around their own function.

RECOMMENDATION (D4): per-TU pragma preamble = the union of the members'
directives at the top of the file, computed AFTER distant-member exclusion;
positional pragmas stay bracketed in place. The union is always well-defined
today because no conflicts exist. If a member fails text-verify under the
union state it is default-state-sensitive — treat that as a mis-split unit
(wrong member set), not a pragma problem, and escalate.

## CROSS-CUTTING — distant absorbed members (a tubuild gap that gates merges)

tu_map's `absorb_unlabelled` (tools/tu_map.py:468-507) claims that a function
called only from inside one cluster's span is file-local to that cluster's
.o ("That is proof, not preference"). ROM layout refutes it for *distant*
absorbs: 12 ov006 units carry members thousands of bytes outside their span
(unit[71] 10, unit[54] 8, unit[63] 7, unit[67] 3, unit[34] 3, unit[50] 2,
unit[57] 2, unit[26] 1, unit[35] 1, unit[60] 1, unit[70] 1, unit[18] 1 —
the dScMg ones of these appear in the census's "distant members" column
above; verified by contiguity analysis, not tu_map's own bookkeeping).
Those functions are unnamed globals of shared-utility .o's
(0x020c0aa8..0x020c1d80, 0x020c3288...), not class members. tubuild's
`unit_functions()` (tools/tubuild.py:282-292) has no span filter, so a
manifest built from a contaminated unit would emit the foreign functions into
the TU. Every merge must exclude distant members first; the roster below is
chosen so the first three TUs do not depend on that fix.

## Mini-wave roster (next three TUs, plus a stretch)

All three roster TUs are verified uncontaminated (members form one contiguous
slice of module function order), so the wave can start before the tubuild fix.

**1. dMgMCarlo2CardObj_c + dScMgMCarlo2_c + MgPairAGoneAndOn (merge of
units 44+45) — 25 functions, 0x020f8ef4..0x020fa75c (0x1868 bytes).**
TU id as tubuild composes it:
`MgPairAGoneAndOn+dMgMCarlo2CardObj_c+dScMgMCarlo2_c+dScMgMCarlo2_c`.
The direct continuation of the banked MCarlo TU — same family, same helpers,
one element class (dMgMCarlo2CardObj_c) plus the scene class plus the
2-function factory unit (MgPairAGoneAndOn_Spawn and func_ov006_020fa740, the
element-ctor callback that stores `&data_ov006_0213d728` — the vptr —
src/func_ov006_020fa740.c). That factory unit is itself banked —
config/tu_manifest.d/ov006/MgPairAGoneAndOn.json, text-verified, 2 functions —
so this TU supersedes a banked entry. Exercises D1 (form-B sibling, multi-class), D2
(first factory merge: the TU must emit SpawnInfo 0x0213d70c -> 0x020fa6ac and
the unnamed element vtable at 0x0213d724), D3 (members func_ov006_020f9560 /
020f9994 / 020fa3d0 call the ApproachLinear/RandomInt helpers), D4 (first
opt_propagation carrier TU). Risks: `+` in the TU id (the known lcf-grammar
blocker §4.2 of the pilot report); the element vtable emits correctly only if
dMgMCarlo2CardObj_c's virtuals are declared exactly; D3 spelling discipline
applies from the first line.

**2. dScMgMemory_c + MgMemoryMatch (merge of units 38+39) — 47 functions,
0x020f3834..0x020f5564 (0x1d30 bytes).** TU id:
`MgMemoryMatch+dScMgMemory_c+dScMgMemory_c`. The simplest clean form-B
sibling — the base header's inline-dtor rule was *measured on this class*
(include/dScMgSingle3DBase_c.h:90-95), so it lands the D1 recommendation on
its own evidence. Exercises D1, D2 (a fresh factory merge, MgMemoryMatch_Spawn
0x020f5504), D3 (func_ov006_020f4cd8), D4 (opt_lifetimes carrier). Risks: the
same `+`-id exposure; MAX_PATH (§4.3) as always.

**3. dMgJump3DMario_c (unit 16) — 28 functions, 0x020c762c..0x020c8a30
(0x1404 bytes).** TU id: `dMgJump3DMario_c` (no `+`). The clean dMg* helper TU
this wave needs: zero pragmas, zero distant members, no factory, no SpawnInfo.
Exercises D3 hardest (the model-helper members call the shared helpers
throughout) and nothing else — the low-risk landing for the extern spellings.
Risk: members are named `Unk_` (`_ZN16dMgJump3DMario_c12Unk_020c762cEv` etc.),
so this TU carries naming work but no layout risk.

**Stretch (only after the tubuild span filter lands): dScMgSlot1_c +
dScMgSlot3_c (unit 54) — 41 in-place functions of 49 listed (the 8 distant
members at 0x020c1eb4..0x020c221c must be excluded), 0x0210a8c0..0x0210d6b8.**
One .o is proven by the interleaved typeinfo run 0x0213e588..0x0213e66c
(`_ZTI12dScMgSlot3_c`, `_ZTIN12dScMgSlot1_c9betIcon_cE`, `_ZTI12dScMgSlot1_c`,
`_ZTSs`, `_ZTVN12dScMgSlot1_c9betIcon_cE`) — the nested betIcon_c class makes
it the multi-class deep end, with 4 opt_strength_reduction + 1 loop_inv
carriers. Field names for both classes are already reconstructed in
notes/minigame-provenance.md.

## ADJUDICATION (coordinator, 2026-08-30)

Spot-checks against primary evidence before ruling: the 0x4f38 static assert
(include/dScMgSingle3DBase_c.h:134), MgLuckyStars_Spawn's 0x51d0 allocation and
`p + 0x4f38` call (src/MgLuckyStars_Spawn.cpp:12,18), zero `D2Ev` in
config/arm9/overlays/ov006/symbols.txt, the MUST-STAY-INLINE warning
(include/dScMgSingle3DBase_c.h:90-95), PoleLift_Spawn inside the PoleLift TU's
function list (config/tu_manifest.d/ov045/PoleLift.json:66), the
true-signature homes (config/arm9/symbols.txt:1510,1537) with definitions on
disk, zero `#pragma opt_* on` anywhere in src/, and the phantom-mangling
arithmetic: `_Z14ApproachLinearRiii` is 22 characters, so
`_Z22_Z14ApproachLinearRiiiPiii` is forced by mangling an identifier that is
itself a mangled name — the probe result is correct by construction.

**D1 — ADOPTED as recommended.** Form B family-wide: inline destructor in the
class body for every dScMg* scene class and dMg* element class in ov006; no
out-of-line destructor. A class run that does not open D1-then-D0 is a
mis-split unit — escalate, do not invent a new form.

**D2 — ADOPTED with a scoping amendment.** One .o per minigame — scene
class(es), element classes, factory plus factory-only callees, file-scope
SpawnInfo, sinit'd statics — is the boundary for every NEW conversion, starting
with the mini-wave (roster items 1 and 2 already merge their factories in).
Amendment: already-banked TUs keep their banked shape for now. BSC's
deliberate stop at MgLuckyStars_Spawn was correct when it banked; the factory
merges for banked TUs (BSC absorbing MgLuckyStars_Spawn, Card absorbing the
banked MgPicturePoker entry) are a follow-up pass AFTER the mini-wave — they
revise banked manifests and re-verify, and that cost is paid once the merge
shape is measured, not mixed into the wave's measurements. One correction to
DECISION 2's evidence table: MgMemoryMatch is NOT banked — unit[39] is a
tu_map candidate, not a manifest entry; the banked factory entries are the six
Mg* manifests (MgBingoBallSlotsShot, MgMushroomRoulette, MgPairAGoneAndOn,
MgPicturePoker, MgTrampolineTerror, MgWhichWiggler), and roster item 1's merge
supersedes the banked MgPairAGoneAndOn entry (text-verified, 2 functions).

**D3 — ADOPTED as recommended, and tied to the goal gate.** Per-TU idiomatic
true-name declarations; never a mangled-name identifier in C++ linkage; never
extend include/decl_common.h. The ratified canonical spellings that
tools/check_tu_idioms.py's cross-TU consistency table is seeded with at gate
review: the ApproachLinear family as true-name C++ declarations with reference
first parameters and int return — `int ApproachLinear(int&, int, int)`,
`int ApproachLinear(s16&, s16, s16)`, `int ApproachLinear2(int&, int, int)` —
which mangle to the exact ROM symbols; `extern "C" int RandomIntInternal(int*
seed)`; every func_ov006_* helper as extern "C" spelled with its definition's
signature. Two citation corrections to DECISION 3's evidence, neither
load-bearing: the definition that corrected MgWhichWiggler's guess is
src/func_ov006_020c1d80.c (a .c file, not .cpp), and its banked signature is
`int func_ov006_020c1d80(char*)` returning the pointer as int — the char*
parameter is the correction that mattered; the return spelling in the note was
imprecise.

**D4 — ADOPTED as recommended.** Per-TU union preamble computed after
distant-member exclusion; positional pragmas stay bracketed in place; a member
that fails text-verify under the union state means a mis-split unit —
escalate, do not tune pragmas.

**Cross-cutting tubuild gap — REAL, queued as its own task, not blocking the
wave.** The absorb_unlabelled locality claim is refuted by ROM layout (12 ov006
units with distant absorbed members) and unit_functions() has no span filter,
so a manifest built from a contaminated unit would emit foreign functions.
The first three roster TUs are verified uncontaminated, so the mini-wave
proceeds; the stretch TU and any contaminated unit wait for the span-filter
fix.

**Roster — CONFIRMED, with two id corrections.** All three TUs approved in the
note's order (MCarlo2+factory first, Memory second, Jump3DMario third; stretch
Slot1+Slot3 after the span filter). The ids as printed cite tubuild's current
composition and still carry the duplicate-label bug: once the tu_map dedupe
lands, roster 1's id is `MgPairAGoneAndOn+dMgMCarlo2CardObj_c+dScMgMCarlo2_c`
and roster 2's is `MgMemoryMatch+dScMgMemory_c`. Coordinator recount of the
three ranges against ov006 symbols gives 26/48/29 function symbols inclusive
of each end address — exactly +1 per range over the note's 25/47/28, which is
the next unit's opening function at each exclusive end; the note's counts
stand.

## AMENDMENT (coordinator, 2026-08-31) — the upstream wave landed; decisions re-scoped to as-landed reality

This note was prepared against the two banked pilot TUs on branch
`pilot2-ov006-tu-merge`. Between 2026-08-30 and 2026-08-31 the upstream wave
promoted minigame TUs directly to src/ and config/tu_manifest.d/ov006/:
five class TUs (dScMgBSC_c, dScMgCard_c, dScMgMCarlo_c, dScMgMCarlo2_c,
dScMgSingle3DBase_c) and the six Mg* factory manifests (MgBingoBallSlotsShot,
MgMushroomRoulette, MgPairAGoneAndOn, MgPicturePoker, MgTrampolineTerror,
MgWhichWiggler). The banked pilots are superseded; every decision below
stays in force for what remains.

**D2 — as-landed: factories are separate TUs on main, so the merge is a
post-wave pass for ALL promoted TUs, not just banked ones.** Main's manifest
carries MgPairAGoneAndOn.json alongside dScMgMCarlo2_c.json and
MgPicturePoker.json alongside dScMgCard_c.json — the one-.o-per-minigame
boundary was NOT applied by the wave. The ruling's substance is unchanged
(the ROM evidence for it is in DECISION 2 and was not contradicted by any
landing), but its scoping amendment now reads: every promoted class TU keeps
its as-landed split shape; the factory merge (Card+MgPicturePoker,
MCarlo2+MgPairAGoneAndOn, and later BSC+MgLuckyStars_Spawn and
MCarlo+the unnamed factory at 0x020f8e44, neither of which has a manifest
entry) is one pass after the class TUs are converted, revising manifests and
re-verifying. NEW conversions still target the merged shape from the first
line. One consequence worth recording: main's promoted entries use
single-class ids (no `+`), which moots the lcf-grammar blocker for those
TUs — but the manifest still carries three multi-class `+` ids
(ov002/EnemySpawner+EnemySwitchTag, ov020/BookShot+BookShotSpawner+Bookend,
ov062/Koopa+KoopaSmall), each naming its tracked shadow source file for the id
so the `+` reaches the object basename and aborts mwldarm's selector grammar
in a whole-tree link — the abort is measured against the pinned mwldarm, not
inferred (the new tubuild test re-measures the grammar and the 259-char path
limit on every run). One correction to the record read: the EnemySpawner
entry's banked partial-isolation record, "result": "failed", is NOT that
abort — that run substitutes per-function objects whose basenames carry no
join, its link phase passed (phases.link: true), and the failing phase is
checkSymbols, a different defect the sanitization does not touch. The
tubuild scratch-side `+`/MAX_PATH sanitization is still needed before any of
them linkcheck.

**Retro-census baseline (the wave's measured starting line).** Run
2026-08-31 with tools/check_tu_idioms.py at the constitution-lane head
(origin/main + the BSC comment fix), report mode, over the promoted ov006
entries:

| TU (ov006/) | funcs | member | externC | raw offsets | named coverage |
| --- | --- | --- | --- | --- | --- |
| dScMgCard_c | 32 | 4 | 28 | 202 | 15% (36 named) |
| dScMgBSC_c | 17 | 3 | 14 | 89 | 11% (11 named) |
| dScMgMCarlo_c | 21 | 3 | 18 | 107 | 30% (46 named) |
| dScMgMCarlo2_c | 21 | 4 | 17 | 69 | 59% (99 named) |
| dScMgSingle3DBase_c | 7 | 4 | 3 | 0 | 100% (4 named) |
| MgPairAGoneAndOn | 2 | 0 | 2 | 3 | n/a (no header) |

dScMgSingle3DBase_c is effectively done — zero raw offsets, full named
coverage. The other four class TUs carry 89-202 raw member-offset accesses
each at 11-59% named coverage: by the census's own measure the wave's
promotions are ~80% shadow idiom, which is exactly what the goal gate exists
to price. These numbers are the baseline the remaining conversion work is
measured against.

**D3 — live drift in the promoted tree, the wave's first work items.** The
2026-08-31 census:

- Retired mangled-name declarations still ship in the promoted TUs:
  `_Z14ApproachLinearRiii` in dScMgBSC_c (extern "C", int& form),
  dScMgCard_c (int* form) and dScMgMCarlo_c (void-return int& form);
  `_Z15ApproachLinear2Rsss` in dScMgCard_c and dScMgMCarlo_c. dScMgMCarlo2_c
  already declares the true names — `int ApproachLinear(int&, int, int)` and
  `int ApproachLinear2(s16&, s16, s16)` — so the wave has already produced
  both dialects, which is the drift the ratified-spellings gate is for.
- RandomIntInternal: dScMgBSC_c and dScMgCard_c declare the ratified
  `extern int RandomIntInternal(int *seed)`. dScMgMCarlo2_c:150 and
  dScMgMCarlo_c:159 declare `int RandomIntInternal(void* seed)` — both inside
  `extern "C"` blocks (MCarlo2's at line 124), so C linkage discards the
  variance and the link is safe; the spelling is still not what the
  definition (src/RandomIntInternal.c: `int RandomIntInternal(int* seed)`)
  or the ruling says. Fixing these four files' declarations is the first
  D3 work item of the wave proper.
- Tree-wide, two more RandomIntInternal dialects exist outside ov006
  (unsigned-int returns in TTC_MovingBar and Bird/Chuckya/FlyGuy/HeaveHo).
  They are out of this wave's scope — the gate runs on ov006 — but the
  canonical-signature table will fail them the day those modules are gated,
  which is the table doing its job.

**Roster — superseded where the wave already landed.** Roster item 1
(MCarlo2 + MgPairAGoneAndOn merge) is superseded by main's promotion: the
class TU and the factory are both promoted, as separate entries, so the
remaining work on MCarlo2 is goal-side conversion (69 raw offsets, 59%
coverage, true-name declarations already partly in place), not
reconstruction. Items 2 (Memory) and 3 (Jump3DMario) stand as written; the
stretch TU still waits on the span filter. The wave's dispatch order is now
census-first: Card (202 raw, 15%), BSC (89 raw, 11%), MCarlo (107 raw, 30%),
then MCarlo2's completion and the factory-merge pass.

## REVIEW #2 FOLD-IN (coordinator, 2026-08-31) — outside perspective, seven findings

The second outside-perspective review (fresh-context agent, the standing
cadence) returned ON TRACK with seven findings; all seven are folded here,
two of them into the gate itself (commit `4e8a2d2a8`, tools only, 52/52
tests).

**D3, extended — four more retired mangled-name identifiers ship in the very
files the wave converts next, and no split machinery could ever see them**
(single-TU declarations form no signature group):
`_ZN5Sound12PlayBank2_2DEj` (Sound::PlayBank2_2D — declared in ALL FOUR
promoted ov006 class TUs; 23 occurrences across them, 12 in dScMgCard_c.cpp
alone), `_ZN2GX11LoadOBJPlttEPKvjj` (Card, MCarlo2, Single3DBase,
ov004/unit020b0a38), `_ZN3GXS11LoadOBJPlttEPKvjj` (BSC, Card, MCarlo2,
Single3DBase, unit020b0a38), `_ZN3G2x13SetBlendAlphaEPVttttj` (Card only).
All four are now in the gate's RATIFIED_TRUE_NAMES table, the forms read off
the mangle and the repo's own headers: G2x.h already carries SetBlendAlpha's
measured EPVttttj shape (a shadow int cost one ROM-build failure once);
Sound.h's namespace-plus-u32 is PlayBank3's precedent; PKv is
`const void*`, so Card's `(void*, u32, u32)` spelling for LoadOBJPltt is the
drift and BSC/MCarlo2 carry the ratified one — the census's split entry now
shows both spellings side by side, the only type-level variance in the
family (the u32 vs unsigned int rows inside a split entry are same-type
respellings, and PlayBank2_2D forms no split at all — all four TUs agree on
its shape). Gated runs now FAIL on all four names with their ratified forms;
report mode lists which TUs carry each.

**Coverage, re-priced — the census counted unk_/pad_ respellings as named
hits.** `->unk_51b8` raises named and lowers rawOffsets, both metrics moving
the "right" way while adding no name the EAD team would have written; the
family base header alone carries 26 unk_/pad_ fields. The gate keeps `named`
as the total (the table above stays comparable) and adds `namedEncoded` for
the respelled share (report schema 2). Re-census at the gate commit: BSC
11 named = 5 real + 6 unk/pad; Card 36 = 23 + 13; MCarlo 46 = 34 + 12;
MCarlo2 99 = 78 + 21; Single3DBase 4 = 4 + 0. On the re-priced numbers BSC
is the least-named TU in the family, not merely the smallest.

**Wave order, re-adjudicated — BSC first, then Card, MCarlo, MCarlo2's
completion; dScMgBase_c deferred; the merge pass composes from converted
sources.** This supersedes the census-first dispatch order above. BSC first
because it is the family's calibration unit: the smallest class TU (17
functions), the one whose shipped false deleting-destructor comment became
the goal gate's founding bug, so its remaining conversion is where the
byte gate and goal gate are proven together on a surface whose every
landmine is already known. dScMgBase_c is DEFERRED to after the four
children, and the reason is recorded here: renaming its base-header members
fans out into every promoted child's re-verification, so it runs once,
last, against children that are already re-verified. The factory-merge
pass stays last for all promoted TUs and must COMPOSE from the converted
sources — never re-derive from the legacy per-function files.

**Comment-per-member — prose is the review cadence's job, not the gate's.**
Pilot 2 §7 required a comment on every re-typed member; that requirement
never landed in any gate, and the goal gate's justification field accepts
any non-empty string. The honest statement: comment quality is enforced by
the review cadence, not by check_tu_idioms.py. Every conversion's diff gets
a reviewer pass that reads the comments against the ROM evidence — the BSC
slot-18 catch was exactly this kind of find.

**PR-body disclosure — the lane carries one src/ change.** `0f62aff48`
(dScMgBSC_c: correct the false deleting-destructor comment on
func_ov006_02125364, byte-verified 19/19) crossed the tools/notes-only
boundary deliberately: this note documents that bug and cites the fix, and a
lane that documents a false comment while continuing to ship it would be
worse. The PR body names the commit explicitly with its evidence so the
reviewer can strike it from the batch if they disagree.

**origin/main moved during the fold-in.** PR #2084 promoted daBrq_c as a
genuine TU; PR #2085 dropped an orphaned _ZTV3Amp declaration. The lane was
rebased onto the current tip (`90763c379`) before the fold-in commits, so
every claim above rides current main, not a stale tree.