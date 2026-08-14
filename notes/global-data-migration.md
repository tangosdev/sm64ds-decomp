# Global data migration — what it would take to source-build `.data` / `.rodata` / `.bss`

**Status: design note. Nothing here is implemented, and no gate was changed to write it.**

The decomp reproduces **functions**. It does not reproduce a single byte of global
data from source — every ROM data symbol reaches `src/` as an `extern`, and the
original bytes are supplied by dsd's delinked gap objects. That is the other half of
the ROM, it is ~0% migrated, and this note is about what unblocking it actually costs.

The prior framing of this gap was "an authoring convention we could relax". **It is
not.** But it is also not blocked where it was assumed to be blocked, and the two
corrections point in opposite directions. Read both.

---

## 1. Current state (measured 2026-08-14, commit on `chore/issue-1407-followups`)

| Quantity | Value | How measured |
|---|---|---|
| `extern` lines in `src/` (`.c`/`.cpp`/`.h`) | 43,204 | `grep -rc extern src` |
| — of which, distinct **data** symbol names (not function prototypes) | 8,629 | dedup by name, exclude `extern` decls with a `(` |
| `kind:function` symbols in `config/**/symbols.txt` | 11,394 | grep over all 106 `symbols.txt` |
| `kind:data` symbols | 15,409 | same |
| `kind:bss` symbols | 4,973 | same |
| `.text` file-entry ranges across all `delinks.txt` | 10,869 | parse of all 106 files |
| `.init` file-entry ranges | 305 | same |
| `.data` / `.rodata` / `.bss` / `.ctor` file-entry ranges | **0** | same |
| Source files that define a ROM data symbol | **0** | `tools/check_data_definitions.py` enforces this |

So: 20,382 data+bss symbols against 11,394 functions. The unmigrated half is *larger*
than the migrated one. Any claim that this is a small gap is wrong.

Two things worth stating precisely, because both are easy to get backwards:

- **None of the 8,629 extern data symbols in `src/` are phantoms.** They name symbols that
  genuinely exist in `symbols.txt` at real ROM addresses. This is not the
  [[phantom-references]] backlog and does not overlap it.
- **`tools/check_data_definitions.py` is correct today and must stay.** It exists for
  a real defect (a bare declaration inside `extern "C" {}` is a *definition* in C++,
  which multiply-defines against the gap object, sometimes silently). Nothing below
  proposes weakening it. A data-migration mechanism would need to *teach* it which
  symbols a file is licensed to define — an allowlist derived from that file's
  delink entries — not switch it off.

---

## 2. The correction: **dsd is not the blocker.** It already works.

The assumption going in was that `config/**/delinks.txt` has no schema for data
ranges and `tools/rombuild.py`'s `retarget_text_section` would need a `.data`
equivalent. Both turned out to be false, and this was settled empirically rather than
reasoned about — an isolated copy of `config/arm9` in a scratch directory, real
`extracted/` ROM, dsd 0.11.0, `dsd delink` + `dsd lcf`. **Nothing in `config/` or
`build/` was touched.**

### 2.1 `delinks.txt` already accepts `.data` / `.rodata` / `.bss` entries

Appending these to `config/arm9/delinks.txt` in the isolated copy:

```
src/testbss.c:
    .bss start:0x0209b000 end:0x0209b004

src/testdata.c:
    .data start:0x02086bc0 end:0x02086bc8

src/testrodata.c:
    .rodata start:0x02075230 end:0x02075238
```

`dsd delink` exits 0 and emits three carved objects with exactly the right shape:

```
== testbss.o ==     .bss     SHT_NOBITS   size=0x4   sym data_0209b000 size=0x4 GLOBAL/OBJECT
== testdata.o ==    .data    SHT_PROGBITS size=0x8   + .rela.data (1 reloc)
== testrodata.o ==  .rodata  SHT_PROGBITS size=0x8
```

### 2.2 The gap object splits, and the lcf places the carve-out in address order

Before: a single monolithic fragment per data section — `_dsd_gap@main_40.o(.rodata)`,
`(.ctor)`, `(.data)`, `(.bss)`. After adding the entries, `dsd lcf` regenerates:

```
ARM9_RODATA_START = .;   testrodata.o(.rodata)   _dsd_gap@main_37.o(.rodata)
ARM9_DATA_START   = .;   testdata.o(.data)       _dsd_gap@main_39.o(.data)
ARM9_BSS_START    = .;   testbss.o(.bss)         _dsd_gap@main_40.o(.bss)
```

A **mid-section** carve splits the gap in two, exactly as it already does for `.text`:

```
src/testbssmid.c:
    complete
    .bss start:0x0209b038 end:0x0209b048
```
```
ARM9_BSS_START = .;
    testbss.o(.bss)
    _dsd_gap@main_0.o(.bss)      <- new fragment, before
    testbssmid.o(.bss)
    _dsd_gap@main_41.o(.bss)     <- new fragment, after
```

and `complete` redirects `objects.txt` from `delinks/src/testbssmid.o` to
`src/testbssmid.o` — our compiled object — the same way it does for functions.

### 2.3 One file entry may carry a `.text` range **and** a `.bss` range

This is the decisive one. A single file entry with two section lines:

```
src/testmulti.c:
    complete
    .text start:0x02005098 end:0x020050dc
    .bss  start:0x0209b058 end:0x0209b068
```

produces **two** lcf selectors from **one** object:

```
line  242:  testmulti.o(.text)     (inside the .text run)
line 3330:  testmulti.o(.bss)      (inside the .bss run)
```

So the "one function per file, plus the globals that function owns" shape — the
shape a real recovered TU would have — is already expressible. The delinker has
been able to do this the whole time; nothing in this repo has ever asked it to.

### 2.4 `retarget_text_section` is a red herring for data

`tools/rombuild.py:retarget_text_section` renames a compiled object's `.text` section
header to `.init` in place (safe only because both names are five bytes, so nothing
in the ELF moves). It exists because mwccarm emits *code* into `.text` no matter what
the ROM's layout calls that address range, so dsd's `File.o(.init)` selector never
matched.

**No analogue is needed for data.** Verified against mwccarm 2004/b56:

```c
int data_0209b038[4];                       -> .bss    SHT_NOBITS   size=0x10 align=4
void (*data_02086bc0[2])(void) = {f, f};    -> .data   SHT_PROGBITS size=0x8  align=4 + .rela.data
const int data_02075230[2] = {1, 2};        -> .rodata SHT_PROGBITS size=0x8  align=4
```

mwccarm already uses the ROM's own section names for data. `.ctor` would still need
retargeting (mwcc emits static-init pointers differently), but `.ctor` is 0x5c bytes
in main and is not the interesting part.

---

## 3. Where it is *actually* blocked: this repo's own tooling

Every remaining obstacle is ours.

| # | Component | What blocks it | Change needed |
|---|---|---|---|
| 1 | `tools/enroll.py` | `SYM_RE` matches `kind:function(...,size=0x..)` only; the writer emits exactly one section line per entry (`enroll.py:199-204`) | a second candidate producer for data/bss symbols, and a writer that can emit N section lines per file |
| 2 | `config/**/symbols.txt` | **no data or bss symbol carries a `size=`.** Verified: 0 of 15,409 `kind:data` and 0 of 4,973 `kind:bss` | sizes must be derived (see §4.2) or authored; `enroll.py` needs an `end:` it can trust |
| 3 | `tools/eligible.py` rule 1 | "exactly one content section, named `.text`" (`eligible.py:164-168`) | must become "the set of content sections equals the set of sections this file's delink entry declares" |
| 4 | `tools/eligible.py` rule 3 | rejects any `.data`/`.rodata`/`.bss`/`.init`/`.ctor` content | same fix as rule 1 — it is the same check |
| 5 | `tools/eligible.py` rule 2 | "exactly one defined global FUNC symbol"; `other_defined` explicitly rejects any defined `STT_OBJECT` (`eligible.py:205-206`) | must allow the objects the entry declares, and check each one's `st_size` against its declared range the way it already checks `.text` |
| 6 | `tools/objisolate.py` | reduces an object to its one declared function and **zeroes every other section** | must keep the declared data sections, and must externalise only the undeclared ones |
| 7 | `tools/check_data_definitions.py` | rejects any defined GLOBAL/WEAK `data_*` / `reg_*` symbol | needs a licensed-definitions allowlist keyed off delinks.txt, not a bypass |
| 8 | `tools/rombuild_check.py` | `FUNC_RE` only counts `kind:function` when attributing reconstructed bytes | a data-aware denominator, or the percentages silently misreport |
| 9 | verification | nothing runs `dsd check symbols` | see §4.3 |

