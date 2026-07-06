// One-off Fable crack of func_ov006_02115b0c (ov006, 0x18bc, 1583 insns) seeded
// from ruspecial's PR #104 draft. The draft is +140 bytes off: the ROM uses NO
// frame pointer, but the current build emits `mov fp, sp`, which (with an elided
// early bl) cascades into whole-function register skew (1649 divergences).
// Worklist row (correct target_hex + PR draft) is progress/wl_pr104.jsonl.
// Launch: Workflow({ scriptPath: "tools/crack_pr104.js", args: { n: 3 } })
export const meta = {
  name: 'crack-pr104',
  description: 'Fable max-effort crack of func_ov006_02115b0c from PR #104 draft; self-verifying',
  phases: [{ title: 'crack' }],
}

const A = (typeof args === 'string' ? JSON.parse(args) : args) || {}
const N = A.n || 3
const MODEL = A.model || 'fable'
const EFFORT = A.effort || 'max'
const NAME = 'func_ov006_02115b0c'
const WL = 'progress/wl_pr104.jsonl'

const SCHEMA = {
  type: 'object', additionalProperties: false,
  properties: {
    name: { type: 'string' },
    matched: { type: 'boolean', description: 'true ONLY if abverify printed exactly MATCH' },
    c_source: { type: 'string', description: 'the LOWEST-divergence source you produced' },
    attempts: { type: 'integer' },
    divergences: { type: 'integer', description: 'lowest divergence count per abverify; 0 when matched' },
    note: { type: 'string', description: 'one line: what moved the needle, or the wall it hit' },
  },
  required: ['name', 'matched', 'c_source', 'attempts', 'divergences', 'note'],
}

function prompt(tag) {
  const src = `_abwork/${NAME}__${tag}.src`
  return `Byte-match ONE large Super Mario 64 DS function against the retail ROM with mwccarm. A compiling from-scratch draft already exists (structurally complete: the seven inlined-destructor loops, twelve tilemap scan loops, spawn/init/zeroing are all in place). Your job is to fix CODEGEN SHAPE, not re-decompile.

FUNCTION: ${NAME}  (ov006, 0x02115b0c, size 0x18bc, 1583 instructions)

STEP 1 - read the annotated target disasm, callee sigs, pool slots, and the stored DRAFT:
  python tools/abrow.py --name ${NAME} --wl ${WL}

COMPILER: mwccarm 1.2/sp2p3. Flags (C): -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
The draft already carries '#pragma opt_strength_reduction off' (required - every ROM loop is un-reduced). Keep it.

WRITE your working copy to ${src} (mkdir -p _abwork), then verify to see the current gap:
  python tools/abverify.py --name ${NAME} --src ${src} --wl ${WL}
NOMATCH prints the aligned target-vs-yours diff then "NOMATCH divergences=N". Edit toward zero.

THE CRUX (diagnosed - attack this FIRST, do not waste attempts rediscovering it):
The draft is +140 bytes / ~1649 divergences off, and it is NOT genuine register permutation - it is one systemic defect that cascades:
  * insn 1: target 'sub sp, sp, #0x124' with NO frame pointer. Your build emits 'sub sp, sp, #0xfc' THEN 'mov fp, sp'.
    The ROM function does NOT set up a frame pointer (fp/r11). mwccarm is emitting one for your draft, which steals a register and shifts the whole allocation, producing the r4/r6/r7/sl skew you see in EVERY subsequent instruction.
  * The PR author suspected this is an interaction with the by-value 'V2'/vector struct argument machinery (passing/spilling a by-value struct arg can force mwccarm to pin a frame pointer). Investigate that path first:
      - Is a struct/vector passed BY VALUE (not by pointer) to this function or to a callee? Try taking its address / passing a pointer instead, or copying the by-value arg into a local up front.
      - Large stack frame or address-taken locals can also force fp. Reduce address-taken locals; let arrays decay rather than taking &.
      - Try reordering the top-of-block C89 local declarations (mwccarm allocates stack/registers in DECLARATION ORDER) so the frame lays out as the ROM's.
  * There is also an early 'bl #0xfff98010' at insn 4 in the TARGET that your draft does not emit at that position - identify that callee from the pool/reloc context (abrow shows it) and place the call where the ROM has it; a missing early call also shifts everything.
KILL THE FRAME POINTER and the 1649 collapses. Once fp is gone and sizes match, the residual register coloring is tractable with the levers below.

STRUCTURAL LEVERS (pick by what the diff shows once sizes match):
- push-set / frame off by a register: reuse a var, re-emit a global/field inline instead of hoisting it to a register, or reorder C89 declarations.
- register coloring: *(T*)&G vs G[0] access form; materialize a bool (int b=(x==k); if(b)) for movne/moveq.
- materialized base (add rX,base,#imm then [rX] where your C folds to [base,#imm]): u64-mask laundering idiom *(int*)(((int)base + 0xOFF) & 0xFFFFFFFFFFFFFFFF) blocks offset folding and forces the add. A base the ROM passes to a call materializes via a pointer arg (Sub *b=&c->sub; b->m(...)). Halfword/byte at offset >= 0x100 materializes from a plain cast.
- arithmetic idiom: signed x/32 and x%32 use the ROM's un-reduced shift idiom; x/K vs (x*magic)>>s; x%K vs x-(x/K)*K.
- load width/signedness: u8/s8/u16/s16 local or cast flips ldrb/ldrsb/ldrh/ldrsh.
- CSE-elided reload before an adjacent store: force a fresh ldr with *(volatile int*)... .
Full catalogue: notes/mwccarm-codegen.md (sec 6e = newest levers) and notes/pret-idioms.md.

KNOWN TARGET-DATA CAVEAT (do NOT chase these until you are down to ~2 divergences at these exact offsets): the published disassembly is lossy for two ldrd/strd-decoded pool words. If your only remaining diffs are constants at +0x59c (a tilemap scan mask, most likely 0x3ff) and +0x5ac (a tile ID, written 0x2d2 but possibly 0x1d2/0x3d2), those are atlas ambiguities, not your bug - report them and stop; do not thrash other logic to satisfy them.

Up to 12 attempts, but STOP EARLY if divergences do not improve for 3 consecutive attempts; keep the LOWEST-divergence version in ${src} and report it.

CONSTRAINTS: do NOT edit anything under src/ or progress/ or nearmiss/ . Only write under _abwork/ .

Return the structured result. c_source = exact final ${src} contents. matched=true ONLY if abverify printed MATCH.`
}

phase('crack')
const t0 = budget.spent()
const tags = Array.from({ length: N }, (_, i) => `a${i + 1}`)
const results = (await parallel(tags.map((tag) => () =>
  agent(prompt(tag), { label: `${MODEL}:${NAME}:${tag}`, phase: 'crack', schema: SCHEMA, effort: EFFORT, model: MODEL })
))).filter(Boolean)
const tokens = budget.spent() - t0
const matched = results.filter((r) => r.matched)
const best = results.slice().sort((a, b) => (a.divergences ?? 9e9) - (b.divergences ?? 9e9))[0]
log(`${MODEL} crack ${NAME}: ${matched.length}/${results.length} matched; best div=${best ? best.divergences : 'n/a'}; ${tokens} out tok`)

return {
  name: NAME,
  matched: matched.length > 0,
  attempts: results.length,
  bestDivergences: best ? best.divergences : null,
  bestSource: best ? best.c_source : null,
  matchedSource: matched.length ? matched[0].c_source : null,
  outputTokens: tokens,
  results: results.map((r) => ({ tag: r.name, matched: r.matched, attempts: r.attempts, divergences: r.divergences, note: r.note })),
}
