# Where the names came from, and which parts of one to trust

A retail NDS cartridge has no symbol table. **Nothing in `config/**/symbols.txt` was read
out of a symbol table, because there isn't one.** Every name is either a placeholder, a
string the ROM genuinely stores, a structure the ROM genuinely proves, or somebody's
assertion. This note says which is which, and — more usefully — which *parts of a single
name* are which, because a mangled name is not one claim. It is four or five claims
welded together, and they have wildly different evidence behind them.

Written after #1226, where two imported names turned out to be asserting parameter types
the ROM refutes. Neither was a sloppy guess; both were unfalsifiable from where they were
written. That is the pattern worth internalising.

> **Counts below are measured at `af680244` (main), before #1226 lands.** They drift as
> migrations land — #1226 alone moves two methods out of the unmigrated column. Re-derive
> with `tools/langmode_audit.py` before quoting any of them; the *ratios* are the durable
> part, not the digits.

## The short version

```sh
_ZN22ExpandingHeapAllocator12AllocateNodeEP10MemoryNodeS1_Pvjt
   # ^class name          ^method name    ^parameter types
   # RTTI-checkable       a label         WEAKEST LINK
```

| part of the name | who could have proven it wrong | trust |
|---|---|---|
| the **address** it binds to | our own `dsd` analysis — must be an exact function start | **high** |
| the **class name** | ROM RTTI, where the class has any | **high** where RTTI exists |
| the **method name** | nothing. It is a label | medium — wrong names are cosmetic |
| **parameter count** | the arity test against the ROM body | **high** — see below |
| **parameter types** | only a byte-match of the callee | **low until migrated** |

If you are about to act on a parameter *type* from a name you did not verify, stop and
read the last three sections.

## The tiers, with counts

Repo-wide: **31,757 symbols across 106 modules.**

| tier | what | count | share |
|---|---|---|---|
| E | `func_<addr>` / `data_<addr>` placeholders | 26,162 | 82.4% |
| D | mangled `_Z…` methods and free functions | 2,663 | 8.4% |
| — | plain identifiers (SDK signatures, local C-style names, and 308 `.L_`/`.p__sinit_` compiler labels) | 1,697 | 5.3% |
| A | `_ZTI…` type_info records | 426 | 1.3% |
| A | `_ZTS…` type-name strings | 422 | 1.3% |
| B | `_ZTV…` vtables | 387 | 1.2% |

### Tier A — read out of the ROM. Trust the string.

The 848 `_ZTS`/`_ZTI` symbols. The suffix on a `_ZTS` name **is the literal byte string
the ROM stores** — that string *is* the Itanium type name, which is why no re-mangling is
needed and why nested names and `St` substitutions come out right for free
(`tools/rtti_symbols.py`). 429 classes and 413 inheritance edges were read this way.

Hard limit: **RTTI gives you class names and nothing else.** No method names, no
parameter types, no static-vs-instance. If someone cites "the RTTI" for a method
signature, they are citing something that does not exist.

### Tier B — structurally proven from ROM data. Trust the wiring, not the English.

