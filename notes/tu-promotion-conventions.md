# Genuine-TU promotion: conventions for the PR

Grounded only in what has landed on `main`. Every rule below cites a promotion that is
already merged; nothing here is proposed from taste. Read it before you open the next
promotion PR, and check it as a reviewer before you approve one.

A *genuine-TU promotion* collapses N per-function files into one real C++ translation
unit and replaces that class's N per-function `delinks.txt` entries with a single
`complete` span. `tools/tu_promote.py` drives the mechanical half (the `git mv` R100 so
attribution follows, the `git rm` of absorbed sources, the manifest flip to
`status: promoted`, the per-symbol attribution overrides, the CONVERTED identity
migration to `promoted-path#symbol`). This note covers the half the tool does not do,
which is the half every review has spent its time on.

## The landed corpus

Seven classes across six PRs. When this note says "landed precedent" it means these:

| PR | class | promoted TU |
| --- | --- | --- |
| #2000 | daKpFr_c | `src/actors/daKpFr_c.cpp` |
| #2043 | daObjFm_Battan_c | `src/actors/daObjFm_Battan_c.cpp` |
| #2045 | daBar_c | `src/actors/daBar_c.cpp` |
| #2047 | daObjCannonShutter_c | `src/actors/daObjCannonShutter_c.cpp` |
| #2047 | daObjFl_Fall_Block_c | `src/actors/daObjFl_Fall_Block_c.cpp` |
| #2051 | daObjKinokoTag_c | `src/actors/daObjKinokoTag_c.cpp` |
| #2055 | daEyBm_c | `src/actors/daEyBm_c.cpp` |

PR #2004 (daObjKm3_Kurumajiku_c) is an open draft. It is a data point, not precedent.
Do not cite it as settled, and do not copy a pattern that appears only there.

*Update.* #2004 was since closed and that class landed instead through #2057 ("first
compiler-built vtable — promote ov047/daObjKm3_Kurumajiku_c to intact-object
production"), so `src/actors/daObjKm3_Kurumajiku_c.cpp` is on `main` and is precedent.
Section 2's Kurumajiku measurements were written while it was a draft; they still hold,
and section 6 cites the landed file.

---

## 1. A coined mangled name must not assert a parameter type the bytes cannot prove

#2055 renamed four functions in `config/arm9/overlays/ov071/symbols.txt`, among them:

    func_ov071_02121b50  ->  _ZN8daEyBm_c15UpdateCollisionER10dBgCh_Actr

The trailing `R10dBgCh_Actr` asserts a **reference** parameter. The ROM cannot prove
that. A reference and a pointer mangle differently but generate identical ARM for this
body, so the bytes are silent on which one the original source wrote. `P10dBgCh_Actr`
would have matched exactly as well. The image carries no mangled names of its own to
appeal to, and RTTI gives class names and nothing else — see
`notes/symbol-name-provenance.md`, whose trust table already rates parameter *types*
low while rating address, class name and parameter *count* high.

**The rule.** You may coin a member name — that is how this tree works. But the moment
the mangled spelling encodes something the bytes cannot distinguish (parameter types
above all, and `const` qualification with them), the guess has to be disclosed next to
the symbol, not left to be re-derived by the next reader.

**Where the disclosure has to live.** Measured: `config/arm9/overlays/ov071/symbols.txt`
contains zero comment lines, and dsd's symbol format has no comment channel at all. The
most authoritative-looking surface in the whole promotion is the one surface that
physically cannot carry the caveat. So it goes in all three of the places that *can*,
and that stay adjacent to the name:

1. **The manifest entry's `notes`.** Landed precedent: `config/tu_manifest.d/ov071/daEyBm_c.json`
   already says the four coined spellings are "truthful inferred private spellings"
   whose "class ownership, inbound calls, bodies, layout, relocations and codegen are
   proven; the exact original English names and UpdateCollision reference spelling are
   not." Copy that shape. Name the *specific* unprovable thing, not just "inferred".
