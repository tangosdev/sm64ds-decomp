# Emulator-assisted matching & labeling plan (melonDS/DeSmuME runtime traces)

## Premise check — what runtime data can and cannot buy

The target asm is already perfect ground truth, so runtime data adds **nothing**
to codegen-floor misses (register coloring, scheduling, value-numbering CSE).
Per nearmiss/db.jsonl (2026-07-14, n=1014):

| divergences | count | nature | emulator value |
|---|---|---|---|
| 1–3   | 30  | pure codegen floor | none |
| 4–10  | 101 | mostly floor | ~none |
| 11–25 | 219 | mixed structural | moderate |
| 26–100| 492 | structurally wrong draft | **high** |
| >100  | 172 | draft is a guess | **high** |

**65% of the miss tail (664 funcs, plus every unmatched func with sim<0.35 and
no draft) fails because the C *shape* is wrong** — unknown arg types, struct
layouts, Fix12/Q-format confusion, virtual-dispatch targets, misread control
flow. Runtime observation constrains all of those. That's the target. Secondary
payoff: naming/labeling matched-but-`func_*` code from real call graphs and
argument values — pure documentation win at ~zero token cost.

## Tooling

- **melonDS** (recommended): GDB stub in settings (JIT must be off; ARM9/ARM7 on
  separate ports). Best accuracy. Script via `gdb-multiarch` batch mode or
  pygdbmi — no emulator fork needed.
- **DeSmuME** fallback: `--arm9gdb=<port>` straight from the CLI; also has Lua
  scripting for input automation / savestate cycling.
- We already have per-function `addr`, `module`, `size`, `target_hex` in the
  ledger + nearmiss DB → breakpoint lists are free.
- **Overlay gotcha**: overlay funcs (ov0xx) only exist while their overlay is
  resident, and another overlay can map the same address. Gate every breakpoint
  hit with a canary: read 4–8 bytes at `addr`, compare against `target_hex`
  prefix; discard mismatched hits.
- **Coverage gotcha**: a function only traces if gameplay reaches it. Maintain a
  savestate library (title, file select, castle, one course of each type,
  minigames, multiplayer lobby) and a per-func hit-count ledger so we know
  what's still dark.

## Pipeline

### Phase 0 — spike (half a day, zero tokens)
Boot ROM in melonDS with GDB stub, attach, break at 2–3 *matched* functions,
confirm ledger addresses line up and the canary check works. Deliverable:
`tools/trace/gdb_harness.py` (connect, set/clear bps, dump regs+mem on hit).
Kill criterion: if stub throughput is unusable (<~5 hits/sec with bps set),
pivot to a small melonDS fork with an in-process addr-set call logger.

### Phase 1 — trace collector (1–2 days, zero tokens)
`tools/trace/collect.py --wl <worklist>`: for each target func capture N hits:
- entry: r0–r3, first ~8 stack words, lr (caller identity), cpsr
- pointer-looking args (2000000–23fffff / overlay ranges): deref 64–128 bytes
- return: r0/r1, plus re-read of arg-pointed memory to see what was written
- context: overlay set, frame counter
Emit `traces/<module>/<name>.jsonl`. Batch ~50–100 bps per run, cycle
savestates, record hit coverage.

### Phase 2 — cheap enrichment (scripts + Haiku-tier only)
Deterministic passes first: Q-format detection (values ≡ 0 mod 4096 → Fix12;
0x1000-scaled sin/cos tables), struct-offset histogram from deref'd memory,
caller→callee edges, arg-count/`this`-pointer detection. Then a *cheap* model
pass (Haiku/Sonnet-low, ~1–3K tok/func) turns trace+asm into a structured
annotation: proposed signature, arg/field types, semantic name, 2-line behavior
summary. Store as `nearmiss` DB annotations / `traces/annotations.jsonl`.

### Phase 3 — route into the existing loop
- **Refine tier**: inject the annotation block into refine_run.js prompts for
  div≥26 targets. Measure tok/landed vs the 16–23K --high-div baseline; the bet
  is fewer wasted structural guesses, not better permutation.
- **Fan-out tier**: same block in sched_run.js prompts for sim<0.35 large-logic
  bands where even Fable floors today.
- **Labeling tier** (independent, cheapest): sweep already-matched `func_*`
  symbols, propose real names + doc comments from call graph + arg semantics.
  Coordinate naming conventions with Tango before mass-renaming.

### Phase 4 (optional, biggest cost-saver) — behavioral diff gate
Replay captured (args, input memory) against our *draft* C compiled for the
host (or ARM via mwccarm + Unicorn), compare retval + memory writes to the
trace. A draft that fails replay is semantically wrong — reject it **before**
spending fan-out/refine tokens permuting it. This turns "compile+diff asm" from
the only oracle into the *second* oracle, and it's all local compute.

## Economics
- Phases 0–2 are essentially free (local compute + Haiku-tier pennies) vs the
  measured 16–23K tok/landed refine floor and 85–140K+ tok/landed fan-outs.
- Success metric: tok/landed on div≥26 refine batches with annotations vs
  without; and count of sim<0.35 funcs promoted to viable drafts.
- Even if match-rate lift is modest, Phase 3 labeling output is a permanent
  repo-quality gain that no other pipeline provides.

## Order of work
1. Phase 0 spike (melonDS + GDB stub + canary check)
2. Phase 1 collector + savestate library, trace top-100 div≥26 nearmisses
3. Phase 2 deterministic enrichment; eyeball 10 annotations by hand
4. Phase 3 A/B: one annotated vs one unannotated refine batch, compare tok/landed
5. Decide on Phase 4 based on how often drafts fail replay in spot checks
