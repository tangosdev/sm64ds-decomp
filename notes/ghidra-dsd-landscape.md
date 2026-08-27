# Ghidra + dsd: what we have, what's on the shelf, what it would cost

Scope: our current Ghidra usage, the `dsd-ghidra` loader extension, whether we drive
`dsd` correctly, and what `ds-decomp` PR #37 would buy us.

Surveyed 2026-08-15 against worktree `C:\tmp\sm64ds-ghidra` (branch `research/ghidra`,
off `origin/main` @ 1704747fa). Confidence markers follow `notes/research-matching-levers.md`.

---

## 1. What Ghidra usage exists today

**G1.1 — There is exactly one Ghidra integration, and it is a raw headless dump.** [high]
`tools/ghidra/DecompDump.java` is a `GhidraScript` post-script: reads a targets file of
`0xADDR name size` lines, forces a function at each address if auto-analysis missed it,
runs `DecompInterface`, writes `<outDir>/0xADDR.c`. Invoked as
`analyzeHeadless ... -postScript DecompDump.java <targets> <outDir>`.

**G1.2 — Its only consumer is the LLM cascade.** [high] `tools/cascade.py:37-45,226-243`
reads `ghidra_out/0x%08x.c` under `--scaffold ghidra` and pastes it into the prompt,
explicitly labelled "approximate, NOT byte-matching; use it for structure/types/callee
names." Nothing else in the tree reads it.

