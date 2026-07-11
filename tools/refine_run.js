// Refine-specialist fan-out: one agent per stored near-miss draft from
// nearmiss/db.jsonl, fixing the last few diverging instructions (codegen shape).
// This recovers STRUCTURAL misses the permuter cannot (branch/store order, loop
// form, && chains, arithmetic idiom, load width). Build the worklist first:
//   python tools/refine_wl.py --max-div 6 --limit 20
// Launch:
//   Workflow({ scriptPath: "tools/refine_run.js", args: <names[]> })
//   Workflow({ scriptPath: "tools/refine_run.js", args: {names:[...], model:"fable", effort:"max"} })
// Bank with: python tools/crackloop.py land --output <task.output> --refine
export const meta = {
  name: 'refine-fanout',
  description: 'Refine specialist over progress/wl_refine.jsonl near-miss drafts; self-verifying',
  phases: [{ title: 'refine' }],
}

const A = typeof args === 'string' ? JSON.parse(args) : args
const names = Array.isArray(A) ? A : A.names
const MODEL = (Array.isArray(A) ? null : A.model) || 'sonnet'
const EFFORT = (Array.isArray(A) ? null : A.effort) || 'xhigh'
const WL = (Array.isArray(A) ? null : A.wl) || 'progress/wl_refine.jsonl'

const SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: {
    name: { type: 'string' },
    matched: { type: 'boolean', description: 'true ONLY if abverify printed exactly MATCH' },
    c_source: { type: 'string', description: 'the LOWEST-divergence source you produced (//cpp first line if C++)' },
    attempts: { type: 'integer' },
    divergences: { type: 'integer', description: 'divergence count of c_source per abverify; 0 when matched, 999 if nothing ever compiled' },
    note: { type: 'string', description: 'one line: which structural fix worked, or the wall it hit' },
  },
  required: ['name', 'matched', 'c_source', 'attempts', 'divergences', 'note'],
}

function prompt(name) {
  return `Finish one NEARLY-MATCHED Super Mario 64 DS function: a compiling draft exists that is only a few instructions from byte-matching the retail ROM with mwccarm. Your job is to fix the remaining codegen SHAPE, not re-decompile.

FUNCTION: ${name}

STEP 1 - read the context (annotated target disasm, callee sigs, pool slots, and the stored DRAFT to start from):
  python tools/abrow.py --name ${name} --wl ${WL}

COMPILER: mwccarm 1.2/sp2p3. Flags (C): -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
If the draft starts with //cpp keep that exact first line (C++).

WRITE the draft verbatim to _abwork/${name}.src (mkdir -p _abwork), then verify to see the current gap:
  python tools/abverify.py --name ${name} --src _abwork/${name}.src --wl ${WL}
NOMATCH prints the exact mismatching instructions (target vs yours) then "NOMATCH divergences=N/words". Edit toward zero. Up to 8 attempts, but STOP EARLY if divergences do not improve for 2 consecutive attempts; keep the LOWEST-divergence version in the file and report it.

STRUCTURAL LEVERS (these fix what the permuter cannot; pick by what the diff shows):
- branch/store ORDER inside an if/else: move a store or call into/out of an arm; swap then/else bodies by inverting the condition
- loop form: while() vs do/while (top vs bottom test); for(;;) with break for find-first loops
- early-outs: merge separate "if(x) return K" into one && chain to a shared exit, or split a && chain into guard clauses; new-ctor null checks as if(p){body} return p
- arithmetic idiom: x/K vs (x*magic)>>s, x%K vs x-(x/K)*K, x*K-C vs (x-C/K)*K, negative const via mov+rsb, -1 via mvn
- load width/signedness: u8/s8/u16/s16 local or cast flips ldrb/ldrsb/ldrh/ldrsh
- push-set / frame off by a register: change how many locals straddle calls - reuse a var, re-emit a global/field inline instead of hoisting it, or reorder the top-of-block C89 declarations (mwccarm allocates in DECLARATION ORDER)
- register coloring: *(T*)&G vs G[0] access form; materialize a bool (int b=(x==k); if(b)) for movne/moveq
- two-word copy batching (ld,ld,st,st): int temps + a fake dependency 'dst_a = b ? a : a;' pins the load order without changing the value
- prologue order (vptr load before homing this): compile as //cpp and call through a REAL vtable struct with dummy virtual slots - C fn-ptr casts always home first
- blocked narrowing: reassign before the call ('dh = (s16)(h - dh);' as a statement) to stop copy-propagation folding the cast into the arg
- branch to the SHARED final epilogue from a nested switch: break out of both switches and put the tail in an else
- pointer-induction loop reduced away: try '#pragma opt_strength_reduction off' above the function (this pragma WORKS; scheduling/peephole are ignored)
- DUPLICATED early-exit epilogue (popeq/bxeq repeated instead of a beq to the shared tail): '#pragma optimize_for_size on' above the function
- halfword/byte access at offset >= 0x100 in the target with a materialized base: that is ENCODING-forced - plain '*(short*)(p+0x100) = k' reproduces it; and a base passed as 'this'/call-arg materializes too (Sub *b = &c->sub; b->m(...))
- stack slots optimized away: a volatile array keeps them live
For the full catalogue read notes/pret-idioms.md and notes/mwccarm-codegen.md (sec 6e has the newest levers).

MATERIALIZED BASE (add rX,base,#imm then [rX] where your C folds to [base,#imm]): the LEVER is the u64-mask laundering idiom - *(int *)(((int)base + 0xOFF) & 0xFFFFFFFFFFFFFFFF) - the identity AND blocks offset folding and forces the add. Verified on multiple previously-floor functions. Also check: halfword/byte at offset >= 0x100 materializes from a plain cast; a base the ROM passes to a call materializes via a pointer arg (Sub *b = &c->sub; b->m(...)). If the residual is pure two-word store-EMISSION order, stop and report your best.

CONSTRAINTS: do NOT edit anything under src/ or progress/ or nearmiss/ . Only write under _abwork/ .

Return the structured result. c_source = exact final _abwork/${name}.src . matched=true ONLY if abverify printed MATCH on it.`
}

phase('refine')
const t0 = budget.spent()
const results = (await parallel(names.map((n) => () =>
  agent(prompt(n), { label: `${MODEL}:${n}`, phase: 'refine', schema: SCHEMA, effort: EFFORT, model: MODEL })
))).filter(Boolean)
const tokens = budget.spent() - t0
const landed = results.filter((r) => r.matched)
log(`${MODEL} refined ${landed.length}/${results.length} (${tokens} out tok, ${landed.length ? Math.round(tokens / landed.length) : 0}/landed)`)

return {
  model: MODEL,
  attempted: results.length,
  landed: landed.length,
  landedNames: landed.map((r) => r.name),
  outputTokens: tokens,
  tokensPerLanded: landed.length ? Math.round(tokens / landed.length) : null,
  results: results.map((r) => ({ name: r.name, matched: r.matched, attempts: r.attempts, divergences: r.divergences, note: r.note })),
  sources: Object.fromEntries(landed.filter((r) => r.c_source).map((r) => [r.name, r.c_source])),
  nearMisses: results.filter((r) => !r.matched && r.c_source).map((r) => ({ name: r.name, c_source: r.c_source })),
}
