"""Record how each match landed, and every near-miss step along the way.

Three stores are meant to describe matching work. Only one was ever filled:

  nearmiss/db.jsonl              best tip C per function      populated
  config/match_provenance.jsonl  final HOW, once matched      was empty
  config/match_attempts.jsonl    every try, including misses   was empty

Both empty ones were manual per-function steps that nobody ran. Reconstructing them
later does not work: only ~13% of the 10,711 matched functions landed in a commit that
names a method, and the rest arrived in early bulk commits like "Add matched functions,
taking progress to 25.9%". So the method has to be captured as work lands or it is gone.

This runs in CI over whatever range was just pushed, so it covers every landing no
matter who pushed it. Evidence comes from three places, best first:

  1. a `Provenance:` trailer in the commit message, which anyone can state explicitly
  2. the near-miss DB `source` tag, which already names the model (`fanout-glm-5.2`)
  3. the commit subject, which names the model on most agent batches

IT DECLINES TO GUESS. A row is written only when one of those actually establishes a
method, or the source is hand-written assembly, which is itself the method. Otherwise
the function is left unrecorded, so the ledger reads as "not captured" rather than
asserting a model and an effort level nobody can vouch for.

  python tools/stamp_landed.py --dry-run
  python tools/stamp_landed.py --range origin/main~40..origin/main
"""
import argparse
import json
import os
import pathlib
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import match_provenance as MP  # noqa: E402
import match_attempts as MA  # noqa: E402

BANNER = "// NONMATCHING"
DB = "nearmiss/db.jsonl"

# First hit wins, so specific patterns sit above generic ones. Used against both the
# commit subject and the near-miss DB source tag, which share this vocabulary.
MODELS = [
    (r"glm[- ]?5\.2", "glm-5.2"),
    (r"\bglm\b", "glm"),
    (r"\bopus\b", "opus-4.8"),
    (r"\bsonnet\b", "sonnet-5"),
    (r"\bfable\b", "fable"),
    (r"deepseek[- ]?v4[- ]?pro", "deepseek-v4-pro"),
    (r"\bdeepseek\b", "deepseek"),
    (r"tangos\(gpt\)|gpt-?5\.6", "gpt-5.6-luna"),
    (r"\bgrok\b", "grok"),
    (r"\bcodex\b", "codex"),
]
HARNESSES = [
    (r"^tangos\(", "tangos-console"),
    (r"\brefine\b", "refine"),
    (r"\bpermuter\b", "permuter"),
    (r"\bghidra\b", "ghidra"),
    (r"fan-?out|\bharvest\b|\bsweep\b|\bbatch\b|\bprobe\b", "fanout"),
]
REASONING = [(r"\bhigh\b", "high"), (r"\bmedium\b", "medium"), (r"\blow\b", "low")]


def sh(*args: str) -> str:
    return subprocess.run(args, cwd=REPO, capture_output=True, text=True,
                          encoding="utf-8", errors="replace").stdout


def blob(rev: str, path: str) -> str:
    return sh("git", "show", f"{rev}:{path}")


def first(table, text, default=None):
    for pattern, value in table:
        if re.search(pattern, text):
            return value
    return default


def parse_trailer(message: str) -> dict | None:
    """`Provenance: ai model=grok-4.5 reasoning=high harness=grok-build` or `human`.

    An explicit statement by whoever did the work always beats anything inferred, and
    it is the only way a hand-written match gets recorded as anything but unlabelled.
    """
    m = re.search(r"^\s*Provenance:\s*(.+)$", message, re.M | re.I)
    if not m:
        return None
    body = m.group(1).strip()
    if body.lower().startswith("human"):
        note = body[5:].strip(" :-") or None
        return {"kind": "human", **({"note": note} if note else {})}
    fields = dict(re.findall(r"(\w+)\s*=\s*([^\s]+)", body))
    if not fields.get("model"):
        return None
    return {"kind": "ai", "model": fields["model"],
            "reasoning": fields.get("reasoning", "unspecified"),
            "harness": fields.get("harness", "unspecified")}


def derive(subject: str, src_text: str, message: str = "") -> dict | None:
    """The how-record for a landing, or None when it cannot be established."""
    stated = parse_trailer(message or subject)
    if stated:
        return stated
    if "HAND-ASM" in src_text[:400]:
        return {"kind": "human", "note": "hand-written assembly"}
    low = subject.lower()
    model = first(MODELS, low)
    if not model:
        return None
    return {"kind": "ai", "model": model,
            "reasoning": first(REASONING, low, "unspecified"),
            "harness": first(HARNESSES, low, "unspecified")}


def from_source_tag(tag: str) -> dict | None:
    """Method from a near-miss DB source tag, e.g. `fanout-glm-5.2`."""
    low = (tag or "").lower()
    model = first(MODELS, low)
    if not model:
        return None
    return {"kind": "ai", "model": model, "reasoning": "unspecified",
            "harness": first(HARNESSES, low, "fanout")}


def symbol_index() -> dict[str, tuple[str, int, int]]:
    out: dict[str, tuple[str, int, int]] = {}
    pattern = re.compile(
        r"^(\S+)\s+kind:function\([^)]*size=(0x[0-9a-fA-F]+)\)\s+addr:(0x[0-9a-fA-F]+)")
    for path in (REPO / "config").rglob("symbols.txt"):
        label = "arm9" if "overlays" not in str(path) else path.parent.name
        for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
            m = pattern.match(line)
            if m:
                out[m.group(1)] = (label, int(m.group(3), 16), int(m.group(2), 16))
    return out


