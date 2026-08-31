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
3. **Do not delete a neighbouring `data_<module>_<addr>` entry** because a gate complains
   about overlap.

A green gate does not distinguish the right approach from the wrong one here. A reviewer
must check three facts directly: (a) the `symbols.txt` `_ZTV` address is unchanged; (b)
the promoted `complete` span in `delinks.txt` covers exactly the promoted functions;
(c) no neighbouring data or bss entry was dropped.

**One nuance, because the census contradicts the blunt form of rule 1.** *Renaming* a
`_ZTV` symbol at an unchanged address is legitimate and has landed: #2045 changed
`_ZTV13InvisiblePole` to `_ZTV7daBar_c` at `0x02108480`. The anti-pattern is moving the
address, not renaming the symbol.

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

## Reviewer checklist

1. Every coined mangled name that encodes an argument type has its disclosure in the
   manifest `notes`, in `symbols/actor_renames.tsv`, and on the declaration.
2. Exactly one `extern int _ZTV<C>[];`, in the TU source at namespace scope, no
   `extern "C"` block, and the store spelled `*(int *)p = (int)&_ZTV<C>[2];`.
3. The `_ZTV` address in `symbols.txt` is unchanged; the `complete` span covers exactly
   the promoted functions; no neighbouring data or bss entry was dropped.
4. Every renamed `func_<module>_<addr>` declaration is gone from `include/decl_common.h`.
5. No comment in the diff names a file the diff deletes.
6. The PR is alone in the ledger queue, and the baseline was regenerated rather than
   merged.

## Known open items at the time of writing

- The three stale declarations quoted in section 3 are still present. This note does not
  remove them; it is documentation-only. Removing them belongs in a PR that carries a
  ROM-link proof.
- `include/daEyBm_c.h`'s comment still names the three deleted files quoted in section 4.
