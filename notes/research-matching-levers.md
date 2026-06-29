# Reclaiming Speed and Success Rate in an mwccarm DS Decompilation at the 63% Hard-Tail

_Deep-research report, 2026-06-29. 5 angles, 20 sources, 95 claims extracted, 25 verified (0 refuted)._

## 1. Executive summary

The single highest-leverage move available is **tooling adoption from the sibling DS/CodeWarrior ecosystem rather than reinventing it**: `dsd` (AetiasHax/ds-decomp) is a DS-specific, mwccarm/mwldarm-native toolkit with `init` auto-config and a `check modules` byte-exact oracle, and `objdiff` natively supports DS ARM with CodeWarrior demangling — both are directly adoptable and would cut wall-clock per landed function (SPEED) while standardizing the verification loop. On the **codegen wall**, none of the surveyed sources produced a documented mwccarm idiom/flag/intrinsic that forces base-address materialization at a zero-offset first access, so that wall remains uncharacterized externally and must be attacked by code-level experiment, not by a known recipe — but the `pokeplatinum` "matching nonmatchings" + `make check` discipline shows the proven community path is per-function hand-iteration against a fast oracle, not a magic flag. On the **LLM-agent pipeline**, the strongest transferable prior art is the *compiler-in-the-loop, in-context* family (SC2Dec/sc²dec, DecLLM) and one real-world matching pipeline (Mizuchi: Claude Sonnet 4.6 + m2c + decomp-permuter + objdiff) reporting **74% on 60 functions with 88% reproducibility** — but every academic number cited (LLM4Decompile 45.4%/18.0%, Idioms 54.4%, SK2Decompile, SC2Dec 52.41%) is **re-executability/functional**, NOT byte-exact, so those headline rates do not transfer to your reloc-aware standard and should not reset expectations. On the **permuter**, the upstream author's own docs **confirm your wall #3 verbatim** (best "when mostly regalloc changes remain"; "neither the scorer nor the randomizer tends to play well with [reorderings or functional changes]"), so the realistic lever is not grinding harder but **adding a custom randomization pass** (e.g., a base-materialization mutation) in code — weights only bias existing passes — and using the laqieer ARM fork as the integration base. Net: pursue SPEED via ecosystem tooling and pipeline architecture borrowed from SC2Dec/Mizuchi, and SUCCESS-RATE via custom permuter passes plus disciplined per-function hand-iteration on the residual walls, while treating all functional-equivalence benchmarks as non-transferable.

## 2. Findings

### Area 1 — Community playbook (DS/CodeWarrior matching decomps)

**F1.1 — `dsd` (ds-decomp) is the DS-native analog of dtk and is directly adoptable.** [high] It is a Nintendo-DS-specific toolkit whose `init` analyzes an extracted ROM and auto-generates config (delinking, symbols, linker-script generation, overlays/autoloads), explicitly aimed at eliminating manual setup. It is real and used in production (DQIX/dqix-decomp) and listed on decomp.wiki. Source: https://github.com/AetiasHax/ds-decomp , https://github.com/DQIX/dqix-decomp , https://decomp.wiki/platforms/nintendo-ds

**F1.2 — `dsd check modules` is a byte-exact (hash-equality) match oracle over arm9 main + autoloads + overlays.** [high] It iterates the main module, every autoload, and every overlay, computes `fxhash::hash64` of the built code, and compares to the base ROM module's stored hash (`--fail` flag for CI). This is a whole-module pass/fail oracle — *coarser* than your existing per-function reloc-aware diff, so it complements rather than replaces it. Source: https://github.com/AetiasHax/ds-decomp (`cli/src/cmd/check/modules.rs`)

**F1.3 — `dsd` targets the exact mwccarm/mwldarm toolchain you use.** [high] Its `lcf` command generates linker command files for `mwldarm`, the Metrowerks ARM linker paired with mwccarm — confirming shared toolchain lineage (caveat: linker ≠ code generator, but the family match holds). Source: https://github.com/AetiasHax/ds-decomp , https://decomp.wiki/compilers/MWCC

