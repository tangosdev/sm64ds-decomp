"""Self-verify helper for the coddog fan-out agents.
Compiles a candidate and relocation-aware byte-compares it to the ROM.

  python tools/abverify.py --name NAME --src cand.src [--wl progress/wl_ab.jsonl]

Final line is one of: MATCH | NOMATCH divergences=N/words | NOMATCH size: ... |
COMPILE_FAIL | NO_SUCH_FUNC. On NOMATCH with equal sizes, the mismatching
instructions (target vs candidate, reloc slots wildcarded) are printed first,
capped, so the caller can converge instead of guessing.
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
code, relocs = M.extract_func(obj, a.name)
if code is None:
    print("COMPILE_FAIL"); sys.exit(1)

target = bytes.fromhex(hexstr)
if len(code) != len(target):
    # sizes differ (missing/extra instruction) - show an ALIGNED instruction diff so
    # the caller can see WHAT is missing/extra, not just the byte counts. Divergence
    # count here uses the same aligned semantics as nearmiss_db.evaluate.
    import difflib
    import nearmiss_db as NM
    c = NM._disasm(code, relocs)
    t = NM._disasm(target, relocs)
    print(f"size differs: compiled {len(code)} bytes vs target {len(target)} - "
          f"aligned diff ('-' = absent on that side):")
    sm = difflib.SequenceMatcher(a=c, b=t, autojunk=False)
    div = shown = 0
    for op, i1, i2, j1, j2 in sm.get_opcodes():
        if op == "equal":
            continue
        n = max(i2 - i1, j2 - j1)
        div += n
        for k in range(n):
            if shown >= 24:
                continue
            cs = c[i1 + k] if i1 + k < i2 else "-"
            ts = t[j1 + k] if j1 + k < j2 else "-"
            print(f"  insn {j1 + k:3} | target: {ts:28} | yours: {cs}")
            shown += 1
    if div > shown:
        print(f"  ... {div - shown} more diverging instructions")
    print(f"NOMATCH divergences={div}/{len(target)//4} (aligned; sizes differ)")
    sys.exit(1)

MAXLINES = 32
shown = ndiff = 0
for i in range(0, len(target), 4):
    if i in relocs:
        continue
    tw, cw = target[i:i + 4], code[i:i + 4]
    if tw == cw:
        continue
    ndiff += 1
    if shown < MAXLINES:
        ti = next(M.md.disasm(tw, 0), None)
        ci = next(M.md.disasm(cw, 0), None)
        ts = f"{ti.mnemonic} {ti.op_str}" if ti else tw.hex()
        cs = f"{ci.mnemonic} {ci.op_str}" if ci else cw.hex()
        print(f"  +0x{i:03x} | target: {ts:28} | yours: {cs}")
        shown += 1
if ndiff > shown:
    print(f"  ... {ndiff - shown} more mismatching words")
print(f"NOMATCH divergences={ndiff}/{len(target)//4}"); sys.exit(1)