2. **The `why` column of `symbols/actor_renames.tsv`**, on the row that performs the
   rename.
3. **The comment on the declaration**, in whichever header or TU declares the member.

Point 3 is the one #2055 did not fully do: `include/daEyBm_c.h` discloses that the
private helper *names* are inferred aliases, but says nothing about the reference-vs-
pointer guess in the mangled spelling. The header is what a reader of the type actually
reads. Close that gap in your own PR.

**Reviewer check.** For every coined symbol in the diff, read the mangled suffix. If it
encodes an argument type, find the disclosure in all three places above or ask for it.

---

## 2. Declare `extern int _ZTV<C>[];` exactly once, in the TU source, at namespace scope

### What actually landed

Four different locations and two different spellings across seven classes:

- **In the TU source, inside `extern "C" { }`** — daKpFr_c, daObjFm_Battan_c, daBar_c.
- **Inherited from `include/decl_common.h`** via `#include` — daObjCannonShutter_c,
  daObjKinokoTag_c.
- **In the class header, at namespace scope** — daEyBm_c (the newest).
- **Not declared at all**, because the TU has no factory — daObjFl_Fall_Block_c.

Spellings: `(int)&_ZTV<C>[2]` at five sites, `(int)(_ZTV<C> + 2)` at one
(`src/actors/daObjCannonShutter_c.cpp`). On an `int[]` those are the same arithmetic,
+8 bytes either way.

One fact that changes the argument: **`include/decl_common.h` already declares
`extern int _ZTV<C>[];` for all seven promoted classes.** Every source-file form and
the daEyBm_c header form are therefore *second* copies of a declaration the tree
already carries.

### The convention

> **One declaration, in the promoted TU source file, at namespace scope, immediately
> above the factory that uses it. Not in the class header. Not inside `extern "C"`.**

### Why — blast radius, not taste

- **It is the majority landed form.** Three of the six promoted classes that have a
  factory declare it in the TU source.
- **Nothing sweeps `_ZTV` on a rename.** `tools/cpp_rename.py` is a *file* renamer keyed
  by address; it has no `_ZTV` handling whatsoever. Every additional copy of the
  spelling is an independent chance to strand it when the class is renamed. That is the
  exact failure `notes/declaration-centralization.md` measures at scale — 8,523 distinct
  declared names across 5,768 files, 27% of them contradicting each other. The tree
  already keeps one copy in `include/decl_common.h`; a third copy buys nothing.
- **A header change is a fan-out build.** `MERGE.md`: any change under the header tree is
  byte-checked on its *consumers* — `tools/affected_src.py` expands the reverse include
  graph and every including source is compiled. Today `include/daEyBm_c.h` has exactly
  one includer, so the cost is invisible; it will not stay that way as neighbouring
  classes migrate. The vptr seam is an implementation detail of one factory in one TU,
  not part of the class's public surface, and it should not be priced as public surface.
- **`#include "decl_common.h"` costs the TU its typing freedom.**
  `src/actors/daObjCannonShutter_c.cpp` documents the bill it paid: a parameter "stays
  `void *` because `include/decl_common.h` — which C translation units also read —
  declares it that way". Reasonable when the TU needs that header anyway; not worth
  pulling in for one line.
- **`extern "C"` around it is inert.** A namespace-scope variable is not mangled under
  the Itanium ABI, so `extern int _ZTV8daEyBm_c[];` emits that symbol verbatim with or
  without the linkage block. daEyBm_c proves it empirically: it declares at plain
  namespace scope and lands validator-green. Omit the block; it implies a rule that
  does not exist.

### The vptr store, and why the +8 is computed and never patched

    extern int _ZTV<C>[];
    ...
    *(int *)actor = (int)&_ZTV<C>[2];

mwcc's own emitted `_ZTV<C>` symbol addresses the vtable **object**: offset-to-top word,
typeinfo word, then the slot array. The `_ZTV<C>` recorded in `symbols.txt` is the
public **address point**, eight bytes later. `&arr[2]` on an `int[]` is exactly +8, so
the compiler computes the bias itself and no relocation ever has to be hand-edited.

