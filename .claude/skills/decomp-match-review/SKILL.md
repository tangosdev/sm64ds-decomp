---
name: decomp-match-review
description: Review a decompilation matching change the way a decomp maintainer would - first verifying the match is real (fakematch / NONMATCHING-banner / wrong-compiler checks), then whether the recovered source is admissible, then what it costs the host port. Use when reviewing or preparing a matching PR/branch/diff in a decomp project (sm64ds-decomp, NSMB-Decomp/nsmb, or any objdiff/dsd/mwccarm tree), or when asked to check matched source for fake matches, magic addresses, un-recovered types/structs, wrong or contradictory extern signatures, formatting drift, portability / host-build impact, or PR shape before submitting.
---

# Decompilation match review

## What the project is for — read this before you rank anything

sm64ds-decomp's goal, in the maintainer's words and in this order:

1. **A historically accurate representation of the C++ version of SM64DS, byte-matched to the
   `.nds`.** One goal, two halves, both required.
2. **Portability.**
3. **Readability.**

The two halves of goal 1 are not equally defended. **The byte match is falsifiable and gated;
the C++ accuracy is neither.** `match.py` fails loudly when the bytes are wrong. Nothing fails
when a file byte-matches while spelling `_ZN9ActorBasenwEj` as a C identifier, declaring a
destructor `int(char*)`, or leaving a member filed as a free function. That asymmetry is why
the C++ half rots, and it is what a human reviewer is *for* — everything the gates catch, they
already caught before you opened the diff.

**The pass order below is review sequencing, not this ranking.** Pass 0 runs first because it is
the cheapest disqualifier and a fake match is a lie in the progress bar — not because bytes
outrank structure. Do not read pass order as priority order.

Two consequences worth stating, because they resolve most ranking questions on their own:

- **A byte-free fix that recovers original structure outranks a byte-free readability fix.**
  Both are free; only one delivers goal 1. Renaming `sp0` → `index` is free and welcome.
  Correcting `extern int _ZN5ActorD2Ev(char*)` to the signature the mangled name already states
  is *also usually free* — "a pointer and an `int` are both one register" — and it is primary
  work. Ask for the second at least as hard as the first. (Usually free, not always: signedness
  changes instruction selection and a float changes codegen wholesale, so it stays a
  verify-cycle question — see B1 — never a patch you hand over.)
- **When accuracy and readability conflict, accuracy wins, and this is the common case.**
  Unnatural store orders, odd-looking casts, and `static` on a callback are frequently what the
  original contained. The "Do NOT recommend these" section near the end is that principle in
  list form; it is not a list of exceptions, it is goal 1 outranking goal 3.

Portability sits between them: worth reporting, never worth breaking goal 1 for. See
"Port impact."

### The safety ladder — what you can actually ask for

Stated once here; everywhere else in this skill points back to it. **"Safe" means codegen-free
AND gate-invisible.** Those are two different predicates, and conflating them is how a reviewer
does damage while believing the change is free.

| Change | Codegen | Gates | Ask for it? |
|---|---|---|---|
| Renaming a **local** variable | free | invisible (not a field, symbol, or offset) | **Always.** Propose the names. |
| `this->` on member access | free | invisible | **Always.** |
| Adding a **comment** | free | ⚠️ **not always invisible** — see below | Usually, with the caveat |
| Correcting a declaration to the demangled truth | *usually* free | — | As a **verify-cycle question**, never a patch |
| Reformatting | free | **destroys them in sm64ds** (A1) | Only where a `.clang-format` exists |
| Portability edits to matched source | varies | — | **Never.** See "Port impact". |

⚠️ **Why comments are not unconditionally free in this tree**, despite being free to the
compiler — two textual gates read them:

- Lines added *above* a `NONMATCHING` banner can push it past the **200-byte** window the
  progress DB scans, silently flipping a draft to "matched" (Pass 0 rule 1).
