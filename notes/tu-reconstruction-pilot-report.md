# TU reconstruction pilot — `ov045/PoleLift`

**What this is:** the read-only experiment described in
[`translation-unit-reconstruction-plan.md`](translation-unit-reconstruction-plan.md)
§18, steps 2–5. One high-confidence candidate translation unit was curated by hand
into a single `.cpp`, compiled once with the pinned toolchain, and compared
function-by-function against the ROM.

**What it is not:** a build change. `src_tu/actors/PoleLift.cpp` is not enrolled,
`config/**/delinks.txt` is untouched, and the seven one-function sources under
`src/` remain the sole enrolled owners of `0x0211150c..0x02111840`. No
`rombuild.py`, no `eligible.py`, no link.

---

## 1. Headline result

> **7 of 7 already-matched functions remain byte-identical when compiled together
> in their candidate original TU context.**

That is the question §18 says the experiment exists to answer, and for this
candidate the answer is unambiguous. It cost one deliberate source-level change
(§5) and surfaced one structural obstacle that per-function compilation had been
hiding (§6).

```python
TU ov045/PoleLift

MATCH  PoleLift::~PoleLift [D1]      0x0211150c  size 0x04c
MATCH  PoleLift::~PoleLift [D0]      0x02111558  size 0x060
MATCH  PoleLift::CleanupResources    0x021115b8  size 0x038
MATCH  PoleLift::Render              0x021115f0  size 0x028
MATCH  PoleLift::Behavior            0x02111618  size 0x120
MATCH  PoleLift::InitResources       0x02111738  size 0x0d0
MATCH  PoleLift_Spawn                0x02111808  size 0x038

Result: 7/7 functions reproduce in merged-TU context
```

Manifest status is therefore `text-verified`, **not** `link-verified` and nowhere
near `promoted`. See §7 for what stands between the two.

### Baseline control

Before and after, the seven untouched legacy files were re-verified with
`build_pin.verify` under their pinned compiler, so a green merged result cannot be
an environment artefact:

```python
OK    src/game/actors/PoleLift/_ZN8PoleLiftD1Ev.cpp                       2004/b56
OK    src/game/actors/PoleLift/_ZN8PoleLiftD0Ev.cpp                       2004/b56
OK    src/game/actors/PoleLift/_ZN8PoleLift16CleanupResourcesEv.cpp       2004/b56
OK    src/game/actors/PoleLift/_ZN8PoleLift6RenderEv.cpp                  2004/b56
OK    src/game/actors/PoleLift/_ZN8PoleLift8BehaviorEv.cpp                2004/b56
OK    src/game/actors/PoleLift/_ZN8PoleLift13InitResourcesEv.cpp          2004/b56
OK    src/game/actors/PoleLift/PoleLift_Spawn.c                           2004/b56
```

---

## 2. Method

| | |
| --- | --- |
| Candidate | `ov045` / `PoleLift`, RTTI name `daObjKm2_Ami_Bou_c` |
| Span | `.text` `0x0211150c`–`0x02111840`, 7 functions |
| Shadow source | `src_tu/actors/PoleLift.cpp` (`//cpp`, so C++) |
| Compiler | `tools/mwccarm/2004/b56/mwccarm.exe` — `rombuild.VERSION`, and `config/rombuild-versions.txt` carries no override for any of the seven stems, so this is the pin for all of them |
| Flags | `rombuild.CFLAGS` with `-lang c99` → `-lang c++`, exactly what `build_pin.flags_for` returns for a `//cpp` file:<br>`-O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc -Cpp_exceptions off -i include` |
| Object | `build/tu/ov045-PoleLift/PoleLift.o` (gitignored) |
| Comparison | `match.extract_func` + `match.compare` — the tree's own relocation-aware word compare, unmodified. `extract_func` already matches the relocation section by `sh_info` rather than by name, which is exactly the multi-`.text` case this object is. |
| ROM bytes | `extracted/overlays/overlay_0045.bin`, base `0x021111a0`, via `tools/modules.py` |

Nothing in `match.py`, `fdiff.py` or `objisolate.py` was modified or
reimplemented. The one new piece is a scratch driver that walks the seven
`(symbol, address, size)` triples against a single multi-function object instead
of seven single-function ones; it lives outside the repository.