Three independent places in the tree pin that 8:

- `tools/objisolate.py`: `VTABLE_PREAMBLE = 8`, subtracted when a vptr store's addend is
  rewritten for the ROM link.
- `tools/reloc_audit.py`: `_VT_PREAMBLE = 8`, with measured examples — a `dBgCh_Lin`
  constructor's addends 8 and 0x18 landing on the configured destinations, and a
  `ModelAnim2` deleting destructor whose addend 44 resolves onto `_ZTI8dFader_c` if the
  preamble is *not* subtracted.
- `tools/tubuild.py`, which refuses to infer it: the manifest must spell both
  `emitted_storage_address` and `address_point_bias`, because "recognizing `_ZTV` and
  subtracting eight here would silently reinterpret every existing symbol table."

And it is stated in a landed manifest. `config/tu_manifest.d/ov071/daEyBm_c.json` records
`_ZTV8daEyBm_c` at `0x02122de8` with the reason: "InitResources naturally emits the
complete class vtable; romdata_check applies the measured eight-byte public address-point
bias and proves every slot through 0x02122e64."

### Anti-patterns — three things that make a gate agree and the ROM wrong

If the vptr store or the vtable does not line up, **fix the construct**. Do not move the
ground truth to meet it:

1. **Do not edit the `_ZTV<C>` address in `symbols.txt`.** That address is the measured
   address point. Changing it to make an addend resolve reinterprets the symbol table.
2. **Do not widen or shift the promoted `complete` range in `delinks.txt`** so a data
   symbol falls inside or outside it.
3. **Do not delete a neighbouring `data_<module>_<addr>` entry _to silence a gate_.**
   Deleting one is sometimes *required* — see the second nuance below. The test is not
   "was an entry dropped" but "is the entry unkeepable".

A green gate does not distinguish the right approach from the wrong one here. A reviewer
must check three facts directly: (a) the `symbols.txt` `_ZTV` address is unchanged; (b)
the promoted `complete` span in `delinks.txt` covers exactly the promoted functions;
(c) any dropped neighbouring data or bss entry is *unkeepable* — proven by restoring it and
watching the link fail, not by argument.

**One nuance, because the census contradicts the blunt form of rule 1.** *Renaming* a
`_ZTV` symbol at an unchanged address is legitimate and has landed: #2045 changed
`_ZTV13InvisiblePole` to `_ZTV7daBar_c` at `0x02108480`. The anti-pattern is moving the
address, not renaming the symbol.

**Second nuance, and it inverts rule 3 in one specific case.** A promotion that makes the TU
supply the vtable *storage* will find a delinker-invented placeholder sitting 8 bytes below
the address point — at the offset-to-top word. That entry is not merely droppable, it is
**unkeepable**, and the promotion cannot link while it exists.

Measured on `ov047/daObjKm3_Kurumajiku_c`, whose `_ZTV21daObjKm3_Kurumajiku_c` address point
is `0x021122a0` and whose placeholder `data_ov047_02112298` sits at the storage start.
Restoring the placeholder on top of the promotion and rebuilding produces a **new** symbol
error, beyond the pre-existing baseline set:

    [ERROR] Symbol 'data_ov047_02112298' in overlay 47
            at 0x02112298 not found in linked binary

Once the TU supplies those 8 bytes, `0x02112298` is the storage start of the
compiler-emitted vtable, and no symbol of that name exists in the linked binary at all.
`dsd check symbols --fail` rejects it. The placeholder was a delinker-invented name for
cartridge bytes that are no longer cartridge bytes.

**How to tell the two cases apart, without trusting a gate.** An entry deleted to silence a
gate makes some metric *better*. An unkeepable entry usually makes one *worse*, because the
dropped name was doing real work as a span boundary. In this promotion, keeping the entry
would bound `RickshawBs_SpawnInfo` at `0x02112298 - 0x0211227c = 28`, exactly its emitted
size, scoring it VERIFIED; dropping it pushes the next configured symbol out to the `_ZTV`
at `0x021122a0`, giving `romExtent 36 > emitted 28` and scoring it PARTIAL. The verified
count falls by one. Nobody deletes a line to make a metric worse — so a drop that *costs* a
verified symbol is evidence for legitimacy, not against it.