- The langmode launder/forced metric is a **regex over source text**, so a comment that *names*
  a hack raises a ratchet that fails closed — even though this skill elsewhere *asks* for
  exactly such comments (tagging a steering pragma, labelling a launder idiom).

Neither makes comments a bad idea. Both mean "just add a comment" is advice with a footnote:
check where the banner sits, and expect the langmode ratchet to notice.

---

A byte-match is the entry ticket, not the deliverable. This review has two halves:

- **Pass 0 — is it matched at all?** Cheap, mechanical, and the only half an outside critic
  will ever audit. Detail: `references/fakematch.md`.
- **Passes A–C — is the source admissible?** Everything the byte-gate cannot see: is this
  plausibly what the original developers wrote, and is it filed the way this project files
  things? Detail: `references/guidelines.md` (46 maintainer comments across NSMB-Decomp/nsmb
  PRs, with verbatim quotes and worked examples). Read it before judging B1–B4 — the "is this
  a cast?" and "is this a struct?" calls need the examples.

Do them in that order. Never open Pass A while Pass 0 is unresolved: an inadmissible match is
a maintenance problem, a fake match is a lie in the progress bar.

There is a third thing worth reporting and it is **not** a third pass: **port impact.** sm64ds
has a host build, which makes a second compiler available over the same source — and a second
compiler sees the one defect class this tree's own gates are structurally blind to (wrong and
mutually contradictory `extern` signatures). It also creates a standing temptation to demand
portability cleanups that break matches. The rules for keeping the first without the second are
in "Port impact" below; read them before you write a single portability finding.

## Scope

Establish what you are reviewing before reading code:

- **A PR** → `gh pr diff <n> --repo <owner/repo>`; also `gh api repos/<r>/pulls/<n> --jq
  '"files=\(.changed_files) +\(.additions)/-\(.deletions) commits=\(.commits) \(.mergeable_state)"'`
- **Working branch** → `git diff main...HEAD --stat` then the diff
- Read the project's `.clang-format`, `contributing.md`/`CONTRIBUTING.md`, `MERGE.md`, and
  `AGENTS.md`/`CLAUDE.md` if present. **Project rules outrank everything in this skill.**
  Note which of these are *absent* — sm64ds has no `.clang-format` on purpose (Pass A1).

Review only changed lines unless a whole file is new.

## Pass 0 — verify the claim (do this first, always)

CI passing is evidence, not proof. The absence of a `NONMATCHING` banner is not even
evidence. Read `references/fakematch.md` in full the first time; the four rules:

1. **A removed `NONMATCHING` banner is a match claim — rebuild it.** In sm64ds the progress
   count, the percentage and the *contributor credit* all key off "file exists and does not
   contain the banner" (`tools/chaos_db_ci.py`), and the banner is only looked for in the
   first 200 bytes. Deleting a comment line is a +1 on the progress bar with no rebuild, and
   it looks like tidying. `git diff origin/main -- 'src/*' | grep -n '^-.*NONMATCHING'`
2. **Rebuild with the project's *canonical* compiler.** sm64ds is `2004/b56`, not the
   `1.2/sp2p3` in older notes and branches. 54 of 70 apparent failures under the old pin were
   version divergence, not defects. If you lack the canonical build, run `--all`; if that
   still fails, **report "not reproduced locally," never "does not match."**
3. **Relocation slots are wildcards — but `match.py` now checks destinations by default.**
   A wrong callee with the right shape used to read as a clean match (16 real bugs hid there);
   `--strict-relocs` is now on by default and closes it. **Two ways to still be fooled:** it
   prints `(reloc-destination check unavailable: …; byte-only compare)` and continues when
   reloc config is missing — read for that line — and `fdiff` / `build_pin.verify` drafts
   remain blind by construction, so for those the link-check per-file table is the artifact,
   not the green tick. Detail: `fakematch.md` §3, "Beyond the match gate".
4. **Say what you actually ran.** "verified" means *you* built it.

