"""Print one worklist row's full fan-out context: annotated disasm (bl/blx resolved
to callee, pc-loads resolved to pool sym/const), callee signatures, pool slots, and
the closest already-matched sibling source(s) that coddog attached as scaffolds.

  python tools/abrow.py --name NAME [--wl progress/wl_ab.jsonl]
"""
import argparse, json, pathlib
REPO = pathlib.Path(__file__).resolve().parent.parent
ap = argparse.ArgumentParser()
ap.add_argument("--name", required=True)
ap.add_argument("--wl", default=str(REPO / "progress" / "wl_ab.jsonl"))
a = ap.parse_args()

row = None
for l in open(a.wl, encoding="utf-8"):
    if l.strip() and json.loads(l)["name"] == a.name:
        row = json.loads(l); break
if row is None:
    print("NO_SUCH_FUNC"); raise SystemExit(2)

cpp = row["name"].startswith("_Z")
print(f"function: {row['name']}   module: {row['module']}   addr: {row['addr']}   size: {row['size']}")
print(f"language: {'C++ (start source with //cpp)' if cpp else 'C'}")
if row.get("self"):
    print(f"signature: {row['self']}")
if row.get("coddog_sim") is not None:
    sibs = ", ".join(f"{s['name']}({s['sim']})" for s in row.get("siblings", []))
    print(f"closest matched siblings (opcode similarity): {sibs}")
print("\nannotated disassembly:")
for ln in row["disasm"]:
    print(ln)
if row.get("callees"):
    print("\ncallee signatures:")
    for c in row["callees"]:
        print(f"  {c}: {row['signatures'].get(c, '(unknown sig)')}")
if row.get("pool"):
    print("\npool slots:")
    for p in row["pool"]:
        print(f"  {p}")
for ex in row.get("examples", []):
    print(f"\nmatched sibling scaffold {ex['name']}:")
    for ln in ex["c_source"].splitlines():
        print(f"    {ln}")