So the reviewer's question is not "was an entry dropped". It is: **restore it and rebuild.**
If the link fails with a new symbol error, the drop was required. If the build still passes,
the entry was keepable and the drop needs a different justification.

---

## 3. Delete your own stale `func_<module>_<addr>` declarations

Promotion renames the absorbed functions in `symbols.txt`, but the old
`func_<module>_<addr>` names survive elsewhere. `include/decl_common.h` still declares
three functions that #2055 renamed out of existence:

    extern void func_ov071_02121b08(void*);
    extern void func_ov071_02121b50(void*, void*);
    extern void func_ov071_02121ba4(void*);

Verified against `origin/main`: a tree-wide search finds **no source reference** to any
of the three. (Note the count — three, not four. `func_ov071_02121c6c` was renamed by the
same PR but was never declared in that header, so a promotion must check each absorbed
symbol individually rather than assume they arrive as a set.)

**The rule.** A promotion removes the declarations of the functions it renamed, in the
same PR. Do it in the promotion PR rather than as a follow-up: the promotion is already
a byte-proof PR carrying a full ROM link, and a standalone header edit would have to buy
that proof again. Note that `include/decl_common.h` carries an AUTO-GENERATED banner
naming a generator that **no longer exists in the tree**, so this is a hand edit, and
touching it makes yours a header PR with the reverse-include fan-out build described
above. Budget for it.

**What is NOT stale and must stay.** The manifest's `legacy_source` fields, the
`path#symbol` keys in the attribution ledger, and the provenance and match-attempt
ledgers all name the pre-promotion paths *on purpose*. They are history. Do not purge
them to make a search come back clean.

---

## 4. Update the provenance comments the promotion just invalidated

`include/daEyBm_c.h` still carries, after #2055:

    [_ZN8daEyBm_c6RenderEv.cpp, _ZN8daEyBm_c8BehaviorEv.cpp,
     _ZN8daEyBm_c13InitResourcesEv.cpp]

All three files were deleted by that same PR. **The rule: a promotion updates its own
provenance comments to name the promoted TU.**

No gate will catch this for you, twice over: those citations carry no directory prefix,
so the dead-reference scanner's path pattern never fires on them, and the header tree is
not one of that gate's prose surfaces regardless. This one is a reviewer's job.

Again: this applies to prose comments a human reads. It does not apply to the manifest's
machine-read `legacy_source` fields, which are the historical record.

---

## 5. Ledger-touching PRs land one at a time, with regeneration between

Every promotion touches `config/converted-baseline.json` and its overlay's `delinks.txt`
and `symbols.txt`.

- The repo's `merge=union` driver **only runs on a local `git merge`.** GitHub's
  server-side merge — the thing that actually lands a PR — ignores it. That is written
  into `.gitattributes` in as many words.
- And none of these three files are union-merged in the first place. They conflict
  outright the instant two promotions touch the same overlay.
- `config/converted-baseline.json` is a **SET ratchet, not a count**. `tools/tiers_ratchet.py`
  enforces `count == len(converted) == len(set(converted))`, and the identities are now
  per-member `path#symbol`. A hand-resolved merge produces a baseline that is neither
  branch's and still passes a casual read.

**The rule.** Do not batch promotions. Land one, then **regenerate** — `python
tools/tiers_ratchet.py --update` — and rebase the next one onto the result. Never
hand-merge the baseline array.

With roughly a dozen promotions queued, this is the rule most likely to cost a day. Plan
the queue as a chain, not a fan.

---

## 6. Mark every hand-written member with `// @symbol`

A promoted TU is scored **per member**, not per file. The only thing that tells
`tools/tiers.py` where one member's text ends and the next begins is a marker comment.
Leave it out and that member is scored against the entire file, so any other member's
defect becomes its defect. This is the cheapest rule in this note to satisfy and the
most expensive one to skip.

