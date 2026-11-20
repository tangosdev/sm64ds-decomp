# Machine residue in `src/game/actors/`, 2026-11-19

A sweep of all 279 files in `src/game/actors/` for source that reads as
machine output rather than as something the EAD team wrote in 2004:
decompiler spill, fake-matching devices, and address-derived naming.

Base: `07ef042f51` (`main`). This note is the audit and the work queue; it
makes no source changes.

## What was swept, and how

Three axes, each reproducible:

1. **Fake-matching devices** — `python tools/fakematch_inventory.py HEAD`,
   then the per-file JSON filtered to `src/game/actors/`. Categories are the
   tool's own (`volatile`, `pragma`, `charthis`, `v3cast`, `launder`,
   `selfthis`, `pmfext`).
2. **Decompiler spill** — grep for m2c/mips2c temporaries
   (`temp_r[0-9]`, `phi_r[0-9]`, `var_r[0-9]`, `unksp`, `sp<hex>`).
3. **Address-derived naming** — `func_<addr>`, `data_<addr>`, `unk_<off>`,
   counted over comment-stripped text.

## Finding 1: no decompiler spill survives

Zero m2c temporaries in the folder. The grep's only hits were `spd`
(speed) locals in `d_a_btfly`, `d_a_c_jugem`, `d_a_popoi`, `d_a_slide_box`,
`d_a_water_ring` — a human abbreviation, matched by the `sp<hex>` pattern
by accident. Nothing to do on this axis.

## Finding 2: the #821 device axis is already fully triaged

21 files carry pattern hits. **All 21 carry an explicit review note**, and
there are no untriaged device sites left in the folder:

| Device | Hits | Files |
| --- | --- | --- |
| `launder` (`*(T *)&x`) | 35 | 15 |
| `v3cast` (`(Vector3 *)&x`) | 15 | 8 |
| `pragma` (per-function optimiser bracket) | 4 | 4 |
| `charthis` (`(char *)this + off`) | 1 | 1 |
| `volatile` | 2 | 2 |

Note spellings in use, which a `Kept (#821` grep alone will under-count:

| Spelling | Count |
| --- | --- |
| `Kept (#821 rN)` | 40 |
| `Kept (#821, measured ...)` | 1 |
| `Kept (#821 rN, measured ...)` | 1 |
| `KEEPER, MEASURED (#821)` | 1 |

Two spot-checks confirm these are real measured keeps, not rubber stamps:

- `d_a_bombking.cpp` — the six `*(VecFx32 *)&...->mPos` snapshots exist
  because the POD copy is a block move (`ldm`/`stm`) while a `Vector3` copy
  is memberwise. Re-measured: the plain spelling grows five `Tick` functions
  by 8 bytes each.
- `d_a_moray.cpp:StateDenWaitMain` — `#pragma opt_propagation off` is
  load-bearing against the cartridge's `0x1c0`; without it, `0x1bc`. The note
  records re-measurement at round 64 and the four functions a file-wide pragma
  would break.

**Conclusion: there is no cleanup work on this axis in actors.** Anyone
planning an #821 round here should pick a different folder.

## Finding 3: the remaining machine residue is naming

This is the whole of the real work in actors. Nobody in 2004 wrote
`func_ov002_020f9468`.

| Residue | Occurrences | Files |
| --- | --- | --- |
| `data_<addr>` | 6,952 | 253 |
| `func_<addr>` | 2,140 | 125 |
| `unk_<off>` / `mUnk*` fields | 100 | 41 |

Deduplicated to distinct symbols, and split by who owns them:

| | Count |
| --- | --- |
| Unique address-derived symbols referenced | 2,235 |
| Defined inside `src/game/actors/` | 1,477 |
| Extern-only (owned by another TU) | 758 |

The 1,477 locally-defined symbols are renameable from inside the folder plus
`config/**/symbols.txt`. The 758 externs are not: renaming one is a
tree-wide campaign in whichever TU owns it, and an actors-local rename of an
extern would just desynchronise the spelling.

Hottest files by distinct symbols touched:

| File | Unique | Locally defined |
| --- | --- | --- |
| `d_a_demo.cpp` | 245 | 229 |
| `d_a_iwante.cpp` | 99 | 44 |
| `d_a_trs.cpp` | 80 | 60 |
| `d_a_door.cpp` | 68 | 46 |
| `d_a_kpa.cpp` | 59 | 48 |
| `d_a_obj_mario_cap.cpp` | 51 | 27 |
| `d_a_c_jugem.cpp` | 50 | 20 |
| `d_a_spd.cpp` | 48 | 28 |
| `d_a_btn.cpp` | 46 | 36 |
| `d_a_propeller_heyho.cpp` | 45 | 29 |
| `d_a_hanachan.cpp` | 52 | 2 |

`d_a_hanachan.cpp` is the shape to avoid: 52 distinct symbols, only 2 of them
its own. It is a consumer of other TUs' unnamed symbols, so almost none of its
residue is fixable here.

## Work queue

Ordered by evidence-per-symbol, best first. A rename is only worth landing
when the new name is *earned* from behaviour; a plausible-sounding guess is
worse than `data_<addr>`, because it asserts something no one verified.

1. **`d_a_demo.cpp`** — 229 of 245 symbols are its own, the highest
   self-contained ratio in the folder. One class, one TU, renames land
   together.
2. **`d_a_trs.cpp` (60/80)**, **`d_a_kpa.cpp` (48/59)**, **`d_a_door.cpp`
   (46/68)** — same shape, smaller.