Items 3–6 are the substantive ones. Note that 3/4/5 are not five rules but **one
concept**: today `eligible.py` hardcodes "a source object is exactly one function."
The generalisation is "a source object is exactly what its delink entry declares." That
is a real refactor of the whitelist, and the whitelist is the thing standing between
the tree and a silently-shifted module.

### 3.1 A schema addition is *not* required, but a convention is

Nothing new needs to be added to `delinks.txt` — `<file>:` + `complete` + N section
lines already parses. What is missing is the **producer**. Concretely, `enroll.py`
would need to emit, per data symbol it enrolls:

- **source file** — which `src/` file owns the definition (see §5, this is the hard part)
- **section kind** — `.data` / `.rodata` / `.bss`, from the module's delinks header
- **start** — the symbol's `addr:` from `symbols.txt`
- **end** — *not available today* (§4.2)

and `eligible.py` would need the same tuple threaded through `enroll.candidates()`
so it can check the compiled object against it. `candidates()` currently returns
`(module_dir, name, srcpath, addr, size, section)` per *function*; it would have to
become per *file*, carrying a list of ranges.

---

## 4. Is BSS meaningfully easier than `.data` / `.rodata`?

**Yes, substantially — for three reasons, and with one real caveat.**

### 4.1 What actually differs at the object-file level

`.bss` is `SHT_NOBITS`: the section occupies no space in the file, contributes no
bytes to the link, and carries **no relocations**. `.data` is `SHT_PROGBITS` and, in
this ROM, is dense with pointers — `_dsd_gap@main_40.o` carries `.rela.data` of
0x582c bytes, i.e. **1,889 relocations** in main's `.data` alone. A source-defined
`.data` object must reproduce every one of those relocations *and* the literal bytes
between them: the initialiser has to be written such that mwccarm emits an
`R_ARM_ABS32` against the right symbol at the right offset. That is a byte-matching
problem of the same character as matching a function, without the disassembly to
guide it.

`.bss` has no such problem. There is nothing to match but an **address and a size**.

Three concrete asymmetries:

- **`.rodata`/`.data` fragment; `.bss` fragments too, but predictably.** mwccarm emits
  **one section per variable**. `int a; short b; int c[3];` produces *three* `.bss`
  sections (sizes 0x2/0x4/0xc), ordered by alignment, not by source order. A
  `const char *p = "hi";` produces *two* `.data` sections (the pointer, and the string
  literal at align 1) plus `.rela.data`. Since dsd's selector is `File.o(.bss)` — which
  matches **all** sections of that name — this is exactly the multi-`.text` destructor
  trap that forced `tools/objisolate.py` into existence. The saving grace: for a file
  defining **exactly one** bss variable, there is exactly one `.bss` section, correctly
  sized and 4-aligned, and it places unambiguously. That is why the pilot in §5 is
  scoped to one-symbol files.
- **A mixed TU works.** `int g[3]; int Foo(int x){...}` compiles to one object with one
  `.bss` (0xc) and one `.text` (0x14) — precisely what §2.3's two-range file entry
  places.
- **No initialiser to get wrong.** Zero-fill is zero-fill.

### 4.2 The caveat: there are no sizes, and the derived size must be *exact*

`symbols.txt` gives data and bss symbols an address and nothing else. The only
available size is the delta to the next symbol. Measured over all 4,973 bss symbols:

```
bss symbols inside a declared .bss section : 4973  (all of them)
  address NOT 4-byte aligned               :   65   <- unplaceable, like the thumb stubs
  implied size 0 (duplicate address/alias)  :   44
  implied size not a multiple of 4          :  114
  bytes covered by symbol deltas            : 0x23814
  bytes declared as .bss across all modules : 0x23820   (12 bytes unaccounted)
```

The symbol list is 99.99% complete, so the deltas *are* an exact partition of BSS.
But the delta is the **stride**, not necessarily the object's true size — a `short`
followed by a symbol 4 bytes later implies 4 where the variable is 2 plus padding. And
because `.bss` is placed by contribution size, declaring `short x;` there emits a
2-byte section and **shifts every later bss symbol by 2**. The delink range must be
filled exactly, which sometimes means declaring a padded type that is *less*
historically honest than the `extern` it replaces. That is a direct goal-1 tension and
should be recorded per-symbol when it happens, not papered over.