### The rule

> **Every hand-written member of a promoted TU carries `// @symbol <linker-name>` on its
> own line, immediately above its definition, spelled exactly as the manifest's
> `functions[].symbol` spells it.**

`tools/tiers.py:74` is the entire recognizer:

    SYMBOL = re.compile(r"//\s*@symbol\s+(\S+)")

### Why — the fallback is the whole file

`score_member` (`tools/tiers.py:406`) tries three things in order, and its own docstring
states the last one plainly:

> Hand-written members use exact ``@symbol`` boundaries.  Compiler-generated
> ctor/dtor variants use the inline lifecycle definition in a directly included
> class header.  **Anything without either form of evidence is scored against the
> entire file, preserving the old conservative behavior.**

`_marked_member_fragment` (line 309) slices from the end of this member's marker to the
start of the next marker, and `score_file` sees only that slice. With no marker and no
lifecycle fallback, `score_file` is handed the whole TU.

Four of the five CONVERTED criteria read that fragment, so all four are poisonable by
text belonging to some other member: `no_raw_offset`, `no_unk_field`, `no_codegen_trick`,
`no_mangled_refs`. The other two readings are not, because `score_member` overwrites them
after scoring — `real_name` is recomputed from the symbol and `shared_header` from the
whole file regardless. So one `volatile` object, one `unk_<off>` field or one mangled
call anywhere in a promoted TU strips the tier from every clean member in it.

### The landed measurement

Scored with `tools/tiers.py` against this tree:

| promoted TU | members | `@symbol` markers | members at 5/5 | banked |
| --- | --- | --- | --- | --- |
| `src/actors/daObjKinokoTag_c.cpp` | 9 | 7 | 6 | 6 |
| `src/actors/daObjFm_Battan_c.cpp` | 9 | 7 | 5 | 5 |
| `src/actors/daObjKm3_Kurumajiku_c.cpp` | 5 | 3 | 4 | 4 |
| `src/actors/daEyBm_c.cpp` | 13 | **0** | 2 | 2 |

"banked" is the count of `promoted-path#symbol` identities in
`config/converted-baseline.json`. In the first three TUs the unmarked members are exactly
the two destructor variants, which the lifecycle path covers. `daEyBm_c` marks nothing:
eleven of its thirteen members fall to the whole-file fallback and every one of them
fails the same two criteria, `no_unk_field` and `no_mangled_refs` — properties of the
factory and of `Behavior`, not of the members being scored. Inserting the eleven marker
lines locally and changing nothing else raises the TU from two members at 5/5 to four.
That promotion also spent five rows in `config/converted-backslide-exceptions.jsonl`.

### The marker must be unique in the file

`_marked_member_fragment` returns `None` unless the symbol appears exactly once:

    matches = [i for i, marker in enumerate(markers)
               if marker.group(1) == symbol]
    if len(matches) != 1:
        return None

A marker copy-pasted from the member above, or repeated on both a forward declaration and
the definition, silently re-enables the whole-file fallback for that member. Nothing warns
and nothing goes red; the member simply scores as if it were unmarked.

### Keep shared mangled externs above the first marker

A member's slice runs to the *next* marker, so anything written between two definitions is
charged to the earlier one, while text above the first marker belongs to no member at all.
`src/actors/daObjKinokoTag_c.cpp` puts its whole `extern "C" { ... }` block of mangled
ABI-seam declarations at the top of the file, above the first marker on line 40, and none
of those spellings costs any member its `no_mangled_refs`. Declaring an ABI seam
immediately above the one function that calls it — as `src/actors/daEyBm_c.cpp` does —
hands the mangled spelling to the preceding member instead.

### Constructors and destructors take the second path, and usually cannot be marked

