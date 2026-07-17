import sys, pathlib, tempfile
REPO = pathlib.Path(".").resolve()
sys.path.insert(0, str(REPO / "tools"))
import swarm as S
M = S.M
src = pathlib.Path("_abvar/scratch920ea914/mini3.c").read_text(encoding="utf-8")
with tempfile.TemporaryDirectory() as td:
    cfile = pathlib.Path(td) / "cand.c"
    cfile.write_text(src)
    obj = M.compile_c(cfile, M.CANONICAL, M.DEFAULT_FLAGS)
code, relocs = M.extract_func(obj, "test_fn")
print("len", len(code))
for i in range(0, len(code), 4):
    w = code[i:i+4]
    ins = next(M.md.disasm(w, 0), None)
    s = f"{ins.mnemonic} {ins.op_str}" if ins else w.hex()
    print(f"{i:#05x} {s}")
