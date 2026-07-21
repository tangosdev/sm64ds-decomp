"""Requesty free-model fan-out driver: crack each function with EVERY free model at once, keep the best.

Requesty (https://router.requesty.ai/v1) is an OpenAI-dialect gateway with a set of $0 models. This
runs glm_refine.py once per model over the SAME worklist -- each a normal, isolated attempt loop --
then merges the per-model outputs into one, picking the best result per function: a byte-match wins
outright; otherwise the lowest-divergence compiling draft becomes the near-miss. The merged file is in
glm_refine's exact output format, so `crackloop.py land --refine` banks + near-miss-pushes it unchanged.

Nothing here changes glm_refine: each model is a plain subprocess with GLM_MODEL/GLM_BASE_URL/GLM_DIALECT
set for Requesty, a private --workdir so concurrent drafts never clobber, and --no-claims so all models
work the same functions. A model that errors or dies just contributes nothing; the best of whoever
succeeded still wins, so the fan-out self-heals down to the models actually up.

Env:
  REQUESTY_API_KEY (required)   the rqsty-sk-... key
  REQUESTY_BASE_URL (optional)  default https://router.requesty.ai/v1
  REQUESTY_MODELS   (optional)  comma-separated override of the model list

Usage:
  REQUESTY_API_KEY=... python tools/requesty_fanout.py --wl <wl> --out <out> [--attempts 6] [--jobs 1]
  python tools/crackloop.py land --output <out> --refine
"""
import argparse, concurrent.futures, json, os, pathlib, re, subprocess, sys, tempfile, threading, time

# Write UTF-8 no matter the OS codepage. Windows Python defaults stdout to cp1252, which can't encode
# the ⟦ tag (or any non-latin1 char a model emits) and raises UnicodeEncodeError - the "'charmap'
# codec can't encode '⟦'" crash that killed every model. errors='replace' keeps a stray odd byte
# from ever sinking a run. The console reads this stream as UTF-8, so the tags arrive intact.
for _s in (sys.stdout, sys.stderr):
    try:
        _s.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        pass

# Every model streams into one stdout; tag each line ⟦model⟧ so the console can split the interleaved
# fan-out into a readable per-model tab. Held while writing a whole line so tags never split mid-line.
_TAG_OPEN, _TAG_CLOSE = "⟦", "⟧"  # ⟦ ⟧
_emit_lock = threading.Lock()

REPO = pathlib.Path(__file__).resolve().parent.parent
PY = sys.executable

# The free ($0) Requesty models that actually generate code. The two omitted from the "10 free":
# nvidia/nemotron-3.5-content-safety (a safety classifier, no code) and poolside/laguna-xs.2 (provider
# error). Override with REQUESTY_MODELS. A model that later goes down just drops out at merge time.
DEFAULT_MODELS = [
    "nvidia/nemotron-3-ultra-550b-a55b",
    "nvidia/nemotron-3-super-120b-a12b",
    "nvidia/nemotron-3-nano-30b-a3b",
    "nvidia/nemotron-3-nano-omni-30b-a3b-reasoning",
    "google/gemma-4-31b-it",
    "mistral/leanstral-1-5",
    "novita/tencent/hy3",
    "poolside/laguna-m.1",
]

BASE_URL = os.environ.get("REQUESTY_BASE_URL", "https://router.requesty.ai/v1")
# Every free model draws on the SAME key's rate limit, so 8 starting at once is a thundering herd of
# 429s. Spread their launches a few seconds apart (STAGGER) and give each more retry patience
# (GLM_RETRIES) so a throttled call waits out the window instead of dropping the model. Tunable.
STAGGER_S = float(os.environ.get("REQUESTY_STAGGER_S", "4"))
FANOUT_RETRIES = os.environ.get("REQUESTY_RETRIES", "12")


def _slug(model):
    return re.sub(r"[^a-z0-9]+", "-", model.lower()).strip("-")


def run_model(model, wl, attempts, jobs, key, launch_delay=0.0):
    """One glm_refine.py run pinned to `model`, into a private out+workdir. Returns the parsed output
    dict (or None if the run produced nothing usable). launch_delay staggers the start."""
    if launch_delay:
        time.sleep(launch_delay)
    out_path = pathlib.Path(tempfile.gettempdir()) / f"rqsty_{_slug(model)}_{os.getpid()}.output"
    env = dict(os.environ)
    env["GLM_API_KEY"] = key
    env["GLM_BASE_URL"] = BASE_URL
    env["GLM_MODEL"] = model
    env["GLM_DIALECT"] = "openai"
    env["GLM_RETRIES"] = FANOUT_RETRIES  # shared-key rate limits need more patience than the default 8
    cmd = [PY, str(REPO / "tools" / "glm_refine.py"),
           "--wl", wl, "--out", str(out_path),
           "--attempts", str(attempts), "--jobs", str(jobs),
           # Under the gitignored _abwork/ so per-model scratch never shows up as untracked files.
           "--no-claims", "--workdir", f"_abwork/rqsty_{_slug(model)}"]
    prefix = f"{_TAG_OPEN}{model}{_TAG_CLOSE} "
    try:
        # Stream child stdout line-by-line, tagging each so the console can route it to this model's
        # tab. Unbuffered so lines arrive live; UTF-8 so a model's non-latin1 reasoning can't crash
        # glm_refine's own stdout before we ever read it (same cp1252 trap as above).
        env["PYTHONUNBUFFERED"] = "1"
        env["PYTHONIOENCODING"] = "utf-8"
        proc = subprocess.Popen(cmd, cwd=REPO, env=env, stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT, text=True, encoding="utf-8",
                                errors="replace", bufsize=1)
        for line in proc.stdout:
            with _emit_lock:
                sys.stdout.write((prefix + line) if line.strip() else line)
                sys.stdout.flush()
        proc.wait(timeout=60 * 60)
    except Exception as e:
        print(f"[fanout] {model}: run failed - {str(e)[:120]}", flush=True)
        return None
    try:
        return json.loads(out_path.read_text(encoding="utf-8"))
    except Exception:
        print(f"[fanout] {model}: no parseable output", flush=True)
        return None
    finally:
        try: out_path.unlink()
        except OSError: pass