Then, before Pass A, sweep the diff for the fakematch categories — **whole-function
`asm`/`dcd` transcriptions with no banner** (the vacuous match: the file *is* the ROM bytes,
so it matches by definition and the byte gate cannot fail — `references/fakematch.md` §3,
"The vacuous match"), hand-spelled mangled names in `.c` files, wrong `extern` return types,
laundering idioms, uncommented steering pragmas, register-named locals (`sp0`, `r4`,
`var_r8`). Classify each as **convention** (tree-wide and
deliberate — do not bill this PR for it), **debt** (real, ask for a `TODO:`), or **defect**
(new in this diff — block). Ask only for the fixes the safety ladder says you can have —
renaming a local is unconditional; adding a comment carries the banner-window footnote.

## Pass A — mechanical (run these first, always)

These are ~40% of real maintainer comments (19 of 46 in the source corpus) and cost nothing. Run them before reading anything.
Report each hit; do not editorialize.

1. **Formatting drift — only where the repo ships a `.clang-format`.** nsmb does; **sm64ds
   deliberately does not, and reformatting there is not safe.** Whitespace is byte-safe for
   mwccarm but the tree's *textual* gates parse source: an Allman reformat takes
   `check_header_offsets` from 5 struct matches to 0 **and it then prints a pass**. So the
   familiar "formatting cannot change codegen, always safe to demand" is true of the compiler
   and false of the gates. Where a config does exist, check `ColumnLimit` especially — a
   contributor on `BasedOnStyle: Linux` gets 80 where the repo may want 1000 —
   `clang-format --dry-run -Werror <changed files>`. Where none exists, do not propose one.

2. **Magic addresses.** Grep changed files for hardware-map literals used inline:
   ```sh
   grep -nE '\b0x0?[4-7][0-9a-fA-F]{6}\b' <changed files>
   grep -n 'reinterpret_cast<volatile' <changed files>
   ```
   **Two ways the obvious pattern is wrong, both measured on this tree (`src/`-scoped):**
   - **Make the leading zero optional.** sm64ds writes `0x4000208` far more often than
     `0x04000208`. The 8-digit-only form finds 45 files where the correct one finds 381
     (2,173 refs) — it misses ~88%.
   - **Cover the whole map, not just I/O.** `[45]` is I/O + palette; **VRAM is `0x06` and OAM
     is `0x07`**. Widening to `[4-7]` adds 88 more files (`0x6400000` ×54, `0x6600000` ×53,
     `0x7000000` ×16) — 408 total.

   Eyeball each hit rather than reporting the count: power-of-two literals are ambiguous, and
   bare `0x4000000` has 164 refs that are mostly `REG_DISPCNT` but not all.

   Each should be a *named* register/macro in the project's shared hardware header, defined
   once. Name the register if you can (`0x04001000` = `REG_DISPCNT_SUB`) — but see B3 on
   provenance, and "Port impact" on why a new wrapper macro is not free here.

3. **Generated or stray files committed.** Anything the build emits (`objdiff.yml`,
   `*.o`, `build/`, delink output) must not be in the diff; propose `.gitignore` instead.
   Flag tooling in a language the project is migrating away from.

4. **Hand-edited generated config.** Changes to `config/**/delinks.txt`, `symbols.txt`,
   `relocs.txt` deserve scrutiny: is there a *source-level* mechanism that achieves this?
   (`#pragma section itcm begin/end` in the `.cpp` rather than moving a file in `delinks.txt`.)
   Check the schema too — `symbols.txt` is an **address** table; constants do not belong in it.

5. **`extern "C"` in a C++ tree.** Every occurrence needs a stated reason. Mangling is
   *evidence* about the original declaration; suppressing it can hide that a function was a
   member or was overloaded.

6. **Dead code.** Commented-out blocks, unreachable code, unused locals left from the
   matching process. Distinguish from *live calls with discarded results* — those were kept to
   reproduce bytes and need a `(void)` cast plus a comment, not deletion.