### 4.3 The verification story — better than "unverifiable", worse than for functions

`.bss` is not in the module image at all. `extracted/dsd/arm9/arm9.bin` is 618,496
bytes = 0x97000 = exactly `.text` start (0x02004000) → `.bss` start (0x0209b000). So
`dsd check modules` and `tools/rombuild_check.py`'s byte diff **cannot see BSS
content**, because there is none to see.

They can, however, see it *indirectly*: every reference to a bss symbol from `.text`
or `.data` is a relocation resolved at link time, and those relocated words **are** in
the compared image. If a source-defined bss object is the wrong size, later symbols
shift and every gap-object reference to them relocates differently — a byte diff.
For the pilot set in §5 (each symbol referenced from at least one file) that means
the existing gate does provide coverage.

Two holes remain, and both should be closed before shipping anything:

- **177 bss symbols are referenced from no `src/` file at all.** Misplacing one is
  invisible to every byte gate.
- **`dsd check symbols` exists and is not wired in.** `dsd check symbols -c <config>
  -e <elf> --fail` compares linked symbol addresses against `symbols.txt` and is the
  direct check for this. `tools/rombuild.py` runs `delink`, `lcf`, `rom config`,
  `rom build` — it never runs `check symbols`. Adding it is cheap and is arguably worth
  doing on its own merits, independent of any of this.

**Not verified here:** no full `rombuild.py` link was run for this note. The indirect
coverage argument above is reasoned from the lcf, the gap-object relocation tables and
the module image size, not observed. Anyone implementing this should confirm it by
deliberately mis-sizing one bss entry and checking that the build goes red.

---

## 5. Is there a small, low-risk first slice? — yes, and it is much smaller than it looks

Funnel over all 4,973 bss symbols (script: throwaway, run read-only, not committed):

```
4973  all bss symbols
2824  referenced by exactly one src/ file
2824  ...and that sole reference is a .c/.cpp (not a src/ header)
2750  ...and not also named in include/
2715  ...and the address is 4-byte aligned
2666  ...and the implied size is nonzero and a multiple of 4
2652  ...and the sole-referencing file is already `complete` in delinks.txt
```

**2,652 looks like the answer. It is not.** Splitting that set by *what kind of file*
the sole reference is:

```
2421  sole reference is a __sinit_*.c file  (253 files)
 231  sole reference is an ordinary function file  (124 files)
```

A `__sinit_ov002_02100560.c` names dozens of bss symbols because it *constructs* them
— it is the compiler-synthesised static-initialiser TU, not the TU that **defined**
them. "Referenced by exactly one file" is a proxy for ownership and for 91% of the set
it is the *wrong* proxy. Those 2,421 must be excluded.

Of the remaining 231 across 124 files, 52 files would gain more than one bss symbol —
and per §4.1 a multi-variable file emits multiple `.bss` sections that dsd's selector
cannot disambiguate, so those need `objisolate` work first.

### The honest pilot number

> **72 bss symbols** — each referenced from exactly one ordinary `src/` `.c`/`.cpp`
> file, that file already `complete`, address 4-aligned, implied size a nonzero
> multiple of 4, and that file gaining **exactly one** bss symbol so the object has
> exactly one `.bss` section.

Sizes are small and tractable (0x4 / 0xc / 0x14 / 0x20, one outlier at 0x600).
Examples:

```
data_ov002_0211114c  0x0211114c  size 0x04  src/_ZN7Minimap8BehaviorEv.cpp
data_ov016_02114d4c  0x02114d4c  size 0x0c  src/_ZN5Unagi13InitResourcesEv.cpp
data_ov026_02113f4c  0x02113f4c  size 0x0c  src/_ZN12WaterSuction13InitResourcesEv.cpp
data_ov006_02140518  0x02140518  size 0x20  src/func_ov006_020c8a30.c
data_ov007_02103f98  0x02103f98  size 0x600 src/func_ov007_020b8548.c
```

Every one is named `data_<module>_<addr>`; **zero** carry a recovered name. So the
pilot proves the *mechanism* and buys no readability — a separate naming pass would
be needed for that to be worth anything to a reader.