`_lifecycle_member_fragment` (line 378) is why D1 and D0 score without a marker. It
demangles the symbol, confirms it is a ctor or dtor, walks the TU's `#include "..."` names,
and looks for a balanced inline definition in one of those headers. `include/daEyBm_c.h`
line 54 carries `virtual ~daEyBm_c() {}`, and that one line is the entire reason two of
that TU's thirteen members are banked.

**This is the intended path for them, not a fallback to be worked around.** All four
landed promoted TUs mark zero structors, and in an inline-destructor TU there is nothing
to mark: the D1 and D0 symbols have no definition text in the `.cpp` at all, so there is
no line for a marker to sit above. A bare marker written somewhere else is worse than
none — the slice runs from it to the next marker, so it would charge the following
member's text to the destructor.

Inlining the destructor is usually deliberate rather than incidental. Written out of
line, mwcc emits the synthesized D0 *ahead* of the written D1, which is the reverse of
the cartridge, and `tubuild.py linkcheck` refuses a TU whose licensed `.text` is not in
ROM address order. Inlining moves the key function to the first other declared virtual,
lets the vtable's slots 16 and 17 odr-use D1 then D0 in ROM order, and deletes the
homeless D2.

So the rule for structors is: **do not mark them, and do keep the inline definition in a
directly included header.** What to check is that the fragment still exists.
`_balanced_lifecycle_fragment` (line 328) needs exactly one occurrence of the class-name
token followed by `(` in the masked header text, plus a brace-balanced body; a
declaration alone returns `None`, and so does a second overload. If a promotion genuinely
must move the destructor out of line, its two variants fall back to whole-file scoring —
and only then can they carry markers, above their out-of-line definitions.

### Put the inline structor's opening brace on the signature line

Inlining the destructor to make it scoreable moves a function *body* into a struct that
`tools/check_header_offsets.py` parses, and that gate recognises the body only when the
signature line itself carries the `{`:

    if n == 0:
        depth = line.count("{") - line.count("}")
        if depth > 0:
            skip_body = depth
        continue

Written Allman the signature line has no brace, `skip_body` never arms, and the body's
lines fall through to declaration parsing and are reported UNPARSED. Measured by
rewriting nothing but the destructor of `include/dScMgBase_c.h` -- a derived class that
declares its destructor first, which is the shape every inline-destructor promotion
produces:

    one line, with body    40 commented fields, 0 mismatched, 0 unparsed, spans 0x4660
    one line, empty {}     40 commented fields, 0 mismatched, 0 unparsed, spans 0x4660
    Allman, with body       0 commented fields, 0 mismatched, 2 unparsed, spans 0x50
    Allman, empty           0 commented fields, 0 mismatched, 1 unparsed, spans 0x50

Two things go wrong and only one of them is loud. The gate exits 1 on the UNPARSED line
-- and it is green on `origin/main`, so this is a **merge-tree-only** red that
`tools/premerge_check.py` will show you and your branch's own CI will not. The quiet half
is the worse one: **one unparsed line suppresses the field walk for the whole header**,
which is this tool's own documented silent-no-op shape. `0 commented fields` there is not
a clean pass, it is no check at all -- forty fields stopped being checked and the reported
span fell back to the base's `0x50`.

Two conditions narrow the hazard, and both hold for precisely the promotions this document
is about. It needs a **derived** class -- a non-derived one is declared unmodelled at its
first method line and never reaches the brace -- whose **structor is declared before any
field**, the key-function convention inherited from `dScene_c.h`. Where the destructor sits
after the fields, a method line ends the field list and the stray brace is never read;
`include/ArrowLift.h` measures identically in both styles. Note also that an **empty**
Allman body breaks it exactly as an empty one-line body does not. It is the newline that
costs, not the statements.

So write

    virtual ~PoleLift() {
        ...
    }

and not the Allman form. Every inline destructor in `include/` today is `virtual ~X() {}`
on a single line, so nothing has exercised this before; the inline-destructor wave will
exercise it repeatedly. This is a defect in the gate rather than in the style, and it is
recorded as one -- but until the gate is fixed, brace position is load-bearing.

### A marker on a still-unnamed member buys nothing

