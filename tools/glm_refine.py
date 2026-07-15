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
import argparse, concurrent.futures, json, os, pathlib, random, re, subprocess, sys, threading, time

import requests

REPO = pathlib.Path(__file__).resolve().parent.parent
ABWORK = REPO / "_abwork"
PY = sys.executable

BASE_URL = os.environ.get("GLM_BASE_URL", "https://api.z.ai/api/anthropic")
MODEL = os.environ.get("GLM_MODEL", "glm-5.2")
API_KEY = os.environ.get("GLM_API_KEY", "")

# Reasoning effort, set by the tangOS console per AI box (TANGOS_EFFORT), mapped to the
# provider's thinking knob in chat(). Empty or "off" = no extended reasoning.
EFFORT = os.environ.get("TANGOS_EFFORT", "").strip().lower()
_IS_ANTHROPIC = "anthropic.com" in BASE_URL  # real Claude vs a GLM/other Anthropic-dialect host
_CLAUDE_BUDGET = {"low": 2048, "medium": 4096, "high": 8192, "xhigh": 16384, "max": 24576}
# DeepSeek (and other OpenAI-compatible hosts) speak the OpenAI /chat/completions dialect, not the
# Anthropic /v1/messages dialect GLM/Claude use. The console sets GLM_DIALECT=openai for those; when
# unset, infer from the base URL. deepseek-reasoner reasons on its own, so no thinking param is sent.
_DIALECT = os.environ.get("GLM_DIALECT", "").strip().lower() or (
    "openai" if "deepseek" in BASE_URL.lower() else "anthropic")
_IS_OPENAI = _DIALECT == "openai"


def _thinking_for(max_tokens):
    """(thinking-param-or-None, effective max_tokens) for the current EFFORT + provider."""
    if not EFFORT or EFFORT == "off" or _IS_OPENAI:
        return None, max_tokens
    if _IS_ANTHROPIC:
        budget = _CLAUDE_BUDGET.get(EFFORT, 8192)
        # Anthropic requires max_tokens > budget_tokens; leave headroom for the actual answer.
        return {"type": "enabled", "budget_tokens": budget}, max(max_tokens, budget + 8000)
    return {"type": "enabled"}, max_tokens  # GLM etc.: reasoning is a plain on/off toggle
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


def _read_openai_stream(r, on_delta):
    """Parse an OpenAI /chat/completions SSE stream. Echo each delta to on_delta as it arrives - both
    reasoning_content (the model thinking) and content (the answer) - so the console live viewer shows
    the model working in real time. Only `content` is accumulated and returned; reasoning is shown but
    not fed to the code extractor. Returns (text, prompt_tokens, completion_tokens) like the non-stream
    path (usage rides the final chunk when the server honors stream_options.include_usage)."""
    parts, in_tok, out_tok = [], 0, 0
    for raw in r.iter_lines(decode_unicode=True):
        if not raw or not raw.startswith("data:"):
            continue
        payload = raw[5:].strip()
        if payload == "[DONE]":
            break
        try:
            chunk = json.loads(payload)
        except ValueError:
            continue
        choices = chunk.get("choices") or []
        if choices:
            delta = choices[0].get("delta") or {}
            reason = delta.get("reasoning_content")
            if reason:
                on_delta(reason)
            piece = delta.get("content")
            if piece:
                parts.append(piece)
                on_delta(piece)
        usage = chunk.get("usage")
        if usage:
            in_tok = usage.get("prompt_tokens") or in_tok
            out_tok = usage.get("completion_tokens") or out_tok
    return "".join(parts), in_tok, out_tok


