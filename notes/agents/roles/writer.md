# Role: writer — gather a class's shards into one real TU

`src/` holds about 9,400 auto-named one-function files (`func_ov009_020cb030.c`,
`_ZN8MetalNet6RenderEv.cpp`). The classes are already declared — 387 of 429 have
a real header. Your job is not to author a class; it is to **gather** its
scattered bodies into the translation unit Nintendo actually shipped.

**Reference commit: `72c6dcfb6`** — "ov009: promote daObjMc_Metalnet_c into a
single C++ TU". Read it (`git show --stat 72c6dcfb6`) before you start.

Do **not** use a `Reconstruct N actor profiles (wave NN)` commit as your
template. Those rename registry rows and never touch the shard pile.

## Inputs

- `notes/data/tu-promotion-queue.tsv` — your target, its `shard_count`,
  `promotion_route`, `blockers`, and a `sibling_oracle` column naming an
  already-promoted class in the same overlay. **Open the oracle's manifest and
  source first.** Copy its shape; do not invent one.
- `notes/data/class-facts/<class>.json` from the scout. Its `unproven` list is
  binding.

Start every job with:

    python tools/tubuild.py inspect <ov>/<Class>

Its "disqualifying complications" block tells you up front what you are dealing
with, but **the conflict detector does not compare parameter types**: it will
print `no cross-file conflicts detected` over a file carrying two contradictory
declarations of the same helper (`extern "C" int f(char*)` from one shard and
`int f(Class*)` from another), and even a nested `extern "C" { extern "C" ... }`.
`create` can emit a TU that does not compile. **At any real size, discard the
generated preamble and hand-curate one declaration per symbol — do not try to
repair it.** The measured failure mode is not a typo you can patch: comment
hoisting **split a multi-line declaration in half**, leaving a bare
`int flags, int speed, unsigned int startFrame);` at file scope with no return
type and no name, alongside two contradictory `PMF` typedefs and one data symbol
declared three mutually exclusive ways in the same file. Two more
collision classes it misses, both measured:

- **The `/* TUBUILD CONFLICT ... */` blocks are not comment-safe.** An alternate
  body containing its own `/* ... */` field comments terminates the outer block
  early, and you get raw `declaration syntax error`s that name nothing.
- **`this` used as a parameter name**, carried verbatim out of a `.c` shard into
  a C++ TU, gives `')' expected`.

**Past roughly 30 members the generated preamble is unusable, and
uniquification stops being advice.** Measured at 41 members: duplicate
`class C`/`struct C`, a `typedef E` colliding with a `#define E`, and
`typedef s16/u16/u8` colliding with `types.h`. Two traps in the fix itself:

- a naive `C` tag rename corrupts `extern "C"` into `extern "C_5fec"` —
  placeholder-protect the string first;
- member-local `extern` declarations that mention a renamed tag must **not** be
  hoisted into the shared block;
- `re.sub(r'^(\s+)extern "C" ', ..., re.M)` matches **across a preceding blank
  line**, because `\s` includes `
`. It rewrites file-scope `extern "C" {`
  into `extern {` and reports `declaration syntax error` eighty lines away. Use
  `[ 	]+`;
- any scan for a member's definition line must **skip comments**. A doc comment
  naming `Class::Method` is taken as the definition, which silently leaves that
  shard's declaration block at file scope — three `illegal function
  overloading` errors, none of them near the real cause.

**And `create`'s merging of a shared `struct` is a codegen hazard, not a naming
one.** On `dScMgSound_c`, eleven dispatch members saw `struct C` **incomplete**
and two saw it complete; merging them onto one complete `C` changes mwccarm's
pointer-to-member representation for the eleven. This file's advice elsewhere
— "a real header should usually win", "kept the first" — points the wrong way
here. The fix was to **split the type** (`C` incomplete, `CFull` complete); no
choice of a single body would have reproduced 82/82. **Trust it over the queue's `blockers` column**, which is
inferred rather than measured: `promotion_route` and any `compiler-only:~N` were
copied from the row's `sibling_oracle` manifest, and 91 of 226 rows have no
oracle at all and simply default to text-only. The `~` means estimate. **`prepush_attribution` reports CREDIT LOST from pure base drift.** If
`origin/main` advanced past your base, it flags files that exist there and not
on yours — one run blamed `dScMgHanachan_c`, which the branch never touched.
Compare against the branch's own **merge-base** before treating CREDIT LOST as a
blocker.

**Measured across four classes, the blockers column has been wrong every
time**, and in both directions: `dScMgRoulette_c`'s `pragma:1` was a phantom (the
shards' pragmas were inert, provable by deleting them outright);
`dScMgSound_c`'s `no-legacy-source:1` was simply false (`inspect` reports "no
legacy source: none"), its `unmatched:3` was really 1, its `pragma:1` was inert,
and its shard count was 80 against a real 82; `dScMgMemory2_c`'s count was 51
against a real 52. **Treat every blocker as a hypothesis to disprove, and expect
the shard count to be a floor** — `tu_map` cuts on symbol *name*, so a factory
not spelled `_ZN<len><Class>...` is never labelled and never counted, even when
it sits adjacent with no gap. Confirm
the real count at build time -- `tubuild verify` will tell you exactly which
symbols are unlicensed, and inheritance depth changes the number per class
(`daBar_c` carries three levels of inherited base RTTI: `fBase_c`, `dBase_c`,
`dActor_c`).

If your class has no `sibling_oracle`, you are the measurement pass. Record what
`tubuild verify` actually reported in your PR so the next agent inherits a real
number instead of a default.

## The loop

`verify` does not work on a class with no manifest — it exits `no manifest entry
for '<ov>/<Class>'; run 'create' first`. The real cycle is:

    python tools/tubuild.py create    <ov>/<Class>     # writes into src_tu/
    #   ... reconcile the merged source ...
    python tools/tubuild.py verify    <ov>/<Class>
    python tools/tubuild.py linkcheck --baseline       # ONCE per fresh worktree
    python tools/tubuild.py linkcheck <ov>/<Class>     # BEFORE promotion; see below

**`linkcheck --baseline` is mandatory in a fresh worktree, and its position in
the sequence is load-bearing.** Skip it and `linkcheck` exits 1 at `[4/8]` naming
two *unrelated* intact-object TUs — `ov036/daObjRcCarpet_c` and
`ov070/daPropeller_Heyho_c` — with `vtable partition baseline proof
unavailable`. Nothing about your class, and it costs a full run to work out.

It builds the **working tree with no substitution**, so it must run while the
tree is still pristine — *before* you move the destructor inline. Do it after
the header edit and the still-enrolled `D1Ev.cpp`/`D0Ev.cpp` shards redefine the
now-inline destructor and the control run breaks. Working order:

    create -> linkcheck --baseline (pristine tree) -> header edit + reconcile
           -> verify -> linkcheck <ov>/<Class>

**For a text-only TU, `linkcheck` cannot be re-run once the promotion is
enrolled.** With the `src/` path in `delinks.txt`, `tubuild` routes the candidate
to the intact-object path and refuses: *"intact production requires one .text
claim and at least one non-text claim"*. That refusal is benign, not damage you
caused — confirm it with a control on an already-promoted sibling
(`dScMgSingle3DBase_c` produces the identical message on untouched `main`). The
consequence is that your recorded link evidence is from a **pre-promotion tree**:
if you then edit the source, you owe a proof that the edit is byte-neutral.
Compile the object before and after and compare sha256 — one writer removing
provably-inert pragmas did exactly this (identical object, 29744 bytes) and
recorded it in the manifest. Do not let "I only removed a no-op" stand unproved.

`create` emits in reverse source order and raises RAW review flags. Both are
accurate and both matter — read them.

**Reverse-source-order is not the only admissible layout.** mwccarm lays `.text`
down in reverse source order *by default*, which is why the merged file is
normally written descending-ROM. With `#pragma defer_codegen off` the TU emits
**ROM-ascending** and can be written ROM-ascending — measured on
`ov006/dScMgMemory2_c`, 52/52 with the pragma, and "51 ordinal pair(s) NOT in
ROM order" the moment it was removed. Ascending is the more readable form. Either
is fine; what is not fine is writing one and assuming the other.

**`verify`'s emission-order success line is boilerplate, not a measurement.** It
prints "mwccarm reverses source order; this TU was written in reverse to
compensate" over a file written strictly ascending. Never infer your own source
order from it.