**F1.4 — `objdiff` natively supports DS ARM and can be adopted directly to diff arm9/overlay .o files.** [high] Its README lists "ARM (GBA, DS, 3DS)"; it uses the `unarm` disassembler for DS, and `ds-decomp` can auto-generate objdiff config. Real DS projects are tracked on decomp.dev (e.g., Zelda: Spirit Tracks). DS arm9 = ARM946E/ARMv5TE, exactly objdiff's target. Source: https://github.com/encounter/objdiff , decomp.dev

**F1.5 — `objdiff` includes CodeWarrior symbol demangling.** [high] Lists "GCC, MSVC, CodeWarrior, Itanium." Useful for C++ overlays, but note: SM64DS is largely C, so the high-value objdiff feature here is **reloc-aware ARM instruction diffing**, not demangling. Source: https://github.com/encounter/objdiff

**F1.6 — `pokeplatinum`'s matching gate is `make check` → `OK`, a fully automatable whole-ROM/component SHA-1 oracle.** [high] CONTRIBUTING requires `make check` returns `OK` = "your code builds a matching ROM." Mechanically it runs `sha1sum -c` against stored expected hashes for shared SBIN, per-revision SBIN, filesystem, and ROM. pokeplatinum IS a DS arm9/ARM946E CodeWarrior decomp — same platform class. Caveat: whole-ROM granularity, coarser than your per-function diff; use as a final gate, keep your per-function oracle for iteration. Source: https://github.com/pret/pokeplatinum (`CONTRIBUTING.md`, `Makefile`, `meson.build`)

**F1.7 — `dtk` (decomp-toolkit) does NOT support DS/ARM; only its workflow concepts transfer.** [high] dtk is strictly GameCube/Wii PowerPC. Its key idea — write split relocatable objects directly with no assembler, swap each for decompiled C, relink, and declare a match when the binary is byte-identical — is the canonical matching workflow, but the *tool* doesn't apply to DS. The DS analog already exists: that's `dsd`. Source: https://github.com/encounter/decomp-toolkit