3. **`d_a_btn.cpp` (36/46)**, **`d_a_propeller_heyho.cpp` (29/45)**,
   **`d_a_spd.cpp` (28/48)**.
4. **The 100 `unk_<off>` fields** — these are shadow-struct offsets, so each
   one named is also a step toward the real class layout. Cheaper per symbol
   than a function rename, since a field's use sites are local.
5. **Not now:** the 758 externs, and files like `d_a_hanachan.cpp` whose
   residue is owned elsewhere.

## Gates for any PR off this queue

A rename changes instruction selection at call sites, so it is matching work,
not a text edit:

- `python tools/rombuild.py -j16 --no-rom` — the byte gate.
- `python tools/prepush_linkcheck.py --range origin/main..HEAD` — a rename is
  exactly the change that produces WRONG-DEST with right bytes.
- `python tools/check_decl_agreement.py --changed origin/main`.
- A symbol rename is all-or-nothing: the definition, every declaration, and
  the `config/**/symbols.txt` row move together or the link silently falls
  back to ROM bytes.

## Where the machine-written code came from

`config/match_provenance.jsonl` records, per matched function, a
`matchProvenance.kind` of `ai` or `human` with the model and harness. It is the
only file in the tree that distinguishes the two: `function-authors.json` is
the credit record and names GitHub accounts, so it cannot corroborate this.

Of its 781 rows, **715 are `ai` and 66 are `human`** — 92% machine-matched.
By model and harness:

| Model | Harness | Matches |
| --- | --- | --- |
| `gpt-5` | codex | 290 |
| `grok-4.5` | grok-build | 185 |
| `claude-opus-5` | claude-code | 90 |
| `claude-opus-4-8` | claude-code | 45 |
| `grok-4.6` | grok-build | 37 |
| `claude-fable-5` | claude-code | 15 |
| (tail: opus-5/tangos-console, opus-5-5, fable/refine, sonnet-5, grok-4.7) | | 43 |

Cross-referenced against the 21 actor files that carry #821 device hits:
**all 14 provenance rows touching those files are `ai`, none `human`**
(`trs` 7, `king_donketu` 2, `obj_mario_cap` 2, `demo` 2, `bombking` 1).

So the fake-matching devices in this folder did concentrate in AI-matched
work — and every one of them has since been caught, measured and documented by
the #821 rounds, which is why the device sweep above comes back empty. The
review layer is the thing that worked, not the matching.

Two limits on that reading, both real: 781 provenance rows against 11,388
functions in `function-authors.json` makes this a sample, not a census; and a
file-name substring match is how the 14 rows were attributed, so it catches
rows whose `srcPath`/`label`/`name` mentions the actor and misses any that do
not.

## Campaign progress, 2026-11-19

30 symbols named in `daDemo_c`, in four stacked PRs (merge in this order):

| PR | Symbols | What |
| --- | --- | --- |
| #5703 | 7 | script handlers (`PlaceFromRecord`, `UpdateDescent`, ...) |
| #5704 | 8 | short handlers (`Despawn`, `SetOpacity`, sound trio, ...) |
| #5706 | 10 | scroll + staff-roll lifecycles and cue entry points |
| #5709 | 5 | scroll helpers (`StepFade`, `ScrollBackgrounds`, ...) |

Per-symbol gates on every one: `tools/match.py` against `2004/b56`,
`tools/prepush_linkcheck.py` at 0 blocking with each renamed symbol
`VERIFIED`, `tools/check_rename_ledger.py`, and
`tools/check_decl_agreement.py --changed`.

The `0x2f` scroll path now reads end to end: `StartScroll` -> `CreateScroll`,
`UpdateScroll` driving `StepFade` / `StepSprites` / `RespawnSprites` /
`DrawCenterSprites` / `ScrollBackgrounds`, then `FreeScroll`.

**Six handlers in `d_a_demo.cpp` are deliberately left address-named**:
`6c60`, `7410`, `7538`, `7780`, `79c0`, `7bb8` — camera work, view-space
interpolation and the two-spline flyby. Their behaviour is intricate enough
that a name would be a guess, and a confident wrong name costs more than
`func_ov002_020f7780` does: it asserts something no one verified. They need
real analysis first.

## Two tooling notes for whoever picks this up

The port reference check **no longer exists** in this tree, although
`AGENTS.md` still lists it as required before a rename or file move. The port
itself moved to its own repository (#5633), so a rename here is checked by
that repository's build, not by a script in this one. Symbol renames like the
ones above do not move files; a file move still needs the port repository's
build to confirm nothing it points at went missing.

`python tools/rombuild.py -j16 --no-rom`, the local byte gate `AGENTS.md`
prescribes, **exits 1 on clean `main`** in this workspace. Measured over three
runs: it fails with and without `--no-rom`, and warm as well as cold, always
producing strict-control ROM SHA `ddab9300...` against the admitted bootstrap
proof `d1506e90...` with `matchesStockRom` never `True` — both halves of the
condition at `tools/tu_production.py:222`. Byte fidelity in those same runs is
perfect: 106/106 modules exact, 3,029 functions reproducing, 0 mismatching. So
the failure is the provenance assertion about the control's own ROM, not a byte
mismatch. Whether it is workspace-specific is unresolved, because no CI
workflow runs `rombuild.py` — the merge gate is the external `PR validation`
service — so there is no second data point. Until it is settled, per-function
`tools/match.py` plus `prepush_linkcheck` is the gate that actually works.
