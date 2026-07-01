"""Self-verify helper for the coddog fan-out agents.
Compiles a candidate and relocation-aware byte-compares it to the ROM.

  python tools/abverify.py --name NAME --src cand.src [--wl progress/wl_ab.jsonl]

Prints exactly one of: MATCH | NOMATCH (compiled ...) | COMPILE_FAIL | NO_SUCH_FUNC
Exit 0 on MATCH, 1 on any non-match, 2 if the name is not in the worklist.
Target bytes come from the worklist row's target_hex, so no ROM path is needed.
"""
import argparse, json, pathlib, sys, tempfile
REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import swarm as S
M = S.M

ap = argparse.ArgumentParser()
ap.add_argument("--name", required=True)
ap.add_argument("--src", required=True)
ap.add_argument("--wl", default=str(REPO / "progress" / "wl_ab.jsonl"))
a = ap.parse_args()

hexstr = None
for l in open(a.wl, encoding="utf-8"):
    if l.strip() and json.loads(l)["name"] == a.name:
        hexstr = json.loads(l)["target_hex"]; break
if hexstr is None:
    print("NO_SUCH_FUNC"); sys.exit(2)

src = pathlib.Path(a.src).read_text(encoding="utf-8")
if S.oracle_ok(src, a.name, bytes.fromhex(hexstr)):
    print("MATCH"); sys.exit(0)

cpp = src.startswith("//cpp")
with tempfile.TemporaryDirectory() as td:
    cfile = pathlib.Path(td) / ("cand.cpp" if cpp else "cand.c")
    cfile.write_text(src)
    obj = M.compile_c(cfile, M.CANONICAL, S.CPP_FLAGS if cpp else M.DEFAULT_FLAGS)
if obj is None:
    print("COMPILE_FAIL"); sys.exit(1)
code, _ = M.extract_func(obj, a.name)
if code is None:
    print("COMPILE_FAIL"); sys.exit(1)
print(f"NOMATCH (compiled {len(code)} bytes vs target {len(hexstr)//2})"); sys.exit(1)
