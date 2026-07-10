// Coddog-scheduled fan-out: one agent per function, each reads its resolved context
// (tools/abrow.py), writes a candidate, and self-verifies against the ROM
// (tools/abverify.py) until MATCH. Returns matched sources; bank with tools/bank_harvest.py.
//
// Launch:
//   Workflow({ scriptPath: "tools/sched_run.js", args: <names[]> })                    // default Sonnet 5
//   Workflow({ scriptPath: "tools/sched_run.js", args: {names:[...], model:"fable", effort:"high"} })
// args is the trimmed worklist names (progress/wl_ab.jsonl must hold their rows).
export const meta = {
  name: 'coddog-fanout',
  description: 'Coddog-scheduled fan-out over progress/wl_ab.jsonl; self-verifying, model-configurable',
  phases: [{ title: 'fan-out' }],
}

const A = typeof args === 'string' ? JSON.parse(args) : args
const names = Array.isArray(A) ? A : A.names
const MODEL = (Array.isArray(A) ? null : A.model) || 'sonnet'
const EFFORT = (Array.isArray(A) ? null : A.effort) || 'xhigh'
const WL = (Array.isArray(A) ? null : A.wl) || 'progress/wl_ab.jsonl'

const SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: {
    name: { type: 'string' },
    matched: { type: 'boolean', description: 'true ONLY if abverify printed exactly MATCH' },
    c_source: { type: 'string', description: 'the LOWEST-divergence source you produced (//cpp first line if C++)' },
    attempts: { type: 'integer' },
    divergences: { type: 'integer', description: 'divergence count of c_source per abverify; 0 when matched, 999 if nothing ever compiled' },
    note: { type: 'string', description: 'one line: what worked, or the wall it hit' },
  },
  required: ['name', 'matched', 'c_source', 'attempts', 'divergences', 'note'],
}

function prompt(name) {
  return `Match one Super Mario 64 DS function to the retail ROM, byte-for-byte, with mwccarm.

FUNCTION: ${name}

STEP 1 - read the resolved context (annotated disasm, callee sigs, pool slots, and the CLOSEST already-matched sibling(s) by opcode similarity - use the sibling as your scaffold, it was picked because it is structurally near-identical):
  python tools/abrow.py --name ${name} --wl ${WL}

COMPILER: mwccarm 1.2/sp2p3. Flags (C): -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
If the name starts with _Z (C++ mangled), write C++ and make the FIRST LINE exactly //cpp . Else write C.

WRITE your candidate to _abwork/${name}.src   (run: mkdir -p _abwork)

VERIFY:
  python tools/abverify.py --name ${name} --src _abwork/${name}.src --wl ${WL}
MATCH = done. On NOMATCH it prints the exact mismatching instructions (target vs yours) then "NOMATCH divergences=N/words" - fix ONLY those instructions and re-verify. Up to 6 attempts, but STOP EARLY if divergences do not improve for 2 consecutive attempts. Keep whichever attempt had the LOWEST divergences in _abwork/${name}.src (re-write it if a later attempt was worse) and report that one - a close near-miss is banked and valuable, never discard it.
If you are within ~6 divergences and stalled, read notes/pret-idioms.md and notes/mwccarm-codegen.md (full idiom catalogue) before giving up.
KNOWN FLOOR - stop, do not grind: residuals that are pure instruction ORDERING (load/store batching order, cond-move polarity) or register-coloring swaps that survive decl-order permutation. Those are documented compiler-floor walls; report the near-miss and finish. EXCEPTION: if the coloring swap is a cyclic r4/r5-style swap between a reused pointer/mask and a value that comes from a virtual or other indirect call inside the loop, that is NOT floor yet - see the block-scoped-temp lever below.
MATERIALIZED BASE is NOT floor until the laundering lever fails BOTH ways: for a target add rX,base,#OFF then [rX] that your build folds into the addressing mode, try *(int*)(((int)base+OFF)&0xFFFFFFFFFFFFFFFF) (routes the address through 64-bit arith so it cannot fold); if that splits into mov+adds, wrap the COMPLETED address instead: ((long long)(int)(base+OFF))&0xFFFFFFFFFFFFFFFFLL. Do not launder inside loops (it hoists as invariant).

mwccarm idioms (cheap levers on a byte-diff): register allocation follows source DECLARATION ORDER - permute top-of-block C89 local decls; reuse a var vs a new one to save a reg; plain u32/int over enum for intermediates; keep ternaries as ternaries; mirror the ROM's loop form and branch direction; for an address materialized in a reg (add rX,base,#OFF then load) use the u64-mask laundering lever above; materialize a bool (int b=(x==k); if(b)) to force movne/moveq/cmp; if the target reloads a value per branch arm that your build CSEs from the if-compare, make the condition's read volatile; a write-only local struct the target emits member-wise: alias it as an address-escaped array so its stores survive DSE (with laundering this can fuse into pre-indexed writeback ldr [rX,#OFF]!); if the target has mov rX,#K then sub where you get rsb, write int v=K; v-=x; instead of v=K-x; for a vtable call NEVER emulate it in C (C hoists the vtable load) - use //cpp and a real virtual call (dummy virtuals to pad the slot), which gives the mov r0,rX; ldr rV,[r0] this-temp shape; on a register-coloring residual that survives decl-order permutation, permute the STORE/statement order at the emission site (compute z,y,x; store x,y,z) before calling floor; on a loop's cyclic pointer/mask swap fed by a virtual or indirect call result, wrap the call in a fresh scope instead of assigning directly - { int t = o->vN(this); mask = t; } NOT mask = o->vN(this); - the extra live-range boundary flips the allocator's rotation (notes 6i); on a big switch/state-machine function with a shared index (e.g. slot*4) reused across many cases, try #pragma opt_common_subs off (EBB-local CSE, the ROM's per-block rematerialization shape on LARGE functions - notes 6e/6f); if a target \`bics rX,rX,#0\` (x & ~m with m==0) keeps folding to cmp/identity in your build, route the mask through a non-const local or tiny inline helper so selection runs before the m==0 proof (notes 6e). Start from the sibling scaffold and edit toward the disasm.

CONSTRAINTS: do NOT edit anything under src/ or progress/ . Only write under _abwork/ .

Return the structured result. c_source = exact final _abwork/${name}.src . matched=true ONLY if abverify printed MATCH on it.`
}

phase('fan-out')
const t0 = budget.spent()
const results = (await parallel(names.map((n) => () =>
  agent(prompt(n), { label: `${MODEL}:${n}`, phase: 'fan-out', schema: SCHEMA, effort: EFFORT, model: MODEL })
))).filter(Boolean)
const tokens = budget.spent() - t0
const landed = results.filter((r) => r.matched)
log(`${MODEL} landed ${landed.length}/${results.length} (${tokens} out tok, ${landed.length ? Math.round(tokens / landed.length) : 0}/landed)`)

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