def merge(outputs_by_model):
    """Pick the best per function across all models. matched (div 0) beats any near-miss; among
    near-misses the lowest divergence wins; ties broken by model order (stable)."""
    # name -> best {matched, div, c_source, model}
    best = {}
    tin = tout = 0
    for model, out in outputs_by_model:
        if not out:
            continue
        tin += out.get("inputTokens") or 0
        tout += out.get("outputTokens") or 0
        srcs = out.get("sources") or {}
        nms = {n["name"]: n["c_source"] for n in (out.get("nearMisses") or []) if n.get("name") and n.get("c_source")}
        for r in out.get("results") or []:
            name = r.get("name")
            if not name:
                continue
            matched = bool(r.get("matched"))
            div = 0 if matched else (r.get("divergences") if isinstance(r.get("divergences"), int) else 999)
            csrc = srcs.get(name) if matched else nms.get(name)
            if csrc is None:
                continue  # non-compiling round for this model; nothing to keep
            cur = best.get(name)
            if cur is None or div < cur["div"]:
                best[name] = {"matched": matched, "div": div, "c_source": csrc, "model": model}

    landed = [n for n, b in best.items() if b["matched"]]
    results = [{"name": n, "matched": b["matched"], "attempts": 0, "divergences": b["div"],
                "note": f"best of fan-out via {b['model']}"} for n, b in best.items()]
    return {
        "model": "requesty-fanout",
        "attempted": len(best),
        "landed": len(landed),
        "landedNames": landed,
        "inputTokens": tin,
        "outputTokens": tout,
        "tokensPerLanded": round(tout / len(landed)) if landed else None,
        "results": results,
        "sources": {n: b["c_source"] for n, b in best.items() if b["matched"]},
        "nearMisses": [{"name": n, "c_source": b["c_source"]}
                       for n, b in best.items() if not b["matched"] and b["div"] < 999],
    }


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--wl", default=str(REPO / "progress" / "wl_refine.jsonl"))
    ap.add_argument("--out", default=str(REPO / "progress" / "requesty_fanout.output"))
    ap.add_argument("--attempts", type=int, default=6)
    # per-MODEL job count: each model run processes functions with this many workers. Keep it low -
    # every model already runs in parallel, so jobs>1 multiplies concurrent free-tier calls and 429s.
    ap.add_argument("--jobs", type=int, default=1)
    args = ap.parse_args()

    key = os.environ.get("REQUESTY_API_KEY", "").strip()
    if not key:
        sys.exit("set REQUESTY_API_KEY (rqsty-sk-... from requesty.ai)")
    models = [m.strip() for m in (os.environ.get("REQUESTY_MODELS") or ",".join(DEFAULT_MODELS)).split(",") if m.strip()]
    print(f"[fanout] {len(models)} free model(s), best-per-function wins:\n  " + "\n  ".join(models), flush=True)

    t0 = time.time()
    outputs = []
    # All models run concurrently (each is one glm_refine process). The pool cap bounds simultaneous
    # processes; API concurrency inside each is --jobs.
    with concurrent.futures.ThreadPoolExecutor(max_workers=len(models)) as ex:
        futs = {ex.submit(run_model, m, args.wl, args.attempts, args.jobs, key, i * STAGGER_S): m
                for i, m in enumerate(models)}
        for f in concurrent.futures.as_completed(futs):
            m = futs[f]
            try:
                outputs.append((m, f.result()))
            except Exception as e:
                print(f"[fanout] {m}: {str(e)[:120]}", flush=True)
                outputs.append((m, None))

    merged = merge(outputs)
    pathlib.Path(args.out).write_text(json.dumps(merged, indent=1), encoding="utf-8")
    live = sum(1 for _, o in outputs if o)
    mins = (time.time() - t0) / 60
    print(f"\n[fanout] {live}/{len(models)} models produced output; "
          f"best-of picked {merged['landed']} match(es) + "
          f"{len(merged['nearMisses'])} near-miss(es) in {mins:.0f}m", flush=True)
    print(f"land with: python tools/crackloop.py land --output {args.out} --refine", flush=True)


if __name__ == "__main__":
    main()
