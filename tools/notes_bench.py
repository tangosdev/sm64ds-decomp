#!/usr/bin/env python3
"""notes_bench.py -- measurement harness for the notes/ reorg. Read-only,
no ROM, no compiler. Per canonical question: resolves (answer_file exists
and answer_regex still matches -- FAIL means a fact was lost), hops (BFS
distance from the front-door set over markdown links / path mentions,
'unreachable' if none), path_bytes (bytes read walking that shortest path),
grep_fanout (notes/**/*.md files matching the question's key terms -- false
candidates to triage). Advisory: exit 0 unless --strict, then 1 on any FAIL.
"""
import argparse, json, os, re, sys
from collections import deque
from pathlib import Path

FRONT_DOORS = ["AGENTS.md", "CONTRIBUTING.md", "README.md", "notes/README.md"]
LINK_RE = re.compile(r'\[[^\]\n]*\]\(([^)\s]+)')
MENTION_RE = re.compile(
    r'(?<![\w/.\-])((?:\.\./)*(?:notes|config|src|include|src_tu|tools|docs|'
    r'port|symbols|nearmiss|audit)/[A-Za-z0-9_./+\-]*\.md)')
STOPWORDS = set((
    "a an the is are do does did my i we you your it its what which why how "
    "who whom this that these those and or but if then else for of in on at "
    "to from by with as be been being can could should would will shall must "
    "may might get got show before after same time vs not no yes so than "
    "also into out up down about between during while since because when "
    "where there here").split())


def load_bench(path):
    rows = []
    with open(path, encoding="utf-8") as f:
        header = f.readline().rstrip("\n").split("\t")
        for line in f:
            line = line.rstrip("\n")
            if line:
                rows.append(dict(zip(header, line.split("\t"))))
    return rows


def clean_target(raw):
    t = raw.strip().strip("`").split("#")[0]
    t = re.sub(r":\d+$", "", t).rstrip(".,;:)")
    if not t.endswith(".md") or t.startswith(("http://", "https://", "mailto:")):
        return None
    return t


def resolve_target(t, cur_dir, repo_root):
    cands = [t] if os.path.isabs(t) else [cur_dir / t, repo_root / t]
    for c in cands:
        c = Path(c)
        try:
            if c.is_file():
                return c.resolve().relative_to(repo_root.resolve()).as_posix()
        except (OSError, ValueError):
            continue
    return None


def build_graph(repo_root):
    graph, sizes = {}, {}
    for p in repo_root.rglob("*.md"):
        if ".git" in p.parts:
            continue
        rel = p.resolve().relative_to(repo_root.resolve()).as_posix()
        try:
            text = p.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        sizes[rel] = p.stat().st_size
        targets = set()
        for raw in LINK_RE.findall(text) + MENTION_RE.findall(text):
            t = clean_target(raw)
            resolved = resolve_target(t, p.parent, repo_root) if t else None
            if resolved and resolved != rel:
                targets.add(resolved)
        graph[rel] = targets
    return graph, sizes


def bfs(graph, repo_root):
    starts = [d for d in FRONT_DOORS if (repo_root / d).is_file()]
    dist = {n: 0 for n in starts}
    parent = {n: None for n in starts}
    dq = deque(starts)
    while dq:
        cur = dq.popleft()
        for nxt in graph.get(cur, ()):
            if nxt not in dist:
                dist[nxt], parent[nxt] = dist[cur] + 1, cur
                dq.append(nxt)
    return dist, parent


def path_bytes_for(target, sizes, parent, dist):
    if target not in dist:
        return None
    total, node = 0, target
    while node is not None:
        total += sizes.get(node, 0)
        node = parent.get(node)
    return total


def key_terms(question, k=3):
    words = re.findall(r"[A-Za-z0-9']+", question.lower())
    seen = []
    for w in words:
        if w not in STOPWORDS and len(w) > 2 and w not in seen:
            seen.append(w)
    seen.sort(key=lambda w: -len(w))
    return seen[:k]


def grep_fanout(question, notes_cache):
    terms = key_terms(question)
    return sum(1 for t in notes_cache.values() if terms and all(k in t for k in terms))