7. **Redundant casts.** A `static_cast` whose source type already satisfies the target is a
   *claim* that the types disagree — usually the declaration upstream is wrong.

8. **Deleted comments in matched files — treat as a defect until proven otherwise.**
   `git diff origin/main -- '*.c' '*.cpp' '*.h' '*.hpp' | grep -E '^-\s*(//|/\*|\*)'`
   A decomp tree accumulates comments that are **load-bearing**, not explanatory: they record
   compiler behavior discovered at real cost and unrecoverable from the code. Real example:
   ```cpp
   /*  Can either be in order z, y, x or x, z, y
       But NOT in order x, y, z  (messes up LDR/STR order in Model functions)  */
   ```
   An assistant tidying that file sees an unnatural store order plus a comment explaining it,
   judges `x, y, z` cleaner, and deletes both — behaving exactly as instructed, while destroying
   information that can only be regenerated by re-burning the hours. **Never let a matching pass
   rewrite a comment it did not author.**

9. **Signature changes in shared headers.** Diff headers *alone* first — this is where scope
   creep hides and where tree-wide regressions originate:
   ```sh
   gh api repos/<r>/pulls/<n>/files --paginate \
     --jq '.[] | select(.filename|test("\\.hp?p?$")) | .filename, .patch'
   ```
   Parameter narrowing (`i32`→`s8`, adding a `u32` arg, pointer→reference) is not byte-neutral
   for existing matched callers even when it is the *correct* recovered signature. Demand the
   affected-caller list. Newly-activated implicit conversion operators are equally ripply.

10. **Config "renames" that are body replacements.** Pair added/removed `symbols.txt` lines by
    address: same address + new name = rename (report as such); address gone = real loss. If the
    old body cannot fit the declared `size=`, it is a *replacement* wearing a rename's clothes.

11. **Register-named locals.** `sp0`, `sp4`, `r4`, `var_r8`, `temp_r1` are stack-slot offsets
    and register numbers — decompiler output, not source, and the most visible fakematch tell
    in the tree (`references/fakematch.md` claim 3c).
    `grep -nE '\b(sp[0-9]+|r[0-9]|r1[0-2]|var_r[0-9]|temp_r[0-9])\b *[;=,]' <changed files>`
    Renaming a local is the top row of the safety ladder — codegen-free *and* gate-invisible.
    Ask for it every time; propose the names.

12. **Port reference integrity — blocking, and the maintainer's own written rule.** If the diff
    contains *any* rename, file move, or `.c`→`.cpp` migration
    (`git diff --name-status origin/main | grep -E '^[RD]'`), run:
    ```sh
    python tools/port_refcheck.py
    ```
    sm64ds's in-tree `port/` references `src/` by literal path and symbol name — the
    `slice_gate*.txt` manifests, the hostgen symbol lists in `port/CMakeLists.txt`, and the
    `/alternatename` linkage bridges in `port/hal/*.cpp`. **None of it is compiled by the decomp
    toolchain, so `validate` is structurally blind to it** and a rename strands the reference
    silently. `MERGE.md:42` and `AGENTS.md:123` both mandate this check; it is also in
    `tools/hooks/pre-push`. No compiler, no ROM, ~1 second.

    This is the one portability check with **zero tension against the byte match** — the fix is
    always on the `port/` side, so it can never ask anyone to touch matched source.

## Pass B — judgment (read the code)

### B1. Under-committed type recovery — the highest-value category

A cast in recovered source is a hypothesis you owe the reviewer an answer on. For each cast in
the diff, decide and *say* which it is:

- **Load-bearing** — the original really did cast. Fine, move on.
- **Symptom** — the cast only exists to make bytes line up, meaning a declaration upstream is
  wrong. Fix the declaration.

