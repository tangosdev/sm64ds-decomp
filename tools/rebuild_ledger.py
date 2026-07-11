"""Rebuild progress/matched.jsonl from committed src/ (the ground truth).

A fresh worktree/clone starts with an empty or near-empty progress/matched.jsonl
(it is gitignored, so `git checkout` does not populate it). The worklist builders
(worklist.py -> harvest_wl.py, refine_wl.py's unmatched() check, etc.) then treat
thousands of ALREADY-matched functions as unmatched and schedule them for a fan-out,
burning tokens re-deriving committed work.

This reconciles the local ledger with what is actually on disk, using the exact same
definition of "matched" as chaos_db_ci.py (the CI truth generator): a config symbol
whose src/<name>.{c,cpp} exists and is not marked "// NONMATCHING" in its first 200
chars. Run it once right after entering a fresh worktree, and again after any large
`git pull`/merge that brings in new src matches.

    python tools/rebuild_ledger.py

matched.jsonl is gitignored, so this only ever touches local state.
"""
import json
import pathlib
import re

REPO = pathlib.Path(__file__).resolve().parent.parent
CONFIG = REPO / "config"
SRC = REPO / "src"
MATCHED = REPO / "progress" / "matched.jsonl"

FUNC_RE = re.compile(
    r"^(\S+)\s+kind:function\((?:arm|thumb),size=0x([0-9a-fA-F]+)\).*?addr:0x([0-9a-fA-F]+)")


def module_label(sym_path):
    """config/arm9/symbols.txt -> arm9; config/arm9/overlays/ovNNN -> ovNNN."""
    rel = sym_path.parent.relative_to(CONFIG).as_posix()
    if rel == "arm9":
        return "arm9"
    m = re.fullmatch(r"arm9/overlays/(ov\d+)", rel)
    return m.group(1) if m else None


def main():
    recs, seen = [], set()
    for sym in sorted(CONFIG.rglob("symbols.txt")):
        label = module_label(sym)
        if label is None:
            continue
        for line in sym.read_text(errors="ignore").splitlines():
            m = FUNC_RE.match(line)
            if not m:
                continue
            name, size, addr = m.group(1), int(m.group(2), 16), int(m.group(3), 16)
            # A function is matched if ANY committed sibling is a real match; a stale
            # NONMATCHING draft (e.g. src/NAME.c) can shadow a landed match (NAME.cpp),
            # so never let the first extension found decide it (chaos_db_ci.py has the
            # same .c-first blind spot -- see tools/rebuild_ledger).
            srcs = [SRC / f"{name}.{ext}" for ext in ("c", "cpp")
                    if (SRC / f"{name}.{ext}").is_file()]
            if not any("NONMATCHING" not in f.read_text(errors="ignore")[:200]
                       for f in srcs):
                continue
            key = (label, addr)
            if key in seen:
                continue
            seen.add(key)
            recs.append({"module": label, "addr": f"0x{addr:08x}", "size": size,
                         "name": name, "tag": "src-rebuild"})
    MATCHED.parent.mkdir(parents=True, exist_ok=True)
    with open(MATCHED, "w", encoding="utf-8") as fh:
        for r in recs:
            fh.write(json.dumps(r) + "\n")
    print(f"rebuilt {MATCHED.relative_to(REPO)} with {len(recs)} matched records "
          f"from committed src/")


if __name__ == "__main__":
    main()
