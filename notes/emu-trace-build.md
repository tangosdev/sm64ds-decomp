# Emu-trace build plan — engineering spec

Companion to `notes/emu-trace-plan.md` (the *why/what*). This is the *how*:
tech decisions, file layout, data schemas, and per-phase acceptance gates so
each step is either provably working or provably dead before the next starts.

Guiding rule from the strategy doc: **Phases 0–2 must stay at ~zero model
credits** (local compute + Haiku-tier pennies). If any phase starts wanting
fan-out tokens to make progress, stop and re-scope — the whole bet is that
runtime facts are cheaper than token-brute-forcing structural guesses.

---

## 0. Fixed facts this build sits on (verified 2026-07-14)

- ROM at repo root: `sm64.nds`.
- `nearmiss/db.jsonl` (n=1014) records carry everything a breakpoint list needs:
  `module` (`arm9` or `ovNNN`), `addr` (absolute hex string e.g. `"0x0211640c"`),
  `name`, `size`, `target_hex` (full function bytes, little-endian hex),
  `divergences`, `c_source`, `source`.
- `addr` is the **absolute runtime address** — no overlay base math needed to set
  a breakpoint. Overlay *residency/aliasing* is still the hazard (§4).
- `config/arm9/config.yaml` maps `ovNNN` ⇄ overlay `id`. Load addresses aren't in
  the snippet we need (addr is absolute), but overlay id is there if we later want
  residency polling.
- Existing loop we plug into: `crackloop.py prep|land`, `tools/sched_run.js`,
  `tools/refine_run.js`. Annotations must land as a block those prompts can inject.

---

## 1. Tech-stack decision (make these before writing code)

| Decision | Recommendation | Rationale / fallback |
|---|---|---|
| Emulator | **melonDS** (Qt frontend, GDB stub, `JIT_Enable=0`) | Accuracy for trace fidelity. DeSmuME `--arm9gdb=<port>` is the fallback and keeps Lua input-automation if savestate cycling gets painful. |
| Stub transport | **Direct RSP over TCP socket** (our own tiny client) | No `gdb-multiarch` dependency (painful on Windows). RSP is a small protocol: `$<payload>#<cksum>`, ack `+`. Commands we need: `Z0`/`z0`, `g`, `p`, `m`, `c`, `?`, `D`. `pygdbmi`+gdb is the fallback if stub quirks fight us. |
| Language | Python 3 (matches all existing `tools/*.py`) | Reuse `ledger.py`, `nearmiss_db.py`, config parsing. |
| Phase-4 replay engine | **Unicorn (ARM)** running mwccarm-compiled draft | Deferred; only if spot-checks show drafts fail replay often enough to be worth a gate. |

**melonDS config to set once** (document exact keys in `tools/trace/README.md`
during Phase 0 — they vary slightly by version):
`GdbEnabled=1`, `GdbPortARM9=3333`, `GdbPortARM7=3334`, `JIT_Enable=0`.
JIT off is non-negotiable — the stub needs the interpreter core.

---

## 2. File layout (all new code under `tools/trace/`)

```
tools/trace/
  README.md            # exact melonDS setup, register-packet layout, run recipes
  rsp.py               # RSP socket client: connect, set/clear bp, read regs/mem, continue
  gdb_harness.py       # Phase 0 spike: attach, bp at N matched funcs, dump on hit
  bplist.py            # nearmiss/ledger -> breakpoint list (addr, canary bytes, module)
  collect.py           # Phase 1: --wl worklist -> traces/<module>/<name>.jsonl
  savestates/          # .mln library + coverage manifest (NOT committed if large)
    manifest.json      # {slot, title, reaches:[modules/scenes], notes}
  enrich.py            # Phase 2 deterministic passes (Q-format, struct histo, callgraph)
  annotate.py          # Phase 2 model pass (Haiku/Sonnet-low) -> annotations.jsonl
  replay.py            # Phase 4 (optional): Unicorn replay gate
traces/                # OUTPUT, gitignored: raw + enriched trace data
  <module>/<name>.jsonl
  annotations.jsonl
  coverage.jsonl       # per-func hit counts across savestates (dark-code ledger)
```