Tells:
- **`>> 12` / `<< 12`** in a fixed-point codebase (NSMB, sm64ds use 20.12) — almost always a
  typed fixed-point value and a conversion in the original, not a shift.
- **The same cast at every call site** (`f((u16)a)` everywhere) — the parameter was that type.
  Widen it, delete the casts, re-verify. If still byte-identical you have recovered the real
  signature; if not, you learned the parameter is genuinely wider.
- **`(u32)(x << 16) >> 16`** and friends — laundering idioms, not source. Acceptable only when
  nothing typed reproduces the bytes, and then they deserve a comment saying so.
- **Signedness** — a comparison or shift that only works for one signedness is evidence.
- **A raw int where an enum/flag constant exists.**

### B2. Un-recovered structure

- Numbered offsets (`_18`, `_778`, `+0x840`) used repeatedly on the same base → propose a
  struct.
- Sequential same-stride accesses → array, and often a `for` loop the matcher unrolled or
  never rolled.
- Repeated `data_xxx[0]`, `data_xxx[1]` with parallel logic → array or struct field.

**A `TODO:` is an acceptable answer.** You are not required to solve every struct; you are
required not to bake offset soup silently into the tree. Converting an objection into a
tracked item is cheap and maintainers accept it.

### B3. Naming and provenance

- Functions still named `func_ov052_02153d5c` whose behavior is now obvious from the recovered
  body — propose a name.
- Consistency with existing naming in the tree; do not invent a parallel convention.
- **⚠️ Clean-room provenance.** Many decomp projects (nsmb explicitly, in `contributing.md`)
  reject any PR suspected of relying on the original SDK. An LLM asked to name `0x04001000`
  will emit `REG_DISPCNT_SUB` **from memorized SDK headers**. The name is right; the
  provenance is a rejection risk. Names must be justifiable from the ROM or public hardware
  documentation. **Flag any identifier that looks lifted from vendor SDK headers**, and in
  AI-assisted PRs state where names came from.

### B4. Idiom for the target language

Cosmetic-to-the-compiler house style. Distinguish it sharply from B5, which is not cosmetic.

- Implicit member access — in a 3000-line recovered file, bare `foo()` gives the reader no way
  to distinguish member from global from static; prefer `this->foo()`. Byte-free.
- C casts and `struct` tags surviving into a C++ tree.

### B5. C++ structure recovery — the primary deliverable

**This is goal 1's second half, and the byte gate is blind to all of it.** The mangled name in
the ROM is a statement by the original compiler about the original declaration. Recovering it is
the work; reproducing the linker symbol without it is not.

Ask, in this order — each is answerable from the ROM, and none is a matter of taste:

1. **Does the symbol decode to a member?** `python tools/demangle.py <symbol>`. If it does, the
   real signature — class, parameter types, cv-qualifiers, and for `D1`/`C1` the ARM ABI's
   `this` return — is *stated*, not guessed. Any hand-written declaration that disagrees with
   the demangled truth is wrong, and disagreeing with 26 other files that also guessed is the
   normal case — see `fakematch.md` Claim 2/4 for how common.
2. **Is the class modelled, or is this a free function wearing a member's name?** A `//cpp`
   marker with the mangled name still spelled as a flat identifier is halfway. The target shape
   is a real class on the real inheritance chain.
3. **Does the vtable agree?** Dumping a class's vtable and diffing it against its base's names
   the overrides, marks pure virtuals, and shows which "own" methods actually belong to the
   base — evidence no amount of reading the body will produce.
4. **Free functions on an object that should be members.** Note this is byte-neutral but *not*
   config-neutral — non-virtual member conversion changes the mangled name, so it batches with
   the `symbols.txt` edit (see "Do NOT recommend these").

**A `TODO:` naming the class is an acceptable answer** — same escape hatch as B2. What is not
acceptable is silence, because nothing downstream will ever ask again.

## Pass C — PR shape (often the real blocker)

