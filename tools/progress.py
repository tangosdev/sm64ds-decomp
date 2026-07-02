"""Report decomp completion: matched functions / bytes vs the whole game.

Totals come from the dsd config (every kind:function across all modules).
Matched functions are recorded in progress/matched.jsonl (one JSON object per
line: {"addr","name","size","module","versions"}). De-duped by addr.

Usage:
    python tools/progress.py               # full report (uses the local ledger)
    python tools/progress.py --bar         # ready-to-paste README "## Progress" block
    python tools/progress.py --write-readme  # rewrite that block in place in README.md

--bar and --write-readme deliberately do NOT use progress/matched.jsonl: that
ledger is git-ignored (local-only, per-contributor, and known to drift stale -
see notes/agent-decomp-knowledge-base.md). Instead they derive the matched set
from committed data alone (config/**/symbols.txt cross-referenced against which
functions have a src/<name>.c[pp] file), so the number is reproducible on a
fresh checkout with no ROM and no local state - which is what the hosted
update-progress.yml workflow needs.
"""
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
CONFIG = REPO / "config"
SRC = REPO / "src"
MATCHED = REPO / "progress" / "matched.jsonl"
README = REPO / "README.md"
README_START = "<!-- progress:start -->"
README_END = "<!-- progress:end -->"

FUNC_RE = re.compile(r"kind:function\((?:arm|thumb),size=0x([0-9a-fA-F]+)\)")
FUNC_NAME_RE = re.compile(r"^(\S+)\s+kind:function\((?:arm|thumb),size=0x([0-9a-fA-F]+)\)")


def totals():
    n = 0
    total_bytes = 0
    per_module = {}
    for sym in CONFIG.rglob("symbols.txt"):
        # module label: e.g. arm9, arm9/itcm, arm9/overlays/ov006
        mod = sym.parent.relative_to(CONFIG).as_posix()
        m_n = m_b = 0
        for line in sym.read_text(errors="ignore").splitlines():
            mm = FUNC_RE.search(line)
            if mm:
                sz = int(mm.group(1), 16)
                m_n += 1
                m_b += sz
        if m_n:
            per_module[mod] = (m_n, m_b)
            n += m_n
            total_bytes += m_b
    return n, total_bytes, per_module


def synced_from_src():
    """Matched set derived only from committed data: a function counts as
    matched if config declares it and src/<name>.c or src/<name>.cpp exists.
    Returns (done_n, done_b, n, total_bytes)."""
    n = total_bytes = done_n = done_b = 0
    for sym in CONFIG.rglob("symbols.txt"):
        for line in sym.read_text(errors="ignore").splitlines():
            m = FUNC_NAME_RE.match(line)
            if not m:
                continue
            name, sz = m.group(1), int(m.group(2), 16)
            n += 1
            total_bytes += sz
            if (SRC / f"{name}.c").is_file() or (SRC / f"{name}.cpp").is_file():
                done_n += 1
                done_b += sz
    return done_n, done_b, n, total_bytes


def matched():
    if not MATCHED.is_file():
        return {}
    seen = {}
    for line in MATCHED.read_text(errors="ignore").splitlines():
        line = line.strip()
        if not line:
            continue
        try:
            o = json.loads(line)
            addr = int(o["addr"], 0) if isinstance(o["addr"], str) else o["addr"]
            # key by (module, addr): overlay addresses overlap across overlays
            seen[(o.get("module", "arm9"), addr)] = o
        except Exception:
            continue
    return seen


def bar(done, tot, width=30):
    filled = round(done / tot * width) if tot else 0
    if done and filled == 0:
        filled = 1
    return "█" * filled + "░" * (width - filled)


def bar_block(done_n, n, done_b, tb):
    """The fenced ``` block shown under the README "## Progress" heading."""
    lines = [
        "```",
        f"Functions  {bar(done_n, n)}  {100*done_n/n:4.1f}%   {done_n:,} / {n:,}",
        f"Code size  {bar(done_b, tb)}  {100*done_b/tb:4.1f}%   {done_b:,} / {tb:,} bytes",
        "```",
    ]
    return "\n".join(lines)


def write_readme(done_n, n, done_b, tb):
    """Replace the text between the progress markers in README.md in place."""
    text = README.read_text(encoding="utf-8")
    start = text.index(README_START) + len(README_START)
    end = text.index(README_END)
    new_text = text[:start] + "\n" + bar_block(done_n, n, done_b, tb) + "\n" + text[end:]
    if new_text != text:
        README.write_text(new_text, encoding="utf-8")
        return True
    return False


def main():
    if "--write-readme" in sys.argv:
        done_n, done_b, n, tb = synced_from_src()
        changed = write_readme(done_n, n, done_b, tb)
        print(f"README.md {'updated' if changed else 'already up to date'}")
        return

    if "--bar" in sys.argv:
        done_n, done_b, n, tb = synced_from_src()
        # ready-to-paste README "## Progress" block; reconfigure stdout so the
        # block characters print on a Windows (cp1252) console
        try:
            sys.stdout.reconfigure(encoding="utf-8")
        except Exception:
            pass
        print("## Progress\n")
        print(bar_block(done_n, n, done_b, tb))
        return

    n, tb, per = totals()
    done = matched()
    done_n = len(done)
    # ledger sizes drift between int and hex-string across writers; accept both
    done_b = sum(int(s, 0) if isinstance(s := o.get("size", 0), str) else int(s)
                 for o in done.values())

    print("=== SM64DS decomp progress ===")
    print(f"  functions : {done_n:,} / {n:,}  ({100*done_n/n:.4f}%)")
    print(f"  code bytes: {done_b:,} / {tb:,}  ({100*done_b/tb:.4f}%)")
    print(f"  modules with code: {len(per)}")
    # biggest modules, to show where the mass is
    top = sorted(per.items(), key=lambda kv: kv[1][1], reverse=True)[:6]
    print("  largest modules (by bytes):")
    for mod, (mn, mb) in top:
        print(f"    {mod:28} {mn:5} funcs  {mb:>9,} bytes")


if __name__ == "__main__":
    main()
