"""Report decomp completion: matched functions / bytes vs the whole game.

Totals come from the dsd config (every kind:function across all modules, less
the zero-size alias records, which are second names for functions already
counted at the same address -- see bytegate.is_zero_size_alias).
Matched functions are recorded in progress/matched.jsonl (one JSON object per
line: {"addr","name","size","module","versions"}). De-duped by addr.

Usage:
    python tools/progress.py               # full report (uses the local ledger)
    python tools/progress.py --bar         # ready-to-paste README "## Progress" block
    python tools/progress.py --bar --from-src  # ignore any ambient chaos-db.json
    python tools/progress.py --write-readme  # rewrite that block in place in README.md

--bar and --write-readme deliberately do NOT use progress/matched.jsonl: that
ledger is git-ignored (local-only, per-contributor, and known to drift stale).
Instead they derive the matched set
from committed data alone (config/**/symbols.txt cross-referenced through srcpath,
then filtered by the shared byte-gate policy), so the number is reproducible on a
fresh checkout with no ROM and no local state - which is what the hosted
update-chaos-data.yml workflow needs.
"""
import json
import asm_policy  # noqa: E402
import bytegate as BG  # noqa: E402
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
CONFIG = REPO / "config"
SRC = REPO / "src"
sys.path.insert(0, str(REPO / "tools"))
import srcpath as SP  # noqa: E402
import relocs as RL  # noqa: E402
MATCHED = REPO / "progress" / "matched.jsonl"
README = REPO / "README.md"
README_START = "<!-- progress:start -->"
README_END = "<!-- progress:end -->"

FUNC_NAME_RE = re.compile(
    r"^(\S+)\s+kind:function\((?:arm|thumb),size=0x([0-9a-fA-F]+)\)"
    r".*?addr:0x([0-9a-fA-F]+)"
)


def source_counts_as_matched(path, src_path, module, addr, size,
                             alias_addrs, excluded_paths):
    """Apply the same committed-data MATCHED policy as ``chaos_db_ci``."""
    text = path.read_text(errors="ignore")
    countable = (not asm_policy.has_draft_banner(text)
                 and asm_policy.classify(text) != "transcribed")
    zero_alias = BG.is_zero_size_alias(module, addr, size, alias_addrs)
    return countable and not zero_alias and src_path not in excluded_paths


def totals():
    n = 0
    total_bytes = 0
    per_module = {}
    alias_addrs = BG.alias_collision_addresses()
    # relocs.module_universe rather than a local rglob, so the module label this asks
    # alias_addrs about is the same label alias_addrs was built with. The display key
    # below stays the config-relative path it always was.
    for sym, label in RL.module_universe():
        # module label: e.g. arm9, arm9/itcm, arm9/overlays/ov006
        mod = sym.parent.relative_to(CONFIG).as_posix()
        m_n = m_b = 0
        for line in sym.read_text(errors="ignore").splitlines():
            mm = FUNC_NAME_RE.match(line)
            if mm:
                sz, addr = int(mm.group(2), 16), int(mm.group(3), 16)
                if BG.is_zero_size_alias(label, addr, sz, alias_addrs):
                    continue  # a second name, not a second function -- see synced_from_src
                m_n += 1
                m_b += sz
        if m_n:
            per_module[mod] = (m_n, m_b)
            n += m_n
            total_bytes += m_b
    return n, total_bytes, per_module