### Boundary evidence

From `build/tu_map.json`, unchanged and not regenerated:

- `ov045` — 41 functions, 6 candidate TUs, 6 static initialisers, 6 `.ctor`
  entries, `sinit_vs_tu: "ok"`, `corroborated: true`, all 5 internal boundaries
  `high`. This is one of the well-segmented modules §14 asks for, not one of the
  four listed as under-segmented.
- The unit record for `0x211150c..0x2111840` lists exactly the seven symbols
  above and one class label, `PoleLift`.
- Neighbours differ: `FireSeaElevator_Spawn` ends at `0x0211150c`,
  `_ZN17ExtendingPlatformD1Ev` begins at `0x02111840`.
- Added here: of ov045's six static initialisers, `__sinit_ov045_02112214` is the
  only one that references this TU's two `.bss` `SharedFilePtr` statics
  (`0x021131a8`, `0x021131b0`), which pins the sixth corroborating initialiser to
  this specific unit rather than to the module in aggregate.

---

## 3. Section order: **CONFIRMED, with a limit the prior finding did not record**

The 2026-08-02 file-consolidation exploration recorded that mwccarm emits one
`.text` section per function and that section order in the object is the
**reverse** of source order. That was twelve days old and flagged as needing
reverification. It was reverified twice here, and it holds — but it holds
*between definitions*, and the qualifier turns out to matter.

### Probe

Three trivial functions, same compiler, same flags:

```cpp
//cpp
extern "C" int probe_A(void) { return 0x11111111; }
extern "C" int probe_B(void) { return 0x22222222; }
extern "C" int probe_C(void) { return 0x33333333; }
```

```python
[5] '.text'  size 0xc   <- probe_C   (LAST in source)
[6] '.text'  size 0xc   <- probe_B
[7] '.text'  size 0xc   <- probe_A   (FIRST in source)
```

Strict reversal. Confirmed.

### The real TU

`src_tu/actors/PoleLift.cpp` is therefore written **bottom-to-top against the ROM**
— `PoleLift_Spawn` first in the file, `~PoleLift` last. The emitted `.text` order
is:

| emitted | section | symbol | size | binding | ROM ordinal |
| ---: | ---: | --- | ---: | --- | --- |
| 1 | 22 | `_ZN8PoleLiftD2Ev` | `0x4c` | GLOBAL | — not in ROM |
| 2 | 24 | `_ZN8PoleLiftD0Ev` | `0x60` | GLOBAL | **1** |
| 3 | 26 | `_ZN8PoleLiftD1Ev` | `0x4c` | GLOBAL | **0** |
| 4 | 28 | `_ZN8PoleLift16CleanupResourcesEv` | `0x38` | GLOBAL | 2 ✓ |
| 5 | 30 | `_ZN8PoleLift6RenderEv` | `0x28` | GLOBAL | 3 ✓ |
| 6 | 31 | `_ZN8PoleLift8BehaviorEv` | `0x120` | GLOBAL | 4 ✓ |
| 7 | 33 | `_ZN8PoleLift13InitResourcesEv` | `0xd0` | GLOBAL | 5 ✓ |
| 8 | 35 | `PoleLift_Spawn` | `0x38` | GLOBAL | 6 ✓ |
| 9 | 39 | `_ZN8PlatformD1Ev` | `0x38` | LOPROC | — elsewhere (ov002 `0x020ee42c`) |
| 10 | 41 | `_ZN8PlatformD0Ev` | `0x4c` | LOPROC | — elsewhere |

**Ordinals 2–6 come out in exact ROM address order.** Writing the file in natural
reading order would have produced them exactly backwards, so the reverse rule is
not merely true, it is load-bearing for any future whole-range link.

**Two limits the old finding did not state, both new:**

1. **The reversal does not reach inside a destructor's variant group.** One
   `PoleLift::~PoleLift()` definition emits three `.text` sections in the fixed
   order **D2, D0, D1**. The ROM's order is **D1 then D0**, with no D2 at all.
   No arrangement of source text can change this: it is one definition, and the
   compiler chooses the intra-group order. Ordinals 0 and 1 are therefore
   emitted swapped, and `_ZN8PoleLiftD2Ev` — byte-identical to D1 — sits in
   front of both.