**That list is not a progression for every check.** `verify` passing does not
mean you are closer to `linkcheck` passing. On emission order specifically the
two report the *same* condition at different volumes: a `verify` that says
`PARTIAL ... not necessarily a bug` is a `linkcheck [4b/8]` FAIL that has already
been decided. Settle the ordering question at `verify` rather than spending a
`linkcheck` round trip to be told again.

**`tubuild.py promote` is `--dry-run` only.** It prints the plan and refuses to
act: *"only --dry-run is implemented … deliberately not available yet."* You
execute that plan by hand — and note that **step 1 of the printed plan is wrong
as written**: it says `git mv src_tu/... -> src/...`, which fails because the
`src_tu/` file is untracked at that point. Use a plain `mv` and `git add`. The full list, including four
steps the dry-run does not print:

1. Move the source out of `src_tu/` and `git rm` the shards.
2. Splice their N `delinks.txt` entries into one.
3. **Edit the manifest yourself:** `status` → `promoted`, `source` → the new
   path. **Write it at `indent=2`** — `tu_manifest.save` uses that, and a
   hand-written entry at `indent=1` fails
   `test_tu_manifest.py::test_rewriting_the_tracked_manifest_is_a_no_op` with
   only "would be rewritten", which never names indentation as the cause.
   **And write bytes, not text.** On Windows `pathlib.write_text` translates
   `
` to CRLF, so a python manifest edit silently converts a LF manifest to
   CRLF — every sibling in `config/tu_manifest.d/` is LF. Normalise afterwards
   if you used it. (Related environment trap: a `python - <<'PY'` heredoc aborts
   on an apostrophe in prose and eats doubled backslashes; write the script to a
   file and run it.) `verify` does not do this, and `promote --dry-run` refuses to run at
   all while `status` is still `text-verified`. **This is not cosmetic and the
   damage is permanent.** `tiers_ratchet.promoted_moves()` skips every manifest
   entry whose status is not exactly `promoted`, so leaving it means your
   absorbed shards resolve as `GONE -- not a tracked source file any more`
   instead of `MOVED -- absorbed into ...`, and `--update` writes a **fake
   backslide row** into `converted-backslide-exceptions.jsonl` for each one,
   blaming a readability loss on a file that was absorbed cleanly. Measured on
   `dBgActor_c`: `text-verified` gave 939 moves with zero for the class and four
   bogus `GONE` rows; flipping the one field gave 10 moves and four correct
   `MOVED` rows. It is invisible once written. (`tubuild.py:4543` reads status
   too, but only for intact-object TUs — that reader is not the one that bites.)
4. **`python tools/tiers_ratchet.py --update --reason "<why>"` — never hand-edit
   `converted-baseline.json`.** The tool also writes
   `config/converted-backslide-exceptions.jsonl`, re-sorts the array, and adds
   `src/actors/<Class>.cpp#<symbol>` member rows for members still passing all
   five criteria. A hand edit gets the count right and the **set** wrong.

   **But check set membership BEFORE you run it at all.** This step used to read
   as "always `--update`", which contradicts `builder.md`'s "if `--check` passes,
   do NOT `--update`". Both were partly wrong. The real test is whether **your
   own files are in the CONVERTED baseline set**: open
   `config/converted-baseline.json`, look for rows in your class's address range,
   and if there are none the promotion is **ratchet-neutral and there is nothing
   to bank**. `dScMgBomroom_c` had 0 of 41 shards in the set; running `--update`
   anyway banked an unrelated `+1` for `src/_ZN12dScStarSel_c8BehaviorEv.cpp`,
   pre-existing drift on `main` that has nothing to do with the class. Two
   separate writers hit that same stray row on the same day. `--update`
   re-derives the whole set, so it sweeps up any drift sitting on your base, and
   `append_exceptions` opens the file `"a"` — the append is permanent. Bank only
   when your files are actually in the set, and say which rows you banked.
4b. **A near-miss recovery and the promotion cannot be one commit.** `linkcheck`
   hard-refuses at `[1/8]` — *"substituting the TU would change enrollment and
   byte provenance in the same step"* — for any shard whose `delinks.txt` entry
   is not `complete` today. So if you fix shards to lengthen a contiguous run,
   the order is: **land the recoveries plus their `complete` markers as their own
   commit, re-run `linkcheck --baseline` against that new pre-substitution
   state, and only then `linkcheck` the promotion.** Remember that the marker
   edit itself invalidates any earlier baseline, since the fingerprint is
   `trackedConfigArm9Sha256`.

5. **Rewrite each manifest CONFLICT note — and only those.**
   `check_tubuild_conflicts.py`'s `NOTE_RE` matches only
   `CONFLICT: (macro|extern declaration|local declaration)`. `create` also emits
   `RAW:` warnings, which the gate never reads; rewriting them is wasted work and
   editing them can only break things. Measured: 40 CONFLICT notes marked, 3
   `RAW:` notes left alone, gate green. Rewrite from `tubuild create warning:
   CONFLICT:` to `tubuild create warning (RESOLVED): CONFLICT:`, or
   `check_tubuild_conflicts.py` fails once per note. **Keep the trailing
   `; kept the first` verbatim** and append your resolution after it — the
   checker's `NOTE_RE` (`check_tubuild_conflicts.py:95-99`) requires that exact
   phrase, so deleting it because it is no longer true turns a `STALE NOTE` into
   an `UNPARSED NOTE`.
6. **Grep the prose for every shard filename you deleted** and remove the
   literal token — `check_dead_references.py` reads bare `src/` tokens, not just
   links, so rewriting the sentence around the name is not enough. Rewording
   `notes/cpp-conversion-enemies.md` was needed for `daOts_c`.
7. Fix the `port/` slice manifests that referenced a deleted shard.
8. **`config/arm9/overlays/*/delinks.txt` is CRLF.** A naive line rewrite
   normalizes the whole file and buries your one-line change in a thousand-line
   diff. Preserve the line endings.

**Run `linkcheck` before you promote.** Once the source is enrolled in
`delinks.txt`, `linkcheck` routes down the intact path unconditionally and fails
at `[4/8]` regardless of `production_mode`. That is pre-existing behaviour, not
something you caused — it fails identically for `ov002/daBar_c` on untouched
`main`. The pre-promotion run is the usable evidence; record it.

## Matching is not promotion

`N/N MATCH, objisolate clean, reloc-destinations clean` is **not** the bar. The
same output will say:

    11 unlicensed section/symbol(s) present -> PROMOTION REFUSED regardless of the above

Every key-function TU lands here, and resolving it — the `compiler_only_output`
block in the manifest — is the single largest piece of work in this job.

Dispositions (from `tubuild.py` ~2146-2260):

| disposition | for |
|---|---|
| `deadstrip` | a symbol with no ROM home |
| `deadstrip-duplicate` | a **function** that does have a ROM home — **requires `canonical_module` and `canonical_address`** |
| `deadstrip-data` | a data symbol (`_ZTV`/`_ZTI`/`_ZTS`) that **does** have a ROM home in your module — plain `deadstrip` is rejected for these |

The real rule is **homed vs. homeless**, not function vs. data: `deadstrip` for
no home, `deadstrip-*` for a home. And there is a **third bucket** this file
long omitted:

| block | for |
|---|---|
| `externalized_output` | a symbol whose one kept copy lives in **another module** — disposition `canonical-import` |

**Prefer `deadstrip-data`. `externalized_output` is the non-promotable route.**
A cross-module home does *not* by itself send a symbol to `externalized_output`:
what matters is whether the address has a **configured ROM home** in some
module's `symbols.txt`. If it does — even in another overlay — it is
`deadstrip-data` and it promotes.

**But "the address has a home" is the wrong test, and this file stated it as the
right one.** `apply_compiler_only_policy` (`tools/tubuild.py:2280`) resolves a
row's home through `all_symbol_homes()`, which is **keyed on the `symbols.txt`
spelling, not the address**. Measured on ov062/`Koopa`: `_ZTI5Koopa`'s record
sits at a fully configured ov062 address — `0x0211da68` — and was refused
anyway, because the cartridge configures it as `_ZTI8daNknk_c`. `tubuild.py`'s
own comment at `:2264` says it outright: *"`homes` is keyed on the symbols.txt
spelling while `_ZTI`/`_ZTS` are LENGTH-PREFIXED mangled strings — a coined name
misses on both the prefix and the body, and the miss reads as 'the ROM has no
such record'."*