Keep `tools/trace/` self-contained; import `ledger`/`nearmiss_db` from `..`.

---

## 3. Data schemas (freeze these before Phase 1)

### 3.1 Breakpoint list (`bplist.py` output, in-memory or `traces/bplist.json`)
```jsonc
{
  "addr": 0x0211640c,          // int
  "name": "func_ov063_0211640c",
  "module": "ov063",
  "size": 672,
  "canary": "f0412de930d04de2" // first 8 bytes of target_hex, hex string
}
```
Canary = `target_hex[:16]`. On every hit, read 8 bytes at `addr`; if they don't
match `canary`, the resident overlay isn't ours — **discard the hit** (§4).

### 3.2 Raw trace record (`traces/<module>/<name>.jsonl`, one line per hit)
```jsonc
{
  "name": "func_ov063_0211640c",
  "hit": 3,                    // Nth capture of this func
  "canary_ok": true,
  "regs": {                    // captured at ENTRY (bp on first insn)
    "r0": 0x0210a3c4, "r1": 0x00001000, "r2": 3, "r3": 0,
    "sp": 0x027e0f40, "lr": 0x02116210, "pc": 0x0211640c, "cpsr": 0x60000013
  },
  "stack": ["0x...", ...],     // first 8 words at sp
  "args_deref": {              // for each of r0-r3 that looks like a pointer
    "r0": { "addr": 0x0210a3c4, "bytes": "..128 bytes hex.." }
  },
  "caller": "func_ov063_02116210",  // symbol resolved from lr via ledger, or null
  "overlay_set": ["ov063","ov000"], // best-effort; may be [] in Phase 1
  "frame": 51234                    // frame counter if available, else null
}
```
Pointer heuristic: value in `0x02000000–0x023fffff` (main RAM) or a known overlay
range → deref 64–128 bytes. Cap deref count per hit to bound socket traffic.

### 3.3 Return capture (optional, Phase 1.5)
Entry bp gives args cheaply. Return values need a second bp at each `bx lr`/`pop
{pc}` site (parse from `target_hex`), or a temporary bp on `lr` after first hit.
**Defer** — args + input memory already constrain most structural misses. Add a
`ret` object to the record only when the return value is the missing fact.

### 3.4 Annotation block (`traces/annotations.jsonl`, Phase 2 output — the deliverable)
```jsonc
{
  "name": "func_ov063_0211640c",
  "module": "ov063",
  "signature": "void f(Actor *self)",     // proposed
  "arg_types": { "r0": "Actor* (this)" },
  "field_types": [                          // struct-offset -> inferred type
    { "off": 0x5c, "type": "Fix12 x", "evidence": "≡0 mod 4096, 3 hits" }
  ],
  "sem_name": "Actor_UpdateShadowMatrix",   // proposed real name (coordinate w/ Tango)
  "behavior": "2-line summary from trace+asm",
  "callers": ["func_ov063_02116210"],
  "callees": ["Vec3_Asr","Matrix4x3_FromTranslation", ...],
  "confidence": "high|med|low",
  "evidence_hits": 3
}
```
This is exactly the block Phase 3 injects into refine/fanout prompts, and the raw
material for the labeling tier.

---

## 4. The three hazards, with concrete mitigations

1. **Overlay residency / address aliasing.** A bp at `0x0211xxxx` is meaningless
   when ov063 isn't loaded, and another overlay can map the same address.
   - *Mitigation A (always):* canary check on every hit (§3.1). Cheap, catches
     aliasing.
   - *Mitigation B (Phase 1):* pick savestates where the target overlay is
     resident (manifest `reaches`). Group the worklist by overlay so one run =
     one overlay-consistent savestate.
   - *Mitigation C (if needed):* melonDS sw breakpoints patch RAM; an overlay
     reload overwrites the patch. Re-arm all bps on a timer or after detecting an
     overlay swap. Only build this if Phase 1 shows lost breakpoints.