2. **Vague-linkage bodies trail the whole file.** `_ZN8PlatformD1Ev` and
   `_ZN8PlatformD0Ev` are emitted because `Platform`'s destructor is declared
   inline in its class body and this TU instantiates the base; both are
   `STB_LOPROC` (mwcc's deadstrippable binding) and both are appended *after*
   every one of the TU's own definitions rather than taking part in the
   reversal.

Neither limit affects the byte result — `match.extract_func` finds each symbol in
its own section — but both are decisive for `tubuild.py linkcheck` (plan §7.6),
which compares a contiguous linked range rather than seven independent functions.

---

## 4. Object inventory

43 sections total: 10 `.text`, 12 `.data`, 9 `.rela.text`, 7 `.rela.data`,
`.comment`, `.symtab`, `.strtab`, `.shstrtab`, and the null section.

**No `.bss`, no `.init`, no `.rodata`, no `.ctor`, no `.exception`/`.exceptix`.**
The last of those is `-Cpp_exceptions off` doing its job; the absence of `.init`
and `.bss` means the TU's static initialiser and its two `SharedFilePtr` statics
are genuinely not being produced by this source, which is correct — nothing here
declares them.

### `.text` — 10 sections, 7 licensed

Licensed: the seven in the table above. Unlicensed and emitted anyway:

| symbol | size | binding | why it is here |
| --- | ---: | --- | --- |
| `_ZN8PoleLiftD2Ev` | `0x4c` | GLOBAL | mwcc emits D0/D1/D2 from one `~PoleLift()`. Byte-identical to D1. **Fact:** no such symbol appears anywhere in `config/`, and nothing in the ROM calls it. **Inference, not verified here:** the original link deadstripped it, which is what a vague-linkage-free `D2` with no caller normally suffers. |
| `_ZN8PlatformD1Ev` | `0x38` | LOPROC | `Platform`'s destructor is inline in `include/Platform.h`; instantiating the base emits a vague-linkage out-of-line copy. The real one lives in ov002 at `0x020ee42c`, and `config/arm9/overlays/ov002/symbols.txt` gives it `size=0x38` — the same size this object emitted. |
| `_ZN8PlatformD0Ev` | `0x4c` | LOPROC | same; ov002 `0x020ee464`, `size=0x4c`, again matching what was emitted here. |

### `.data` — 12 sections, 0 licensed

All are RTTI or vtable output, and all are a consequence of this TU being
`PoleLift`'s **key-function TU**: `PoleLift` declares exactly one virtual of its
own, `virtual ~PoleLift()`, and defining it here is what makes the compiler emit
the class's vtable.

| symbol | size | binding |
| --- | ---: | --- |
| `_ZTV8PoleLift` | `0x84` | **GLOBAL** |
| `_ZTV8Platform` | `0x84` | LOPROC |
| `_ZTI8PoleLift` / `_ZTS8PoleLift` | `0xc` / `0xa` | LOPROC |
| `_ZTI8Platform` / `_ZTS8Platform` | `0xc` / `0xa` | LOPROC |
| `_ZTI5Actor` / `_ZTS5Actor` | `0xc` / `0x7` | LOPROC |
| `_ZTI12ActorDerived` / `_ZTS12ActorDerived` | `0xc` / `0xf` | LOPROC |
| `_ZTI9ActorBase` / `_ZTS9ActorBase` | `0x8` / `0xb` | LOPROC |

This is expected per plan §10 and §11 and is **inventoried, not verified**. The
correctness of `_ZTV8PoleLift`'s 33 words against the ROM's `0x02112db4..0x02112e38`
was not checked and is not claimed. Note only that `_ZTV8PoleLift` is the single
`STB_GLOBAL` data symbol — the strong definition a key-function TU is supposed to
produce — while every other data symbol is vague-linkage.

### Relocations

55 in `.text` and 78 in `.data`. Of the `.text` relocations, **39 belong to the
seven licensed functions** — in ROM order 6 / 8 / 5 / 0 / 6 / 10 / 4, `Render`
having none because it is a pure vtable dispatch — and 16 belong to the three
unlicensed bodies (`_ZN8PoleLiftD2Ev` 6, `_ZN8PlatformD1Ev` 4, `_ZN8PlatformD0Ev`
6). The `.data` relocations are all vtable slot pointers and typeinfo links, 32
each for `_ZTV8PoleLift` and `_ZTV8Platform` and 2–3 per RTTI record.

56 undefined imports, every one a real ROM symbol. All relocation sections are
`RELA`, none `REL`, and `objisolate.plan` found no unnamed section-symbol
relocation into a dropped section for any of the seven.

---

## 5. What consolidation forced

One change was required. Two more were taken because consolidation made them
available, and both were measured byte-free.

### 5.1 Required: `PoleLift_Spawn`'s vptr store, `(int)_ZTV8PoleLift` → `(int)(_ZTV8PoleLift + 2)`

This is the interesting one, and per-function compilation could not have exposed
it.

The two conventions collide:

- `decl_common.h` declares `extern int _ZTV8PoleLift[];`, and the config's
  `_ZTV8PoleLift` sits at `0x02112dbc` — which is the **slot array**, already past
  the Itanium preamble. The ROM's `PoleLift_Spawn` literal pool holds exactly
  `0x02112dbc`, and the ROM's `_ZN8PoleLiftD1Ev` stores exactly `0x02112dbc`.
- mwcc's own `_ZTV8PoleLift` names the **vtable object**, which begins two words
  earlier at `0x02112db4` (`0x00000000` offset-to-top, then
  `0x02112d74` = typeinfo). Its own vptr stores relocate against it with
  addend `8`.

In the legacy tree the two never meet: `src/game/actors/PoleLift/PoleLift_Spawn.c` is a **C** TU that
only ever references an external `_ZTV8PoleLift`, so addend 0 is right; and
`src/game/actors/PoleLift/_ZN8PoleLiftD1Ev.cpp` defines the vtable, but `objisolate` strips it,
externalises the symbol and subtracts `VTABLE_PREAMBLE` from the addend — the
correction its own comment documents at length.

Merge them and both spellings land in one object against **one locally-defined
symbol**: the destructors say `+8`, `PoleLift_Spawn` says `+0`, and they differ by
eight bytes. This is invisible to every byte gate, because a relocated word is a
wildcard — the naive merge reports **7/7 MATCH** with a vptr store eight bytes
low. What caught it was `objisolate.plan`:

```python
PoleLift_Spawn   error='_ZTV8PoleLift: unexpected reloc type=2 addend=0'
```

The tooling fails closed, which is the right outcome, but it fails closed on a
file that had already been declared matching. Writing `(int)(_ZTV8PoleLift + 2)`
— `int`-indexed, so eight bytes — makes `PoleLift_Spawn` agree with the addend the
compiler generates for itself. Bytes unchanged (7/7 before and after), and
`objisolate` then accepts all seven:

```python
_ZN8PoleLiftD1Ev                   error=None  externalise=['_ZTV8Platform', '_ZTV8PoleLift']
_ZN8PoleLiftD0Ev                   error=None  externalise=['_ZTV8Platform', '_ZTV8PoleLift']
_ZN8PoleLift16CleanupResourcesEv   error=None  externalise=[]
_ZN8PoleLift6RenderEv              error=None  externalise=[]
_ZN8PoleLift8BehaviorEv            error=None  externalise=[]
_ZN8PoleLift13InitResourcesEv      error=None  externalise=[]
PoleLift_Spawn                     error=None  externalise=['_ZTV8PoleLift']
```

**Generalisation, and it is a warning for the workstream:** any TU that both
defines a class's vtable *and* contains a hand-written `_ZTV`-by-name reference
hits this. That is the shape of every `<Class>_Spawn` factory merged into its own
class's TU, which is a large fraction of the actor tree. Byte comparison will say
nothing. `objisolate`'s addend check is currently the only thing that does, and a
future `tubuild.py verify` must run it — plan §7.5's item 5, "relocation slots and
destinations match", is not optional here.

### 5.2 Taken, measured free: two shadow declarations retired

Both are §7.3's "local shadow declarations replaced by real headers", and both
were verified byte-identical rather than assumed:

- **`Render`** carried a local `struct Base` with six virtuals and
  `struct Derived { char pad[0xd4]; Base base; }`. Those are `Platform::mModel`
  at `+0xd4` and `Model`'s slot-5 `Render(const Vector3 *)`, both already correct
  in `include/Model.h`. Two globally-visible names as generic as `Base` and
  `Derived` cannot survive in a real TU. Replaced by `mModel.Render(0);` — a
  plain member call on a member object, which this compiler dispatches virtually
  (a qualified `mModel.Model::Render(0)` would not have). `0x28`/`0x28`, MATCH.
- **`CleanupResources`** reached its collider through
  `((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))->Disable()`, laundering
  what used to be a `u8` marker. `Platform`'s C++ half types that member as a real
  `MovingMeshCollider`, and `MovingMeshCollider : MeshCollider : MeshColliderBase`,
  so the cast chain was restating an inheritance the headers already know.
  Replaced by `mMeshCollider.Disable();`. `0x38`/`0x38`, MATCH.

### 5.3 Reconciled with no code change

- **Include sets.** Six different sets across seven files, reduced to one.
  `Platform.h`'s "`common.h` FIRST, and the order matters" constraint on
  `Matrix4x3` is satisfied either way here, since `Platform.h` includes `common.h`
  itself before `Model.h`.
- **Duplicated externs.** Ten mangled ROM symbols were declared across six files;
  `_ZN8Platform21UpdateModelPosAndRotYEv` and `_ZN8Platform19UpdateClsnPosAndRotEv`
  appeared twice each. **Nothing contradicted anything** — all repeated signatures
  agreed. One `extern "C"` block now.
- **Language mode.** `PoleLift_Spawn` was the only C file of the seven; it is now
  compiled as C++ and needs an explicit `extern "C"` to keep its unmangled ROM
  symbol. Its bytes are unchanged by the mode switch.
- **`data_02082214`** was declared in the `Behavior` file outside any linkage
  specification. Moved inside `extern "C"` — no ABI effect for a namespace-scope
  variable, but the intent is now stated rather than relying on that.
- **The destructor** was two files each carrying the same empty
  `PoleLift::~PoleLift() {}`, with `objisolate` keeping the variant the filename
  named. Now one definition.
- **No `#pragma` anywhere** in the seven legacy sources — §14's "no local
  `#pragma opt_*` workarounds" criterion is met on the evidence, not by
  assumption, and §10's warning about function-scoped pragma state does not
  apply to this candidate.

---

## 6. What did **not** get verified, and is not claimed

Per the corrected pilot framing, these are **WAIVED, not PASS**:

| §14 criterion | Verdict |
| --- | --- |
| no constructor/destructor variants | **WAIVED.** This TU is a key-function TU with a virtual destructor and emits D0, D1 and D2. |
| no unexpected data, BSS, RTTI, or vtable emission | **WAIVED.** One vtable, one inherited vtable and ten RTTI records are emitted (§4). Their *correctness* was not checked. |

Also **not run**: any link; `dsd check symbols --fail`; module byte comparison;
ROM build; symbol-address checks; relocation *destination* checks against
`config/**/relocs.txt`. §12's layers 4 through 7 are all untouched. A `text-verified`
status means layers 2 and 3 only.

---

## 7. What stands between `text-verified` and `link-verified`

For this candidate specifically, in ascending difficulty:

1. **`_ZN8PoleLiftD2Ev` has nowhere to go.** It is `0x4c` of `STB_GLOBAL` `.text`
   that the ROM range does not contain. `rombuild.py` links `-nodead`, so it
   cannot be dropped the way the original build dropped it. Either the TU
   contributes through per-function isolation (plan §9/§13, which works today —
   `objisolate` accepts all seven), or the linkcheck needs a rule for
   deadstrippable extras. **Not a new obstacle:** the legacy, already-enrolled
   `src/game/actors/PoleLift/_ZN8PoleLiftD1Ev.cpp` compiled alone emits the identical D2 today, and
   `objisolate` already strips it on every push. TU consolidation does not
   introduce this shape; it only makes it visible in one object instead of
   scattered across per-function ones.
2. **The destructor pair is emitted in the wrong order** and source ordering
   cannot fix it (§3). A whole-range link would place D2, D0, D1 where the ROM has
   D1, D0. **This one is genuinely new** — per-function objects never had to agree
   on a *relative* order with anything, and it only matters once a contiguous
   whole-range link is attempted.
3. **`_ZN8PlatformD1Ev` / `_ZN8PlatformD0Ev`** trail the object; both are
   `STB_LOPROC` so a linker that honours vague linkage should discard them, but
   that is an assumption this round did not test. **Also not new:** the legacy
   single-file destructor object emits the same two vague-linkage bodies today,
   and today's per-function link already discards them the same way. Untested
   here only in the whole-range sense, not in the sense that matters for the
   current, trusted build.
4. **The vtable, RTTI, `.data` and `.bss` phases** (plan §11 items 5 and 6) are
   untouched.

Re-reading 1–4 with that correction: the only genuinely new obstacle TU
consolidation introduces is **item 2, the destructor emission order** — items 1
and 3 are the pre-existing, already-tolerated shape of any single-file
destructor object in this tree, just relocated into a bigger object.
Nonetheless, a cleaner second pilot — a TU with no virtual destructor — is still
worth doing first: it isolates whether whole-range linking works at all before
adding item 2's genuinely new confound on top.

---

## 8. Answer to §18's open question

> How many already-matched functions remain matched when compiled together in
> their candidate original TU context?

**All of them, for this TU: 7/7.** Of §10's twelve listed hazards, exactly one
bit — shared vtable/key-function emission, and it bit through a *relocation*, not
through instruction bytes. The other eleven (pragma state, declaration order,
language mode, linkage, literal placement, static initialisation, inlining,
template instantiation, compiler-generated helpers) cost nothing measurable here,
including the `.c` → C++ language-mode change on `PoleLift_Spawn`, which §10 lists
as a hazard and which turned out to be free.

Two caveats on generalising from one data point:

- **This candidate is friendlier than average.** Seven small functions, no
  pragmas, no `.rodata`, no shared literals between functions, one class.
- **The failure mode found is invisible to the gate this experiment used.** 7/7
  MATCH was reported *before* the vtable addend was corrected, and the bytes were
  identical either way. Had the pilot stopped at `match.compare`, it would have
  published a false green. Any scaled-up `tubuild.py verify` must run
  `objisolate.plan` and a relocation-destination check, not just a byte compare.

On the evidence: **implement scratch linking and promotion**, but make the second
pilot a TU with no virtual destructor, so §7's items 1–3 do not confound the
first whole-range link.

---

## 9. Repository invariants checked

Plan §13 item 6 and §12's last negative control — *confirm no shadow file under
`src_tu/` is accidentally enrolled in the production build* — verified by reading
the tools rather than assuming a new top-level directory is ignored:

- `enroll.py` — `SRC = REPO / "src"`; candidacy is the existence of
  `src/<symbol>.c[pp]`. No glob reaches `src_tu/`.
- `eligible.py` — walks `config/**/symbols.txt` and resolves through `src/`.
- `srcpath.py` — the single answer to "where does symbol X live"; direct hit
  `src/<symbol>.c|.cpp`, fallback `rglob` over `src/` only.
- `rombuild.py` — compiles `rombuild.enrolled()`, i.e. paths written into
  `config/**/delinks.txt`, which this change does not touch.
- `asset_catalog.py` (`--src`, default `REPO / "src"`), `tiers.py`
  (`src_root or SRC`), `marker_census.py` (`(root / "src").rglob`) — every
  source-tree scan in `tools/` is rooted at `src/`.
- `grep -rn src_tu tools/` — no hits.
- `build/` is gitignored, so `build/tu/ov045-PoleLift/` cannot be committed.

`src/` was not modified: the seven legacy files are byte-identical to their
committed state and still `complete` in `config/arm9/overlays/ov045/delinks.txt`.

## 10. Change set

| Path | |
| --- | --- |
| `src_tu/actors/PoleLift.cpp` | new — the shadow TU, not enrolled |
| `config/tu_manifest.d/` | new — one entry, `ov045/PoleLift`, status `text-verified` |
| `notes/tu-reconstruction-pilot-report.md` | new — this file |

Untracked build output at `build/tu/ov045-PoleLift/` (`PoleLift.o`,
`inventory.txt`) is gitignored, per §13 item 10.