So read the rule as **spelled-the-cartridge's-way vs. not**, and expect it to
bite exactly one class of symbol: a coined class's own `_ZTI`/`_ZTS`, in a TU
that owns the key function. `_ZTV` frequently escapes because somebody added a
coined alias row for it — honest for a vtable, and **not available for `_ZTS`**,
whose *content* is the length-prefixed name (`"5Koopa"` where the cartridge
holds `"8daNknk_c"`). Three separate classes hit this in one day (`Koopa`,
`Scuttlebug`, `KingBobOmb`); `class_rename.py` counts **125** in this state. The
unblock is the rename, as its own change. The promoted precedent `ov029/daObjWcObj01_c`
carries an **empty** `externalized_output` and licenses all 13 of its RTTI
records as `deadstrip-data`.

`externalized_output` / `canonical-import` is what a TU that stops at
text-verified uses. (The counts in the next sentence are historical and do not
match the tree: `daObjFallBlock_c`'s manifest today carries **14** rows — 11
`deadstrip-data` plus 3 `deadstrip-duplicate` — with `externalized_output`
empty. Read it for the mechanism, not the arithmetic.)
`daObjFallBlock_c` first wrote 10 rows there and could not
promote; every one had a configured home, so all 10 converted to `deadstrip-data`
and the class promoted. If you find yourself filling this block, check first
whether those addresses are actually homed — most are.

The formula below predicts the **total**; it does not predict the split.

**`verify` does not deduct `externalized_output` rows from its unlicensed
count.** A complete, correct policy still reports a high number. It reads like
failure and is not.

**A TUBUILD CONFLICT note is two edits, not one.** Marking the note `(RESOLVED)`
while the `/* TUBUILD CONFLICT ... */` marker survives in the merged source is
*worse* than leaving the note alone — `check_tubuild_conflicts` reports
`REOPENED: ... is noted RESOLVED, but <file> carries its marker again once`.
Mark the note `(RESOLVED)`, append your resolution after `; kept the first`
verbatim, **and delete the marker comment from the source**.

**`git mv src_tu/... src/...` fails only while the `src_tu/` file is untracked.**
On a second pass over an already-committed shadow TU — which is exactly what a
partial promotion is — `git mv` is correct and preserves the rename.

**Do not read `tubuild.py`'s `promotion_refusals()` as a live gate.** It refuses
any entry carrying `externalized_output`, but its only caller is `cmd_promote`,
which is `--dry-run`-only and prints "the mutating path ... is deliberately not
available yet". Every promotion here is done by hand, so that refusal governs a
path nobody runs. A writer took it at face value and nearly abandoned a class
that was promotable — if it were live, no class with cross-module RTTI could
ever promote, which is plainly false.

**The row count is predictable from inheritance depth**, so do not copy an
oracle's count unless the oracle sits at the same depth. **Count the chain
yourself before trusting the queue's `compiler-only:~N`** — that column is copied
from the `sibling_oracle` with no depth check, and has now been wrong in both
directions. (Note what an out-of-line destructor actually costs. "Out-of-line emits D2, D0,
D1" reads as though the order is unusable; the tree says otherwise.
`dScMgMemory2_c` declares its destructor out of line, is link-verified with D1
below D0, and pays for it with a **homeless** `_ZN14dScMgMemory2_cD2Ev` — that
is its 14th row, and it is where the formula's otherwise-unexplained "+1" comes
from. The lever set is right; the consequence is a homeless D2 plus a licensing
row, not necessarily a wrong order.)

`dScMgMemory2_c` is six levels
(`dScMgSingle3DBase_c` -> `dScMgBase_c` -> `dScene_c` -> `dBase_c` -> `fBase_c`)
and needs `2x6 + 1 = 13`, +1 homeless `D2Ev` = **14**; the queue said ~11 because
its oracle sits one level shallower.

    rows = 2 x (ancestors + self)  +  1 vtable  +  1 per Vector3-like
                                                     member with an inline D1

**A coined class name is harmless UNLESS your TU owns the key function — and
then it is a hard blocker.** Three runs in one day reached opposite conclusions
about renaming, and this is the reconciliation:

- **Your range does not own the key function** → the TU emits no vtable and no
  RTTI at all, so the tree's coined spelling never reaches a manifest. Keep it.
  Renaming is a `class_rename` campaign, not a promotion. (`Goomboss` kept
  `Goomboss` over the cartridge's `daKuriKing_c`; `Eyerok` kept `Eyerok` over
  `daIwante_c`.)
- **Your range does own the key function** → the TU emits `_ZTI`/`_ZTS`/`_ZTV`
  for its own class, and those rows **must** carry the ROM's RTTI spelling.
  `verify` refuses a coined RTTI row — it is "banked as a plain deadstrip, never
  compared against the cartridge" — and the precedent is unanimous: **994 RTTI
  and vtable rows across every promoted manifest, zero of them under a coined
  name** (measured 2026-09-05 against `build/rtti.json`). There is no such thing
  as a promoted coined class.

**And the inline-destructor lever can CREATE this blocker out of nothing.**
Inlining moves the key function to the first out-of-line virtual declared. On
ov071/`Scuttlebug` that pulled the key function *into* the claimed range — which
is what the writer wanted for emission order, and it is also what turned a
harmless coined name into a hard refusal. The TU then emitted
`_ZTI10Scuttlebug` / `_ZTS10Scuttlebug`, which no `symbols.txt` anywhere names,
and both dispositions refuse:

    deadstrip-data -> "declared compiler-only data but has no configured ROM
                       home; a homeless object is a plain deadstrip"
    deadstrip      -> "an RTTI/vtable record banked as a plain deadstrip, which
                       is never compared against the cartridge. If the class
                       carries a coined name, rename it to the cartridge's RTTI
                       spelling..."

Result: `verify` 37/37 MATCH, TEXT-VERIFIED, **PROMOTION REFUSED** on two
symbols. So before you reach for the inline destructor, check whether the class
name is coined — the two levers interact, and `class_rename.py` reports **125
classes** sitting in this state. Note the vtable often escapes because
`symbols.txt` already aliases `_ZTV` under both spellings; nobody did the same
for `_ZTI`/`_ZTS`.

**Do not "fix" this by adding an alias, and understand why the temptation is
dangerous: faking it passes every gate you can run locally.** `deadstrip-data`'s
home check is a **name lookup**, and the byte comparison is deferred to
`rombuild`. Giving `_ZTS10Scuttlebug` a home address would produce a green
`verify` and a genuine 13-versus-12-byte mismatch downstream. The refusal is
correct; the fix is the rename, as its own reviewable change.

**Every path you cite must resolve from the repo root on a fresh clone.**
This is a real defect, not a hypothetical: four private Claude-memory slugs are
committed in this tree as though they were repo notes. Naming the slugs alone,
because spelling them the way the source does would trip the dead-reference gate
in this very file: `actor-class-names-off-by-one` in `include/daObjHmBskt_c.h`,
`key-function-tu-vptr-store-blocker` in
`src/game/actors/d_a_obj_km3_dorifu.cpp` and twice in
`src_tu/actors/TTC_MovingBar.cpp`, `phantom-references` in
`include/nitro/hw/registers.h`, and `stale-tu-map-overcut-ov006` in two
`config/tu_manifest.d/ov006/*.json` `boundary_evidence` strings — each written
there with a `notes/` prefix and a `.md` suffix. Those files live
in one machine's private memory directory; nobody else can follow the reference,
and the dead-reference gate never saw them because it walked only `.md`.

State the test as the fresh-clone property, not as "don't cite memory". An agent
that can see its own memory directory finds the file sitting right there, so
"is this a memory slug" is not a question it can answer — "does this path resolve
in a fresh clone" is. The same test catches the neighbouring mistake of citing an
untracked local scratch file. Cite a tracked repo path, or state the fact inline.

Look the class up in `build/rtti.json` before you decide anything else, and
expect the alias case: `symbols.txt` can carry **both** spellings at one address
(`_ZTV10KingBobOmb` and `_ZTV12daBombking_c` at `0x02126e4c`). If you own the key
function and hit that, say so in your report rather than renaming silently — the
rename touches `symbols.txt` tree-wide and is a separate, reviewable decision.

**The whole formula is conditional on the licensed range owning the key
function, and this file used to state it unconditionally.** A TU that does not
own the key function emits **no vtable and no RTTI at all** — zero rows from
this formula, not `2x(ancestors+self)+1`. Measured on `Goomboss` (ov074): a
key-function-owning probe emitted exactly the predicted 11; the range actually
licensed excludes the key function and needs **1** row, the `_ZN7Vector3D1Ev`
duplicate. Both numbers are right. Decide which TU you are building first.

**The last term is conditional, and "member" is the wrong test.** The trigger is
any `Vector3` the TU *odr-uses* — **function locals count**. `dScMgD3DBase_c` is
5 levels deep and needs **11** because it touches none. `daDsnBase_c` is also 5
levels and needs **13**: it has no `Vector3` member either, but
`func_ov091_02132f04` holds two as locals.