2. **Throughput.** Each hit = `g` + several `m` round-trips on localhost. Kill
   criterion (from strategy doc): <5 hits/sec with bps set → pivot to a small
   melonDS fork with an in-process addr-set logger. Mitigate first by: capping
   deref regions per hit, batching mem reads, and **never** bp-ing hot per-frame
   funcs (we target the div≥26 tail, mostly not hot).

3. **Savestate cycling / coverage.** melonDS has no Lua; DeSmuME does.
   - Phase 0/1: **manual** savestate capture into `savestates/`, harness just
     loads-and-collects. Good enough to prove value. Whatever the savestate
     doesn't reach stays dark and gets logged in `coverage.jsonl` — never
     silently missed.
   - If coverage stalls, either (a) hand-record more savestates at the dark
     scenes, or (b) switch that leg to DeSmuME + Lua input scripts.

---

## 5. Phase 0 spike — concrete checklist (½ day, zero tokens)

Goal: prove attach + breakpoint + canary + register/mem dump end-to-end on
**known-matched** functions (ground truth we can sanity-check against).

1. `tools/trace/rsp.py`: TCP client. Methods: `connect(port)`, `send(pkt)` with
   `$…#cksum` framing + `+`/`-` ack, `read_regs()`, `read_mem(addr,len)`,
   `set_bp(addr)`, `clear_bp(addr)`, `cont()`, `wait_stop()`.
2. `tools/trace/gdb_harness.py`: load ROM+savestate manually in melonDS, run
   harness → attach, set bps at **2–3 matched funcs** whose overlay is resident,
   continue, on stop dump regs+8 stack words + canary.
3. Validate against ground truth:
   - **Register-packet layout** — confirm the `g` packet byte-order maps to
     r0…r15,cpsr as expected (classic stub gotcha; document the real layout in
     README).
   - **Canary** — printed 8 bytes == `target_hex[:16]` for the resident func;
     force a miss (bp an address whose overlay is swapped out) and confirm the
     canary rejects it.
   - **pc == addr** on entry.
4. **Throughput probe**: bp a moderately-frequent matched func, count hits/sec.

**Acceptance gate:** canary works both directions, register layout documented,
≥5 hits/sec. **Kill gate:** <5 hits/sec after mitigations → write the fork-logger
spike ticket instead and stop here.

---

## 6. Phase 1 — collector (1–2 days, zero tokens)

- `bplist.py`: read a worklist (names) → join nearmiss/ledger → bp list grouped
  by module/overlay. Sort worklist so one run covers one overlay-consistent
  savestate.
- `collect.py --wl <worklist> [--hits N] [--savestate slot]`: arm ≤50–100 bps,
  run, on each hit capture the §3.2 record (entry regs, stack, pointer derefs,
  caller-from-lr via `ledger`), write `traces/<module>/<name>.jsonl`, bump
  `coverage.jsonl`. Stop a func after N hits (default 3–5) to keep files small
  and move the socket on.
- First real target set: **top-100 div≥26 nearmisses** (the high-value tail).

**Acceptance gate:** ≥60% of the top-100 produce ≥1 canary-clean hit from the
savestate library; the rest logged as dark in `coverage.jsonl` (not silently
dropped). If canary-clean hit-rate is low, the problem is coverage (more
savestates) not the harness.

---

## 7. Phase 2 — enrichment (scripts first, then Haiku-tier)

- `enrich.py` — **deterministic, zero model**:
  - Q-format detector: arg/field values ≡ 0 mod 4096 and in plausible Fix12 range
    → tag `Fix12`; flag 0x1000-scaled values near sin/cos table ranges.
  - Struct-offset histogram: across hits, which offsets in a deref'd `this` are
    read/written, and their value shape (ptr / small-int / Fix12 / angle s16).
  - Callgraph edges: `lr`→caller and disasm-extracted `bl` targets → callee list.
  - `this`-pointer / arg-count detection: is r0 always a stable pointer? do r1–r3
    vary or stay 0 (unused)?
