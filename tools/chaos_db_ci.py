"""CI-safe Chaos Viewer data generator: rebuilds chaos-db.json from COMMITTED
data only (no ROM, no local ledger), so GitHub Actions can refresh the
chaos-data branch on every push and the hosted viewer always shows current
modules and percentages.

Derived the same way as progress.py --write-readme:
  universe   config/**/symbols.txt  (name, addr, size per module)
  matched    src/<name>.c[pp] exists and is not marked // NONMATCHING
  near-miss  nearmiss/db.jsonl (committed) -> div badge
  author     git history: the author of the commit that ADDED each src file
             (login from users.noreply emails when available, else author name)
  project    tools/chaosviewer.config.json (committed branding/prompt config)

Not derivable without the ROM (left to local regens): disasm/callee detail
chunks, coddog sim/sibling. The details/ directory on the chaos-data branch is
preserved as-is by the workflow.

Usage: python tools/chaos_db_ci.py [--out chaos-db.json]
"""
import argparse
import json
import pathlib
import re
import subprocess
import time

REPO = pathlib.Path(__file__).resolve().parent.parent
CONFIG = REPO / "config"
SRC = REPO / "src"

FUNC_RE = re.compile(
    r"^(\S+)\s+kind:function\((?:arm|thumb),size=0x([0-9a-fA-F]+)\).*?addr:0x([0-9a-fA-F]+)")
LOGIN_RE = re.compile(r"^(?:\d+\+)?([^@]+)@users\.noreply\.github\.com$")


def module_label(sym_path: pathlib.Path) -> str | None:
    """config/arm9/symbols.txt -> arm9; config/arm9/overlays/ovNNN -> ovNNN.
    itcm/dtcm are skipped to match the viewer's module universe."""
    rel = sym_path.parent.relative_to(CONFIG).as_posix()
    if rel == "arm9":
        return "arm9"
    m = re.fullmatch(r"arm9/overlays/(ov\d+)", rel)
    return m.group(1) if m else None


def src_authors() -> dict[str, str]:
    """{'src/name.c': handle} from the commit that first added each file."""
    out = subprocess.run(
        ["git", "log", "--diff-filter=A", "--format=%x01%an%x02%ae", "--name-only", "--", "src/"],
        cwd=REPO, capture_output=True, text=True, encoding="utf-8", errors="replace").stdout
    authors: dict[str, str] = {}
    handle = None
    for line in out.splitlines():
        if line.startswith("\x01"):
            name, _, email = line[1:].partition("\x02")
            email = email.strip()
            m = LOGIN_RE.match(email)
            # identity = noreply login, else the email local-part: stable across
            # git author-name typos, and usually equals the GitHub handle
            handle = m.group(1) if m else (email.split("@")[0].lower() or name.strip())
        elif line.startswith("src/") and handle:
            # log is newest-first; keep the FIRST seen (most recent add wins
            # for files that were re-added)
            authors.setdefault(line.strip(), handle)
    return authors


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default="chaos-db.json")
    args = ap.parse_args()

    nm = {}
    nm_path = REPO / "nearmiss" / "db.jsonl"
    if nm_path.is_file():
        for l in nm_path.read_text(encoding="utf-8", errors="ignore").splitlines():
            if l.strip():
                try:
                    r = json.loads(l)
                    a = r["addr"]
                    nm[(r["module"], int(a, 0) if isinstance(a, str) else a)] = r
                except Exception:
                    continue

    authors = src_authors()

    functions = []
    total_b = matched_b = matched_n = 0
    for sym in sorted(CONFIG.rglob("symbols.txt")):
        label = module_label(sym)
        if label is None:
            continue
        for line in sym.read_text(errors="ignore").splitlines():
            m = FUNC_RE.match(line)
            if not m:
                continue
            name, size, addr = m.group(1), int(m.group(2), 16), int(m.group(3), 16)
            src_path = None
            for ext in ("c", "cpp"):
                f = SRC / f"{name}.{ext}"
                if f.is_file():
                    src_path = f"src/{name}.{ext}"
                    break
            matched = bool(src_path) and "NONMATCHING" not in (
                (SRC / src_path.split("/", 1)[1]).read_text(errors="ignore")[:200] if src_path else "")
            total_b += size
            rec = {"id": f"{label}:0x{addr:08x}", "module": label, "name": name,
                   "addr": addr, "size": size, "matched": matched}
            if src_path:
                rec["srcPath"] = src_path
                if matched and src_path in authors:
                    rec["author"] = authors[src_path]
            if matched:
                matched_b += size
                matched_n += 1
            else:
                r = nm.get((label, addr))
                if r and r.get("divergences") is not None:
                    rec["div"] = r["divergences"]
            functions.append(rec)

    project = None
    pc = REPO / "tools" / "chaosviewer.config.json"
    if pc.is_file():
        project = json.loads(pc.read_text(encoding="utf-8"))

    db = {
        "generatedAt": time.strftime("%Y-%m-%d %H:%M", time.gmtime()) + " UTC",
        "project": project,
        "stats": {
            "totalFunctions": len(functions),
            "matchedFunctions": matched_n,
            "totalBytes": total_b,
            "matchedBytes": matched_b,
            "moduleCount": len({f["module"] for f in functions}),
        },
        "functions": functions,
    }
    out = pathlib.Path(args.out)
    out.write_text(json.dumps(db), encoding="utf-8")
    print(f"wrote {out} ({out.stat().st_size // 1024} KB): "
          f"{matched_n}/{len(functions)} funcs, {matched_b}/{total_b} bytes, "
          f"{db['stats']['moduleCount']} modules, "
          f"{sum(1 for f in functions if 'author' in f)} authored")


if __name__ == "__main__":
    main()
