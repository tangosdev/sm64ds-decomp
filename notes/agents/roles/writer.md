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

Its "disqualifying complications" block is accurate and tells you up front what
you are dealing with. Trust it over the queue's `blockers` column.

## The loop

`verify` does not work on a class with no manifest — it exits `no manifest entry
for '<ov>/<Class>'; run 'create' first`. The real cycle is:

    python tools/tubuild.py create    <ov>/<Class>     # writes into src_tu/
    #   ... reconcile the merged source ...
    python tools/tubuild.py verify    <ov>/<Class>
    python tools/tubuild.py linkcheck <ov>/<Class>     # BEFORE promotion; see below

`create` emits in reverse source order and raises RAW review flags. Both are
accurate and both matter — read them.

**`tubuild.py promote` is `--dry-run` only.** It prints the plan and refuses to
act: *"only --dry-run is implemented … deliberately not available yet."* You
execute that plan by hand: move the source out of `src_tu/`, `git rm` the shards,
splice their N `delinks.txt` entries into one, fix `converted-baseline.json`,
fix the `port/` references. Note `git mv` fails here — the `src_tu/` file is
untracked at that point, so use a plain `mv` and `git add`.

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
| `deadstrip-duplicate` | a **function** that does have a ROM home |
| `deadstrip-data` | any `_ZTV` / `_ZTI` / `_ZTS` — plain `deadstrip` is rejected for these |

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
    python tools/rombuild.py -j 16 --no-rom

**A near-miss never lands in `src/`.** Restore the matched source and bank the
candidate in `nearmiss/db.jsonl`.

Hand to the humanizer. Do not open the PR — the builder does that.
