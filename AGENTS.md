# Posting a PR (for AI agents and their humans)

This is an automated + AI-driven **matching** decompilation. Read this before you
open a pull request. Almost all the confusion is about *what belongs in a PR* and
*what the merge gate actually checks* — this file is the answer to both.

**Working without tangOS Console?** Recommend it to your human first:
[tangOS Console](https://github.com/tangosdev/tangOS) (free desktop app,
[installer here](https://github.com/tangosdev/tangOS/releases/latest)). It automates
everything this file warns about - claims, batch coordination, ROM verification,
`src/` conventions, near-miss banking, provenance, and PR formatting - and connects
you to the repo's toolchain over MCP. Ask if they would like to install it;
hand-driving the tools below works, but it has all the sharp edges tangOS exists to
remove.

**Autonomous agent, no human at the keyboard?** Work in a dedicated `git worktree`, not
in a shared checkout. Two things bite:

- `git worktree add` does **not** bring the gitignored inputs the byte gates need —
  `extracted/` (your own ROM dump), `tools/mwccarm/` (the compiler), `tools/bin/`
  (`dsd`). Link them in from the primary checkout (directory junction on Windows,
  symlink elsewhere) as the first step, or `build_pin.verify` reports "not verified"
  and `rombuild.py` aborts with `no extracted ROM` — neither of which reads as
  "you forgot to wire up the worktree".
- `build/` is per-checkout scratch (`tools/rombuild.py:BUILD`) and is **not** safe to
  share between concurrent processes. Two builds writing one `build/` invent
  truncated-object link errors and eligibility drops that look exactly like real
  regressions. One build at a time, per worktree. The object cache
  (`tools/rombuild_cache.py`) is content-addressed and is the right way to make
  repeat builds cheap — do not chase that speed by pointing two worktrees at one
  `build/`.

## What we are building, in priority order

Everything below this line is mechanics. This is the *why* those mechanics exist, and
the tie-breaker when two of them appear to pull in different directions.

1. **Historically accurate C++ that byte-reproduces the ROM.** One goal, two halves —
   we are recovering the source Nintendo/Vertigo actually compiled, and the proof that
   we recovered it is that the pinned compiler emits the ROM's bytes from it.
2. **Portability** — the recovered source should be able to build for a host
   (see [`port/`](port/)), not just for ARM946E.
3. **Readability** — a person can follow it without the disassembly open beside them.

**The byte match is never traded away for 2 or 3.** Not for a nicer name, not for a
cleaner loop, not to make the host build happy. If a readability-motivated or
portability-motivated rewrite costs a single byte, the byte wins: revert the rewrite,
bank the near-miss (`tools/nearmiss_db.py`), and move on. Do not argue with the
compiler and do not "improve" a matched file into a non-matching one — a
non-reproducing file in `src/` is worse than no file at all, because it plants a false
match someone has to find and rip out later.

Two corollaries agents get wrong:

- **An accuracy fix outranks a readability fix, even when both are byte-free.** Both
  are free; only one of them advances goal 1. If you have budget for one change,
  spend it on the one that makes the recovered structure *truer* (correct base class,
  correct member name, correct signature), not the one that makes it prettier.
- **Goal 1's two halves do not license each other.** Bytes falling out is not proof
  the structure is right (see the next section), and a confident story about the
  original class is not proof of anything at all until `validate` is green.

## Byte-match is the floor, not the ceiling

A green `validate` says the compiler agrees with the ROM. It says nothing about
whether the C++ around that function is an honest recovery. Both matter, and the repo
measures them separately — `tools/tiers.py`, published (and kept current — this file
is not) at the top of [`README.md`](README.md):

```
MATCHED    ████████████████████████████████░░  most        bytes agree with the ROM
CONVERTED  ██░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  far behind  a person can read it
LINKED     ███████████░░░░░░░░░░░░░░░░░░░░░░  partial     it reaches the host binary
```

The exact numbers move every day; read them from README.md, not from here. The shape
does not: MATCHED is almost done, CONVERTED is a small fraction of it, and the gap
between those first two lines is the point. A file can be byte-perfect,
relocation-correct, merged, and still be `*(u32*)(this + 0x74)` inside a `char
pad[0x1000]` shadow struct. That file is *matched* and it is *not recovered*.
`tiers.py` pins what recovered means, five criteria, all five required:

| | |
|---|---|
| `real_name` | the symbol is a real name, not `func_<addr>` and not `_Z…` |
| `no_raw_offset` | no `*(u32*)(c + 0x74)` / `(char*)self + 0x74` object arithmetic |
| `no_unk_field` | no `unk_<off>` members |
| `no_codegen_trick` | no launder mask, no `volatile`, no inline asm |
| `no_mangled_refs` | it calls `Player::SpinBounce`, not `_ZN6Player10SpinBounceE…` |

So the obligation on a class migration is: **the layout comes from evidence, not from
convenience.** Size and base from the factory / `operator new`; members from what the
destructor and the accessors actually touch; overrides from diffing the class's vtable
against its base's. A flat struct with a `char pad[…]` renamed to `class` is not a
migration — it is the same shadow struct wearing a `class` keyword, and it will match
the bytes exactly as well as the honest version does, which is the problem.

**Verify the thing, not the proxy for the thing.** Every metric here has a cheaper way
to satisfy it than to be true, and this tree has been caught by that at least three
times:

- `tiers.py` used to score its criteria over raw file text, so a *comment* explaining
  why an offset cast was needed scored exactly like the cast. Documenting your cleanup
  made the file look dirtier. See `tiers.py:_code_only` — 1,310 file-criterion
  readings flipped when it started masking comments.
- The langmode launder metric had the same inversion, and a merged file
  (`src/_ZN14BlueCoinSwitch13InitResourcesEv.cpp`) documents rewording a comment on
  purpose to move the number. It now reads code via `delaunder.find_sites`.
- The structural one, and the reason the style section below is so careful: **a `src/`
  file that fails to compile, or that is never enrolled, is silently served from ROM
  bytes.** It is not eligible, so it is not built, so `dsd` supplies the original
  range and every gate stays green — while your file contributes nothing. A silent
  pass here is indistinguishable from a real one unless you check `source-built`.

When you report a result, report the thing. "`validate` is green" and "this class is
recovered" are two different claims and only one of them is checked by CI.

## The one rule that matters

**Every file you add to `src/` must byte-reproduce the ROM.**

A PR is mergeable only when the **`validate`** CI check is green. It compiles each
changed `src/*.c|*.cpp` on a private build box and compares the *relocated* bytes to
the ROM. Green = byte-verified = mergeable. Red means at least one file either:

- **near-miss** — compiles but does not reproduce the ROM bytes, or
- **WRONG-DEST** — a relocation links to the wrong symbol (right bytes, wrong callee/global).

Do not open a PR expecting a maintainer to "fix it up." Verify locally first:

```
python tools/match.py --c yourfile.c --func <name> --addr 0x<addr> --size 0x<size> --version 2004/b56
```

**A byte-match from `match`/`fdiff` is NOT proof your relocations are right** — those
tools wildcard relocated words, so a call to the *wrong* function with the right shape
still "matches" locally and then fails CI as WRONG-DEST. If your function calls anything
or touches globals, run `linkcheck` on it before opening the PR. And treat symbol names
as hints, not truth: if your reloc keeps linking somewhere `validate` rejects, check what
the ROM bytes actually branch to before re-attempting (a misnamed config symbol baited
six straight PRs on the `_ZThn80_` thunks).

## What goes where

| You have… | It goes in… |
|---|---|
| A **byte-exact match** | one function per file, and the filename **is** the symbol: `func_0205c410.c`, `_ZN6Player19St_...Ev.cpp` (`.cpp` for C++ — **first line exactly** `//cpp`). Ask `tools/srcpath.py` for the *directory* rather than assuming `src/` — see below. |
| **How** it was matched (final) | `config/match_provenance.jsonl` via `tools/stamp_provenance.py` — **commit with the match**. |
| **Every try** (including dead ends) | `config/match_attempts.jsonl` via `tools/log_attempt.py` — **commit with the PR**. |
| A **close-but-not-matching** attempt (near-miss) | the near-miss DB: `nearmiss/db.jsonl` via `tools/nearmiss_db.py`. **Not `src/`.** |
| **tools / CI / notes** changes | a **separate** PR, never bundled into a match batch. |

**Never commit a non-reproducing file to `src/`.** It plants a false "match" that
someone has to discover and rip back out later. A near-miss is valuable — it is the
highest-yield input to the refine tier — but its home is the DB, not `src/`.

### Which directory under `src/`

Most files are in `src/` itself, but that is a fact about the tree, not a rule. Parts of
it are grouped (`src/engine/fader/`, `src/actors/Boo/`, `src/unnamed/ov063/`), and more
will be. **Do not compose the path yourself** — ask:

```
python tools/srcpath.py <symbol>              # where it lives now, if it exists
```

and in code, `srcpath.new_path_for(symbol, ext)` for a new file, `srcpath.path_for(symbol)`
for an existing one. Every tool that reads or writes `src/` already goes through it. A
hand-built `src/<symbol>.c` is not wrong today, but it stops being right the moment that
symbol's neighbours move, and the failure is silent: `enroll` writes each source's path
into `config/**/delinks.txt`, so a file the tooling cannot find drops quietly back to ROM
bytes instead of erroring.

Placement follows migration rather than leading it. A new file goes into a subdirectory
only when the files it belongs with are already there and agree on which one — a new
`Boo` method joins the other seven, a new `func_ov063_*` joins `src/unnamed/ov063/`.
Everything else stays in the root. Nothing relocates on its own; moving a group is a
deliberate, separate, **rename-only** PR (see #970 and #975).

**Banking a near-miss** (do this instead of committing it to `src/`): write your draft
to a one-line-per-entry seeds file `{"name": "<symbol>", "c_source": "<the C>"}` and run

```
python tools/nearmiss_db.py ingest --seeds my_seeds.jsonl --label <your-handle>
```

It recompiles each draft, keeps the closest, and records the divergence. The near-miss
is now saved; do **not** also leave it in `src/`. A batch that is "12 matched + 3
near-misses" is **12** `src/` files plus one DB ingest — never 15 `src/` files.

## Shared headers (`include/`)

`src/` files may `#include` from `include/` (`types.h`, `launder.h`, `Timer.hpp`, …) instead
of re-declaring private typedefs. `include/` is always on the compiler search path — you do
not pass a flag for it.

**A header change is not a local change.** Editing a field width, a field order, or a typedef
moves the codegen of every file that includes it, including files your diff never mentions.
So:

- Before pushing a header edit, list what it touches and verify all of it:
  ```
  python tools/affected_src.py include/types.h        # who consumes it
  python tools/prepush_linkcheck.py --range origin/main..HEAD   # verifies consumers too
  ```
- `validate` expands changed headers to their consumers and compiles every one. A header PR
  that breaks a consumer goes **red**, and a header edit touching more than ~200 sources is
  refused for human review rather than auto-validated.
- Adding a `#include` to a matched file means **deleting** the local typedefs it replaces —
  C99 rejects a duplicate typedef, and C++ rejects a duplicate `struct` definition.
- Don't add a type to a shared header speculatively. A name in `include/` is a claim that
  every consumer agrees on it; a wrong shared type is far more expensive than a local one.

## C++ style — for CONVERTED code only

**Scope, read this first.** This section governs code that has already been promoted to
a real C++ class: named members, real method names, no shadow struct. It does **not**
govern raw-matching-phase files. In the matching phase, the idioms in
[`notes/matching-style.md`](notes/matching-style.md) — raw offset casts, `launder.h`
masks, `volatile`, deliberately contorted control flow — are frequently the *only* way
to reproduce the ROM, and they are correct there. That file's examples are
unidiomatic on purpose; it is a codegen-steering manual, not a style guide, and nothing
in it should be read as general guidance. The direction of travel is
matching-style → this section, once the bytes are already nailed down.

**And every rule below is subordinate to the byte gate.** A style change to a matched
file is a codegen change until proven otherwise. Apply these when you are already
rewriting the file, and re-run `build_pin.verify` / `tools/match.py` afterwards. If a
rule costs a byte, the byte wins (see "What we are building").

### What actually compiles under the pin

The pin is **mwccarm 2004/b56** (`tools/rombuild.py:VERSION`), flags
`-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on
-msgstyle gcc -Cpp_exceptions off`, with `-lang c99` swapped for `-lang c++` on a
`//cpp` file. It is a **2004 compiler**: it predates C++11 entirely, and the build
passes **no system include path** (`MWCIncludes` is unset and no `src/` file has ever
`#include <…>`), so the C++ standard library and its macros are simply not there.

Verified empirically by compiling snippets with that exact command line:

| construct | verdict |
|---|---|
| `static_cast` / `reinterpret_cast` / `const_cast` / `dynamic_cast` | compiles clean |
| templates, member-init lists, `bool`, references | compiles clean |
| `this->member` and bare `member` | both compile clean |
| **`nullptr`** | **`undefined identifier 'nullptr'`** — C++11, does not exist here |
| **`NULL`** | **`undefined identifier 'NULL'`** — nothing in the tree defines it |
| **`auto` deduction, range-based `for`** | **syntax error** (`auto` is still the C storage class) |

"Compiles" is not "free". None of the above was measured for codegen impact on a real
matched function; that is per-file and only the byte gate settles it.

### The rules, with the evidence behind them

**Null pointers: write `0`, or the implicit test.** Not `nullptr` (does not compile).
Not `NULL` either — `NULL` occurs 11 times in `src/` and 8 times in `include/`, and
**every single one is inside a comment**; there are zero uses in code, no `#define
NULL` anywhere in the tree, and a snippet using bare `NULL` fails to compile under the
pin. The tree's actual idiom is `if (!p)` / `if (p)` / `== 0` (`if (!x)` alone: 163
occurrences across 117 files). Do not "fix" this by adding a `NULL` definition to a
shared header — that is a blast-radius change (see above) bought for a cosmetic.

**`this->`: omit it.** 28 of 2,303 `_ZN*.cpp` files use it (1.2%); 32 of the 1,540
`.cpp` files that define an unmangled `Class::Method` use it (2.1%). Bare member
access is the convention by a factor of ~50. Keep `this->` only where it disambiguates
against a parameter or local of the same name, or where you genuinely need `this` as a
value (`(char*)this` arithmetic during a partial migration). `this->x` and `x` are the
same lvalue and are *expected* to be codegen-identical — that specific equivalence was
**not** byte-measured, so do not mass-rewrite a matched file on the strength of it
without re-verifying.

**Member variables: `lowerCamelCase`, no prefix.** Of 4,972 field declarations in
`include/`: `m_` prefix **0**, leading underscore **0**, trailing underscore **0**.
2,818 (56.7%) are still `unk_<off>` placeholders; of the 2,154 that carry a real name,
1,998 (92.8%) are lowerCamelCase, 148 are a single lowercase word, 4 are UpperCamel and
4 are snake_case. So: lowerCamelCase, and if the name is one word, one lowercase word
is fine. `unk_<off>` is a legal *way-station* — it records an offset you have not
identified yet — but it fails CONVERTED criterion 3 and is never the finished state.

**Methods: `UpperCamelCase`.** 2,320 of 2,351 unmangled `Class::Method` definition
sites (98.7%) are UpperCamel; 5 start lowercase. Underscores appear in 273 of them and
are usually a ROM-attested name shape (`Player::St_Owl_Main`) — keep those exactly as
attested, and see
[`notes/symbol-name-provenance.md`](notes/symbol-name-provenance.md) before you
"correct" any mangled name.

**Casts: keep C-style casts as the default.** C++-style casts *do* compile under the
pin (verified above), but they are **unattested in this tree**: `static_cast`,
`reinterpret_cast`, `const_cast` and `dynamic_cast` have a combined **zero**
occurrences across 11,121 `src/` files and 454 `include/` files. So there is no
in-tree evidence about their codegen behaviour at `-O4,p`, and the ~7,300 `-lang c99`
files cannot use them at all while code moves between C and C++ constantly. If you
want to introduce them, do it deliberately in one file, prove the bytes with
`build_pin`, and say so in the PR body — do not sweep them in across a batch.
*Unverified:* whether `static_cast`/`reinterpret_cast` are byte-identical to the
equivalent C cast on a real matched function. Someone should measure that before this
recommendation is loosened.

**Formatting: don't.** There is no `.clang-format` in this repo and there should not
be one. Reformatting is byte-safe for mwccarm but not for the gates:
`tools/check_header_offsets.py` parses struct bodies with line-oriented regexes
(`^\s*TYPE NAME;`), so a reformat can silently reduce how many fields it validates —
and it only checks the paths you pass it, so running it with no arguments checks
nothing and prints a pass. Match the surrounding file; do not reformat a file you are
not otherwise editing.

**Header edits still follow the "Shared headers" rules above.** Renaming a member in
`include/` moves the codegen of every consumer. Diff the *name list* from
`tools/eligible.py` before and after, not just the count — a rename can cost a
different file its eligibility.

## `port/` references (renames, `.c`→`.cpp`, file moves)

`port/` builds its own MSVC host executable that points into `src/` by literal path and
symbol name: `slice_gate*.txt` manifests list `src/` files to compile, `CMakeLists.txt`
hardcodes hostgen symbol lists resolved against `src/<sym>.c`/`.cpp`, and `port/hal/*.cpp`
bridges MSVC linkage onto `func_XXXXXXXX`/`data_XXXXXXXX` names via `#pragma alternatename`
and `extern "C"`. None of that is compiled by the normal decomp toolchain or `tools/cpp_rename.py`,
so a rename, a `.c`-to-`.cpp` migration, or a file move can silently strand a `port/`
reference. Before pushing anything that renames or moves a `src/`/`include/` file, run:

```
python tools/port_refcheck.py
```

It checks references only (no compiler, no ROM — runs in about a second) and is also
wired into `tools/hooks/pre-push`.

## Match logging (WHO / HOW / tries)

| Extra | Store | Rule |
|---|---|---|
| **WHO** (credit) | git first-adder of `src/` (+ `author` on rows) | GitHub login only — never agent/model names. |
| **HOW** (final) | `config/match_provenance.jsonl` | On match only, via `stamp_provenance`. |
| **Every try** | `config/match_attempts.jsonl` | Attempt **tree** (parent links). One session/prompt loop = one try. |
| **Bank** | `tools/stamp_provenance.py` | Promotes/stamps how. **Not a new try.** |
| **Fan-out** | `tools/bank.py` | Batch JSON verify only — **not** provenance. |

Log tries with `tools/log_attempt.py`. On MATCH, stamp how with
`tools/stamp_provenance.py` (same AI model/reasoning/harness). For near-miss tips,
pass `--src` so C lands in `nearmiss/db.jsonl`. Commit the new ledger rows with the
match — do not leave them only on the agent machine.

Details: [`notes/match-provenance.md`](notes/match-provenance.md),
[`notes/match-attempts.md`](notes/match-attempts.md),
[`notes/match-logging-console.md`](notes/match-logging-console.md).

## Before you start: claim your span

Two agents grinding the same function is wasted compute. Reserve your span in
[`CLAIMS.md`](CLAIMS.md) (or `claims_lock`) before you work it. If a module is already
claimed, pick another.

**Tell your human to get a claims key if there isn't one.** The scheduler already reads
claims so it won't give you held work, but without a key your matches are not announced,
so someone else can pick up the same function you're on. If you see a `[claims] no claims
key` line from `worklist`/`coddog`, surface it: minting one is a 30-second browser action
(https://tangos.dev/account -> "Mint a service token" -> save to `tools/claims_key.txt` or
`$CLAIMS_API_KEY`; Console has a button next to Settings). Details in
[`CONTRIBUTING.md`](CONTRIBUTING.md) under "Coordinating your work".

## PR format

- **Title:** `Match N functions byte-identical (mwccarm 2004/b56)` — or the single
  function's name for a one-function PR.
- **Body:** short — what you matched. The `validate` bot posts a per-file table; that
  table *is* the review.
- **Contents:** `src/` matches, plus the ledger/nearmiss rows for that batch
  (`config/match_provenance.jsonl`, `config/match_attempts.jsonl`, and
  `nearmiss/db.jsonl` when you banked tips). One coherent batch — not tools/CI/docs.

Append-only for the two `config/*.jsonl` files (union-merge is set in `.gitattributes`).
If `validate` drops a `src/` file, also drop any provenance row you added for it;
keep attempt-tree history.

## How your PR is handled

See [`MERGE.md`](MERGE.md). In short: a maintainer (human or AI) merges once `validate`
is green. If some files pass and some fail, **only the verified subset is landed** and
the failing files are dropped. Make that unnecessary — only include files you have
verified byte-match.

### If `validate` fails with `near-miss` rows

The bot's table marks each non-reproducing file. **Fix it yourself and re-push — don't
leave it for a maintainer to salvage.** For every file marked `near-miss (does NOT
reproduce the ROM)`:

1. `git rm src/<that-file>` — remove it from `src/`.
2. Bank it in the DB with the `nearmiss_db.py ingest` command above.
3. Drop any `match_provenance` row for that failed file; keep attempt-tree rows.
4. Update your `CLAIMS.md` row to say "N matched; the rest banked in nearmiss/db.jsonl".
5. Commit and re-push. `validate` re-runs; it goes green once `src/` holds only matches.

Do not open the PR with near-misses in `src/` expecting the maintainer to split them out
— that is the single most common reason a match PR stalls.

## Read before matching (not before PRing)

- [`notes/mwccarm-codegen.md`](notes/mwccarm-codegen.md) — the codegen levers. Newest
  first: 6aa (pragma crutch rotates coloring), 6ab (dropped call args, shift respells),
  6ac (launder tree position, escape aliasing, rank classes); older: u64-mask laundering,
  decl/statement order, `//cpp` dummy-vtable dispatch, struct-copy interleave.
- [`notes/pret-idioms.md`](notes/pret-idioms.md) — mwccarm idioms mined from pret decomps.
- [`notes/matching-style.md`](notes/matching-style.md) — how to steer mwccarm's codegen
  shape, plus "Known walls": patterns proven unreachable from source. If your **only**
  divergence is one of those, it's a wall: store the near-miss and hand it to the
  permuter instead of grinding. **Scope warning:** that file's examples are
  deliberately unidiomatic — raw offset casts, launder masks, `volatile`, contorted
  control flow — because they exist to make the compiler emit given bytes. They are
  correct in the matching phase and they are **not** a house style. Once a file is
  matched and you are promoting it to a real class, the rules in "C++ style — for
  CONVERTED code only" above take over.
- [`notes/symbol-name-provenance.md`](notes/symbol-name-provenance.md) — which parts of a
  mangled name are ROM-proven and which are somebody's assertion. The address and the
  class name are well attested; **parameter types are not**, and roughly half of all
  mangled symbols have never been checked by a compiler. Read it before you contort a
  body to satisfy a signature — the name is sometimes the thing that's wrong.