- `annotate.py` — **cheap model pass** (Haiku or Sonnet-low, ~1–3K tok/func):
  feed `{asm/target_hex, c_source draft, enrich.py facts}` → emit the §3.4
  annotation. Batch through the same infra as the loop but at the cheap tier.
- **Hand-eyeball 10 annotations** before trusting the batch (strategy step 3).

**Acceptance gate:** on the 10 hand-checked, deterministic facts are correct
(Q-format/field types match what we know) and ≥50% of proposed signatures are
plausibly better than the current draft's guessed types.

---

## 8. Phase 3 — route into the loop + the A/B that decides everything

The whole plan lives or dies on one measurement. Run it deliberately:

- Pick **one overlay/band** with enough div≥26 nearmisses for two comparable
  batches (≈10–15 funcs each), matched on divergence distribution.
- **Batch A (control):** current `refine_run.js` / `--high-div` prompt, no
  annotations.
- **Batch B (treatment):** identical, with the §3.4 annotation block injected.
- Metric: **tok/landed** (primary) and **match count** (secondary), vs the
  measured 16–23K tok/landed `--high-div` baseline. Also log annotation
  confidence per func so we can see if high-confidence annotations carry the lift.

**Decision:** B beats A on tok/landed by a margin that covers Phase-2 cost →
productionize (auto-enrich the refine tier). No lift → the labeling tier (§9) is
still a permanent win; ship that and shelve the matching bet.

## 9. Labeling tier (independent, cheapest, ship regardless of the A/B)

Sweep already-**matched** `func_*` symbols that have traces → propose real names +
doc comments from callgraph + arg semantics. This is pure repo-quality gain at
Haiku-tier cost and doesn't depend on any match-rate lift.
**Coordinate naming conventions with Tango before any mass rename** (see
`notes/actor-naming.md`).

## 10. Phase 4 — behavioral replay gate (optional, build only if justified)

Trigger to build: Phase 1/3 spot-checks show drafts *frequently* fail a
replay of `(entry args, input memory) → retval + memory writes`. If so:
`replay.py` compiles the draft with mwccarm (or host cc), runs under Unicorn ARM
with the captured input memory, diffs retval + written regions against the trace.
A draft that fails replay is semantically wrong → reject **before** spending
fan-out/refine tokens. Second oracle, all local compute.

---

## 11. Sequencing & effort

| Step | Effort | Tokens | Gate to pass before next |
|---|---|---|---|
| Phase 0 spike | ½ day | 0 | canary both-ways + regs documented + ≥5 hits/s |
| Phase 1 collector + savestates | 1–2 days | 0 | ≥60% of top-100 div≥26 get a clean hit |
| Phase 2 enrich + annotate | 1 day | Haiku pennies | 10 hand-checked annotations hold up |
| Phase 3 A/B | ½ day + 2 batches | refine tokens (measured) | B beats A on tok/landed, or pivot to §9 |
| Phase 4 replay | 1–2 days | 0 | only if drafts fail replay often in spot checks |

**Critical path is Phase 0** — everything downstream is dead if the stub
throughput or register layout doesn't come together, and that's knowable in half
a day. Do not build Phase 1 collector code before the Phase 0 gate passes.

## 12. Open decisions for the user

1. **Commit scope now:** just scaffold+run Phase 0 spike, or scaffold Phases 0–2
   skeleton up front? (Recommend: Phase 0 only — cheap kill signal.)
2. **melonDS vs DeSmuME primary** if you already have one installed/configured.
3. **Savestate capture** — who records the library, and which scenes to
   prioritize (title/file-select/castle/one-of-each-course/minigame/MP lobby).