Judge the change as an artifact to be reviewed, not just as code. In the reviewed corpus, a
3-function PR merged in 16 minutes with one word of feedback; a 131-match / 187-file /
42-commit PR drew 46 comments and sat unmerged for 10 days. Same quality of matching.

Flag, with the numbers:

- **Size.** Roughly: >20 files or >2 concerns in one PR is a review-latency problem. Propose
  the split explicitly, by file group.
- **Bundled tree reorganization.** A rename/move mixed with matching work makes the progress
  bot report phantom "broken matches" (`-620` at the old path, `+620` at the new) and **hides
  real regressions among the fakes**. Reorg is its own byte-neutral PR. This is the single most
  damaging shape problem — call it out loudly.
- **Regressions outside the stated scope.** Read the progress-bot report (`decomp-dev` or
  equivalent) and pull out any item the PR does not claim to touch. Shared-header edits ripple.
  These are usually the only *real* findings in a large report.
- **Mergeability.** `mergeable_state == dirty`, stale base, unresolved conflicts.
- **Readability changes to already-matched code** — welcome, but the PR must *assert* the bytes
  are unchanged. Do not make the maintainer ask.
- **Undisclosed AI assistance.** Where the project asks for it, or where a name-provenance
  question (B3) is live.

## Port impact — report as inventory, never as edits

sm64ds has a second consumer: `port/` in-tree, and the peer repo `sm64ds-port`, which compile
matched sources for a 32-bit MSVC host. That makes a second compiler available over the same
source. **Read the rule before the evidence — the evidence is persuasive and the rule is what
keeps it from doing damage.**

**The rule: portability findings are inventory deltas, not edit suggestions.** They go in one
`## Port impact` output section, phrased as ledger movements — "+1 asm TU on the HAL
reimplementation backlog", "new MMIO write shape hostgen won't recognize", "port_refcheck: 2
stranded references". The *only* path from a portability observation to a source change is the
B1 verify-cycle question: *"does the typed spelling still byte-match? `build_pin.verify` it"* —
never "change this."

That is not a compromise, it is how the tree already works: the port absorbs hostile-but-load-
bearing constructs downstream (the hostgen text transform, HAL shims, `port/unmatched/`) and
keeps the shim surface deliberately enumerable.

**Severity maps onto the existing taxonomy. Do not invent a fourth verdict:**

