---
name: decomp-tu-build
description: Execute the sm64ds-decomp translation-unit rebuild/verify loop - reconstruct a merged TU in src_tu/, compile it with the pinned mwccarm, byte-verify every member, run the relocation audits, and take it up the shadow -> text-verified -> link-verified ladder. Use when merging one-function files into a real .cpp, when tubuild.py reports MISSING/DIFF/unlicensed, when a TU compiles but will not link, when generating a TU-granular config_tu/ delinks root, or when asked to convert a module to the ph shape. For deciding WHICH functions share a TU, use decomp-tu-slicing first.
---

# The TU rebuild / verify loop

`src/` holds ~11k one-function files, a shape the original build never had.
`tools/tu_map.py` recovers the real `.cpp` boundaries; **this skill is about executing
the merge and proving it**. For deciding *which* functions belong together, read
`decomp-tu-slicing` first — a class is not a file.

## 0. Prerequisites, in this exact order

```sh
python tools/rtti_extract.py     # -> build/rtti.json
python tools/rtti_vtables.py     # -> build/rtti_vtables.json
python tools/tu_map.py           # -> build/tu_map.json
```

`build/` is gitignored, so a fresh worktree has none of this — always regenerate.

**Nothing enforces that order.** `tu_map.py` reads `build/rtti_vtables.json` if it is
present and silently falls back to an empty label table if it is not, so a missing
prerequisite produces a complete, self-consistent, **wrong** map — 516 TUs instead of
532 — with every gate passing and **exit 0**. Verify the prerequisites yourself; no exit
code will do it for you, and `--check` will not either (it exits 0 on failure too). The
full damage table is in `decomp-tu-slicing` §0.

`tubuild.py` regenerates the map only when it is *entirely absent*; a stale-but-present
map is reported as a note and then used as-is.

`--blind` is a negative control, not an opt-out: it drops the mangled-name signal and
scores what the map retains on RTTI alone, which is the honest measure for the anonymous
overlays. It is not something to reach for in a normal run. There is no flag that
suppresses the RTTI labels while keeping the names.

Figures move whenever the map changes — run the command rather than quoting these.
Measured on `main` at `343eab070` with the full chain: **74 modules, 11,091 functions,
532 TUs**, boundaries `{low: 68, medium: 110, high: 280}`, 400/532 carrying a class.

## 1. The loop

```sh
python tools/tubuild.py list                      # candidate worklist
python tools/tubuild.py inspect ov045/PoleLift    # one candidate, full detail
python tools/tubuild.py create  ov062/Chuckya     # generate a shadow .cpp
python tools/tubuild.py compile ov045/PoleLift
python tools/tubuild.py verify  ov045/PoleLift    # byte + relocation verification
python tools/tubuild.py partial ov045/PoleLift    # N derived per-function objects
python tools/tubuild.py linkcheck ov002/LevelObjects
python tools/tubuild.py promote  ov045/PoleLift   # --dry-run only
```

`tubuild.py` writes **only** `src_tu/`, `config/tu_manifest.d/`, and `build/tu/`. It
never touches `src/` or `config/**/delinks.txt` and — in its own words — never runs real
`eligible.py` / `rombuild.py`, though it imports both as libraries. Keep it that
way. It delegates every byte and relocation check to
`match.py`, `objisolate.py`, `reloc_audit.py`, `build_pin.py` — never reimplement those.

`create` **refuses** a legacy file whose body is wrapped in `extern "C" { }` — its
splitter consumes the whole block as extern declarations and then reports
`scanned to end of file without finding a function body`. Hand-assemble, then build the
manifest entry with `tubuild.build_manifest_entry` so the schema stays the tool's own.

### The status ladder

| status | means |
|---|---|
| `shadow` | exists, nothing proven |
| `text-verified` | every declared function's bytes reproduce in TU context |
| `link-verified` | a scratch link reproduces the whole licensed text range |
| `data-verified` | declared data/BSS contributions pass too |
| `promoted` | canonical; legacy one-function files deleted |

## 2. FUNCTION ORDER IS REVERSED

**mwccarm 2004/b56 emits one `.text` section per function, in the REVERSE of source
order.** So write the highest-address ROM function FIRST. The rule holds between
distinct definitions but **not inside a destructor's variant group** — see
`decomp-cpp-class-form`.

Consequence for tooling: `st_value` is 0 for every symbol because each function has its
own section. **Order must be read from the ELF section index, never from `st_value`.**

## 3. Hazards that have already cost days

- **`#pragma opt_propagation` / `optimize_for_size` are FILE-GLOBAL.** Position is
  irrelevant; the last state set anywhere applies to every function. One re-added member
  carrying a stray pragma silently recompiles the whole TU (ov062/001: 14 match/24 differ
  from one stray `optimize_for_size on`; removing it gave 36/2). If a pragma buys one
  member a match and costs four others, **drop it** — maximise cartridge-verified
  functions, never protect a single pre-existing match.
- **Greedy per-member admission is the whole ballgame.** All-or-nothing merging is a
  cliff, not a slope: 80% of 1–4 function TUs compiled, **0 of 159 with >=10 members did**.
  Admit one member at a time, keep it only if the TU still compiles. Never trade a MATCH
  for a DIFFER. Cap at ~200 members or it goes quadratic.
