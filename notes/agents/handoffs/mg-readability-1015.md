# Minigame readability passes: handoff (2026-10-15)

Branch `mg/readability-passes`, based on origin/main `bbdf1582a3`.

Goal (the user's): three passes over the minigame classes for human-readable code
that still byte-matches. Rename with confidence; otherwise record the guess and a
plan to confirm it by playing under melonDS and the GDB stub.

## Status

| Pass | Scope | State |
|---|---|---|
| 1 Names | every unnamed function, virtual, base field and global in ov004/ov005/ov006 | **done** |
| 2 Shape | structural rewrites, each proven by object compare | **done**; `MgSharedState` names carry confidence tags that pass 3 confirms |
| 3 Review and runtime | top-to-bottom read; runtime confirmation of the medium/low names | **first session done** (6 games); win/lose/tie and credits still to observe |

**Pass 1, as committed:**
- **Scene virtuals.** `dScMgBase_c` slots 18-35 carried `dActor_c` names copied by slot index (`OnAimedAtWithEgg` and the rest). All 18 are renamed from their bodies on the base and every override in the 33-class family. The investigation into where those names came from is recorded in names.md.
- **Unnamed helpers.** Of the 758 in the three overlays (174 ov004 + 17 ov005 + 567 ov006), 753 are named. Five stay unnamed for lack of evidence: ov004 `func_ov004_020b67e4` (empty), ov006 `func_ov006_020ca840`, `dScMgBomroom_c::func_ov006_020d8c88`, `func_ov006_020fea70` and `dScMgTeresa_c::func_ov006_0211cc90`.
- **Base fields.** Eighteen `dScMgBase_c` fields are renamed. The compiler located every use, so same-named fields of other structs were left alone.
- **Globals.** The minigame globals are renamed: `gMgScene`, `gMgMenuRow`, `gMgSaveDirty`, the banner, coin and fx pools.
- **Member-pointer records.** The 630 `{fn, adj}` records are now `pmf_<target>`.

**Pass 2** (details and measured negatives in themes.md, themes 1, 7 and 16):
- **`gMgScene`.** It is declared `dScMgBase_c *`, its casts are dropped, and its `char *` aliases and local externs are retired where the code stays identical.
- **Base fields.** Raw base-field reads through `this` aliases and the scene pointer became members. `+0x4650` is named `mTopRecord`.
- **`dMgPsOpt_c`.** `Open`/`Close`/`Update`/`Draw`/`IsIdle` are real members.
- **Teresa.** `SmallBoo mBoos[16]` is typed at 0x4660 and 13 Boo handlers read it by field. Pachinko's mFx/mFlyer/mTarget/mBg are typed (0x5958..0x5c10), with four readers converted. Teresa's mExclaims/mFoundMarks/mRisingIcons (0x48a0..0x4ba0) and the round counters at 0x4c21..0x4c26 are typed too. Teresa 0x4ba0..0x4be8 is now mReveal[1] + mMarker + the big Boo; the supposed overlap was a view starting 0xc bytes into the reveal record. Pachinko 0x5418..0x5958 is never accessed by any reconstructed function, so it stays padding with a note.
- **Not done.** The 13 capped score bumps stay spelled out, because a header inline renumbers the pinned labels.
- **Compare.** The object compare must match the compiler-local `@N`/`$N` names exactly. The manifests pin them.

**Pass 3, first session (2026-10-15):**
- **Setup.** melonDS 1.1 ran the EU ROM with the GDB stub. The watcher armed 47 breakpoints from runtime-checks.md and logged 267 hits, each checked against the overlay's bytes. A screenshot of the window every 1.5 s let the banner text be read off the frames.
- **Games played.** Six, in the user's menu order: Wanted! (four times), the ladder game, the propeller Shy Guy game, Pachinko (the slingshot), Snowball, and Loves Me Not.
- **Seen and recorded.** `MgSharedState` 1, 3, 12, 16, 17 and 18; banners 0, 2, 5, 7, 8, 0xc, 0xd, 0xe, 0x10 and 0x13. Also `StartRound(state)`, `OnNewRecord`, the "new" badge, and Pachinko's Lakitu flyer and flower targets.
- **Rename.** `MG_STATE_CONTINUE_PROMPT` became `MG_STATE_RETRY_PROMPT_WIDE`; the rest of the findings are notes and tags.
- **Where the findings are.** runtime-checks.md "Observed so far" holds them all. names.md and the enum carry the confidence changes.

**Evidence:**
- [notes/minigames/names.md](../../minigames/names.md): every renamed name with its evidence and confidence.
- [notes/minigames/runtime-checks.md](../../minigames/runtime-checks.md): the breakpoint plan.

## Proof (every commit)

**Per commit:**
- All 45 minigame TUs (`config/tu_manifest.d/ov004/`, `config/tu_manifest.d/ov005/` and `config/tu_manifest.d/ov006/` with `src/minigames/` sources) were compiled with `tools/tubuild.py compile`.
- Each object was compared to the pre-pass baseline, either byte for byte or by a semantic ELF compare: every section's bytes, every relocation's offset/type/target and the symbol table, with the recorded rename map applied. Every commit came out identical.

**Gates:** `tools/port_refcheck.py`, `tools/check_decl_agreement.py --changed origin/main`
and `tools/check_rename_ledger.py` pass at HEAD. Lineage rows for the renamed mangled
members are appended to `symbols/actor_renames.tsv`.

**Pending at handoff:** `tools/rombuild.py -j16 --no-rom` (started at handoff) and
`tools/prepush_linkcheck.py --range origin/main..HEAD`. The link check was started
early on and had verified about 2,260 functions across 219 header consumers with no
failure; its WARN lines are pre-existing BLIND sinit/dtor notes outside the
minigames. The `validate` CI on the PR is the authority.

## How the renames were made (to continue the work)

The helper scripts are not in the PR (tooling goes in its own PR). A local copy, with
the baseline objects, is in `build/mgnames-tools/` (gitignored):
- `mgsnap.py snap|cmp <dir> [filter]`: compile the minigame TUs and compare them to a baseline. `objsem.py` is the semantic compare, applying `renames.json` and `methods.json`.
- `mgrename.py symbols <map>`: in-place symbol renames across symbols.txt, the manifests, attribution, function-authors, baselines and `src/` + `include/`. Retired `src/.../func_x.c` paths keep their old spelling.
- `mgrename.py methods <map>` does the same for method names scoped to `dScM*` classes.
- `memren.py`: turns a `{func_addr: Name}` map into member-aware mangled renames.
- `fieldren.py`: the compile-guided member rename.
- `ledger_fix.py`: appends lineage rows for whatever `check_rename_ledger` reports.
- `pmfmap.py` / `gameprep.py` / `fsum.py` / `peek.py`: resolve state tables through relocs and summarize helpers.
- `trykeep.py <rules> <files>`: the Pass 2 harness. It applies a rewrite, compiles, and keeps the rewrite only if the object is identical.

## Next steps

1. **Pass 3, second session (done 2026-10-15).** Played: Slot3, Memory, Picture Poker, Roulette and Sound.
   - **Seen:** shared states 0, 4, 5, 7 and 8; `mCredits` is coins; "bet" fits; `OnDealDone` fires; jingles 1, 3 and 4; and banners 3 "CLEAR" and 6 "PERFECT".
   - **Renamed:** the presenter's `Presenter_PlayAnim1/2/4/7` became `Presenter_PlayMiss/Match/Lose/Cheer`.
   - **Details:** runtime-checks.md "Observed so far" and names.md.
   - **Still open:**
     - shared states 2, 6, 9-11, 13-15 and 19 (Snowball has 19);
     - the `IsAltVariant` rows;
     - Hanachan's Wiggler passes;
     - Trampoline2's items;
     - Teresa's sprites;
     - the graph-callback order.
   - **Setup:** the handoff zip's `HANDOFF.md`, or the steps below.
     1. Start `build/melonds/melonDS.exe build/melonds/sm64_eu.nds`. Check that `[Instance0.Gdb] Enabled = true`, because melonDS can rewrite it to false.
     2. Check `[Instance0.Keyboard]` too. It shipped with every key at -1, which is why keys never reached the game.
     3. Attach `python tools/trace/mg/mgwatch.py --unlock` (setup and gotchas: `tools/trace/mg/README.md`).
   - **Driving the game:** `tools/trace/mg/input.ps1` gives taps, drags and keys. Its foreground unlock uses F24, because an Alt tap opens melonDS's menu bar.
   - **Reading memory:** write `addr len` (both hex) to `build/melonds/PEEK`, and the watcher answers in `PEEK.out`. That is how Sound's chest notes were read.
   - The stub takes one client per launch. A second attach halts the game, so restart melonDS instead of reattaching.
   - **Finding a game:** a game's menu row is its tab + 4 × its position (Mario 0, Luigi 1, Wario 2, Yoshi 3). Rows 24-35 are each tab's second page.
2. **Pass 2 leftovers.** These handlers measured different in the typed form, so they keep their offsets: Teresa StepBoo, CountRubbedTiles, Boo_State7, Reveal_State1/4, ResetRound; Pachinko Fx_Spawn, Bg_State0/1/3, Bg_Start, Flyer_State0/2/3/4. Retry one only with a new spelling idea, one function at a time, with `tryrepl.py`.
3. **Baseline after main merges.** Main's `Heap` → `mHeap::Heap_t` rename changes the minigame objects' `Memory::Deallocate` reference. The local `renames.json` maps it, so compares against the pre-pass baseline stay exact.

There is also one unrelated defect, found while investigating the transplanted
names: ov064 `symbols.txt:111-112` files two `daObjFl_Puzzle_c` vtable slots (32 and
34) under `daWater_Hakidasi_c`.
