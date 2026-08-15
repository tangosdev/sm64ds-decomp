#!/usr/bin/env python3
"""Overnight loop: crunch permuter on batch16 floors + log results."""
import json
import pathlib
import subprocess
import sys
import time

REPO = pathlib.Path(__file__).resolve().parent.parent
LOG = REPO / "progress" / "batch16_overnight.jsonl"
FLOORS = [
    "func_ov007_020bc3dc",
    "func_0206e3dc",
    "func_ov002_020bf36c",
    "_ZN5Model9DoSetFileEPcii",
]


def batch_status():
    out = []
    entries = [
        ("func_ov007_020bc3dc", "src/func_ov007_020bc3dc.c", "0x020bc3dc", "0x58",
         "extracted/dsd/arm9_overlays/ov007.bin", "0x020ad660"),
        ("func_0206e3dc", "src/func_0206e3dc.c", "0x0206e3dc", "0x74", None, None),
        ("func_ov002_020bf36c", "src/func_ov002_020bf36c.cpp", "0x020bf36c", "0xa0",
         "extracted/dsd/arm9_overlays/ov002.bin", "0x020ad660"),
        ("_ZN5Model9DoSetFileEPcii", "src/_ZN5Model9DoSetFileEPcii.cpp", "0x02016bf8", "0xa0", None, None),
    ]
    for name, src, addr, size, bp, bb in entries:
        mcmd = [sys.executable, str(REPO / "tools/match.py"), "--c", str(REPO / src),
                "--func", name, "--addr", addr, "--size", size, "--version", "1.2/sp2p3", "--brief"]
        if bp:
            mcmd += ["--bin", str(REPO / bp), "--base", bb]
        r = subprocess.run(mcmd, capture_output=True, text=True, cwd=str(REPO))
        line = r.stdout
        if "MATCHING VERSIONS: 1.2/sp2p3" in line:
            st = "MATCH"
        else:
            st = "?"
            for ln in line.splitlines():
                if "sp2p3:" in ln:
                    st = ln.split("sp2p3:")[-1].strip()
        out.append((name, st))
    return out


def run_crunch(secs, category=None):
    cmd = [sys.executable, str(REPO / "tools/permuter/crunch.py"),
           "--max-div", "2", "--secs", str(secs), "--limit", "4", "-j", "4"]
    if category:
        cmd += ["--category", category]
    return subprocess.run(cmd, capture_output=True, text=True, cwd=str(REPO), timeout=secs * 5 + 120)


def main():
    REPO.mkdir(exist_ok=True)
    (REPO / "progress").mkdir(exist_ok=True)
    rounds = int(sys.argv[1]) if len(sys.argv) > 1 else 3
    secs = int(sys.argv[2]) if len(sys.argv) > 2 else 600
    for rnd in range(rounds):
        ts = time.strftime("%Y-%m-%dT%H:%M:%S")
        status = dict(batch_status())
        rec = {"round": rnd, "ts": ts, "status": status, "crunch": []}
        print(f"\n=== round {rnd+1}/{rounds} @ {ts} ===")
        for name, st in status.items():
            print(f"  {name}: {st}")
        # clear permuter_tried for floors still at 2 words
        triedp = REPO / "progress" / "permuter_tried.json"
        tried = json.loads(triedp.read_text()) if triedp.exists() else {}
        for n in FLOORS:
            if status.get(n, "").endswith("2 word(s) differ"):
                tried.pop(n, None)
        triedp.write_text(json.dumps(tried))
        for cat in [None, "register", "instruction reorder"]:
            label = cat or "all"
            print(f"  crunch category={label} secs={secs}...")
            try:
                r = run_crunch(secs, cat)
                tail = [ln for ln in r.stdout.splitlines() if "div=" in ln or "pass done" in ln]
                rec["crunch"].append({"category": label, "lines": tail})
                for ln in tail:
                    print(f"    {ln}")
            except Exception as e:
                rec["crunch"].append({"category": label, "error": str(e)})
                print(f"    ERROR: {e}")
        with LOG.open("a", encoding="utf-8") as f:
            f.write(json.dumps(rec) + "\n")
        if all(v == "MATCH" for v in status.values()):
            print("ALL MATCH - done")
            break
    print(f"\nLog: {LOG}")


if __name__ == "__main__":
    main()