- **Reconcile in place; NEVER hoist declarations.** Five of five hoisting experiments
  regressed (preamble hoist 72->62, dependency-ordered hoist 72->46). Files carry real
  ordering dependencies — some declare `typedef int s32;` inline. Keep one declaration at
  its first occurrence and delete later duplicates.
- **`extern "C"` is a DIAGNOSTIC, not just a fix.** Give every definition C linkage
  first, then read the errors — it exposes declaration conflicts C++ was silently
  swallowing as overloads. A TU that "compiles" before that step has proven nothing.
- **Reconcile conflicts by keeping the most complete observation** — `extern Mtx43 data_x`
  over `extern char`, `void f(void*)` over implicit `int f()`. Each one-function file
  declared only what it needed; the union beats any single view.
- **`999 word(s) differ` means SIZES differ**, not a type error — usually the D0/D1/D2
  collapse artifact.
- `include/decl_common.h` is sometimes actively wrong (declares a TU's own functions as
  *data*). Dropping it and restating the 3–17 lines it supplied has fixed real mismatches.

## 4. A byte MATCH is not proof

`match.compare` **wildcards every relocated word**, so a member can reproduce the bytes
while calling the WRONG function. This cost a full day: ov077's `func_ov077_02124118`
called `ApproachLinear` where the ROM calls `ApproachLinear2` — same signature shape,
reported MATCH.

**Always confirm the run reported all three.** `verify` prints the first two as labelled
lines and folds the third into its verdict line:
```sh
byte comparison   : 7/7 MATCH  (tools/match.py extract_func + compare, relocation-aware)
objisolate check  : clean  (tools/objisolate.py plan() -- relocation type/addend ...)
...
Result: 7/7 MATCH, objisolate clean, reloc-destinations clean -> TEXT-VERIFIED
```
There is no standalone `reloc-destinations:` line — a destination failure shows up on
the offending symbol's row as `N reloc destination(s) WRONG (first: ...)`, and the
`Result:` line degrades to `NOT verified`. `verify` **does** exit non-zero when it is not
text-verified (unlike `tu_map.py --check`, which always exits 0). If any of the three
could not run, the result is "not verified" — never "probably fine".

## 5. Enrollment and the ph-shaped config

`config/**/delinks.txt` is per-function and is the **sole authority over what the ROM
build compiles**. Converting a module to the ph shape means standing up a parallel
TU-granular delinks root under `config_tu/`, whose entries are TUs rather than
functions.

**The generator for that root is not in the tree.** Nothing in `tools/` emits
`config_tu/` today, so there is no command to run here — do not go looking for one. The
invariants below are what make such a root correct, and they hold however it is
produced.

- **Module-level exclusivity**: a module lives in the old shape or the new one, never
  both — two entries claiming the same bytes is incoherent. Presence of a `delinks.txt`
  in `config_tu/` *is* the conversion flag.
- A delinks file is a **partition**. Verify no overlaps, no gaps, and that nothing the
  old config claimed became unclaimed. The new root must not claim more text than the
  old one did.
- **Without `complete`, dsd supplies the range from ROM bytes and your source is never
  compiled.** A TU-granular root with no `complete` markers is an objdiff *target* root,
  not a build. Check `source-built`, not `eligible`.
- Attributing a TU's `.data` is what makes its `_ZTV` TU-owned instead of gap-owned —
  which is what unblocks a key-function TU's link.

Once a root exists, one ordering constraint on it is absolute: **`dsd delink` must run
before `dsd objdiff`**, because objdiff consumes what delink wrote.

```sh
tools/bin/dsd.exe delink  -c config_tu/arm9/config.yaml
tools/bin/dsd.exe objdiff -c config_tu/arm9/config.yaml -o build/tu/objdiff
```

### The attribution rule

Object *k*'s contribution to **every** section precedes object *k+1*'s. Discard any rule
whose per-TU intervals are not ascending and disjoint. Do **not** use "TU *k*'s code
loads this address, so TU *k* owns it" — that direction produced a non-monotone ordering
in ov045 and would have swept 1,760 bytes of a **data-only object with no `.text`** into
the wrong TU. Follow only pointers a run *contains*, then close transitively over
referrers (`_ZTV` -> `_ZTI` -> `_ZTS`).

## 6. Do not

- Run `python tools/rombuild.py` or `python tools/eligible.py` when another agent shares
  the worktree — they write shared `build/` state and invent link errors that look real.
  Separate worktrees are safe (`build/` is per-worktree; only `build/objcache` is shared,
  and it is content-keyed).
- Tear down a wired worktree with `git worktree remove`. The gitignored inputs
  (`extracted/`, `tools/mwccarm/`, `tools/bin/`) are normally linked in rather than
  copied, and git **recurses into the link and deletes the target's contents** — that is
  the real ROM dump, and it is not re-downloadable. Unlink first, then remove. See
  `notes/worktree-inputs.md`.
- Report a status the run did not print. A partial honest result beats an optimistic one.
