"""GLM refine driver: run the refine tier through Zhipu's GLM coding-plan API
instead of Claude Workflow agents. Same worklist in, same .output JSON out, so
crackloop.py land --refine banks it unchanged.

The Workflow agents are agentic (they run abrow/abverify themselves); an API
model cannot, so this driver does the tool half of the loop: it fetches the
abrow context, sends it with the refine prompt, extracts the returned C source,
writes _abwork/<name>.src, runs abverify, and feeds the per-instruction diff
back as the next turn. Up to --attempts rounds per function, early-stop when
divergence stops improving (2 stale rounds), track the best draft throughout.

Endpoint: the GLM Coding Plan key speaks the Anthropic Messages dialect at
https://api.z.ai/api/anthropic (that is how Claude Code runs on GLM). Raw
metered API keys work too via the same dialect. Configure with:
  GLM_API_KEY   (required)
  GLM_BASE_URL  (default https://api.z.ai/api/anthropic)
  GLM_MODEL     (default glm-5.2)

Usage:
  python tools/refine_wl.py --max-div 20 --limit 12        # build the worklist
  python tools/glm_refine.py [--wl progress/wl_refine.jsonl] [--jobs 6]
                             [--attempts 6] [--out progress/glm_refine.output]
  python tools/crackloop.py land --output progress/glm_refine.output --refine
"""
import argparse, concurrent.futures, json, os, pathlib, re, subprocess, sys, threading, time

import requests

REPO = pathlib.Path(__file__).resolve().parent.parent
ABWORK = REPO / "_abwork"
PY = sys.executable

BASE_URL = os.environ.get("GLM_BASE_URL", "https://api.z.ai/api/anthropic")
MODEL = os.environ.get("GLM_MODEL", "glm-5.2")
API_KEY = os.environ.get("GLM_API_KEY", "")
if not API_KEY:  # fallback: first line of ~/.glm_key (never commit or paste the key)
    _kf = pathlib.Path.home() / ".glm_key"
    if _kf.is_file():
        API_KEY = _kf.read_text(encoding="utf-8").strip().splitlines()[0].strip()

_print_lock = threading.Lock()

# reassigned from --workdir before the pool runs, so a claimless mass sweep can
# write its drafts in a private dir and never clobber a concurrent Fable/DeepSeek
# batch working the same function name.
_WORKDIR = ABWORK


def log(msg):
    with _print_lock:
        print(msg, flush=True)


def run_tool(args_list, timeout=180):
    p = subprocess.run([PY] + args_list, cwd=REPO, capture_output=True,
                       text=True, timeout=timeout)
    return (p.stdout or "") + (p.stderr or "")


# The instruction block mirrors tools/refine_run.js prompt() minus the parts an
# API model cannot do (no shell); the driver supplies the abrow context and runs
# abverify for it. Keep the lever catalogue in sync with refine_run.js.
INSTRUCTIONS = """You are finishing one NEARLY-MATCHED Super Mario 64 DS function: a compiling \
draft exists that is only a few instructions from byte-matching the retail ROM with mwccarm. \
Fix the remaining codegen SHAPE, not the semantics (unless the diff proves the draft's \
semantics wrong).

COMPILER: mwccarm 1.2/sp2p3. Flags (C): -O4,p -enum int -lang c99 -char signed -interworking \
-proc arm946e -gccext,on -msgstyle gcc
If the draft starts with //cpp keep that exact first line (C++).

STRUCTURAL LEVERS (pick by what the diff shows):
- branch/store ORDER inside an if/else: move a store or call into/out of an arm; swap then/else bodies by inverting the condition
- loop form: while() vs do/while (top vs bottom test); for(;;) with break for find-first loops
- early-outs: merge separate "if(x) return K" into one && chain to a shared exit, or split a && chain into guard clauses; new-ctor null checks as if(p){body} return p
- arithmetic idiom: x/K vs (x*magic)>>s, x%K vs x-(x/K)*K, negative const via mov+rsb, -1 via mvn
- load width/signedness: u8/s8/u16/s16 local or cast flips ldrb/ldrsb/ldrh/ldrsh
- push-set / frame off by a register: reuse a var, re-emit a global/field inline instead of hoisting, or reorder top-of-block C89 declarations (mwccarm allocates in DECLARATION ORDER)
- register coloring: *(T*)&G vs G[0] access form; materialize a bool (int b=(x==k); if(b)) for movne/moveq
- two-word copy batching (ld,ld,st,st): int temps + fake dependency 'dst_a = b ? a : a;'
- MATERIALIZED BASE (add rX,base,#imm then [rX] where your C folds to [base,#imm]): the u64-mask laundering idiom - *(int *)(((int)base + 0xOFF) & 0xFFFFFFFFFFFFFFFF) - the identity AND blocks offset folding and forces the add. Cast to (int) BEFORE adding the offset, then mask.
- pointer-induction loop reduced away: '#pragma opt_strength_reduction off' above the function
- DUPLICATED early-exit epilogue (popeq/bxeq repeated instead of beq to shared tail): '#pragma optimize_for_size on'
- const-fold breaking when u64-laundering is inert (pure constants): '#pragma opt_propagation off'
- stack slots optimized away: a volatile local (or volatile struct field access) keeps them live
- halfword/byte access at offset >= 0x100 with a materialized base: plain '*(short*)(p+0x100) = k' reproduces it

STOP EARLY and report your best if the residual is a documented compiler floor: pure \
instruction-ordering (two-word store emission order), register-coloring swaps that survive \
declaration-order permutation, spilled-param stack homing, frame-pointer prologue decisions.

OUTPUT FORMAT - reply with EXACTLY one fenced code block containing the COMPLETE source file \
(everything needed to compile standalone: typedefs, externs, pragmas, the function), then on \
the line after the closing fence a single JSON line:
{"note": "<one line: what you changed and why>", "floor": <true if you believe the residual is a documented floor>}
No other prose."""


