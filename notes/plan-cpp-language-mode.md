# Plan: closing the C/C++ language-mode gap (issue #821)

**Status:** proposal.
**Provoked by:** [#821 "This is 100% fakematch"](https://github.com/tangosdev/sm64ds-decomp/issues/821).
**Prerequisite reading:** `notes/runbook-type-reconstruction.md` (the ladder, the safety
model, the dead ends). This plan schedules that runbook; it does not replace it.

---

## 1. What the issue claims, and what survives checking

Every number below is reproducible on a clean tree at `7b293af4`. Commands in §9.

| # | Claim | Verdict | Evidence |
|---|---|---|---|
| 1 | Name mangling proves C++, ignored | **Holds** | 1,359 of 2,515 mangled-symbol files (54%) still spell the mangled name by hand — 1,044 in `.c` files, and 315 more in `.cpp` files that renamed the extension and nothing else |
| 2 | Incorrect return types | **Holds** | `int _ZN10FaderColorD1Ev(int *self)` — the ARM C++ dtor ABI returns `this`, so this is `FaderColor* FaderColor::~FaderColor()` |
| 3 | Forced register usage | **Holds, smaller than stated** | 14 files carry inline `asm`; 67 name a codegen-forcing hack in a comment; 1,246 use `long long` (an unknown subset legitimately) |
| 4 | Incorrect argument types | **Holds** | `int *self` for `FaderColor*`; and per the runbook, 27% of local declarations contradict each other |
| 5 | Using a C compiler for C++ | **Holds in substance** | mwccarm does compile both, so "wrong compiler" is imprecise — but 8,125 files build as C and 1,044 of them define a C++ symbol. The mode is wrong where it matters, and the follow-up jab that `//cpp` "changes nothing" is right for 315 files |

What does **not** survive: *"every single function is fake matched."* The byte gate is
real and it is honest. 1,059 files already carry real C++ definitions, 79 of them real
destructors, and enrolled functions reproduce the ROM byte-for-byte. The rhetorical
core still lands, though, and this repo already said so first —
`notes/runbook-type-reconstruction.md` §1: **"The build is correct; the description is
not."** The types are wrong as *documentation*, not as *machinery*.

That distinction is the whole plan. Nothing below changes a single byte of output. It
changes what the source *claims*, and the byte gate is what proves each step didn't lie.

## 2. The measured backlog

Run `python tools/langmode_audit.py` for the live numbers; it is the authority and this
section is its output at `7b293af4`. **The backlog is 1,359 files — 54% of all
mangled-symbol files — not the 1,044 that a file-extension count suggests.**

The extension is not the test. A migration means *the compiler mangles the name for you*;
a file that renamed itself `.cpp` and still writes `extern "C" void
_ZN5ActorC1Ev(Actor *self) {` has changed nothing that #821 complained about. Counted
honestly:

| population | count |
|---|---|
| `.c` extension, mangled symbol | 1,044 |
| `.cpp` extension, symbol still hand-spelled | 315 |
| **not migrated, total** | **1,359** of 2,515 (54.0%) |

By symbol kind, with what is actually *proven* — a genuinely migrated file that is not a
`// NONMATCHING` draft:

| kind | unmigrated | proven | status |
|---|---|---|---|
| plain methods | 842 | 1,079 | **proven at scale** |
| `D1` complete dtor | 188 | 72 | **proven** |
| `D0` deleting dtor | 258 | 3 | **barely proven** — treat as near-research |
| `D2` base dtor | 17 | 0 | **UNPROVEN** |
| `C1` complete ctor | 41 | 0 | **UNPROVEN** |
| `C2` base ctor | 11 | 0 | **UNPROVEN** |
| `C3` | 2 | 0 | **UNPROVEN** |

**57 files are excluded, not backlog:** they take a class by value, the runbook §7 dead
end (mwccarm homes `r0-r3` to the stack, +0x14, on all 25 sweep versions at every
optimization level). `--list excluded` names them.

"Unproven" is literal and it survived scrutiny. Every `.cpp` file for every constructor
variant still hand-spells its symbol; `src/_ZN9ActorBaseC1Ev.cpp` is hand-written `asm`
and marked `// NONMATCHING`, so it is a draft, not a match. **No constructor has ever
been migrated in this tree.** The same is true of `D2`. And `D0` — 258 outstanding
against 3 proven — is far weaker than its volume suggests, because the deleting
destructor also has to get `operator delete` right.

**A second, quieter backlog.** The proven destructor pattern often bought its bytes with a
new lie. `src/_ZN6CannonD1Ev.cpp` is a genuine `Cannon::~Cannon()`, but it re-declares its
own base locally rather than including the real header:

```cpp
struct Actor { char pad[0xd0]; virtual ~Actor(); };
```

Tree-wide: **2,664** files define a struct or class body locally (1,446 of them `.cpp`),
**1,641** do so while including no project header at all (776 `.cpp`), and **690** use a
`char pad[0x..]` shadow layout. A shadow base is invisible to `tools/affected_src.py`, so
a later header fix silently misses it, and two files can disagree about the same class
forever. This is the
same disease as the declaration debt in `notes/declaration-centralization.md` (**43,931**
`extern` lines across **8,950** files at `7b293af4`, 27% mutually inconsistent,
`ModelAnim::SetAnim` under 123 spellings) wearing a different coat. The runbook quotes
43,922/8,948 for those first two; the tree has since drifted by 9 lines and 2 files, so
re-measure rather than cite — that paragraph has been wrong once before and says so.

**The load-bearing consequence for sequencing:** migrating a destructor the quick way
*manufactures* shadow-declaration debt. That is how 770 files got there. So the unit of
work must be **a class**, not a file.

## 3. Principles

1. **Class-at-a-time vertical slices, not file-at-a-time.** A slice takes one class from
   rung 0/2 to rung 3 *with its real header*: fields named and typed, every method and
   dtor migrated, no local shadow struct left behind. Ten finished classes beat 300
   files converted into shadow-struct debt.
2. **Never trade a big lie for a small one silently.** If a slice must leave a shadow
   decl or a raw mangled call, say so in the file, as the runbook's rung-3 exemplar does.
3. **The byte gate is the only verdict, and `rombuild.py` alone is not it.** Bracket
   every header edit with `tools/eligible.py` before and after on a clean tree — it
   compiles only enrolled files, and a retype can un-match a non-enrolled includer while
   106/106 still passes.
4. **Order by blast radius, not by file count.** Layout-free work first.
5. **Same width unless a width change is intended and stated.** Equal width is necessary,
   not sufficient (`s16`→`u16` flips `ldrsh`/`ldrh`).

## 4. Phases

### Phase 0 — Instrument and gate — **DONE**

`tools/langmode_audit.py` ships with this plan, and `langmode-baseline.json` banks the
starting point. It needs no compiler and no ROM, so it runs on a bare clone. It reports
the populations above, the per-class backlog (`--by-class`), work lists (`--list`), and
the named exclusions.

`--check langmode-baseline.json` is the **CI ratchet**: every metric is a defect count, so
it may fall and never rise; the gate has been tested against a deliberately planted
regression and fails closed. The banked baseline holds only the metrics `--check` reads
(67 lines), so re-banking after a slice stays a readable diff; `--full` dumps the
per-class detail and file lists for humans. The headline metric is `unmigrated_total`, which cannot be
gamed by renaming a `.c` to `.cpp` — a hand-spelled symbol counts however the file is
named. That property is the whole reason this phase came first: it makes the *easy* fake
progress unrewarding, and it turns the reply to #821 into a number instead of an argument.

Remaining, and the natural next commit: wire `--check` into CI, and re-bank the baseline
in any commit that legitimately lowers a count.

### Phase 1 — SDK namespaces *(169 files, 37 namespaces) — zero layout risk*

37 of the mangled-name prefixes have no `include/<Name>.h` **and no ctor/dtor**, which
together mean they are namespaces rather than classes. Both halves matter: a missing
header alone proves nothing, since `Chuckya` has a `D0`/`D1` and therefore a vtable and a
layout whether a header exists or not. `langmode_audit.py` reports this set as
`layout_free`; do not widen it by eye.

`_ZN2GX10EndLoadTexEv` is `GX::EndLoadTex()` — no `this`, no vtable, no struct, no
includers. The migration is a language-mode flip and nothing else:

```cpp
//cpp
// @symbol _ZN2GX10EndLoadTexEv
namespace GX { void EndLoadTex() { /* body unchanged */ } }
```

Largest prefixes: `GX` 18, `CP15` 16, `Sound` 15, `cstd` 11, `IRQ` 9, `G2S` 8, `GXS` 8.

This phase proves the pipeline (audit → migrate → byte-verify → re-bank) at the smallest
possible blast radius, and retires 12% of the backlog without touching one header.
Do it first for that reason, not because it is the biggest.

### Phase 2 — Destructors, class-at-a-time *(463 files: 258 `D0`, 188 `D1`, 17 `D2`)*

Largest single block, real header discipline required. Per §3.1 a slice is a class, and
`D0`/`D1`/`D2` for one class ship together — they share the vtable write and the
base-subobject call, so splitting them across commits means verifying the same layout
hypothesis three times.

**Only `D1` is properly proven** (72 landed). `D0` has 3 and `D2` none, so the first
slices should be chosen to include all three variants and settle `D0`/`D2` early —
`D0` additionally has to get `operator delete` right, which `D1` never exercises. If
`D2` turns out to be blocked, that is a runbook §7 entry and 17 files leave the backlog.

Pilot targets — highest unmigrated count *and* an existing reconstructed header
(`--by-class` for the live table):

| class | unmigrated | kinds |
|---|---|---|
| `Actor` | 65 | `C1:1 C2:1 D0:1 D1:1 D2:1 method:60` |
| `Player` | 48 | `C1:1 C3:1 D0:1 D2:1 method:44` |
| `Stage` | 25 | `C3:1 D0:1 D2:1 method:22` |
| `Scene` | 23 | `D0:1 D1:1 D2:1 method:20` |
| `Heap` | 19 | `C1:1 D0:1 D1:1 D2:1 method:15` |

Start with **`Scene`**. Its header is already mostly named, so the slice tests the
*migration* rather than migration plus field reconstruction at once, and it carries one of
each dtor variant — exactly the `D0`/`D2` evidence Phase 2 is missing — without dragging in
a constructor. `Actor` is the prize (65 files, base of the actor hierarchy) and also the
widest blast radius in the tree: take it third or fourth, once the procedure is boring.
Note that `Actor`, `Player`, `Stage` and `Heap` each carry a ctor variant, which is Phase 5
research — split those files out of the slice rather than letting them block it.

Heed the runbook's warning on the C side: a polymorphic class needs an explicit
`void* vtable; /* 0x00 */` under `#else`, or every C includer's offsets shift by 4.

### Phase 3 — Plain methods *(842, less the 57 excluded)*

Same slices, same classes, after that class's dtors are done. Includes converting raw
mangled sibling calls into real calls where the callee now has a proper declaration —
migration is per-reference, not only per-function, so this phase shrinks the declaration
debt as a side effect.

### Phase 4 — Retire shadow declarations *(1,641 files; 776 of them `.cpp`)*

The phase that decides whether the critique actually stops being true. Point every
shadow struct at the real header and delete the local copy. Byte-neutral in intent and
therefore fully gated: `tools/affected_src.py` per header, then the `eligible.py` bracket.

Sequenced after 2–3 because those phases keep adding to it until the class-slice
discipline is habitual, and because a shadow struct whose class has no reconstructed
header yet has nowhere to point.

### Phase 5 — Constructors *(54 files: 41 `C1`, 11 `C2`, 2 `C3`) — timeboxed research spike*

Zero migrated, ever, so this is research and not throughput. The open question is whether
mwccarm's member-initializer-list and base-ctor codegen can be steered to the ROM's bytes
at all, or whether the ctor variants are a genuine dead end like by-value `Fix12<int>`.

`src/_ZN9ActorBaseC1Ev.cpp` is the warning: someone already reached for hand-written `asm`
here and marked it `// NONMATCHING`. That is a fair summary of the difficulty, and it is
also the natural spike subject, since its inheritance chain and callees are already known.

Spike on one such class, measure across the sweep, and publish the answer **either way** —
a documented dead end in runbook §7 is a real deliverable and stops the next contributor
burning a week. Do not schedule the other 53 files until the spike returns.

### Phase 6 — Codegen hacks *(14 inline `asm`, 67 commented laundering)*

`#define LAUNDER(p) ((volatile u32 *)(int)(((long long)(int)(p))))` has no meaning in the
original C++; it exists to force address materialization. Small, and the most quotable
evidence in the issue, but each one is a bespoke codegen fight with no shared pattern —
so it pays worst per hour. Attack opportunistically when a slice already touches the file,
and treat the standalone 14 as last.

## 5. Explicit non-goals

- **Consolidating one-function files into real TUs.** Measured and declined: byte-safe,
  worth only 14–19% (runbook §4). The repo convention is one function per file, filename
  *is* the symbol (`AGENTS.md`). Do not reopen this without new numbers.
- **Declaring any method with a by-value class parameter** (`5Fix12IiE`). Dead end across
  all 25 compilers and every optimization level. Keep as `extern "C"` with scalar args.
- **Deriving a signature from call sites.** They disagree 27% of the time; the definition
  is the only non-guess.
- **Migrating before types are right.** A real `struct` on guessed member types is a lie
  every later file inherits.

## 6. Definition of done, per slice

Inherits runbook §6, plus:

- [ ] `ROM-build analysis: PASS`, module fidelity 106/106 exact
- [ ] `eligible.py` before/after on a clean tree: enrolled count did not fall
- [ ] attribution 0 changed, 0 lost; no commit both moves and rewrites a file
- [ ] **no file in the slice defines a struct body locally** — all include the real header
- [ ] every retyped field same-width, or the change deliberate and stated
- [ ] each field comment says what the value *means*, with range and encoding
- [ ] `langmode_audit.py` counts fell, and the CI ratchet reflects it

## 7. Risks

| risk | mitigation |
|---|---|
| Header retype silently un-matches a non-enrolled includer | the `eligible.py` bracket; never `rombuild.py` alone |
| Shadow structs make `affected_src.py` under-report | Phase 4; and §6 forbids new ones |
| Phase 2 volume tempts file-at-a-time throughput | slice = class; the ratchet counts shadow files too, so quick wins show up as debt |
| `Actor` blast radius (65 files + hierarchy base) | sequence it after two boring slices |
| Ctor spike open-ends | timeboxed; a documented dead end is an accepted outcome |
| Toolchain absent, so nothing is verifiable | §10; Phase 0 was scoped to need neither compiler nor ROM |

## 8. On answering #821

The strongest reply already in the thread is the collaborator's own audit, which conceded
1, 2, 4, most of 3, and half of 5 with counts attached. Phase 0 turns that one-off audit
into a standing number. Recommend replying with the audit output, this plan, and the
`Fix12<int>` and TU-consolidation exclusions stated as measured constraints rather than
preferences — the reporter's technical points are largely correct, and the repo's position
is stronger when it says so plainly and shows the ratchet.

## 9. Reproducing every number above

Every figure in §1–§4 except the declaration debt is `tools/langmode_audit.py` output:

```sh
python tools/langmode_audit.py                    # the summary quoted in section 2
python tools/langmode_audit.py --by-class         # the pilot-target table in phase 2
python tools/langmode_audit.py --check langmode-baseline.json    # the CI ratchet
```

The declaration debt is the runbook's own pair, **43,931 lines across 8,950 files** at
`7b293af4` (the runbook still says 43,922/8,948; re-measure rather than cite):

```sh
git grep -h -cE "^\s*extern " -- 'src/*' | awk '{s+=$1} END{print s}'
git grep -lE  "^\s*extern " -- 'src/*' | wc -l
```

Do not hand-roll the language-mode counts from file extensions. That is exactly how the
first draft of this plan reported 1,044 instead of 1,359 and called `D0` proven on the
strength of 15 files that turned out to be 3: an extension is not a migration, and a
`// NONMATCHING` draft is not proof of anything.

## 10. Two prerequisites this working copy does not have

Phase 0 was built to need neither, but **every later phase is blocked** until both are
present, because nothing can be byte-verified without them:

| missing | needed for | how to get it |
|---|---|---|
| `tools/mwccarm/2004/b56/` | the only verdict the build honours — `rombuild.py`, `build_pin.verify` | `notes/setup-mwccarm.md` |
| `tools/bin/dsd.exe` | `rombuild.py` delink/link/rom phases | `notes/rom-build.md`, ds-decomp 0.11.0 |

24 of the 25 sweep compilers can be on disk and the build still cannot be trusted: the pin
is `2004/b56` (`rombuild.VERSION`), the linker is `1.2/sp2p3`, and `match.py`'s `CANONICAL`
is `2004/b56` too. A sweep hit under any other version is iteration, never a verdict —
runbook §5 says so explicitly.

`notes/setup-mwccarm.md` predates the `2004/b56` pin (#1000) and still presents `1.2`/`2.0`
as the whole set. It should be corrected by whoever sources that compiler.