**G1.3 — Both inputs and outputs are gitignored and currently absent.** [high]
`.gitignore:108-110` covers `ghidra_out/` and `ghidra_targets.txt` ("derived from ROM;
local only"). Neither exists in the main checkout today. So the scaffold path is
**dormant**, not broken.

**G1.4 — Ghidra is not installed on this machine.** [high] No `ghidra*` directory under
`C:\`, `D:\`, `~/Downloads`, `~/source`, or `~/AppData/Local` (depth 2); no `ghidraRun`
or `analyzeHeadless` on PATH. Only JDK **11.0.16** is installed (`C:\Program
Files\Microsoft\jdk-11.0.16.101-hotspot`); Ghidra 11.x needs JDK 21. Whatever produced
the historical `ghidra_out` drafts ran elsewhere or has been removed.

**G1.5 — The historical verdict on raw Ghidra drafts is mixed, and recorded.** [high]
`CLAIMS.md` cites them by name both ways: a win (`ov102 func_ov102_0214b53c` — "Ghidra
dest + ROM-order angle" → byte-identical) and repeated losses (`ov006 func_ov006_020dbe9c`
— "Ghidra missed s64 matrix"; `arm9 OAM::Render` — "Ghidra dump div=999 (frame 0x44)";
`ov006 func_ov006_0211e72c` — "Ghidra-shaped 30w attractor" that *stalls* at 26 words).
`README.md:102` states the house position: useful for reading a function, "its output
never matches on its own."

---

## 2. `dsd-ghidra` — the loader extension

<https://github.com/AetiasHax/dsd-ghidra>, MIT, by the dsd author.

**G2.1 — It is a DS ROM *loader* plus a one-way sync script.** [high] `DsRomLoader.java`
loads a `.nds` dropped into a Ghidra project; `ghidra_scripts/SyncDsd.java` then pulls a
dsd config into the program. The README explicitly says **do not run auto-analysis**
after import — run SyncDsd instead, "will result in more accurate function analysis."

**G2.2 — SyncDsd imports what our whole config knows, and writes nothing back.** [high]
Functions (with ARM/Thumb mode), data symbols, sections, the arm9 module, overlays,
**autoloads (ITCM/DTCM)**, relocations as real cross-references, and inline constant
pools. It has a dry-run checkbox. Data flow is dsd → Ghidra only; the config files on
disk are not touched. That makes it **safe against every byte gate we run** — it cannot
perturb `config/**/symbols.txt`.

**G2.3 — This is the actual delta over `DecompDump`.** [high, by construction] Our
current path imports a flat binary with no memory map: no overlay boundaries, no
ITCM/DTCM, no relocations, no symbol names, and function starts guessed by
auto-analysis or forced by address. SyncDsd supplies all five from the config we already
maintain. Whether better decompiler *output* follows is [medium] — a cross-overlay `bl`
resolving to `_ZN7Wiggler8BehaviorEv` instead of `undefined FUN_02112b5c` is a
straightforward gain; the s64-matrix and stack-frame misses in G1.5 are decompiler
limitations that a correct memory map does not fix.

**G2.4 — Version pin: use v0.6.0, NOT the latest.** [high] Every dsd-ghidra release
ships a prebuilt `ghidra_11.2.1_PUBLIC_<date>_dsd-ghidra.zip` — all eleven releases
target **Ghidra 11.2.1**, so that exact Ghidra version is required, not the newest.
The native half links ds-decomp as a Rust library:

| dsd-ghidra | date | `ds-decomp` pin | `ds-rom` |
|---|---|---|---|
| v0.7.0 | 2026-08-14 | **0.12** | 0.8 |
| **v0.6.0** | 2026-04-10 | **0.11** | 0.7 |
| v0.5.0 | 2025-07-24 | 0.10 | 0.6 |

Our `tools/bin/dsd.exe` is **0.11.0**. v0.6.0 is the version-exact match. v0.7.0 would
read our 0.11 config with a 0.12 parser — probably fine (0.12's changes look additive:
`.exceptix`, reloc addend hints, weak symbols) but unverified, and there is no reason to
take the risk when v0.6.0 exists.

**G2.5 — No build step needed.** [high] Prebuilt release zips install via Ghidra's
File → Install Extensions. Building from source *would* need Rust + Gradle, neither of
which is installed here (`cargo`, `rustc`, `gradle` all absent) — so take the release.

**Install cost, end to end:** JDK 21 + Ghidra 11.2.1 + the v0.6.0 zip. ~1.5 GB, no
compilation, nothing touching the decomp tree.

---

## 3. Are we using `dsd` correctly?

`tools/bin/dsd.exe --version` → **ds-decomp-cli 0.11.0**. Latest upstream is **0.12.0**
(2026-08-14, one day old).

**G3.1 — We use six of dsd's fifteen subcommands, and they are the right six for a
build-and-verify loop.** [high] From `tools/rombuild.py:487-545` and
`tools/tubuild.py:2136-2424`:

| used | where | role |
|---|---|---|
| `delink` | rombuild [1/6], tubuild [2/8] | gap objects carrying ROM bytes |
| `lcf` | rombuild [2/6], tubuild [3/8] | `arm9.lcf` + `objects.txt` for mwldarm |
| `check modules` | tubuild [6/8] (`-f`) | the byte gate: 106/106 |
| `check symbols` | tubuild [7/8] (`--fail`) | named symbols survive the link |
| `rom config` | rombuild [5/6], tubuild [8/8] | ROM spec from the linked ELF |
| `rom build` | rombuild [5/6], tubuild [8/8] | reassemble `sm64ds.nds` |

That is exactly the upstream workflow's back half (delink → lcf → build → verify), and
`eligible.py:26` correctly names `dsd check modules` as the arbiter of a replacement.
**No misuse found.**

**G3.2 — Nine subcommands are unused. Three of them are worth something.** [high]
Unused: `init`, `dis`, `import`, `objdiff`, `fix`, `apply`, `dump`, `json`, `sig`,
`format`. `init` is correctly one-time-only (already run). Of the rest:

- **`sig`** — see §4. The big one.
- **`objdiff`** — generates an `objdiff.json` (with `-C`/`-f`/`-p` for decomp.me
  scratches and `-m`/`-M` for a custom build command). We hand-roll our own diff via
  `match.py`/`nearmiss`. Worth a look for the interactive per-function diff UI, but our
  oracle already works; low urgency. [medium]
- **`dump ambig-relocs`** — dumps ambiguous relocations. This is directly adjacent to a
  standing failure mode in memory (*relocated words are wildcards* — a byte match can be
  a fakematch on call targets). Cheap to run, read-only, and it may enumerate exactly
  the words where our byte gate is blind. **Highest value-per-minute item in this note.**
  [medium — the mapping to our fakematch class is inferred, not confirmed]
- `apply` / `import symbols` — pull symbol properties/names from a built ELF back into
  `symbols.txt`. Careful: these *write* config, so they collide with the attribution and
  rename hazards in memory. Not to be run casually. [high]
- `format` — sorts config `.txt` files. Would produce a large diff for no byte change.

**G3.3 — Do not chase 0.12.0 yet.** [high] dsd is our byte gate; a version bump changes
delink/lcf output and revalidating 106/106 is the cost. 0.12.0's headline items (DS
Protect decryption, `.exceptix` C++ exception sections, reloc addend hints, weak symbols,
per-module delink/dis flags) buy us nothing we're currently blocked on. The one reason
to bump would be pairing with dsd-ghidra v0.7.0 — and v0.6.0 removes that reason.

---

## 4. `ds-decomp` PR #37 — what it would give us

<https://github.com/AetiasHax/ds-decomp/pull/37> — "[Draft] Add function signatures",
opened 2025-07-17 by Yanis002, **still a draft, unmerged, no reviews**, 5 commits.

**G4.1 — dsd ships two signatures today.** [high] `dsd sig list` on our 0.11.0 binary
prints exactly `FS_LoadOverlay` and `FS_UnloadOverlay`. The machinery
(`sig new`, `sig new-elf`, `sig apply -a/--dry`) is complete; the **database is empty**.

**G4.2 — PR #37 is the database.** [high] 43 new `assets/signatures/*.yaml` plus 37 lines
registering them in `cli/src/analysis/signature.rs`:

- CodeWarrior/MSL runtime (9): `_dadd _ddiv _dflt _dmul _dsub _fflt _ll_udiv
  _s32_div_f _u32_div_f`
- Nitro GX/GXS (13): `GX_Init`, the `GX_SetBankFor*` family, `GX_DispOff`,
  `GX_HBlankIntr`, `GX_VBlankIntr`, `GX_SetGraphicsMode`, `GXS_SetGraphicsMode`, …
- C++ runtime (6): `__cxa_vec_ctor`, `__cxa_vec_cleanup`, `__register_global_object`,
  `__destroy_global_chain`, `__assert_failed`, `__memset_impl`
- OS (4): `OS_Init`, `OS_InitAlarm`, `OS_InitTick`, `OS_WaitVBlankIntr`
- Peripherals (5) + FS (1): `FX_Init`, `TP_Init`, `RTC_Init`, `SND_Init`,
  `PM_GoSleepMode`, `FS_Init`

**G4.3 — The target it would land on is 7,531 placeholder functions.** [high] Census of
`config/**/symbols.txt` on this branch: **11,394** `kind:function` symbols, of which
**7,531 (66%)** are still `func_*`/`sub_*` and **3,863** are named. Every one of these 43
that hits is a free, ROM-evidenced name — and SM64DS is a Nitro SDK + CodeWarrior title,
so these are precisely the functions we'd expect to be present.

**G4.4 — MEASURED: 39 signatures, 4 match, 5 symbols renamed, 3 genuinely new names.**
[high] No Rust build was needed — `sig apply -s <path>` takes a YAML path, so the PR's
files were fetched from `Yanis002/ds-decomp@signatures` (bb810e3) and dry-run one by one
against `config/arm9/config.yaml`. (The PR holds **39** `.yaml` files, not the 43 a
summary suggested.) `git status config` stayed clean throughout — `--dry` writes nothing.

| outcome | count |
|---|---|
| `No matching function found` | **33** |
| matched and would rename | **4** |
| `Multiple matching functions found, cannot apply` | 2 (`FX_Init`, `GX_DisableBankForLCDC`) |

The five renames, against what our config calls those addresses today:

| addr | module | our name now | signature says | verdict |
|---|---|---|---|---|
| `0x020731dc` | ARM9 | `func_020731dc` | `__register_global_object` | **new name** |
| `0x020aa3f0` | ARM9 | `data_020aa3f0` (bss) | `__global_destructor_chain` | **new name** |
| `0x02054430` | ARM9 | `func_02054430` | `GX_SetBankForLCDC` | **new name** |
| `0x01ffa9dc` | ITCM | `__aeabi_uldiv` | `_ll_udiv` | **conflict** |
| `0x02053abc` | ARM9 | `_ZN3GXS15SetGraphicsModeEi` | `GXS_SetGraphicsMode` | **conflict** |

**G4.5 — The transitive-rename multiplier is ~1.25x, not the hoped-for fan-out.** [high]
Only one signature (`__register_global_object`) renamed anything beyond its own function,
and it reached exactly one data symbol. "Related symbols" means symbols the matched
function references through relocations — real, but a small constant, not a cascade.
So PR #37's ceiling against our 7,531 placeholders is **3**, not thousands.

**G4.6 — The two conflicts are worth more than the three new names.** [medium] Both are
places where our config disagrees with an upstream-attested byte-exact signature:

- `0x01ffa9dc` — we call it `__aeabi_uldiv` (the ARM EABI name). CodeWarrior's MSL calls
  the same routine `_ll_udiv`. Same function, and the signature matched byte-exactly, so
  this is a naming-convention disagreement, not an error — but the ROM was built by
  CodeWarrior, so `_ll_udiv` is the name the original source used.
- `0x02053abc` — we call it `_ZN3GXS15SetGraphicsModeEi`, i.e. a C++ method
  `GXS::SetGraphicsMode(int)`. In the Nitro SDK, `GXS_SetGraphicsMode` is a **plain C
  function**. If the signature is right, our mangled name is wrong, and it is wrong in
  the specific way memory warns about (a mangled name raises the langmode ratchet).
  Worth checking against the other 20-odd `_ZN3GX*` names we carry.

**G4.7 — Why 33 missed.** [medium] dsd signature matching is a whole-body bitmask
compare. The PR's signatures were lifted from other DS decomp projects, i.e. other Nitro
SDK versions and other CodeWarrior builds. SM64DS is a 2004 launch title on an early
SDK; a one-instruction codegen difference kills the match. Regenerating signatures
*from* SM64DS (`dsd sig new -c ... -f <name>`) and pushing them upstream is the direction
that would actually pay — we have 3,863 named functions to harvest from.

**G4.8 — Adoption risk if we take the 3 names.** [high] `sig apply` without `--dry`
writes `symbols.txt`. Even a 3-symbol rename is the shape memory flags for attribution
permutation and rename shadowing. Route: bracket `eligible.py`, run `port_refcheck.py`,
and keep the two conflicts out of the same commit as the three additions.

---

## 5. `dsd dump ambig-relocs` — MEASURED

Run against `config/arm9/config.yaml`, read-only, ~seconds. **1,560 ambiguous
relocations** across 483 distinct containing symbols.

By module — this is not evenly spread:

| module | count |
|---|---|
| **overlay 6** | **706** (45%) |
| ARM9 main | 319 |
| overlay 2 | 36 |
| overlay 65 | 35 |
| overlay 22 | 32 |
| …59 more overlays | ≤24 each |

By what contains the ambiguous word:

| container | count |
|---|---|
| data symbols (`data_*`) | 758 |
| **named functions** | **555** |
| **placeholder functions** (`func_*`/`sub_*`) | **247** |

**G5.1 — The mechanism, exactly.** [high] `config/**/relocs.txt` records the resolved
module per relocation. A normal line is unambiguous:

```sh
from:0x020c027c kind:arm_call to:0x0203aed8 module:main
```

An ambiguous one names a *candidate set* — dsd knows the target address but not which
overlay owns it:

```sh
from:0x020e0554 kind:load     to:0x020bc880 module:overlays(0,4)
from:0x02117d4c kind:arm_call to:0x020aea30 module:overlays(2,4)
```

Overlays share address space, so both candidates sit at the same address. The encoded
`bl`/`ldr` word is therefore **identical whichever candidate is correct** — the byte gate
cannot distinguish them, and a wrong choice byte-matches. That is precisely the
*relocated words are wildcards* hazard, now enumerated.

Across all 91,808 relocation entries, **1,560 (1.70%)** are ambiguous: 1,398 `load`,
**162 `arm_call`**. Most are 2-way (1,226), but the tail runs to 22 candidates.

**G5.2 — MEASURED: 135 ambiguous `arm_call` sites inside 85 already-matched functions.**
[high] Cross-referencing the dump against the matched set (`progress.py:synced_from_src`
— config declares it, `src/<name>.c[pp]` exists, no NONMATCHING banner):

| container | `load` | `arm_call` |
|---|---|---|
| data symbols | 758 | 0 |
| **matched functions** | 141 | **135** |
| unmatched functions | 500 | 26 |

The 135 call-site ambiguities are the sharp end: matched C source calling a function
whose identity is genuinely undetermined.

**G5.3 — They collapse to TEN distinct (target, candidate-set) pairs.** [high] This is
the good news — it is not 135 problems:

| target | candidates | sites | matched fns |
|---|---|---|---|
| `0x020aed98` | overlays(2,7) | **57** | 57 |
| `0x020aea30` | overlays(2,4) | 32 | 10 |
| `0x020adc74` | overlays(3,4) | 22 | 3 |
| `0x020ada40` | overlays(2,4) | 15 | 13 |
| `0x020ad660` | overlays(2,3,4,7) | 3 | 3 |
| `0x020efaf0` | overlays(2,6) | 2 | 2 |
| `0x020effb8`, `0x020aa420`, `0x02123804`, `0x020ca78c` | various | 1 each | 1 each |

**Resolving `0x020aed98` alone settles 57 of the 135.** Its callers are the enemy
`*_Spawn` family — `Wiggler_Spawn`, `Koopa_Spawn`, `ChainChomp_Spawn`, `Whomp_Spawn`,
`PiranhaPlant_Spawn`, … — one call each, spread over ~30 overlays, every one of them
already matched and named. It is one shared spawn helper, called the same way everywhere,
that dsd cannot attribute to overlay 2 or overlay 7.

The next three (`0x020aea30`, `0x020adc74`, `0x020ada40`) account for another 69. Four
addresses cover 126 of 135.

**G5.4 — The data side is dominated by two tables.** [high] `data_02090864` alone
accounts for **244** entries and `data_02092208` for 51; the rest of the top-12 are ten
`data_ov006_*` blocks at 21-23 each. A 244-entry ambiguous run at one address is a
function-pointer table, not 244 independent problems.

**G5.5 — Overlay 6 holds 706 of the 1,560, and `overlays(0,4)` alone is 626.** [high]
ov006 is the minigame overlay and the most-worked module in the tree. Its share is one
structural cause, not 706 scattered defects.

**G5.6 — CORRECTION: these are NOT 135 latent fakematches. 60 are provably safe.**
[high] I initially framed every ambiguous call site in a matched function as a possible
fakematch. Checking what the callers actually reference refutes that for 60 of the 135.
`src/game/actors/Wiggler/Wiggler_Spawn.c` declares `extern void _ZN5EnemyC2Ev(void);` and calls it **by
name**. Only ov002 defines that symbol, so mwldarm pins the callee at link time
regardless of what `relocs.txt` says. The ambiguity is dsd's own bookkeeping — it
governs the gap objects dsd supplies for undecompiled code, not a name-resolved call.

Breaking the 135 down by how the caller pins its callee:

| how the matched source refers to the ambiguous target | sites |
|---|---|
| **unique symbol name → link resolves it, safe** | **60** |
| **no candidate name appears in the source at all** | **75** |
| raw address literal | 0 |

Of the 60 safe ones, 57 are `0x020aed98 → ov002 _ZN5EnemyC2Ev` (`Enemy::Enemy()`,
size 0x24). The rival at that address is `func_ov007_020aed98`, size 0x1e0 — a
480-byte function, obviously not a base constructor. **The top ambiguity is settled:
overlay 2, for all 57.** That is consistent with the bounded enemy-subclass family in
`[[enemy-subclass-census]]`.

**G5.7 — The other 75 ARE the phantom-reference worklist, and this explains its cause.**
[high] `src/unnamed/ov006/0211/func_ov006_02115b0c.c:59` declares:

```c
extern void *func_020adc74(void *p);
```

There is no `func_020adc74` in any `symbols.txt`. The real symbols at that address are
`func_ov003_020adc74` and `func_ov004_020adc74`. The author could not tell which overlay
owned the target, so they wrote a **module-less placeholder** — which resolves to
nothing. Checking the ten ambiguous call targets against
`config/unresolved-baseline.json`: **nine of ten are present as module-less phantom
names** (all but `func_020aed98`, which is the one already correctly named).

Reference counts for the module-less form across `src/`: `func_020ada40` in 12 files,
`func_020aea30` in 10, `func_020adc74` in 6 — none defined anywhere.

**This identifies the mechanism behind `[[phantom-references]]`.** It is not random
sloppiness: a phantom `func_0<addr>` is what gets written when a relocation is
overlay-ambiguous. The two worklists are one worklist, and each entry is resolvable by
the same evidence that settled `0x020aed98` — compare the candidates' sizes and names
and pick the one that fits the call site. Per memory, each fix is a free +1.

**G5.8 — Separately: `dsd delink` emits 11 warnings that both build paths suppress.**
[high] `rombuild.py:489` and `tubuild.py:2138` both pass
`quiet_patterns=("No module for relocation",)`. Run unfiltered, `dsd delink` prints
**11** such warnings, all targeting `0x01ff9xxx`-`0x01ffaxxx` (ITCM). This is a *different*
and much smaller issue than the 1,560 above — "no candidate at all" rather than "several
candidates" — but it is being hidden rather than triaged. 11 lines is cheap to just fix
or explicitly bless.

**G5.2 — The data side is dominated by two tables.** [high] `data_02090864` alone
accounts for **244** entries and `data_02092208` for 51; the rest of the top-12 are ten
`data_ov006_*` blocks at 21-23 each. A 244-entry ambiguous run at one address is a
function-pointer table (overlay dispatch, most likely), not 244 independent problems.
Fixing the table's type would collapse a sixth of the whole list.

**G5.3 — Overlay 6's 706 is a concentration, not a diffusion.** [medium] ov006 is the
minigame overlay and is already the most-worked module in the tree. Its ambiguity share
being 45% suggests one structural cause (the `data_ov006_*` table family above) rather
than 706 scattered defects.

Output preserved at `scratchpad/ambig-relocs.txt` (1,560 lines,
`module: 0xADDR -> 0xTARGET (containing_symbol + 0xOFF)`).

---

## 6. Recommended order

1. ~~`dsd dump ambig-relocs`~~ — **done, §5**, including the matched-function
   cross-reference. Outcome: not a fakematch pile; it is the phantom-reference cause.
2. ~~`dsd sig apply --dry`~~ — **done, §4.** Yield is 3 new names; not worth a workstream.
   The two conflicts (G4.6) are worth a look, especially the `_ZN3GX*` naming question.
3. ~~Install JDK 21 + Ghidra 11.2.1 + dsd-ghidra v0.6.0~~ — **done, §7.** Installed and
   confirmed working, but the extension is GUI-only, so the decompiler A/B is blocked
   on a GUI session rather than on the install.
4. **Resolve the ten ambiguous call targets** (G5.6-G5.7). Highest-value item now, and
   it needs no Ghidra: `0x020aed98` is already settled (ov002 `Enemy::Enemy()`, 57
   sites), and the other nine are the same question. Each resolution turns a
   module-less phantom extern into a real symbol across 6-12 files at once.
5. Only if (3) unblocks: re-point `tools/ghidra/DecompDump.java` at a SyncDsd'd project
   and re-enable `cascade.py --scaffold ghidra`.
6. Independent of Ghidra: harvest signatures *out of* SM64DS with `dsd sig new` and
   offer them upstream (G4.7). We have 3,863 named functions; PR #37 has 39 total.

Steps 1, 2 and 4's evidence gathering modified nothing — `git status config` clean
throughout.

---

## 7. The toolchain, as installed

| component | version | location |
|---|---|---|
| Temurin JDK | 21.0.12+8 | `C:\tools\jdk-21.0.12+8` |
| Ghidra | 11.2.1_PUBLIC | `C:\tools\ghidra_11.2.1_PUBLIC` |
| dsd-ghidra | v0.6.0 (`ds-decomp 0.11`) | `%APPDATA%\ghidra\ghidra_11.2.1_PUBLIC\Extensions\dsd-ghidra` |

Nothing was added to `PATH` and no system JDK was replaced — the existing JDK 11 install
is untouched. Ghidra needs `JAVA_HOME=C:\tools\jdk-21.0.12+8` set for the session.

**G7.1 — Extension install location matters.** [high] Unzipping into
`<install>\Extensions\Ghidra\` (the *archive* directory) does not register the module —
`-loader` reports `Invalid loader name`. The working location is the user extensions
directory, `%APPDATA%\ghidra\ghidra_11.2.1_PUBLIC\Extensions\<name>\`.

**G7.2 — `-loader` takes the class simple name, not the loader's display name.** [high]
`DsRomLoader` works; `dsd-ghidra-loader` (what `getName()` returns, and what the log
prints) is rejected by `HeadlessOptions.setLoader`.

**G7.3 — BLOCKER: dsd-ghidra cannot be driven headless. Both halves need the GUI.**
[high] Two separate blocks, one of which is not fixable from outside the extension:

- `SyncDsd.java` unconditionally constructs a Swing `DsdConfigChooser` with no
  argument-based bypass. Fixable — a headless variant taking the config path as a script
  arg is written and installed at
  `…\Extensions\dsd-ghidra\ghidra_scripts\SyncDsdHeadless.java` (a copy of SyncDsd with
  only the dialog block replaced).
- **`DsRomLoader` itself blocks.** `analyzeHeadless … -import sm64.nds -loader
  DsRomLoader -noanalysis` gets as far as `Using Loader: dsd-ghidra-loader` /
  `Using Language/Compiler: ARM:LE:32:v5t:default`, then hangs forever. A `jstack` of the
  stalled JVM shows `main` in `Object.wait()` on `AWTTreeLock` under
  `DockingWindowManager.doShowDialog → Dialog.setVisible` — the loader is waiting on a
  modal dialog that no one can dismiss. 852 s elapsed for 3.4 s of CPU; killed.

So the loader half needs the GUI, or a patched loader (Java sources ship in
`lib/dsd-ghidra-src.zip`, but rebuilding needs Rust + Gradle, neither installed).

**Practical route:** launch the Ghidra GUI, drag `sm64.nds` in, decline auto-analysis,
run SyncDsd, and save the project. After that a SyncDsd'd project *may* be reusable
headless for `DecompDump`-style batch work, since the loader only runs at import time —
untested.

**G7.4 — `DecompDump.java` is not overlay-safe on a dsd-loaded program.** [high] It calls
`toAddr(long)`, which resolves in the *default* address space. The dsd loader gives each
overlay its own Ghidra overlay space, and overlapping overlays share numeric addresses —
the same ambiguity as §5. A bare `toAddr(0x0214b53c)` therefore cannot express which
overlay is meant. Two replacements are written to `tools/ghidra/`:

- `ListBlocks.java` — dumps the memory map (block → space, range, overlay flag).
- `DecompDumpOv.java` — same job as `DecompDump`, but the targets file is
  `<block> <0xADDR> [name]` and addresses resolve inside the named block, with `-` for
  the default space. Emits a TSV of OK/FAIL per target.

---

## 8. Resolving the ten ambiguous call targets

Overlays that share an address range are **mutually exclusive** — only one is resident at
a time. So the candidate is decidable from the *calling* module's own unambiguous
relocations: whichever candidate the caller demonstrably calls elsewhere is the one that
co-resides with it. `relocs.txt` writes `module:overlay(N)` for resolved overlay targets
(39,068 of them) and `module:overlays(N,M,…)` only for the ambiguous 1,560, so the
evidence is already in the tree.

| target | candidates | unambiguous calls from the callers | verdict |
|---|---|---|---|
| `0x020aed98` | 2, 7 | ov002 **1289** / ov007 6 | **ov002** |
| `0x020aea30` | 2, 4 | ov002 **387** / ov004 0 | **ov002** |
| `0x020adc74` | 3, 4 | ov003 0 / ov004 **993** | **ov004** |
| `0x020ada40` | 2, 4 | ov002 **545** / ov004 0 | **ov002** |
| `0x020ad660` | 2,3,4,7 | ov002 **139** / others 0 | **ov002** |
| `0x020efaf0` | 2, 6 | ov002 **104** / ov006 0 | **ov002** |
| `0x020effb8` | 2, 6 | ov002 **148** / ov006 5 | **ov002** |
| `0x020aa420` | 0, 1 | ov000 1 / ov001 **7** | **ov001** (weak) |
| `0x020ca78c` | 2, 6 | ov002 **53** / ov006 0 | **ov002** |
| `0x02123804` | 77,78,79,80 | all zero | **inconclusive** |

**G8.1 — `0x020aed98` is confirmed twice over.** [high] Co-residency gives ov002 1289-to-6,
with 30 of 31 calling modules calling ov002 and nothing else. Independently, the ov002
symbol is `_ZN5EnemyC2Ev` (`Enemy::Enemy()`, size 0x24) and 57 `*_Spawn` callers name it
explicitly, while the ov007 rival is a 0x1e0-byte function. Two independent lines agree.

**G8.2 — `0x020ada40` exposes one wrong reference in a byte-matching file.** [medium-high]
All fifteen call sites live in overlays whose unambiguous calls go **exclusively** to
ov002 — ov062 66/0, ov063 42/0, ov064 69/0, ov065 93/0, ov081 42/0, ov084 65/0, ov090
29/0, ov100 53/0, ov102 86/0. Twelve callers write the phantom `func_020ada40`. One does
not: **`func_ov081_02123910` names `_ZN5Enemy20KillByInvincibleCharERK10Vector3_16R6Player`,
which is the ov004 symbol** — while ov081's own evidence is 42 calls to ov002 and zero to
ov004. The file byte-matches, because both candidates sit at the same address.

Two readings, and choosing between them is a human call:
- the *reference* is wrong and should point at ov002's `func_ov002_020ada40` (size 0x100); or
- ov002 and ov004 are two variants of the same enemy-base code, the *name* is right but
  attached to the ov004 copy, and ov002's copy should carry the name too.

Note the sizes differ — 0x100 (ov002) vs 0xbc (ov004) — which argues against a
straight duplicate and so favours the first reading. Either way, a matched file currently
references a symbol its own module never otherwise calls.

**G8.3 — `0x02123804` stays open.** [high] Its single caller `func_ov002_020ec670` makes
no unambiguous call to any of ov077/078/079/080, so co-residency says nothing. The ov080
candidate is named (`_ZN13MontyMoleRockD0Ev`, size 0x54) and the others are placeholders
of size 0x8 / 0x60 / 0x288. Needs different evidence — a call-shape or runtime check.

**G8.4 — Ghidra independently corroborates the ov006 → ov004 verdict.** [high] See §9:
the SyncDsd'd decompilation of `func_ov006_020dbe9c` names its callee
`func_ov004_020b023c`. That is a third, independent line of evidence for ov006
co-residing with ov004 rather than ov003 — arrived at through the imported relocation
table rather than through the `relocs.txt` histogram.

**G8.5 — Applying these is a symbol-rename change, with the usual hazards.** [high]
Nine resolutions convert module-less phantom externs into real symbols across 6-12 files
each (`func_020ada40` appears in 12 files, `func_020aea30` in 10, `func_020adc74` in 6).
That is squarely `[[symbol-rename-shadowing]]` and `[[attribution-permutation-bug]]`
territory: bracket `eligible.py`, run `port_refcheck.py`, and keep G8.2 in its own commit
since it changes behaviour-relevant meaning rather than just a name.

---

## 9. The A/B: does SyncDsd change draft quality, or only names?

Ran headlessly against the GUI-imported, GUI-synced program — `-process` needs no loader,
so the modal-dialog deadlock of G7.3 does not recur:

```powershell
$env:JAVA_HOME="C:\tools\jdk-21.0.12+8"
analyzeHeadless C:\tools\ghidra_proj sm64ds -process sm64.nds -noanalysis `
  -scriptPath C:\tmp\sm64ds-ghidra\tools\ghidra `
  -postScript DecompDumpOv.java <targets> <outDir>
```

**G9.1 — The sync is essentially complete.** [high] `ListBlocks` reports **486 memory
blocks** and **11,382 functions** against the 11,394 `kind:function` symbols in the dsd
config — a 99.9% import. Overlays land in their own address spaces, and the ambiguity of
§5 is directly visible in the memory map: `arm9_ov002::020ad660` and
`arm9_ov003::020ad660` both exist.

**G9.2 — Names: transformed. Zero `FUN_xxxxxxxx` in any of the three drafts.** [high]
Every callee carries its real dsd name, and C++ symbols come back demangled (the
extension links `cpp_demangle`):

| function | callees in the draft |
|---|---|
| `func_ov102_0214b53c` | `Matrix4x3_FromRotationY`, `MulMat4x3Mat4x3`, `Vec3_Lsl`, `Vec3_LslInPlace`, `IsFrontSliding`, `LostGrabbedObject`, `UpdateCarry`, `func_ov002_020e496c` |
| `OAM::Render` | `GetObjWidth`, `GetObjHeight`, `LoadAffineParams`, `fdiv` — and the function itself comes back as `OAM::Render(...)` with 10 parameters, not `FUN_02020994` |
| `func_ov006_020dbe9c` | `func_ov004_020b023c` — **correctly attributed to ov004** |

This is the whole delta over the old raw-binary path, and it is a real one: a draft that
says `MulMat4x3Mat4x3(...)` tells the LLM tier what the function *is*, where
`FUN_020b1234(...)` tells it nothing.

**G9.3 — Types: unchanged. SyncDsd carries no layout information.** [high]
`func_ov102_0214b53c` still decompiles to 64 `undefined*` types and 67 raw
`*(int *)(param_1 + 0xNN)` field accesses, with the signature `void f(int param_1)`.
That is expected — dsd's config has symbols and relocations, not struct definitions, so
there is nothing for SyncDsd to import. Class layouts would have to come from our own
headers via Ghidra's data-type manager, which nothing currently does.

**G9.4 — The specific historical miss is still missed.** [high] `CLAIMS.md` records
`ov006 func_ov006_020dbe9c` as "Ghidra missed s64 matrix". The SyncDsd'd draft is:

```c
void func_ov006_020dbe9c(int param_1) {
  if (*(char *)(param_1 + 0x51bd) != '\0') {
    func_ov004_020b023c(*(undefined4 *)(DAT_arm9_ov006__020dbf78 +
                          (uint)*(byte *)(param_1 + 0x51be) * 4),
                        *(int *)(param_1 + 0x51a8) >> 0xc,
                        *(int *)(param_1 + 0x51ac) >> 0xc, 0xffffffff);
  }
}
```

Still `int >> 0xc` fixed-point, still no s64 matrix. The correct memory map did not
recover the type — as predicted in G2.3, that miss is a decompiler limitation, not an
addressing one.

**Verdict: SyncDsd buys callee names and overlay attribution, not draft quality.** For
the `cascade.py --scaffold ghidra` use case that is the more valuable half — the LLM tier
can re-derive structure from the disassembly but cannot invent the right callee names —
so re-enabling the scaffold on a SyncDsd'd project is worth doing. It will not, on this
evidence, rescue functions that are hard for reasons of type recovery.

---

## 10. Outcome (applied)

Commit `193fc107` applies §8's nine resolutions: 118 substitutions across 30 `src/`
files, then `enroll.py --complete-list` promoted the newly eligible names.

| gate | before | after |
|---|---|---|
| `eligible.py` | 10854 / 11174 | **10880 / 11174** (+26, 0 removed) |
| `rombuild.py` source-built | 10,854 fns, 88.68% of code bytes | **10,880 fns, 89.75%** |
| reproducing / mismatching | 10,854 / 0 | **10,880 / 0** |
| module fidelity | 106/106 exact | **106/106 exact, 100.000000%** |
| `port_refcheck.py` | — | 393 checked, 0 stale |
| `prepush_attribution.py` | — | 11330 tracked, 0 changed, 0 lost |

**G10.1 — The langmode ratchet fails, and it is not this change.** [high] `--check`
against `origin/chaos-data:langmode-baseline.json` reports
`unmigrated_total 683 -> 688` and `cpp_still_handspelled 138 -> 140`. Stashing every
change and re-running on the clean tree gives the **identical** numbers, so the ratchet
is stuck on main. This is the `[[stale-baseline-gates]]` case; reproduce before owning.

**G10.2 — CORRECTION to G8.2: `func_ov081_02123910` is not a landed fakematch.** [high]
It is `reason: "compile failed"` in the eligibility report —
`func_ov081_02123910.cpp:31: illegal function overloading` under 2004/b56. It is not
eligible, not enrolled, and not in the build, so its reference to the ov004 symbol has
never affected a byte. It is a latent problem in an unbuildable file, which is exactly
the `[[unbuildable-files-invisible]]` class. The finding is still worth keeping — when
that file is made to compile, the reference needs deciding first.

**G10.3 — ov002 is itself an Enemy-base overlay, which explains the whole pattern.**
[high] `_ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj` is defined in
**ov002** at `0x020ad838` (size 0x208). So the enemy behaviour overlays calling into
ov002 is semantically expected, not just statistically supported — the co-residency
verdict and the class structure agree. ov004 holds a parallel, mutually exclusive copy;
only three ov004-resident files plus the unbuildable ov081 one name its
`_ZN5Enemy20KillByInvincibleCharERK10Vector3_16R6Player` at `0x020ada40`.

**G10.4 — What is left of this worklist.** [high] Repo-wide, **213 files are rejected
with missing symbols across 254 distinct names**. This change cleared 30 of those files.
The largest remaining single names:

| missing name | files blocked |
|---|---|
| `func_020beb68` | **25** |
| `_ZTV10dBgActor_c` | 12 |
| `_ll_udiv` | **9** |
| `func_020bc7d4` | 8 |
| `_Z35_ZN5Model8LoadFileER13SharedFilePtrPv` | 6 |
| `_Z77_ZN18MovingMeshCollider7SetFileE…` | 6 |

`func_020beb68` and `func_020bc7d4` are the same module-less shape as the nine just
resolved and should yield to the same co-residency method. `_ll_udiv` connects back to
§4: PR #37's signature matched it byte-exactly at `0x01ffa9dc`, where our config calls it
`__aeabi_uldiv` — so nine files are blocked on a name the upstream signature already
supplies. The `_Z35_…`/`_Z77_…` entries are the `[[double-mangling-defect]]` shape.