That 13th row is a second thing the formula never predicted: a file-local
anonymous struct holding those locals emits its own destructor,
`_ZN<N>@class$<n><file>_cppD1Ev`. It is genuinely homeless — plain `deadstrip`,
the one disposition you will rarely otherwise use. So: **count the `Vector3`s the
TU touches, members and locals alike, and add one more for any local anonymous
struct containing them.**

**Correction, measured: the Vector3 term is `+1` if the TU odr-uses `Vector3`
AT ALL — not one per object.** The emitted symbol is the single vague-linkage
`_ZN7Vector3D1Ev`, so the count is one however many objects there are.
`daObjMarioCap_c` odr-uses **four** (three locals in one function, one in
another), has **no** `Vector3` member, and got exactly **one** extra row: 12 =
2x5 + 1 + 1. Read literally, "1 per Vector3" predicts 4 or 5. The worked examples
above are consistent with the corrected rule; only the prose was wrong. The
local-anonymous-struct term is separate and genuinely additive — that one emits
its own distinct `_ZN<N>@class$<n><file>_cppD1Ev`.

`daPgDfdr_c` (`dBgActor_c` -> `dActor_c` -> `dBase_c` -> `fBase_c`, 5 levels)
needs 2x5+2 = **12**. Its oracle `daIDonketu_c` sits one level deeper and needs
14 — which is exactly how the queue's estimate came out wrong.

**Do not assume a symbol's `canonical_module` is your TU's module.** `_ZTI`/`_ZTS`
have vague linkage, so the linker keeps **one** copy wherever it first landed,
which can be a different overlay entirely. Measured on `daOts_c` (ov064): its own
`_ZTI7daOts_c`/`_ZTS7daOts_c` live in **ov027**, and ov064's vtable header word
relocates across to them. `dsd` reports that relocation's module as a list of
sixteen overlays; the real one is whichever overlay's `symbols.txt` actually
names the address. Two rows will be wrong if you guess.

**`decl_common.h` is not always right, and correcting it is precedented.** It is
generated by `decl_headers.py`, which reads a legacy shard's K&R forward
declaration in preference to its definition — so a member can be recorded with
the wrong signature entirely. Measured: `func_ov006_020d672c` was recorded
`int f()` from an `extern int f();` line, while the cartridge's bytes take the
scene pointer in r0 and leave r0 untouched on the early-out path, making it
`void(void*)`. Most overload collisions are adaptable byte-neutrally (take
`void*`, cast on line 1 — six of seven were), but when the header is simply
wrong, correct it with a comment and check every other caller: a caller that
declares the function locally and does not include the header will not be
affected, and you must confirm that rather than assume it.

`tubuild create` writes `// @symbol` markers only for mangled or already-named
members; **auto-named `func_ovNN_ADDR` shards are skipped silently.** Add theirs
by hand or tiers scoring misses them. The gap is narrower than it sounds — a
shard carried in RAW, inside its own `extern "C"` block, *does* get a marker.
Only the plain `.c`-derived ones lack them. Check which you have rather than
blanket-adding.

**Measured, that last sentence is too optimistic:** on `dScMgSound_c`, `create`
emitted **10 markers and skipped 72 of 82**, the three RAW members among them.
So the RAW-members-are-covered rule does not hold. Count what you actually got;
blanket-adding was the right call there.

**A missing `complete` marker refuses the whole run at `[1/8]`, before any of
the documented failure modes.** If any shard in your TU has a `delinks.txt` entry
without `complete`, `linkcheck` stops with *"substituting the TU would change
enrollment and byte provenance in the same step"*. This fires first on any class
carrying an `unmatched:` blocker, so expect it there. A shard usually lacks the
marker for a reason — one measured case did not compile at all, because it
referenced a **phantom** symbol that `decl_common.h` declares and no module
defines. Find out why before setting the marker; setting it on a genuinely
broken shard converts a clean refusal into a link failure.

When the policy is refused, `verify` re-reports **all** extras as unlicensed from
the unaudited object. One bad row makes it look like nothing is licensed. Fix the
row, do not chase the count.

**Most classes do not need `production_mode: intact-object`.** If the class's
`_ZTV`/`_ZTI`/`_ZTS` live outside the TU's `.text` range, the ordinary text-only
route plus a `compiler_only_output` block is correct. In all of ov002,
intact-object is used exactly once. Attempting it wrongly gives `intact
production requires one .text claim and at least one non-text claim`, which
reads like your source is broken and is not.

Forcing scaffolds (`p->~X(); delete p;`) that the shards carried are usually
**not** needed once merged — owning the key function emits the vtable and drags
both destructor variants in.

## Rules that decide whether the bytes come back

mwccarm 2004/b56 behaviours, not style preferences.

- **The destructor form is a MEASUREMENT, not a default.** Two forms reproduce
  the ROM's usual `D1`-then-`D0`-and-no-`D2`, and which one a TU needs is a
  question to measure per TU:

  - `virtual ~X() {}` **inline, declared FIRST member** — the variants emit in
    reverse order of their first odr-use;
  - the destructor **out of line** under `#pragma defer_codegen off` — emits
    `D1, D0, D2`.

  Out-of-line *without* that pragma emits `D2/D0/D1` in the wrong order plus a
  homeless `D2`, which is where the flat "always inline it" rule this file used
  to state came from. Landed both ways: ov006/`dScMgTeresa_c` and
  ov006/`dScMgPanel_c` (71/71 bytes; its stage-2 "link-verified" was **not
  reproducible** and stage 4 had to repair the linkage — see below) are both
  out-of-line +
  `defer_codegen off`. **Try both before you take a partial** — and note the
  inline form has a cost of its own, since it moves the key function to the
  first out-of-line virtual declared, which on a coined-name class turns a
  harmless name into a hard promotion refusal.
- **Emission order is a hard gate, and it is the constraint most likely to stop
  you.** `linkcheck [4b/8]` fatally refuses a TU whose licensed `.text` sections
  are not in ROM-ascending order: `licensed .text functions are not emitted in
  ROM address order`. Beware that `tubuild verify` reports the same condition as
  `PARTIAL ... not necessarily a bug`, which reads advisory and is not.

  **And `verify`'s exit code carries no verdict about promotability.** It is
  `return 0 if text_verified else 1` — so a run that prints `PROMOTION REFUSED`,
  for unlicensed output or a compiler-only policy refusal, still **exits 0**.
  Only a compile failure or a byte/reloc failure gives 1. Read the console text;
  never gate on `$?` alone here. (Measured on ov071/`Scuttlebug`: two refused
  runs, both exit 0.)

  **Two more things that run does not say out loud.** The manifest's
  `verification.criteria` records the **pre-policy** unlicensed count while the
  console prints the post-policy one — `FAIL-BY-DESIGN -- 10 unlicensed
  section/symbol(s)` in the file against `2 unlicensed section/symbol(s)` on
  screen, after 8 were licensed. It is deterministic wording, not drift (a
  re-run produced a byte-identical manifest), but a reader trusting the manifest
  sees five times the real number. And it is **`linkcheck`**, not `verify`, that
  was observed rewriting the manifest here — it adds a `verification.linkcheck`
  block, and it can race your `git add`.

  The destructor pair is the usual way to hit it. Under **default** codegen the
  lever set is closed — measured, not guessed: **inline in class ⇒ D1 then D0,
  always**; out-of-line ⇒ D2, D0, D1. A `delete p` scaffold, a `p->~X()`
  scaffold, and moving the declaration below the overrides all change
  **nothing**. See the comment block in
  `src/game/actors/d_a_obj_wc_obj01.cpp` and `notes/tu-reconstruction-pilot-report.md`
  section 3.

  **But that closure holds only under deferred codegen, and `#pragma
  defer_codegen off` is a fourth lever the old measurement never tried.**
  Measured 2026-09-05 on `dScMgTeresa_c` (pinned 2004/b56, real header, both
  ways): out-of-line + default codegen emits **D2, D0, D1**; out-of-line +
  `#pragma defer_codegen off` emits **D1, D0, D2** — the cartridge's order.
  Sizes 0x24 / 0x38 / 0x24, byte-MATCH either way. So a cartridge that puts D0
  *below* D1 is **not** automatically a refusal any more: try the pragma before
  taking the partial promotion. It flips emission order for the whole file, so
  adopting it and laying the source out ROM-ascending are one edit, not two.

  **A fifth lever, from the same day:** with the destructor **inline**, the two
  variants come out in the *reverse order of their first odr-use*. On `Goomboss`
  a `delete p; p->~Goomboss();` scaffold gave D1-then-D0 and swapping the two
  statements gave D0-then-D1, read out of the compiled object's section order.
  That is a real handle on the order — but the destructor group travels with the
  scaffold that forces it, emitted immediately below that function wherever it
  lands, so it is unusable when the pair belongs at the very start of a module's
  `.text` (on `Goomboss`, D1 *is* the first byte of ov074's `.text`; there is no
  address below it).

  Do not read that as "inline is the fallback". On `dScMgTeresa_c` inlining
  moved the key function to a member *above* the run's sourceless hole, so the
  destructor pair could not share a licensed TU with it; the probe TU emitted
  one `.text` section and **neither** D1 nor D0. Check where inlining puts the
  key function before assuming it is available.