CODE_RE = re.compile(r"```(?:c|cpp|C|c\+\+)?\s*\n(.*?)```", re.DOTALL)
NOTE_RE = re.compile(r'\{"note".*\}')


def extract(reply):
    m = CODE_RE.search(reply or "")
    src = m.group(1) if m else None
    note, floor = "", False
    nm = NOTE_RE.search((reply or "").rsplit("```", 1)[-1])
    if nm:
        try:
            j = json.loads(nm.group(0))
            note, floor = str(j.get("note", ""))[:300], bool(j.get("floor"))
        except json.JSONDecodeError:
            pass
    return src, note, floor


def chat(messages, max_tokens=8000, retries=4):
    url = BASE_URL.rstrip("/") + "/v1/messages"
    headers = {"x-api-key": API_KEY, "anthropic-version": "2023-06-01",
               "content-type": "application/json"}
    body = {"model": MODEL, "max_tokens": max_tokens, "messages": messages}
    delay = 5
    for i in range(retries):
        r = requests.post(url, headers=headers, json=body, timeout=600)
        if r.status_code == 200:
            data = r.json()
            text = "".join(b.get("text", "") for b in data.get("content", [])
                           if b.get("type") == "text")
            u = data.get("usage", {})
            return text, u.get("input_tokens", 0), u.get("output_tokens", 0)
        if r.status_code in (429, 500, 502, 503, 529):
            time.sleep(delay); delay = min(delay * 2, 120); continue
        if r.status_code == 402 or "insufficient" in r.text.lower():
            raise RuntimeError("BALANCE_EXHAUSTED")
        raise RuntimeError(f"GLM API {r.status_code}: {r.text[:300]}")
    raise RuntimeError("GLM API: retries exhausted")


DIV_RE = re.compile(r"NOMATCH divergences=(\d+)")


def verify(name, wl):
    out = run_tool(["tools/abverify.py", "--name", name,
                    "--src", str(_WORKDIR / f"{name}.src"), "--wl", wl])
    if re.search(r"^MATCH\s*$", out, re.MULTILINE):
        return 0, out
    m = DIV_RE.search(out)
    return (int(m.group(1)) if m else 999), out


