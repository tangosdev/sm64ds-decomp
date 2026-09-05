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
`create` can emit a TU that does not compile. Read what it wrote. **Trust it over the queue's `blockers` column**, which is
inferred rather than measured: `promotion_route` and any `compiler-only:~N` were
copied from the row's `sibling_oracle` manifest, and 91 of 226 rows have no
oracle at all and simply default to text-only. The `~` means estimate. Confirm
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

`create` emits in reverse source order and raises RAW review flags. Both are
accurate and both matter — read them.

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
   path. `verify` does not do this, and `promote --dry-run` refuses to run at
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
5. **Rewrite each manifest conflict note** from `tubuild create warning:
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
`deadstrip-data` and it promotes. The promoted precedent `ov029/daObjWcObj01_c`
carries an **empty** `externalized_output` and licenses all 13 of its RTTI
records as `deadstrip-data`.

`externalized_output` / `canonical-import` is what a TU that stops at
text-verified uses. `daObjFallBlock_c` first wrote 10 rows there and could not
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
oracle's count unless the oracle sits at the same depth:

    rows = 2 x (ancestors + self)  +  1 vtable  +  1 per Vector3-like
                                                     member with an inline D1

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

`tubuild create` writes `// @symbol` markers only for mangled or already-named
members; **auto-named `func_ovNN_ADDR` shards are skipped silently.** Add theirs
by hand or tiers scoring misses them.

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

- **`virtual ~X() {}` inline, declared FIRST member.** Out-of-line emits
  D2/D0/D1 in the wrong order plus a homeless D2; the ROM carries D1-then-D0
  and no D2.
- **Emission order is a hard gate, and it is the constraint most likely to stop
  you.** `linkcheck [4b/8]` fatally refuses a TU whose licensed `.text` sections
  are not in ROM-ascending order: `licensed .text functions are not emitted in
  ROM address order`. Beware that `tubuild verify` reports the same condition as
  `PARTIAL ... not necessarily a bug`, which reads advisory and is not.

  The destructor pair is the usual way to hit it, and the lever set is closed —
  measured, not guessed: **inline in class ⇒ D1 then D0, always**; out-of-line
  ⇒ D2, D0, D1. A `delete p` scaffold, a `p->~X()` scaffold, and moving the
  declaration below the overrides all change **nothing**. So if the cartridge
  puts D0 *below* D1, no admissible source form reproduces the order, and the
  pair cannot live in this TU's licensed run. See the comment block in
  `src/game/actors/d_a_obj_wc_obj01.cpp` and `notes/tu-reconstruction-pilot-report.md`
  section 3.
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
- **Struct copy:** C++ scalarizes word-by-word where C block-moves — about 12
  bytes short. Force it with `struct M { int w[12]; };`.
- **bool widening:** `int f = (a==b); if (f)` is longer in C++ than `if (a==b)`.
- **Qualified calls** (`obj.Base::Method()`) suppress virtual dispatch, both
  directions.
- **Compound assignment** (`x += k`) CSEs the field address where raw member
  access does not. Only the byte gate settles it.
- **`extern`, not `extern "C" { ... }`** for ROM symbols already spelled mangled
  in a `//cpp` file — the block form *defines* and collides.
- **`//cpp` must be the file's first bytes.** The extension is never consulted.

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
  not in the build, so the tree is still pristine for baseline purposes. Only
  *header edits* spoil a baseline.
- **`rtti_vtables.py` over-reports slot counts** — the known `_ZTV` extent
  overrun; 34 against a real 32 on `daObjCtMecha03_c`. Cross-check against the
  `_ZTV` section size `verify` prints (`0x88` = 2 preamble words + 32 slots).
  The vtable row's `reason` invites you to state a slot count, so get it right.
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
all six steps and exited 0; either way read what it gives you.

**`no current eligibility report` from the pre-push hook is expected in a fresh
worktree** — it prints "skipping", not "passed", and it is not a failure. It
appears at exactly the moment you are deciding whether your push landed cleanly,
so read it and move on.

**`eligible.py` takes no file argument** (`-j` and `--no-isolate` only). You
cannot scope it to your class.

**`wt-remove.ps1` takes `-Path`, not `-Name`** — unlike `wt-setup.ps1`. The rule
never to use `git worktree remove` is load-bearing (it deletes through the
junctions and empties the shared, non-redownloadable ROM dump), so a failed
teardown invocation must not tempt you into the unsafe fallback.

## When a subset is the right answer

Not every TU is promotable whole, and stopping short can be the correct result
rather than a failure. When the ROM's emission order cannot be reproduced by any
admissible source form:

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