| Finding | Class |
|---|---|
| `port_refcheck` failure | **defect — block** (fix is port-side, byte-neutral by construction) |
| Hostile construct where a portable spelling was *verified* byte-equal | **should fix** — this is B1 with a second motive |
| Hostile but load-bearing (launder, steering pragma, asm hatch, MMIO deref) | **debt at most, usually convention** |
| **Wrong `extern` on a mangled symbol** (the host build's biggest bucket) | **B1 question, never an edit demand** — see below |

⚠️ **That last row is the one this section will tempt you to get wrong**, because the evidence
for it is the strongest. Attach the demangle output and ask — *"demangle says `X`; does
declaring it as `X` still match?"* — because the naive fix has two traps: parameter changes are
**not** byte-neutral for existing matched callers (A9), and in a `//cpp` file a bare `extern` on
a mangled name **double-mangles**, which `build_pin`'s loose verify cannot see at all — only
`eligible.py` / `check_references.py` catch it.

**Why the evidence is strong anyway:** the host build reads declarations `mwccarm` never
checks, and disagrees with itself out loud — `signature-mismatch` is its largest non-trivial
bucket. `fakematch.md` Claim 2/4 carries the numbers, the grep, and the restamp date; do not
re-copy them here.

**Anti-checklist — do NOT flag these as portability findings.** Each is a false positive against
written project policy, and filing them restarts exactly the forbidden-cleanup cycle below:

- **Endianness / type punning.** DS ARM9 and the x86 host are both little-endian. Non-issue.
- **`int`-sized-pointer assumptions.** The host target is deliberately **32-bit first** —
  "the recovered ABI assumes 4-byte pointers; x64 comes after struct recovery"
  (`port/README.md`). Flagging these argues with policy.
- **`#pragma section itcm/dtcm`** and the `opt_*` steering pragmas — inert under MSVC.
- **Hand-spelled `_Z` externs** — bridged by `/alternatename` in `port/hal/`. Already Claim 1
  debt on honesty grounds; don't bill it twice.
- **MMIO derefs and `HAND-ASM PRIMITIVE` bodies that the original genuinely contained.** Here
  hostility to the host *is* fidelity. Note the ledger movement (every new asm primitive is +1
  on a ~110-file hand-reimplementation backlog) and move on.

The one greppable hazard hostgen cannot absorb, because it transforms text and leaves computed
addresses alone — a **write** through a computed `0x04…` address. Reads still work through the
mapped latch window; write-triggered side effects silently do not fire:

```sh
grep -nE '\*\s*\(\s*(volatile\s+)?(u8|u16|u32|unsigned)[a-z ]*\*\s*\)\s*\(\s*0x0?4' <changed files>
```

Rare, real when it hits, non-blocking.

**Heavy check, pre-submit and >20-file PRs only:** `python port/tools/host_frontier.py` sweeps
all of `src/` with `cl /Zs` and buckets first errors; the contract is that the number only goes
up. Useful form for a review is the changed files' status before vs after. Two documented false
positives: the ~97 HAL-owned asm TUs are *expected* failures the tool classifies out, and MSVC
is commonly installed but off `PATH` — find it via `vswhere.exe`. **If you cannot run it, report
"not run"** — same discipline as Trap A.

## Do NOT recommend these — they look like cleanups and break matches

Every one of these is a natural review instinct that is wrong in a decomp tree. Check against
this list before writing a suggestion.

- **Removing `static` from callbacks.** Often `static` is load-bearing: a non-static member
  changes the ABI (`this` in r0) and breaks the match. If a comment says "static temporarily to
  match code," believe it. Fix the callback *field's* type instead.
- **Blanket-removing `extern "C"`.** For SDK-region C symbols it is load-bearing — it matches the
  unmangled names in `config/**/symbols.txt`, and removing it breaks the link. "Justify each" is
  right; "remove all" is wrong.
- **Converting free functions to members / adding structs, in isolation.** Byte-neutral but *not
  config-neutral*: non-virtual member conversion changes the mangled name, so it must be batched
  with the corresponding `symbols.txt` edit or the link gate fails.
- **Accepting type-recovery suggestions as edits.** Signedness changes instruction selection; a
  float changes codegen wholesale. These are questions requiring a rebuild-and-verify cycle, not
  patches. Say "try this and check" — never "change this."
- **Flattening register helpers into call-site macro pokes.** Inline hardware helpers may model
  SDK functions the ROM inlined; flattening them discards a recovered function boundary. Keep the
  helper, use the `REG_*` macro *inside* it.
- **Reformatting inside a rename commit.** In a tree with a `.clang-format`, byte-safe but
  blame-hostile — the objection is "separate format-only commit," not "don't format."
  **In sm64ds the objection is "don't"**: reformatting silently disables textual gates (A1).
- **Portability cleanups on matched source.** MMIO derefs, ITCM pragmas, steering pragmas,
  `_Z` externs and asm primitives are absorbed downstream by the port, by design. Report them
  as ledger deltas; never as edits. See "Port impact" above for the mechanism and the
  anti-checklist.

## Gates that already exist — read their output, don't propose them

Several checks earlier versions of this skill listed as "worth building" are built. Proposing
them again reads as not having looked:

- **Unbannered-asm gate** → `tools/asm_policy.py`, wired into `validate_merge.py:29`. It is
  *better* than the version this skill used to propose: two tiers, not one. `transcribed`
  (dcd words + no banner) hard-fails; `unbannered-asm` (asm mnemonics, no dcd) warns only —
  which is what stops it wrongly rescinding legitimate CP15-hatch matches.
- **Header-ripple compile** → `tools/affected_src.py` builds the reverse-include graph inside
  `validate` (`MERGE.md:30-36`).
- **Reloc-destination check** → default-on in `match.py` (Pass 0 rule 3).
- **Port reference integrity** → `tools/port_refcheck.py` (Pass A12).

Genuinely unbuilt, and still worth proposing:

1. **Per-symbol byte-neutrality gate** — build base and head, dump per-symbol match % (objdiff
   report JSON), diff, fail on any symbol that moved and is *not* in the PR's claimed list. This
   single check catches shared-header ripple, deleted load-bearing comments, and smuggled
   signature changes at once. Highest value by a wide margin.
2. **Rename classifier** — pair added/removed `symbols.txt` lines by `addr:`; report same-address
   pairs as renames so the bot's "broken matches" table becomes trustworthy.
3. **Laundering-idiom grep** — `<< 16) >> 16`, `BOOL(...) [!=]= (TRUE|FALSE)`, function-pointer
   `reinterpret_cast`, `(void *)…Callback` — each hit needs a comment or an upstream type fix.
4. **Declaration-agreement gate** — the highest-value unbuilt item, and the one the byte gate
   can never do. For each symbol declared `extern` in more than one file, compare the
   declarations against the *demangled* truth (`python tools/demangle.py`), which already
   states the real signature. A host-compiler sweep approximates this today; a demangle-based
   check would be exact and need no compiler. Scale: `fakematch.md` Claim 2/4.
5. **Offset-name consistency** — auto-generate an offset guard from each `unkNNN`/`unusedNNN`
   field name; a name that lies about its offset fails the build.

## Output

Order by what blocks the merge, not by file. Suggested shape:

```markdown
## Verification        (what you rebuilt, with which compiler, and what you could not check)
## Blocking
## Should fix
## Worth checking  (the "is this a cast?" / "is this a struct?" questions — phrase as questions)
## Port impact     (ledger deltas only — see the rules above; omit the section if nothing moved)
## Mechanical  (collapse: "8 magic-address hits, all `0x04000xxx` → REG_* in the shared header")
```

Lead with Verification even when everything passed — it is the section that tells the
maintainer how much your other sections are worth.

Rules:
- **Collapse repeats.** Eight identical magic-address hits are one finding with eight
  locations, not eight findings.
- **Give the fix**, not just the objection. Name the register, name the struct, name the flag.
- **Phrase type/struct questions as questions.** You are usually less certain than the code
  makes you sound, and the contributor has the ROM open.
- **Never assert a byte-match is broken without evidence** from a real build with the
  *canonical* compiler. "Not reproduced locally" and "does not match" are different findings;
  filing the second when you mean the first is the most damaging error you can make here.
  If you did not build it, say the check was not run.
- **Keep the vocabulary honest**: matched / verified / decompiled / near-miss (div=N) / not
  reproduced locally / laundered. `references/fakematch.md` §5 has the table.
- **Separate convention from debt from defect.** Charging one PR for a tree-wide convention
  reads as noise and gets the whole review discounted.
- If the harness is doing a structured review, `ReportFindings` is appropriate; otherwise
  markdown.

## Pre-submit mode

If preparing rather than reviewing: rebuild every claim (Pass 0) — especially any file whose
`NONMATCHING` banner you removed — then run Pass A and fix everything it finds, add `TODO:`
for every B2 item you are not solving, state byte-neutrality for any readability change, and
check Pass C shape before opening. Pass A is ~40% of the comments a maintainer would otherwise
write; Pass 0 is 100% of the ones that would cost the project its credibility.

Two extra steps a reviewer can skip and a preparer cannot: `python tools/port_refcheck.py` if
you renamed or moved anything, and the host-frontier sweep ("Port impact") on a large PR.
Letting the host build disagree with your declarations in private is cheaper than a reviewer
finding it.