- **The TU must odr-use the class** or nothing is emitted at all.
- **vptr store is `(int)&_ZTV...[2]`**, never the raw symbol — the addend loses
  8 on rebind, and only `objisolate`'s addend check sees a miss.
- **Declare the constructor (`X();`), never define it inline** — an implicit
  ctor is inlined and emits no `bl` to `_ZN...C2Ev`.
- **Reconciling a raw-offset access to a named field can change codegen.**
  Measured on `dBgActor_c::UpdateKillByMegaChar`: rewriting
  `*(short*)((char*)this + 0x8c)` to `mAngleX` let mwcc CSE the field address and
  dropped four instructions, `0x158` → `0x148`. Keeping the raw spelling **for
  those accesses only** restored the match. Reconcile freely, but when a function
  goes short, suspect the field-address CSE first and put the raw spelling back
  on the smallest set of accesses that fixes it.
- **A merge changes the include set, and that can change a type.** The merged TU
  sees the *union* of N shards' includes, so a type with two guarded definitions
  resolves to whichever header now arrives first. Measured on `daPgDfdr_c`:
  `common.h` and `math/Matrix.h` both define `Matrix4x3` at 0x30, the shard saw
  `common.h`'s flat `s32 m[12]`, and the merged file reached `math/Matrix.h`
  first (via `daPgDfdr_c.h` -> `Model.h`) and got `Matrix3x3 r; Vector3 t`.
  mwcc then split one whole-matrix assignment into a 9-word `ldm`/`stm` plus a
  3-word tail it CSE'd — `0x94` became `0xac`, in a function nobody had touched.
  Reordering the includes restored the match. `Vector3_16` behaves the same way.
  **When a function goes LONG after a merge and you did not touch it, check the
  include union before you look at CSE.** (Short, and you touched it: CSE.)
  This heuristic has now found a regression it was written for: `daDgr_c`'s sole
  DIFF was an untouched function whose `Matrix4x3` spelling flipped because the
  merge made `math/Matrix.h` arrive first through `daDgr_c.h -> dBgW_KcMbg.h`.
  A local flat `Mat12` restored 9/9.
- **Struct copy:** C++ scalarizes word-by-word where C block-moves — about 12
  bytes short. Force it with `struct M { int w[12]; };`.
- **bool widening:** `int f = (a==b); if (f)` is longer in C++ than `if (a==b)`.
- **Qualified calls** (`obj.Base::Method()`) suppress virtual dispatch, both
  directions.
- **Compound assignment** (`x += k`) CSEs the field address where raw member
  access does not. Only the byte gate settles it.
- **`extern`, not `extern "C" { ... }`** for ROM symbols already spelled mangled
  in a `//cpp` file — the block form *defines* and collides.
- **The mirror case, and it is the common one.** A symbol whose ROM name is
  mangled but which the shards spell **unmangled** must be declared **outside**
  the `extern "C"` block: inside it, C linkage strips the mangling and `[5/8]`
  fails `Undefined : "<name>"`. Measured on `ApproachLinear`, ROM symbol
  `_Z14ApproachLinearRiii`. The rule is *mangled spelling inside, unmangled C++
  spelling outside* — and **`verify` stays green either way**, so nothing catches
  this until `linkcheck`.
- **`//cpp` must be the file's first bytes.** The extension is never consulted.

## Mechanics of a large merge

These are steps, not debugging responses. Do them before the first compile and
they cost minutes; discover them after and each is a cycle.

- **Emit forward declarations for every member.** mwcc lays `.text` down in
  *reverse* source order, so the merged file is written descending-ROM and
  nearly every intra-TU call is a forward reference. Skipping this produced
  about twenty `undefined identifier func_ovNN_*` errors on one 49-function
  attempt.
- **Discard the generated preamble above roughly 30 members.** tubuild's emitted
  one had `Self`/`Pmf` in dependency-wrong order, a `Vector3` redeclaration
  against the real header, duplicate `P2`/`Pair`/`Obj` tags, and three
  contradictory externs for a single data symbol.

  **But "hand-curate one declaration per symbol" is the wrong next step, and at
  real size it is unworkable.** 301 independent recoveries of one symbol
  disagree constantly, and forcing a single spelling rewrites call sites.
  Measured on ov002/`Player` (301 members): in mwccarm 2004/b56 a declaration
  written at **block scope inside an `extern "C"` region gets C linkage**, and
  two function bodies may declare one C symbol with **different types**. So keep
  every member's declarations **inside its own body** and merge nothing. Hoist
  only what a body cannot hold — namespaces, classes with static or
  declared-but-undefined members, and any type named in a member's own
  signature — uniquifying tags per member.

  **Read "inside an `extern "C"` region" as the load-bearing half of that
  sentence, because a C++-NAMED MEMBER CANNOT BE INSIDE ONE.** A `func_ov006_*`
  member is wrapped in its own file-scope `extern "C" { }`, so its block-scope
  `extern` names the ROM symbol. A class member function — `Class::Method` — may
  not sit in a linkage-specification region at all, so the identical block-scope
  declaration written in *its* body gets **C++ linkage** and the reference
  mangles. Measured on ov006/`dScMgPanel_c`: 67 wrapped members were fine, the
  four C++-named ones (ordinals 67-70) emitted **41 mangled undefined symbols**
  — `_Z8LoadFilei`, `_Z10DeallocatePv`, `_Z19func_ov006_021063a0Pv` — and
  mwldarm aborted with 22 `Undefined`. Nothing before the link noticed:
  `tubuild verify` was 71/71 MATCH with objisolate and reloc-destinations clean,
  and the `[4b]` object audit was `order_ok True, {'LICENSED': 71}, 0 refusals`.
  **And you cannot fix it in the body**: mwccarm 2004/b56 rejects a block-scope
  linkage specification outright — `extern "C" { extern void f(int); }` inside a
  function is `Error: declarator expected`. So a TU with C++-named members needs
  **one file-scope `extern "C"` region** for what those members call, the way
  ov006/`dScMgTeresa_c` and ov006/`dScMgRoulette_c` already do. Place it *after*
  the last wrapped member so none of them can see it and their independently
  recovered spellings stand. Declarations naming members of the TU itself do not
  belong in it at all — delete them and let the call bind to the definition
  already above it, casting at the call site if the definition's pointer type
  differs. Data is unaffected: mwccarm leaves a file-scope variable's name
  unmangled in C++, so `data_*` declarations stay in their bodies.

- **"Block-scope data redeclaration is rejected" is false**, and believing it
  costs bytes. Measured directly: `extern int dv;`, `extern char dv[];` and
  `extern struct V dv;` in three separate function bodies compile cleanly. Only
  a **file-scope** redeclaration is rejected.

  **But the licence is for FUNCTIONS with C linkage, and a project header
  revokes it for data.** Measured on ov006/`dScMgPanel_c`:
  `include/dScMgBase_c.h:12` already declares
  `extern "C" void *data_ov004_020beb68;`, ordinal 60 recovered the object as
  `char *`, and the block-scope form is rejected outright —
  `identifier 'data_ov004_020beb68' redeclared; was declared as: 'void *'`.
  The wording above sends you hunting for a file-scope duplicate *inside your
  own TU* that is not there; the conflicting declaration is in a header you
  include. The fix that kept 71/71 was a reinterpreting macro, not a
  redeclaration.

- **Canonicalising a DATA declaration is a codegen hazard, exactly like merging
  a shared `struct`.** Hoisting one canonical data declaration and casting each
  member's view back with a macro **changed the codegen of six members** on
  `Player` — and which spelling became canonical depended on which *other*
  members happened to be in the file. Moving the data declarations back to block
  scope recovered all six. The role file named the struct case as a hazard and
  said nothing about data; they are the same hazard.
