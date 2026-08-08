"""Batch-verify the cleaned-up mangled template functions still byte-match.

Reads progress/matched.jsonl, selects every entry whose first version is a
"template:..." tag and whose symbol is Itanium-mangled (_Z...), then re-runs the
canonical single-version match for each and prints a pass/fail summary.

NOT EVERY LEDGER ROW CARRIES VERSION PROVENANCE, and this tool used to crash on the
ones that do not. progress/matched.jsonl has two writers with different row shapes:
the banking paths append {"versions": [...]}, while tools/rebuild_ledger.py reconstructs
the whole file from committed src/ and writes {"tag": "src-rebuild"} instead -- it works
from what is on disk and genuinely cannot know which compiler version each function was
matched at. Indexing d["versions"] therefore raised KeyError on every row of a rebuilt
ledger, which is the normal state after a fresh worktree or a large merge.

Selecting nothing is now reported, not silently treated as success: a ledger with no
version provenance cannot answer the question this tool asks, and printing "0 pass,
0 fail" would read as an all-clear. Rebuild provenance by re-banking, or verify the
functions you care about directly with tools/match.py.

Exit codes: 0 all selected functions matched, 1 at least one failed, 2 nothing could
be selected (see above).

Usage:
    python tools/verify_mangled.py            # verify all mangled template funcs
    python tools/verify_mangled.py _ZN5Actor  # verify only names matching a substring
"""
import json
import pathlib
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
MATCHED = REPO / "progress" / "matched.jsonl"
sys.path.insert(0, str(REPO / "tools"))
import srcpath as SP  # noqa: E402
VERSION = "1.2/sp2p3"


def overlay_bin_and_base(module: str):
    """For an overlay module like 'ov002', return (bin_path, text_base_addr).

    The raw decompressed overlay lives in extracted/overlays/overlay_NNNN.bin and
    its .text load address is the start of the first segment in the module's
    delinks.txt. Returns (None, None) for the main arm9 module.
    """
    if not module.startswith("ov"):
        return None, None
    num = int(module[2:])
    binp = REPO / "extracted" / "overlays" / f"overlay_{num:04d}.bin"
    delinks = REPO / "config" / "arm9" / "overlays" / module / "delinks.txt"
    base = None
    for line in delinks.read_text().splitlines():
        if "start:" in line:
            base = int(line.split("start:")[1].split()[0], 0)
            break
    return binp, base

filt = sys.argv[1] if len(sys.argv) > 1 else ""

rows = []
total = no_provenance = 0
for line in MATCHED.read_text().splitlines():
    if not line.strip():
        continue
    d = json.loads(line)
    total += 1
    # rebuild_ledger.py rows carry "tag" instead of "versions" -- see the module docstring.
    versions = d.get("versions") or []
    if not versions:
        no_provenance += 1
        continue
    if "template:" in versions[0] and d["name"].startswith("_Z") and filt in d["name"]:
        rows.append(d)

if no_provenance:
    print(f"note: {no_provenance} of {total} ledger row(s) carry no version provenance "
          f"(written by rebuild_ledger.py, which cannot know it); skipped.")

if not rows:
    where = f" matching {filt!r}" if filt else ""
    if no_provenance == total and total:
        print(f"\nnothing to verify: the whole ledger was rebuilt from committed src/, so no "
              f"row records the version it was matched at.\nRe-bank the functions you want "
              f"tracked, or check one directly with tools/match.py.")
        sys.exit(2)
    print(f"\nnothing to verify: no mangled template row{where} in the ledger.")
    sys.exit(2)

rows.sort(key=lambda d: d["addr"])
npass = nfail = 0
fails = []
for d in rows:
    name = d["name"]
    cfile = SP.path_for(name)
    if cfile is None:
        nfail += 1
        fails.append(name)
        print(f"FAIL {name}: no source file")
        continue
    cmd = [
        sys.executable, str(REPO / "tools" / "match.py"),
        "--c", str(cfile), "--func", name,
        "--addr", d["addr"], "--size", hex(d["size"]),
        "--version", VERSION, "--brief",
    ]
    binp, base = overlay_bin_and_base(d.get("module", "arm9"))
    if binp is not None:
        cmd += ["--bin", str(binp), "--base", hex(base)]
    r = subprocess.run(cmd, capture_output=True, text=True)
    ok = "MATCHING VERSIONS: " + VERSION in r.stdout
    if ok:
        npass += 1
    else:
        nfail += 1
        fails.append(name)
        print(f"FAIL {name}")
        print("    " + (r.stdout.strip().replace("\n", "\n    ")[-800:]))

print(f"\n{npass} pass, {nfail} fail (of {len(rows)})")
if fails:
    print("FAILED:", ", ".join(fails))
    sys.exit(1)
