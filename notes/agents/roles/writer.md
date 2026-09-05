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
| `deadstrip-data` | any `_ZTV` / `_ZTI` / `_ZTS` — plain `deadstrip` is rejected for these |

**The row count is predictable from inheritance depth**, so do not copy an
oracle's count unless the oracle sits at the same depth:

    rows = 2 x (ancestors + self)  +  1 vtable  +  1 Vector3 D1

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
- No C++11: no `nullptr`, `auto`, `override`, STL, east-const. This is 2004.

Ask the compiler rather than hand-mangling:

    python tools/mangle.py candidate.cpp --expect _ZN...

## Conventions — fixed, so nine agents do not pick nine ways

- **File:** `src/actors/<Class>.cpp`, the class name exactly, matching the
  class-named majority and the `layout_check` L2 stem rule. (`d_a_*.cpp` snake
  names also exist in the tree; do not add more.)
- **Branch:** `cpp/<Class>-tu`, class name verbatim including case —
  `cpp/dBgActor_c-tu`.
- **Facts JSON** goes on the same branch, not a separate `facts/` one. Scout and
  writer are one claim, so one branch.

## Done when

Pushed on `cpp/<Class>-tu`, claim released, and you have reported verbatim:

    python tools/tubuild.py verify <ov>/<Class>
    python tools/rombuild.py -j 6 --no-rom

Use `-j 6`, not `-j 16`, whenever sibling writers are running — check with
`ListAgents` or `python tools/classqueue.py list`. Three concurrent `-j 16`
builds oversubscribe the machine and slow all three.

## When a subset is the right answer

Not every TU is promotable whole, and stopping short can be the correct result
rather than a failure. When the ROM's emission order cannot be reproduced by any
admissible source form:

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