def check_resolves(repo_root, rel_path, pattern):
    p = repo_root / rel_path
    if not p.is_file():
        return False, "missing file"
    text = p.read_text(encoding="utf-8", errors="replace")
    # Match against a whitespace-flattened copy. Prose in notes/ gets re-wrapped
    # routinely (a TU promotion rewrites a path, the paragraph reflows), which
    # splits an answer phrase across a newline without changing a single fact.
    # Matching the raw text makes this harness fail on reflow -- a gate that
    # cries wolf on reformatting is a gate people learn to ignore. Flattening
    # keeps it sensitive to the thing it exists to catch: a fact going away.
    flat = re.sub(r"\s+", " ", text)
    try:
        rx = re.compile(pattern, re.MULTILINE | re.DOTALL)
    except re.error as e:
        return False, "bad regex: %s" % e
    return (True, "") if (rx.search(flat) or rx.search(text)) else (False, "regex did not match")


def run(repo_root, bench_rows):
    graph, sizes = build_graph(repo_root)
    dist, parent = bfs(graph, repo_root)
    notes_cache = {}
    for p in (repo_root / "notes").rglob("*.md"):
        try:
            notes_cache[str(p)] = p.read_text(encoding="utf-8", errors="replace").lower()
        except OSError:
            pass
    results = []
    for row in bench_rows:
        af = row["answer_file"]
        ok, reason = check_resolves(repo_root, af, row["answer_regex"])
        results.append({
            "id": row["id"], "answer_file": af, "resolves": ok, "reason": reason,
            "hops": dist.get(af), "path_bytes": path_bytes_for(af, sizes, parent, dist),
            "grep_fanout": grep_fanout(row["question"], notes_cache),
        })
    return results


def fmt(v, none_str):
    return none_str if v is None else str(v)


def print_table(results):
    hdr = "%-4s %-6s %-11s %-45s %-11s %-10s %s" % (
        "id", "status", "hops", "answer_file", "path_bytes", "fanout", "reason")
    print(hdr); print("-" * len(hdr))
    total_bytes, unreachable, fails = 0, 0, 0
    for r in results:
        fails += 0 if r["resolves"] else 1
        if r["hops"] is None:
            unreachable += 1
        else:
            total_bytes += r["path_bytes"]
        print("%-4s %-6s %-11s %-45s %-11s %-10s %s" % (
            r["id"], "OK" if r["resolves"] else "FAIL", fmt(r["hops"], "unreachable"),
            r["answer_file"], fmt(r["path_bytes"], "-"), r["grep_fanout"], r["reason"]))
    print("-" * len(hdr))
    print("TOTAL: %d/%d resolve, %d unreachable, sum(path_bytes over reachable)=%d"
          % (len(results) - fails, len(results), unreachable, total_bytes))


def print_compare(old, results):
    old_by_id = {r["id"]: r for r in old}
    hdr = "%-4s %-16s %-16s %-22s %s" % ("id", "resolves", "hops", "path_bytes", "grep_fanout")
    print(hdr); print("-" * len(hdr))
    d_bytes_total = 0
    for r in results:
        o = old_by_id.get(r["id"])
        if o is None:
            print("%-4s (new row, no baseline)" % r["id"]); continue
        ob, nb = o["path_bytes"], r["path_bytes"]
        db = (nb or 0) - (ob or 0) if (nb is not None and ob is not None) else 0
        d_bytes_total += db
        bytes_c = "%s->%s (%+d)" % (fmt(ob, "-"), fmt(nb, "-"), db)
        print("%-4s %-16s %-16s %-22s %s->%s" % (
            r["id"], "%s->%s" % (o["resolves"], r["resolves"]),
            "%s->%s" % (fmt(o["hops"], "unreachable"), fmt(r["hops"], "unreachable")),
            bytes_c, o["grep_fanout"], r["grep_fanout"]))
    print("-" * len(hdr))
    print("TOTAL path_bytes delta over matched rows: %+d" % d_bytes_total)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--repo", default=None, help="repo root (default: cwd)")
    ap.add_argument("--bench", default=str(Path(__file__).resolve().parent.parent / "notes" / "data" / "notes-bench.tsv"))
    ap.add_argument("--baseline", default=None, help="write a JSON snapshot to this path")
    ap.add_argument("--compare", default=None, help="diff current run against a saved snapshot")
    ap.add_argument("--strict", action="store_true", help="exit 1 if any row fails to resolve")
    args = ap.parse_args()

    repo_root = Path(args.repo).resolve() if args.repo else Path.cwd().resolve()
    results = run(repo_root, load_bench(args.bench))

    if args.compare:
        with open(args.compare, encoding="utf-8") as f:
            print_compare(json.load(f), results)
        print()
    print_table(results)
    if args.baseline:
        with open(args.baseline, "w", encoding="utf-8") as f:
            json.dump(results, f, indent=2)
        print("\nSnapshot written to %s" % args.baseline)

    sys.exit(1 if args.strict and any(not r["resolves"] for r in results) else 0)


if __name__ == "__main__":
    main()