def landings(base: str, head: str) -> list[tuple[str, str, str, str]]:
    """(path, subject, full message, blob) per src file that BECAME matched in range.

    Full history, because `git log -- <path>` prunes commits reached through a merge and
    would silently drop matches that landed on a side branch.
    """
    REC, FLD, END = "\x01", "\x02", "\x03"
    out = sh("git", "log", "--full-history", "--reverse",
             f"--format={REC}%H{FLD}%s{FLD}%B{END}", "--name-status", "-M",
             f"{base}..{head}", "--", "src/")
    blocks = out.split(REC)
    seen: dict[str, tuple[str, str, str]] = {}
    for block in blocks[1:]:
        header, _, files = block.partition(END)
        parts = header.split(FLD)
        if len(parts) < 3:
            continue
        sha, subject, message = parts[0], parts[1], parts[2]
        for line in files.splitlines():
            if not line.strip():
                continue
            cols = line.split("\t")
            if len(cols) < 2 or cols[0].startswith("D"):
                continue
            path = cols[-1].strip()
            if not path.endswith((".c", ".cpp")):
                continue
            text = blob(sha, path)[:400]
            if not text or BANNER in text:
                continue
            was = blob(base, path)[:400]
            if was and BANNER not in was:
                continue                  # already matched before this range
            seen.setdefault(path, (subject, message, text))
    return [(p, s, m, t) for p, (s, m, t) in seen.items()]


def db_at(rev: str) -> dict[tuple[str, str], dict]:
    out: dict[tuple[str, str], dict] = {}
    for line in blob(rev, DB).splitlines():
        if line.strip():
            r = json.loads(line)
            out[(r["module"], str(r["addr"]))] = r
    return out


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--range", default=None)
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    before = os.environ.get("GITHUB_EVENT_BEFORE", "")
    rng = args.range or (
        f"{before}..{os.environ['GITHUB_SHA']}"
        if before.strip("0") and os.environ.get("GITHUB_SHA") else "HEAD~1..HEAD")
    base, _, head = rng.partition("..")
    base, head = base or "HEAD~1", head or "HEAD"

    syms = symbol_index()
    ledger = MP.load_ledger()
    landed = landings(base, head)
    scope = "batch" if len(landed) > 1 else "focused"

    # existing attempt rows, so a manual re-run over the same range cannot duplicate
    try:
        prior = {(r.get("functionId"), r.get("status"), r.get("divergences"),
                  r.get("model")) for r in MA.load_attempts()}
    except Exception:
        prior = set()

    stamped = attempts = unlabelled = skipped = 0

    for path, subject, message, text in landed:
        name = path.split("/")[-1].rsplit(".", 1)[0]
        info = syms.get(name)
        if not info:
            continue
        module, addr, _ = info
        prov = derive(subject, text, message)
        if not prov:
            unlabelled += 1
            continue
        key = (MP.make_id(module, addr), "matched", None, prov.get("model"))
        if MP.make_id(module, addr) in ledger:
            skipped += 1
        elif args.dry_run:
            print(f"  provenance {name}  {json.dumps(prov)}")
            stamped += 1
        else:
            try:
                MP.append_ledger_row(module=module, addr=addr, name=name,
                                     provenance=prov, src_path=path)
                stamped += 1
            except MP.ProvenanceError as exc:
                print(f"  skip {name}: {exc}", file=sys.stderr)
                continue
        if key in prior:
            continue
        if args.dry_run:
            print(f"  attempt    {name}  matched")
            attempts += 1
        else:
            try:
                MA.append_attempt(module=module, addr=addr, name=name, status="matched",
                                  kind=prov["kind"], model=prov.get("model"),
                                  reasoning=prov.get("reasoning"),
                                  harness=prov.get("harness"), src_path=path,
                                  session_scope=scope, batch_size=max(len(landed), 1))
                attempts += 1
            except Exception as exc:
                print(f"  attempt skip {name}: {exc}", file=sys.stderr)

    # Near-miss steps: a draft that got closer is a real attempt, and the DB source tag
    # names the model that did it. Without this the tree would only ever hold successes.
    try:
        old_db, new_db = db_at(base), db_at(head)
    except Exception:
        old_db = new_db = {}
    steps = []
    for key, row in new_db.items():
        div = row.get("divergences")
        if not isinstance(div, int):
            continue
        prev = (old_db.get(key) or {}).get("divergences")
        if isinstance(prev, int) and div >= prev:
            continue                      # unchanged or worse: not a step forward
        method = from_source_tag(row.get("source") or "")
        info = syms.get(row["name"])
        if not method or not info:
            continue
        module, addr, _ = info
        if (MP.make_id(module, addr), "near_miss", div, method["model"]) in prior:
            continue
        steps.append((module, addr, row["name"], div, prev, method))

    # Scope describes THIS push's near-miss work, so it counts the drafts that actually
    # moved -- not the DB's row count, which barely changes when entries only improve.
    nm_scope = "batch" if len(steps) > 1 else "focused"
    nm = 0
    for module, addr, name, div, prev, method in steps:
        if args.dry_run:
            print(f"  attempt    {name}  near_miss {prev} -> {div}  {method['model']}")
            nm += 1
            continue
        try:
            MA.append_attempt(module=module, addr=addr, name=name,
                              status="near_miss", kind="ai", model=method["model"],
                              reasoning=method["reasoning"], harness=method["harness"],
                              divergences=div, prev_best_divergences=prev,
                              improved_near_miss=True, session_scope=nm_scope,
                              batch_size=len(steps),
                              base_kind="near_miss_draft" if prev is not None else "scratch")
            nm += 1
        except Exception as exc:
            print(f"  near-miss skip {name}: {exc}", file=sys.stderr)

    print(f"provenance: stamped {stamped}, already recorded {skipped}, "
          f"unlabelled {unlabelled}; attempts: {attempts} matched, {nm} near-miss ({rng})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