- **A caller may pass more arguments than the definition takes.** Two sites did.
  Declare the unused extra parameter — it preserves the caller's argument setup
  and costs the callee nothing.
- **Every 2-int shadow struct that is ever whole-struct-assigned must be spelled
  with an array member.** C++ scalarizes what C block-moved. This one rule
  accounted for seven byte-diffs in a single merge; treat it as a checklist
  sweep, not a per-diff discovery.

## Reconciling the merged declarations

`tubuild`'s conflict detector is weaker than this file used to imply: it is not
merely blind to parameter *types*, it misses conflicts that are outright compile
errors. On `dScMgD3DBase_c` it reported 7 textual conflicts and missed 5 hard
ones — `unsigned short` against `unsigned int`, `int(void*)` against `int(int)`,
a function declared `void(void)` but called with `this`, a const-ness mismatch,
and a scalar declared against an array.

**Grep every shadow declaration against the `decl_*.h` headers under `include/`
before you compile.** This is not polish; it is the difference between compiling
and not. Every auto-named `func_ovNN_ADDR` shard is declared in `decl_common.h`,
so **every** merged shard needs the check. When the merge makes both spellings
visible mwccarm rejects it as `illegal function overloading`, and the error text
points at your *definition* line while saying nothing about the header —
measured on `func_ov091_02133098`, defined `void*` against the header's `char*`.
**The fix pattern is to change the *definition* to the header's type and add a
one-line cast alias inside the body** — not to fight the header. Three members of
one TU hit this at once (`char*` vs `void*` twice, `int*` vs `char*` once).

**The detector also misses *return-type* disagreement between two shards, and
emits both spellings.** `func_0201267c` was declared `int f(int, void*)` by one
shard and `void` by another; `create` reported `no cross-file conflicts` and
wrote one into the shared block and one into a RAW per-member block — an
unconditional redeclaration error. The shard-vs-real-header case is covered
below; this is shard-vs-shard, and it needs the same grep.

**Include `decl_common.h` and match it** — that is the actual instruction. The
reason matters: **no header under `include/` includes `decl_common.h`**, so the
`illegal function overloading` collision only fires if *your* TU pulls it in.
Both promoted precedents (`daDsnBase_c`, `daObjCtMecha03_c`) do. Do the same and
align your spellings to it, rather than assuming the header will collide with
you on its own.

**But a `decl_common.h` return type can cost you the match, and then the header
is what gives way.** Measured on ov066/`Eyerok`: `decl_common.h:2730` types
`func_ov066_02119454` as `void(void*, void*)` and the ROM function returns a
value — declaring it `int` MATCHes, declaring it `void` does not. That TU
excludes the header and declares every symbol the header would have supplied.
So: include it and align *by default*, but when a spelling it dictates breaks a
byte match, measure both ways and say which you took and why. The bytes outrank
the header.

**And "include it" inverts above some member count — this is a size-dependent
rule, not a default.** `decl_common.h` declares 10 of ov006/`dScMgPanel_c`'s 71
members and **7 of the 10 contradict the byte-matched shard**, two of them on
the return type. Including it makes each an `illegal function overloading`
error against code that already matches. That TU excluded the header (the ov002
`Player` precedent) and gave the four class-method members their `decl_common`
declarations at block scope instead — **and that half did not link**; they had
to move to a file-scope `extern "C"` region, for the reason measured above.
Excluding the header does not free you from giving a C++-named member's
declarations C linkage somewhere. The more members a TU absorbs, the likelier
the header contradicts one — so count the collisions before you decide. Two
promoted TUs include it and two of the largest exclude it, and both choices are
right for their size.

**Grep shadow struct *tags* too, not only declarations.** A shadow type can
collide with a **ROM symbol**, which is a different failure and a nastier one.
`decl_common.h` declares `extern int VT[];`, and the legacy shards carry a
shadow `struct VT` — invisible until the merge pulls `decl_common.h` in. In C++
a variable name hides a struct tag, so this surfaces as `undefined identifier
'VT'` reported against **the struct that uses it**: never the header, never the
name that actually collided. Grepping declarations alone will not find it.
The detector does not compare against real headers at all, and a real header
always wins. Two measured disagreements were on *return type*: `decl_common.h`
types `func_02012718` as returning void where the shard said int, and
`decl_Particle.h` does the same for `SysTracker::Initialise`. Where that
happens, drop the `return` and let the tail call forward r0.

**When source cannot settle it, ask the ROM.** One conflict — a helper declared
`void(void)` but called with an argument — was unresolvable from the shards.
Disassembling retail `AfterCleanupResources` showed `mov r0, r5` immediately
before the `bl`, proving the argument is real and the callee simply ignores it.
Two `disasm.py` runs replaced a guess and a full verify cycle. Remember
`--base` is applied at file offset 0, so pass the module's **first
`symbols.txt` address** (ov006 = `0x020bfec0`).
- No C++11: no `nullptr`, `auto`, `override`, STL, east-const. This is 2004.

Ask the compiler rather than hand-mangling:

    python tools/mangle.py candidate.cpp --expect _ZN...

## Tool traps measured by writers before you

- **`linkcheck --partial` skips the emission-order gate.** Plain `linkcheck`
  runs the `[4b/8]` licensing audit only when `not baseline and not partial and
  not partitioned` (`tubuild.py:5061`), and that audit is where the hard
  ROM-ascending `.text` refusal lives. So when your order is *not* reproducible
  you can still get real link-and-ROM evidence — full module fidelity and a
  stock-identical ROM sha256 — out of `--partial`. Settling order at `verify` is
  still right; this is the extra evidence available once you have.
- **`create` before `linkcheck --baseline` is safe.** `src_tu/` is untracked and
  not in the build, so the tree is still pristine for baseline purposes.
  **But it is NOT only header edits that spoil a baseline** — this file used to
  say so. The baseline's fingerprint is `trackedConfigArm9Sha256`, so **any**
  edit under `config/arm9` invalidates it, including a one-line `complete` marker
  in a `delinks.txt`. Worse, **the failure is indistinguishable from never having
  run a baseline at all**: both die at `[4/8]` with the same
  `ov036/daObjRcCarpet_c` + `ov070/daPropeller_Heyho_c` "vtable partition
  baseline proof unavailable" pair. Recovery costs a second full baseline run
  with the header **temporarily reverted** — the still-enrolled D1/D0 shards will
  not compile against an inline destructor.
- **Reading the `_ZTV` extent from the next `symbols.txt` row UNDER-reports it** —
  the opposite error to the one below, and easier to fall for because it looks
  authoritative. `_ZTV7daDkk_c` at `0x02113850` is followed by
  `data_ov025_021138a8`, implying `0x58` / 22 slots; it is really `0x88` / 32.
  Three `ambiguous` `data_ov025_*` rows are **phantom interior symbols sitting
  inside the table**. `relocs.txt` narrows it — every word through `0x021138cc`
  is a relocated entry, and `0x021138d0` already belongs to the next class's
  `_ZTI`. Corroborate with `romdata_check`'s **`emitted`/`bytes`** fields and
  `blindWords: 0`.

  **But "take the contiguous relocated run" OVERSHOOTS, and this worked example
  is what proves it.** Measured on the same ov025 table: the relocated run from
  `0x0211384c` reaches `0x021138d8` with **no gap anywhere** — 36 words, four
  more than the real 32 — because `_ZTI14daObjDpBrock_c` at `0x021138d0` is a
  `__si_class_type_info`, three fully-relocated words butted straight against the
  table. Nothing in `relocs.txt` marks that boundary.

  **The `_ZTI`/`_ZTS` tie-break first proposed here does NOT generalise — a
  later sweep refuted it.** All 51 `_ZTV` in ov006 were checked: the run-overshoot
  is real in **15 of them**, but in every one of those the overshooting words are
  ordinary `data_ov006_*` / `g_profile_*` objects, **never** a `_ZTI`. So "look
  for a `_ZTI`/`_ZTS` named inside the run" would have caught **0 of 15**. And
  the naive next-symbol read *under*-reports in the other direction
  (`dScMgD3DBase_c`: `0x50` against a real `0x90`, because of an interior phantom
  symbol). **Neither rule is sound alone.**

  **What actually works is corroboration from three independent tools**, which is
  what every correct extent in this campaign has rested on — though the third is
  **unavailable to a TU that does not own the key function**: `check_object` only
  sees the vtable if the TU emits it, so on a partial promotion get that number
  from whichever shard still owns the key function, or from a throwaway probe TU
  that does. The three are the relocation gap,
  `rtti_vtables.py --own <Class>` for the slot count — **but it answers only to
  the ROM's RTTI spelling, not the tree's**. `--own Goomboss` prints
  `no vtable for Goomboss`; `--own daKuriKing_c` prints the 31 slots. For a
  coined-name class that output reads as "no vtable exists", which is false;
  look the class up in `build/rtti.json` and ask with the cartridge's name — and
  `romdata_check.check_object()` for `bytes`/`emitted` with `blindWords: 0`.
  Worked example, `dScMgRoulette_c`: relocated run `0x0213e398..0x0213e428`, then
  a real `0x18` gap before the next reloc at `0x0213e444`; storage
  `0x0213e394..0x0213e42c` = `0x98`; symbol extent from the address point =
  `0x90` = 36 slots; `rtti_vtables` says 36; `check_object` says
  `bytes 144, emitted 144, blindWords 0, VERIFIED`. Three tools, one answer.
  **Distinguish storage size from symbol extent** — they differ by the 8-byte
  offset-to-top + typeinfo preamble, and quoting one where the other is meant is
  its own source of confusion.