def chat(messages, max_tokens=8000, retries=8, on_delta=None):
    think, mt = _thinking_for(max_tokens)
    # Token streaming (OpenAI dialect only): when on_delta is given, read the SSE stream and echo each
    # delta as it arrives instead of waiting for the whole reply. Same tokens at the same rate, just
    # delivered incrementally. Anthropic-dialect callers ignore on_delta (they stay non-streaming).
    stream = bool(on_delta) and _IS_OPENAI
    if _IS_OPENAI:
        # OpenAI Chat Completions dialect (DeepSeek / local LM Studio): Bearer auth, /chat/completions.
        url = BASE_URL.rstrip("/") + "/chat/completions"
        headers = {"authorization": f"Bearer {API_KEY}", "content-type": "application/json"}
        # A reasoning model (deepseek-reasoner, nemotron) spends its hidden thinking INSIDE
        # max_tokens; at the default 8000 a hard function's reasoning starves/truncates the code
        # block ("no code block returned"). Give the known reasoners real headroom - they only bill
        # what they use. (nemotron reasons heavily but its name lacks "reason", so name it explicitly.
        # NOTE: this is still bounded by the server's loaded context window - LM Studio defaults
        # Nemotron to 8192, too small for this; load it with a bigger --context to actually use this.)
        if "reason" in MODEL.lower() or "nemotron" in MODEL.lower():
            mt = max(mt, 24000)
        body = {"model": MODEL, "max_tokens": mt, "messages": messages}
        if stream:
            body["stream"] = True
            body["stream_options"] = {"include_usage": True}  # usage rides the final chunk
    else:
        url = BASE_URL.rstrip("/") + "/v1/messages"
        headers = {"x-api-key": API_KEY, "anthropic-version": "2023-06-01",
                   "content-type": "application/json"}
        body = {"model": MODEL, "max_tokens": mt, "messages": messages}
        if think:
            body["thinking"] = think
            if _IS_ANTHROPIC:
                body["temperature"] = 1  # Anthropic requires temperature=1 when thinking is on
    # Rate-limit resilience: with several parallel workers the API 429s a lot. Back off with
    # jitter and honor Retry-After so a throttled request recovers instead of erroring the whole
    # function. Network hiccups are treated the same as a retryable 5xx.
    delay = 5
    for i in range(retries):
        try:
            r = requests.post(url, headers=headers, json=body, timeout=600, stream=stream)
        except requests.RequestException:
            if i == retries - 1:
                raise RuntimeError("GLM API: network error, retries exhausted")
            time.sleep(delay + random.uniform(0, 3)); delay = min(delay * 2, 120); continue
        if r.status_code == 200:
            if stream:
                # A mid-stream drop is retried like any network hiccup (partial deltas already echoed).
                try:
                    return _read_openai_stream(r, on_delta)
                except requests.RequestException:
                    if i == retries - 1:
                        raise RuntimeError("GLM API: stream error, retries exhausted")
                    time.sleep(delay + random.uniform(0, 3)); delay = min(delay * 2, 120); continue
            data = r.json()
            if _IS_OPENAI:
                msg = (data.get("choices") or [{}])[0].get("message", {}) or {}
                text = msg.get("content", "") or ""  # reasoner's thinking is in reasoning_content; ignore it
                u = data.get("usage", {})
                return text, u.get("prompt_tokens", 0), u.get("completion_tokens", 0)
            text = "".join(b.get("text", "") for b in data.get("content", [])
                           if b.get("type") == "text")  # thinking blocks are ignored
            u = data.get("usage", {})
            return text, u.get("input_tokens", 0), u.get("output_tokens", 0)
        if r.status_code in (429, 500, 502, 503, 529):
            ra = r.headers.get("retry-after", "")
            wait = float(ra) if ra.replace(".", "", 1).isdigit() else delay
            time.sleep(min(wait, 120) + random.uniform(0, 3)); delay = min(delay * 2, 120); continue
        if r.status_code == 402 or "insufficient" in r.text.lower():
            raise RuntimeError("BALANCE_EXHAUSTED")
        # Model rejected the reasoning param -> drop it and retry without extended thinking.
        if r.status_code == 400 and "thinking" in body:
            body.pop("thinking", None); body.pop("temperature", None); continue
        raise RuntimeError(f"GLM API {r.status_code}: {r.text[:300]}")
    raise RuntimeError("GLM API: rate-limited, retries exhausted")


