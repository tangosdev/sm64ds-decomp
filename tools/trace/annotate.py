"""Phase-2 batch annotation pass.

For each function that has a runtime trace, run the deterministic enrichment
(enrich.py) and a CHEAP model call to turn {draft C, trace facts} into a
structured annotation (corrected signature, arg types, semantic name, behavior,
draft errors, confidence). Results are appended to traces/annotations.jsonl and
are what Phase 3 injects into refine/fanout prompts.

Model access mirrors tools/glm_refine.py: the Anthropic Messages dialect over
the env-configured endpoint, so it plugs into the same setup the refine loop
uses. Defaults to a cheap tier.

  GLM_BASE_URL   (default https://api.z.ai/api/anthropic; set to
                  https://api.anthropic.com for real Claude)
  GLM_API_KEY    (or first line of ~/.glm_key)
  ANNOTATE_MODEL (default: GLM_MODEL, else claude-haiku-4-5-20251001)

Usage:
  python tools/trace/annotate.py --all-traced            # every func with a trace
  python tools/trace/annotate.py --wl funcs.txt
  python tools/trace/annotate.py --all-traced --dry-run  # build prompts, no model
  python tools/trace/annotate.py --emit-prompts out/     # write prompts for a
                                                         # Workflow fan-out (keyless)
"""
import argparse
import json
import os
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import enrich as enrich_mod  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parents[2]
TRACES = REPO / "traces"
ANNOTATIONS = TRACES / "annotations.jsonl"

BASE_URL = os.environ.get("GLM_BASE_URL", "https://api.z.ai/api/anthropic")
API_KEY = os.environ.get("GLM_API_KEY", "")
MODEL = os.environ.get("ANNOTATE_MODEL", os.environ.get("GLM_MODEL", "claude-haiku-4-5-20251001"))
if not API_KEY:
    _kf = pathlib.Path.home() / ".glm_key"
    if _kf.is_file():
        API_KEY = _kf.read_text(encoding="utf-8").strip().splitlines()[0].strip()

PROMPT_HEAD = """You are annotating a decompiled Nintendo DS (ARM9) function to help a decomp \
matching effort. Below is the current decomp DRAFT plus RUNTIME TRACE FACTS captured from a real \
emulator (per-arg value profiles at entry, pointer-deref memory, callers, callees). Use the trace \
to propose a BETTER-INFORMED annotation than the draft: which of r0-r3 are actually live args (the \
draft may have the arity wrong), each arg's type (pointer vs small int vs hardware I/O register -- \
NDS I/O is 0x04000000+, the 0x040004xx range is the GX geometry/command engine), Fix12 fixed-point \
(values that are multiples of 4096), and what pointer args point to.

Output ONLY this block, terse and concrete, no preamble:

SIGNATURE: <corrected C prototype>
ARGS:
  r0: <type> -- <role>
  r1: <type> -- <role>
  r2: <type> -- <role>
  r3: <type> -- <role>
SEMANTIC_NAME: <best guess at a real function name>
BEHAVIOR: <2 sentences>
DRAFT_ERRORS: <what the trace shows the draft got wrong>
CONFIDENCE: <high|medium|low> -- <one clause why>

--- TRACE FACTS + DRAFT ---
"""


def build_prompt(facts):
    L = [f"FUNCTION: {facts['name']} (module {facts['module']}, addr {facts['addr']}, "
         f"size {facts['size']}, divergences {facts['divergences']}, {facts['n_hits']} hits)", ""]
    L.append("RUNTIME ARG PROFILES (r0-r3 at entry across hits):")
    for k, v in facts["arg_profiles"].items():
        tags = [t for t in ("stable", "all_zero", "ram_ptr", "io_reg", "fix12_whole") if v[t]]
        L.append(f"  {k}: tags={tags or ['(varies)']} distinct={v['distinct']}")
    L.append("")
    for a in ("r0", "r1", "r2", "r3"):
        if facts["arg_derefs"].get(a):
            L.append(f"*{a} memory words: {facts['arg_derefs'][a]}")
    L.append("")
    L.append(f"CALLERS (runtime): {facts['callers']}")
    L.append(f"CALLEES (disasm): {[c['name'] or c['addr'] for c in facts['callees']] or 'leaf'}")
    L.append("")
    L.append("CURRENT DECOMP DRAFT (structurally imperfect):")
    L.append(facts["draft_c_source"])
    return PROMPT_HEAD + "\n".join(L)


_FIELD = re.compile(r"^(SIGNATURE|SEMANTIC_NAME|BEHAVIOR|DRAFT_ERRORS|CONFIDENCE):\s*(.*)$")


