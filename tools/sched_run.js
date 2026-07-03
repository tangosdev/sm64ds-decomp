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
KNOWN FLOOR - stop, do not grind: residuals that are pure instruction ORDERING (load/store batching order, cond-move polarity) or register-coloring swaps that survive decl-order permutation; on big spill-heavy functions, the spilled-first-param STACK-SLOT HOMING order ([sp,#0] in ROM vs [sp,#high] in your build, shifting every frame offset) - 15+ source shapes coalesce, do not fight it. Those are documented compiler-floor walls; report the near-miss and finish.
MATERIALIZED BASE is NOT floor until the laundering lever fails BOTH ways: for a target add rX,base,#OFF then [rX] that your build folds into the addressing mode, try *(int*)(((int)base+OFF)&0xFFFFFFFFFFFFFFFF) (routes the address through 64-bit arith so it cannot fold); if that splits into mov+adds, wrap the COMPLETED address instead: ((long long)(int)(base+OFF))&0xFFFFFFFFFFFFFFFFLL. Do not launder inside loops (it hoists as invariant).

mwccarm idioms (cheap levers on a byte-diff): register allocation follows source DECLARATION ORDER - permute top-of-block C89 local decls; reuse a var vs a new one to save a reg; plain u32/int over enum for intermediates; keep ternaries as ternaries; mirror the ROM's loop form and branch direction; for an address materialized in a reg (add rX,base,#OFF then load) use the u64-mask laundering lever above; materialize a bool (int b=(x==k); if(b)) to force movne/moveq/cmp; if the target reloads a value per branch arm that your build CSEs from the if-compare, make the condition's read volatile; a write-only local struct the target emits member-wise: alias it as an address-escaped array so its stores survive DSE (with laundering this can fuse into pre-indexed writeback ldr [rX,#OFF]!); if the target has mov rX,#K then sub where you get rsb, write int v=K; v-=x; instead of v=K-x; for a vtable call NEVER emulate it in C (C hoists the vtable load) - use //cpp and a real virtual call (dummy virtuals to pad the slot), which gives the mov r0,rX; ldr rV,[r0] this-temp shape; on a register-coloring residual that survives decl-order permutation, permute the STORE/statement order at the emission site (compute z,y,x; store x,y,z) before calling floor; in a //cpp file a write-only dead stack object only survives DSE if its type has an inline ctor AND an EMPTY inline dtor (~T(){} - zero code, blocks scalarization); if the BODY matches but the stack frame is too small (dead-local slots the ROM keeps), declare the dead locals volatile and escape them with (void)&x; if the target keeps a callee's return directly in r0 for +offset math but you get mov rX,r0 first, fold the call into a pointer temp's INITIALIZER (T *p = f(...) + k;); unnamed inline expressions color BEFORE named vars (drop a named local to fix a swap), while the converse one-read named local of a global base (char *g=G; then g+0xNNN accesses) lets the compiler CSE a split offset in place; when TWO sites need the same materialized address, vary the laundering SPELLING per site ((int) vs (unsigned) vs completed-address) or the compiler CSEs them back together; for a guard flag, set-then-conditionally-clear (v=1; *p=1; if(cond) v=0;) colors differently than compute-then-store; unsigned char arrays give plain ldrb (signed gives ldrsb) and the dead <0 check survives; a bare if(cond) right after computing cond gets jump-threaded past the ROM's materialized bool - write ok=1/0; if(ok!=0); add an explicit case 0: stub to force jump-table over sequential compares; pass a function's own address as (void*)f, not (int)f, to keep per-use materializations separate. Start from the sibling scaffold and edit toward the disasm.

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