- **Do NOT read the extent from `romdata_check`'s `romExtent` field.** It carries
  the *wrong short* value — `88` decimal, the false `0x58` — on the very class
  where the answer is `0x88`. Trusting it confirms the trap instead of catching
  it. `emitted` and `bytes` are the truthful fields. **This is class-specific, not universal** — on
  `dScMgRoulette_c` the field reads `144`, which is correct and equal to
  `emitted`/`bytes`. So it is unreliable rather than always-wrong: never take it
  alone, and corroborate as above.
- **`verify` DOES print a `_ZTV` size while the licensing policy is still
  missing**, and that is worth exploiting. Each unlicensed symbol gets an
  `EXTRA <section> <symbol> size=0x..` line, e.g.
  `EXTRA .data _ZTV15daObjMarioCap_c size=0x84`. That is **mwcc's own emitted
  storage size**, free, before you read a single ROM word — the cheapest of the
  three corroborations for the extent. Run `verify` once before writing the
  licensing policy just to harvest it.
- **On a green run, `verify` prints no `_ZTV` section size.** This file said twice to cross-check
  against one. It does not exist; `verify` prints the MATCH table, byte
  comparison, objisolate, emission order and the result.
- **Corroborate every slot count, but `rtti_vtables.py` is no longer the known-bad
  one.** It used to over-report (34 against a real 32 on `daObjCtMecha03_c`); it
  now trims following-table tails and has agreed with direct reads since. The
  vtable row's `reason` invites you to state a slot count, so get it from two
  sources regardless.
- **`rtti_vtables.py` needs `build/rtti.json`**, which only `rtti_extract.py`
  writes. Without it you get a bare `FileNotFoundError` traceback that names no
  remedy.
- **If no facts file exists, write one.** This role file treats
  `notes/data/class-facts/<Class>.json` as a hard input whose `unproven` list is
  binding, but a scout does not always run first. Produce it yourself from
  `rtti_extract.py` plus a direct overlay read, and commit it — the next stage
  reads it.
- **Commit the facts file *before* `promote --dry-run` step 1.** Steps 1-2 (`mv`
  + `git rm`) leave the source promotion **staged**, so any commit you make
  afterwards silently swallows it. A writer following this file's own "facts
  JSON goes on the same branch" convention had to `reset --soft` and re-split.

## Conventions — fixed, so nine agents do not pick nine ways

- **File:** `src/actors/<Class>.cpp`, the class name exactly, matching the
  class-named majority and the `layout_check` L2 stem rule. (`d_a_*.cpp` snake
  names also exist in the tree; do not add more.)

  **A `promoted_source` already sitting in the manifest is a plan, not an
  instruction.** `dScMgMemory2_c`'s manifest pre-declared
  a `d_s_mg_memory2.cpp` snake name under `src/minigames/`
  (written bare, not repo-rooted: the full dead path would fail
  `check_dead_references`), and its own sibling and sibling-oracle did
  land under `src/minigames/` with snake names — 13 such files against 12
  class-named ones in `src/actors/`, both sets landed the same day. The
  convention above wins; override the field and record in the manifest that you
  did. Do not treat a pre-existing field as a decision someone else already made.

- **Regenerate `notes/cpp-tu-current-state.md`** with
  `python tools/cpp_tu_state.py --write-note` before you push. It is generated,
  it goes stale on every promotion, and `--check-note` goes red in CI. That flag
  **hard-fails on unstaged changes first** — "authority inputs have
  unstaged/untracked changes; stage or stash" — which reads like a different
  failure than staleness. Stage, then re-run.

- **A text-only promotion does not touch `symbols.txt`.** That file maps
  addresses to names and is unaffected by a source move; the reference promotion
  `e193406f3` edits only `delinks.txt`, the exceptions file, the manifest and the
  sources. If a launch prompt tells you to repoint `symbols.txt`, the prompt is
  wrong.
- **Branch:** `cpp/<Class>-tu`, class name verbatim including case —
  `cpp/dBgActor_c-tu`.
- **Facts JSON** goes on the same branch, not a separate `facts/` one. Scout and
  writer are one claim, so one branch.

## Before you claim: look for abandoned work

`classqueue.py claim` now prints a warning listing every branch and worktree
mentioning your class. **Read them.** The lock is ephemeral by design — `release`
deletes the ref — so a class abandoned mid-pipeline leaves *no* claim behind, and
its branch may be local-only where no remote check can see it. A writer
reconstructed `dScMgD3DBase_c` from scratch before noticing
`wip/dScMgD3DBase_c-humanizer-blocked-0905` in a sibling worktree with the class
already done.

Harvest the facts file from such a branch even when you reject its source; that
one's scout facts upgraded a vtable-extent claim from inference to proof.

## Done when

Pushed on `cpp/<Class>-tu`, claim released, and you have reported verbatim:

    python tools/tubuild.py verify <ov>/<Class>
    python tools/tubuild.py linkcheck <ov>/<Class>     # pre-promotion only

**Do not run `rombuild`.** An earlier version of this section asked for it, which
contradicted the pipeline's own split — the builder owns full-ROM proof, and a
writer running it duplicates 20 minutes of work and invites the writer to believe
its own green. Byte proof is the builder's job precisely because it is a
different instance.

Where you do run a parallel build, use `-j 6`, not `-j 16`, whenever sibling
writers are running — check with `ListAgents` or `python tools/classqueue.py
list`. Three concurrent `-j 16` builds oversubscribe the machine and slow all
three.

**`promote --dry-run` prints a refusal banner and still gives you the plan.** It
leads with `<<< promotion would be REFUSED` while `status` is `text-verified`,
which is expected and is not a defect in your branch — you need the plan in
order to *reach* the status flip. One run stopped after step 3, another printed
all six steps and exited 0; a third printed the **complete** plan (1, 2, 3, 4,
4b, 5, 6) and exited **1**. **So the exit code carries no information about plan
completeness** — read the output, never the status, and do not re-run just
because it exited non-zero.

**Two of its steps are missing from the hand-promotion list above, and nothing
gates them:** the dry-run also prints RETARGET of `srcPath` in
`config/match_provenance.jsonl` and `config/match_attempts.jsonl`. A writer
following the list alone leaves both dangling, and `check_dead_references` walks
only `.md`, so no gate notices.

**`no current eligibility report` from the pre-push hook is expected in a fresh
worktree** — it prints "skipping", not "passed", and it is not a failure. It
appears at exactly the moment you are deciding whether your push landed cleanly,
so read it and move on.

**`eligible.py` takes no file argument** (`-j` and `--no-isolate` only). You
cannot scope it to your class — grep `build/eligible-names.txt` afterwards.

**`-j` is a `linkcheck` flag, not a `verify` flag.** `tubuild.py verify -j 6`
dies with `unrecognized arguments: -j 6`. When this file or a launcher says "use
`-j 6` while siblings are running", that applies to `linkcheck` and `rombuild`.

**`wt-remove.ps1` prints `error: failed to delete ... Permission denied` and then
succeeds**, falling back to a robocopy purge and reporting `removed: True`. Read
the final line, not the error. This matters more than it looks: the rule never to
use `git worktree remove` is load-bearing — it deletes through the junctions and
empties the shared, non-redownloadable ROM dump — and an expected-looking failure
here is exactly what would tempt someone into the unsafe fallback.

**`wt-remove.ps1` takes `-Path`, not `-Name`** — unlike `wt-setup.ps1`. The rule
never to use `git worktree remove` is load-bearing (it deletes through the
junctions and empties the shared, non-redownloadable ROM dump), so a failed
teardown invocation must not tempt you into the unsafe fallback.