**F1.8 — `m2c` supports ARM little-endian + the MetroWerks CodeWarrior family, and is matching-oriented.** [high/medium] m2c ("Machine code to C") targets MIPS/PPC/**ARM (little-endian)**, supports the `mwcc` compiler target, and is explicitly designed to find "matching" C that yields byte-identical output — distinguishing it from IDA/Ghidra. CAVEAT [medium]: m2c's *documented* mwcc target is PowerPC (`ppc-mwcc-c`); there is **no documented `arm-mwcc` codegen-idiom path**, and the only named CW-specific behavior is C++ demangling. So treat m2c's ARM+CodeWarrior *fidelity* as **unverified** — it produces a useful starting-point C draft that still requires your match-iteration loop. Source: https://github.com/matt-kempster/m2c

### Area 2 — The codegen wall (base materialization, register pressure)

**F2.1 — No surveyed source supplies a documented mwccarm idiom/flag/pragma/intrinsic that forces base-address materialization at a zero-offset first access.** [high — by absence] Across the academic decompilation literature (Idioms, LLM4Decompile, SK2Decompile, SC2Dec, DecLLM, MCGD/Agent4Decompile) there is **zero** mention of CodeWarrior, mwccarm, ARM register allocation, callee-saved register sets, or base-address materialization (explicitly confirmed for the MCGD paper). Source: https://arxiv.org/abs/2604.23940 and the absence of any contrary source. **Implication:** your wall #1 is not solved by a known recipe in the surveyed corpus; it must be attacked empirically (see Recommendations R7).

**F2.2 — The community's proven mechanism for exactly this class of residual is per-function hand-iteration against a fast diff oracle ("matching nonmatchings"), not a compiler flag.** [high] pokeplatinum's `make check` discipline and dtk's swap-relink-verify loop both encode the same answer: where codegen diverges by a few instructions, the path forward is iterating C source forms by hand against an instant oracle, capturing near-misses, and accepting that some residuals are hand-work. This corroborates your existing routing of ordering-only/materialization near-misses to hand-fix. Source: https://github.com/pret/pokeplatinum , https://github.com/encounter/decomp-toolkit

> Note: No surveyed source documents a *controllable-from-C* lever for the callee-saved-register-set / register-pressure reproduction problem (wall #2) either. This remains an open, project-specific characterization task.

### Area 3 — LLM-agent pipeline (2024–2026 state of the art)

**F3.1 — A real LLM matching pipeline architecturally identical to yours hit 74% on 60 functions with 88% reproducibility.** [high] Mizuchi = Claude Sonnet 4.6 + m2c + decomp-permuter + objdiff oracle, tested on Sonic Advance 3 (agbcc/Thumb) and Animal Forest (IDO/N64), 6 runs, average 74% match (best 80%), and 53/60 (88%) identical across repeated runs. **RELEVANCE FLAG:** this is GBA-Thumb/agbcc and N64/IDO at easy/medium/hard mix — NOT mwccarm DS at the 63% hard-tail — so 74% is *not* a transferable target for your remaining material. The architecture validation transfers; the headline rate does not. Corroborating "long-tail plateau ~75%" blog: blog.chrislewis.au. Source: https://macabeus.medium.com/can-llms-really-do-matching-decompilation-i-tested-60-functions-to-find-out-4e39b0ae4288

**F3.2 — SC2Dec/sc²dec is direct prior art for a fine-tuning-free, compiler-in-the-loop in-context pipeline.** [high] It recompiles the LLM's own decompilation output to build (assembly, C) pairs used as in-context examples for the next attempt — the same LLM+compiler feedback architecture as your loop (mechanism differs: SC2Dec builds *demonstrations*; yours is an accept/reject *verification* gate). Reported 52.41% **Re-Executability** (+3.90pp SOTA, mid-2024) — functional, not byte-exact, and since surpassed. Source: https://arxiv.org/abs/2406.17233 , https://github.com/AlongWY/sccdec , EMNLP 2024 Findings

**F3.3 — DecLLM validates the compiler+runtime oracle feedback-loop architecture.** [high] An iterative LLM repair loop using static recompilation AND dynamic runtime feedback as oracles lifts off-the-shelf LLM outputs to ~70% recompilation success (70/100 previously non-recompilable become recompilable, GPT-3.5/4). **Bar is recompilability, NOT byte-exact** — validates architecture only; does not address your codegen walls. Source: https://dl.acm.org/doi/10.1145/3728958 (ISSTA 2025 / PACMSE)

**F3.4 — Every fine-tuned/benchmark LLM-decompiler number is functional, not byte-exact.** [high] LLM4Decompile-End-6.7B: 45.4% (HumanEval) / 18.0% (ExeBench) **re-executability**; relies on SFT over 15B tokens (1.3B–33B params), no in-context, no inference-time compiler loop — orthogonal to your in-context+oracle agent loop. Idioms: 54.4% on ExeBench (> LLM4Decompile 46.3% > Nova 37.5%) — functional/I/O equivalence, "unsound, optimistic." SK2Decompile: optimizes re-executability + readable identifiers, not byte-exactness. MCGD/Agent4Decompile: 84–97% re-executability on x86-64 GCC/ExeBench — re-executability, explicitly states "syntactic equivalence … rarely achievable." **None of these success metrics map onto a reloc-aware byte-exact standard; do not use them to calibrate expectations or to choose a fine-tuned model.** Sources: https://arxiv.org/html/2403.05286v3 , https://arxiv.org/abs/2502.04536 , https://arxiv.org/pdf/2509.22114 , https://arxiv.org/abs/2604.23940

### Area 4 — Permuter leverage

**F4.1 — The permuter's documented sweet spot is exactly your wall #3; upstream confirms it cannot reliably flip ordering/structural gaps.** [high] README: "generally best towards the end, when mostly regalloc changes remain"; and "If there are reorderings or functional changes, it's often easy to resolve those by hand, and neither the scorer nor the randomizer tends to play well with them." This is the tool author saying what your empirics already showed (~8k iters on func_0203faa8 and func_ov004_020b7e38, reorder penalty never dropped). Source: https://github.com/simonlindholm/decomp-permuter

**F4.2 — The scorer diffs objdump'd .o files with graded penalties (same / same-instruction / reordered / no-match); stack positions ignored unless `--stack-diffs`.** [high] This constrains what gaps it can close and dictates ARM setup. Source: https://github.com/laqieer/decomp-permuter-arm , `scorer.py`

**F4.3 — Randomization passes have per-pass tunable weights (`default_weights.toml`, per-compiler `[mwcc]`/`[ido]`/`[gcc]` tables; overridable in project `settings.toml`).** [high] e.g., the `[mwcc]` table lowers chain-assignment and struct-reference passes to 0.5. **Critical limit:** weights only adjust the *frequency of existing* passes — they cannot *add* a base-materialization pass. Adding a new pass is a code contribution (write it in `randomizer.py`, then weight it). Source: https://github.com/simonlindholm/decomp-permuter (`default_weights.toml`)

**F4.4 — The laqieer ARM fork targets ARM/Thumb from embedded ARM compilers but documents NO mwccarm/CodeWarrior-specific support.** [high] Architecture-relevant (DS = ARMv5TE), but its objdump-based scoring is tuned for GBA/GCC (devkitARM/agbcc) and would need adaptation for mwccarm. Source: https://github.com/laqieer/decomp-permuter-arm , https://laqieer.github.io/reverse-engineering.html

**F4.5 — The randomizer DOES include some structural mutations** (`perm_ins_block` wrapping ranges in `if(1){}`/`do{}while(0)`, `perm_condition` rewriting conditionals, `PERM_LINESWAP`/`perm_reorder_stmts`). [high] So it is not literally zero-structural — but the author's guidance and your empirics agree it doesn't reliably *resolve* ordering/structural residuals. This is the seam where a *custom* pass could be added. Source: https://github.com/simonlindholm/decomp-permuter (`src/randomizer.py`)

## 3. Prioritized recommendations

Ordered by expected payoff.

1. **Adopt `objdiff` as the standard per-function ARM diff/oracle UI and wire it into the agent loop.** — **BOTH** — effort **low**. Native DS-ARM support (F1.4) plus CodeWarrior demangling (F1.5) gives a battle-tested, reloc-aware visual+machine diff your subagents can consume directly, replacing/augmenting bespoke diff tooling and tightening the feedback loop. SPEED (faster iteration, less custom code to maintain) and SUCCESS-RATE (better near-miss diagnosis). Sources: https://github.com/encounter/objdiff

2. **Stand up `dsd` (ds-decomp) alongside your pipeline for `init` auto-config, `lcf` generation, and `check modules` as a CI gate.** — **SPEED** (primarily) — effort **med**. It is the DS-native, mwccarm/mwldarm-aware analog of dtk (F1.1–F1.3), eliminates manual setup, and gives a whole-module byte-exact CI gate (F1.2) that backstops your per-function oracle. Keep your finer per-function reloc-aware diff for iteration; use `check modules` as the merge gate. Sources: https://github.com/AetiasHax/ds-decomp

3. **Re-architect the agent loop toward SC2Dec-style self-constructed in-context examples + DecLLM-style iterative repair.** — **BOTH** — effort **med**. Instead of pure cold attempts, feed each subagent recompiled-and-diffed (asm, C) pairs from its own prior near-misses and from your 1,244-entry near-miss DB as few-shot demonstrations (F3.2), and structure attempts as iterative repair rounds gated by the oracle (F3.3). Targets your 25%/88K-token economics: in-context exemplars raise first-pass hit rate (SUCCESS) and reduce wasted exploratory tokens (SPEED). Calibrate against the **74%/88%-reproducibility Mizuchi** result as the architecture proof point (F3.1) — not as a target number for your hard-tail. Sources: https://arxiv.org/abs/2406.17233 , https://dl.acm.org/doi/10.1145/3728958 , macabeus Medium

4. **Mine pokeplatinum/pokediamond/pokeheartgold for "matching nonmatchings" idioms and CodeWarrior source patterns, and import their conventions.** — **SUCCESS-RATE** — effort **med**. These are same-platform (DS arm9/ARM946E, mwccarm) decomps that already crossed the hard tail; their resolved-nonmatching commits encode the exact C-form tricks for register pressure and address materialization that the academic literature lacks (F2.2, F1.6). Diff their `NONMATCHING`/resolved functions for materialization and callee-saved patterns. Source: https://github.com/pret/pokeplatinum

5. **Write a custom `decomp-permuter` randomization pass that materializes a base pointer into a temp register; add it in code, then weight it via `[mwcc]`.** — **SUCCESS-RATE** — effort **high**. F4.3 confirms weights cannot *add* the pass — you must implement it in `randomizer.py` (e.g., a mutation rewriting `base->field`/`base[0]` first-access forms into an explicit `T *p = (T*)((char*)base + OFF); ... *p`), then bias it in `[mwcc]`. Build on the laqieer ARM fork (F4.4) but expect to adapt its objdump scoring for mwccarm. This is the only surveyed path that even *attempts* wall #1 mechanically; validate on your 77 entries that are 1–4 instructions off. Sources: https://github.com/simonlindholm/decomp-permuter , https://github.com/laqieer/decomp-permuter-arm

6. **Use `m2c` (ARM little-endian, `mwcc` target) to generate first-draft C for untried functions, feeding the agent loop.** — **SPEED** — effort **low/med**. m2c is matching-oriented (F1.8) and produces a structured starting point that cuts the LLM's cold-start token cost. **Test, don't assume**, its ARM+CodeWarrior idiom fidelity — the documented mwcc path is PowerPC, so treat ARM output as a draft to refine, not a match. Source: https://github.com/matt-kempster/m2c

7. **Run a focused empirical sweep for a zero-offset base-materialization trigger, since no external recipe exists.** — **SUCCESS-RATE** — effort **med/high**. F2.1 means wall #1 has no known solution in the surveyed corpus — so treat it as a controlled mwccarm experiment: sweep source forms (e.g., escaping the base through a `volatile` *pointer-to-pointer*, an opaque/`asm`-laundered identity function, taking `&base[0]` through a separately-compiled helper, or `#pragma` variants across sp2/sp3/sp4 and 2.0) specifically at the *first* access, scored by objdiff. Capture every form as a near-miss. This is also where mining pokeplatinum (R4) should be cross-checked. Sources: project-internal + https://github.com/pret/pokeplatinum

8. **Stop using functional-equivalence benchmark rates to calibrate expectations or model choice; prefer in-context + oracle over fine-tuned decompilers.** — **SPEED** (avoids a wasted fine-tuning detour) — effort **low**. F3.4 shows LLM4Decompile/Idioms/SK2Decompile/MCGD all measure re-executability, orthogonal to byte-exact matching; LLM4Decompile is single-pass SFT with no inference-time oracle. Your in-context Claude + compile/diff oracle is the right paradigm; don't pivot to fine-tuning a decompiler model on these grounds. Sources: https://arxiv.org/html/2403.05286v3 , https://arxiv.org/abs/2502.04536 , https://arxiv.org/pdf/2509.22114

## 4. Flags against the asker's assumptions

- **Wall #3 (permuter cannot flip ordering/structural gaps) — CONFIRMED, not contradicted.** Upstream README states it verbatim (F4.1, F4.2). No re-test needed; this is settled. The only new lever is *adding a pass in code* (R5), not tuning.
- **Wall #1 (base-address materialization) — NO external contradiction found, but NO external solution either.** [Flag for re-test via experiment, not via a cited recipe.] The surveyed corpus is silent (F2.1). The *only* place a counter-recipe is likely to exist is the sibling pret DS decomps' resolved-nonmatching history (R4) — re-test by mining those before concluding the wall is absolute.
- **Wall #2 (register-pressure / callee-saved-set reproduction) — UNADDRESSED by any source.** No surveyed tool or paper documents a from-C lever for controlling the callee-saved set. Treat as an open project-specific characterization task; pokeplatinum mining (R4) is the most likely external source of patterns.
- **m2c "ARM + CodeWarrior" capability — partial overreach flag.** [Re-test] m2c's documented mwcc codegen path is PowerPC; ARM+mwcc idiom fidelity is unverified (F1.8). Do not assume parity with the ppc-mwcc path — measure it on a sample before relying on m2c drafts for hard functions.
- **LLM benchmark headline rates (74%, 54.4%, 52.41%, ~70%) — do NOT transfer.** [Calibration flag] All are functional/re-executability or recompilability on different toolchains/difficulty regimes, not reloc-aware byte-exact on mwccarm hard-tail (F3.1, F3.4). Using them as success targets would set false expectations.

## 5. Caveats

- **Functional vs. byte-exact is the recurring trap.** Every academic number in this report (LLM4Decompile, Idioms, Nova, SK2Decompile, SC2Dec, DecLLM, MCGD) measures re-executability/recompilability, explicitly weaker than your standard. They validate *architecture*, never *rates*.
- **The 74%/88% Mizuchi figures are self-reported by the tool's own author**, n=60, two games, with mid-test API degradation and a 12-attempt cap (some functions historically need up to 87 attempts). Real and correctly cited, but small-sample and off-toolchain.
- **`dsd check modules` and pokeplatinum `make check` are whole-module/whole-ROM** SHA-1/hash gates — coarser than your per-function reloc-aware diff. Adopt as final/CI gates, not as iteration oracles.
- **Time-sensitivity:** the LLM-decompilation field moves fast (SC2Dec's mid-2024 SOTA already surpassed; Idioms is NDSS 2026). Re-survey before any fine-tuning investment.
- **One vote-split claim:** F1.6 (pokeplatinum gate) and F1.8/F3.2-adjacent mwcc-on-ARM inference (m2c) carried a dissenting vote on the *inference clause* (codegen-idiom tuning), not the core fact — hence the "test, don't assume" framing.
- **The permuter ARM fork's scoring is GBA/GCC-tuned** (objdump-based), so any custom pass work (R5) carries an mwccarm-adaptation cost on top of writing the pass itself.

## 6. Open questions

1. **Do the pret DS decomps (pokeplatinum/pokediamond/pokeheartgold) contain a resolved-nonmatching C idiom that forces zero-offset base materialization under mwccarm?** This is the single most likely external source of a wall-#1 recipe and was not directly inspected in this survey.
2. **Can mwccarm's callee-saved-register-set / register-pressure be made controllable from C** via any documented pragma, `register` hint, or `volatile`/aliasing trick — and do the pret decomps demonstrate one? No source addressed wall #2.
3. **What is m2c's *actual* match quality on mwccarm DS ARM** versus its documented PowerPC mwcc path, on a representative sample of your remaining functions?
4. **Would a SC2Dec-style self-constructed in-context exemplar set, drawn from your near-miss DB, measurably move the 25%/88K-token economics** on the smallest-untried band — and by how much?
5. **Can a custom permuter "materialize base into temp" pass actually close any of the 77 near-misses that are 1–4 instructions off**, or does the mwccarm scorer/regalloc interaction defeat it the way the ordering passes were defeated?

## Sources fetched (20)
- https://github.com/AetiasHax/ds-decomp [primary]
- https://github.com/pret/pokeplatinum [primary]
- https://github.com/encounter/objdiff [primary]
- https://decomp-academy.dev/ [secondary]
- https://github.com/encounter/decomp-toolkit [primary]
- https://decomp.wiki/platforms/nintendo-ds [secondary]
- https://wiki.decomp.dev/compilers/MWCC [secondary]
- https://github.com/matt-kempster/m2c [primary]
- https://gcc.gnu.org/onlinedocs/gcc/Explicit-Register-Variables.html [primary]
- https://arxiv.org/abs/2604.23940 [primary]
- https://dl.acm.org/doi/10.1145/3728958 [primary]
- https://arxiv.org/abs/2502.04536 [primary]
- https://arxiv.org/html/2403.05286v3 [primary]
- https://arxiv.org/abs/2406.17233 [primary]
- https://arxiv.org/pdf/2509.22114 [primary]
- https://github.com/simonlindholm/decomp-permuter [primary]
- https://github.com/laqieer/decomp-permuter-arm [primary]
- https://macabeus.medium.com/can-llms-really-do-matching-decompilation-i-tested-60-functions-to-find-out-4e39b0ae4288 [primary]
- https://botw.link/contribute/how-to-decompile [primary]
- https://decomp.wiki/compilers/MWCC [secondary]
