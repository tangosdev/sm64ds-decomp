# Building a playable `.nds` from source

**Goal.** Produce a bootable `build/sm64ds.nds` whose ARM9 code comes from *our* C
wherever `src/` has a verified match, and from the ROM's delinked objects everywhere
else. Then change a line of C, rebuild, and see the change in a running game.

This is the "hybrid link" that ds-decomp is designed for. It does **not** require 100%
matching first — at 97.9% of functions we can already source-build the overwhelming
majority and let delinked gap objects supply the residue. Phase 1 of
[`roadmap.md`](roadmap.md) ("`dsd rom build` turns the source back into the exact retail
ROM") is the 100% version of the same pipeline; this document is the path to standing
that pipeline up now.

## Status

**M0, M1, M2a and M2b are done. The built ROM boots and plays in melonDS.**
`python tools/rombuild.py` produces a 16,777,216-byte `build/sm64ds.nds` in about a
minute, with all 106 modules byte-identical to the ROM. **9,115 functions — 1,475,772
code bytes, 66.7% of the project's total — are compiled from `src/` by mwccarm**; the
rest of each module is supplied from delinked ROM bytes. All four milestones are complete. Results are recorded under each milestone below.

```sh
python tools/eligible.py             # classify: which files may be compiled in
python tools/enroll.py --complete-list build/eligible-names.txt
python tools/rombuild.py             # build and verify the stock profile (default)
python tools/rombuild.py --profile mods  # intentionally include mods/
python tools/rombuild_check.py       # per-function byte diff, names any culprit
python tools/rombuild_diag.py        # explain a mismatch: which symbol does it name?
python tools/rombuild_versions.py build/bad.txt --write   # per-file compiler overrides
```

`rombuild.py` compiles exactly the files enrolled by a `complete` file entry in some
`config/**/delinks.txt`; everything else comes from ROM bytes. Enrolling more source is
therefore a config edit, not a code change. `--no-rom` stops after linking for fast
iteration.

### Stock and mod profiles

The tracked `delinks.txt` files may keep a convenient `mods/<symbol>.c` enrollment, but
that no longer makes an ordinary build modded. `rombuild.py` generates a disposable
config under `build/rombuild-config/`:

- `--profile stock` is the default. Each `mods/<symbol>.c` entry is redirected in the
  generated config to its verified `src/<symbol>.c` counterpart. If no verified source
  counterpart exists, that range falls back to retail gap bytes. The tracked config and
  the useful mod source are not edited.
- `--profile mods` preserves explicit `mods/` entries and writes
  `build/sm64ds-mod.nds`.

Stock validation rejects every executable-module byte difference. Mod validation permits
differences only inside the address ranges explicitly enrolled from `mods/`, and fails if
an enrolled mod did not actually alter its range. Both profiles write structured reports:
`build/rombuild-report.json` for stock and `build/rombuild-mod-report.json` for mods.
The reports keep module fidelity separate from source reconstruction: the former answers
"does the linked code reproduce retail?"; the latter answers "how many functions and
code bytes in this output came from our source?"

**The compiler default is `2004/b56`** (`rombuild.VERSION`), with per-file exceptions in
`config/rombuild-versions.txt`. The **linker** is always 1.2/sp2p3 (`LD_VERSION`) because
the 2004 toolchain ships no `mwldarm`.

## The pipeline

```sh
src/*.c|cpp  --mwccarm-->  build/src/*.o  ─┐
                                           ├─ mwldarm ─> build/final_link.o (ELF)
config/**/delinks.txt --dsd delink--> build/delinks/*.o  ─┘   + build/*.bin per region
        │                                                          │
        └──> dsd lcf ──> build/arm9.lcf + build/objects.txt        │
                                                                   v
                                        dsd rom config --elf build/final_link.o
                                                                   │
                                                                   v
                                        dsd rom build ──> build/sm64ds.nds
                                                                   │
                                        dsd check modules ──> byte-diff vs retail
```

Commands (matching the reference dsd project, `DQIX/dqix-decomp`, adjusted for the
0.11.0 CLI actually on disk):

```sh
dsd lcf        -c config/arm9/config.yaml            # writes arm9.lcf + objects.txt to build_path
mwldarm  -proc arm946e -nostdlib -interworking -m Entry -map closure,unused \
         -msgstyle gcc -nodead @build/objects.txt build/arm9.lcf -o build/final_link.o
dsd rom config --elf build/final_link.o --config config/arm9/config.yaml
dsd rom build  --config <generated rom config> --rom build/sm64ds.nds
dsd check modules --config-path config/arm9/config.yaml --fail
dsd check symbols --config-path config/arm9/config.yaml --elf-path build/final_link.o --fail
```

### The object cache

The compile step is the expensive one and almost none of it is compilation. A
one-function file costs ~100 ms under Wine on the build box, nearly all of it mwccarm
process startup, so a full build spends ~15 CPU-minutes producing 9,116 objects that
are usually identical to last build's. The PR validator paid that twice per job — once
for the base tree, once for the merged tree — to check a handful of edited files.

`tools/rombuild_cache.py` keys each object on its exact inputs and skips the compiler
on a hit:

```sh
source key = sha256(schema, source path, compiler version, compiler bytes,
                    final flags, source bytes)
manifest   = the header list mwccarm reported for that source key last time
object key = sha256(source key, each dependency's repo path + content hash)
```

The compiler's own bytes are in the key, not just its version directory name:
`tools/mwccarm` is licensed material no repository tracks, so nothing else would
notice the box being re-provisioned under the same version string while a cache
that is designed to outlive any single build sits beside it.

A file with no manifest compiles with `-MD`, and the `.d` mwccarm writes names the
headers it actually read. That list is stored, so the next build computes the object
key without preprocessing anything, and editing a header rebuilds exactly the files
that include it. The store lives in `build/objcache` (override with `ROMBUILD_CACHE`),
is pruned back under `--cache-max-mb`, and `--no-cache` restores the old
compile-everything behaviour. Every build reports what it reused under `objectCache`
in the JSON report, so a validator verdict states how much of the ROM it rebuilt
rather than leaving that to be assumed.

Three properties keep a hit as trustworthy as a compile, and are worth preserving:

- **Keys are content, never mtime.** The validator's clone resets between unrelated
  commits with `build/` surviving, so timestamps there carry no ordering meaning; an
  mtime-driven cache would happily reuse objects from a different tree.
- **`-MD` and the scratch working directory do not perturb codegen.** Both were
  checked byte-identical against a plain `cwd=REPO` compile for a C and a C++ source
  before the cache was written. Re-check if the compiler version default moves.
- **An unresolvable dependency makes a compile uncacheable**, not cached with an
  unknown input. Wrong-but-slow is recoverable; fast-but-wrong is a false green.

One caveat direct-mode caches share: a manifest records the headers that *were*
read, never the lookups that failed, so a newly added header shadowing an existing
one would be invisible to the key. That vector is closed here by the compiler's own
behaviour rather than by the cache — mwccarm resolves a quoted include from
`-i include` even with a same-named header sitting beside the source, and there is
only one include root for a PR to shadow. Re-check it if `-cwd` or a second `-i`
ever joins `CFLAGS`.

`-j` now defaults to twice the container's CPU quota rather than `os.cpu_count()`,
which reports host cores the build cannot use. Twice, not once: these compiles park
in Wine process startup, so one thread per whole CPU measurably leaves the quota
unspent — matching the quota exactly measured slower than the old host-core default.

## What already exists

| Piece | State |
|---|---|
| `dsd` (ds-decomp 0.11.0) | `tools/bin/dsd.exe` |
| ROM extracted (`dsd rom extract`) | `extracted/dsd/` — header, banner, 2,072 asset files, arm7, arm9, overlays |
| dsd project config | `config/arm9/config.yaml` + **106 modules** (main + itcm + dtcm + 103 overlays) of `symbols.txt` / `relocs.txt` / `delinks.txt` |
| Delinked gap objects | `build/delinks/*.o` — **93**, because 13 overlays are empty and emit none |
| Linker script + object list | `build/arm9.lcf`, `build/objects.txt` |
| Compiler **and linker** | `tools/mwccarm/1.2/sp2p3/{mwccarm,mwldarm}.exe` |
| Matched source | `src/` — 8,147 `.c` + 3,056 `.cpp`, one function per file |
| Symbol → address map | `config/**/symbols.txt` (`name kind:function(arm,size=0x14) addr:0x...`) |

At the time this plan was written nothing had ever been linked and `build/build/` was
empty. M0 changed that.

Coverage measured for this plan: **11,197 of 11,389** function symbols have a `src/`
file; in the main module, 3,084 of 3,090.

## `delinks.txt` file-entry syntax

Verified by running dsd 0.11.0 against an isolated copy of the config. A *file entry* is
an unindented path ending in `:`, followed by indented lines:

```text
    .text       start:0x02004000 end:0x020736f4 kind:code align:32
    ...
src/AngleDiff.c:
    complete
    .text start:0x0203b0e8 end:0x0203b0fc
```

Two behaviours that matter, both confirmed empirically rather than assumed:

- **`align:32` is not inherited.** dsd emitted the carved object with
  `sh_addralign=4`, and the generated lcf places gap and file objects back-to-back with
  no `ALIGN` directive between them, under `ALIGNALL(4)`. No `align:4` override needed.
- **`complete` selects the object source.** With `complete`, `objects.txt` points at
  `build\src\AngleDiff.o` — our compiled object. *Without* it, dsd emits
  `build\delinks\src\AngleDiff.o` from ROM bytes and points there instead. That second
  mode is what M2a below exploits.

Gap objects reference a carved-out symbol as **weak undefined**, so a missing compiled
object links silently to address 0 rather than erroring. `dsd check symbols` /
`check modules` is the safety net here, not the linker.

## Milestones

### M0 — Baseline relink, zero source involved

Link the 93 existing gap objects exactly as they are, build a ROM, and prove it is the
retail game. This validates linker flags, the lcf dialect, `rom config`, `rom build`,
and the ARM7/secure-area/CRC handling *before* any of our C is in the mix.

**Settle path plumbing first (30 seconds, before interpreting any error).**
`build/objects.txt` holds repo-root-relative paths (`build\delinks\...`), the lcf's
MEMORY block writes `> build/arm9.bin`, but `config/arm9/config.yaml` expects built
binaries at `../../build/build/arm9.bin`. Run mwldarm from the repo root, then `ls
build/ build/build/` to see where the region binaries actually landed. Also
`mkdir -p build/src build/build` up front.

**Gate:** `dsd check modules --fail` green for all modules, and `build/sm64ds.nds` boots
to the title screen in melonDS.

**The gate is deliberately not "the .nds hashes equal the dump."** `arm9.yaml` says
`compressed: true`, and `overlays.yaml` has `table_signed: true` plus per-overlay
`compressed: true, signed: true` against `arm9/hmac_sha1_key.bin`. File-level identity
therefore depends on dsd's BLZ compressor and HMAC re-signing being bit-exact, which is
not what this milestone is testing. `check modules` compares decompressed modules
against the `hash:` fields in `config.yaml` and is compression-independent. If the .nds
differs while modules are green, that is the compression/signing story — record it and
move on.

Remaining unknowns to settle here, not guess:
- Whether 1.2/sp2p3 `mwldarm.exe` accepts this lcf dialect (`AFTER`, `ALIGNALL`,
  `WRITEW`). A parse error naming an lcf line would be fatal to the whole approach —
  which is exactly why this milestone is first.
- Where `dsd rom config` writes its output.
- Whether `dsd rom build` needs `--arm7-bios`. The secure area is already plaintext
  (`encrypted: false`), so probably not, but confirm.
- Benign `dsd delink` warnings ("No module for relocation … to 0x01ff…") are expected:
  unmodeled relocs leave raw ROM bytes in the slot, which is byte-preserving at fixed
  addresses.

ARM7 (`extracted/dsd/arm7/arm7.bin`) and the asset file image (`extracted/dsd/files/`
plus `path_order.txt`) ride through `rom build` untouched. The `.sav` is emulator-created;
nothing to do.

#### M0 result — passed

Every unknown resolved favourably on the first attempt:

- **mwldarm 1.2/sp2p3 accepts the lcf dialect** — `AFTER`, `ALIGNALL`, `WRITEW` and all —
  linking 93 gap objects with exit 0 and no diagnostics.
- **The path question answered itself.** mwldarm resolves `@build/objects.txt` entries
  against the CWD (repo root) and the MEMORY block's `> build/arm9.bin` against the lcf's
  own directory, so the region binaries land in `build/build/` — exactly where
  `config.yaml` expects them. Run mwldarm from the repo root and it just works.
- `dsd lcf -c` writes both `build/arm9.lcf` and `build/objects.txt` from `build_path`.
- `dsd rom config` writes `build/build/rom_config.yaml` (plus per-module yaml).
- **`--arm7-bios` is not needed.** `dsd rom build` produced a full 16,777,216-byte ROM
  without it.
- `dsd check modules --fail`: all 106 modules OK, exit 0.

And the .nds is much closer to the dump than the gate required: **107 differing bytes out
of 16,777,216**, all metadata, in three groups —

| Offset | Count | What |
|---|---:|---|
| `0x06152F` + `0x20`·n | 103 | overlay table entry flag byte, `3` → `1`. dsd compresses each overlay but does not re-apply the HMAC signature, so it clears the "signed" bit and the ARM9 loader skips verification. |
| `0x00006C` | 2 | secure area CRC, zeroed (the area is already plaintext, `encrypted: false`). |
| `0x00015E` | 2 | header CRC16, correctly recomputed for the changed header. |

Every byte of code, data, and all 2,072 asset files is identical, and dsd's BLZ
compressor reproduces the retail compressed streams exactly. The overlay-signature bit
is the one thing to remember: **signature verification is off in every built ROM**, which
is what makes overlay edits possible at all, but it also means a signing bug could never
show up as a failure here.

### M1 — One function from source

`AngleDiff` is a verified-good pick: `config/arm9/symbols.txt:1513`
(`kind:function(arm,size=0x14) addr:0x0203b0e8`), the next symbol is exactly adjacent at
`0x0203b0fc`, `src/AngleDiff.c` is pure ALU with no literal pool, no data and no calls,
and zero relocations originate inside it.

1. Add its file entry (with `complete`) to `config/arm9/delinks.txt`.
2. `dsd delink` — the main gap object splits, regenerated without that range.
3. Compile with the canonical toolchain and flags (`2004/b56`, `-O4,p -enum int
   -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc`,
   `-i include/`) to `build/src/AngleDiff.o`.
4. `dsd lcf`, link, `dsd rom config`, `dsd rom build`.

**Gate:** `dsd check modules` still green — a byte-identical main module where one
function's bytes came from our C.

#### M1 result — passed

`config/arm9/delinks.txt` grew the three-line entry; `dsd delink` split the single
`_dsd_gap@main_7.o` into `_dsd_gap@main_8.o` (everything before) and `_dsd_gap@main_1.o`
(everything after), and the generated lcf placed them back-to-back around ours:

```sh
_dsd_gap@main_8.o(.text)
AngleDiff.o(.text)
_dsd_gap@main_1.o(.text)
```

`objects.txt` names `build\src\AngleDiff.o` — confirming the object-path derivation is
`build_path` + the delink file path with a `.o` extension. Linked ARM9 is byte-identical
to the extracted one and `check modules` stays green.

**Negative control.** Because gap objects reference carved-out symbols as *weak*
undefined, a green build could in principle mean "our object was ignored". So a
deliberately wrong `AngleDiff` (`return d + 1`, compiled into the same object slot
without touching `src/`) was linked: the ROM changed, confirming our bytes are really the
ones placed. It also demonstrated the cascade to respect at M3 — the extra instruction
grew the function past its 0x14-byte slot, shifting every later address and re-pointing
absolute pool words throughout the module, so **305,977** bytes of `arm9.bin` differed,
starting well *before* the edited function. Restoring the real source returned the build
to byte-identical. **A behaviour edit must keep the function's size unchanged.**

Then flip **one deliberate interworking caller** (a function whose `BL` targets a Thumb
SDK symbol; main alone has 26 Thumb functions) and observe whether mwldarm emits `BLX`
or synthesizes a veneer. A new veneer adds bytes, and via the lcf's `AFTER()` chains
that moves downstream overlay origins — so this is worth knowing at M1 with one file in
play rather than at M2 with thousands.

### M2a — All candidate file entries, no `complete`

Land file entries for the whole candidate set **without** `complete`, so dsd supplies
ROM-bytes objects for every one of them. The result is byte-identical *by construction*,
which makes this a pure scale test: it proves `dsd delink`, `dsd lcf`, and an ~11,000-object
`mwldarm` link survive before correctness is ever in play.

Scale is friendlier than the raw file count suggests. Matched functions form only **210
contiguous runs corpus-wide (7 in main)**, and gap-object count tracks run boundaries,
not function count — main's `.text` has just 25 inter-function holes totalling 5,878
bytes. Object count (~11k) is fixed by the one-function-per-file convention and cannot be
coalesced at the `delinks.txt` level without merging TUs, which the repo layout forbids.
If mwldarm chokes, the fallback is partial-linking runs into intermediate objects
(`mwldarm -r`, unverified) or capping enrollment per module.

Ramp per module: main first (3,084 entries, 7 runs) is the cleanest stress test.

#### M2a result — passed

`tools/enroll.py` generates the file entries; `config/rombuild-exclude.txt` records what
cannot be carved and why. **11,090 entries** across 73 modules, **11,390 objects** in
`build/objects.txt`, and `mwldarm` linked the lot in ~50 s wall clock for the whole
pipeline. All 106 modules green; the packaged `.nds` is still exactly the same **107
metadata bytes** away from the retail dump as at M0. Carving raised the gap-object count
from 93 to 374 — still small, as predicted by the contiguous-run analysis.

The scale worry was unfounded; what actually cost the iterations were three concrete
discoveries, none of which show up at one-function scale:

1. **A local label cannot cross an object boundary.** `func_0206ce90`, `func_0206cee0`
   and `func_0206d868` `arm_call` 0x0206d9dc and 0x0206da28 — addresses with no symbol,
   because they are *secondary entry points* 0x10 bytes inside `func_0206d9cc` and
   `func_0206da18`. dsd names such a target `.L_0206d9dc` and then refuses:
   `Imported symbol .L_0206d9dc … is local, it cannot be used in relocation`.
   Excluding just the three callers does not help — the error simply moves to the gap
   object, because carving any function *between* caller and target splits the span. The
   whole 12-function run has to stay in one object. Giving those two addresses real
   symbols in `symbols.txt` would recover all twelve.
2. **Per-entry alignment cannot be overridden.** dsd rejects it outright:
   `attribute 'align' should be omitted as it is inherited from this file's header`.
   Carved code sections get 4-byte alignment, so a function that does not start
   4-aligned — the thumb SDK stubs (`Div` at 0x0200406a, `IntrWait`, `CpuSet`, …) — is
   padded up by the linker. That inserted 2 bytes at the very first entry and shifted
   the entire module: main came out 32 bytes long with 497,337 differing bytes. **12
   functions** are excluded on `addr % 4 != 0`.

   This refines the M1 finding. Fable verified alignment was not inherited *for a
   4-aligned ARM function*, which was true and is why M1 passed; the inherited-align
   problem only bites where the address is not already 4-aligned.
3. **The `// NONMATCHING` hatch must be honoured.** 84 files have a `src/` entry but are
   explicitly not byte-matches, so they are candidates for neither mode.

Final tally: 11,090 enrolled, 191 with no `src/` file, 84 NONMATCHING, 12 unaligned, 12
in the secondary-entry-point cluster.

### M2b — Eligibility flip

Now flip `complete` on, per module, for files that pass a **whitelist**. A blacklist is
the wrong shape here: the modules also carry `.exception`, `.exceptix`, `.init` and
`.ctor` sections, and anything unexpected grows the module and shifts every later
address.

A file enrolls only if:

- its object has **exactly one non-empty section, named `.text`**, holding **exactly one
  defined global FUNC symbol** whose `st_size` equals the declared symbol size;
- the symbol's address lies in a `.text` range — **not `.init`**. About **301 matched
  functions live inside `.init` ranges**; their objects emit code into a section named
  `.text`, so a file entry declaring `.init` would generate a `File.o(.init)` selector
  that matches nothing. Hard-exclude them;
- every undefined reference resolves to a real, addressed symbol;
- a **fresh strict `linkcheck` pass** rates it VERIFIED — not BENIGN, not BLIND. The
  corpus figures in [`link-verification.md`](link-verification.md) are from 7,180
  matches on 2026-06-29 and `src/` now holds 11,203 files, so ~4,000 matches have never
  been linkchecked and the 1,787 BLIND count is a floor. BENIGN matters because those 26
  cases are exactly the veneer/twin/ARM→Thumb cases that `linkcheck` waves through for
  *match* verification but that a real relink resolves differently — silently shifting
  bytes instead of failing loudly.

Why one `.text` section and not "no extra sections": `tools/linkcheck.py` documents that
mwccarm emits **one `.text` section per function** within a TU, and the lcf's
`File.o(.text)` selector places *all* of them. A multi-function TU (any C++ dtor emitting
`D0`/`D1`/`D2` plus thunks) injects extra code at the wrong addresses even when no symbol
name collides — and `-nodead` disables the dead-stripping that might otherwise hide it.

Bisection on a mismatch starts at the **earliest module in the `AFTER()` partial order**
(`build/arm9.lcf:5–109`): a size change in ov002 shifts the origins of ov008–ov102, so
dozens of red modules mean "look upstream", not "dozens of bugs". Verdicts land in
`build/rombuild-eligibility.json`.

**Gate:** thousands of functions sourced from `src/`, `dsd check modules` still green,
one-command reproducible build.

#### M2b result — passed

**7,916 functions are compiled from `src/` and every one of the 106 modules is still
byte-identical to the ROM.** That is 1,346,916 code bytes, **60.9%** of the project's
2,211,124. The `.nds` remains exactly 107 metadata bytes from the retail dump. Whole
build: ~80 seconds.

Four things got it there, and only the first was in the plan.

**1. `-Cpp_exceptions off`.** The dominant blocker was not C++ TUs or BLIND symbols — it
was `.exceptix`, on **8,520 of 11,090** candidates. mwccarm emits an exception-unwind
index alongside almost every function and the match gate never noticed, because it only
ever compares `.text`. A ROM link very much notices: duplicate content that grows the
module and shifts every later address. Retail carries just 636 bytes of `.exceptix` in
main, so the original build had them off too. The flag cleared the extra sections on
**60 of 60** sampled files with **zero `.text` change**.

**2. The default / matching-pin compiler is `2004/b56`.** Matching docs and
`tools/match.py` CANONICAL use the same pin. A function still counts as matched if
*any* swept version reproduces it (use `match.py --all` / `--trio`), and a link has
to pick one. Defaulting to the recovered 2004 build 0056
(`notes/mwccarm-codegen.md` 6ai) reproduces strictly more of this corpus: eligibility
7,466 → 7,923, and post-link mismatches **70 → 14**. Note b56 ships *only* `mwccarm.exe`,
so `LD_VERSION` keeps the link on 1.2/sp2p3's `mwldarm` regardless.

**3. Per-file version overrides -- now empty for the ROM build.** Every enrolled
function reproduces under the default `2004/b56`, so the ROM build runs ONE compiler.
`config/rombuild-versions.txt` retains a single entry,
`_ZN11dScMgCard_c13InitResourcesEv`, which is **not enrolled** and so never reaches a
ROM build; it is 999 words off under b56 and genuinely still needs `1.2/base` when its
turn comes. `tools/rombuild_versions.py` regenerates the file by sweeping `match.py`'s
version list over whatever `rombuild_check.py` reported.

The eight that used to need `1.2/base` were not compiler facts, they were **source
facts**. Retail was built with one toolchain, so a function reproducing only under a
different one meant our reconstruction differed from the original in a way b56 exposed
and 1.2/base happened to hide. Two source levers moved all eight, both verified by
compiling the same snippet under both compilers in isolation:

- **Spell a re-read differently from the write.** Where the ROM stores through a
  computed address and then RE-READS the field with a base+displacement load, b56
  recognises the second access as the first lvalue and reuses the register. Writing the
  two accesses with different expression shapes (`(char *)p + K` vs `(int)p + K`, or an
  index) stops it. Every sharing site has to be respelled together -- `func_02062428`
  has three, and respelling any one alone left the other two sharing. Fixed
  `func_ov015_02111e80`, `func_ov006_020ded00`, `func_ov013_021112a8`, `func_02062428`,
  `func_ov006_02111e90`.
- **Prefer pointer arithmetic on a typed pointer over integer arithmetic then a cast.**
  `ldr` carries a 12-bit displacement and `ldrh`/`ldrsh`/`strh` only 8, so a large
  offset must be split. Given pointer arithmetic b56 splits it the ROM's way; given
  integer arithmetic it materialises the whole constant, from the literal pool if
  needed. In `func_0206a6d0` that one extra pool entry shifted every pc-relative load in
  the function by 4 -- a 52-word diff from a single spelling. Also fixed
  `func_ov081_02127558`, where the compound-assignment form is the only spelling b56
  refuses to split; an explicit read-modify-write through a temporary reproduces, and
  pre-splitting the base in the source does NOT work -- b56 has to do its own splitting.

`func_ov084_0212f460` was neither: a straight r6/r7 swap. Callee-saved registers are
handed out in **assignment order** under b56 -- declaration order moves nothing -- so
hoisting the pointer's assignment above the other local's put them the ROM's way round.
The ROM emits the two in the opposite order regardless, because the independent store
schedules ahead of the address computation that feeds the compare.

**The table is still keyed by file stem, and that still matters.** `compile_one` does
`vers.get(pathlib.Path(rel).stem, VERSION)`, so **renaming a pinned function's file
detaches its pin unless the same commit re-keys it.** That failure is uniquely
expensive: the build falls back to `2004/b56` and emits wrong bytes for a function whose
source is perfectly correct, while every per-file gate keeps calling it exact, because
`build_pin.verify`, `pr_linkcheck` and `eligible.py` all read this same table and
compile *with* the pin. Only the whole-module compare disagrees, by a handful of bytes,
in one module. #1607 spent a day on that shape before
`_ZN21ClockPaintingPendulum8BehaviorEv` -- the first mangled pin key a ROM build had
ever exercised -- was identified as the cause. With the pins gone that route is closed
for now, but it reopens the moment a new pin is recorded.

`rombuild.audit_version_pins` checks the table against the tree before the first
compile: a pin naming no `src/` or `mods/` file at all is a hard preflight error, a pin
naming an uninstalled service pack is too, and every applied pin is printed by name in
phase [3/6] and recorded in the report as `alternateToolchain.applied`.
`tools/validate_merge.py` renders that alongside the failing module, so a `105/106` in a
PR comment names the module, the function, the address and the byte count instead of
only counting modules.

**4. Per-function diffing beats bisection.** Because eligibility requires the object's
`.text` to equal the declared size exactly, no function can shift its neighbours, so
every mismatch is *confined to the function that caused it*.
`tools/rombuild_check.py` compares each source-built function against the ROM
individually and names all the culprits from **one** build. No bisection, no walking the
`AFTER()` order.

##### What the first 70 mismatches turned out to be

Worth recording, because the intuitive reading was wrong. Under the old 1.2/sp2p3
default, 70 of 7,466 did not reproduce. Running them through the repo's own oracle split
them cleanly:

- **54 were not broken at all.** `tools/match.py` rejected them under 1.2/sp2p3 but
  *every one* matched under `2004/b56`. They were version-divergent, not wrong — which
  is what motivated flipping the default. Their last-touching commits are mostly the
  `readable: strip N unnecessary compiler-steering no-op masks` passes, so those passes
  appear to have re-verified under a different version than the ROM build was using.
- **16 were genuine wrong relocation destinations** — the class the match gate
  structurally cannot see, because it wildcards every relocated word, so a call to the
  wrong function with the right shape reads as a match. `tools/rombuild_diag.py` decodes
  each differing word and resolves both sides back to symbols, turning the byte diff into
  "the source calls X, the ROM calls Y".

Nine of the sixteen were mechanical symbol repoints, applied to `src/` and verified
individually with `match.py` before the link confirmed them:

| function | source named | ROM uses |
|---|---|---|
| `_ZN13PrincessPeach6RenderEv` | `CommonModel::Render` | `Model::Render` |
| `_ZN7Tornado6RenderEv`, `_ZN9WaterRing6RenderEv` | `TextureSequence::Update` | `TextureTransformer::Update` |
| `_ZN11BillBlaster4KillEv` (then `func_ov079_02126e58`) | `Actor::DisappearPoofDustAt` | `Actor::PoofDustAt` |
| `func_ov085_0212e778` | `…ApplyInPlaceToRotationXYZExt` | `…ApplyInPlaceToRotationZXYExt` |
| `func_ov006_0211a048`, `func_ov006_0211a5ec` | wrong `data_ov006_*` base | the adjacent one |
| `func_ov006_02120c08` | `func_ov006_020eed68` | `func_ov006_02120a64` |
| `func_ov002_020f23d0` | the veneer `func_0203cbc0` | `Memory::operator_delete2` |

The `XYZExt` → `ZXYExt` one is a genuine behavioural bug, not just a naming slip.

##### The vtable off-by-8 — a real bug, found by review

`_ZN11ShadowModelC1Ev` was initially filed above as a non-bug. It is not. The source
wrote `thiz->vtable = _ZTV11ShadowModel + 2;` on a `u32[]`, i.e. **symbol + 8 bytes**,
while the ROM's pool word is the symbol **+ 0**:

| function | `_ZTV*` symbol | ROM pool word |
|---|---|---|
| `_ZN11ShadowModelC1Ev` | `0x0208e868` | `0x0208e868` (+0) |
| `_ZN5ModelC1Ev` | `0x0208e90c` | `0x0208e90c` (+0) |
| `_ZN5ModelC2Ev` | `0x0208e90c` | `0x0208e90c` (+0) |
| `_ZN11CommonModelC1Ev` | `0x0208e8a4` | `0x0208e8a4` (+0) |

The `+ 2` is the Itanium-ABI reflex (skip offset-to-top and RTTI), but this ABI points
the vptr at the vtable base, so all four were storing a vptr 8 bytes too high — virtual
dispatch through it would index the wrong slots. Three of the four were invisible to the
ROM build only because they are blocked upstream on an unrelated unresolvable symbol.

The match gate cannot see this: the pool word is a relocation, and `match.py` wildcards
every relocated word, so all four "matched" byte-for-byte with the bug in place. Removing
`+ 2` keeps them byte-matching and fixes the destination. **This is the single strongest
argument for the ROM build as a gate** — it catches a class the byte oracle is blind to
by construction.

**Three remain. All three of the original six that took the cast-and-call shape were
retired by the C++ class migration; see the struck-through bullet below.**

- three (`_ZN5ModelD1Ev`, `_ZN5ModelD2Ev`, `_ZN14BlendModelAnimD1Ev`) call `_ZdlPv`
  where the ROM goes through the 12-byte veneer at `0x0203cbc0`. That trampoline is the
  standard `LDR ip,[pc]; BX ip; .word target` shape, and its target word is
  **`0x0203cbf0` = `_ZdlPv`** —
  the same callee the source names — passing *over* `_ZN6Memory16operator_delete2EPv` at
  `0x0203cbcc`. So ROM and source call the identical function and only the route differs.
  [`link-verification.md`](link-verification.md) is explicit that naming the real symbol
  is correct and inserting a veneer is the linker's job. It is also unreachable: the
  veneer is a pre-existing ROM function living in the gap object, and mwldarm only
  synthesizes veneers it *needs* (out-of-range or interworking, neither of which applies
  to an in-range ARM→ARM `BL`). No linker flag routes a call through another object's
  existing trampoline. Left alone deliberately.
- ~~three (`_ZN11MirrorLuigiD1Ev`, `_ZN15RecRoomCupboardD0Ev`, `_ZN15RecRoomCupboardD1Ev`)
  write `((Actor *)c)->~Actor()`, for which the compiler emits the D1 complete-object
  destructor while the ROM calls the D2 base-object one.~~ **Two of the three are fixed,
  and by exactly the route this bullet predicted.** `include/RecRoomCupboard.h` now derives
  the class from `dActor_c` and gives it its five `dCcAcPos_c` members, and both destructor
  files are an empty `RecRoomCupboard::~RecRoomCupboard()`; the compiler picks D2 for the
  base step on its own. Both came off `config/rombuild-exclude.txt` and the ROM build
  carries them: source-built functions 11,061 -> 11,063, mismatching 0,
  106/106 exact. `_ZN11MirrorLuigiD1Ev` came off the list separately, in the MirrorLuigi
  class migration (#1782), by the same route.
  Only real inheritance would make the compiler choose D2, so this is structural, not a rename — exactly the kind of work
  the `readable/` C++-promotion branches do.
- ~~one (`_ZN11ShadowModelC1Ev`) stores a vtable pointer that resolves elsewhere than
  `_ZTV11ShadowModel`.~~ **This one was a real bug, not a non-bug** — see below.

Why the rest are not source-built yet:

| count | reason |
|---:|---|
| 1,972 | references a symbol name `config/**/symbols.txt` does not define — the BLIND matches. No address means nothing to link to, and gap objects import weakly, so it would silently resolve to 0. |
| 711 | the object defines a different symbol than the file/config name (e.g. `src/func_ov091_02132a0c.c` defines `daDsn_c_OnAimedAtWithEgg`) — a src/config naming drift, likely recoverable by reconciling names. |
| 70 | compiled `st_size` ≠ the size `symbols.txt` declares |
| 301 | lives in a `.init` range, where a `File.o(.init)` selector would match nothing in an object whose code is in `.text` |
| 88 | emits `.data` or `.bss` whose ROM address we do not know |
| 13 | multiple `.text` sections — a multi-function TU |
| 12 | fails to compile with the build flags |
| 6 | the veneer / D1-vs-D2 cases above |

### M2c — Reconcile the name mismatches

The largest remaining bucket was 711 files whose object defined a different symbol than
their filename. They turned out to share an annotation pair:

```sh
// @symbol func_ov091_02132a0c        <- the symbol at this ROM address
// @emits  daDsn_c_OnAimedAtWithEgg   <- what the C actually defines
```

The second name is recovered knowledge, usually from a vtable slot. **Nothing in the repo
consumes either annotation** — no tool, no CI config, no doc mentions them — and the
divergence quietly broke two things:

- **`match.py` cannot verify these files at all.** Asked for the filename's symbol it
  reports `symbol 'func_ov091_02132a0c' not found in object`, so the byte oracle returns
  `MATCHING VERSIONS: none` for every one of them. Meanwhile `progress.py` counts them as
  matched, because it only checks that a `src/<name>.c` exists without a `NONMATCHING`
  hatch. **712 files, about 6% of `src/`, were being counted as matched while being
  unverifiable by the repo's own gate.**
- **The ROM link cannot use them.** Gap objects import a carved-out function by its
  `symbols.txt` name as a *weak* undefined, so a differently-named definition leaves every
  caller silently binding to address 0 instead of erroring.

`tools/reconcile_names.py` renames the emitted function to the `@symbol` name, replaces
the now-redundant `@emits` line with `// recovered name: <name>` so the recovered identity
is preserved, and byte-verifies each file against the ROM before keeping the edit.

**711 of 712 verified and were applied.** The one holdout, `func_ov022_021123d0`, declares
its own symbol with a conflicting signature and needs a human. Of the 711, **699 reproduce
under the build's default `2004/b56`** and only 12 need a `1.2/base` override — further
evidence that b56 is the right default for this corpus.

Eligibility went 7,914 → 8,374 and source-built functions 7,907 → **8,367** (60.9% →
63.8% of code bytes). The gap between +711 renamed and +460 enrolled is files that were
blocked by a second reason as well — a BLIND symbol, a size mismatch, an `.init` home.

The opposite direction — adopting the recovered names into `config/**/symbols.txt` — is
the better long-term move and is left deliberately undone: it changes the canonical symbol
table and every reference to those names, which is a maintainer's call, not a build fix.

### M2d — Resolve the BLIND references

The next bucket was ~1,970 files referencing a name `config/**/symbols.txt` does not
define. [`link-verification.md`](link-verification.md) calls these the BLIND matches and
concludes "No static tool can verify those destinations; only a fuller symbol table
would."

That is true of a *static* tool, but the ROM build is not one. The function byte-matches,
so the ROM's own linked word at the relocation slot **is** the destination: decode it (a
`BL` target, or an `R_ARM_ABS32` pool word minus its addend) and look the address up.

`tools/resolve_blind.py` does that per file and per relocation, which matters because the
names come in two very different shapes and this handles both the same way:

- **Shared names with one true target.** `_Z28_ZN13SharedFilePtr7ReleaseEvPv` (177 files)
  is `_ZN13SharedFilePtr7ReleaseEv` — a double-mangling artifact, someone mangled an
  already-mangled name. Same for `_Z39_ZN9Animation8LoadFileER13SharedFilePtrPv`,
  `_Z31_ZN16MeshColliderBase7DisableEvPv`, `_Z10DeallocatePv` → `Deallocate`.
- **Per-file placeholders.** `G0` appears in 480 files and means **173 different
  addresses**; `G1` 107, `G2` 142, `G3` 111, `VT1` 42. A global rename would be actively
  wrong here — only per-file resolution works.

Of 6,238 unresolvable references, **6,238 − 65 resolve to a real symbol**. The 65 that do
not are mostly not addresses at all (`0x04000204` is a hardware register, `0x00000006` a
constant), so nothing is renamed for them.

Every rename is byte-verified before it is kept; a file that stops matching is reverted.
1,227 files were renamed, eligibility went 8,374 → 9,124, and source-built functions
8,367 → **9,115** (63.8% → 66.7% of code bytes).

### M3 — Prove it is really our code

Change a constant in a source-built function, rebuild, boot in melonDS, observe the
difference, then revert and confirm the ROM returns to identical.

**Overlay edits are fine after all.** The original worry was HMAC signing, but M0 showed
dsd *clears* the per-overlay signed bit (the 103 flag bytes `3` → `1`), so the loader
never verifies and an overlay edit loads normally — which the ov002 mod confirms in
practice. Still worth knowing: a white screen with green `check modules` would point at
the header CRCs (`extracted/dsd/header.yaml`), which `check modules` does not cover.

**Gate:** a visible in-game change traced to a one-line C edit. This is the deliverable
the whole exercise is for.

#### M3 result — passed

**The built ROM boots and plays.** melonDS 1.1 runs `build/sm64ds.nds` at a locked
60/60 through title → "touch the picture" → main menu (3D Yoshi) → file select (castle
flyover) → the opening cutscene → gameplay on the castle grounds, with the HUD, the
touch-screen minimap, 2D and 3D all correct. No BIOS files and no `--arm7-bios` needed.

**Intentional divergences live in `mods/`, not `src/`.** `src/` must byte-reproduce the
ROM, so a modified function goes in `mods/<Symbol>.c`. Select it explicitly with
`python tools/rombuild.py --profile mods`; the normal command remains stock even when a
tracked `delinks.txt` entry points at that mod.

`mods/Player_ScaleByCharFactor.c` (shift 12 → 11, doubling the scale factor) built
cleanly and landed exactly as intended: **3 bytes changed**, both at the shift
instructions, all inside the function, ov002's size unchanged at 394,048 bytes, so
nothing downstream moved. Saved as `build/sm64ds-mod.nds`.

**Confirmed in play.** A/B of `build/sm64ds-mod.nds` against `build/sm64ds.nds` shows the
difference in the running game. `Player_ScaleByCharFactor` is called from 44 sites across
ov002's player physics, so doubling it is felt in movement rather than seen in a still
frame — which is why a screenshot comparison was the wrong instrument for it and playing
was the right one.

So the whole chain holds end to end: a one-operator edit in a C file, through mwccarm and
mwldarm, into a linked overlay that is byte-identical everywhere except the three bytes
that changed, packaged into a ROM that boots and plays differently. That is the
deliverable.

`mods/` is where an intentional divergence lives; `tools/enroll.py` may still prefer
`mods/<symbol>.c` when regenerating tracked enrollment, while the generated stock build
profile safely redirects it back to `src/<symbol>.c`.

## Constraints

- **Nothing ROM-derived is ever committed.** `build/`, `extracted/`, `*.nds`, `*.bin`
  and `*.o` are already gitignored. The built ROM stays local, like the dump it came
  from. Only `tools/rombuild.py`, the `delinks.txt` file entries, and this note are repo
  content.
- **`delinks.txt` edits are config, not matches**, and `AGENTS.md:49` keeps tooling and
  config changes out of match batches — they go in their own PR.
- **ARM7 is not decompiled** and passes through untouched.
- **An emulator is required and is not installed.** melonDS is the accuracy reference;
  DeSmuME also works.