## When a subset is the right answer

Not every TU is promotable whole, and stopping short can be the correct result
rather than a failure. When the ROM's emission order cannot be reproduced by any
admissible source form:

**The destructor direction is per-class and is not inherited.** `daDkk_c`
derives from `daDsnBase_c`, and the two go opposite ways: ov025 orders `daDkk_c`
D1-below-D0 so it promoted whole 8-of-8, while ov091 orders `daDsnBase_c`
D0-below-D1 so it promoted 9-of-11. Measure your own class.

**Never infer destructor placement from a sibling header.** `daDgr_c.h` declared
its destructor out of line by analogy with `BigBrickBlock.h`'s leaf-class
convention; the ROM's own addresses refuted it, and merged, the two out-of-line
definitions were a duplicate definition *as well as* the wrong order. Read the
cartridge, not the neighbour.

**Destructor order is not the only reason to take a subset.** Two more, both
measured on `ov006/dScMgHanachan_c` (22 of 61):

- **A sourceless function splits the run.** `func_ov006_020ea914` has no `src/`
  file anywhere and no `delinks.txt` entry — the cartridge's own bytes cover it.
  **A licensed claim cannot have a hole**: no delink block and no TU manifest in
  the tree carries two `.text` runs. So a run with a sourceless member in the
  middle can only be taken as one of its two contiguous sides. Take the larger
  and say so.
- **Two file-global pragmas that contradict each other.** Four members reproduce
  only with `opt_strength_reduction off` and three only with it **on**. Both that
  pragma and `opt_common_subs` are file-global last-wins in mwccarm 2004/b56, so
  there is no positional escape — bracketing `off ... on` around exactly the four
  that want it gave byte-identical results to omitting it entirely. (`#pragma
  push`/`O3`/`pop` *is* positional; these two are not.)

  **SETTLED: `#pragma defer_codegen off` makes the bracket bind, and it recovered
  this exact class from 22 of 61 to 49 of 61.** The pragma-driven exclusions were
  all 27 of them; the only functions still held back are the sourceless hole and
  the 11 on its smaller side, which is a different and unfixable refusal. Measured
  on `ov006/dScMgHanachan_c` (PR #2309): `verify` 49/49 MATCH, `linkcheck [4b/8]`
  49 LICENSED with `emittedTextOrderIsRomAscending = true`, 106/106 modules exact,
  ROM sha256 identical to stock. The decisive probe was adding one
  `opt_strength_reduction off` member under a `push`/`pop` bracket: 23/23, with
  the two `opt_strength_reduction`-**on** members still matching. Deleting that
  one pragma line from the finished file drops it to 44/49 **and** 48 ordinal
  pairs out of ROM order — it buys the bytes and the order together.

  **So the rule above is true by default and false under `defer_codegen off`,
  now confirmed on all three pragma families tried.** Any partial that was cut on
  a pragma contradiction is worth re-measuring.

  **Why the bracket alone is not enough, stated precisely: with codegen
  deferred, the state that binds is the state at END OF FILE.** So a `push` /
  `opt_X off` / `pop` bracket restores the *other* members but cannot give the
  bracketed member its own setting. The full chain, measured on ov002/`Player`
  (301 members, one `#pragma opt_propagation off` at ordinal 35):

  | configuration | result |
  |---|---|
  | pragma present, file-global by default | 255/301 |
  | pragma deleted outright (the control) | 300/301 |
  | pragma bracketed in `push`/`pop` | 300/301 — **identical to deleting it** |
  | `#pragma defer_codegen off` + the bracket, source ROM-ascending | **301/301** |

  **The bracketed and deleted rows are the same number, and that is the point.**
  While codegen is deferred the bracket binds to nothing, so it is worth **zero**
  members — not "buys back the casualties but not its own". An earlier revision of
  this file printed 250 / 293 / 294 here and glossed a "294-vs-301 gap"; those
  figures do not reproduce. Re-measured on the shipped ov002/`Player` TU.

  **The ROM-ascending rewrite is not optional when you adopt it.** Same class,
  same bytes, three configurations:

  | source order | `defer_codegen` | bytes | emission order |
  |---|---|---|---|
  | ROM-descending | deferred (default) | 22/22 | ROM-ascending |
  | ROM-descending | **off** | 22/22 | **21 pairs NOT in ROM order** |
  | ROM-ascending | **off** | 22/22 | ROM-ascending |

  The rewrite is mechanical and belongs in the same edit.

  **The older, superseded framing follows.**
  Bracketed pragmas do not bind while codegen is deferred, which is the default,
  and the measurement above was almost certainly taken that way. Measured on
  `ov006/dScMgMemory2_c`: with `#pragma defer_codegen off` at the top of the TU,
  **52/52 MATCH**; removing that one line dropped it to **50/52**, and the two
  DIFFs were exactly the two functions carrying bracketed pragmas —
  `opt_propagation off` (27 words) and `opt_loop_invariants off` (6 words). The
  pair actually named above, `opt_strength_reduction` and `opt_common_subs`, was
  the last untested family; it has since been measured and behaves the same way
  (see the settled result at the top of this bullet). Do not take a pragma
  collision as an automatic subset until you have tried it, and report what you
  measure either way.

  **A second writer confirmed the lever on a third pragma AND confirmed the
  default.** Five controls on `ov006/dScMgRoulette_c`: `optimize_for_size on` at
  file scope with no closing `off` gave 23/40 (so the pragma really does move
  bytes there); bracketing `on`/`off` around one offender gave 40/40, i.e. the
  bracket did **not** bind and the trailing `off` restored the default file-wide
  — so the file-global last-wins rule above is **correct by default**; adding
  `#pragma defer_codegen off` to that same bracket made it bind, surgically.

  **`defer_codegen off` and source layout are ONE decision, not two.** The same
  run flipped emission order to "38 ordinal pair(s) NOT in ROM order", because
  that TU was written descending-ROM to compensate for deferred codegen's
  reversal. `linkcheck [4b/8]` refuses the mixture and it is a hard refusal.
  Adopting the pragma means **rewriting the TU ROM-ascending in the same change**.
  Budget for that before reaching for it, and never report a recovered `verify`
  count without saying whether emission order still passes — `linkcheck
  --partial` skips that audit entirely.

  **Beware proving nothing.** That writer's own first draft claimed the opposite
  from a single control: bracketed pragmas present gave 40/40, so the brackets
  "worked". Deleting the pragmas outright *also* gave 40/40 — both members match
  at plain `-O4,p` anyway, so the first control had no signal in it. Always run
  the delete-outright control before concluding a pragma did anything. Score every setting
  across the whole side and take the longest **contiguous** all-matching run:
  here SR-off scored 44/49 overall but SR-on gave the longest contiguous run, 22.

**When the key function falls outside your licensed range, the row-count formula
does not apply at all.** That TU emits no `_ZTV`/`_ZTI`/`_ZTS`, so there is no
`compiler_only_output` block — zero rows, not the ~9 the formula predicts — and
the header needs no edit. Check where the key function landed before deriving
anything.

The discriminator is which way the cartridge ordered the destructors. ROM **D1
below D0** is the reproducible direction and promotes whole (`ov029/daObjWcObj01_c`);
ROM **D0 below D1** cannot be reproduced by any admissible form and costs you the
pair (`daObjFloatBoard_c` 5-of-7, `daObjFallBlock_c` 10-of-12, `daDsnBase_c`
9-of-11). Check that before you plan the range, not after.

When you hold the pair out, prove it is unreachable rather than asserting it:
in all three cases the *only* references to either address anywhere in `config/`
were two `relocs.txt` entries landing on vtable slots 16 and 17, with no
instruction in the image calling either.

1. License the sub-range that *can* be ordered.
2. Leave the remaining shards enrolled — they keep their `delinks.txt` entries.
3. License the TU's own copies of those functions as `deadstrip-duplicate`.
4. Record why in the manifest's `boundary_evidence`.

`daObjFloatBoard_c` absorbed 5 of 7 this way: the cartridge puts `D0` at
`0x020b5a18` below `D1` at `0x020b5a70`, all three descendants inline the vptr
store, and **no instruction in the image calls either address** — only vtable
slots 16 and 17 name them. So the header's inline body is the cartridge's form
and the pair cannot be in this TU's licensed run.

State the refusal and its evidence in your report. A measured 5-of-7 is worth
more than a forced 7-of-7, and far more than a near-miss.

**A near-miss never lands in `src/`.** Restore the matched source and bank the
candidate in `nearmiss/db.jsonl`.

Hand to the humanizer. Do not open the PR — the builder does that.