def synced_from_src():
    """Matched set derived only from committed data: a function counts as
    matched if config declares it, srcpath resolves a source, and the committed-data
    byte gate accepts that source/record.
    Returns (done_n, done_b, n, total_bytes)."""
    n = total_bytes = done_n = done_b = 0
    alias_addrs = BG.alias_collision_addresses()
    excluded_paths = BG.excluded_paths()
    # Every module, itcm included, via the one definition in relocs.py. This used
    # to skip itcm/dtcm to agree with chaos-db and the treemap, which skipped them
    # too, so all the surfaces agreed on a number that left out 43 real functions
    # and 24344 bytes of real game code. They agree again, on the honest figure:
    # counting itcm moves the published rate from 92.480% to 91.580%.
    #
    # The denominator then had a second honesty problem, fixed the same way on
    # 2026-09-06. Ten symbols in config are zero-size aliases: each one shares its
    # address with a sized function that is already in this loop, so `_dmul` and
    # func_01ff8708 are one body under two names. The numerator has always refused
    # them (source_counts_as_matched, above) because a zero-length range cannot be
    # byte-compared against anything -- but `n += 1` counted them anyway, so ten
    # records sat in the denominator that no amount of decompilation could ever
    # clear. They are dropped from both sides now, using the numerator's own
    # predicate rather than a name list, so a config fix that gives one a real size
    # brings it straight back. Measured at this commit's base: the count was
    # 11,304 / 11,402 = 99.141%; it is 11,304 / 11,392 = 99.228%, and the work left
    # is 88 functions, not 98. No function's matched status changes and no byte total
    # moves, because all ten records are size 0.
    for sym, _label in RL.module_universe():
        for line in sym.read_text(errors="ignore").splitlines():
            m = FUNC_NAME_RE.match(line)
            if not m:
                continue
            name, sz, addr = m.group(1), int(m.group(2), 16), int(m.group(3), 16)
            if BG.is_zero_size_alias(_label, addr, sz, alias_addrs):
                continue
            n += 1
            total_bytes += sz
            f = SP.path_for(name)
            if f is not None:
                src_path = f.relative_to(REPO).as_posix()
                if source_counts_as_matched(
                        f, src_path, _label, addr, sz, alias_addrs, excluded_paths):
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


def from_db(path):
    """Read counts from chaos-db.json - the canonical source the Atlas and treemap also read - so
    every progress surface reports ONE number. Returns (done_n, done_b, n, tb, per_module_totals)."""
    db = json.loads(pathlib.Path(path).read_text(encoding="utf-8"))
    fns = db["functions"] if isinstance(db, dict) else db
    st = db.get("stats", {}) if isinstance(db, dict) else {}
    n = st.get("totalFunctions") or len(fns)
    done_n = st.get("matchedFunctions")
    if done_n is None:
        done_n = sum(1 for f in fns if f.get("matched"))
    tb = st.get("totalBytes") or sum(int(f.get("size", 0)) for f in fns)
    done_b = st.get("matchedBytes")
    if done_b is None:
        done_b = sum(int(f.get("size", 0)) for f in fns if f.get("matched"))
    per = {}
    for f in fns:
        mod = f.get("module", "?")
        mn, mb = per.get(mod, (0, 0))
        per[mod] = (mn + 1, mb + int(f.get("size", 0)))
    return done_n, done_b, n, tb, per


def _db_path():
    """Select an explicit DB, ambient DB, or the committed-source scan.

    ``--from-src`` is the deterministic escape hatch for authority/reporting callers:
    it ignores a stale local ``chaos-db.json`` that would otherwise win.
    """
    if "--from-src" in sys.argv:
        return None
    if "--from-db" in sys.argv:
        i = sys.argv.index("--from-db")
        if i + 1 < len(sys.argv):
            p = pathlib.Path(sys.argv[i + 1])
            return p if p.is_file() else None
    p = REPO / "chaos-db.json"
    return p if p.is_file() else None


def main():
    dbp = _db_path()  # ONE source of truth: chaos-db.json (the Atlas/treemap read the same file)

    if "--write-readme" in sys.argv:
        done_n, done_b, n, tb = (from_db(dbp)[:4] if dbp else synced_from_src())
        changed = write_readme(done_n, n, done_b, tb)
        print(f"README.md {'updated' if changed else 'already up to date'}")
        return

    if "--bar" in sys.argv:
        done_n, done_b, n, tb = (from_db(dbp)[:4] if dbp else synced_from_src())
        # ready-to-paste README "## Progress" block; reconfigure stdout so the
        # block characters print on a Windows (cp1252) console
        try:
            sys.stdout.reconfigure(encoding="utf-8")
        except Exception:
            pass
        print("## Progress\n")
        print(bar_block(done_n, n, done_b, tb))
        return

    if dbp:
        done_n, done_b, n, tb, per = from_db(dbp)
    else:
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