2,241 actor symbols across 68 modules (#211) came from walking `ACTOR_SPAWN_TABLE`
(EU `0x02090864`, 391 entries). The table proves *which function spawns which actor*,
*which vtable belongs to it*, and *which slot is which* — the slot order was validated
against a byte-matched function on ov056.

But `BigMovingIceBlock` is a drafted label, not a ROM string. `config/rom-name-glossary.json`
is explicit about this and carries a per-entry confidence, noting: *"nothing in the binary
says what 'Ukiyuka' means."* So: the *relationship* the name encodes is ROM-proven; the
English word is a community reading. Renaming one costs nothing; relying on the wiring is
safe.

### Tier C — SDK/BIOS signatures

`CpuFastSet`, `VBlankIntrWait`, `MIi_CpuClear16`, `G3_MultMtx33` and friends, recognised
by `dsd` against the known Nintendo SDK. External ground truth, and the signatures come
with the SDK rather than being reconstructed. High trust.

### Tier D — imported community reconstruction. **This is where the errors live.**

At least **1,349 mangled names are verbatim imports** from
`reference/DynamicAllocationDecomp/symbols.x`, applied by `tools/import_symbols.py`.
Broken down:

| kind | imported | not imported |
|---|---|---|
| member / free function | 1,164 | 898 |
| destructor | 99 | 442 |
| constructor | 53 | 1 |
| vtable | 33 | 354 |

> **Measurement caveat:** `import_symbols.py` lists four upstream sources but only
> `DynamicAllocationDecomp` is present in this checkout. The "imported" column is a
> **lower bound**; some names counted as local may also be imports.

**What that upstream file actually is matters enormously.** DynamicAllocationDecomp is an
ASM-hack/mod project. For most of these symbols it has no decompiled body — it has a
hand-written header declaring the function so mod code can *call* into the ROM, plus a
linker script binding that spelling to an address. Their build links successfully no
matter what the parameter types say.

What the import process *did* verify: **the address.** `import_symbols.py` refuses any
name whose address is not an exact function-or-data start in our own analysis; 71.9% of
upstream's addresses cleared that bar and the rest were dropped. So the address is good
and the identifier is probably good. **The parameter types were never checked by anything.**

### Tier E — placeholders

`func_02005000`. No claim at all. Not a weak name — an *absent* one. Never cite one as
evidence of anything.

## Why a caller can never catch a wrong parameter type

This is the mental model that explains every correction in the track record.

Upstream's declarations exist to **call** these functions. Consider the two errors #1226
fixed:

- `AllocateNode`'s fifth parameter, declared `u32`, is really `u16`. AAPCS widens a
  sub-word argument to a full word on the way in, so *the caller emits identical
  instructions either way*. The callee ignores the high half. Nothing observable.
- `DeallocateAll`'s callback, declared as pointer-to-pointer-to-function, is really a
  plain function pointer. A function pointer and its address are the same word.

Neither project's build could ever have failed. The types were **unfalsifiable from the
call site.** They are only observable from *inside the callee*, in the instruction the
compiler picks — and nobody was looking there until someone tried to declare the function
as a real C++ member.

> **The rule:** treat an imported parameter type as unverified wherever a member signature
> cannot reproduce the bytes. When the name and a reproducing body disagree, **the ROM's
> codegen is the tiebreaker, and the name is the thing that yields.**

## Static-vs-instance is on much firmer ground than the types

Worth stating plainly, because it comes up constantly and the anxiety is misplaced.

**Static-vs-instance comes from the arity test, not from trusting the name.** Count the
parameters the mangled name declares, count the arguments the ROM body actually uses. Equal
→ static. Body uses one more → the leading one is `this`.

That test is self-validating. It reads the name's parameter *count* and checks it against
the ROM. If the count were wrong the test would return nonsense — a negative implied
parameter, or a `this` never used as one — and you would notice immediately.

Empirically the corrections are *mostly* to a parameter's type: `Pvjj → Pvjt` preserves
arity, `PPFvPvPS_jEj → PFvPvPS_jEj` preserves arity, and the D1/D2 corrections were
identity errors rather than signature errors.

**This note previously said the count had never been wrong. That is false, and two
symbols in this tree disprove it** — both witnessed by a byte-matched file, not by
inference:

| symbol | name declares | tree says | witness |
|---|---|---|---|
| `_ZN7Clipper13Func_020156DCEv` | 0 | **5** | `src/Camera_UpdateMatrices.c` declares `(void*,int,int,int,int)` and matches the ROM |
| `_ZN12dEnemyBase_c12KillByAttackER8dActor_c` | 1 (+`this`) | **4** | its own definition takes 4, and three call sites pass 4 |
| `_ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player` | 2 (+`this`) | **3** | 15 call sites outside ov004 materialise r3; the four inside it do not |

### And two of the three were on the wrong function entirely (2026-08-27)

Both `dEnemyBase_c` rows above sat on an **ov004** address, and ov002 and ov004 are
alternates in one overlay slot -- both based at 0x020ad3e0 -- so a single address is
two unrelated functions and every actor overlay's `relocs.txt` records the call as
the ambiguous `module:overlays(2,4)`. The config cannot disambiguate it; the code
can.

`KillByInvincibleChar` was on ov004 0x020ada40, which reads only r0 and range-checks
it as a scalar. The ov002 body at the same address consumes r0/r1/r2 as (enemy,
Vector3_16 &, Player &). Moved, given its third parameter, and migrated to a real
method that matches -- which is the strongest form of check this note describes.

`KillByAttack` was on ov004 0x020aea30, whose body walks an 8-byte table to an
0xffff sentinel: r0 is a table base, not an actor. The ov002 function at the same
address dispatches a pointer-to-member from `data_ov002_0210db80[mDeathState - 1]`
after clearing bit 0x10000000 of mFlags and zeroing mDeathTimer -- an enemy method,
and the one all 34 enemy-overlay call sites reach. ov004's is now
`func_ov004_020aea30`; ov002's keeps its placeholder rather than inherit a name
coined against the wrong body, but its arity is settled: an unused fourth argument
makes it match, and the call sites spell the first three
`(dActor_c *attacker, dBgCh_Actr *clsn, int kind)`.

**The lesson for this note.** An arity disagreement is worth treating as a symptom,
not just a defect in the count. Both of these turned out to be the name pointing at
a different function, and in both cases the ambiguity was visible in `relocs.txt`
as `module:overlays(2,4)` before anyone disassembled anything. Grep for that.

So the arity test is sound *as a test* — it reads the ROM, not the name — but the name's
count is an input that can itself be wrong, and when the two disagree the ROM wins. Run it
per function and treat a disagreement as a defect in the name.

So the arity test is sound. What it does *not* license is inheriting a neighbour's answer:
`ExpandingHeapAllocator::AllocateForwards` mangles `Ejj` and is an **instance** method;
`SolidHeapAllocator`'s identically-named method mangles `EPvjj` and is **static**. Same
name, opposite answer, one class apart (#1221, #1215). Run the test per function.

## The one check that actually verifies a signature

Declaring the function as a **real C++ member** and byte-matching it. Parameter types feed
codegen — load widths, argument homing, whether a call needs an extra indirection — so a
member declaration that reproduces the ROM has had its types *checked by the compiler
against the ROM*. A hand-spelled `extern "C"` name has not: the string is whatever you
typed, and the linker resolves it by address regardless.

**A strictly weaker check now covers 100% of the tree: `tools/name_roundtrip.py`.** It
synthesises each mangled name's declaration, compiles it with the pin, and reads the
emitted symbol back — 4,428 of 4,436 round-trip. Do not quote that 99.8% as "the names are
verified". Round-tripping proves only that the pinned compiler *can emit* the name;
`void f(unsigned int)` and `void f(int)` both compile, and Itanium encodes neither the
return type nor static-vs-instance. It is a floor, not a signature check. What it does buy
is that a `MISMATCH` or `REJECTED` verdict is a hard defect regardless of who wrote the
name, and `--check` keeps a new one from landing.

`tools/langmode_audit.py` measures exactly this gap:

```python
NOT MIGRATED   1219  of 2525 mangled-symbol files (48.3%)
    .c extension              943
    .cpp, symbol hand-spelt   276   renamed, never migrated
```

**Roughly half of all mangled symbols in this tree have never been checked by a compiler.**
Per kind (unmigrated / migrated):

| kind | unmigrated | migrated | status |
|---|---|---|---|
| method | 693 | 1,229 | mixed |
| D0 | 258 | 3 | effectively unproven |
| D1 | 194 | 72 | mixed |
| D2 | 20 | 0 | **UNPROVEN** |
| C1 | 41 | 0 | **UNPROVEN** |
| C2 | 11 | 0 | **UNPROVEN** |
| C3 | 2 | 0 | **UNPROVEN** |

A `.cpp` extension is **not** evidence of migration — 274 files are `.cpp` while still
hand-spelling their symbol inside `extern "C"`. Check for a real member definition.

## Grep will miss references. Use the linker.

`langmode_audit.py` also counts **2,643 shadow declarations** — files carrying a local copy
of a type or class instead of including the shared header — of which **1,626 include
nothing at all**.

This is not theoretical. In #1226, `ExpandingHeap::VDeallocateAll` carried its own private
redeclaration of `ExpandingHeapAllocator`, so the wrong symbol `PPFvPvPS_jE` **never
appeared as a literal string anywhere in the tree**. A repo-wide grep found nothing. It
surfaced only as an `mwldarm` undefined-symbol error during the ROM build.

> **If you rename a symbol, `tools/rombuild.py` is the check that matters.** Grep is a
> hint; the linker is the proof.

That file's call site had been carrying the tell all along:

```cpp
(Visitor*)&ExpandingHeapAllocator::InvokeDeallocate
```

`&InvokeDeallocate` already *is* a `Visitor`. **A cast whose only purpose is to satisfy a
signature is evidence against the signature.** Worth grepping your own work for.

## Track record

Imported names have been corrected repeatedly. This is normal, not scandalous — but it
calibrates how much weight to put on one.

| PR | correction | class of error |
|---|---|---|
| #178 | six `_ZThn80` thunks pointed at the wrong classes | identity |
| #201 | duplicate `_Znwj`; real operator-new was elsewhere | identity |
| #206 | both `Scene` D0 dtors were false matches | identity |
| #774 | `Actor` / `ActorBase` D1 and D2 names swapped | identity |
| #1203 | 7 of 17 "D2" symbols appear in a vtable, so they cannot be D2s | identity |
| #1226 | `AllocateNode` `Pvjj→Pvjt`; `DeallocateAll` `PPF…→PF…` | **parameter type** |

Note the asymmetry. Identity errors get caught because the link or the byte compare
screams. **A wrong parameter type is silent until someone tries to migrate the function.**
Expect more of them, one at a time, at exactly that moment.

## What to do when a member signature won't reproduce

The procedure that worked, in order:

1. **Suspect the name before you contort the body.** If you find yourself adding a
   truncating local, an address-taken cast, or a use-site cast to force a load width — the
   declaration is what's wrong. Those were all measured on `AllocateNode` and cost 999, 5
   and 0 words respectively. None can work, because none changes the declared type.
2. **Read the divergent instruction as evidence.** `ldrh` vs `ldr` on a parameter is the
   compiler telling you the declared width. `blx r6` with no preceding `ldr r6,[r6]` is it
   telling you the thing is a function pointer, not a pointer to one.
3. **Check the provenance.** Is the name in `reference/*/symbols.x`? Does that project have
   an actual body for it, or only a calling declaration? If the latter, its parameter types
   are decoration.
4. **Change the declaration and let the compiler mangle it.** Do not hand-derive the new
   mangled string — substitution numbering (`S_`, `S0_`, `S1_`) re-resolves in ways that are
   easy to get wrong. Compile and read the emitted symbol out of the object.
5. **Verify with the full build**, not just `match.py` — see the grep warning above.

## Files worth knowing

| path | what it is |
|---|---|
| `config/**/symbols.txt` | **what we assert.** The only file that states our claim |
| `symbols/verified.tsv` | what was **imported**. A record, not an assertion — left stale on purpose |
| `config/match_attempts.jsonl` | append-only attempt history, keyed on address, never on name |
| `reference/DynamicAllocationDecomp/symbols.x` | upstream. 25 of its 2,125 entries are commented out, with margin notes like *"this is NOT the start of a function!!!"* — it is honest about being a working artifact |
| `config/rom-name-glossary.json` | the hand-authored EAD glosses, with per-entry confidence |
| `notes/cpp-naming-guide.md` | Itanium mangling reference — the type codes (`j` = u32, `t` = u16, …) |

## See also

`notes/actor-naming.md` · `notes/rtti-reconciliation.md` ·
`notes/runbook-type-reconstruction.md` · `notes/plan-cpp-language-mode.md` · `CREDITS.md`