DIV_RE = re.compile(r"NOMATCH divergences=(\d+)")


def verify(name, wl):
    out = run_tool(["tools/abverify.py", "--name", name,
                    "--src", str(_WORKDIR / f"{name}.src"), "--wl", wl])
    if re.search(r"^MATCH\s*$", out, re.MULTILINE):
        return 0, out
    m = DIV_RE.search(out)
    return (int(m.group(1)) if m else 999), out


def crack_one(name, wl, attempts, row, live=False):
    t_in = t_out = 0
    orig_div = None  # stored draft's divergence before refining; None if verify never ran
    att_log = []  # per-attempt lines, printed by the caller BELOW the result header
    # On a sequential drive (jobs=1, e.g. GLM / local Nemotron) stream each attempt as it lands so
    # the console's live viewer fills in instead of sitting silent for the minute+ a slow local model
    # takes per function. Parallel drives keep the tidy result-first block (live=False), since
    # interleaved workers would scramble it. The line still goes into att_log for the .output file.
    def note_attempt(msg):
        att_log.append(msg)
        if live:
            log(f"    {name} {msg}")
    # Live token echo -> STDERR. The console shows stderr in its live viewer, but its progress parser
    # only reads STDOUT, so raw model text (which could contain a "(1/2) ...: MATCH"-looking line) can
    # never be miscounted as a completion. Buffered so a fast stream doesn't flood the IPC: flushed on
    # a newline or ~120 chars, with the tail flushed after each reply.
    _sbuf, _slen = [], [0]
    def stream_echo(frag):
        _sbuf.append(frag); _slen[0] += len(frag)
        if _slen[0] >= 120 or "\n" in frag:
            sys.stderr.write("".join(_sbuf)); sys.stderr.flush()
            _sbuf.clear(); _slen[0] = 0
    def flush_echo():
        if _sbuf:
            sys.stderr.write("".join(_sbuf) + "\n"); sys.stderr.flush()
            _sbuf.clear(); _slen[0] = 0
    on_delta = stream_echo if live else None
    try:
        ctx = run_tool(["tools/abrow.py", "--name", name, "--wl", wl])
        draft = row.get("draft") or ""
        _WORKDIR.mkdir(exist_ok=True, parents=True)
        (_WORKDIR / f"{name}.src").write_text(draft, encoding="utf-8", newline="\n")
        best_div, vout = verify(name, wl)
        orig_div = best_div  # the "from": stored draft's divergence before any refine attempt
        best_src = draft
        if best_div == 0:
            return {"name": name, "matched": True, "c_source": draft, "attempts": 0,
                    "divergences": 0, "orig_div": orig_div, "note": "stored draft already matches",
                    "log": att_log}, 0, 0

        has_draft = bool(draft.strip())
        if live:  # immediate feedback: which function is being worked, before the slow model call
            phase = f"draft div={best_div}, refining" if has_draft else "no draft, writing from scratch"
            log(f"-> {name}: {phase} with {MODEL} (up to {attempts} attempts)...")
        if has_draft:
            draft_block = (f"=== CURRENT DRAFT (divergences={best_div}) ===\n{draft}\n\n"
                           f"=== VERIFIER OUTPUT ===\n{vout}")
        else:
            # No candidate exists yet (a fresh target). Say so plainly: an empty "CURRENT DRAFT
            # (divergences=999)" block just reads to the model as "the draft is missing" and sends it
            # hunting for one or reaching for an inline-asm hack. Point it at writing real C instead.
            draft_block = ("=== NO DRAFT YET ===\nThere is no candidate C for this function - write the "
                           "COMPLETE source file from scratch so mwccarm compiles it to byte-match the "
                           "target disassembly above. Write real C/C++ (declarations, types, the function "
                           "body); do NOT emit an inline-asm blob - that is not a valid match and will not "
                           "compile with these flags.")
        messages = [{"role": "user", "content":
                     f"{INSTRUCTIONS}\n\nFUNCTION: {name}\n\n=== CONTEXT (annotated target "
                     f"disasm, callee sigs, pool slots, stored draft) ===\n{ctx}\n\n{draft_block}"}]
        note, stale, apierr = "", 0, 0
        for att in range(1, attempts + 1):
            # Per-attempt error handling: a rate-limit/network error on one attempt must NOT sink
            # the whole function (the old behavior: div=999 with zero logged attempts). Log it and
            # keep going, up to a few consecutive failures. BALANCE_EXHAUSTED still hard-stops.
            try:
                reply, i_tok, o_tok = chat(messages, on_delta=on_delta)
            except RuntimeError as e:
                if "BALANCE_EXHAUSTED" in str(e):
                    raise
                apierr += 1
                if live:
                    flush_echo()
                note_attempt(f"attempt {att}: api error - {str(e)[:50]}")
                if apierr >= 3:
                    break
                continue
            apierr = 0
            if live:
                flush_echo()  # push the tail of the streamed reply before the attempt-summary line
            t_in += i_tok; t_out += o_tok
            src, note, floor = extract(reply)
            if not src:
                note_attempt(f"attempt {att}: no code block returned")
                messages.append({"role": "assistant", "content": reply})
                messages.append({"role": "user", "content":
                                 "No code block found. Reply with the complete source file "
                                 "in one fenced block plus the JSON line."})
                continue
            (_WORKDIR / f"{name}.src").write_text(src, encoding="utf-8", newline="\n")
            div, vout = verify(name, wl)
            note_attempt(f"attempt {att}: div={div} (best {min(best_div, div)})")
            if div < best_div:
                best_div, best_src, stale = div, src, 0
            else:
                stale += 1
            if div == 0:
                return {"name": name, "matched": True, "c_source": src, "attempts": att,
                        "divergences": 0, "orig_div": orig_div, "note": note or "matched",
                        "log": att_log}, t_in, t_out
            if floor or stale >= 2:
                break
            messages.append({"role": "assistant", "content": reply})
            messages.append({"role": "user", "content":
                             f"Still NOMATCH, divergences={div}. Verifier output:\n{vout}\n\n"
                             f"Best so far is {best_div}. Try a different lever."})
        (_WORKDIR / f"{name}.src").write_text(best_src, encoding="utf-8", newline="\n")
        return {"name": name, "matched": False, "c_source": best_src,
                "attempts": attempts, "divergences": best_div, "orig_div": orig_div,
                "note": note or "no improvement", "log": att_log}, t_in, t_out
    except Exception as e:  # one function failing must not sink the batch
        return {"name": name, "matched": False, "c_source": row.get("draft") or "",
                "attempts": 0, "divergences": 999, "orig_div": orig_div,
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
    live = args.jobs == 1  # sequential drive: stream per-attempt progress (see crack_one)
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
                futs = {ex.submit(crack_one, r["name"], args.wl, args.attempts, r, live): r
                        for r, _ in locked}
                for f in concurrent.futures.as_completed(futs):
                    res, i_tok, o_tok = f.result()
                    tin += i_tok; tout += o_tok
                    results.append(res)
                    # Result-first block: header (index, name, final div) then this
                    # function's attempts indented below, so each function reads as one
                    # tidy unit and a new header means the previous one finished.
                    status = 'MATCH' if res['matched'] else 'div=' + str(res['divergences'])
                    # Show the stored draft's starting divergence so a near-miss improvement (or
                    # lack of one) is legible at a glance: "div=6 (from: 40)" vs "div=6 (from: 6)".
                    orig = res.get('orig_div')
                    if isinstance(orig, int) and orig > 0:
                        status += f" (from: {orig})"
                    lines = [f"({len(results)}/{len(rows)}) {res['name']}: {status}"]
                    if not live:  # sequential runs already streamed these attempts live
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
