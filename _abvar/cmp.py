import sys, pathlib, tempfile, json
sys.path.insert(0, "tools")
import swarm as S
M = S.M

wl = "progress/wl_ab.jsonl"
name = "func_0206c51c"
hexstr = None
for l in open(wl, encoding="utf-8"):
    if l.strip() and json.loads(l)["name"] == name:
        hexstr = json.loads(l)["target_hex"]; break
target = bytes.fromhex(hexstr)

src = pathlib.Path("_abwork/func_0206c51c.src").read_text()
cpp = src.startswith("//cpp")
with tempfile.TemporaryDirectory() as td:
    cfile = pathlib.Path(td) / ("cand.cpp" if cpp else "cand.c")
    cfile.write_text(src)
    obj = M.compile_c(cfile, M.CANONICAL, S.CPP_FLAGS if cpp else M.DEFAULT_FLAGS)
if obj is None:
    print("COMPILE_FAIL"); sys.exit(1)
code, relocs = M.extract_func(obj, name)
if code is None:
    print("COMPILE_FAIL"); sys.exit(1)

print("len", len(code), "target", len(target))
if len(code) != len(target):
    import difflib
    import nearmiss_db as NM
    c = NM._disasm(code, relocs)
    t = NM._disasm(target, relocs)
    sm = difflib.SequenceMatcher(a=c, b=t, autojunk=False)
    div = 0
    for op, i1, i2, j1, j2 in sm.get_opcodes():
        if op == "equal":
            continue
        n = max(i2 - i1, j2 - j1)
        div += n
        for k in range(n):
            cs = c[i1 + k] if i1 + k < i2 else "-"
            ts = t[j1 + k] if j1 + k < j2 else "-"
            print(f"  insn {j1 + k:3} | target: {ts:28} | yours: {cs}")
    print(f"NOMATCH divergences={div}/{len(target)//4} (aligned; sizes differ)")
    sys.exit(1)

ndiff = 0
for i in range(0, len(target), 4):
    if i in relocs:
        continue
    tw, cw = target[i:i+4], code[i:i+4]
    if tw == cw:
        continue
    ndiff += 1
    ti = next(M.md.disasm(tw, 0), None)
    ci = next(M.md.disasm(cw, 0), None)
    ts = f"{ti.mnemonic} {ti.op_str}" if ti else tw.hex()
    cs = f"{ci.mnemonic} {ci.op_str}" if ci else cw.hex()
    print(f"  +0x{i:03x} | target: {ts:28} | yours: {cs}")
if ndiff == 0:
    print("MATCH")
else:
    print(f"NOMATCH divergences={ndiff}/{len(target)//4}")
