"""Naming-pass evidence gatherer.

For an unnamed ``func_<addr>``, collect the material a human (or an LLM) needs to
propose a real name: the function's own decompiled source, the functions it calls
(callees -- especially the ones already NAMED, which pin behavior), and its call
sites (callers, with a few lines of context each). Also scores every unnamed
function so the worklist can lead with the well-constrained ones.

This does not propose names -- it assembles evidence. Naming stays a human-reviewed
judgement; a wrong name is worse than none.

Usage:
  python tools/name_evidence.py --worklist --top 40      # ranked candidates
  python tools/name_evidence.py --func func_020ce6a0     # evidence for one
  python tools/name_evidence.py --evidence-for a.txt --json out.json   # batch
"""
import argparse, json, pathlib, re, subprocess, sys
from collections import Counter

REPO = pathlib.Path(__file__).resolve().parent.parent
SRC = REPO / "src"
sys.path.insert(0, str(REPO / "tools"))
import srcpath as SP  # noqa: E402
FUNC_RE = re.compile(r"\bfunc_(?:ov\d+_)?[0-9a-fA-F]{6,8}\b")
NAMED_CALL_RE = re.compile(r"\b(_ZN[0-9A-Za-z_]+|[A-Z][A-Za-z0-9_]*::[A-Za-z0-9_]+|[A-Za-z_][A-Za-z0-9_]*)\s*\(")


def src_path(sym):
    return SP.path_for(sym)


def callers(sym):
    """Files that reference sym, excluding its own definition file."""
    out = subprocess.run(["git", "grep", "-l", "-F", sym, "--", "src/"],
                         cwd=REPO, capture_output=True, text=True).stdout
    # Ask srcpath where the definition actually is rather than assuming src/<sym>.<ext>:
    # once a symbol lives in a subdirectory the flat guess stops matching and the symbol
    # is reported as a caller of itself.
    own = {p.relative_to(REPO).as_posix() for p in SP.paths_for(sym)}
    return [l for l in out.splitlines() if l.strip() and l not in own]


def named_callees(text):
    """Named (non-func_<addr>) symbols this source calls -- the behavior anchors."""
    calls = set()
    for m in NAMED_CALL_RE.finditer(text):
        n = m.group(1)
        if n.startswith(("if", "for", "while", "switch", "return", "sizeof")):
            continue
        if FUNC_RE.fullmatch(n):
            continue
        if n.startswith(("_ZN", "_ZL")) or "::" in n or n[0].isupper():
            calls.add(n)
    return sorted(calls)


def evidence(sym):
    p = src_path(sym)
    if not p:
        return {"func": sym, "error": "no src file"}
    text = p.read_text(errors="ignore")
    cs = callers(sym)
    # a couple call-site context lines from up to 3 callers
    ctx = []
    for cf in cs[:3]:
        try:
            lines = (REPO / cf).read_text(errors="ignore").splitlines()
        except OSError:
            continue
        for i, ln in enumerate(lines):
            if sym in ln:
                ctx.append({"file": cf, "line": ln.strip()[:200]})
                break
    return {
        "func": sym,
        "src": str(p.relative_to(REPO)),
        "size_lines": text.count("\n") + 1,
        "source": text,
        "named_callees": named_callees(text),
        "n_callers": len(cs),
        "call_sites": ctx,
    }


def score(sym):
    """Higher = better first target: many callers + calls named functions + small."""
    p = src_path(sym)
    if not p:
        return -1, {}
    text = p.read_text(errors="ignore")
    nc = len(callers(sym))
    anchors = len(named_callees(text))
    lines = text.count("\n") + 1
    s = nc * 2 + anchors * 3 - max(0, lines - 60) // 20
    return s, {"n_callers": nc, "named_callees": anchors, "lines": lines}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--worklist", action="store_true")
    ap.add_argument("--top", type=int, default=40)
    ap.add_argument("--func")
    ap.add_argument("--evidence-for", help="file with one func_<addr> per line")
    ap.add_argument("--json")
    args = ap.parse_args()

    if args.func:
        print(json.dumps(evidence(args.func), indent=2))
        return
    if args.evidence_for:
        syms = [l.strip() for l in open(args.evidence_for) if l.strip()]
        out = [evidence(s) for s in syms]
        (open(args.json, "w") if args.json else sys.stdout).write(json.dumps(out, indent=2))
        return
    if args.worklist:
        syms = sorted(set(m.group(0) for f in SP.iter_sources()
                          for m in [FUNC_RE.fullmatch(f.stem)] if m))
        scored = []
        for s in syms:
            sc, meta = score(s)
            if sc >= 0:
                scored.append((sc, s, meta))
        scored.sort(reverse=True)
        for sc, s, meta in scored[:args.top]:
            print(f"{sc:4}  {s:28} callers={meta['n_callers']:3} anchors={meta['named_callees']:2} lines={meta['lines']}")
        if args.json:
            json.dump([{"func": s, "score": sc, **meta} for sc, s, meta in scored], open(args.json, "w"))


if __name__ == "__main__":
    main()
