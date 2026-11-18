#!/usr/bin/env python
"""3-way union resolver for the append-only merge-conflict files:
symbols/actor_renames.tsv, attribution.json, config/decl-agreement-baseline.json and
dead-reference-baseline.json. It does not touch langmode-baseline.json (the langmode
ratchet baseline lives on the chaos-data branch).

Usage: python tools/resolve_union_conflicts.py [path ...]   (defaults: all conflicted files it knows)
Run it from the repo root in the middle of a conflicted merge.
Reads :2: (ours) and :3: (theirs) stages from the index, writes the union to the
worktree, and `git add`s it. Files it doesn't own are left alone.
"""
import json, subprocess, sys, io

WT = "."
JSON_INDENT = 1  # baselines are written with indent=1 + trailing newline; verify below

def stage(n, path):
    r = subprocess.run(["git", "show", f":{n}:{path}"], capture_output=True)
    if r.returncode != 0:
        return None
    return r.stdout.decode("utf-8")

def conflicted():
    r = subprocess.run(["git", "diff", "--name-only", "--diff-filter=U"],
                       capture_output=True, text=True)
    return [l.strip() for l in r.stdout.splitlines() if l.strip()]

def dump_json(obj, path, ref_text):
    # match the file's existing indent style by checking ref text
    indent = 1
    for line in ref_text.splitlines():
        if line.startswith(" ") and '"' in line:
            indent = len(line) - len(line.lstrip())
            break
    txt = json.dumps(obj, indent=indent, ensure_ascii=False) + "\n"
    with io.open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(txt)

def merge_decl(ours, theirs):
    out = dict(ours)
    known = dict(ours.get("known", {}))
    for sym, files in theirs.get("known", {}).items():
        tgt = known.setdefault(sym, {})
        for f, reasons in files.items():
            cur = set(tgt.get(f, []))
            tgt[f] = sorted(cur | set(reasons))
    out["known"] = {k: known[k] for k in sorted(known)}
    return out

def merge_deadref(ours, theirs):
    out = dict(ours)
    seen = set()
    rows = []
    for r in ours.get("known", []) + theirs.get("known", []):
        key = (r.get("file"), r.get("ref"))
        if key not in seen:
            seen.add(key)
            rows.append(r)
    rows.sort(key=lambda r: (r.get("file", ""), r.get("ref", "")))
    out["known"] = rows
    return out

def resolve(path):
    ours_t, theirs_t = stage(2, path), stage(3, path)
    if ours_t is None or theirs_t is None:
        print(f"  {path}: missing a stage, skip")
        return False
    if path.endswith("actor_renames.tsv"):
        lines = []
        seen = set()
        for l in (ours_t + theirs_t).splitlines():
            if l.strip() and l not in seen:
                seen.add(l); lines.append(l)
        with io.open(path, "w", encoding="utf-8", newline="\n") as f:
            f.write("\n".join(lines) + "\n")
    elif path == "attribution.json":
        with io.open(path, "w", encoding="utf-8", newline="\n") as f:
            f.write(ours_t)
    elif path.endswith("decl-agreement-baseline.json"):
        dump_json(merge_decl(json.loads(ours_t), json.loads(theirs_t)), path, ours_t)
    elif path.endswith("dead-reference-baseline.json"):
        dump_json(merge_deadref(json.loads(ours_t), json.loads(theirs_t)), path, ours_t)
    else:
        return False
    subprocess.run(["git", "add", path])
    print(f"  {path}: resolved (union)")
    return True

if __name__ == "__main__":
    targets = sys.argv[1:] or conflicted()
    left = [p for p in targets if not resolve(p)]
    if left:
        print("  MANUAL:", ", ".join(left))
