"""Turn a worklist (or a nearmiss filter) into a breakpoint-target list.

A target dict is:
    {"name", "addr" (int), "module", "size", "canary" (16 hex chars)}

The canary is target_hex[:16] -- the first 8 bytes of the function. On a bp hit
we read 8 bytes at addr and compare; a mismatch means a DIFFERENT overlay is
mapped there (address aliasing) and the hit is discarded. melonDS keeps
breakpoints as a checked PC-list (not a RAM patch), so bps survive overlay
reloads -- residency only affects whether the code runs, i.e. coverage.
"""
import json
import pathlib

REPO = pathlib.Path(__file__).resolve().parents[2]
DB = REPO / "nearmiss" / "db.jsonl"


def _iter_db():
    for line in DB.read_text(encoding="utf-8", errors="replace").splitlines():
        if not line.strip():
            continue
        try:
            yield json.loads(line)
        except json.JSONDecodeError:
            continue


def _target(r):
    return {
        "name": r["name"],
        "addr": int(str(r["addr"]), 16),
        "module": r.get("module", "arm9"),
        "size": r.get("size"),
        "canary": r["target_hex"][:16].lower(),
    }


def from_names(names):
    """Targets for an explicit list of function names (skips unknowns)."""
    want = {n.strip() for n in names if n.strip()}
    by_name = {r["name"]: r for r in _iter_db()}
    out, missing = [], []
    for n in want:
        if n in by_name:
            out.append(_target(by_name[n]))
        else:
            missing.append(n)
    return out, missing


def from_worklist(path):
    """Read names from a worklist file: one per line, or a JSON list, or a
    JSONL of records with a 'name' field. '#' comments and blanks ignored."""
    text = pathlib.Path(path).read_text(encoding="utf-8", errors="replace").strip()
    names = []
    if text.startswith("["):
        for item in json.loads(text):
            names.append(item if isinstance(item, str) else item.get("name"))
    else:
        for line in text.splitlines():
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            if line.startswith("{"):
                try:
                    names.append(json.loads(line).get("name"))
                    continue
                except json.JSONDecodeError:
                    pass
            names.append(line.split()[0])
    return from_names([n for n in names if n])


def from_nearmiss(min_div=None, module=None, limit=None):
    """Targets straight from the nearmiss DB, filtered."""
    out = []
    for r in _iter_db():
        if min_div is not None and r.get("divergences", 0) < min_div:
            continue
        if module is not None and r.get("module", "arm9") != module:
            continue
        out.append(_target(r))
    # densest-first so a fixed run captures the highest-divergence funcs earliest
    out.sort(key=lambda t: -(t["size"] or 0))
    if limit:
        out = out[:limit]
    return out


def group_by_module(targets):
    g = {}
    for t in targets:
        g.setdefault(t["module"], []).append(t)
    return g
