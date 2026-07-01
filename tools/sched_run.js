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

const SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: {
    name: { type: 'string' },
    matched: { type: 'boolean', description: 'true ONLY if abverify printed exactly MATCH' },
    c_source: { type: 'string', description: 'exact final source (//cpp first line if C++)' },
    attempts: { type: 'integer' },
    note: { type: 'string', description: 'one line: what worked, or the wall it hit' },
  },
  required: ['name', 'matched', 'c_source', 'attempts', 'note'],
}

function prompt(name) {
  return `Match one Super Mario 64 DS function to the retail ROM, byte-for-byte, with mwccarm.

FUNCTION: ${name}

STEP 1 - read the resolved context (annotated disasm, callee sigs, pool slots, and the CLOSEST already-matched sibling(s) by opcode similarity - use the sibling as your scaffold, it was picked because it is structurally near-identical):
  python tools/abrow.py --name ${name}

COMPILER: mwccarm 1.2/sp2p3. Flags (C): -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
If the name starts with _Z (C++ mangled), write C++ and make the FIRST LINE exactly //cpp . Else write C.

WRITE your candidate to _abwork/${name}.src   (run: mkdir -p _abwork)

VERIFY:
  python tools/abverify.py --name ${name} --src _abwork/${name}.src
Prints one of: MATCH | NOMATCH (compiled ...) | COMPILE_FAIL | NO_SUCH_FUNC . Only MATCH counts. Iterate up to 6 attempts.

mwccarm idioms (cheap levers on a byte-diff): register allocation follows source DECLARATION ORDER - permute top-of-block C89 local decls; reuse a var vs a new one to save a reg; plain u32/int over enum for intermediates; keep ternaries as ternaries; mirror the ROM's loop form and branch direction; for an address materialized in a reg (add rX,base,#OFF then load) try void* byte arithmetic (u32)((void*)p+off); materialize a bool (int b=(x==k); if(b)) to force movne/moveq/cmp. Start from the sibling scaffold and edit toward the disasm.

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
  results: results.map((r) => ({ name: r.name, matched: r.matched, attempts: r.attempts, note: r.note })),
  sources: Object.fromEntries(landed.filter((r) => r.c_source).map((r) => [r.name, r.c_source])),
}