`score_member` recomputes `real_name` from the symbol, not from the fragment, so a member
still called `func_ov006_0210a534` fails that criterion whichever text it is scored
against. Marking it is still worth doing, because the boundary it creates is what protects
its *neighbour* — that is the whole point of the rule above — but do not expect the marker
to move that member's own score. Renaming it is the thing that does.

### Markers cannot move a ROM byte

`_code_only` (line 198) blanks comments before three of the five criteria run, and the
preprocessor deletes them before mwccarm sees the TU. Adding markers to an already
byte-proven TU cannot change an instruction and needs no new byte proof. It does change
the CONVERTED count, so regenerate `config/converted-baseline.json` per rule 5 in the same
PR.

### The failure mode, concretely

An unmarked promotion is *green*. Every byte gate passes, because the bytes are right. The
CONVERTED count drops, the author covers the drop with one backslide-exception row per
lost member, writes a true-sounding reason on each, and the PR merges. What actually
happened is that comment lines were traded for ledger rows.

- **#2062** (`daObjHatenaSwitch_c`, thirteen functions) adds **zero** markers. Its diff
  moves `config/converted-baseline.json` from `"count": 2568` to `"count": 2560` and adds
  **eight** rows to `config/converted-backslide-exceptions.jsonl`, one per lost member.
  Every row carries the same reason, and that reason names the cause without naming the
  fix: the TU "needs the volatile stack value to avoid a 17-word codegen mismatch". The
  `volatile` is in one member. The other eight paid for it.
- **#2064** (`dScMgSingle3DBase_c`, nine functions) adds **six** markers and **zero**
  backslide-exception rows. The three it leaves unmarked are the two destructor variants,
  which is right, and one member still named `func_ov006_0210a534`, which is not.
  Nothing backslides — neither it nor its neighbour was CONVERTED before — but the
  omission still costs, and it costs the *other* member: with no marker after it, the
  preceding `func_ov006_0210a600` fragment runs from its own marker to end of file and
  swallows a534's `volatile` body and raw addresses. `func_ov006_0210a600` is an
  eight-byte `return 1;` that can never score readable while that is true. This is the
  general shape of the omission — the member that pays is rarely the member you left
  unmarked.

Both are open at the time of writing, so neither is landed precedent in the sense section
1 uses. They are cited here as the worked pair: same operation, same gates, one of them
paying eight ledger rows for the omission.

### Reviewer check

Count the `@symbol` lines in the promoted TU against the length of the manifest's
`functions` array. The difference should be the ctor/dtor variants and nothing else. If a
promotion adds backslide-exception rows for members of
its own destination TU, check for markers before reading the reasons — the reasons will be
about codegen and the cause will be a boundary.

---

## Reviewer checklist

1. Every coined mangled name that encodes an argument type has its disclosure in the
   manifest `notes`, in `symbols/actor_renames.tsv`, and on the declaration.
2. Exactly one `extern int _ZTV<C>[];`, in the TU source at namespace scope, no
   `extern "C"` block, and the store spelled `*(int *)p = (int)&_ZTV<C>[2];`.
3. The `_ZTV` address in `symbols.txt` is unchanged; the `complete` span covers exactly
   the promoted functions; and any dropped neighbouring data or bss entry was shown to be
   unkeepable by the restore test.
4. Every renamed `func_<module>_<addr>` declaration is gone from `include/decl_common.h`.
5. No comment in the diff names a file the diff deletes.
6. The PR is alone in the ledger queue, and the baseline was regenerated rather than
   merged.
7. Every hand-written member of the promoted TU carries a unique `// @symbol` line, the
   shared mangled externs sit above the first marker, the destructor is inline in a
   directly included header and unmarked, and no backslide-exception row is covering a
   member a marker would have saved.

## Known open items at the time of writing

- The three stale declarations quoted in section 3 are still present. This note does not
  remove them; it is documentation-only. Removing them belongs in a PR that carries a
  ROM-link proof.
- `include/daEyBm_c.h`'s comment still names the three deleted files quoted in section 4.