def crack_one(name, wl, attempts, row):
    t_in = t_out = 0
    att_log = []  # per-attempt lines, printed by the caller BELOW the result header
    try:
        ctx = run_tool(["tools/abrow.py", "--name", name, "--wl", wl])
        draft = row.get("draft") or ""
        _WORKDIR.mkdir(exist_ok=True, parents=True)
        (_WORKDIR / f"{name}.src").write_text(draft, encoding="utf-8", newline="\n")
        best_div, vout = verify(name, wl)
        best_src = draft
        if best_div == 0:
            return {"name": name, "matched": True, "c_source": draft, "attempts": 0,
                    "divergences": 0, "note": "stored draft already matches",
                    "log": att_log}, 0, 0

        messages = [{"role": "user", "content":
                     f"{INSTRUCTIONS}\n\nFUNCTION: {name}\n\n=== CONTEXT (annotated target "
                     f"disasm, callee sigs, pool slots, stored draft) ===\n{ctx}\n\n"
                     f"=== CURRENT DRAFT (divergences={best_div}) ===\n{draft}\n\n"
                     f"=== VERIFIER OUTPUT ===\n{vout}"}]
        note, stale = "", 0
        for att in range(1, attempts + 1):
            reply, i_tok, o_tok = chat(messages)
            t_in += i_tok; t_out += o_tok
            src, note, floor = extract(reply)
            if not src:
                messages.append({"role": "assistant", "content": reply})
                messages.append({"role": "user", "content":
                                 "No code block found. Reply with the complete source file "
                                 "in one fenced block plus the JSON line."})
                continue
            (_WORKDIR / f"{name}.src").write_text(src, encoding="utf-8", newline="\n")
            div, vout = verify(name, wl)
            att_log.append(f"attempt {att}: div={div} (best {min(best_div, div)})")
            if div < best_div:
                best_div, best_src, stale = div, src, 0
            else:
                stale += 1
            if div == 0:
                return {"name": name, "matched": True, "c_source": src, "attempts": att,
                        "divergences": 0, "note": note or "matched",
                        "log": att_log}, t_in, t_out
            if floor or stale >= 2:
                break
            messages.append({"role": "assistant", "content": reply})
            messages.append({"role": "user", "content":
                             f"Still NOMATCH, divergences={div}. Verifier output:\n{vout}\n\n"
                             f"Best so far is {best_div}. Try a different lever."})
        (_WORKDIR / f"{name}.src").write_text(best_src, encoding="utf-8", newline="\n")
        return {"name": name, "matched": False, "c_source": best_src,
                "attempts": attempts, "divergences": best_div,
                "note": note or "no improvement", "log": att_log}, t_in, t_out
    except Exception as e:  # one function failing must not sink the batch
        return {"name": name, "matched": False, "c_source": row.get("draft") or "",
                "attempts": 0, "divergences": 999,
                "note": f"driver error: {e}"[:300], "log": att_log}, t_in, t_out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--wl", default=str(REPO / "progress" / "wl_refine.jsonl"))
    ap.add_argument("--jobs", type=int, default=6)
    ap.add_argument("--attempts", type=int, default=6)
    ap.add_argument("--limit", type=int, default=0, help="cap names (0 = all)")
    ap.add_argument("--out", default=str(REPO / "progress" / "glm_refine.output"))
    ap.add_argument("--chunk", type=int, default=25,
                    help="claim-lock and work this many functions at a time, so a "
                         "mass sweep never hogs the pool from other tiers/contributors")
    ap.add_argument("--no-claims", action="store_true",
                    help="do NOT claim-lock; attempt every row even if held elsewhere. "
                         "Use for a huge cheap sweep where holding claims would block "
                         "other contributors; duplicate work is dedup-skipped at land.")
    ap.add_argument("--workdir", default=None,
                    help="private dir for _abwork drafts (default _abwork). Set a "
                         "distinct dir for a claimless sweep so it cannot clobber a "
                         "concurrent batch working the same function name.")
    ap.add_argument("--dry-run", action="store_true",
                    help="build one prompt, print sizes, no API call")
    args = ap.parse_args()

    global _WORKDIR
    if args.workdir:
        _WORKDIR = pathlib.Path(args.workdir)
        if not _WORKDIR.is_absolute():
            _WORKDIR = REPO / args.workdir

    rows = [json.loads(l) for l in
            pathlib.Path(args.wl).read_text(encoding="utf-8").splitlines() if l.strip()]
    if args.limit:
        rows = rows[:args.limit]

    if args.dry_run:
        r = rows[0]
        ctx = run_tool(["tools/abrow.py", "--name", r["name"], "--wl", args.wl])
        print(f"name={r['name']} ctx={len(ctx)}ch draft={len(r.get('draft') or '')}ch "
              f"prompt~{(len(INSTRUCTIONS) + len(ctx)) // 4} tok")
        return

    if not API_KEY:
        sys.exit("set GLM_API_KEY (coding-plan key from z.ai)")

    claims = None
    if not args.no_claims:
        sys.path.insert(0, str(REPO / "tools"))
        import claims  # belongto.us lock service; per-chunk try-lock/release

    def addr_int(r):
        a = r["addr"]
        return int(a, 0) if isinstance(a, str) else a

    def size_int(r):
        s = r["size"]
        return int(s, 0) if isinstance(s, str) else s

    t0 = time.time()
    results, tin, tout, skipped = [], 0, 0, 0
    for c0 in range(0, len(rows), args.chunk):
        chunk = rows[c0:c0 + args.chunk]
        locked = []           # (row, claim_id or None)
        if args.no_claims:
            locked = [(r, None) for r in chunk]   # attempt everything, lock nothing
        else:
            for r in chunk:
                try:
                    a, sz = addr_int(r), size_int(r)
                    st, res = claims.try_lock(r["module"], f"0x{a:08x}", f"0x{a + sz:08x}",
                                              note=f"glm-5.2 sweep: {r['name'][:60]}")
                    if st is None:
                        sys.exit(f"claims service unreachable ({res.get('error')}) - "
                                 f"refusing to run unlocked; fix and rerun")
                    if st == 401:
                        sys.exit("claims key expired/invalid (401) - refusing to run "
                                 "unlocked; refresh tools/claims_key.txt and rerun")
                    cid = claims._claim_id(res)
                    if cid is None:
                        skipped += 1   # conflict: someone else holds it - skip
                        continue
                    locked.append((r, cid))
                except SystemExit:
                    raise
                except Exception as e:
                    sys.exit(f"claims lock failed ({e}) - refusing to run unlocked")
        if not locked:
            continue
        try:
            with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
                futs = {ex.submit(crack_one, r["name"], args.wl, args.attempts, r): r
                        for r, _ in locked}
                for f in concurrent.futures.as_completed(futs):
                    res, i_tok, o_tok = f.result()
                    tin += i_tok; tout += o_tok
                    results.append(res)
                    # Result-first block: header (index, name, final div) then this
                    # function's attempts indented below, so each function reads as one
                    # tidy unit and a new header means the previous one finished.
                    status = 'MATCH' if res['matched'] else 'div=' + str(res['divergences'])
                    lines = [f"({len(results)}/{len(rows)}) {res['name']}: {status}"]
                    lines += [f"    {ln}" for ln in res.get('log', [])]
                    log("\n".join(lines))
        finally:
            if claims is not None:
                for _, cid in locked:
                    if cid is None:
                        continue
                    try:
                        claims.release(cid)
                    except Exception:
                        pass  # claim TTL expires on its own; do not sink the sweep
        _write_output(args.out, results, tin, tout)   # partial results survive a crash
        if any("BALANCE_EXHAUSTED" in (r.get("note") or "") for r in results):
            log("API balance exhausted - stopping the sweep (partial output written)")
            break

    landed = [r for r in results if r["matched"]]
    mins = (time.time() - t0) / 60
    print(f"\n{MODEL}: landed {len(landed)}/{len(results)} "
          f"({skipped} skipped as claim-locked elsewhere) in {mins:.0f}m; "
          f"in={tin} out={tout} tok"
          + (f" ({round(tout / len(landed))} out/landed)" if landed else ""))
    print(f"land with: python tools/crackloop.py land --output {args.out} --refine")


def _write_output(path, results, tin, tout):
    landed = [r for r in results if r["matched"]]
    out = {
        "model": MODEL,
        "attempted": len(results),
        "landed": len(landed),
        "landedNames": [r["name"] for r in landed],
        "outputTokens": tout,
        "inputTokens": tin,
        "tokensPerLanded": round(tout / len(landed)) if landed else None,
        "results": [{"name": r["name"], "matched": r["matched"], "attempts": r["attempts"],
                     "divergences": r["divergences"], "note": r["note"]} for r in results],
        "sources": {r["name"]: r["c_source"] for r in landed if r["c_source"]},
        "nearMisses": [{"name": r["name"], "c_source": r["c_source"]}
                       for r in results if not r["matched"] and r["c_source"]],
    }
    pathlib.Path(path).write_text(json.dumps(out, indent=1), encoding="utf-8")


if __name__ == "__main__":
    main()