### The ownership problem the count cannot solve

`src/` is one function per file. The ROM's `.bss` was laid out per **translation
unit**, and a TU is many functions. Putting `data_ov016_02114d4c`'s definition in
`_ZN5Unagi13InitResourcesEv.cpp` because that is the only file mentioning it is a
*guess* about ownership dressed up as a fact — and it is a guess the byte gate cannot
falsify, because any file in the module can host the range and still link identically.

This is the goal-1 problem at the centre of the whole idea. `build/tu_map.json` and
the `decomp-tu-slicing` skill exist precisely to answer "which functions shared a
TU", and any serious data migration should be driven off TU reconstruction, not off
"who mentions the symbol". A pilot that ignores this is proving plumbing, not
recovering source — which is fine as long as it is labelled that way and the 72
entries are not left in the tree as if they were recovered structure.

---

## 6. Effort and risk

**This is a multi-month structural project, not a quick win.** Sized honestly:

| Phase | Scope | Effort | Risk |
|---|---|---|---|
| A | Wire `dsd check symbols` into `rombuild.py`; close the "no gate sees bss placement" hole | ~1 session | low — pure addition, useful regardless |
| B | Generalise `eligible.py` rules 1/2/3 from "one function" to "what the entry declares", plus matching `objisolate.py` changes, plus a licensed-definitions list for `check_data_definitions.py` | 2–4 sessions | **high** — this whitelist is the only thing preventing a silently shifted module; a wrong relaxation is not loud |
| C | `enroll.py` data-candidate producer + multi-range entry writer + size derivation | 2–3 sessions | medium — mechanical, but the size derivation is a judgement call per symbol |
| D | The 72-symbol BSS pilot, end to end, with a deliberately mis-sized negative control | 1–2 sessions | medium |
| E | Scale BSS beyond the pilot (multi-variable files) | large | needs `objisolate` per-variable section surgery |
| F | `.data` / `.rodata` at all | very large | needs initialiser recovery + relocation matching; ~1,889 relocs in main's `.data` alone |
| G | Ownership: which TU defined which global | open-ended | this is the goal-1 half, and it is research, not plumbing |

Phases A–D get to "the mechanism works, 72 symbols deep." That is roughly
**0.35% of the 20,382 data+bss symbols** and buys no readability by itself. Phase F is
where the other ~15,000 live and it is a genuinely different kind of work.

### Recommendation

- **Do phase A soon.** `dsd check symbols` is not run today, costs almost nothing, and
  the tree currently has *no* gate that would notice a wrong symbol address that
  happens not to shift bytes. That is worth having whether or not any of the rest
  happens.
- **Do not start B–D as a side quest.** The generalisation in phase B touches the
  whitelist that makes every one of the current 10,8xx enrolled functions safe, and
  its failure mode (a module that shifts and still passes) is exactly the class of bug
  this repo has been bitten by before ([[unbuildable-files-invisible]],
  [[eligible-is-not-enrolled]], [[module-mismatch-blast-radius]]). It deserves its own
  workstream with its own negative controls, not a corner of a data PR.
- **Do not frame any of this as an authoring-convention fix.** "Just let files define
  their globals" would, today, produce objects that multiply-define against the gap
  object, or place duplicate content with no delink range, or shift a module silently.
  `check_data_definitions.py` is right to reject them and must keep doing so until
  phases B and C exist.
- **Fix the ownership question before, not after.** The 72-symbol slice is defensible
  as a plumbing proof. It is *not* defensible as recovered source, and the difference
  should be written into whatever PR carries it.

---

## Appendix — how the dsd findings in §2 were obtained

All experiments ran against a **copy** of `config/arm9` in a scratch directory with
`build_path` / `delinks_path` / `rom_config` repointed; the real `config/` and
`build/` were never written to (`git status` clean for `config/`). Commands:

```
dsd.exe delink -c <scratch>/arm9/config.yaml
dsd.exe lcf    -c <scratch>/arm9/config.yaml
```

dsd version: `ds-decomp-cli 0.11.0` (`tools/bin/dsd.exe`).
Compiler probes: `tools/mwccarm/2004/b56/mwccarm.exe` with `tools/rombuild.py:CFLAGS`.
No `rombuild.py` link was run; §4.3 says which claims are therefore reasoned rather
than observed.