def parse_annotation(text):
    """Parse the fixed block into fields; keep the raw too."""
    out = {"signature": None, "args": {}, "semantic_name": None, "behavior": None,
           "draft_errors": None, "confidence": None, "raw": text.strip()}
    in_args = False
    for line in text.splitlines():
        m = _FIELD.match(line.strip())
        if m:
            in_args = False
            key = m.group(1).lower()
            out[{"signature": "signature", "semantic_name": "semantic_name",
                 "behavior": "behavior", "draft_errors": "draft_errors",
                 "confidence": "confidence"}[key]] = m.group(2).strip()
            continue
        if line.strip() == "ARGS:":
            in_args = True
            continue
        if in_args:
            am = re.match(r"\s*(r[0-3]):\s*(.*)", line)
            if am:
                out["args"][am.group(1)] = am.group(2).strip()
    return out


def call_model(prompt, max_tokens=1024, timeout=120):
    import requests
    if not API_KEY:
        raise RuntimeError("no API key (GLM_API_KEY / ~/.glm_key). Use --dry-run or "
                           "--emit-prompts and drive a Workflow fan-out instead.")
    url = BASE_URL.rstrip("/") + "/v1/messages"
    headers = {"x-api-key": API_KEY, "anthropic-version": "2023-06-01",
               "content-type": "application/json"}
    body = {"model": MODEL, "max_tokens": max_tokens,
            "messages": [{"role": "user", "content": prompt}]}
    r = requests.post(url, headers=headers, json=body, timeout=timeout)
    r.raise_for_status()
    data = r.json()
    return "".join(b.get("text", "") for b in data.get("content", []))


def traced_funcs():
    """Every func name that has a traces/<module>/<name>.jsonl file."""
    out = []
    if not TRACES.is_dir():
        return out
    for p in TRACES.glob("*/*.jsonl"):
        if p.name != "coverage.jsonl":
            out.append(p.stem)
    return sorted(set(out))


def already_done():
    done = set()
    if ANNOTATIONS.is_file():
        for line in ANNOTATIONS.read_text(encoding="utf-8").splitlines():
            if line.strip():
                try:
                    done.add(json.loads(line)["name"])
                except (json.JSONDecodeError, KeyError):
                    pass
    return done


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--all-traced", action="store_true", help="annotate every traced func")
    ap.add_argument("--wl", default=None, help="worklist of names to annotate")
    ap.add_argument("--limit", type=int, default=None)
    ap.add_argument("--dry-run", action="store_true", help="build prompts, no model call")
    ap.add_argument("--emit-prompts", default=None,
                    help="write per-func prompt .txt files to this dir (for Workflow fan-out)")
    ap.add_argument("--force", action="store_true", help="re-annotate already-done funcs")
    args = ap.parse_args()

    if args.wl:
        names = [n.strip() for n in pathlib.Path(args.wl).read_text().split()
                 if n.strip() and not n.startswith("#")]
    elif args.all_traced:
        names = traced_funcs()
    else:
        print("specify --all-traced or --wl", file=sys.stderr)
        return 2

    if not args.force:
        done = already_done()
        names = [n for n in names if n not in done]
    if args.limit:
        names = names[:args.limit]
    print(f"[*] {len(names)} func(s) to annotate; model={MODEL}", file=sys.stderr)

    emit_dir = pathlib.Path(args.emit_prompts) if args.emit_prompts else None
    if emit_dir:
        emit_dir.mkdir(parents=True, exist_ok=True)

    ANNOTATIONS.parent.mkdir(parents=True, exist_ok=True)
    n_ok = 0
    for name in names:
        try:
            facts = enrich_mod.enrich(name)
        except SystemExit as e:
            print(f"  ! {name}: {e}", file=sys.stderr)
            continue
        prompt = build_prompt(facts)
        if emit_dir:
            (emit_dir / f"{name}.txt").write_text(prompt, encoding="utf-8")
            n_ok += 1
            continue
        if args.dry_run:
            print(f"\n===== {name} ({len(prompt)} chars) =====\n{prompt[:600]} ...")
            n_ok += 1
            continue
        try:
            resp = call_model(prompt)
        except Exception as e:
            print(f"  ! {name}: model call failed: {e}", file=sys.stderr)
            continue
        ann = parse_annotation(resp)
        ann.update({"name": name, "module": facts["module"], "addr": facts["addr"],
                    "divergences": facts["divergences"], "model": MODEL})
        with open(ANNOTATIONS, "a", encoding="utf-8") as f:
            f.write(json.dumps(ann) + "\n")
        n_ok += 1
        print(f"  ok {name}: {ann['semantic_name']} [{ann['confidence']}]", file=sys.stderr)

    where = emit_dir if emit_dir else ("(dry-run)" if args.dry_run else ANNOTATIONS)
    print(f"[=] {n_ok}/{len(names)} done -> {where}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
